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

class RADIO {
private:
    static void __EVENT_com1freq();
    static void __EVENT_com2freq();
    static void __EVENT_com1power();
    static void __EVENT_com2power();
    static void __EVENT_com1active();
    static void __EVENT_com2active();
    static void __EVENT_battery();

    static int __com1_freq;
    static int __com2_freq;
    static int __com1_status;
    static int __com2_status;
    static int __com1_power;
    static int __com2_power;
    static int __battery_on;

    RADIO() = default;

    static double __white_rabbit_obj(double factor);

public:
    static void init();

    enum class COM_RADIO {
        COM1,
        COM2,
        ANY
    };

    enum class FREQ_TYPE {
        ICAO,
        NUMERIC
    };

    static constexpr double VHF_MAX_RANGE   =   200.0f;
    static constexpr double BASE_VISIBILITY = 64000.0f;

    static double getRange();
    static int getNoiseVolume();
    static int getSpeakVolume(int noiseVolume);
    static bool isWithinRange();
    static double getDistanceToAirport();
    static void updateStatus();
    static int getFreq(COM_RADIO radio);
    static bool isActive(COM_RADIO radio);
    static bool isPower(COM_RADIO radio = COM_RADIO::ANY);
    static void showStatus(COM_RADIO radio = COM_RADIO::ANY);
    static void showFrequency(FREQ_TYPE type, COM_RADIO radio = COM_RADIO::ANY);
    static float getVHF_Quality(float distance, float freqMhz, float txAlt, float rxAlt, float precipitation, float txPower = 40.0f, float rxSensitivity = -90.0f);
};