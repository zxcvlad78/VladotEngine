// Network.hpp

#pragma once
#include <enet/enet.h>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include "object/ref_counted/RefCounted.hpp"
#include "object/Object.hpp"

class Network { 
public:
    static Network& get() {
        static Network instance;
        return instance;
    }

    enum PACKET_TYPE {
        RPC = 1
    };

    bool init();
    void shutdown();

    bool start_server(int port, int max_clients = 32);
    bool connect(const std::string& ip, int port);

    void update(float dt);

    void send_rpc(const std::string& func_name, const nlohmann::json& args,
                  int target_object_id = -1,
                  int target_peer_id = -1,
                  bool reliable = true);

    template <typename T>
    void send_packet(int type, T value, int target_peer_id = -1, bool reliable = true) {
        if (!m_host) return;

        struct Packet {
            int type;
            T data;
        } packet{type, value};

        size_t size = sizeof(Packet);
        ENetPacket* enet_packet = enet_packet_create(&packet, size,
            reliable ? ENET_PACKET_FLAG_RELIABLE : 0);

        if (target_peer_id == -1) {
            if (m_server_peer) enet_peer_send(m_server_peer, 0, enet_packet);
            else enet_host_broadcast(m_host, 0, enet_packet);
        } else {
            ENetPeer* peer = get_peer_by_id(target_peer_id);
            if (peer) enet_peer_send(peer, 0, enet_packet);
            else { enet_packet_destroy(enet_packet); return; }
        }
        enet_host_flush(m_host);
    }

    void set_rpc_handler(std::function<void(std::string, nlohmann::json, int, int)> handler) {
        m_rpc_handler = handler;
    }

    int get_my_peer_id() const { return m_my_peer_id; }
    int get_last_sender_id() const { return m_last_sender_id; }
    int generate_next_object_id();

    void register_object(Ref<Object> obj);
    Ref<Object> get_object_by_id(int id);
    void unregister_object(int id);

private:
    Network() = default;

    ENetHost* m_host = nullptr;
    ENetPeer* m_server_peer = nullptr;
    std::function<void(std::string, nlohmann::json, int, int)> m_rpc_handler;
    std::unordered_map<int, ENetPeer*> m_peers;
    std::unordered_map<int, Ref<Object>> m_objects;
    int m_next_peer_id = 1;
    int m_next_object_id = 1;
    int m_my_peer_id = 0;
    int m_last_sender_id = 0;

    void handle_packet(ENetPacket* packet, ENetPeer* sender);
    int assign_new_peer_id(ENetPeer* peer);
    ENetPeer* get_peer_by_id(int id);
};
