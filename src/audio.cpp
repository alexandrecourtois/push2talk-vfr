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

#include "SDL2/SDL_audio.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_stdinc.h"
#include "piper.h"
#include "speaker.h"
#include <cstdint>
#include <msg.h>
#include <audio.h>
#include <string>
#include <tools.h>
#include <xprint.h>
#include <session.h>
#include <radio.h>
#include <callbacks.h>
#include <lang.h>
#include <sox.h>

std::map<std::string, Mix_Chunk*>   AUDIO::__WAV_files;
std::map<std::string, int>          AUDIO::__WAV_durations;
std::vector<unsigned char>          AUDIO::__audio_stream;
bool                                AUDIO::__is_recording = false;
SDL_AudioDeviceID                   AUDIO::__input_device_id;
SDL_AudioDeviceID                   AUDIO::__output_device_id;
int                                 AUDIO::__sample_rate = 16000;
std::map<std::string, std::string>  AUDIO::__phrases;
std::string                         AUDIO::__input_device_name;
std::string                         AUDIO::__output_device_name;
bool                                AUDIO::__isSelectedDeviceOpen = false;

const std::string& AUDIO::getInputDeviceName() {
    return __input_device_name;
}

const std::string& AUDIO::getOutputDeviceName() {
    return __output_device_name;
}

void AUDIO::__play_radiostart() {
    //__play("debut.wav", CHANNEL_RADIO_REC);
    //__play("fond.wav", CHANNEL_RADIO_REC, true);
}

void AUDIO::__play_radiostop() {
    //Mix_HaltChannel(CHANNEL_RADIO_REC);
    //__play("fin.wav", CHANNEL_RADIO_REC);
}

int AUDIO::__get_audio_duration(Mix_Chunk* sound) {
    int frequency, channels;
    Uint16 format;

    Mix_QuerySpec(&frequency, &format, &channels);

    int sampleSize = (format == AUDIO_U8) ? 1 : 2;

    return (sound->alen * 1000) / (frequency * channels * sampleSize);
}

void AUDIO::init(const std::string& audioPath, const std::string& phrasePath) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        X_OUTPUT::xprint(MSG_STYLE::M_ERROR, lang(T_MSG::ERROR_WHILE_INITIALIZING_SDL_AUDIO));
        SESSION::no_audio = true;
    }

    selectDevice(AUDIO::Device::AUDIO_INPUT);
    selectDevice(AUDIO::Device::AUDIO_OUTPUT);
}

void AUDIO::__audio_callback(void* userdata, Uint8* stream, int len) {
    if (isRecording()) {
        __audio_stream.insert(__audio_stream.end(), stream, stream + len);
    }
}

