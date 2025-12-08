#include <asio.hpp>
#include <iostream>
#include "Message/Login.h"
#include "Stream/PiranhaMessage.h"

using asio::ip::tcp;

class Server {
public:
    Server(asio::io_context& io_context, short port)
        : io_context_(io_context),
          acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        start_accept();
    }

private:
    void start_accept() {
        acceptor_.async_accept(
            [this](const asio::error_code& ec, tcp::socket socket) {
                if (!ec) {
                    auto client = std::make_shared<Login>(std::move(socket), io_context_);
                    client->start();
                }
                start_accept();
            });
    }

    asio::io_context& io_context_;
    tcp::acceptor acceptor_;
};

int main() {
    try {
        asio::io_context io_context;
        Server server(io_context, 9339);
        std::cout << "Server started! :3" << std::endl;
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}