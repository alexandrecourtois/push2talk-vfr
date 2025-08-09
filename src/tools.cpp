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

#include <msg.h>
#include <weather.h>
#include <fstream>
#include <tools.h>
#include <xprint.h>
#include <inputs.h>
#include <session.h>
#include <lang.h>

std::string TOOLBOX::removeQuotes(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), '\"'), str.end());
    return str;
}

std::optional<int> TOOLBOX::toInt(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (const std::invalid_argument&) {
        return std::nullopt;
    } catch (const std::out_of_range&) {
        return std::nullopt;
    }
}

int TOOLBOX::selectById(int count_id) {
    bool selected = false;
    int index;

    while(!selected) {
        X_OUTPUT::xprint(MSG_STYLE::REQU);

        std::string indexStr;
        std::optional<int> result;

        do {
            //X_INPUT::xscan(indexStr);
            X_INPUT::enableInput();
            std::getline(std::cin, indexStr);
            result = toInt(indexStr);
            X_INPUT::disableInput();
        } while(!result);

        index = *result;

        if (index < 0 || index >= count_id) {
            X_OUTPUT::xprint(MSG_STYLE::M_ERROR, "Invalid ID");
        } else {
            selected = true;
        }
    }

    return index;
}

bool TOOLBOX::isValidIP(const std::string& ip) {
    // Expression régulière pour une adresse IPv4
    const std::regex ipRegex(
        R"(^((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\.){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])$)"
    );

    // Vérifie si l'adresse IP correspond au motif
    return std::regex_match(ip, ipRegex);
}

std::string TOOLBOX::removeAnsiSq(const std::string& input) {
    // Regex pour détecter les séquences ANSI
    std::regex ansi_escape_regex("\033\\[[0-9;]*[mK]");

    // Remplacer toutes les séquences ANSI par une chaîne vide
    std::string result = std::regex_replace(input, ansi_escape_regex, "");

    // Retourner la chaîne nettoyée
    return result;
}

double TOOLBOX::round(double value, int decimalPlaces) {
    double factor = std::pow(10, decimalPlaces);
    return std::round(value * factor) / factor;
}

std::string TOOLBOX::toString(double value, int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}

int TOOLBOX::countLines(const std::string &str) {
    return std::count(str.begin(), str.end(), '\n');
}

void TOOLBOX::displayProgressBar(const std::string& msg, int current, int total) {
    int barWidth = 30;
    float progress = (float)current / total;
    int pos = barWidth * progress;

    std::cout << '\r';
    X_OUTPUT::xprint(MSG_STYLE::INIT, msg, "");
    std::cout << " [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %";
    std::cout.flush();
}

int TOOLBOX::generateRandomNumber(int max) {
    // Initialise le générateur aléatoire avec une graine basée sur l'horloge
    std::random_device rd; // Source aléatoire
    std::mt19937 gen(rd()); // Générateur Mersenne Twister
    std::uniform_int_distribution<> distrib(0, max); // Distribution uniforme entre 0 et max

    return distrib(gen); // Génère le nombre aléatoire
}

