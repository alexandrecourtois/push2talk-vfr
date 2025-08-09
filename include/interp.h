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

#define CMD_HELP                "cmd_help"
#define CMD_RELOAD_JSON         "cmd_reload_json"
#define CMD_RELOAD_PHRASES      "cmd_reload_phrases"
#define CMD_REINIT_JOYSTICK     "cmd_reinit_joystick"
#define CMD_REINIT_AUDIO        "cmd_reinit_audio"
#define CMD_GETMEM              "cmd_getmem"
#define CMD_GET_RADIO           "cmd_get_radio"
#define CMD_GET_WEATHER         "cmd_get_weather"
#define CMD_GET_POSITION        "cmd_get_position"
#define CMD_GET_RADIO_RANGE     "cmd_get_radio_range"
#define CMD_GET_RADIO_QUALITY   "cmd_get_radio_quality"
#define CMD_MONITOR             "cmd_monitor"
#define CMD_SHOW_KEYWORDS       "cmd_show_keywords"
#define CMD_HIDE_KEYWORDS       "cmd_hide_keywords"
#define CMD_SHOW_READBACK       "cmd_show_readback"
#define CMD_HIDE_READBACK       "cmd_hide_readback"
#define CMD_SHOW_LDISTANCE      "cmd_show_ldistance"
#define CMD_HIDE_LDISTANCE      "cmd_hide_ldistance"

class CMD {
private:
    enum class cmd_ {
        help,
        reload_json,
        reload_phrases,
        reinit_joystick,
        reinit_audio,
        getmem,
        get_radio,
        get_weather,
        get_position,
        get_radio_range,
        get_radio_quality,
        monitor,
        show_keywords,
        hide_keywords,
        show_readback,
        hide_readback,
        show_ldistance,
        hide_ldistance
    };

    static std::map<std::string, cmd_> __cmd_list;
    static bool __isInit;

    CMD() = default;

    static void __init();

public:
    static int run(std::string_view cmd);
    static void monitor(const std::vector<std::string>& args);
};