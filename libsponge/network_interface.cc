#include "network_interface.hh"

#include "arp_message.hh"
#include "ethernet_frame.hh"

#include <iostream>

// Dummy implementation of a network interface
// Translates from {IP datagram, next hop address} to link-layer frame, and from link-layer frame to IP datagram

// For Lab 5, please replace with a real implementation that passes the
// automated checks run by `make check_lab5`.

// You will need to add private members to the class declaration in `network_interface.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface(const EthernetAddress &ethernet_address, const Address &ip_address)
    : _ethernet_address(ethernet_address), _ip_address(ip_address) {
    cerr << "DEBUG: Network interface has Ethernet address " << to_string(_ethernet_address) << " and IP address "
         << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but may also be another host if directly connected to the same network as the destination)
//! (Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) with the Address::ipv4_numeric() method.)
void NetworkInterface::send_datagram(const InternetDatagram &dgram, const Address &next_hop) {
    // convert IP address of next hop to raw 32-bit representation (used in ARP header)
    const uint32_t next_hop_ip = next_hop.ipv4_numeric();

    if(_ip_mac.find(next_hop_ip) != _ip_mac.end()) {
        if(_ip_mac_expiration_times[next_hop_ip] <= _elapsed_time_since_start) {
            auto [it, inserted] = _unk_dgrams.try_emplace(next_hop_ip); // C++17
            it->second.push_back(dgram);
            _ip_mac_expiration_times.erase(next_hop_ip);
            EthernetFrame ethframe{};
            EthernetHeader &ethheader = ethframe.header();
            ethheader.src = _ethernet_address;
            ethheader.dst = ETHERNET_BROADCAST;
            ethheader.type = EthernetHeader::TYPE_ARP;
            ARPMessage arpmsg{};
            arpmsg.opcode = ARPMessage::OPCODE_REQUEST;
            arpmsg.target_ethernet_address = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};;
            arpmsg.target_ip_address = next_hop_ip;
            arpmsg.sender_ip_address = _ip_address.ipv4_numeric();
            arpmsg.sender_ethernet_address = _ethernet_address;
            ethframe.payload() = arpmsg.serialize();
            // cout << "ip " << Address::from_ipv4_numeric(arpmsg.target_ip_address).to_string() << endl;
            // cout << "ethframe.payload.size = " << ethframe.payload().size() << endl;
            _frames_out.push(ethframe);
            return;
        }
        EthernetFrame ethframe{};
        EthernetHeader &ethheader = ethframe.header();
        ethheader.src = _ethernet_address;
        ethheader.dst = _ip_mac[next_hop_ip];
        ethheader.type = EthernetHeader::TYPE_IPv4;
        ethframe.payload() = dgram.serialize();
        _frames_out.push(ethframe);
    } else {
        auto [it, inserted] = _unk_dgrams.try_emplace(next_hop_ip); // C++17
        it->second.push_back(dgram);
        if(_waiting_arp_response_ip_addr.find(next_hop_ip) != _waiting_arp_response_ip_addr.end()) {
            if(_waiting_arp_response_ip_addr[next_hop_ip] + 5000 > _elapsed_time_since_start) {
                return;
            }
        }
        _waiting_arp_response_ip_addr.insert_or_assign(next_hop_ip, _elapsed_time_since_start);
        EthernetFrame ethframe{};
        EthernetHeader &ethheader = ethframe.header();
        ethheader.src = _ethernet_address;
        ethheader.dst = ETHERNET_BROADCAST;
        ethheader.type = EthernetHeader::TYPE_ARP;
        ARPMessage arpmsg{};
        arpmsg.opcode = ARPMessage::OPCODE_REQUEST;
        arpmsg.target_ethernet_address = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};;
        arpmsg.target_ip_address = next_hop_ip;
        arpmsg.sender_ip_address = _ip_address.ipv4_numeric();
        arpmsg.sender_ethernet_address = _ethernet_address;
        ethframe.payload() = arpmsg.serialize();
        // cout << "ip " << Address::from_ipv4_numeric(arpmsg.target_ip_address).to_string() << endl;
        // cout << "ethframe.payload.size = " << ethframe.payload().size() << endl;
        _frames_out.push(ethframe);
    }
}

//! \param[in] frame the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame(const EthernetFrame &frame) {
    // DUMMY_CODE(frame);
    EthernetHeader header{};
    NetParser netparser(Buffer(frame.header().serialize()));
    if(header.parse(netparser) != ParseResult::NoError) {
        return nullopt;
    }
    cout << header.to_string() << endl;
    if(header.type == EthernetHeader::TYPE_IPv4) {
        InternetDatagram datagram{};
        if(datagram.parse(Buffer(frame.payload().concatenate())) != ParseResult::NoError) {
            return nullopt;
        } else if(header.dst == _ethernet_address) {
            return datagram;
        } else {
            return nullopt;
        }
    } else if(header.type == EthernetHeader::TYPE_ARP) {
        ARPMessage arpmsg{};
        if(arpmsg.parse(Buffer(frame.payload().concatenate())) != ParseResult::NoError) {
            return nullopt;
        } 
        // cout << "arpmsg = " << arpmsg.to_string() << endl;
        _ip_mac_expiration_times.insert_or_assign(arpmsg.sender_ip_address, _elapsed_time_since_start + _elapsed_time);
        _ip_mac.insert_or_assign(arpmsg.sender_ip_address, arpmsg.sender_ethernet_address);
        if(arpmsg.opcode == ARPMessage::OPCODE_REPLY) {
            // cout << "ip " << Address::from_ipv4_numeric(arpmsg.sender_ip_address).to_string() << endl;
            if(_unk_dgrams.find(arpmsg.sender_ip_address) != _unk_dgrams.end()) {
                for(auto dgram : _unk_dgrams[arpmsg.sender_ip_address]) {
                    EthernetFrame ethframe{};
                    EthernetHeader &ethheader = ethframe.header();
                    ethheader.src = _ethernet_address;
                    ethheader.dst = _ip_mac[arpmsg.sender_ip_address];
                    ethheader.type = EthernetHeader::TYPE_IPv4;
                    ethframe.payload().append(Buffer(dgram.serialize().concatenate()));
                    _frames_out.push(ethframe);
                }
                _unk_dgrams.erase(arpmsg.sender_ip_address);
            }
        } else if(arpmsg.opcode == ARPMessage::OPCODE_REQUEST && arpmsg.target_ip_address == _ip_address.ipv4_numeric()) {
            arpmsg.opcode = ARPMessage::OPCODE_REPLY;
            arpmsg.target_ethernet_address = arpmsg.sender_ethernet_address;
            arpmsg.target_ip_address = arpmsg.sender_ip_address;
            arpmsg.sender_ip_address = _ip_address.ipv4_numeric();
            arpmsg.sender_ethernet_address = _ethernet_address;
            EthernetFrame ethframe{};
            EthernetHeader &ethheader = ethframe.header();
            ethheader.src = arpmsg.sender_ethernet_address;
            ethheader.dst = arpmsg.target_ethernet_address;
            ethheader.type = EthernetHeader::TYPE_ARP;
            Buffer buffer(arpmsg.serialize());
            ethframe.payload().append(buffer);
            _frames_out.push(ethframe);
        }
    }
    // cout << "_ip_mac_expiration_times size = " << _ip_mac_expiration_times.size() << endl;
    // cout << "_ip_mac size = " << _ip_mac.size() << endl;
    return nullopt;
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick(const size_t ms_since_last_tick) {
    _elapsed_time_since_start += ms_since_last_tick;
}
