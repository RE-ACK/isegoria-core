#include "VoiceRouter.hpp"
#include "RoomManager.hpp"
#include "network/Session.hpp"
#include "utils/Logger.hpp"

void VoiceRouter::route(
	asio::ip::udp::socket& socket,
	const uint8_t* data,
	std::size_t size,
	int64_t senderUserId,
	uint64_t channelId
) {
	auto members = RoomManager::getInstance().getVoiceChannelMembers(channelId);

	for (auto& session : members) {
		// 본인 제외
		if (session->getUserId() == senderUserId)
			continue;
		// 헤드셋 음소거 제외
		if (session->isDeafened())
			continue;

		auto endpoint = session->getUdpEndpoint();

		// endpoint 미등록 세션 제외
		if (endpoint.port() == 0)
			continue;

		socket.async_send_to(
			asio::buffer(data, size), endpoint,
			[](std::error_code ec, std::size_t) {
				if (ec) {
					LOG_WARN("UDP SEND FAIL : {}", ec.message());
				}
			}
		);
	}
}