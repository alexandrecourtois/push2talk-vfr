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
#include <constraints.h>
#include <msg.h>
#include <xprint.h>

const std::string& lang(T_MSG msg);

class Lang {
protected:
    Lang(const std::string& lname);
    ~Lang();

    static void setString(T_MSG msg, std::string str);

private:
    static std::map<T_MSG, std::string> _sentences;
    static std::string _lname;

public:
    template <typename TLang> static void setLang() {
        Derived_from<TLang, Lang>();
        TLang _lang;
        //X_OUTPUT::xprint(MSG_STYLE::INFO, getString(T_MSG::LANGUAGE_SET_TO),getLang());
        X_OUTPUT::xprint(MSG_STYLE::INFO, lang(T_MSG::LANGUAGE_SET_TO), getLang());
    }

    static std::string& getString(T_MSG msg);
    static std::string getLang();
};