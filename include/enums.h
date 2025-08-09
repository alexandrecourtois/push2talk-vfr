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

enum class ServiceType {
    SERVER,
    CLIENT
};

class ServerType {
public:
    enum class Protocol {
        TCP,
        IPC,
        INPROC
    };

private:
    Protocol __protocol;
    std::string __address;

public:
    const std::string& getAddress() const {
        return __address;
    }

    const Protocol& getProtocol() const {
        return __protocol;
    }

    ServerType(const ServerType& type): __protocol(type.__protocol), __address(type.__address) { }

protected:
    ServerType(Protocol protocol, std::string_view address): __protocol(protocol), __address(address) { }
};

class TCP_Server: public ServerType {
public:
    explicit TCP_Server(std::string_view address): ServerType(Protocol::TCP, "tcp://" + std::string(address)) { }
};

class IPC_Server: public ServerType {
public:
    explicit IPC_Server(std::string_view address): ServerType(Protocol::IPC, "ipc:///tmp/" + std::string(address)) { }
};

class INPROC_Server: public ServerType {
public:
    explicit INPROC_Server(std::string_view address): ServerType(Protocol::INPROC, "inproc://" + std::string(address)) { }
};

enum class Request {
    GET_DATAFRAME,
    UPD_AIRPORT,
    UPD_MONITOR
};