/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2025, Alexandre Courtois                                                        *
 *                                                                                               *
 * This file is part of Push2Talk-VFR.                                                           *
 *                                                                                               *
 * Push2Talk-VFR is free software: you can redistribute it and/or modify it under the terms of   *
 * the GNU General Public License as published by the Free Software Foundation, either version 3 *
 * of the License, or (at your option) any later version.                                        *
 *                                                                                               *
 * Push2Talk-VFR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY,    *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     *
 * See the GNU General Public License for more details.                                          *
 *                                                                                               *
 * You should have received a copy of the GNU General Public License along with Push2Talk-VFR.   *
 * If not, see <https://www.gnu.org/licenses/>.                                                  *
 *                                                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "weather.h"
#include <enums.h>
#include <msg.h>
#include <airport.h>
#include <xprint.h>
#include <tools.h>
#include <session.h>
#include <zmq_client.h>
#include <lang.h>

void AIRPORTS::Airport::updateWeather() {
    if (SESSION::is_xp11) {
        Weather w = TOOLBOX::getWeatherAtLocation(this->lat, this->lon);

        SESSION::dataframe.acquire([&]() {
            SESSION::dataframe.infreq_visibility = w.visibility;
            SESSION::dataframe.infreq_qnh = w.qnh;
            SESSION::dataframe.infreq_winddir = w.winddir;
            SESSION::dataframe.infreq_windspeed = w.windspeed;
        });
    } else {
        Req rq;
        rq.argD_1 = this->lat;
        rq.argD_2 = this->lon;
        rq.argD_3 = this->alt;
        rq.type   = Request::UPD_AIRPORT;

        SESSION::client->sendRequest(rq);
    }
}

void AIRPORTS::loadAiports(const std::string& filename) {
    X_OUTPUT::xprint(MSG_STYLE::INIT, lang(T_MSG::LOADING_AIRPORTS), filename);

    __airports = TOOLBOX::loadJSON(filename);

    X_OUTPUT::xprint(MSG_STYLE::DONE, std::to_string(__airports.size()) + " " + lang(T_MSG::AIRPORTS_FOUND));
}

bool AIRPORTS::isSupportedAirport() {
    return !(__current_airport.ICAO == UNSUPPORTED_AIRPORT);
}

void AIRPORTS::updateCurrentAirport() {
    SESSION::dataframe.acquire([&]() {
        int active_freq;

        // si com1 et com2 sont actives alors l'aérodrome retenu sera toujours celui
        // correspondant à la fréquence sélectionné sur com1 (pas de dataref trouvé pour Com1/Com2 Mic)
        if (SESSION::dataframe.com1_active || (SESSION::dataframe.com1_active && SESSION::dataframe.com2_active))
            active_freq = SESSION::dataframe.com1_freq;
        else
            active_freq = SESSION::dataframe.com2_freq;

        __current_airport.ICAO = getAirportByFrequency(active_freq);
    });

    if (__current_airport.ICAO != UNSUPPORTED_AIRPORT) {
        __current_airport.lat     = __airports[__current_airport.ICAO]["latitude"].get<float>();
        __current_airport.lon     = __airports[__current_airport.ICAO]["longitude"].get<float>();
        __current_airport.alt     = __airports[__current_airport.ICAO]["altitude"].get<float>();
        __current_airport.tower   = __airports[__current_airport.ICAO]["tower"].get<int>();
        __current_airport.atis    = __airports[__current_airport.ICAO]["atis"].get<int>();

        std::vector<std::string> r = TOOLBOX::splitString(__airports[__current_airport.ICAO]["runways"].get<std::string>(), ',');
        std::vector<std::string> h = TOOLBOX::splitString(__airports[__current_airport.ICAO]["holdpoints"].get<std::string>(), ',');
        int i = 0;

        for(std::string &str: r) {
            std::string key = str;

            if (str[str.size() - 1] > '9')
                str.pop_back();

            __current_airport.runways[key] = (std::stoi(str) * 10 + 180) % 360;
            __current_airport.holdpoints[key] = h[i++];
        }

        __current_airport.sourceFile = __airports[__current_airport.ICAO]["source"].get<std::string>();
        __current_airport.type = __airports[__current_airport.ICAO]["type"].get<std::string>();
        __current_airport.lang = __airports[__current_airport.ICAO]["lang"].get<std::string>();
        __current_airport.tts = __airports[__current_airport.ICAO]["tts"].get<std::string>();
        __current_airport.controller = __airports[__current_airport.ICAO]["controller"].get<std::string>();

        Req airport_update_rq;
        airport_update_rq.type = Request::UPD_AIRPORT;
        airport_update_rq.argD_1 = __current_airport.lat;
        airport_update_rq.argD_2 = __current_airport.lon;
        airport_update_rq.argD_3 = __current_airport.alt;
        SESSION::client->sendRequest(airport_update_rq);
    }
}

std::string AIRPORTS::getAirportByFrequency(float frequency) {
    for (auto& [icao, data] : __airports.items()) {
        if (data.contains("tower")) {
            float tower_frequency = data["tower"].get<int>();
            if (tower_frequency == frequency)
                return icao;
        }
    }

    return UNSUPPORTED_AIRPORT;  // Aucun aéroport correspondant trouvé
}

// Fonction pour calculer la piste en service
std::string AIRPORTS::getActiveRunway(int windDirection) {
    std::string bestRunway = "";
    int minDiff = 360;  // Différence initiale maximale (360 degrés)

    // Trouver la piste la plus proche de l'opposée du vent
    for (const auto& runway : __current_airport.runways) {
        // Calculer la direction idéale de la piste (opposée au vent)
        int oppositeWind = (windDirection + 180) % 360;

        // Calculer la différence entre la direction de la piste et l'opposée du vent
        int diff = std::abs(runway.second - oppositeWind);

        // Ajuster la différence pour tenir compte de la circularité (360°)
        if (diff > 180) {
            diff = 360 - diff;
        }

        // Si la différence est la plus petite, choisir cette piste
        if (diff < minDiff) {
            minDiff = diff;
            bestRunway = runway.first;  // Récupérer le nom de la piste
        }
    }

    return bestRunway;
}

AIRPORTS::Airport& AIRPORTS::getCurrentAirport() {
    return __current_airport;
}

nlohmann::json AIRPORTS::__airports;
AIRPORTS::Airport AIRPORTS::__current_airport;