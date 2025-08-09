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

#include "msg.h"
#include <en_US.h>

en_US::en_US(): Lang("en_US") {
    setString(T_MSG::GET_HELP,					"Type \"cmd_help\" to get help");
    setString(T_MSG::FAKE_SERVER_ENABLED, 		"FAKE SERVER ENABLED OVERRIDE IP ADDRESS");
    setString(T_MSG::DISABLE, 					"Disable");
    setString(T_MSG::GOODBYE, 					"Good bye !");
    setString(T_MSG::CONNECTING_TO_XPLANE, 		"Connecting to X-Plane");
    setString(T_MSG::WAITING_FOR_AIRCRAFT, 		"Waiting for aircraft");
    setString(T_MSG::DONE, 						"done");
    setString(T_MSG::LANGUAGE_SET_TO, 			"Language set to");
    setString(T_MSG::LOADING_AIRPORTS, 			"Loading JSON airports");
    setString(T_MSG::LOADING_DIALOG, 			"Loading JSON dialog tree");
    setString(T_MSG::CREATING_SERVER, 			"Creating server");
    setString(T_MSG::CREATING_CLIENT,			"Creating client");
    setString(T_MSG::AIRPORTS_FOUND,				"airports found");
    setString(T_MSG::INITIALIZING_VOSK,			"Initializing Vosk");
    setString(T_MSG::LISTING_AVAILABLE_DEVICES,	"Listing available devices");
    setString(T_MSG::NO_DEVICE_FOUND, 			"No device found");
    setString(T_MSG::UNKNOWN_NAME, 				"Unknown name");
    setString(T_MSG::UNABLE_TO_OPEN_JOYSTICK,	"Unable to open joystick");
    setString(T_MSG::SELECTING_DEVICE, 			"Selecting device");
    setString(T_MSG::PRESS_ANY_BUTTON_ON_DEVICE, "Press any button on device");
    setString(T_MSG::LOADING_AUDIO_FILES, 		"Loading audio files");
    setString(T_MSG::LOADING_PHRASES, 			"Loading phrase file");
    setString(T_MSG::UNABLE_TO_OPEN_FILE, 		"Unable to open file");
    setString(T_MSG::MISFORMATTED_LINE, 			"Misformatted line");
    setString(T_MSG::NO_WAV_FILES_IN_DIRECTORY, 	"No WAV files in directory");
    setString(T_MSG::LOADING_ERROR, 				"Error loading");
    setString(T_MSG::ANALYZING, 					"Analyzing");
    setString(T_MSG::RECORDING,					"Recording... Press Enter to stop");
    setString(T_MSG::ACTIVE,                               "ACTIVE");
    setString(T_MSG::INACTIVE,                             "INACTIVE");
    setString(T_MSG::NO_CTRL_DEFINED,                      "No active controller");
    setString(T_MSG::CONNECTION_LOST,                      "Server disconnected");
    setString(T_MSG::REBOOT_IN,                               "Reboot in :");
    setString(T_MSG::LOADING_CONFIG,                       "Loading config");
    setString(T_MSG::WRITING_CONFIG,                       "Writing config");
    setString(T_MSG::SELECT_AUDIO_INPUT,                   "Select input audio device:");
    setString(T_MSG::SELECT_AUDIO_OUTPUT,                  "Select output audio device:");
}
