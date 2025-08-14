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

#include <algorithm>
#include <fstream>
#include <inputs.h>
#include <lang.h>
#include <msg.h>
#include <regex>
#include <session.h>
#include <string>
#include <tools.h>
#include <unordered_set>
#include <weather.h>
#include <xprint.h>

std::string TOOLBOX::removeQuotes(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), '\"'), str.end());
    return str;
}

std::optional<int> TOOLBOX::toInt(const std::string &str) {
    try {
        return std::stoi(str);
    } catch (const std::invalid_argument &) {
        return std::nullopt;
    } catch (const std::out_of_range &) {
        return std::nullopt;
    }
}

int TOOLBOX::selectById(int count_id) {
    bool selected = false;
    int index;

    while (!selected) {
        X_OUTPUT::xprint(MSG_STYLE::REQU);

        std::string indexStr;
        std::optional<int> result;

        do {
            // X_INPUT::xscan(indexStr);
            X_INPUT::enableInput();
            std::getline(std::cin, indexStr);
            result = toInt(indexStr);
            X_INPUT::disableInput();
        } while (!result);

        index = *result;

        if (index < 0 || index >= count_id) {
            X_OUTPUT::xprint(MSG_STYLE::M_ERROR, "Invalid ID");
        } else {
            selected = true;
        }
    }

    return index;
}

bool TOOLBOX::isValidIP(const std::string &ip) {
    // Expression régulière pour une adresse IPv4
    const std::regex ipRegex(
        R"(^((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\.){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])$)");

    // Vérifie si l'adresse IP correspond au motif
    return std::regex_match(ip, ipRegex);
}

