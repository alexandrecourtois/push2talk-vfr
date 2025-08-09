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

#include "reqrsp.h"
#include <zmq_server.h>
#include <xprint.h>
#include <lang.h>

ZMQ_Server::ZMQ_Server(zmq::context_t& context, ServerType type, const Rsp_Dataframe& dataframe): ZMQ_Service(context, ServiceType::SERVER, type, dataframe) {
    X_OUTPUT::xprint(MSG_STYLE::INIT, lang(T_MSG::CREATING_SERVER));
    this->getSocket().bind(type.getAddress());
    X_OUTPUT::xprint(MSG_STYLE::DONE, type.getAddress());
}

void ZMQ_Server::__thread_loop(std::stop_token token) {
    std::vector<zmq::pollitem_t> items(1);

    items[0].socket = this->getSocket();
    items[0].events = ZMQ_POLLIN;

    while(!token.stop_requested()) {
        zmq::poll(&items[0], 1, std::chrono::milliseconds(500));

        if (items[0].revents && ZMQ_POLLIN) {
            zmq::message_t msg_req;//(sizeof(Req));
            if (this->getSocket().recv(msg_req, zmq::recv_flags::none)) {
                T_SERIALIZED buffer(static_cast<T_BYTE*>(msg_req.data()), static_cast<T_BYTE*>(msg_req.data()) + msg_req.size());

                this->__last_request.lock();
                deserialize<Req>(this->__last_request, buffer);
                this->__last_request.unlock();
            }

            zmq::message_t msg_rep;

            this->__last_request.lock();
            if (this->__last_request.type == Request::GET_DATAFRAME) {
                const_cast<Rsp_Dataframe&>(this->getDataframe()).lock();
                T_SERIALIZED buffer = serialize<Rsp_Dataframe>(this->getDataframe());
                msg_rep = zmq::message_t(buffer.data(), buffer.size());
                const_cast<Rsp_Dataframe&>(this->getDataframe()).unlock();
            }

            if (this->__last_request.type == Request::UPD_AIRPORT) {
                Rsp<> rsp;
                rsp.value = RSP_ACK;
                T_SERIALIZED buffer = serialize<Rsp<>>(rsp);
                msg_rep = zmq::message_t(buffer.data(), buffer.size());
            }
            this->__last_request.unlock();

            this->getSocket().send(msg_rep, zmq::send_flags::none);
        }
    }
}

Req ZMQ_Server::getLastRequest() {
    Req request;
    this->__last_request.lock();
    request = Req(this->__last_request);
    this->__last_request.unlock();

    return request;
}