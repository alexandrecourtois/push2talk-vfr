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

#include "tools.h"
#include "event_handler.h"
#include "en_US.h"
#include "fr_FR.h"
#include "msg.h"
#include "reqrsp.h"
#include <session.h>
#include <tools.h>
#include <xprint.h>
#include <audio.h>
#include <inputs.h>
#include <zmq_client.h>
#include <zmq_server.h>
#include <lang.h>
#include <event_handler.h>

bool SESSION::no_audio;
bool SESSION::no_xplane;
bool SESSION::no_joystick;
bool SESSION::is_monitor;
bool SESSION::verbose;
bool SESSION::is_xp11;
zmq::context_t SESSION::zmq_context = zmq::context_t(1);

Rsp_Dataframe SESSION::dataframe;
Rsp_Dataframe SESSION::dataframe_fake {
    Lockable(),
    1000,
    1000,
    "F-SERV",
    10000.0f,
    101300.0f,
    5.0f,
    120.0f,
    47.6785011f,
    1.20884f,
    130.0f,
    000000, // com1 freq
    000000, // com2 freq
    1.0f,
    1.0f,
    1,      // com1 active
    0,
    1,      // com1 power
    0,
    {0, 0, 0, 0, 0, 0, 0, 0}, // battery
    10000.0f,
    101300.0f,
    5.0f,
    120.0f,
    true,
    0.0f,
    false,
    40000.0f,
    false
};

const std::string SESSION::AFIS = " AFIS   ";
const std::string SESSION::SIV = " SIV    ";
std::string SESSION::ip_address = "127.0.0.1";
std::string SESSION::ctrl_id = SESSION::AFIS;
std::string SESSION::cmd_prefix = "cmd_";
Speaker SESSION::agent;
ZMQ_Client* SESSION::client;
ZMQ_Client* SESSION::client_monitor;
ZMQ_Server* SESSION::server_fake;
ZMQ_Server* SESSION::server_monitor;
Aircraft SESSION::aircraft;
std::map<std::string, std::pair<VoskModel*, int>> SESSION::vosk_Models;
std::map<std::string, VoskRecognizer*> SESSION::vosk_Recognizers;
bool SESSION::show_keywords = false;
bool SESSION::show_readback = false;
bool SESSION::show_ldistance = false;
int SESSION::levenshtein_threshold = 3;
nlohmann::json SESSION::config;
bool SESSION::input_enabled = true;
bool SESSION::sim_paused = false;
bool SESSION::shutdown_requested = false;

bool SESSION::__process_args(int argc, char** argv) {
    // Parcours des arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // Traitement des options longues
        if (arg == "--no-audio") {
            SESSION::no_audio = true;
            SESSION::no_joystick = true;
        } else if (arg == "--no-joystick") {
            SESSION::no_joystick = true;
        } else if (arg == "--no-xplane") {
            SESSION::no_xplane = true;
        } else if (arg == "--show-keywords") {
            SESSION::show_keywords = true;
        } else if (arg == "--xp-address") {
            if (i < argc) {
                if (TOOLBOX::isValidIP(argv[i+1])) {
                    SESSION::ip_address = argv[i+1];
                } else {
                    X_OUTPUT::xprint(MSG_STYLE::M_ERROR, "Invalid IP address. Networking disabled");
                    SESSION::no_xplane = true;
                }
            }
        } else if (arg == "--monitor") {
            SESSION::no_audio = true;
            SESSION::no_joystick = true;
            SESSION::no_xplane = true;
            SESSION::is_monitor = true;
        } else if (arg == "--verbose") {
            SESSION::verbose = true;
        }
        // Traitement des options courtes avec "-"
        else if (arg[0] == '-' && arg.length() > 1 && arg[1] != '-') {
            // Boucle sur chaque caractère après "-"
            for (size_t j = 1; j < arg.length(); ++j) {
                switch (arg[j]) {
                case 'a':
                    SESSION::no_audio = true;
                    SESSION::no_joystick = true;
                    break;
                case 'j':
                    SESSION::no_joystick = true;
                    break;
                case 'k':
                    SESSION::show_keywords = true;
                    break;
                case 'x':
                    SESSION::no_xplane = true;
                    break;
                case 'v':
                    SESSION::verbose = true;
                    break;
                default:
                    std::cerr << "Unknown argument : -" << arg[j] << std::endl;
                    return false; // Sortie avec erreur
                }
            }
        }
    }

    return true;
}