std::string TOOLBOX::removeAnsiSq(const std::string &input) {
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

void TOOLBOX::displayProgressBar(const std::string &msg, int current,
                                 int total) {
    int barWidth = 30;
    float progress = (float)current / total;
    int pos = barWidth * progress;

    std::cout << '\r';
    X_OUTPUT::xprint(MSG_STYLE::INIT, msg, "");
    std::cout << " [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos)
            std::cout << "=";
        else if (i == pos)
            std::cout << ">";
        else
            std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %";
    std::cout.flush();
}

int TOOLBOX::generateRandomNumber(int max) {
    // Initialise le générateur aléatoire avec une graine basée sur l'horloge
    std::random_device rd;  // Source aléatoire
    std::mt19937 gen(rd()); // Générateur Mersenne Twister
    std::uniform_int_distribution<> distrib(
        0, max); // Distribution uniforme entre 0 et max

    return distrib(gen); // Génère le nombre aléatoire
}

std::vector<std::string> TOOLBOX::splitString(const std::string &str,
        char delimiter) {
    std::vector<std::string> result;
    std::string current;

    for (char c : str) {
        if (c == delimiter) {
            if (!current.empty()) {
                // Supprime les espaces de la sous-chaîne
                current.erase(std::remove_if(current.begin(), current.end(), ::isspace),
                              current.end());
                result.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }

    // Ajouter le dernier segment, s'il existe
    if (!current.empty()) {
        current.erase(std::remove_if(current.begin(), current.end(), ::isspace),
                      current.end());
        result.push_back(current);
    }

    return result;
}

std::string TOOLBOX::extractBetweenBraces(const std::string &str) {
    size_t start = str.find('{');
    size_t end = str.find('}', start);

    // Si une paire d'accolades est trouvée
    if (start != std::string::npos && end != std::string::npos && end > start) {
        return str.substr(start + 1,
                          end - start -
                          1); // Extraire la sous-chaîne entre les accolades
    }

    // Retourne une chaîne vide si les accolades ne sont pas présentes ou sont mal
    // formées
    return "";
}

std::string TOOLBOX::removeSubstring(const std::string &str,
                                     const std::string &substring) {
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
    double a =
        std::sin(dlat / 2) * std::sin(dlat / 2) +
        std::cos(lat1) * std::cos(lat2) * std::sin(dlon / 2) * std::sin(dlon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

    // Calcul de la distance
    return 6371.0f * c; // Distance en kilomètres
}

nlohmann::json TOOLBOX::loadJSON(const std::string &filename) {
    try {
        std::ifstream file(filename);
        //        if (!file) {
        // std::cerr << "Erreur: Impossible de lire le fichier " << filename <<
        // std::endl;
        //            exit(1);
        //        }
        nlohmann::json data;
        file >> data;
        return data;
    } catch (const std::exception &e) {
        X_OUTPUT::xprint(MSG_STYLE::M_ERROR, e.what());
        exit(1);
    }
}

bool TOOLBOX::saveJSON(const nlohmann::json &json,
                       const std::string &filename) {
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

            X_OUTPUT::xprint(MSG_STYLE::INFO, str1 + str2 + ": " +
    std::to_string(v0[s2_size])); return v0[s2_size];
        }

        return -1;
    }
    */
    int size1 = word1.size();
    int size2 = word2.size();
    int verif[size1 + 1][size2 + 1]; // Verification matrix i.e. 2D array which
    // will store the calculated distance.

    // If one of the words has zero length, the distance is equal to the size of
    // the other word.
    if (size1 == 0)
        return size2;
    if (size2 == 0)
        return size1;

    // Sets the first row and the first column of the verification matrix with the
    // numerical order from 0 to the length of each word.
    for (int i = 0; i <= size1; i++)
        verif[i][0] = i;
    for (int j = 0; j <= size2; j++)
        verif[0][j] = j;

    // Verification step / matrix filling.
    for (int i = 1; i <= size1; i++) {
        for (int j = 1; j <= size2; j++) {
            // Sets the modification cost.
            // 0 means no modification (i.e. equal letters) and 1 means that a
            // modification is needed (i.e. unequal letters).
            int cost = (word2[j - 1] == word1[i - 1]) ? 0 : 1;

            // Sets the current position of the matrix as the minimum value between a
            // (deletion), b (insertion) and c (substitution). a = the upper adjacent
            // value plus 1: verif[i - 1][j] + 1 b = the left adjacent value plus 1:
            // verif[i][j - 1] + 1 c = the upper left adjacent value plus the
            // modification cost: verif[i - 1][j - 1] + cost
            verif[i][j] = std::min(std::min(verif[i - 1][j] + 1, verif[i][j - 1] + 1),
                                   verif[i - 1][j - 1] + cost);
        }
    }

    // The last position of the matrix will contain the Levenshtein distance.
    if (SESSION::show_ldistance &&
            verif[size1][size2] <= SESSION::levenshtein_threshold)
        X_OUTPUT::xprint(MSG_STYLE::INFO, word1 + " / " + word2 + ": " +
                         std::to_string(verif[size1][size2]));

    return verif[size1][size2];
}

int TOOLBOX::getLevenshteinDistance_alt(const std::string &str1,
                                        const std::string &str2) {
    size_t s1_size = str1.size();
    size_t s2_size = str2.size();
    unsigned int i;
    unsigned int j;

    if (s1_size && s2_size) {
        int d[s1_size + 1][s2_size + 1];

        for (i = 1; i <= s1_size; ++i)
            d[i][0] = i;

        for (j = 1; j <= s2_size; ++j)
            d[0][j] = j;

        for (j = 1; j <= s2_size; ++j)
            for (i = 1; i <= s1_size; ++i) {
                int s;

                if (str1[i] == str2[j])
                    s = 0;
                else
                    s = 1;

                d[i][j] =
                    std::min({d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + s});
            }

        return d[s1_size][s2_size];
    }

    return -1;
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                     std::string *output) {
    size_t total_size = size * nmemb;
    output->append((char *)contents, total_size);
    return total_size;
}

Weather TOOLBOX::getWeatherAtLocation(float lat, float lon) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    Weather weather;

    // Remplacez par votre clé API et vos coordonnées GPS
    std::string apiKey =
        "d8e336a711fcd491a4d73260f123f78f"; // Remplacez par votre clé
    // OpenWeatherMap

    // Construire l'URL avec les coordonnées GPS
    std::string url = "http://api.openweathermap.org/data/2.5/weather?lat=" +
                      std::to_string(lat) + "&lon=" + std::to_string(lon) +
                      "&appid=" + apiKey + "&units=metric";

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

        } catch (nlohmann::json::parse_error &e) {
            std::cerr << "Erreur lors du parsing JSON: " << e.what() << std::endl;
        }
    }

    return weather;
}