void AUDIO::selectDevice(AUDIO::Device deviceType) {
    bool isCapture = (deviceType == AUDIO::Device::AUDIO_INPUT) ? SDL_TRUE : SDL_FALSE;
    std::string msg = (deviceType == AUDIO::Device::AUDIO_INPUT) ? lang(T_MSG::SELECT_AUDIO_INPUT) : lang(T_MSG::SELECT_AUDIO_OUTPUT);

    // Obtenir le nombre de périphériques d'entrée audio disponibles
    int numAudioDevices = SDL_GetNumAudioDevices(isCapture);  // SDL_TRUE pour les périphériques d'entrée
    if (numAudioDevices <= 0) {
        X_OUTPUT::xprint(MSG_STYLE::M_ERROR, lang(T_MSG::ERROR_WHILE_ENUMERATING_AUDIO));
        //SESSION::no_audio = true;    à différencier input/output
        return;
    }

    const char* selectedDeviceName = nullptr;
    std::string savedDeviceName_str;

    if (deviceType == AUDIO::Device::AUDIO_INPUT)
        savedDeviceName_str = SESSION::getConfigKey(SESSION::CONFIG_KEYS::CFG_AUDIO_INPUT);
    else
        savedDeviceName_str = SESSION::getConfigKey(SESSION::CONFIG_KEYS::CFG_AUDIO_OUTPUT);

    //selectedDeviceName = savedDeviceName_str.c_str();

    X_OUTPUT::xprint(MSG_STYLE::INFO, msg);

    for (int i = 0; i < numAudioDevices; ++i) {
        const char* deviceName = SDL_GetAudioDeviceName(i, isCapture);  // SDL_TRUE pour l'entrée
        if (deviceName) {
            std::string str(deviceName);

            if (str == savedDeviceName_str) {
                X_OUTPUT::xprint(MSG_STYLE::REQU, "§", deviceName);
                selectedDeviceName = savedDeviceName_str.c_str();
            }
            else
                X_OUTPUT::xprint(MSG_STYLE::REQU, std::to_string(i), deviceName);
        } else {
            X_OUTPUT::xprint(MSG_STYLE::REQU, std::to_string(i), lang(T_MSG::UNAMED_AUDIO_DEVICE));
        }
    }

    if (numAudioDevices) {
        //if (!selectedDeviceName) {
        if (!selectedDeviceName) {
            int audioIndex = TOOLBOX::selectById(numAudioDevices);
            /*const char**/ selectedDeviceName = SDL_GetAudioDeviceName(audioIndex, isCapture);
        }

        if (!selectedDeviceName || std::strcmp(selectedDeviceName, "") == 0) {
            X_OUTPUT::xprint(MSG_STYLE::M_ERROR, lang(T_MSG::INVALID_AUDIO_DEVICE));
            return;
        }

        //std::cout << selectedInputDeviceName << std::endl << selectedOutputDeviceName << std::endl;

        // Spécifications du format audio souhaité
        SDL_AudioSpec desiredSpec;
        SDL_zero(desiredSpec);             // Initialiser à zéro
        desiredSpec.freq = __sample_rate;           // Fréquence d'échantillonnage
        desiredSpec.format = AUDIO_S16SYS;     // Format de l'échantillon (32 bits float)
        desiredSpec.channels = 1;           // Nombre de canaux (stéréo)
        desiredSpec.samples = 4096;         // Taille de la mémoire tampon
        desiredSpec.callback = __audio_callback;//__audio_callback;

        // Structure pour stocker les spécifications obtenues
        SDL_AudioSpec obtainedSpec;

        // Ouvrir le périphérique audio
        if (deviceType == AUDIO::Device::AUDIO_INPUT) {
            __input_device_id = SDL_OpenAudioDevice(selectedDeviceName, SDL_TRUE, &desiredSpec, &obtainedSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
            //__output_device_id = Mix_OpenAudioDevice(44100, MIX_DEFAULT_FORMAT, 2, 4096, selectedOutputDeviceName,SDL_AUDIO_ALLOW_FORMAT_CHANGE);

            if (__input_device_id == 0 ) {
                X_OUTPUT::xprint(MSG_STYLE::M_ERROR, lang(T_MSG::AUDIO_DEVICE_ERROR) + ": " + std::string(SDL_GetError()));
            } else {
                AUDIO::__input_device_name = std::string(selectedDeviceName);
            }
        } else {
                AUDIO::__output_device_name = std::string(selectedDeviceName);
        }
    }
}

void AUDIO::startRecording() {
    __audio_stream.clear();
    __is_recording = true;
    SDL_PauseAudioDevice(__input_device_id, 0);
}

void AUDIO::stopRecording() {
    SDL_PauseAudioDevice(__input_device_id, 1);
    __is_recording = false;
    auto silence = static_cast<size_t>(__sample_rate * 2);
    __audio_stream.insert(__audio_stream.end(), silence, 0);
}

void AUDIO::play(Type type) {
    switch(type) {
    case Type::RADIOSTART:
        __play_radiostart();
        break;

    case Type::RADIOSTOP:
        __play_radiostop();
        break;

    default:
        break;
    }
}

bool AUDIO::isRecording() {
    return __is_recording;
}

const std::vector<unsigned char>& AUDIO::getAudioStream() {
    return __audio_stream;
}

std::vector<float> apply_sox_fx_speech(const std::vector<float>& pcm_in, int sr) {
    std::vector<float> pcm_out;
    if (pcm_in.empty()) return pcm_out;

    if (sox_init() != SOX_SUCCESS) {
        std::cerr << "sox_init failed\n";
        return pcm_out;
    }

    // Signal: infos "physiques" (pas d'encodage ici)
    sox_signalinfo_t sig{};
    sig.rate      = (sox_rate_t)sr;      // ex. 22050
    sig.channels  = 1;                   // mono
    sig.precision = 32;                  // bits par échantillon
    sig.length    = pcm_in.size();       // nb d'échantillons (mono)
    // sig.encoding : n'existe pas -> normal

    // Encodage: float32
    sox_encodinginfo_t enc{};
    enc.encoding         = SOX_ENCODING_FLOAT;
    enc.bits_per_sample  = 32;
    enc.compression      = 0;

    // Input mémoire (float32 LE)
    sox_format_t* in = sox_open_mem_read(
        (void*)pcm_in.data(),
        pcm_in.size() * sizeof(float),
        &sig, &enc, "f32"    // type "f32" = float32 little-endian brut
    );
    if (!in) { sox_quit(); return pcm_out; }

    // Output mémoire (on sur-alloue puis on retaillera après)
    size_t out_cap_bytes = pcm_in.size() * sizeof(float) * 2; // marge
    pcm_out.resize(out_cap_bytes / sizeof(float));
    sox_format_t* out = sox_open_mem_write(
        (void*)pcm_out.data(),
        out_cap_bytes,
        &sig, &enc, "f32", nullptr
    );
    if (!out) {
        sox_close(in);
        sox_quit();
        pcm_out.clear();
        return pcm_out;
    }

    // Chaîne d'effets
    sox_effects_chain_t* chain =
        sox_create_effects_chain(&in->encoding, &out->encoding);

    auto add_effect = [&](const char* name, int argc, const char* argv[]) -> bool {
        sox_effect_t* e = sox_create_effect(sox_find_effect(name));
        if (!e) return false;
        if (sox_effect_options(e, argc, const_cast<char**>(argv)) != SOX_SUCCESS) {
            free(e); return false;
        }
        if (sox_add_effect(chain, e, &in->signal, &out->signal) != SOX_SUCCESS) {
            free(e); return false;
        }
        free(e);
        return true;
    };

    // input
    {
        sox_effect_t* e = sox_create_effect(sox_find_effect("input"));
        char* args[] = { (char*)in };
        if (sox_effect_options(e, 1, args) != SOX_SUCCESS ||
            sox_add_effect(chain, e, &in->signal, &out->signal) != SOX_SUCCESS) {
            free(e);
            sox_delete_effects_chain(chain);
            sox_close(in); sox_close(out); sox_quit();
            pcm_out.clear(); return pcm_out;
        }
        free(e);
    }

    // === Ta chaîne d’effets ===
    const char* sinc_args[]      = { "300-3000" };
    const char* gain_args[]      = { "-10" };
    const char* overdrive_args[] = { "5" };
    const char* highpass_args[]  = { "200" };
    const char* lowpass_args[]   = { "3000" };
    const char* contrast_args[]  = { "50" };

    if (!add_effect("sinc", 1, sinc_args) ||
        !add_effect("gain", 1, gain_args) ||
        !add_effect("overdrive", 1, overdrive_args) ||
        !add_effect("highpass", 1, highpass_args) ||
        !add_effect("lowpass", 1, lowpass_args) ||
        !add_effect("contrast", 1, contrast_args)) {
        sox_delete_effects_chain(chain);
        sox_close(in); sox_close(out); sox_quit();
        pcm_out.clear(); return pcm_out;
    }

    // output
    {
        sox_effect_t* e = sox_create_effect(sox_find_effect("output"));
        char* args[] = { (char*)out };
        if (sox_effect_options(e, 1, args) != SOX_SUCCESS ||
            sox_add_effect(chain, e, &in->signal, &out->signal) != SOX_SUCCESS) {
            free(e);
            sox_delete_effects_chain(chain);
            sox_close(in); sox_close(out); sox_quit();
            pcm_out.clear(); return pcm_out;
        }
        free(e);
    }

    // Traiter
    if (sox_flow_effects(chain, nullptr, nullptr) != SOX_SUCCESS) {
        sox_delete_effects_chain(chain);
        sox_close(in); sox_close(out); sox_quit();
        pcm_out.clear(); return pcm_out;
    }

    // Retailler à la quantité réellement écrite (octets -> échantillons)
    size_t written_bytes = out->olength;           // octets écrits en mémoire
    size_t samples       = written_bytes / sizeof(float);
    pcm_out.resize(samples);

    // Nettoyage
    sox_delete_effects_chain(chain);
    sox_close(in);
    sox_close(out);
    sox_quit();
    return pcm_out;
}


void AUDIO::play(const Speaker::Controller& ctrl, const std::string& str) {
    piper_synthesize_start(ctrl.synth, str.c_str(), &ctrl.synth_cfg);
    piper_audio_chunk chunk;
    std::vector<float> pcm;
    while (piper_synthesize_next(ctrl.synth, &chunk) != PIPER_DONE) {
        const float* f = static_cast<const float*>(chunk.samples);
        pcm.insert(pcm.end(), f, f + chunk.num_samples);
    }
    auto pcm_fx = apply_sox_fx_speech(pcm, 22050);
    if (pcm_fx.empty()) {
        std::cerr << "SoX processing failed.\n";
        return;
    }
    std::vector<uint8_t> out_bytes;
    const uint8_t* p = reinterpret_cast<const uint8_t*>(pcm_fx.data());
    out_bytes.assign(p, p + pcm.size() * sizeof(float));
    Mix_Chunk chunk_mix{};
    chunk_mix.allocated = 0;
    chunk_mix.abuf = out_bytes.data();
    chunk_mix.alen = static_cast<Uint32>(out_bytes.size());
    chunk_mix.volume = MIX_MAX_VOLUME;
    int channel = Mix_PlayChannel(CHANNEL_RADIO_COM, &chunk_mix, 0);
    while (Mix_Playing(channel) != 0) {
        SDL_Delay(10);
    }
}

void AUDIO::openSelectedDevice(int frequency) {
    Mix_CloseAudio();
    
    if (Mix_OpenAudioDevice(frequency, AUDIO_F32SYS, 1, 1024, AUDIO::__output_device_name.c_str(), 0) < 0) {
        X_OUTPUT::xprint(MSG_STYLE::M_ERROR, lang(T_MSG::AUDIO_DEVICE_ERROR) + ": " + std::string(Mix_GetError()));
        SESSION::no_audio = true;
    } else {
        Mix_AllocateChannels(32);
        X_OUTPUT::xprint(MSG_STYLE::INFO, lang(T_MSG::AUDIO_DEVICE_FREQ) + ": " + std::to_string(frequency) + "Hz");
    }
}