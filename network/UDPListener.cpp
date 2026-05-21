#include "UDPListener.hpp"
#include "Session.hpp"
#include "SessionManager.hpp"
#include "room/VoiceRouter.hpp"
#include "protocol/Packet.hpp"
#include "utils/Logger.hpp"

UDPListener::UDPListener(asio::io_context& ioContext, uint16_t port)
	:_socket(ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), port))
{
	LOG_INFO("[UDPLISTENER] LISTENING ON PORT {}", port);
}

void UDPListener::start() {
	receive();
}

void UDPListener::receive() {
	_socket.async_receive_from(
		asio::buffer(_buffer), _senderEndpoint,
		[this](std::error_code ec, std::size_t bytesReceived) {
			if (!ec) {
				if (bytesReceived > MAX_PACKET_SIZE) {
					LOG_WARN("[UDPLISTENER] PACKET TOO LARGE: {} BYTES, DISCARDED", bytesReceived);
					receive();
					return;
				}

				if (bytesReceived < VOICE_HEADER_SIZE) {
					LOG_WARN("[UDPLISTENER] PACKET TOO SMALL: {} BYTES, DISCARDED", bytesReceived);
					receive();
					return;
				}

				VoiceHeader header;
				if (!parseVoiceHeader(_buffer.data(), bytesReceived, header)) {
					receive();
					return;
				}

				std::array<uint8_t, 32> token;
				std::memcpy(token.data(), header.sessionToken, 32);

				auto session = SessionManager::getInstance().findByToken(token);
				if (!session) {
					LOG_WARN("[UDPListener] INVALID TOKEN, PACKET DISCARDED");
					receive();
					return;
				}

				VoiceRouter::getInstance().route(
					_socket,
					_buffer.data(),
					bytesReceived,
					session->getUserId(),
					header.channelId
				);
			}
			else {
				LOG_ERROR("[UDPLISTENER] RECEIVE ERROR: {}", ec.message());
			}

			receive();
		}
	);
}