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

#include "nlohmann/json.hpp"
#include <pch.h>
#include <aircraft.h>
#include <reqrsp.h>
#include <speaker.h>

class ZMQ_Client;
class ZMQ_Server;

class SESSION {
private:
    SESSION() = default;

    static bool __process_args(int argc, char** argv);

public:
    enum class CONFIG_KEYS {
        CFG_AUDIO_INPUT,
        CFG_AUDIO_OUTPUT,
        CFG_LANG,
        CFG_PTT_INPUT_DEVICE,
        CFG_PTT_INPUT_BUTTON,
        CFG_MODELS
    };

    static zmq::context_t zmq_context;

    static const std::string AFIS;
    static const std::string SIV;

    static bool no_audio;
    static bool no_xplane;
    static bool no_joystick;
    static bool is_monitor;
    static bool verbose;
    static bool is_xp11;
    //static std::atomic<bool> server_down;

    static std::string ip_address;

    static Rsp_Dataframe dataframe;
    static Rsp_Dataframe dataframe_fake;

    static std::string ctrl_id;
    static std::string cmd_prefix;

    static Speaker agent;

    static ZMQ_Client* client;
    static ZMQ_Client* client_monitor;
    static ZMQ_Server* server_fake;
    static ZMQ_Server* server_monitor;

    static Aircraft aircraft;

    static std::map<std::string, std::pair<VoskModel*, int>> vosk_Models;
    static std::map<std::string, VoskRecognizer*> vosk_Recognizers;

    static bool show_keywords;
    static bool show_readback;
    static bool show_ldistance;
    static bool input_enabled;
    static bool sim_paused;
    static bool shutdown_requested;

    static int levenshtein_threshold;

    static nlohmann::json config;

    static void loadConfig();
    static void saveConfig();
    static std::string getConfigKey(CONFIG_KEYS key);
    static void setConfigKey(CONFIG_KEYS key, const std::string& value);
    static nlohmann::json getConfigList(CONFIG_KEYS key);
    static void coldStart(int argc, char** argv);
    static void hotStart();
    static void hardShutdown();
    static void softShutdown();
};