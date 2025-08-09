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

#include "msg.h"
#include <radio.h>
#include <session.h>
#include <tools.h>
#include <xprint.h>
#include <airport.h>
#include <lang.h>
#include <event_handler.h>

int RADIO::__com1_freq;
int RADIO::__com2_freq;
int RADIO::__com1_status;
int RADIO::__com2_status;
int RADIO::__com1_power;
int RADIO::__com2_power;
int RADIO::__battery_on;

void RADIO::init() {
    EVENT_HANDLER::watch<int>(&SESSION::dataframe.com1_freq, __EVENT_com1freq);
    EVENT_HANDLER::watch<int>(&SESSION::dataframe.com2_freq, __EVENT_com2freq);
    //EVENT_HANDLER::watch<int>(&SESSION::dataframe.com1_power, __EVENT_com1power);
    //EVENT_HANDLER::watch<int>(&SESSION::dataframe.com2_power, __EVENT_com2power);
    EVENT_HANDLER::watch<int>(&SESSION::dataframe.com1_active, __EVENT_com1power);
    EVENT_HANDLER::watch<int>(&SESSION::dataframe.com2_active, __EVENT_com2power);
    EVENT_HANDLER::watch<int>(&SESSION::dataframe.battery_on[0], __EVENT_battery);
}

void RADIO::__EVENT_com1power() {
  //    std::cout << "changed!\n";
  //    SESSION::dataframe.acquire([&]() {

    if (__com1_power != isPower(RADIO::COM_RADIO::COM1)) {
        X_OUTPUT::backup();
        X_OUTPUT::xprint(MSG_STYLE::CLEAN);

        if (isPower(RADIO::COM_RADIO::COM1)) {
        X_OUTPUT::xprint(MSG_STYLE::STATUS_ON, "COM1 Radio");
        __EVENT_com1freq();
        // AIRPORTS::updateCurrentAirport();
        // SESSION::agent.setupController(AIRPORTS::getCurrentAirport());
        } else
        X_OUTPUT::xprint(MSG_STYLE::STATUS_OFF, "COM1 Radio");

        //            SESSION::agent.showKeywords();
        //            X_OUTPUT::xprint(MSG_STYLE::INVITE);

        X_OUTPUT::restore();
        __com1_power = isPower(RADIO::COM_RADIO::COM1);
    }


  //    });
}

void RADIO::__EVENT_com2power() {
//    SESSION::dataframe.acquire([&]() {
        if (__com2_power != isPower(RADIO::COM_RADIO::COM2)) {
            X_OUTPUT::backup();
            X_OUTPUT::xprint(MSG_STYLE::CLEAN);

            if (isPower(RADIO::COM_RADIO::COM2)) {
                X_OUTPUT::xprint(MSG_STYLE::STATUS_ON, "COM2 Radio");
                __EVENT_com2freq();
                //AIRPORTS::updateCurrentAirport();
                //SESSION::agent.setupController(AIRPORTS::getCurrentAirport());
            }
            else
                X_OUTPUT::xprint(MSG_STYLE::STATUS_OFF, "COM2 Radio");

            //SESSION::agent.showKeywords();
            //X_OUTPUT::xprint(MSG_STYLE::INVITE);
            X_OUTPUT::restore();

            __com2_power = isPower(RADIO::COM_RADIO::COM2);
        }
//    });
}

void RADIO::__EVENT_com1freq() {
//    SESSION::dataframe.acquire([&]() {
    if (isPower(RADIO::COM_RADIO::COM1)) {
        std::string airport = AIRPORTS::getAirportByFrequency(SESSION::dataframe.com1_freq);

        X_OUTPUT::xprint(MSG_STYLE::CLEAN);
        X_OUTPUT::xprint(MSG_STYLE::STATUS_ON, "COM1 " + lang(T_MSG::FREQ), airport);
        X_OUTPUT::xprint(MSG_STYLE::INVITE);

        AIRPORTS::updateCurrentAirport();
        SESSION::agent.setupController(AIRPORTS::getCurrentAirport());
    }
//    });
}

void RADIO::__EVENT_com2freq() {
//    SESSION::dataframe.acquire([&]() {
    if (isPower(RADIO::COM_RADIO::COM2)) {
        std::string airport = AIRPORTS::getAirportByFrequency(SESSION::dataframe.com2_freq);

        X_OUTPUT::xprint(MSG_STYLE::CLEAN);
        X_OUTPUT::xprint(MSG_STYLE::STATUS_ON, "COM2 " + lang(T_MSG::FREQ), airport);
        X_OUTPUT::xprint(MSG_STYLE::INVITE);

        AIRPORTS::updateCurrentAirport();
        SESSION::agent.setupController(AIRPORTS::getCurrentAirport());
    }
//    });
}

void RADIO::__EVENT_battery() {
    __EVENT_com1power();
    __EVENT_com2power();
}

double RADIO::__white_rabbit_obj(double factor) {
    return std::log(2.0f * factor) / std::exp(1.4f) + 0.84f;
}

double RADIO::getRange() {
    //double distance_km;
    double altitude_m;
    float visibility_m;
    double range_km;

    SESSION::dataframe.acquire([&]() {
    //distance_km = haversine(SESSION::dataframe.latitude, SESSION::dataframe.longitude, current_airport.lat, current_airport.lon);
        altitude_m = SESSION::dataframe.altitude * 0.3048;
        visibility_m = SESSION::dataframe.visibility;
    });

    //double visibility_factor = std::log(std::log10(0.09f + visibility_m / BASE_VISIBILITY) + 2.68);
    //double visibility_factor = std::log(2.0f * visibility_m / BASE_VISIBILITY) / std::exp(2.0f) + 0.9f;

    range_km = 3.57 * sqrt(altitude_m) * __white_rabbit_obj(visibility_m / BASE_VISIBILITY);

    if (range_km < VHF_MAX_RANGE) {
        return range_km;
    }

    return VHF_MAX_RANGE;
}

