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
#include <string>

class X_INPUT {
private:
//#ifdef PLATFORM_LINUX
    static struct termios   __old_term;
    static struct termios   __new_term;
//#endif // PLATFORM_LINUX
    static int              __prev_ptt_state;
    static std::string      __garbage;
    static int              __ptt_button;
    static SDL_Joystick*    __joystick;
    static bool             __reset;
    static std::string      __input_device_name;
    static std::string      __input_device_button;


    X_INPUT() = default;

    static bool __isUtf8Character(const std::string& str);
    static bool __isLastCharacterASCII(const std::string& str);

public:
    // --- KEYBOARD ---
    static char kbhit(bool in_xscan = false);
    static bool xscan(std::string &str, void (*callback)() = nullptr, int delay = 100);
    static bool xscan();
    static void xscan_anykey();
    static void enableInput();
    static void disableInput();
    static void reset();

    // --- PUSH-TO-TALK ---
    static bool ptt_changed();
    static bool ptt_pushed();

    // --- JOYSTICK ---
    static void selectJoystick();
    static const std::string& getJoystickName();
    static const std::string& getJoystickPTTButton();
};