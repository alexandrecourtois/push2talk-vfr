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

#include "vosk_api.h"
#include <msg.h>
#include <speaker.h>
#include <string>
#include <xprint.h>
#include <callbacks.h>
#include <session.h>
#include <audio.h>
#include <inputs.h>
#include <airport.h>
#include <radio.h>
#include <tools.h>
#include <lang.h>
#include <interp.h>
#include <updater.h>

void Speaker::__init_vosk(const char* path) {
    X_OUTPUT::xprint(MSG_STYLE::INIT, lang(T_MSG::INITIALIZING_VOSK), path);

    if (SESSION::vosk_Models.contains(this->__vmodel_name)) {
        this->__vmodel = SESSION::vosk_Models[this->__vmodel_name].first;
        this->__vrec = SESSION::vosk_Recognizers[this->__vmodel_name];
        SESSION::vosk_Models[this->__vmodel_name].second++;
    } else {
        vosk_set_log_level(-1);
        SESSION::vosk_Models[this->__vmodel_name].first = vosk_model_new(path);
        SESSION::vosk_Recognizers[this->__vmodel_name] = vosk_recognizer_new(SESSION::vosk_Models[__vmodel_name].first, 16000.0f);
        SESSION::vosk_Models[this->__vmodel_name].second = 1;
        this->__vmodel = SESSION::vosk_Models[this->__vmodel_name].first;
        this->__vrec = SESSION::vosk_Recognizers[this->__vmodel_name];
    }

    X_OUTPUT::xprint(MSG_STYLE::DONE);
}

void Speaker::__init_ctrl_vrec(Controller& ctrl, const AIRPORTS::Airport& airport) {
    std::string model_name = SESSION::getConfigList(SESSION::CONFIG_KEYS::CFG_MODELS)[airport.lang];
    
    if (SESSION::vosk_Models.contains(model_name))
        SESSION::vosk_Models[model_name].second++;
    else {
        vosk_set_log_level(-1);
        SESSION::vosk_Models[model_name].first = vosk_model_new(model_name.c_str());
        SESSION::vosk_Recognizers[model_name] = vosk_recognizer_new(SESSION::vosk_Models[model_name].first, 16000.0f);
        SESSION::vosk_Models[model_name].second = 1;
    }

    ctrl.vmodel = model_name;
    ctrl.vrec = SESSION::vosk_Recognizers[model_name];
}

std::string Speaker::__replace_keys(const std::string& input) {
    std::string result = input;
    std::regex pattern(R"(\{([A-Z]+)\})"); // Cherche des mots majuscules entre {}

    auto it = std::sregex_iterator(result.begin(), result.end(), pattern);
    auto end = std::sregex_iterator();

    while (it != end) {
        std::smatch match = *it;
        std::string key = match.str(1); // Récupérer la clé sans les {}
        if (CALLBACKS::hasCallback(CALLBACKS::TEXT, key)) {
            std::string value = std::string(RESET) + std::string(WHITE_BOLD) + CALLBACKS::getValue(CALLBACKS::TEXT, key) + std::string(RESET) + std::string(GREEN_BOLD); // Appeler la fonction associée
            result.replace(match.position(), match.length(), value);
            // Recalculer l'itérateur car la chaîne a changé
            it = std::sregex_iterator(result.begin(), result.end(), pattern);
        } else {
            ++it; // Passer à la prochaine correspondance si clé inconnue
        }
    }

    return result;
}

bool Speaker::__matchKeywords(const std::string& userInput, const std::string& pattern) {
    if (pattern.empty())
        return false;
    
    std::string lowerInput = TOOLBOX::toLower(userInput);

    // Séparation du pattern sur le caractère '&'
    auto parts = TOOLBOX::split(pattern, '&');
    for (auto& part : parts) {
        part = TOOLBOX::trim(part);
        // Si la partie est entourée de parenthèses, on les retire.
        if(!part.empty() && part.front() == '(' && part.back() == ')') {
            part = part.substr(1, part.size() - 2);
        }
        // Pour chaque partie, on sépare les alternatives avec '|'
        auto alternatives = TOOLBOX::split(part, '|');
        bool partMatched = false;
        for (auto& alt : alternatives) {
            alt = TOOLBOX::trim(alt);
            // Recherche case-insensitive d'une occurrence de l'alternative dans l'entrée.
            if(lowerInput.find(TOOLBOX::toLower(alt)) != std::string::npos) {
                partMatched = true;
                break;
            } else {
                auto userWords = TOOLBOX::split(lowerInput, ' ');

                for(auto& word: userWords) {
                    int ld = TOOLBOX::getLevenshteinDistance(alt, word);
                    partMatched = partMatched || (ld >= 0 && ld <= SESSION::levenshtein_threshold);
                }

                if (partMatched)
                    break;
            }
        }
        // Si une partie n'est pas trouvée, le pattern ne correspond pas.
        if (!partMatched) {
            return false;
        }
    }

    return true;
}