std::vector<std::string> TOOLBOX::splitString(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::string current;

    for (char c : str) {
        if (c == delimiter) {
            if (!current.empty()) {
                // Supprime les espaces de la sous-chaîne
                current.erase(std::remove_if(current.begin(), current.end(), ::isspace), current.end());
                result.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }

    // Ajouter le dernier segment, s'il existe
    if (!current.empty()) {
        current.erase(std::remove_if(current.begin(), current.end(), ::isspace), current.end());
        result.push_back(current);
    }

    return result;
}

std::string TOOLBOX::extractBetweenBraces(const std::string& str) {
    size_t start = str.find('{');
    size_t end = str.find('}', start);

    // Si une paire d'accolades est trouvée
    if (start != std::string::npos && end != std::string::npos && end > start) {
        return str.substr(start + 1, end - start - 1);  // Extraire la sous-chaîne entre les accolades
    }

    // Retourne une chaîne vide si les accolades ne sont pas présentes ou sont mal formées
    return "";
}

std::string TOOLBOX::removeSubstring(const std::string& str, const std::string& substring) {
    std::string result = str; // Créer une copie de la chaîne d'entrée
    size_t pos = 0;

    // Rechercher et supprimer toutes les occurrences de la sous-chaîne
    while ((pos = result.find(substring, pos)) != std::string::npos) {
        result.erase(pos, substring.length());
    }

    return result; // Retourner la chaîne modifiée
}

double TOOLBOX::toRad(double degree) {
    return degree * 3.14159 / 180.0;
}

// Fonction pour calculer la distance entre deux points en latitude et longitude
double TOOLBOX::haversine(double lat1, double lon1, double lat2, double lon2) {
    // Convertir les coordonnées en radians
    lat1 = toRad(lat1);
    lon1 = toRad(lon1);
    lat2 = toRad(lat2);
    lon2 = toRad(lon2);

    // Différences de coordonnées
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    // Formule de Haversine
    double a = std::sin(dlat / 2) * std::sin(dlat / 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(dlon / 2) * std::sin(dlon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

    // Calcul de la distance
    return 6371.0f * c; // Distance en kilomètres
}

nlohmann::json TOOLBOX::loadJSON(const std::string& filename) {
    try {
        std::ifstream file(filename);
//        if (!file) {
        //std::cerr << "Erreur: Impossible de lire le fichier " << filename << std::endl;
//            exit(1);
//        }
        nlohmann::json data;
        file >> data;
        return data;
    } catch(const std::exception& e) {
        X_OUTPUT::xprint(MSG_STYLE::M_ERROR, e.what());
        exit(1);
    }
}

bool TOOLBOX::saveJSON(const nlohmann::json& json, const std::string &filename) {
    std::ofstream file(filename);

    if (file) {
        file << json.dump(4);
        file.close();

        return true;
    }

    return false;
}

int TOOLBOX::getLevenshteinDistance(std::string word1, std::string word2) {
    /*    size_t s1_size = str1.size();
        size_t s2_size = str2.size();
        unsigned int i;
        unsigned int j;

        if (s1_size && s2_size) {
            if (s1_size > s2_size) {
                str2.append(s1_size - s2_size, ' ');
                s2_size = str2.size();
            }

            if (s2_size > s1_size) {
                str1.append(s2_size - s1_size, ' ');
                s1_size = str1.size();
            }

            int v0[s1_size + 1];
            int v1[s2_size + 1];

            for(i = 0; i < s1_size; ++i)
                v0[i] = i;

            for(i = 0; i <= s1_size - 1; ++i) {
                v1[0] = i + 1;

                for(j = 0; j <= s2_size - 1; ++j) {
                    int delc = v0[j + 1] + 1;
                    int insc = v1[j] + 1;
                    int subc;

                    if (str1[i] == str2[j])
                        subc = v0[j];
                    else
                        subc = v0[j] + 1;

                    v1[j + 1] = std::min({delc, insc, subc});
                }

                memcpy(v0, v1, (s1_size + 1) * sizeof(int));
            }

            X_OUTPUT::xprint(MSG_STYLE::INFO, str1 + str2 + ": " + std::to_string(v0[s2_size]));
            return v0[s2_size];
        }

        return -1;
    }
    */
    int size1 = word1.size();
    int size2 = word2.size();
    int verif[size1 + 1][size2 + 1]; // Verification matrix i.e. 2D array which will store the calculated distance.

    // If one of the words has zero length, the distance is equal to the size of the other word.
    if (size1 == 0)
        return size2;
    if (size2 == 0)
        return size1;

    // Sets the first row and the first column of the verification matrix with the numerical order from 0 to the length of each word.
    for (int i = 0; i <= size1; i++)
        verif[i][0] = i;
    for (int j = 0; j <= size2; j++)
        verif[0][j] = j;

    // Verification step / matrix filling.
    for (int i = 1; i <= size1; i++) {
        for (int j = 1; j <= size2; j++) {
            // Sets the modification cost.
            // 0 means no modification (i.e. equal letters) and 1 means that a modification is needed (i.e. unequal letters).
            int cost = (word2[j - 1] == word1[i - 1]) ? 0 : 1;

            // Sets the current position of the matrix as the minimum value between a (deletion), b (insertion) and c (substitution).
            // a = the upper adjacent value plus 1: verif[i - 1][j] + 1
            // b = the left adjacent value plus 1: verif[i][j - 1] + 1
            // c = the upper left adjacent value plus the modification cost: verif[i - 1][j - 1] + cost
            verif[i][j] = std::min(
                              std::min(verif[i - 1][j] + 1, verif[i][j - 1] + 1),
                              verif[i - 1][j - 1] + cost
                          );
        }
    }

    // The last position of the matrix will contain the Levenshtein distance.
    if (SESSION::show_ldistance && verif[size1][size2] <= SESSION::levenshtein_threshold)
        X_OUTPUT::xprint(MSG_STYLE::INFO, word1 + " / " + word2 + ": " + std::to_string(verif[size1][size2]));

    return verif[size1][size2];
}

int TOOLBOX::getLevenshteinDistance_alt(const std::string &str1, const std::string &str2) {
    size_t s1_size = str1.size();
    size_t s2_size = str2.size();
    unsigned int i;
    unsigned int j;

    if (s1_size && s2_size) {
        int d[s1_size + 1][s2_size + 1];

        for(i = 1; i <= s1_size; ++i)
            d[i][0] = i;

        for(j = 1; j <= s2_size; ++j)
            d[0][j] = j;

        for(j = 1; j <= s2_size; ++j)
            for(i = 1; i <= s1_size; ++i) {
                int s;

                if (str1[i] == str2[j])
                    s = 0;
                else
                    s = 1;

                d[i][j] = std::min({d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + s});
            }

        return d[s1_size][s2_size];
    }

    return -1;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

Weather TOOLBOX::getWeatherAtLocation(float lat, float lon) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    Weather weather;

    // Remplacez par votre clé API et vos coordonnées GPS
    std::string apiKey = "d8e336a711fcd491a4d73260f123f78f";  // Remplacez par votre clé OpenWeatherMap

    // Construire l'URL avec les coordonnées GPS
    std::string url = "http://api.openweathermap.org/data/2.5/weather?lat=" + std::to_string(lat) + "&lon=" + std::to_string(lon) + "&appid=" + apiKey + "&units=metric";

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "Erreur CURL: " << curl_easy_strerror(res) << std::endl;
            return weather;
        }

        try {
            // Parsing du JSON
            nlohmann::json jsonData = nlohmann::json::parse(readBuffer);

            // Extraction des valeurs
            weather.visibility = jsonData["visibility"]; // -1 si non dispo
            weather.qnh = jsonData["main"]["pressure"];
            weather.windspeed = jsonData["wind"]["speed"];
            weather.winddir = jsonData["wind"]["deg"]; // -1 si non dispo

        } catch (nlohmann::json::parse_error& e) {
            std::cerr << "Erreur lors du parsing JSON: " << e.what() << std::endl;
        }
    }

    return weather;
}

void TOOLBOX::waitFor(int milliseconds) {
    auto start = std::chrono::high_resolution_clock::now();

    while(true) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

        if (duration.count() >= milliseconds)
            break;

        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

std::vector<std::string> TOOLBOX::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream tokenStream(s);
    std::string token;
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Fonction utilitaire : supprime les espaces en début et fin de chaîne.
std::string TOOLBOX::trim(const std::string& s) {
    auto start = s.begin();
    while(start != s.end() && std::isspace(*start)) {
        start++;
    }
    auto end = s.end();
    do {
        end--;
    } while(std::distance(start, end) > 0 && std::isspace(*end));
    return std::string(start, end + 1);
}

// Fonction utilitaire : convertit une chaîne en minuscules.
std::string TOOLBOX::toLower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
    [](unsigned char c) {
        return std::tolower(c);
    });
    return result;
}

void TOOLBOX::wait(unsigned int seconds) {
    seconds++;
    
    while(--seconds) {
        std::cout << seconds << ".." << std::flush;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "0" << std::endl;
}

bool TOOLBOX::JSON_Integrity::isString(const nlohmann::json &json, const std::string& key) {
    auto it = json.find(key);
    return it == json.end() || it->is_string();
}

bool TOOLBOX::JSON_Integrity::isObjectArray(const nlohmann::json &json, const std::string &key) {
    auto it = json.find(key);

    if (it == json.end())
        return true;

    if (!it->is_array())
        return false;

    for(auto& el: *it)
        if (!el.is_object())
            return false;

    return true;
}

TOOLBOX::JSON_Integrity::Result TOOLBOX::JSON_Integrity::verify(const std::string &path) {
    TOOLBOX::JSON_Integrity::Result res{true, {}};

    // 1) Lecture & parsing
    std::ifstream f(path);
    if (!f) {
        res.is_ok = false;
        res.errors.push_back(lang(T_MSG::UNABLE_TO_OPEN_FILE) + ": " + path);
        return res;
    }
    nlohmann::json root;
    try { f >> root; }
    catch (const std::exception& e) {
        res.is_ok = false;
        res.errors.push_back(std::string(lang(T_MSG::INVALID_JSON) + ": ") + e.what());
        return res;
    }

    if (!root.is_object()) {
        res.is_ok = false;
        res.errors.push_back(lang(T_MSG::ROOT_MUST_BE_A_PAIR_OBJECT));
        return res;
    }

    // 2) Collecte des noms d’états existants
    std::unordered_set<std::string> states;
    for (auto it = root.begin(); it != root.end(); ++it) {
        if (!it.value().is_object()) {
            res.is_ok = false;
            res.errors.push_back("Etat \"" + it.key() + "\" n’a pas une définition objet.");
        } else {
            states.insert(it.key());
        }
    }

    // 3) Collecte des valeurs possibles de PATH (depuis les cibles qui définissent "path")
    //    Exemple dans ton fichier: sous >INTRO, certaines targets posent "path":"TOUR_DE_PISTE"/"VOL_LOCAL_OU_DEST". :contentReference[oaicite:1]{index=1}
    std::unordered_set<std::string> pathValues;
    for (auto it = root.begin(); it != root.end(); ++it) {
        const nlohmann::json& def = it.value();
        auto tit = def.find("targets");
        if (tit != def.end() && tit->is_array()) {
            for (const auto& tgt : *tit) {
                if (tgt.is_object()) {
                    auto pit = tgt.find("path");
                    if (pit != tgt.end() && pit->is_string()) {
                        pathValues.insert(pit->get<std::string>());
                    }
                }
            }
        }
    }

    // 4) Validation des champs standards & des cibles
    for (auto it = root.begin(); it != root.end(); ++it) {
        const std::string stateName = it.key();
        const nlohmann::json& def = it.value();

        // Champs simples: phrase, audio, repeat, readback (tous optionnels sauf qu’ils doivent être string s’ils existent)
        if (!isString(def, "phrase"))
            res.errors.push_back(stateName + ": 'phrase' " + lang(T_MSG::MUST_BE_A_STRING) + ".");
        if (!isString(def, "audio"))
            res.errors.push_back(stateName + ": 'audio' " + lang(T_MSG::MUST_BE_A_STRING) + ".");
        if (!isString(def, "repeat"))
            res.errors.push_back(stateName + ": 'repeat' " + lang(T_MSG::MUST_BE_A_STRING) + ".");
        if (!isString(def, "readback"))
            res.errors.push_back(stateName + ": 'readback' " + lang(T_MSG::MUST_BE_A_STRING) + ".");

        // targets : tableau d’objets { keywords: string, goto: string, ... }
        if (!isObjectArray(def, "targets")) {
            res.errors.push_back(stateName + ": 'targets' " + lang(T_MSG::MUST_BE_AN_ARRAY_OF_OBJECTS) + ".");
            continue;
        }

        auto tit = def.find("targets");
        if (tit != def.end() && tit->is_array()) {
            for (size_t i = 0; i < tit->size(); ++i) {
                const nlohmann::json& tgt = (*tit)[i];

                // keywords : requis (string)
                if (!tgt.contains("keywords") || !tgt["keywords"].is_string()) {
                    res.errors.push_back(stateName + " -> targets[" + std::to_string(i) + "]: 'keywords' " + lang(T_MSG::MISSING_OR_NON_STRING) +".");
                } else {
                    // Sanity check sur l’expression (optionnel) : on vérifie juste que la regex peut se compiler
                    try {
                        // Remplace '&' par lookahead simple pour tester la validité globale
                        // (le moteur réel peut être différent, on fait un test basique)
                        std::string rx = tgt["keywords"].get<std::string>();
                        std::string rxTest = std::regex_replace(rx, std::regex("&"), ".*");
                        std::regex re(rxTest, std::regex::icase);
                        (void)re;
                    } catch (...) {
                        res.errors.push_back(stateName + " -> targets[" + std::to_string(i) + "]: " + lang(T_MSG::REGEX_KEYWORDS_INVALID_BASIC_TEST) + ".");
                    }
                }

                // goto : requis si présent; il peut contenir _$PATH (ex. >ALIGNE_PRET_DEPART_$PATH) :contentReference[oaicite:2]{index=2}
                if (tgt.contains("goto")) {
                    if (!tgt["goto"].is_string()) {
                        res.errors.push_back(stateName + " -> targets[" + std::to_string(i) + "]: " + lang(T_MSG::GOTO_MUST_BE_A_STRING) + ".");
                    } else {
                        std::string dest = tgt["goto"].get<std::string>();
                        bool okGoto = false;

                        if (dest.find("$PATH") == std::string::npos) {
                            // goto direct : doit exister
                            okGoto = states.count(dest) > 0;
                        } else {
                            // goto paramétré : on valide contre les valeurs connues de PATH, ou au moins une correspondance
                            if (pathValues.empty()) {
                                // S’il n’y a aucune valeur connue, on tolère mais on le signale
                                res.errors.push_back(stateName + " -> targets[" + std::to_string(i) + "]: " + lang(T_MSG::GOTO_WITH_MISSING_PATH) + ".");
                                okGoto = true; // tolérance
                            } else {
                                size_t okCount = 0;
                                for (const auto& pv : pathValues) {
                                    std::string candidate = dest;
                                    // Remplacement simple
                                    size_t pos = 0;
                                    while ((pos = candidate.find("$PATH", pos)) != std::string::npos) {
                                        candidate.replace(pos, 5, pv);
                                        pos += pv.size();
                                    }
                                    if (states.count(candidate)) okCount++;
                                }
                                okGoto = okCount > 0;
                            }
                        }

                        if (!okGoto) {
                            res.errors.push_back(stateName + " -> targets[" + std::to_string(i) + "]: " + lang(T_MSG::GOTO_MISSING_STATE) + ": " + dest);
                        }
                    }
                }
            }
        }
    }

    if (!res.errors.empty()) res.is_ok = false;
    return res;
}

void TOOLBOX::JSON_Integrity::printResultOnErrors(const Result &result) {
    X_OUTPUT::xprint(MSG_STYLE::M_ERROR, lang(T_MSG::ERRORS_FOUND) + ": " + std::to_string(result.errors.size()));

    for (auto& e: result.errors)
        X_OUTPUT::xprint(MSG_STYLE::M_ERROR, e);
}