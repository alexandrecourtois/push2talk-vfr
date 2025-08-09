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

#include <cstring>
#include <msg.h>
#include <audio.h>
#include <string>
#include <tools.h>
#include <xprint.h>
#include <session.h>
#include <radio.h>
#include <callbacks.h>
#include <lang.h>

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

const std::string& AUDIO::getInputDeviceName() {
    return __input_device_name;
}

const std::string& AUDIO::getOutputDeviceName() {
    return __output_device_name;
}

void AUDIO::__init_phrs(const std::string& phrasePath) {
    X_OUTPUT::xprint(MSG_STYLE::INIT, lang(T_MSG::LOADING_PHRASES), "phrases.cfg");

    std::ifstream file(phrasePath);

    if (!file.is_open()) {
        throw std::runtime_error(lang(T_MSG::UNABLE_TO_OPEN_FILE));
    }

    std::string line;
    while (std::getline(file, line)) {
        // Supprimer les espaces en début et fin de ligne
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        // Ignorer les lignes vides ou les commentaires
        if (line.empty() || line[0] == '#') continue;

        // Trouver la position du caractère '='
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            throw std::runtime_error(lang(T_MSG::MISFORMATTED_LINE) + line);
        }

        // Séparer la clé (à gauche de '=') et la valeur (à droite de '=')
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        // Supprimer les espaces autour de la clé et de la valeur
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        // Ajouter dans la map
        __phrases[key] = value;
    }

    X_OUTPUT::xprint(MSG_STYLE::DONE);
}

void AUDIO::__load_audio(const std::string& audioPath) {
    // Étape 1 : Compter le nombre total de fichiers .wav
    int totalFiles = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(audioPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".wav") {
            totalFiles++;
        }
    }

    // Vérifie s'il y a des fichiers à charger
    if (totalFiles == 0) {
        X_OUTPUT::xprint(MSG_STYLE::M_ERROR, lang(T_MSG::NO_WAV_FILES_IN_DIRECTORY));
        exit(1);
    }

    // Étape 2 : Charger les fichiers et afficher la barre de progression
    int loadedFiles = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(audioPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".wav") {
            std::string filename = entry.path().stem().string() + entry.path().extension().string();
            Mix_Chunk* chunk = Mix_LoadWAV(entry.path().string().c_str());
            if (chunk != nullptr) {
                __WAV_files[filename] = chunk;
            } else {
                X_OUTPUT::xprint(MSG_STYLE::M_ERROR, lang(T_MSG::LOADING_ERROR) + " ", entry.path().string());
            }

            // Mise à jour de la barre de progression
            loadedFiles++;
            TOOLBOX::displayProgressBar(lang(T_MSG::LOADING_AUDIO_FILES), loadedFiles, totalFiles);
        }
    }

    // Terminer la barre de progression à 100 %
    TOOLBOX::displayProgressBar(lang(T_MSG::LOADING_AUDIO_FILES), totalFiles, totalFiles);
    std::cout << CLEAN_LINE;
    X_OUTPUT::xprint(MSG_STYLE::INIT, lang(T_MSG::LOADING_AUDIO_FILES));
    X_OUTPUT::xprint(MSG_STYLE::DONE, std::to_string(totalFiles) + " " + lang(T_MSG::FILES));
}

void AUDIO::__play_tailnum() {
    std::vector<std::string> tailnum_files;

    for(unsigned int i = 0; i < SESSION::aircraft.tailnum.size(); ++i) {
        if ((SESSION::aircraft.tailnum[i] >= 'A') && (SESSION::aircraft.tailnum[i] <= 'Z')) {
            tailnum_files.push_back(std::string(1, std::tolower(SESSION::aircraft.tailnum[i])) + ".wav");
        }

        if (i == 0)
            i = SESSION::aircraft.tailnum.size() - 3;
    }

    //__play(tailnum_files, CHANNEL_RADIO_COM);
    __play(__concat_audio(tailnum_files, true, -30000), CHANNEL_RADIO_COM);
}

int AUDIO::__play(const std::string &filename, int channel, bool loop) {
    if (__WAV_files.find(filename) != __WAV_files.end()) {
        Mix_Chunk* sound = __WAV_files[filename];

        if (int ch = Mix_PlayChannel(channel, sound, loop ? -1 : 0) == -1) {
            X_OUTPUT::xprint(MSG_STYLE::M_ERROR, lang(T_MSG::UNABLE_TO_PLAY) + " " + filename);
        } else {
            if (!loop)
                while(int p = Mix_Playing(channel) > 0) {
                    SDL_Delay(100);
                }

            return ch;
        }
    }

    return -1;
}

void AUDIO::__play(const std::vector<std::string>& fileNames, int channel) {
    for (/*const auto&*/ std::string fileName : fileNames) {
        auto it = __WAV_files.find(fileName);
        if (it != __WAV_files.end()) {
            __play(fileName, channel);
        } else {
            //std::cerr << "Fichier non trouvé dans la map: " << fileName << std::endl;
            X_OUTPUT::xprint(MSG_STYLE::WARNING, lang(T_MSG::MISSING_FILE), fileName);
        }
    }
}

int AUDIO::__play(Mix_Chunk* src, int channel, bool loop) {
    int ch = Mix_PlayChannel(channel, src, loop ? -1 : 0);

    if (!loop)
        while(int p = Mix_Playing(channel) > 0) {
            SDL_Delay(100);
        }

    return ch;
}

