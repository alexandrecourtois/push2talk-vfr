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
#include <lang.h>

const std::string& lang(T_MSG msg) {
    return Lang::getString(msg);
}

Lang::Lang(const std::string& lname) {
    _lname = lname;
    std::cout << lname << std::flush;
}

Lang::~Lang() {
}

void Lang::setString(T_MSG msg, std::string str) {
    _sentences[msg] = str;
}

std::string& Lang::getString(T_MSG msg) {
    return _sentences[msg];
}

std::string Lang::getLang() {
    return _lname;
}

std::map<T_MSG, std::string> Lang::_sentences = { { T_MSG::EMPTY, "" } };
std::string Lang::_lname = "";