void TOOLBOX::waitFor(int milliseconds) {
    auto start = std::chrono::high_resolution_clock::now();

    while (true) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

        if (duration.count() >= milliseconds)
            break;

        // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

std::vector<std::string> TOOLBOX::split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream tokenStream(s);
    std::string token;
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Fonction utilitaire : supprime les espaces en début et fin de chaîne.
std::string TOOLBOX::trim(const std::string &s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start)) {
        start++;
    }
    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));
    return std::string(start, end + 1);
}

// Fonction utilitaire : convertit une chaîne en minuscules.
std::string TOOLBOX::toLower(const std::string &s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
    [](unsigned char c) {
        return std::tolower(c);
    });
    return result;
}

void TOOLBOX::wait(unsigned int seconds) {
    seconds++;

    while (--seconds) {
        std::cout << seconds << ".." << std::flush;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "0" << std::endl;
}

std::string TOOLBOX::toICAO(char c) {
    static const std::unordered_map<char, std::string> icaoMap = {
        {'A', lang(T_MSG::NATO_A)}, {'B', lang(T_MSG::NATO_B)},
        {'C', lang(T_MSG::NATO_C)}, {'D', lang(T_MSG::NATO_D)},
        {'E', lang(T_MSG::NATO_E)}, {'F', lang(T_MSG::NATO_F)},
        {'G', lang(T_MSG::NATO_G)}, {'H', lang(T_MSG::NATO_H)},
        {'I', lang(T_MSG::NATO_I)}, {'J', lang(T_MSG::NATO_J)},
        {'K', lang(T_MSG::NATO_K)}, {'L', lang(T_MSG::NATO_L)},
        {'M', lang(T_MSG::NATO_M)}, {'N', lang(T_MSG::NATO_N)},
        {'O', lang(T_MSG::NATO_O)}, {'P', lang(T_MSG::NATO_P)},
        {'Q', lang(T_MSG::NATO_Q)}, {'R', lang(T_MSG::NATO_R)},
        {'S', lang(T_MSG::NATO_S)}, {'T', lang(T_MSG::NATO_T)},
        {'U', lang(T_MSG::NATO_U)}, {'V', lang(T_MSG::NATO_V)},
        {'W', lang(T_MSG::NATO_W)}, {'X', lang(T_MSG::NATO_X)},
        {'Y', lang(T_MSG::NATO_Y)}, {'Z', lang(T_MSG::NATO_Z)}
    };

    char upperC = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));

    auto it = icaoMap.find(upperC);
    if (it != icaoMap.end()) {
        return it->second;
    } else {
        return std::string(1, c); // Renvoie le caractère tel quel si non trouvé
    }
}

std::string TOOLBOX::tailnumToICAO(const std::string &tailnum_str) {
    if (tailnum_str.size() == 6) {
        char _1st = static_cast<char>(
                        std::toupper(static_cast<unsigned char>(tailnum_str[0])));
        char _5th = static_cast<char>(
                        std::toupper(static_cast<unsigned char>(tailnum_str[4])));
        char _6th = static_cast<char>(
                        std::toupper(static_cast<unsigned char>(tailnum_str[5])));

        if (_1st >= 'A' && _1st <= 'Z' && _5th >= 'A' && _5th <= 'Z' &&
                _6th >= 'A' && _6th <= 'Z')
            return toICAO(_1st) + " " + toICAO(_5th) + " " + toICAO(_6th);
    }

    return std::string();
}

