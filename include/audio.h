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

class AUDIO {
private:
    static std::map<std::string, Mix_Chunk*>    __WAV_files;
    static std::map<std::string, int>           __WAV_durations;
    static std::vector<unsigned char>           __audio_stream;
    static bool                                 __is_recording;
    static SDL_AudioDeviceID                    __input_device_id;
    static SDL_AudioDeviceID                    __output_device_id;
    static std::string                          __input_device_name;
    static std::string                          __output_device_name;
    static int                                  __sample_rate;
    static std::map<std::string, std::string>   __phrases;

    AUDIO() = default;

    static void __init_phrs(const std::string& phrasePath);
    static void __load_audio(const std::string& audioPath);
    static void __play(const std::vector<std::string>& files, int channel = CHANNEL_RANDOM);
    static int  __play(const std::string& file, int channel = CHANNEL_RANDOM, bool loop = false);
    static int  __play(Mix_Chunk* src, int channel = CHANNEL_RANDOM, bool loop = false);
    static void __play_tailnum();
    static void __play_radiostart();
    static void __play_radiostop();
    static void __play_radiocom(const std::vector<std::string>& files);
    static void __play_silence(unsigned int msec);
    static void __audio_callback(void* userdata, Uint8* stream, int len);
    static int  __get_audio_duration(Mix_Chunk* sound);
    static Mix_Chunk* __concat_audio(const std::vector<std::string>& audio, bool isFromFiles, int trim = 0);

    static std::vector<std::string> __phrase_to_waves(const std::string& phrase);

public:
    enum class Type {
        TAILNUM,
        RADIOCOM,
        RADIOSTART,
        RADIOSTOP,
    };

    enum class Device {
        AUDIO_INPUT,
        AUDIO_OUTPUT
    };

    static constexpr int CHANNEL_RANDOM         = -1;
    static constexpr int CHANNEL_RADIO_REC      =  0;
    static constexpr int CHANNEL_RADIO_COM      =  1;
    static constexpr int CHANNEL_RADIO_NOISE    =  2;

    static void init(const std::string& audioPath, const std::string& phrasePath);
    static void free();
    static void selectDevice(Device deviceType);

    static void startRecording();
    static void stopRecording();

    static void play(Type type, const std::vector<std::string>& files = std::vector<std::string>());

    static bool isRecording();

    static const std::vector<unsigned char>& getAudioStream();

    const static std::string& getInputDeviceName();
    const static std::string& getOutputDeviceName();
};