#include "TCPListener.hpp"
#include "Session.hpp"
#include "SessionManager.hpp"
#include "utils/Logger.hpp"

TCPListener::TCPListener(asio::io_context& ioContext, uint16_t port)
    : _ioContext(ioContext)
    , _acceptor(ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
{
    LOG_INFO("[TCPListener] LISTENING ON PORT {}", port);
}

void TCPListener::start() {
    accept();
}

void TCPListener::accept() {
    _acceptor.async_accept(
        [this](std::error_code ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                auto remoteEp = socket.remote_endpoint();
                LOG_INFO("[TCPLISTENER] NEW CONNECTION: {}:{}",
                    remoteEp.address().to_string(), remoteEp.port());

                auto session = std::make_shared<Session>(std::move(socket));
                session->start();
            }
            else {
                LOG_ERROR("[TCPLISTENER] ACCEPT ERROR: {}", ec.message());
            }
            accept();
        }
    );
}