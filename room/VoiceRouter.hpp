#pragma once

#include <asio.hpp>
#include <cstdint>

class VoiceRouter {
public:
	static VoiceRouter& getInstance() {
		static VoiceRouter instance;
		return instance;
	}

	void route(
		asio::ip::udp::socket& socket,
		const uint8_t* data,
		std::size_t size,
		int64_t senderUserId,
		uint64_t channelId
	);

private:
	VoiceRouter() = default;
	VoiceRouter(const VoiceRouter&) = delete;
	VoiceRouter& operator=(const VoiceRouter&) = delete;
};