std::string TOOLBOX::toConsole(std::string &str) {
    std::replace(str.begin(), str.end(), '_', ' ');
    return str;
}

std::string TOOLBOX::toTTS(std::string &str) {
    str.erase(std::remove(str.begin(), str.end(), '_'), str.end());
    return str;
}

bool TOOLBOX::isUpperCase(const std::string& str) {
    return std::ranges::all_of(str, [](unsigned char c) {
        return !std::isalpha(c) || std::isupper(c);
    });
}

void TOOLBOX::JSON_Validator::Result::printErrors(const TOOLBOX::JSON_Validator::Result &result) {
    X_OUTPUT::xprint(MSG_STYLE::M_ERROR, std::to_string(result.errors.size()) + " " + lang(T_MSG::ERRORS_FOUND));

    for(auto& line: result.errors) {
        X_OUTPUT::xprint(MSG_STYLE::M_ERROR, line);
    }
}

void TOOLBOX::JSON_Validator::Result::printWarnings(const TOOLBOX::JSON_Validator::Result &result) {
    if (result.warnings.size()) {
        X_OUTPUT::xprint(MSG_STYLE::WARNING, std::to_string(result.warnings.size()) + " " + lang(T_MSG::WARNINGS_FOUND));

        for (auto& line: result.warnings) {
            X_OUTPUT::xprint(MSG_STYLE::WARNING, line);
        }
    }
}

// --- Helpers lisibles -------------------------------------------------------

std::string TOOLBOX::JSON_Validator::Dialog::__join_path(const std::string& a, const std::string& b) {
    if (a.empty()) return b;
    if (b.empty()) return a;
    return a + "." + b;
}

// Construit un contexte du type: >NODE.targets[3].keywords
std::string TOOLBOX::JSON_Validator::Dialog::__ctx(const std::string& entryPoint, const std::string& subpath, std::optional<size_t> index)
{
    std::string p = entryPoint;
    if (!subpath.empty()) p = __join_path(p, subpath);
    if (index.has_value()) p += "[" + std::to_string(*index) + "]";
    return p;
}

void TOOLBOX::JSON_Validator::Dialog::__add_error(Result& r, const std::string& context, const std::string& message, const std::string& suggestion)
{
    r.is_ok = false;
    std::string out = "Erreur dans \"" + context + "\" : " + message;
    if (!suggestion.empty()) out += " Astuce : " + suggestion;
    r.errors.push_back(out);
}

void TOOLBOX::JSON_Validator::Dialog::__add_warning(Result& r, const std::string& context, const std::string& message, const std::string& suggestion)
{
    std::string out = "Avertissement dans \"" + context + "\" : " + message;
    if (!suggestion.empty()) out += " Astuce : " + suggestion;
    r.warnings.push_back(out);
}

void TOOLBOX::JSON_Validator::Result::merge(TOOLBOX::JSON_Validator::Result& dst, const TOOLBOX::JSON_Validator::Result& src) {
    dst.is_ok = dst.is_ok && src.is_ok;
    dst.errors.insert(dst.errors.end(), src.errors.begin(), src.errors.end());
    dst.warnings.insert(dst.warnings.end(), src.warnings.begin(), src.warnings.end());
}

