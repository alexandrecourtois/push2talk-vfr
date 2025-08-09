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

class EVENT_HANDLER {
    public:
        enum class Event {
            E_CHANGED,
            E_EQUAL,
            E_GREATER,
            E_LESS,
            E_NOT_EQUAL,
            E_LESS_OR_EQUAL,
            E_GREATER_OR_EQUAL
        };
    
    private:
        static std::map<void*, std::tuple<std::string, std::any, std::function<void()>, Event>> __watched;

        template<typename T> static bool __check(auto& line) {
            if (std::get<0>(line.second) == typeid(T).name()) {
                switch(std::get<3>(line.second)) {
                    case Event::E_CHANGED:
                        if (*static_cast<T*>(line.first) != std::any_cast<T>(std::get<1>(line.second))) {
                            //std::cout << "changed!" << std::endl;
                            std::get<1>(line.second) = *static_cast<T*>(line.first);

                            if (std::get<2>(line.second))
                                std::get<2>(line.second)();

                            return true;
                        }
                        break;

                    case Event::E_EQUAL:
                        if (*static_cast<T*>(line.first) == std::any_cast<T>(std::get<1>(line.second))) {
                            //std::cout << "equal!" << std::endl;

                            if (std::get<2>(line.second))
                                std::get<2>(line.second)();

                            return true;
                        }
                        break;

                    case Event::E_LESS:
                        if (*static_cast<T*>(line.first) < std::any_cast<T>(std::get<1>(line.second))) {
                            //std::cout << "less!" << std::endl;

                            if (std::get<2>(line.second))
                                std::get<2>(line.second)();

                            return true;
                        }
                        break;

                    case Event::E_LESS_OR_EQUAL:
                        if (*static_cast<T*>(line.first) <= std::any_cast<T>(std::get<1>(line.second))) {
                            //std::cout << "less or equal!" << std::endl;

                            if (std::get<2>(line.second))
                                std::get<2>(line.second)();

                            return true;
                        }
                        break;

                    case Event::E_GREATER:
                        if (*static_cast<T*>(line.first) > std::any_cast<T>(std::get<1>(line.second))) {
                            //std::cout << "greater!" << std::endl;

                            if (std::get<2>(line.second))
                                std::get<2>(line.second)();

                            return true;
                        }
                        break;

                    case Event::E_GREATER_OR_EQUAL:
                        if (*static_cast<T*>(line.first) >= std::any_cast<T>(std::get<1>(line.second))) {
                            //std::cout << "greater or equal!" << std::endl;

                            if (std::get<2>(line.second))
                                std::get<2>(line.second)();

                            return true;
                        }
                        break;

                    case Event::E_NOT_EQUAL:
                        if (*static_cast<T*>(line.first) != std::any_cast<T>(std::get<1>(line.second))) {
                            //std::cout << "not equal!" << std::endl;

                            if (std::get<2>(line.second))
                                std::get<2>(line.second)();

                            return true;
                        }
                        break;

                    default:
                        break;
                }
            }

            return false;            
        }

    public:
        static void init() {}

        template<typename T> static void watch(T* watched_ptr, const std::function<void()>& func) {
            watch(watched_ptr, func, Event::E_CHANGED, *watched_ptr/*T{}*/);
        }

        template<typename T> static void watch(T* watched_ptr, const std::function<void()>& func, Event evt, T refvalue) {
            if (watched_ptr) {
                if (__watched.find(watched_ptr) == __watched.end()) {
                    __watched[watched_ptr] = std::tuple<std::string, std::any, std::function<void()>, Event>(typeid(T).name(), refvalue, func, evt);
                }
            }            
        }

        template<typename T> static void release(T* value_ptr) {
            if (value_ptr) {
                if (__watched.find(value_ptr) != __watched.end()) {
                    __watched.erase(value_ptr);
                }
            }
        }

        static void pollEvents();
};