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

enum class MSG_STYLE {
    DEFAULT,
    INIT,
    DONE,
    REQU,
    M_ERROR,
    WARNING,
    HELP,
    INFO,
    INVITE,
    CLEAN,
    USER,
    USER_ALT,
    XTALK,
    XTALK_ALT,
    BLINK_BEGIN,
    BLINK_END,
    ENDL,
    CMD,
    KEYWORDS,
    STATUS_ON,
    STATUS_OFF
};

enum class T_MSG {
    EMPTY,
    GET_HELP,
    FAKE_SERVER_ENABLED,
    DISABLE,
    GOODBYE,
    CONNECTING_TO_XPLANE,
    WAITING_FOR_AIRCRAFT,
    DONE,
    LANGUAGE_SET_TO,
    LOADING_AIRPORTS,
    LOADING_DIALOG,
    CREATING_SERVER,
    CREATING_CLIENT,
    AIRPORTS_FOUND,
    AIRPORT_FREQUENCY_SELECTED,
    INITIALIZING_VOSK,
    LISTING_AVAILABLE_DEVICES,
    NO_DEVICE_FOUND,
    UNKNOWN_NAME,
    UNABLE_TO_OPEN_JOYSTICK,
    SELECTING_DEVICE,
    PRESS_ANY_BUTTON_ON_DEVICE,
    LOADING_AUDIO_FILES,
    LOADING_PHRASES,
    UNABLE_TO_OPEN_FILE,
    MISFORMATTED_LINE,
    NO_WAV_FILES_IN_DIRECTORY,
    LOADING_ERROR,
    ANALYZING,
    RECORDING,
    SELECT_AUDIO_INPUT,
    SELECT_AUDIO_OUTPUT,
    FILES,
    UNABLE_TO_PLAY,
    MISSING_FILE,
    ERROR_WHILE_INITIALIZING_SDL_AUDIO,
    ERROR_WHILE_ENUMERATING_AUDIO,
    UNAMED_AUDIO_DEVICE,
    INVALID_AUDIO_DEVICE,
    AUDIO_DEVICE_ERROR,
    FREQ,
    XP11_WARNING,
    ACTIVE,
    INACTIVE,
    NO_CTRL_DEFINED,
    CONNECTION_LOST,
    REBOOT_IN,
    LOADING_CONFIG,
    WRITING_CONFIG
};