void SESSION::loadConfig() {
    //X_OUTPUT::xprint(MSG_STYLE::INIT, lang(T_MSG::LOADING_CONFIG));
    config = TOOLBOX::loadJSON("../../res/config.json");

    std::string lang = SESSION::getConfigKey(CONFIG_KEYS::CFG_LANG);

    if (lang == "fr_FR" || lang.empty())    Lang::setLang<fr_FR>();
    if (lang == "en_US")                    Lang::setLang<en_US>();
    //X_OUTPUT::xprint(MSG_STYLE::DONE);
}

void SESSION::saveConfig() {
    X_OUTPUT::xprint(MSG_STYLE::INIT, lang(T_MSG::WRITING_CONFIG));

    setConfigKey(CONFIG_KEYS::CFG_AUDIO_INPUT, AUDIO::getInputDeviceName());
    setConfigKey(CONFIG_KEYS::CFG_AUDIO_OUTPUT, AUDIO::getOutputDeviceName());
    setConfigKey(CONFIG_KEYS::CFG_LANG, Lang::getLang());
    setConfigKey(CONFIG_KEYS::CFG_PTT_INPUT_DEVICE, X_INPUT::getJoystickName());
    setConfigKey(CONFIG_KEYS::CFG_PTT_INPUT_BUTTON, X_INPUT::getJoystickPTTButton());

    if (TOOLBOX::saveJSON(config, "../../res/config.json"))
        X_OUTPUT::xprint(MSG_STYLE::DONE);
}

std::string SESSION::getConfigKey(CONFIG_KEYS key) {
    switch(key) {
        case CONFIG_KEYS::CFG_AUDIO_INPUT:
            return config["DeviceAudio_Input"];

        case CONFIG_KEYS::CFG_AUDIO_OUTPUT:
            return config["DeviceAudio_Output"];

        case CONFIG_KEYS::CFG_LANG:
            return config["Lang"];

        case CONFIG_KEYS::CFG_PTT_INPUT_DEVICE:
            return config["PTT_Device"];

        case CONFIG_KEYS::CFG_PTT_INPUT_BUTTON:
            return config["PTT_Button"];

        default:
            return std::string();
    }
}

nlohmann::json SESSION::getConfigList(CONFIG_KEYS key) {
    switch(key) {
        case CONFIG_KEYS::CFG_MODELS:
            return config["Models"];

        default:
            return nlohmann::json();
    }
}

void SESSION::setConfigKey(CONFIG_KEYS key, const std::string &value) {
    switch (key) {
        case CONFIG_KEYS::CFG_AUDIO_INPUT:
            config["DeviceAudio_Input"] = value;
            break;

        case CONFIG_KEYS::CFG_AUDIO_OUTPUT:
            config["DeviceAudio_Output"] = value;
            break;

        case CONFIG_KEYS::CFG_LANG:
            config["Lang"] = value;
            break;

        default:
            break;
    }
}

void connect_xp() {
    X_OUTPUT::xprint(MSG_STYLE::INIT,lang(T_MSG::CONNECTING_TO_XPLANE));

    Req rq;
    rq.type = Request::GET_DATAFRAME;
    SESSION::client->sendRequest(rq, false, 0);

    X_OUTPUT::xprint(MSG_STYLE::DONE, "X-Plane " + std::to_string(SESSION::dataframe.xp_major / 1000) + "." + std::to_string((SESSION::dataframe.xp_major / 10) % ((SESSION::dataframe.xp_major / 1000) * 100)));

    SESSION::is_xp11 = (SESSION::dataframe.xp_major / 1000 == 11) ? true : false;

    if (SESSION::is_xp11) {
        X_OUTPUT::xprint(MSG_STYLE::M_ERROR, lang(T_MSG::XP11_WARNING));
    }
}

