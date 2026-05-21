#pragma once

#include <asio.hpp>
#include <memory>

class TCPListener {
public:
    TCPListener(asio::io_context& ioContext, uint16_t port);
    void start();

private:
    void accept();

    asio::io_context& _ioContext;
    asio::ip::tcp::acceptor _acceptor;
};