std::set<std::string> Speaker::__split_user_input(const std::string& userInput) {
    std::set<std::string> words;
    std::istringstream stream(userInput);
    std::string word;

    while (stream >> word) {
        words.insert(word);
    }

    return words;
}

//Speaker::Speaker(const std::string& callID, const std::string& modelPath, const std::string& dialogPath): __call_id(callID), __vmodel_name(modelPath) {
    //if (!SESSION::no_audio) {
    //    this->__init_vosk(modelPath.c_str());
    //}

Speaker::Speaker() {
    std::string json_str = R"({">ROOT":{"targets":[{"keywords":"","goto":">ROOT"}]},">MISCOPY":{"phrase":")" + lang(T_MSG::NO_CTRL_DEFINED) + std::string(R"("}})");
    __default_ctrl.call_id      = " D-CTRL ";
    __default_ctrl.dialog       = nlohmann::json::parse(json_str);
    __default_ctrl.curr_node    = ">ROOT";
    SESSION::ctrl_id            = __default_ctrl.call_id;
}

Speaker::~Speaker() {
    if (!SESSION::no_audio) {
        for(const auto &pair: this->__controllers) {
            if (pair.second.vrec)
                if (SESSION::vosk_Models[pair.second.vmodel].second == 1) {
                    vosk_recognizer_free(pair.second.vrec);
                    vosk_model_free(SESSION::vosk_Models[pair.second.vmodel].first);
                    SESSION::vosk_Models.erase(pair.second.vmodel);
                    SESSION::vosk_Recognizers.erase(pair.second.vmodel);
                } else {
                    SESSION::vosk_Models[pair.second.vmodel].second--;
            }
        }
    }
}

Speaker& Speaker::operator=(const Speaker& orig) {
    this->__call_id = orig.__call_id;
    this->__vmodel_name = orig.__vmodel_name;
    this->__vmodel = orig.__vmodel;
    this->__vrec = orig.__vrec;
    this->__dialog = orig.__dialog;
    this->__currentNode = orig.__currentNode;
    SESSION::vosk_Models[this->__vmodel_name].second++;
    this->__controllers = orig.__controllers;
    this->__default_ctrl = orig.__default_ctrl;

    return *this;
}

void Speaker::__speak(const std::string& audio) {
    X_OUTPUT::xprint(MSG_STYLE::BLINK_BEGIN, "Speaking");

    std::vector<std::string> keys = TOOLBOX::splitString(audio, ',');

    AUDIO::play(AUDIO::Type::RADIOCOM, keys);

    X_OUTPUT::xprint(MSG_STYLE::ENDL);
    X_OUTPUT::xprint(MSG_STYLE::BLINK_END);
}

Speaker::Controller* Speaker::getController(const std::string &id) {
    if (__controllers.contains(id))
        return &__controllers[id];
    else
        return &__default_ctrl;
}

