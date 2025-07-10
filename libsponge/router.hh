#ifndef SPONGE_LIBSPONGE_ROUTER_HH
#define SPONGE_LIBSPONGE_ROUTER_HH

#include "network_interface.hh"

#include <optional>
#include <queue>
#include <memory>
#include <iostream>
#include <string>

class TrieNode {
public:
    std::unique_ptr<TrieNode> child[2];  // 子节点，0 和 1
    bool is_end = false;                 // 是否为合法路径结尾

    TrieNode() = default;
};

class Trie {
private:
    std::unique_ptr<TrieNode> root;

public:
    Trie() : root(std::make_unique<TrieNode>()) {}

    // 插入一串二进制 bool 序列（如 {1,0,1,1}）
    void insert(const std::vector<bool> &bits) {
        TrieNode *node = root.get();
        for (bool bit : bits) {
            int b = bit ? 1 : 0;
            if (!node->child[b]) {
                node->child[b] = std::make_unique<TrieNode>();
            }
            node = node->child[b].get();
        }
        node->is_end = true;
    }

    // 查找一串二进制序列是否存在
    bool search(const std::vector<bool> &bits) const {
        const TrieNode *node = root.get();
        for (bool bit : bits) {
            int b = bit ? 1 : 0;
            if (!node->child[b]) {
                return false;
            }
            node = node->child[b].get();
        }
        return node->is_end;
    }

    // 前缀匹配（例如用于最长前缀匹配）
    bool starts_with(const std::vector<bool> &prefix) const {
        const TrieNode *node = root.get();
        for (bool bit : prefix) {
            int b = bit ? 1 : 0;
            if (!node->child[b]) {
                return false;
            }
            node = node->child[b].get();
        }
        return true;
    }
};


//! \brief A wrapper for NetworkInterface that makes the host-side
//! interface asynchronous: instead of returning received datagrams
//! immediately (from the `recv_frame` method), it stores them for
//! later retrieval. Otherwise, behaves identically to the underlying
//! implementation of NetworkInterface.
class AsyncNetworkInterface : public NetworkInterface {
    std::queue<InternetDatagram> _datagrams_out{};

  public:
    using NetworkInterface::NetworkInterface;

    //! Construct from a NetworkInterface
    AsyncNetworkInterface(NetworkInterface &&interface) : NetworkInterface(interface) {}

    //! \brief Receives and Ethernet frame and responds appropriately.

    //! - If type is IPv4, pushes to the `datagrams_out` queue for later retrieval by the owner.
    //! - If type is ARP request, learn a mapping from the "sender" fields, and send an ARP reply.
    //! - If type is ARP reply, learn a mapping from the "target" fields.
    //!
    //! \param[in] frame the incoming Ethernet frame
    void recv_frame(const EthernetFrame &frame) {
        auto optional_dgram = NetworkInterface::recv_frame(frame);
        if (optional_dgram.has_value()) {
            _datagrams_out.push(std::move(optional_dgram.value()));
        }
    };

    //! Access queue of Internet datagrams that have been received
    std::queue<InternetDatagram> &datagrams_out() { return _datagrams_out; }
};

class Route_item {
    public:
        Route_item(uint32_t prefix, uint8_t length, const std::optional<Address>& hop, size_t interface)
            : route_prefix(prefix),
            prefix_length(length),
            next_hop(hop),
            interface_num(interface) {}

        const uint32_t route_prefix;
        const uint8_t prefix_length;
        const std::optional<Address> next_hop;
        const size_t interface_num;
};

//! \brief A router that has multiple network interfaces and
//! performs longest-prefix-match routing between them.
class Router {
    //! The router's collection of network interfaces
    std::vector<AsyncNetworkInterface> _interfaces{};

    //! Send a single datagram from the appropriate outbound interface to the next hop,
    //! as specified by the route with the longest prefix_length that matches the
    //! datagram's destination address.
    void route_one_datagram(InternetDatagram &dgram);

    std::vector<Route_item> _router_table{};

  public:
    //! Add an interface to the router
    //! \param[in] interface an already-constructed network interface
    //! \returns The index of the interface after it has been added to the router
    size_t add_interface(AsyncNetworkInterface &&interface) {
        _interfaces.push_back(std::move(interface));
        return _interfaces.size() - 1;
    }

    //! Access an interface by index
    AsyncNetworkInterface &interface(const size_t N) { return _interfaces.at(N); }

    //! Add a route (a forwarding rule)
    void add_route(const uint32_t route_prefix,
                   const uint8_t prefix_length,
                   const std::optional<Address> next_hop,
                   const size_t interface_num);

    //! Route packets between the interfaces
    void route();
};

#endif  // SPONGE_LIBSPONGE_ROUTER_HH
