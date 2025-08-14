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

#include "piper.h"
#include <vosk_api.h>
#include <airport.h>
#include <pch.h>
#include <piper.h>

class Speaker {
public:
    struct Controller {
        std::string     call_id;
        nlohmann::json  dialog;
        std::string     readback_keywords;
        std::string     curr_node;
        std::string     prev_node;
        bool            waiting_for_readback;
        std::string     path;
        std::string     vmodel;
        VoskRecognizer* vrec;
        piper_synthesizer *synth;
        piper_synthesize_options synth_cfg;
    };

private:
    std::string             __call_id;
    std::string             __vmodel_name;
    VoskModel*              __vmodel;
    VoskRecognizer*         __vrec;
    nlohmann::json          __dialog;
    std::string             __readback_keywords;
    std::string             __currentNode;
    std::string             __previousNode;
    bool                    __waiting_for_readback;
    bool                    __quit;

    std::map<std::string, Controller> __controllers;
    Controller              __default_ctrl;

    void                    __init_vosk(const char* path);
    void                    __init_ctrl_vrec(Controller& ctrl, const AIRPORTS::Airport& airport);

    std::set<std::string>   __split_user_input(const std::string& userInput);
    bool                    __evaluate_expression(const std::string& expression, const std::set<std::string>& userWords);
    bool                    __matchKeywords(const std::string& userInput, const std::string& pattern);
    std::string             __replace_keys(const std::string& input);
    void                    __speak(const std::string& audio);
    std::string             __get_user_input();
    bool                    __get_readback(const std::string& readback_keywords);
    void                    __play_node(const nlohmann::json& node);
    bool                    __check_cmd(const std::string& input);
    bool                    __check_quit(const std::string& input);


public:
    //Speaker() = default;
    //Speaker(const std::string& callID, const std::string& modelPath, const std::string& dialogPath);
    Speaker();
    ~Speaker();

    Speaker& operator=(const Speaker& orig);

    void runDialog();
    void showKeywords();
    void setupController(const AIRPORTS::Airport& airport = AIRPORTS::Airport());
    Controller* getController(const std::string& id);
    void reset();
};