//void Speaker::tell(std::string& input) {
void Speaker::runDialog() {
    std::string input;
    nlohmann::json node;

    if (!SESSION::is_monitor) {
        //RADIO::updateStatus();
        
        UPDATER::update();
        //Controller* ctrl = &this->__controllers[AIRPORTS::getCurrentAirport().ICAO];
        Controller* ctrl = this->getController(AIRPORTS::getCurrentAirport().ICAO);

        //this->__dialog_running = true;

        while(!SESSION::shutdown_requested/*true*//*this->__dialog_running*/) {
            //if (ctrl->dialog.find(ctrl->curr_node) == ctrl->dialog.end()) {
            //    std::cerr << "Noeud '" << ctrl->curr_node << "' non trouvé dans le JSON." << std::endl;
            //    return;
            //}

            //auto node = ctrl->dialog[ctrl->curr_node];
            node = ctrl->dialog[ctrl->curr_node];
            //std::cout << ctrl->dialog.dump(4) << std::endl;
            //std::cout << node["targets"] << std::endl;

            this->__play_node(node);

            if (node.contains("readback")) {
                //if (SESSION::show_keywords)
                //    X_OUTPUT::xprint(MSG_STYLE::KEYWORDS,std::string(node["readback"]));

                this->__waiting_for_readback = true;
                while(!this->__get_readback(node["readback"]));

                if (this->__quit)
                    return; 

                this->__waiting_for_readback = false;
            }

            // S'il n'y a pas de cibles, on considère la conversation comme terminée.
            if (!node.contains("targets") || node["targets"].empty()) {
                X_OUTPUT::xprint(MSG_STYLE::M_ERROR, "Terminal Node found (" + ctrl->curr_node + ")");
                return;
            }

            bool foundMatch = false;

            while (!foundMatch && !SESSION::shutdown_requested) {
                input = this->__get_user_input();

                if (this->__check_quit(input))
                    return;

                //ctrl = &this->__controllers[AIRPORTS::getCurrentAirport().ICAO];
                //std::cout << ctrl->dialog.dump(4) << std::endl;
                ctrl = this->getController(AIRPORTS::getCurrentAirport().ICAO);
                //std::cout << ctrl->dialog.dump(4) << std::endl; 
                node = ctrl->dialog[ctrl->curr_node];

                if (!__check_cmd(input)) {
                    bool simPaused;

                    SESSION::dataframe.acquire([&]() {
                        simPaused = SESSION::dataframe.sim_paused;
                    });

                    if (!simPaused && !input.empty() && RADIO::isPower(RADIO::COM_RADIO::ANY)) {
                        if (AIRPORTS::isSupportedAirport() /*&& RADIO::isWithinRange()*/) {
                            //if (RADIO::isWithinRange()) {
                            // Parcours de toutes les cibles du noeud
                            for (const auto &target : node["targets"]) {
                                if (!target.contains("keywords") || !target.contains("goto"))
                                    continue;

                                std::string keywords = target["keywords"];

                                // Si l'entrée correspond aux mots clés du target
                                if (this->__matchKeywords(input, keywords)) {
                                    // Passage au noeud suivant indiqué par "goto"
                                    ctrl->curr_node = target["goto"];


                                    if (size_t spos = ctrl->curr_node.find("$PATH"); spos != std::string::npos) {
                                        std::cout << "redirect to " << ctrl->path << std::endl;
                                        ctrl->curr_node.replace(spos, 5, ctrl->path);
                                    }

                                    foundMatch = true;

                                    if (target.contains("path"))
                                        ctrl->path = target["path"];

                                    break;
                                }
                            }

                            // Si aucun mot clé ne correspond, on sort de la boucle
                            if (!foundMatch) {
                                //std::cout << ctrl->dialog.dump(4) << std::endl;
                                X_OUTPUT::xprint(MSG_STYLE::XTALK, ctrl->dialog[">MISCOPY"]["phrase"]);

                                if (!SESSION::no_audio)
                                    this->__speak(ctrl->dialog[">MISCOPY"]["audio"]);
                            }
                            //}
                            //else {
                            //    X_OUTPUT::xprint(MSG_STYLE::M_ERROR, AIRPORTS::getCurrentAirport().ICAO + " out of range");
                            //}
                        }
                    } else {
                        std::cout << BACK_LINE << std::flush;
                    }
                } else {
                    CMD::run(input);
                }
            }
        }
    } else {
        while(true) {
            X_INPUT::xscan(input);

            if (!__check_quit(input))
                CMD::run(input);
            else
                return;
        }
    }
}

std::string Speaker::__get_user_input() {
    std::string input;

    if (SESSION::show_keywords && RADIO::isPower(RADIO::COM_RADIO::ANY))
        this->showKeywords();

    SESSION::input_enabled = true;
    X_OUTPUT::xprint(MSG_STYLE::INVITE);

    bool was_xscan_reset;

    if (!SESSION::is_monitor)
        was_xscan_reset = X_INPUT::xscan(input, UPDATER::update);
    else
        was_xscan_reset = X_INPUT::xscan(input);

    if (!was_xscan_reset) {
        if (!SESSION::is_monitor) {
            if (input.empty() && !SESSION::no_audio && RADIO::isPower()) {
                AUDIO::play(AUDIO::Type::RADIOSTART);
                AUDIO::startRecording();

                if (!X_INPUT::ptt_pushed()) {
                    std::cout << BACK_LINE << "> " << std::flush;
                } else
                    X_INPUT::disableInput();

                X_OUTPUT::xprint(MSG_STYLE::BLINK_BEGIN, lang(T_MSG::RECORDING));

                if (X_INPUT::ptt_pushed()) {
                    while (X_INPUT::ptt_pushed());
                    X_INPUT::enableInput();
                    std::cout << std::endl;
                } else
                    X_INPUT::xscan();

                X_OUTPUT::xprint(MSG_STYLE::BLINK_END);
                X_OUTPUT::xprint(MSG_STYLE::INVITE);

                AUDIO::stopRecording();
                AUDIO::play(AUDIO::Type::RADIOSTOP);

                std::cout << lang(T_MSG::ANALYZING) << "... " << std::flush;

                Controller* ctrl = getController(AIRPORTS::getCurrentAirport().ICAO);

                vosk_recognizer_accept_waveform(
                    ctrl->vrec,
                    reinterpret_cast<const char *>(AUDIO::getAudioStream().data()),
                    AUDIO::getAudioStream().size());

                const char *vres = vosk_recognizer_result(ctrl->vrec);
                std::string userInput(nlohmann::json::parse(vres)["text"]);

                std::cout << CLEAN_LINE << "> " << std::endl;

                input = userInput;
            }
        }

        if (!input.empty()) {
            std::cout << BACK_LINE << CLEAN_LINE;

            if (!__check_cmd(input)) {
                if (AIRPORTS::isSupportedAirport())
                    X_OUTPUT::xprint(MSG_STYLE::USER, input);
                else
                    X_OUTPUT::xprint(MSG_STYLE::USER_ALT, input);
            }
        }

        return input;
    }
    else {
        return std::string();
    }
}

