//Network.hpp

#pragma once
#include <enet/enet.h>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <memory>
#include "object/ref_counted/RefCounted.hpp"
#include "object/Object.hpp"

class Network {
public:
    static Network& get() {
        static Network instance;
        return instance;
    }

    bool init();
    void shutdown();

    bool start_server(int port, int max_clients = 32);
    bool connect(const std::string& ip, int port);

    void update(float dt);

    void send_rpc(const std::string& func_name, const nlohmann::json& args, 
                  int target_object_id = -1, 
                  int target_peer_id = -1, 
                  bool reliable = true);
    
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
