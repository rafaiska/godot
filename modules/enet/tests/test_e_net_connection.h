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

#include "core/crypto/crypto.h"
#include "core/error/error_list.h"
#include "core/io/ip_address.h"
#include "core/io/packet_peer_udp.h"
#include "core/io/udp_server.h"
#include "modules/enet/enet_connection.h"
#include "modules/enet/enet_packet_peer.h"
#include "tests/test_macros.h"
#include "tests/test_utils.h"
#include <enet/enet.h>
#include <enet/enet_godot.h>

#define TEST_SERVER_PORT 61125
#define TEST_SERVER_HOST "127.0.0.1"

namespace TestENetConnection {

TEST_CASE("[ENetConnection] Create host") {
	Ref<ENetConnection> enet_connection;
	enet_connection.instantiate();
	Error error = enet_connection->create_host();
	CHECK_EQ(Error::OK, error);

	ENetConnection::Event event;
	ENetConnection::EventType type = enet_connection->service(0, event);
	CHECK_EQ(ENetConnection::EventType::EVENT_NONE, type);
}

TEST_CASE("[ENetConnection] Create host bound") {
	Ref<ENetConnection> enet_connection_bound;
	enet_connection_bound.instantiate();
	Error error = enet_connection_bound->create_host_bound(IPAddress(TEST_SERVER_HOST), TEST_SERVER_PORT);
	CHECK_EQ(Error::OK, error);
	CHECK_EQ(TEST_SERVER_PORT, enet_connection_bound->get_local_port());

	ENetConnection::Event event;
	ENetConnection::EventType type = enet_connection_bound->service(0, event);
	CHECK_EQ(ENetConnection::EventType::EVENT_NONE, type);
}

TEST_CASE("[ENetConnection] Socket send") {
	Ref<ENetConnection> enet_connection;
	Ref<UDPServer> server;
	Ref<PacketPeerUDP> packet;
	const uint8_t *buffer;
	const unsigned char data[] = { 0x20, 0x21, '\0' };
	PackedByteArray array = { 0x20, 0x21 };
	int buffer_size = 0;

	enet_connection.instantiate();
	server.instantiate();

	enet_connection->create_host();
	server->listen(TEST_SERVER_PORT);

	enet_connection->socket_send(TEST_SERVER_HOST, TEST_SERVER_PORT, array);
	server->poll();
	CHECK(server->is_connection_available());

	packet = server->take_connection();
	packet->get_packet(&buffer, buffer_size);

	CHECK_NE(nullptr, buffer);
	CHECK_EQ(2, buffer_size);
	CHECK_EQ(0, strcmp((char *)data, (char *)buffer));
}

TEST_CASE("[ENetConnection] Bandwidth limit") {
	Ref<ENetConnection> connection;
	ENetConnection::Event event;
	ENetConnection::EventType event_type;

	connection.instantiate();
	Error error = connection->create_host();
	CHECK_EQ(Error::OK, error);

	connection->bandwidth_limit(100, 80);
	connection->bandwidth_throttle();
	event_type = connection->service(0, event);
	CHECK_EQ(ENetConnection::EventType::EVENT_NONE, event_type);
}

TEST_CASE("[ENetConnection] Broadcast") {
	Ref<ENetConnection> enet_connection;
	ENetConnection::Event event;
	ENetConnection::EventType event_type;
	ENetPacket *packet = enet_packet_create("TEST", 32, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);

	enet_connection.instantiate();
	enet_connection->create_host();
	enet_connection->broadcast(0, packet);
	event_type = enet_connection->service(0, event);
	CHECK_EQ(ENetConnection::EventType::EVENT_NONE, event_type);
}

TEST_CASE("[ENetConnection] Channel limit") {
	Ref<ENetConnection> enet_connection;
	enet_connection.instantiate();

	enet_connection->create_host(8, 8);
	CHECK_EQ(8, enet_connection->get_max_channels());

	enet_connection->channel_limit(12);
	CHECK_EQ(12, enet_connection->get_max_channels());
}

TEST_CASE("[ENetConnection] Compress") {
	Ref<ENetConnection> enet_connection;
	ENetConnection::Event event;
	ENetConnection::EventType event_type;
	ENetPacket *packet = enet_packet_create("TEST", 32, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);

	enet_connection.instantiate();
	enet_connection->create_host();

	enet_connection->compress(ENetConnection::COMPRESS_NONE);
	enet_connection->broadcast(0, packet);
	event_type = enet_connection->service(0, event);
	CHECK_EQ(ENetConnection::EventType::EVENT_NONE, event_type);

	enet_connection->compress(ENetConnection::COMPRESS_FASTLZ);
	enet_connection->broadcast(0, packet);
	event_type = enet_connection->service(0, event);
	CHECK_EQ(ENetConnection::EventType::EVENT_NONE, event_type);

	enet_connection->compress(ENetConnection::COMPRESS_RANGE_CODER);
	enet_connection->broadcast(0, packet);
	event_type = enet_connection->service(0, event);
	CHECK_EQ(ENetConnection::EventType::EVENT_NONE, event_type);

	enet_connection->compress(ENetConnection::COMPRESS_ZLIB);
	enet_connection->broadcast(0, packet);
	event_type = enet_connection->service(0, event);
	CHECK_EQ(ENetConnection::EventType::EVENT_NONE, event_type);

	enet_connection->compress(ENetConnection::COMPRESS_ZSTD);
	enet_connection->broadcast(0, packet);
	event_type = enet_connection->service(0, event);
	CHECK_EQ(ENetConnection::EventType::EVENT_NONE, event_type);
}

TEST_CASE("[ENetConnection] Destroy") {
	Ref<ENetConnection> enet_connection;
	enet_connection.instantiate();
	enet_connection->create_host();

	enet_connection->destroy();
	Error error = enet_connection->create_host();
	CHECK_EQ(Error::OK, error);
}

TEST_CASE("[ENetConnection] TLS Server") {
	Ref<ENetConnection> enet_connection_server;
	enet_connection_server.instantiate();

	Ref<CryptoKey> crypto_key = Ref<CryptoKey>(CryptoKey::create());
	crypto_key->load(TestUtils::get_data_path("crypto/in.key"), false);

	Ref<TLSOptions> tls_options = TLSOptions::server(crypto_key, X509Certificate::create());

	enet_connection_server->create_host();
	Error error = enet_connection_server->dtls_server_setup(tls_options);
	CHECK_EQ(Error::OK, error);
}

TEST_CASE("[ENetConnection] TLS Client") {
	Ref<ENetConnection> enet_connection_client;
	enet_connection_client.instantiate();
	Ref<TLSOptions> tls_options = TLSOptions::client(X509Certificate::create());

	enet_connection_client->create_host();
	Error error = enet_connection_client->dtls_client_setup("localhost", tls_options);
	CHECK_EQ(Error::OK, error);
}

} // namespace TestENetConnection
