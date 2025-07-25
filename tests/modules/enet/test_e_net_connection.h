/**************************************************************************/
/*  test_e_net_connection.h                                               */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include "core/io/ip_address.h"
#include "modules/enet/enet_connection.h"
#include "tests/test_macros.h"

namespace TestENetConnection {

TEST_CASE("[ENetConnection] Create host") {
    Ref<ENetConnection> enet_connection;
    enet_connection.instantiate();
    enet_connection->create_host();

    CHECK_EQ(0.0, enet_connection->pop_statistic(ENetConnection::HOST_TOTAL_RECEIVED_DATA));
    CHECK_EQ(0.0, enet_connection->pop_statistic(ENetConnection::HOST_TOTAL_RECEIVED_PACKETS));
    CHECK_EQ(0.0, enet_connection->pop_statistic(ENetConnection::HOST_TOTAL_SENT_DATA));
    CHECK_EQ(0.0, enet_connection->pop_statistic(ENetConnection::HOST_TOTAL_SENT_PACKETS));
}

TEST_CASE("[ENetConnection] Create host bound") {
    Ref<ENetConnection> enet_connection_bound;
    enet_connection_bound.instantiate();
    enet_connection_bound->create_host_bound(IPAddress("127.0.0.1"), 6660);

    CHECK_EQ(0.0, enet_connection_bound->pop_statistic(ENetConnection::HOST_TOTAL_RECEIVED_DATA));
    CHECK_EQ(0.0, enet_connection_bound->pop_statistic(ENetConnection::HOST_TOTAL_RECEIVED_PACKETS));
    CHECK_EQ(0.0, enet_connection_bound->pop_statistic(ENetConnection::HOST_TOTAL_SENT_DATA));
    CHECK_EQ(0.0, enet_connection_bound->pop_statistic(ENetConnection::HOST_TOTAL_SENT_PACKETS));
}

} // namespace TestENetConnection
