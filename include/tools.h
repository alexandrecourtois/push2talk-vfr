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
#include <unordered_set>
#include <weather.h>

class TOOLBOX {
private:
    TOOLBOX() = default;

public:
    class JSON_Validator {
        private:
            static bool __isString(const nlohmann::json& json, const char* key);
            static bool __isObjectArray(const nlohmann::json& json, const char* key);

        public:
            struct Result {
                bool is_ok;
                unsigned int branch_explored;
                std::vector<std::string> errors;
                std::vector<std::string> warnings;

                static void printErrors(const Result& result);
                static void printWarnings(const Result& result);
                static void merge(TOOLBOX::JSON_Validator::Result& dst, const TOOLBOX::JSON_Validator::Result& src);
            };

            class Dialog {
                private:
                    static std::string __join_path(const std::string& a, const std::string& b);
                    static std::string __ctx(const std::string& entryPoint, const std::string& subpath = "", std::optional<size_t> index = std::nullopt);
                    static void __add_error(Result& r, const std::string& context, const std::string& message, const std::string& suggestion = "");
                    static void __add_warning(Result& r, const std::string& context, const std::string& message, const std::string& suggestion = "");

                public:
                    static Result verify(const nlohmann::json& json,
                        const std::string& entryPoint = ">ROOT",
                        const std::string& path = "",
                        std::unordered_set<std::string>* history_ptr = nullptr);
            };
    };

    static std::string removeQuotes(std::string str);
    static std::string removeAnsiSq(const std::string& str);
    static std::string toString(double value, int precision);
    static std::optional<int> toInt(const std::string& str);
    static int selectById(int count_id);
    static int countLines(const std::string& str);
    static int generateRandomNumber(int max);
    static double round(double value, int decimal);
    static bool isValidIP(const std::string& str);
    static void displayProgressBar(const std::string& msg, int current, int total);
    static std::vector<std::string> splitString(const std::string& str, char delimiter);
    static std::string extractBetweenBraces(const std::string& str);
    static std::string removeSubstring(const std::string& str, const std::string& substring);
    static double haversine(double lat1, double lon1, double lat2, double lon2);
    static double toRad(double degree);
    static nlohmann::json loadJSON(const std::string& filename);
    static bool saveJSON(const nlohmann::json& json, const std::string& filename);
    static int getLevenshteinDistance(std::string str1, std::string str2);
    static int getLevenshteinDistance_alt(const std::string& str1, const std::string& str2);
    static Weather getWeatherAtLocation(float lat, float lon);
    static void waitFor(int milliseconds);
    static std::vector<std::string> split(const std::string& s, char delimiter);
    static std::string trim(const std::string& s);
    static std::string toLower(const std::string& s);
    static void wait(unsigned int seconds);
    static std::string toICAO(char c);
    static std::string tailnumToICAO(const std::string& tailnum_str);
    static std::string toConsole(std::string& str);
    static std::string toTTS(std::string &str);
    static bool isUpperCase(const std::string& str);
};