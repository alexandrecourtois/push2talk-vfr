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
#include <weather.h>

class TOOLBOX {
private:
    TOOLBOX() = default;

public:
    struct JSON_Integrity {
        struct Result {
            bool is_ok;
            std::vector<std::string> errors;
        };

        static bool isString(const nlohmann::json& json, const std::string& key);
        static bool isObjectArray(const nlohmann::json& json, const std::string& key);
        static Result verify(const std::string& path);
        static void printResultOnErrors(const Result& result);
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
};