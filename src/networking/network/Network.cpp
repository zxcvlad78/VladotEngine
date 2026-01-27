// Network.cpp

#include "Network.hpp"
#include <iostream>
#include <cstdint>

bool Network::init() {
    return enet_initialize() == 0;
}

void Network::shutdown() {
    if (m_host) enet_host_destroy(m_host);
    enet_deinitialize();
}

bool Network::start_server(int port, int max_clients) {
    ENetAddress addr{ ENET_HOST_ANY, (uint16_t)port };
    m_host = enet_host_create(&addr, max_clients, 2, 0, 0);
    if (m_host) m_my_peer_id = 0;
    return m_host != nullptr;
}

bool Network::connect(const std::string& ip, int port) {
    m_host = enet_host_create(nullptr, 1, 2, 0, 0);
    if (!m_host) return false;

    ENetAddress addr;
    enet_address_set_host(&addr, ip.c_str());
    addr.port = port;

    m_server_peer = enet_host_connect(m_host, &addr, 2, 0);
    if (!m_server_peer) { enet_host_destroy(m_host); m_host = nullptr; return false; }

    m_my_peer_id = -1;
    return true;
}

int Network::assign_new_peer_id(ENetPeer* peer) {
    int id = m_next_peer_id++;
    m_peers[id] = peer;
    peer->data = (void*)(intptr_t)id;
    return id;
}

ENetPeer* Network::get_peer_by_id(int id) {
    auto it = m_peers.find(id);
    return it != m_peers.end() ? it->second : nullptr;
}

void Network::update(float dt) {
    if (!m_host) return;
    ENetEvent event;
    while (enet_host_service(m_host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                if (!m_server_peer) {
                    int id = assign_new_peer_id(event.peer);
                    std::cout << "[Network] Peer ID " << id << " connected.\n";
                    nlohmann::json args{{"peer_id", id}};
                    send_rpc("on_peer_connected", args, -1, id, true);
                } else {
                    std::cout << "[Network] Connected to server.\n";
                }
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                handle_packet(event.packet, event.peer);
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                if (!m_server_peer) {
                    m_peers.erase((int)(intptr_t)event.peer->data);
                }
                std::cout << "[Network] Peer disconnected.\n";
                break;
        }
    }
}

void Network::send_rpc(const std::string& func_name, const nlohmann::json& args,
                       int target_object_id, int target_peer_id, bool reliable) {
    if (!m_host) return;

    nlohmann::json j{{"f", func_name}, {"a", args}, {"id", target_object_id}};
    std::vector<uint8_t> data = nlohmann::json::to_cbor(j);
    ENetPacket* packet = enet_packet_create(data.data(), data.size(),
        reliable ? ENET_PACKET_FLAG_RELIABLE : 0);

    if (target_peer_id == -1) {
        if (m_server_peer) enet_peer_send(m_server_peer, 0, packet);
        else enet_host_broadcast(m_host, 0, packet);
    } else {
        ENetPeer* peer = get_peer_by_id(target_peer_id);
        if (peer) enet_peer_send(peer, 0, packet);
    }
    enet_host_flush(m_host);
}

void Network::handle_packet(ENetPacket* packet, ENetPeer* sender) {
    try {
        m_last_sender_id = (int)(intptr_t)sender->data;
        if (packet->dataLength == 0) { m_last_sender_id = 0; return; }

        // Попробуем как CBOR (RPC)
        nlohmann::json j = nlohmann::json::from_cbor(packet->data, packet->data + packet->dataLength);
        if (j.contains("f") && j.contains("a") && j.contains("id") && m_rpc_handler) {
            m_rpc_handler(j["f"], j["a"], j["id"], m_last_sender_id);
        }
        m_last_sender_id = 0;
    }
    catch (...) {
        // Не CBOR — возможно, бинарный пакет обрабатывается вне через шаблон
    }
}

int Network::generate_next_object_id() { return m_next_object_id++; }

void Network::register_object(Ref<Object> obj) {
    if (obj && obj->network_id != -1) {
        m_objects[obj->network_id] = obj;
    }
}

Ref<Object> Network::get_object_by_id(int id) {
    auto it = m_objects.find(id);
    return it != m_objects.end() ? it->second : nullptr;
}

void Network::unregister_object(int id) {
    m_objects.erase(id);
}