int RADIO::getNoiseVolume() {
    //double range_km = getRange();
    float distance_km = getDistanceToAirport() * 1000.0f;
    float freqMhz = AIRPORTS::getCurrentAirport().tower / 1000.0f;
    float txAlt = AIRPORTS::getCurrentAirport().alt + 20;
    float rxAlt;
    float precipitation;

    SESSION::dataframe.acquire([&]() {
        rxAlt = SESSION::dataframe.altitude * 0.3048;
        precipitation = SESSION::dataframe.region_rain;
    });

    float volume = MIX_MAX_VOLUME * (1 - getVHF_Quality(distance_km, freqMhz, txAlt, rxAlt, precipitation));


    //double delta = range_km - distance_km;

    //double volume = MIX_MAX_VOLUME - delta * MIX_MAX_VOLUME / range_km;
    //double volume = MIX_MAX_VOLUME * __white_rabbit_obj(1 - delta / range_km);//cos(std::numbers::pi * delta / (2.0f * range_km));

    if (volume > MIX_MAX_VOLUME)
        return MIX_MAX_VOLUME;

    return std::floor(volume);
    //return 0;
}


float RADIO::getVHF_Quality(float distance,            // distance entre l'avion et la tour en mètres
                        float freqMHz,         // fréquence VHF en MHz (ex : 125 MHz)
                        float txAlt,         // altitude de la tour (m)
                        float rxAlt,         // altitude de l'avion (m)
                        float precipitation, // pourcentage de précipitation (0 à 1)
                        float txPower,      // puissance émetteur en dBm
                        float rxSensitivity // sensibilité du récepteur en dBm (seuil minimal pour une réception acceptable)
                        )
{
    // --- 1. Pertes en espace libre (FSPL) ---
    float d_km = distance / 1000.0f;
    float FSPL = 20.0f * std::log10(d_km) + 20.0f * std::log10(freqMHz) + 32.44f;
    
    // --- 2. Calcul de la distance de ligne de vue (LOS) ---
    const float R = 6371000.0f; // rayon moyen de la Terre en mètres
    float d_h_tx = std::sqrt(2.0f * R * txAlt);
    float d_h_rx = std::sqrt(2.0f * R * rxAlt);
    float d_LOS = d_h_tx + d_h_rx;
    
    // --- 3. Pénalité si la distance dépasse la ligne de vue ---
    float losPenalty = 1.0f;
    if (distance > d_LOS) {
        float extraDistance = distance - d_LOS;
        float decayConstant = 1000.0f;
        losPenalty = std::exp(-extraDistance / decayConstant);
    }
    
    // --- 4. Facteur météo basé sur la précipitation ---
    // La qualité ne doit pas être dégradée de plus de 20% (facteur >= 0.8)
    float weatherFactor = 1.0f - (0.2f * precipitation);
    weatherFactor = std::max(0.8f, weatherFactor);
    
    // --- 5. Calcul de la puissance reçue ---
    float P_r = txPower - FSPL;
    
    // Appliquer la pénalité liée à la perte de ligne de vue
    float penalty_dB = 10.0f * std::log10(losPenalty);
    P_r += penalty_dB;
    
    // --- 6. Conversion en qualité du signal ---
    float margin = P_r - rxSensitivity;
    float Q_signal = margin / 20.0f;
    Q_signal = std::max(0.0f, std::min(1.0f, Q_signal));
    
    // La qualité finale combine la qualité du signal et le facteur météo.
    float Q = Q_signal * weatherFactor;
    
    return Q;
}

int RADIO::getSpeakVolume(int noiseVolume) {
    return MIX_MAX_VOLUME - noiseVolume;
}

bool RADIO::isWithinRange() {
    return getDistanceToAirport() < getRange();
}

double RADIO::getDistanceToAirport() {
    double result;

    SESSION::dataframe.acquire([&]() {
        result = TOOLBOX::haversine(SESSION::dataframe.latitude, SESSION::dataframe.longitude, AIRPORTS::getCurrentAirport().lat, AIRPORTS::getCurrentAirport().lon);
    });

    return result;
}

void RADIO::showFrequency(FREQ_TYPE type, COM_RADIO radio) {

}

void RADIO::showStatus(COM_RADIO radio) {

}

int RADIO::getFreq(COM_RADIO radio) {
    switch (radio) {
    case COM_RADIO::COM1:
        return __com1_freq;

    case COM_RADIO::COM2:
        return __com2_freq;
    }

    return 0;
}

bool RADIO::isActive(COM_RADIO radio) {
    switch (radio) {
    case COM_RADIO::COM1:
        return __com1_status == 1;

    case COM_RADIO::COM2:
        return __com2_status == 1;
    }

    return false;
}

bool RADIO::isPower(COM_RADIO radio) {
    bool result = false;

    SESSION::dataframe.acquire([&]() {
        switch (radio) {
            case COM_RADIO::COM1:
                result = SESSION::dataframe.com1_power && SESSION::dataframe.com1_active && SESSION::dataframe.battery_on[0];
                break;

            case COM_RADIO::COM2:
                result = SESSION::dataframe.com2_power && SESSION::dataframe.com2_active && SESSION::dataframe.battery_on[0];
                break;

            case COM_RADIO::ANY:
                result = isPower(COM_RADIO::COM1) || isPower(COM_RADIO::COM2);
                break;

            default:
                break;
        }
    });

    return result;
}