TOOLBOX::JSON_Validator::Result TOOLBOX::JSON_Validator::Dialog::verify(const nlohmann::json &json, const std::string& entryPoint, const std::string& path, std::unordered_set<std::string>* history_ptr) {
    //bool result = true;
    Result res;
    res.is_ok = true;

    std::unordered_set<std::string> history;
    
    if (!history_ptr)
        history_ptr = &history;

    if (!json.contains(entryPoint)) {
        __add_error(res,
            __ctx(entryPoint),
            lang(T_MSG::KEY) + " \"" + entryPoint + "\" " + lang(T_MSG::MUST_EXIST_AT_ROOT) + "." );

        return res;
    }

    if (entryPoint[0] != '>') {
        __add_warning(res, __ctx(entryPoint), lang(T_MSG::EXPECTED_1ST_CHAR_MISSING));
    }

    if (!TOOLBOX::isUpperCase(entryPoint)) {
        __add_warning(res,__ctx(entryPoint), lang(T_MSG::UPPERCASE_EXPECTED));
    }

    const std::string key = entryPoint + "|" + path;

    if (history_ptr->count(key)) {
        //res.is_ok = true;
        return res;
    }

    history_ptr->insert(key);
    std::cout << key;

    if (entryPoint != ">ROOT") {
        if (!json[entryPoint].contains("phrase")) {
            __add_error(res, __ctx(entryPoint), lang(T_MSG::MISSING_PHRASE));
            //return res;
        } else {
            if (!json[entryPoint]["phrase"].is_string()) {
                __add_error(res, __ctx(entryPoint, "phrase"), lang(T_MSG::MUST_BE_A_STRING));
                //return res;
            }
        }
    }

    if (json[entryPoint].contains("readback")) {
        if (!json[entryPoint]["readback"].is_string()) {
            __add_error(res, __ctx(entryPoint, "readback"), lang(T_MSG::MUST_BE_A_STRING));
            //return res;
        } else {
            //////////////////////////////////////////////////////////
        }
    }

    if (json[entryPoint].contains("targets")) {
        if (!json[entryPoint]["targets"].is_array()) {
            __add_error(res, __ctx(entryPoint), lang(T_MSG::MUST_BE_AN_ARRAY_OF_OBJECTS));
        }
        else {
            const auto& targets = json[entryPoint]["targets"];
            for(size_t i = 0; i < targets.size(); ++i) {
                const auto& target = targets[i];
                const std::string t_ctx = __ctx(entryPoint, "targets", i);

                if (!target.contains("keywords")) {
                    __add_error(res, __join_path(t_ctx, "keywords"), lang(T_MSG::MISSING_KEYWORDS));
                    continue;
                } else {
                    if (!target["keywords"].is_string()) {
                        __add_error(res, __join_path(t_ctx, "keywords"), lang(T_MSG::MUST_BE_A_STRING));
                        continue;
                    } else {
                        ///////////////////////////////////////////////
                    }
                }

                if (target.contains("goto")) {

                    if (!target["goto"].is_string()) {
                        __add_error(res, __join_path(t_ctx, "goto"), lang(T_MSG::MUST_BE_A_STRING));
                    } else {
                        std::string goto_str = target["goto"];

                        if (goto_str.ends_with("_$PATH")) {
                            goto_str.resize(goto_str.size() - 5);
                            
                            if (!path.empty())
                                goto_str += path;
                        }

                        if (json.contains(goto_str) && goto_str != ">ROOT") {
                            if (target.contains("path")) {
                                //res.is_ok = res.is_ok && verify(json, goto_str, target["path"], history_ptr).is_ok;
                                auto child = verify(json, goto_str, target["path"], history_ptr);
                                TOOLBOX::JSON_Validator::Result::merge(res, child);
                            }
                            else {
                                //res.is_ok = res.is_ok && verify(json, goto_str, path, history_ptr).is_ok;
                                auto child = verify(json, goto_str, path, history_ptr);
                                TOOLBOX::JSON_Validator::Result::merge(res, child);
                            }
                        }
                        else
                            if (goto_str == ">ROOT")
                                continue;
                            else {
                                //res.is_ok = false;
                                __add_error(res, __join_path(t_ctx, "goto"), lang(T_MSG::GOTO_MISSING_STATE) + " \"" + goto_str + "\"");
                                continue;
                            }
                    }
                } else {
                    //res.is_ok = false;
                    __add_error(res, __join_path(t_ctx, "goto"), lang(T_MSG::MISSING_GOTO));
                }
            }
        }
    }

    return res;
}