void AUDIO::__play_silence(unsigned int msec) {
    SDL_Delay(msec);
}

Mix_Chunk* AUDIO::__concat_audio(const std::vector<std::string>& audio, bool isFromFiles, int trim) {
    Uint32 length = 0;
    std::vector<std::string> waves;

    if (!isFromFiles) {
        for(std::string item: audio) {
            std::vector<std::string> v = __phrase_to_waves(item);
            waves.insert(waves.end(), v.begin(), v.end());
        }
    } else {
        waves = audio;
    }

    for(std::string file: waves) {
        auto it = __WAV_files.find(file);

        if (it != __WAV_files.end()) {
            length += __WAV_files[file]->alen + trim;
        }
    }

    std::vector<Uint8> buffer(length);
    length = 0;

    for(std::string file: waves) {
        std::memcpy(buffer.data() + length, __WAV_files[file]->abuf, __WAV_files[file]->alen + trim);
        length += __WAV_files[file]->alen + trim;
    }

    Mix_Chunk* chk = new Mix_Chunk;
    
    chk->allocated = 1;
    chk->abuf = new Uint8[buffer.size()];
    chk->alen = buffer.size();
    chk->volume = MIX_MAX_VOLUME;

    std::memcpy(chk->abuf, buffer.data(), buffer.size());

    return chk;
}

void AUDIO::__play_radiocom(const std::vector<std::string> &filenames) {
    int noise_volume = RADIO::getNoiseVolume();

    Mix_Volume(CHANNEL_RADIO_NOISE, noise_volume);
    Mix_Volume(CHANNEL_RADIO_COM, RADIO::getSpeakVolume(noise_volume));
    __play("bruit.wav", CHANNEL_RADIO_NOISE, true);
    __play_tailnum();
    __play_silence(100);

    //for(int i = 0; i < filenames.size(); ++i) 
    //    __play(__phrase_to_waves(filenames[i]), CHANNEL_RADIO_COM);
    __play(__concat_audio(filenames, false), CHANNEL_RADIO_COM);

    __play("fin.wav", CHANNEL_RADIO_COM);
    Mix_HaltChannel(CHANNEL_RADIO_NOISE);
}

void AUDIO::__play_radiostart() {
    __play("debut.wav", CHANNEL_RADIO_REC);
    __play("fond.wav", CHANNEL_RADIO_REC, true);
}

void AUDIO::__play_radiostop() {
    Mix_HaltChannel(CHANNEL_RADIO_REC);
    __play("fin.wav", CHANNEL_RADIO_REC);
}

std::vector<std::string> AUDIO::__phrase_to_waves(const std::string& str) {
    std::vector<std::string> subphrases = TOOLBOX::splitString(__phrases[str], '|');
    std::string phrase;
    std::vector<std::string> words;
    std::vector<std::string> waves;

    if (subphrases.size() > 1) {
        int retained_index = TOOLBOX::generateRandomNumber(subphrases.size() - 1);
        phrase = subphrases[retained_index];
    } else
        phrase = subphrases[0];

    words = TOOLBOX::splitString(phrase, '&');

    for(unsigned int i = 0; i < words.size(); ++i) {
        if (words[i][0] == '[' && words[i][words[i].size() - 1] == ']') {
            std::vector<std::string> tmp_waves = __phrase_to_waves(words[i]);
            waves.insert(waves.end(), tmp_waves.begin(), tmp_waves.end());
        } else {
            std::string value = TOOLBOX::extractBetweenBraces(words[i]);

            if (!value.empty()) {
                std::vector<std::string> filename = TOOLBOX::splitString(words[i], '.');
                words[i] = TOOLBOX::removeSubstring(filename[0], "{" + value + "}") + CALLBACKS::getValue(CALLBACKS::AUDIO, value) + "." + filename[1];
            }

            waves.push_back(words[i]);
        }
    }

    return waves;
}

void AUDIO::__audio_callback(void* userdata, Uint8* stream, int len) {
    if (isRecording()) {
        __audio_stream.insert(__audio_stream.end(), stream, stream + len);
    }
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
    }

    selectDevice(AUDIO::Device::AUDIO_INPUT);
    selectDevice(AUDIO::Device::AUDIO_OUTPUT);

    __load_audio(audioPath);
    __init_phrs(phrasePath);
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
        desiredSpec.callback = __audio_callback;

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
            Mix_CloseAudio();
            __output_device_id = Mix_OpenAudioDevice(48000, MIX_DEFAULT_FORMAT, 2, 2048, selectedDeviceName,SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_FORMAT_CHANGE);
            Mix_AllocateChannels(32);

            if (__output_device_id < 0 ) {
                X_OUTPUT::xprint(MSG_STYLE::M_ERROR, lang(T_MSG::AUDIO_DEVICE_ERROR) + ": " + std::string(Mix_GetError()));
            } else {
                AUDIO::__output_device_name = std::string(selectedDeviceName);
            }
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

void AUDIO::play(Type type, const std::vector<std::string>& files) {
    switch(type) {
    case Type::RADIOCOM:
        __play_radiocom(files);
        break;

    case Type::RADIOSTART:
        __play_radiostart();
        break;

    case Type::RADIOSTOP:
        __play_radiostop();
        break;

    case Type::TAILNUM:
        __play_tailnum();
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

void AUDIO::free() {
    for(auto it = __WAV_files.begin(); it != __WAV_files.end(); ++it)
        Mix_FreeChunk(it->second);

    __WAV_files.clear();
}