void waitfor_aircraft() {
    X_OUTPUT::xprint(MSG_STYLE::INIT, lang(T_MSG::WAITING_FOR_AIRCRAFT));

    Req rq;
    rq.type = Request::GET_DATAFRAME;

    while(!strlen(SESSION::dataframe.tailnum)) {
        SESSION::client->sendRequest(rq);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    SESSION::aircraft.tailnum = SESSION::dataframe.tailnum;
    X_OUTPUT::xprint(MSG_STYLE::DONE, SESSION::aircraft.tailnum);
}

void SESSION::hardShutdown() {
    X_OUTPUT::xprint(MSG_STYLE::M_ERROR, lang(T_MSG::CONNECTION_LOST));
    SESSION::shutdown_requested = true;    
    std::cout << std::endl;
}

void SESSION::softShutdown() {
    delete SESSION::client;

    if (SESSION::no_xplane)
        delete SESSION::server_fake;

    if (SESSION::is_monitor)
        delete SESSION::client_monitor;
    else
        delete SESSION::server_monitor;

    if (!SESSION::no_audio)
        AUDIO::free();

    SDL_Quit();
    X_INPUT::enableInput();
    SESSION::saveConfig();
}

void SESSION::coldStart(int argc, char** argv) {
    SESSION::loadConfig();
    
    if (!SESSION::__process_args(argc, argv))
        exit(1);

    if (SESSION::no_xplane)
        X_OUTPUT::xprint(MSG_STYLE::WARNING, lang(T_MSG::FAKE_SERVER_ENABLED));

    if (!SESSION::no_audio) {
        //AUDIO::selectDevice();
        AUDIO::init("audio/VOIX_AFIS/", "phrases.cfg");
    } else
        X_OUTPUT::xprint(MSG_STYLE::WARNING, lang(T_MSG::DISABLE), "AUDIO");

    if (!SESSION::is_monitor) {
        AIRPORTS::loadAiports("airports.json");
//        SESSION::agent = Speaker(SESSION::ctrl_id, "vosk-model-fr-0.22", "responses_afis_any.json");
    }

    if (!SESSION::no_joystick)
        X_INPUT::selectJoystick();
    else
        X_OUTPUT::xprint(MSG_STYLE::WARNING, lang(T_MSG::DISABLE), "JOYSTICK");

    if (SESSION::no_xplane) {
        SESSION::server_fake = new ZMQ_Server(SESSION::zmq_context, INPROC_Server("fake_server"), SESSION::dataframe_fake);
        SESSION::server_fake->start();
    }

    //init_netw();
//    if (!SESSION::no_xplane)
//        SESSION::client = new ZMQ_Client(SESSION::zmq_context, TCP_Server(SESSION::ip_address + ":5555"));
//    else
//        SESSION::client = new ZMQ_Client(SESSION::zmq_context, INPROC_Server("fake_server"));

    hotStart();

    SESSION::dataframe.acquire([&]() {
        EVENT_HANDLER::watch<bool>(&SESSION::dataframe.server_down, hardShutdown, EVENT_HANDLER::Event::E_EQUAL, true);
    });
}

void SESSION::hotStart() {
    //SESSION::agent.reset();
    if (!SESSION::no_xplane)
        SESSION::client = new ZMQ_Client(SESSION::zmq_context, TCP_Server(SESSION::ip_address + ":5555"));
    else
        SESSION::client = new ZMQ_Client(SESSION::zmq_context, INPROC_Server("fake_server"));


    //if (!SESSION::is_monitor)
    //    SESSION::agent = Speaker(SESSION::ctrl_id, "vosk-model-small-fr-0.22", "responses_afis_any.json");

    connect_xp();
    waitfor_aircraft();

    SESSION::client->start();

    if (SESSION::is_monitor) {
        SESSION::client->stop();
        SESSION::ctrl_id = "MONITOR";
        SESSION::client_monitor = new ZMQ_Client(SESSION::zmq_context, TCP_Server("127.0.0.1:5556"));
        SESSION::client_monitor->start();
    } else {
        SESSION::server_monitor = new ZMQ_Server(SESSION::zmq_context, TCP_Server("127.0.0.1:5556"));
        SESSION::server_monitor->start();
    }
}