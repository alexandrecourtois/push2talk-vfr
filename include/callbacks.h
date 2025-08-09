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

#pragma once

#include <pch.h>

class CALLBACKS {
private:
    static bool __is_init;

    static std::map<std::string, std::function<std::string()>> __callbacks_audio;
    static std::map<std::string, std::function<std::string()>> __callbacks_text;

    CALLBACKS() = default;

    static void __init();

    static std::string __get_visibility_AUDIO();
    static std::string __get_visibility_TEXT();
    static std::string __get_qnh();
    static std::string __get_winddir();
    static std::string __get_windspeed();
    static std::string __get_runway();
    static std::string __get_holdpoint();
    static std::string __get_tailnum();

public:
    enum {
        AUDIO,
        TEXT
    };

    static std::string getValue(int type, const std::string &str);
    static bool hasCallback(int type, const std::string& key);
};