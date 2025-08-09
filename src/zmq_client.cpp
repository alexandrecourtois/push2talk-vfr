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
#include "session.h"
#include <zmq_client.h>
#include <xprint.h>
#include <lang.h>

ZMQ_Client::ZMQ_Client(zmq::context_t& context, ServerType type, const Rsp_Dataframe& dataframe): ZMQ_Service(context, ServiceType::CLIENT, type, dataframe) {
    X_OUTPUT::xprint(MSG_STYLE::INIT, lang(T_MSG::CREATING_CLIENT));
    this->getSocket().connect(type.getAddress());
    X_OUTPUT::xprint(MSG_STYLE::DONE, type.getAddress());
}

void ZMQ_Client::__thread_loop(std::stop_token token) {
    while(!token.stop_requested() /*&& /*!__server_down*/ /*!this->__server_down*/) {
        zmq::message_t msg_req(sizeof(Req));
        Req req;
        req.type = Request::GET_DATAFRAME;
        this->sendRequest(req, true);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void ZMQ_Client::sendRequest(const Req& request, bool inClientThread, int timeout) {
    std::scoped_lock<std::mutex> lock(this->__mutex);
    zmq::socket_t* l_socket;

    if (!inClientThread) {
        l_socket = new zmq::socket_t(ZMQ_Service::getContext(), ZMQ_REQ);
        l_socket->connect(this->getTarget().getAddress());
    } else
        l_socket = &this->getSocket();

    if (timeout) {
        l_socket->set(zmq::sockopt::rcvtimeo, 500);   // 500 ms
        l_socket->set(zmq::sockopt::sndtimeo, 500);   // 500 ms
        l_socket->set(zmq::sockopt::linger, 0);       // important pour ne pas bloquer à la destruction
    } else {
        l_socket->set(zmq::sockopt::rcvtimeo, -1);   // 500 ms
        l_socket->set(zmq::sockopt::sndtimeo, -1);   // 500 ms
    }

    zmq::message_t msg_req;
    zmq::message_t msg_rep;

    T_SERIALIZED buffer = serialize<Req>(request);
    msg_req = zmq::message_t(buffer.data(), buffer.size());

    l_socket->send(msg_req, zmq::send_flags::none);

    switch(request.type) {
        case Request::GET_DATAFRAME:
            if (l_socket->recv(msg_rep, zmq::recv_flags::none)) {
                const_cast<Rsp_Dataframe&>(this->getDataframe()).lock();
                T_SERIALIZED buffer(static_cast<T_BYTE*>(msg_rep.data()), static_cast<T_BYTE*>(msg_rep.data()) + msg_rep.size());
                deserialize<Rsp_Dataframe>(const_cast<Rsp_Dataframe&>(this->getDataframe()), buffer);
                this->__server_down = this->getDataframe().server_down;
                const_cast<Rsp_Dataframe&>(this->getDataframe()).unlock();
            }
            break;

        case Request::UPD_AIRPORT:
            (void) l_socket->recv(msg_rep, zmq::recv_flags::none);
            break;

        default:
            break;
    }

    if (!inClientThread)
        delete l_socket;
}