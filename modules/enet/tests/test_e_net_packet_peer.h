/**************************************************************************/
/*  test_e_net_packet_peer.h                                               */
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
#include "modules/enet/enet_packet_peer.h"
#include "tests/test_macros.h"
#include <enet/enet.h>
#include <enet/enet_godot.h>
#include <enet/list.h>
#include <enet/types.h>

#define TEST_SERVER_PORT 61125
#define TEST_SERVER_HOST "127.0.0.1"

namespace TestENetPacketPeer {

class _ENetPacketPeerMock : public ENetPacketPeer {
	ENetHost mock_host{};
	ENetPeer *mock_peer;

public:
	_ENetPacketPeerMock(ENetPeer *p_peer) :
			ENetPacketPeer(p_peer) {
		mock_peer = p_peer;
		p_peer->host = &mock_host;

		enet_list_clear(&mock_peer->acknowledgements);
		enet_list_clear(&mock_peer->sentReliableCommands);
		enet_list_clear(&mock_peer->outgoingCommands);
		enet_list_clear(&mock_peer->outgoingSendReliableCommands);
		enet_list_clear(&mock_peer->dispatchedCommands);
	}

	void queue_packet(ENetPacket *p_packet) { _queue_packet(p_packet); }

	void mock_connected() {
		mock_peer->state = ENET_PEER_STATE_CONNECTED;
	}

	void add_outgoing_command() {
		ENetOutgoingCommand *mock_packet_node = new ENetOutgoingCommand();
		mock_packet_node->packet = nullptr;

		enet_list_insert(enet_list_begin(&mock_peer->outgoingCommands), mock_packet_node);
		mock_peer->dispatchList = mock_peer->outgoingCommands.sentinel;
	}

	size_t count_outgoing_commands() {
		return enet_list_size(&mock_peer->outgoingCommands);
	}

