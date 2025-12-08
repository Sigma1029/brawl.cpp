#ifndef LOGIN_H
#define LOGIN_H

#include <asio.hpp>
#include <memory>
#include <vector>
#include <cstdint>

using asio::ip::tcp;

class Login : public std::enable_shared_from_this<Login> {
public:
    Login(tcp::socket sock, asio::io_context& ctx);
    void start();
    void write(const std::vector<uint8_t>& data);
private:
    void do_read();
    void handle_data(const std::vector<uint8_t>& data);
    void handle_packet(const std::vector<uint8_t>& packet);
    
    tcp::socket socket;
    asio::io_context& io_context;
    std::array<uint8_t, 8192> buffer;
};

#endif