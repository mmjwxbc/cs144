#include "router.hh"

#include <iostream>

using namespace std;

// Dummy implementation of an IP router

// Given an incoming Internet datagram, the router decides
// (1) which interface to send it out on, and
// (2) what next hop address to send it to.

// For Lab 6, please replace with a real implementation that passes the
// automated checks run by `make check_lab6`.

// You will need to add private members to the class declaration in `router.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

//! \param[in] route_prefix The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
//! \param[in] prefix_length For this route to be applicable, how many high-order (most-significant) bits of the route_prefix will need to match the corresponding bits of the datagram's destination address?
//! \param[in] next_hop The IP address of the next hop. Will be empty if the network is directly attached to the router (in which case, the next hop address should be the datagram's final destination).
//! \param[in] interface_num The index of the interface to send the datagram out on.
void Router::add_route(const uint32_t route_prefix,
                       const uint8_t prefix_length,
                       const optional<Address> next_hop,
                       const size_t interface_num) {
    // cerr << "DEBUG: adding route " << Address::from_ipv4_numeric(route_prefix).ip() << "/" << int(prefix_length)
        //  << " => " << (next_hop.has_value() ? next_hop->ip() : "(direct)") << " on interface " << interface_num << "\n";

    // DUMMY_CODE(route_prefix, prefix_length, next_hop, interface_num);
    // Your code here.
    
    _router_table.emplace_back(route_prefix, prefix_length, next_hop, interface_num);
}


uint32_t generate_subnet_mask(const uint8_t prefix_len) {
    if (prefix_len > 32) {
        return 0;
    }

    if (prefix_len == 0) {
        return 0;
    }
    return ~((1U << (32 - prefix_len)) - 1);
}

//! \param[in] dgram The datagram to be routed
// void Router::route_one_datagram(InternetDatagram &dgram) {
//     // DUMMY_CODE(dgram);
//     // Your code here.
//     if (dgram.header().ttl <= 1) {
//         return; // Drop the datagram
//     }
//     if(!_router_table.empty()) {
//         return;
//     }

//     dgram.header().ttl--;
//     uint32_t ip = dgram.header().dst;
//     size_t best_item_idx = 0;
//     uint8_t best_len = 0;
//     for(size_t i = 0; i < _router_table.size(); i++) {
//         auto item = _router_table[i];
//         uint32_t sub_mask = generate_subnet_mask(item.prefix_length);
//         if((item.route_prefix & sub_mask) == (ip & sub_mask) && item.prefix_length >= best_len) {
//             best_item_idx = i;
//             best_len = item.prefix_length;
//         }
//     }
//     if(best_len != 0) {
//         auto item = _router_table[best_item_idx];
//         _interfaces[item.interface_num].send_datagram(dgram, item.next_hop);
//     }
// }
void Router::route_one_datagram(InternetDatagram &dgram) {
    if (dgram.header().ttl <= 1) {
        return;  // drop
    }

    dgram.header().ttl--;

    uint32_t dst_ip = dgram.header().dst;

    const Route_item* best_match = nullptr;
    uint8_t longest_prefix = 0;

    for (const auto &entry : _router_table) {
        uint32_t mask = generate_subnet_mask(entry.prefix_length);
        if ((dst_ip & mask) == (entry.route_prefix & mask)) {
            if (entry.prefix_length >= longest_prefix) {
                best_match = &entry;
                longest_prefix = entry.prefix_length;
            }
        }
    }

    if (best_match) {
        Address next_hop = best_match->next_hop.value_or(Address::from_ipv4_numeric(dst_ip));
        _interfaces[best_match->interface_num].send_datagram(dgram, next_hop);
    }
}


void Router::route() {
    // Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
    for (auto &interface : _interfaces) {
        auto &queue = interface.datagrams_out();
        while (not queue.empty()) {
            route_one_datagram(queue.front());
            queue.pop();
        }
    }
}
