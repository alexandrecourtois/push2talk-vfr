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

#define UNSUPPORTED_AIRPORT     std::string("UNSUPPORTED_AIRPORT")

std::string get_airport_by_frequency(float frequency);
std::string get_active_rwy(int windDirection, const std::map<std::string, int>& runways);
void update_current_airport();
bool supported_airport();

class AIRPORTS {
public:
    struct Airport {
        std::string ICAO;
        float lat;
        float lon;
        float alt;
        int tower;
        int atis;
        std::map<std::string, int> runways;
        std::map<std::string, std::string> holdpoints;
        std::string active_rwy;
        Weather lastKnownWeather;
        std::string sourceFile;
        std::string type;
        std::string lang;

        void updateWeather();
    };

private:
    static nlohmann::json __airports;
    static Airport __current_airport;

    AIRPORTS() = default;

public:
    static void loadAiports(const std::string& filename);
    static std::string getAirportByFrequency(float frequency);
    static std::string getActiveRunway(int windDirection);
    static void updateCurrentAirport();
    static bool isSupportedAirport();
    static Airport& getCurrentAirport();
};