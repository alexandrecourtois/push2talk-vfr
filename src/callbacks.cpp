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

#include <callbacks.h>
#include <session.h>
#include <airport.h>
#include <string>

std::string CALLBACKS::__get_visibility_TEXT() {
    int v;

    AIRPORTS::getCurrentAirport().updateWeather();
    
    SESSION::dataframe.acquire([&]() {
        v = static_cast<int>(SESSION::dataframe.infreq_visibility);
    });

    if (v < 1500)
        return std::string("inférieure à 1500 mètres");

    if (v < 5000)
        return std::string("inférieure à 5 kilomètres");

    return std::string("supérieure à 5 kilomètres");
}

std::string CALLBACKS::__get_qnh() {
    std::string str;

    AIRPORTS::getCurrentAirport().updateWeather();

    SESSION::dataframe.acquire([&]() {
        if (!SESSION::is_xp11)
            str = std::to_string(static_cast<int>(std::round(SESSION::dataframe.infreq_qnh / 100.0f)));
        else
            str = std::to_string(static_cast<int>(std::round(SESSION::dataframe.infreq_qnh)));
    });

    return str;
}

std::string CALLBACKS::__get_windspeed() {
    std::string str;

    AIRPORTS::getCurrentAirport().updateWeather();

    SESSION::dataframe.acquire([&]() {
        str = std::to_string(static_cast<int>(SESSION::dataframe.infreq_windspeed));
    });

    return str;
}

std::string CALLBACKS::__get_winddir() {
    std::string str;

    AIRPORTS::getCurrentAirport().updateWeather();

    SESSION::dataframe.acquire([&]() {
        str = std::to_string(static_cast<int>(SESSION::dataframe.infreq_winddir));
    });

    return str;
}

std::string CALLBACKS::__get_runway() {
    std::string str;

    SESSION::dataframe.acquire([&]() {
        AIRPORTS::getCurrentAirport().active_rwy = AIRPORTS::getActiveRunway(SESSION::dataframe.infreq_winddir);
    });

    return AIRPORTS::getCurrentAirport().active_rwy;
}

std::string CALLBACKS::__get_holdpoint() {
    std::string str;

    SESSION::dataframe.acquire([&]() {
        str = AIRPORTS::getCurrentAirport().holdpoints[AIRPORTS::getActiveRunway(SESSION::dataframe.infreq_winddir)];
    });

    return str;
}

std::string CALLBACKS::__get_tailnum() {
    return SESSION::aircraft.tailnum;
}

void CALLBACKS::__init() {
    __callbacks_text["QNH"]          = __get_qnh;
    __callbacks_text["WINDSPEED"]    = __get_windspeed;
    __callbacks_text["WINDDIR"]      = __get_winddir;
    __callbacks_text["RUNWAY"]       = __get_runway;
    __callbacks_text["HOLDPOINT"]    = __get_holdpoint;
    __callbacks_text["VISIBILITY"]    = __get_visibility_TEXT;
    __callbacks_text["TAILNUM"]       = __get_tailnum;
}

std::string CALLBACKS::getValue(int type, const std::string &str) {
    std::map<std::string, std::function<std::string()>>* callbacks;

    switch (type) {
    case AUDIO:
        callbacks = &__callbacks_audio;
        break;

    case TEXT:
        callbacks = &__callbacks_text;
        break;

    default:
        return std::string();
    }

    if (__callbacks_audio.empty())
        __init();

    AIRPORTS::updateCurrentAirport();

    return (*callbacks)[str]();
}

bool CALLBACKS::hasCallback(int type, const std::string& key) {
    if (__callbacks_audio.empty())
        __init();

    switch(type) {
    case AUDIO:
        return __callbacks_audio.contains(key);

    case TEXT:
        return __callbacks_text.contains(key);

    default:
        break;
    }

    return false;
}

std::map<std::string, std::function<std::string()>> CALLBACKS::__callbacks_audio;
std::map<std::string, std::function<std::string()>> CALLBACKS::__callbacks_text;