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

#include <msg.h>
#include <en_US.h>

en_US::en_US(): Lang("en_US") {
    setString(T_MSG::GET_HELP,						        "Type \"cmd_help\" for help");
    setString(T_MSG::FAKE_SERVER_ENABLED,			        "SANDBOX MODE");
    setString(T_MSG::DISABLE,						        "Disable");
    setString(T_MSG::GOODBYE,						        "Goodbye!");
    setString(T_MSG::CONNECTING_TO_XPLANE, 		        "Connecting to X-Plane");
    setString(T_MSG::WAITING_FOR_AIRCRAFT, 		        "Detecting aircraft");
    setString(T_MSG::DONE, 						        "done");
    setString(T_MSG::LANGUAGE_SET_TO, 				        "Language selected");
    setString(T_MSG::LOADING_AIRPORTS, 			        "Loading airports");
    setString(T_MSG::LOADING_DIALOG, 				        "Loading dialogs");
    setString(T_MSG::CREATING_SERVER, 				        "Starting server");
    setString(T_MSG::CREATING_CLIENT, 				        "Starting client");
    setString(T_MSG::AIRPORTS_FOUND, 				        "airports found");
    setString(T_MSG::INITIALIZING_VOSK,			        "Initializing Vosk");
    setString(T_MSG::LISTING_AVAILABLE_DEVICES,	        "Searching for compatible devices");
    setString(T_MSG::NO_DEVICE_FOUND, 				        "No device found");
    setString(T_MSG::UNKNOWN_NAME, 				        "Unknown name");
    setString(T_MSG::UNABLE_TO_OPEN_JOYSTICK,		        "Unable to open joystick");
    setString(T_MSG::SELECTING_DEVICE, 			        "Select a joystick");
    setString(T_MSG::PRESS_ANY_BUTTON_ON_DEVICE, 	        "Press a joystick button");
    setString(T_MSG::LOADING_AUDIO_FILES, 		        	"Loading audio files");
    setString(T_MSG::LOADING_PHRASES, 				        "Loading phrases file");
    setString(T_MSG::UNABLE_TO_OPEN_FILE, 			        "Unable to open file");
    setString(T_MSG::MISFORMATTED_LINE, 			        "Malformed line");
    setString(T_MSG::NO_WAV_FILES_IN_DIRECTORY, 	        "No WAV file in the directory");
    setString(T_MSG::LOADING_ERROR, 				        "Error loading");
    setString(T_MSG::ANALYZING, 					        "Analysis");
    setString(T_MSG::RECORDING, 					        "Recording... Press Enter to stop");
    setString(T_MSG::SELECT_AUDIO_INPUT,                   "Select an input audio device:");
    setString(T_MSG::SELECT_AUDIO_OUTPUT,                  "Select an output audio device:");
    setString(T_MSG::FILES,                                "files");
    setString(T_MSG::UNABLE_TO_PLAY,                       "Unable to read");
    setString(T_MSG::MISSING_FILE,                         "Missing file");
    setString(T_MSG::ERROR_WHILE_INITIALIZING_SDL_AUDIO,   "Error initializing SDL_AUDIO");
    setString(T_MSG::ERROR_WHILE_ENUMERATING_AUDIO,        "Error while searching for audio devices");
    setString(T_MSG::UNAMED_AUDIO_DEVICE,                  "Unnamed audio device");
    setString(T_MSG::INVALID_AUDIO_DEVICE,                 "Invalid audio device");
    setString(T_MSG::AUDIO_DEVICE_ERROR,                   "Audio device error");
    setString(T_MSG::FREQ,                                 "Freq");
    setString(T_MSG::XP11_WARNING,                         "THE WEATHER SETTINGS SHOWN MAY NOT MATCH\n  THE SIMULATOR DATA");
    setString(T_MSG::ACTIVE,                               "ACTIVE");
    setString(T_MSG::INACTIVE,                             "INACTIVE");
    setString(T_MSG::NO_CTRL_DEFINED,                      "No controller active");
    setString(T_MSG::CONNECTION_LOST,                      "Server disconnected");
    setString(T_MSG::REBOOT_IN,                            "Restarting in:");
    setString(T_MSG::LOADING_CONFIG,                       "Loading configuration");
    setString(T_MSG::WRITING_CONFIG,                       "Saving configuration");
    setString(T_MSG::ERRORS_FOUND,                         "Errors found");
    setString(T_MSG::INVALID_JSON, "Invalid JSON");
    setString(T_MSG::ROOT_MUST_BE_A_PAIR_OBJECT, "The root must be an object of pairs {state -> definition}.");
    setString(T_MSG::MUST_BE_A_STRING, "must be a string");
    setString(T_MSG::MUST_BE_AN_ARRAY_OF_OBJECTS, "must be an array of objects");
    setString(T_MSG::MISSING_OR_NON_STRING, "missing or not a string");
    setString(T_MSG::REGEX_KEYWORDS_INVALID_BASIC_TEST, "invalid 'keywords' regex (basic test)");
    setString(T_MSG::GOTO_MUST_BE_A_STRING, "'goto' must be a string");
    setString(T_MSG::GOTO_WITH_MISSING_PATH, "'goto' set with $PATH but no 'path' value found elsewhere to validate");
    setString(T_MSG::GOTO_MISSING_STATE, "'goto' points to a missing state");
}
