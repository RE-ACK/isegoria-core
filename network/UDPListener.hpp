#pragma once

#include <asio.hpp>
#include <array>

class UDPListener {
public:
	UDPListener(asio::io_context& ioContext, uint16_t port);
	void start();

private:
	void receive();

	asio::ip::udp::socket _socket;
	asio::ip::udp::endpoint _senderEndpoint;

	static constexpr size_t MAX_PACKET_SIZE = 256;
	std::array<uint8_t, MAX_PACKET_SIZE> _buffer;
};