bool Speaker::__get_readback(const std::string& readback_keywords) {
    std::string input = this->__get_user_input();
    Controller* ctrl = &this->__controllers[AIRPORTS::getCurrentAirport().ICAO];

    if (this->__check_quit(input))
        return true;

    if (!this->__check_cmd(input) && RADIO::isPower(RADIO::COM_RADIO::ANY)) {
        if (ctrl->dialog[this->__currentNode].contains("repeat")) {
            if (this->__matchKeywords(input, ctrl->dialog[ctrl->curr_node]["repeat"])) {
                this->__play_node(ctrl->dialog[ctrl->curr_node]);
                return false;
            }
        }

        if (this->__matchKeywords(input, readback_keywords)) {
            return true;
        } else {
            X_OUTPUT::xprint(MSG_STYLE::XTALK, ctrl->dialog[">MISCOPY"]["phrase"]);

            if (!SESSION::no_audio) {
                this->__speak(ctrl->dialog[">MISCOPY"]["audio"]);
            }
        }
    }

    return false;
}

void Speaker::showKeywords() {
    //Controller* ctrl = &this->__controllers[AIRPORTS::getCurrentAirport().ICAO];
    Controller* ctrl = this->getController(AIRPORTS::getCurrentAirport().ICAO);
    
    if (!this->__waiting_for_readback) {
        for (const auto &target : ctrl->dialog[ctrl->curr_node]["targets"]) {
            if (!target.contains("keywords") || !target.contains("goto") || std::string(target["keywords"]).empty()) // si keywords est vide, alors sa taille est 1 ???
                continue;

                X_OUTPUT::xprint(MSG_STYLE::KEYWORDS,std::string(target["keywords"]));
            // Si l'entrée correspond aux mots clés du target
        }
    } else {
        X_OUTPUT::xprint(MSG_STYLE::KEYWORDS, std::string(ctrl->dialog[ctrl->curr_node]["readback"]));
    }
}

void Speaker::__play_node(const nlohmann::json& node) {
    if (node.contains("phrase")) {
        X_OUTPUT::xprint(MSG_STYLE::XTALK, this->__replace_keys(TOOLBOX::removeQuotes(node["phrase"])));
    }

    if (node.contains("audio") && !SESSION::no_audio) {
        this->__speak(TOOLBOX::removeQuotes(node["audio"]));
    }
}

void Speaker::setupController(const AIRPORTS::Airport &airport) {
    if (!__controllers.contains(airport.ICAO)) {
        X_OUTPUT::xprint(MSG_STYLE::INIT, lang(T_MSG::LOADING_DIALOG), airport.ICAO);

        Controller ctrl;


        ctrl.call_id = " " + airport.type;
        ctrl.call_id.resize(8, ' ');

        auto json_integrity = TOOLBOX::JSON_Integrity::verify(airport.sourceFile);

        if (!json_integrity.is_ok) {
            TOOLBOX::JSON_Integrity::printResultOnErrors(json_integrity);
            SESSION::hardShutdown();
            return;
        }

        ctrl.dialog = TOOLBOX::loadJSON(airport.sourceFile);
        ctrl.curr_node = ">ROOT";
        
        if (!SESSION::no_audio)
            Speaker::__init_ctrl_vrec(ctrl, airport);

        __controllers[airport.ICAO] = ctrl;


        X_OUTPUT::xprint(MSG_STYLE::DONE, airport.sourceFile);
        X_OUTPUT::xprint(MSG_STYLE::INVITE);

        SESSION::ctrl_id = ctrl.call_id;
    }
}

bool Speaker::__check_cmd(const std::string& input) {
    if (std::equal(SESSION::cmd_prefix.begin(), SESSION::cmd_prefix.end(), input.begin()) || SESSION::is_monitor) {
        //CMD::run(input);
        return true;
    }

    return false;
}

bool Speaker::__check_quit(const std::string& input) {
    if (input == "q" || input == "quit") {
        std::cout << BG_GREEN_BOLD << BLACK << SESSION::ctrl_id << RESET
                  << GREEN_BOLD << ": " << Lang::getString(T_MSG::GOODBYE)
                  << RESET << std::endl;

        this->__quit = true;
        return true;
    }

    this->__quit = false;
    return false;
}

void Speaker::reset() {
    
}