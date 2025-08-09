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

// ******************************************************
// * STD INCLUDES
// ******************************************************

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <set>
#include <stdexcept>
#include <cmath>
#include <string>
#include <optional>
#include <stop_token>
#include <thread>
#include <functional>
#include <compare>
#include <string_view>
#include <random>
#include <regex>
#include <algorithm>
#include <sstream>
#include <memory>
#include <mutex>
#include <filesystem>
#include <ostream>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <tuple>
#include <typeinfo>
#include <any>
#include <cstdint>
#include <cstring>
#include <atomic>
#include <csignal>
#include "third_party/cereal/archives/binary.hpp"
#include <cstdio>
#include <stdio.h>
#include <math.h>
#include <bits/chrono.h>
#include <exception>



#ifdef PLATFORM_LINUX
#include <termios.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <utf8cpp/utf8.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <vosk_api.h>
#include <zmq.hpp>
#include <curl/curl.h>
#include <system.h>
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#endif

#ifdef PLATFORM_WINDOWS
#include "third_party/nlohmann/json.hpp"
#include "third_party/nlohmann/json_fwd.hpp"
#include "third_party/utf8/utf8.h"
#include "third_party/SDL2/SDL.h"
#include "third_party/SDL2/SDL_audio.h"
#include "third_party/SDL2/SDL_mixer.h"
#include "third_party/SDL2/SDL_keyboard.h"
#include "third_party/SDL2/SDL_events.h"
#include "third_party/SDL2/SDL_keycode.h"
#include "third_party/vosk_api/vosk_api.h"
#include "third_party/cppzmq/zmq.hpp"
#include "third_party/curl/curl/curl.h"
#include <windows.h>
#include <psapi.h>
#endif