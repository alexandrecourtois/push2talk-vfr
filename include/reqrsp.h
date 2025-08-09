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

#include "third_party/cereal/archives/binary.hpp"
#include <pch.h>
#include <enums.h>
#include <lockable.h>

#define REQ_GET_DATAFRAME       int(0)
#define REQ_UPD_AIRPORT         int(1)

#define RSP_ACK                 int(0)

class Req: public Lockable {
public:
    Request type;
    double argD_1;
    double argD_2;
    double argD_3;

    template<class Archive> void serialize(Archive& archive) {
        archive(type,
                argD_1,
                argD_2,
                argD_3);
    }
};

class Rsp_Dataframe: public Lockable {
public:
    int xp_major;               // XP release number
    int xp_minor;               // ...

    char tailnum[256];          // Aircraft tail number

    float visibility;           // Visibility in m around aircraft
    float qnh;                  // QNH around aircraft
    float windspeed;            // Wind speed around aircraft
    float winddir;              // Wind direction around aircraft
    float latitude;             // ...
    float longitude;            // Position of aircraft (XYZ)
    double altitude;            // ...
    int com1_freq;              // COM1 radio frequency
    int com2_freq;              // COM2 ...
    float com1_vol;             // COM1 radio volume
    float com2_vol;             // COM2 ...
    int com1_active;            // COM1 radio status
    int com2_active;            // COM2 ...
    int com1_power;             // COM1 power status
    int com2_power;             // COM2 ...
    int battery_on[8];
    float infreq_visibility;
    float infreq_qnh;
    float infreq_windspeed;
    float infreq_winddir;
    bool weatherUpToDate;
    float region_rain;
    bool sim_paused;
    float local_time_sec;
    bool server_down;

    auto operator <=>(const Rsp_Dataframe&) const = default;

    template<class Archive> void serialize(Archive &archive) {
        archive(altitude,
                battery_on,
                com1_active,
                com1_freq,
                com1_power,
                com1_vol,
                com2_active,
                com2_freq,
                com2_power,
                com2_vol,
                infreq_qnh,
                infreq_visibility,
                infreq_winddir,
                infreq_windspeed,
                latitude,
                longitude,
                local_time_sec,
                qnh,
                region_rain,
                server_down,
                sim_paused,
                tailnum,
                visibility,
                weatherUpToDate,
                winddir,
                windspeed,
                xp_major,
                xp_minor);
    }
};

template<typename T = int> class Rsp: public Lockable {
public:
    T value;

    template<class Archive> void serialize(Archive& archive) {
        archive(value);
    }
};

typedef std::vector<uint8_t> T_SERIALIZED;
typedef uint8_t T_BYTE;

template<typename T> T_SERIALIZED serialize(const T& src) {
    std::stringstream ss;
    cereal::BinaryOutputArchive archive(ss);
    archive(src);
    std::string str = ss.str();

    return std::vector<uint8_t>(str.begin(), str.end());
}

template<typename T> void deserialize(T& dest, T_SERIALIZED& src) {
    std::string str(src.begin(), src.end());
    std::stringstream ss(str);
    cereal::BinaryInputArchive archive(ss);
    archive(dest);
}