	enet_uint32 get_ping_interval() {
		return mock_peer->pingInterval;
	}
};

TEST_CASE("[ENetPacketPeer] Get Channels") {
	Ref<ENetConnection> enet_connection;
	enet_connection.instantiate();
	enet_connection->create_host_bound(IPAddress(TEST_SERVER_HOST), TEST_SERVER_PORT, 1, 2);
	Ref<ENetPacketPeer> peer = enet_connection->connect_to_host(TEST_SERVER_HOST, TEST_SERVER_PORT, 2);
	CHECK_EQ(2, peer->get_channels());
}

TEST_CASE("[ENetPacketPeer] Get Packet Flags") {
	Ref<_ENetPacketPeerMock> enet_packet_peer_m;
	ENetPeer peer;
	enet_uint32 flags = ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT | ENET_PACKET_FLAG_UNSEQUENCED;
	ENetPacket *packet = enet_packet_create("TEST", 32, flags);

	enet_packet_peer_m.instantiate(&peer);
	enet_packet_peer_m->queue_packet(packet);

	CHECK_EQ(flags, enet_packet_peer_m->get_packet_flags());
}

TEST_CASE("[ENetPacketPeer] Get Remote Address and Port") {
	Ref<ENetConnection> enet_connection;
	enet_connection.instantiate();
	enet_connection->create_host_bound(IPAddress(TEST_SERVER_HOST), TEST_SERVER_PORT, 1, 2);
	Ref<ENetPacketPeer> peer = enet_connection->connect_to_host(TEST_SERVER_HOST, TEST_SERVER_PORT, 2);
	CHECK_EQ(IPAddress(TEST_SERVER_HOST), peer->get_remote_address());
	CHECK_EQ(TEST_SERVER_PORT, peer->get_remote_port());
}

TEST_CASE("[ENetPacketPeer] Get State") {
	Ref<ENetConnection> enet_connection;
	enet_connection.instantiate();
	enet_connection->create_host_bound(IPAddress(TEST_SERVER_HOST), TEST_SERVER_PORT, 1, 2);
	Ref<ENetPacketPeer> peer = enet_connection->connect_to_host(TEST_SERVER_HOST, TEST_SERVER_PORT, 2);
	CHECK_EQ(ENetPacketPeer::PeerState::STATE_CONNECTING, peer->get_state());
}

TEST_CASE("[ENetPacketPeer] Get Statistic") {
	Ref<ENetConnection> enet_connection;
	enet_connection.instantiate();
	enet_connection->create_host_bound(IPAddress(TEST_SERVER_HOST), TEST_SERVER_PORT, 1, 2);
	Ref<ENetPacketPeer> peer = enet_connection->connect_to_host(TEST_SERVER_HOST, TEST_SERVER_PORT, 2);
	CHECK_EQ(0, peer->get_statistic(ENetPacketPeer::PeerStatistic::PEER_PACKET_LOSS));
	CHECK_EQ(0, peer->get_statistic(ENetPacketPeer::PeerStatistic::PEER_PACKET_LOSS_VARIANCE));
	CHECK_EQ(0, peer->get_statistic(ENetPacketPeer::PeerStatistic::PEER_PACKET_LOSS_EPOCH));
	CHECK_EQ(500, peer->get_statistic(ENetPacketPeer::PeerStatistic::PEER_ROUND_TRIP_TIME));
	CHECK_EQ(0, peer->get_statistic(ENetPacketPeer::PeerStatistic::PEER_ROUND_TRIP_TIME_VARIANCE));
	CHECK_EQ(500, peer->get_statistic(ENetPacketPeer::PeerStatistic::PEER_LAST_ROUND_TRIP_TIME));
	CHECK_EQ(0, peer->get_statistic(ENetPacketPeer::PeerStatistic::PEER_LAST_ROUND_TRIP_TIME_VARIANCE));
	CHECK_EQ(32, peer->get_statistic(ENetPacketPeer::PeerStatistic::PEER_PACKET_THROTTLE));
	CHECK_EQ(32, peer->get_statistic(ENetPacketPeer::PeerStatistic::PEER_PACKET_THROTTLE_LIMIT));
	CHECK_EQ(0, peer->get_statistic(ENetPacketPeer::PeerStatistic::PEER_PACKET_THROTTLE_COUNTER));
	CHECK_EQ(0, peer->get_statistic(ENetPacketPeer::PeerStatistic::PEER_PACKET_THROTTLE_EPOCH));
	CHECK_EQ(2, peer->get_statistic(ENetPacketPeer::PeerStatistic::PEER_PACKET_THROTTLE_ACCELERATION));
	CHECK_EQ(2, peer->get_statistic(ENetPacketPeer::PeerStatistic::PEER_PACKET_THROTTLE_DECELERATION));
	CHECK_EQ(5000, peer->get_statistic(ENetPacketPeer::PeerStatistic::PEER_PACKET_THROTTLE_INTERVAL));
}

TEST_CASE("[ENetPacketPeer] Is Active") {
	Ref<ENetConnection> enet_connection;
	enet_connection.instantiate();
	enet_connection->create_host_bound(IPAddress(TEST_SERVER_HOST), TEST_SERVER_PORT, 1, 2);
	Ref<ENetPacketPeer> peer = enet_connection->connect_to_host(TEST_SERVER_HOST, TEST_SERVER_PORT, 2);
	CHECK_EQ(true, peer->is_active());
}

TEST_CASE("[ENetPacketPeer] Disconnect") {
	Ref<_ENetPacketPeerMock> enet_packet_peer_m;
	ENetPeer peer;
	enet_packet_peer_m.instantiate(&peer);

	enet_packet_peer_m->mock_connected();
	enet_packet_peer_m->add_outgoing_command();

	enet_packet_peer_m->peer_disconnect_later();
	CHECK_EQ(ENetPacketPeer::PeerState::STATE_DISCONNECT_LATER, enet_packet_peer_m->get_state());

	enet_packet_peer_m->peer_disconnect();
	CHECK_EQ(ENetPacketPeer::PeerState::STATE_DISCONNECTING, enet_packet_peer_m->get_state());

	enet_packet_peer_m->peer_disconnect_now();
	CHECK_EQ(ENetPacketPeer::PeerState::STATE_DISCONNECTED, enet_packet_peer_m->get_state());
}

TEST_CASE("[ENetPacketPeer] Ping") {
	Ref<_ENetPacketPeerMock> enet_packet_peer_m;
	ENetPeer peer;
	enet_packet_peer_m.instantiate(&peer);

	enet_packet_peer_m->ping_interval(1000);
	CHECK_EQ(1000, enet_packet_peer_m->get_ping_interval());

	CHECK_EQ(0, enet_packet_peer_m->count_outgoing_commands());
	enet_packet_peer_m->mock_connected();
	enet_packet_peer_m->ping();
	CHECK_EQ(1, enet_packet_peer_m->count_outgoing_commands());
}

TEST_CASE("[ENetPacketPeer] Reset") {
	Ref<_ENetPacketPeerMock> enet_packet_peer_m;
	ENetPeer peer;
	enet_packet_peer_m.instantiate(&peer);

	enet_packet_peer_m->mock_connected();
	CHECK_EQ(ENetPacketPeer::PeerState::STATE_CONNECTED, enet_packet_peer_m->get_state());

	enet_packet_peer_m->reset();
	CHECK_EQ(ENetPacketPeer::PeerState::STATE_DISCONNECTED, enet_packet_peer_m->get_state());
}

TEST_CASE("[ENetPacketPeer] Send") {
	Ref<ENetConnection> enet_connection;
	ENetPacket *packet = enet_packet_create("TEST", 32, ENET_PACKET_FLAG_RELIABLE);
	enet_connection.instantiate();
	enet_connection->create_host_bound(IPAddress(TEST_SERVER_HOST), TEST_SERVER_PORT, 1, 2);
	Ref<ENetPacketPeer> peer = enet_connection->connect_to_host(TEST_SERVER_HOST, TEST_SERVER_PORT, 2);
	CHECK_EQ(-1, peer->send(0, packet));
}

TEST_CASE("[ENetPacketPeer] Set Timeout") {
	Ref<_ENetPacketPeerMock> enet_packet_peer_m;
	ENetPeer peer;
	enet_packet_peer_m.instantiate(&peer);

	enet_packet_peer_m->set_timeout(100, 200, 300);
	CHECK_EQ(100, peer.timeoutLimit);
	CHECK_EQ(200, peer.timeoutMinimum);
	CHECK_EQ(300, peer.timeoutMaximum);
}

TEST_CASE("[ENetPacketPeer] Set Timeout") {
	Ref<_ENetPacketPeerMock> enet_packet_peer_m;
	ENetPeer peer;
	enet_packet_peer_m.instantiate(&peer);

	enet_packet_peer_m->throttle_configure(16, 4, 2);
	CHECK_EQ(16, peer.packetThrottleInterval);
	CHECK_EQ(4, peer.packetThrottleAcceleration);
	CHECK_EQ(2, peer.packetThrottleDeceleration);
}

} // namespace TestENetPacketPeer
