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

#include <event_handler.h>
#include "msg.h"
#include <xprint.h>
#include <inputs.h>
#include <session.h>
#include <updater.h>
#include <radio.h>

void UPDATER::init() {
    EVENT_HANDLER::watch<bool>(&SESSION::dataframe.sim_paused, __EVENT_simpaused);
}

void UPDATER::__EVENT_simpaused() {
    if (SESSION::dataframe.sim_paused) {
        if (!SESSION::sim_paused) {
            SESSION::input_enabled = false;
            SESSION::sim_paused = true;
            X_OUTPUT::xprint(MSG_STYLE::INFO, "SIM PAUSED");
            X_OUTPUT::xprint(MSG_STYLE::INVITE);
        }
    }
    else {
        if (SESSION::sim_paused) {
            SESSION::input_enabled = true;
            SESSION::sim_paused = false;
            X_OUTPUT::xprint(MSG_STYLE::INFO, "SIM RUNNING");
            X_OUTPUT::xprint(MSG_STYLE::INVITE);
        }
    }
}

void UPDATER::update() {
    SESSION::dataframe.acquire([&]() { 
        EVENT_HANDLER::pollEvents();
    });
}