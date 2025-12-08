#include <asio.hpp>
#include <memory>
#include <vector>
#include <cstdint>
#include <functional>
#include "Login.h"
#include "LoginOk.h"
#include "OwnHomeData.h"

using asio::ip::tcp;

Login::Login(tcp::socket sock, asio::io_context& ctx) 
    : socket(std::move(sock)), io_context(ctx) {}

void Login::start() {
    do_read();
}

void Login::write(const std::vector<uint8_t>& data) {
    auto self = shared_from_this();
    asio::async_write(socket, asio::buffer(data),
        [self](const asio::error_code& ec, size_t) {});
}

void Login::do_read() {
    auto self = shared_from_this();
    socket.async_read_some(asio::buffer(buffer),
        [this, self](const asio::error_code& ec, size_t bytes) {
            if (!ec) {
                std::vector<uint8_t> data(buffer.begin(), buffer.begin() + bytes);
                handle_data(data);
                do_read();
            }
        });
}

void Login::handle_data(const std::vector<uint8_t>& data) {
    static std::vector<uint8_t> packet_buffer;
    static size_t expected_length = 0;
    
    packet_buffer.insert(packet_buffer.end(), data.begin(), data.end());
    
    while (true) {
        if (expected_length > 0) {
            if (packet_buffer.size() >= 7 + expected_length) {
                auto packet = std::vector<uint8_t>(
                    packet_buffer.begin(), 
                    packet_buffer.begin() + 7 + expected_length
                );
                handle_packet(packet);
                
                packet_buffer.erase(
                    packet_buffer.begin(), 
                    packet_buffer.begin() + 7 + expected_length
                );
                expected_length = 0;
            } else {
                break;
            }
        } else if (packet_buffer.size() >= 7) {
            expected_length = (packet_buffer[2] << 16) | 
                             (packet_buffer[3] << 8) | 
                             packet_buffer[4];
            
            if (packet_buffer.size() >= 7 + expected_length) {
                auto packet = std::vector<uint8_t>(
                    packet_buffer.begin(), 
                    packet_buffer.begin() + 7 + expected_length
                );
                handle_packet(packet);
                
                packet_buffer.erase(
                    packet_buffer.begin(), 
                    packet_buffer.begin() + 7 + expected_length
                );
                expected_length = 0;
            } else {
                break;
            }
        } else {
            break;
        }
    }
}

void Login::handle_packet(const std::vector<uint8_t>& packet) {
    if (packet.size() < 7) return;
    
    uint16_t message_id = (packet[0] << 8) | packet[1];
    uint32_t length = (packet[2] << 16) | (packet[3] << 8) | packet[4];
    uint16_t version = (packet[5] << 8) | packet[6];
    
    if (packet.size() < 7 + length) return;
    
    if (message_id == 10101) {
        LoginOk loginOk;
        OwnHomeData ownHomeData;
        
        loginOk.version = version;
        ownHomeData.version = version;
        
        write(loginOk.encodePacket());
        write(ownHomeData.encodePacket());
    }
}