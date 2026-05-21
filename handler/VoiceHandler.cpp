#include "VoiceHandler.hpp"
#include "network/Session.hpp"
#include "network/SessionManager.hpp"
#include "room/RoomManager.hpp"
#include "protocol/PacketTypes.hpp"
#include "utils/Logger.hpp"

void VoiceHandler::handleJoin(std::shared_ptr<Session> session, const nlohmann::json& json) {
	if (!json.contains("channelId") || !json.contains("udpPort"))
		return;

	uint64_t channelId = json["channelId"].get<uint64_t>();
	uint16_t udpPort = json["udpPort"].get<uint16_t>();

	if (!RoomManager::getInstance().joinVoiceChannel(session->getUserId(), channelId)) {
		session->sendPacket({
			{"type", PacketTypes::VOICE_FULL}
		});
		return;
	}

	// TCP IP + 클라이언트가 알려준 udpPort로 UDP 엔드포인트 등록
	auto tcpEndpoint = session->getSocket().remote_endpoint();
	asio::ip::udp::endpoint udpEndpoint(tcpEndpoint.address(), udpPort);
	session->setUdpEndpoint(udpEndpoint);
	session->setVoiceChannelId(channelId);

	auto members = RoomManager::getInstance().getVoiceChannelMembers(channelId);

	// 기존 채널 멤버들의 상태를 신규 입장 세션에게 전달
	for (auto& member : members) {
		if (member->getUserId() == session->getUserId())
			continue;
		nlohmann::json existingState;
		existingState["type"] = PacketTypes::VOICE_STATE;
		existingState["userId"] = member->getUserId();
		existingState["channelId"] = channelId;
		existingState["joined"] = true;
		session->sendPacket(existingState);
	}

	nlohmann::json broadcast;
	broadcast["type"] = PacketTypes::VOICE_STATE;
	broadcast["userId"] = session->getUserId();
	broadcast["channelId"] = channelId;
	broadcast["joined"] = true;

	auto all = SessionManager::getInstance().getAll();
	//for (auto& member : members) {
	for(auto& s : all){
		if(s->isWatching(session->getUserId()))
			s->sendPacket(broadcast);
	}

	LOG_INFO("ENTER VOICE CHANNEL : userId={}, channelId={}", session->getUserId(), channelId);
}

void VoiceHandler::handleLeave(std::shared_ptr<Session> session, const nlohmann::json& json) {
	uint64_t channelId = session->getVoiceChannelId();

	if (channelId == 0)
		return;

	RoomManager::getInstance().leaveVoiceChannel(session->getUserId(), channelId);
	session->setVoiceChannelId(0);

	nlohmann::json broadcast;
	broadcast["type"] = PacketTypes::VOICE_STATE;
	broadcast["userId"] = session->getUserId();
	broadcast["channelId"] = channelId;
	broadcast["joined"] = false;

	//auto members = RoomManager::getInstance().getVoiceChannelMembers(channelId);
	auto all = SessionManager::getInstance().getAll();
	//for (auto& member : members) {
	for(auto& s : all){
		if(s->isWatching(session->getUserId()))
			s->sendPacket(broadcast);
	}

	LOG_INFO("LEAVE VOICE CHANNEL : userId={}, channelId={}", session->getUserId(), channelId);
}

void VoiceHandler::handleVoiceUsersRequest(
	std::shared_ptr<Session> session,
	const nlohmann::json& json)
{
	auto& rm = RoomManager::getInstance();

	auto voiceChannels = rm.getAllVoiceChannels();

	for (auto& [channelId, members] : voiceChannels)
	{
		nlohmann::json packet;
		packet["type"] = PacketTypes::VOICE_USERS;
		packet["channelId"] = channelId;

		nlohmann::json users = nlohmann::json::array();

		for (auto userId : members)
		{
			users.push_back(userId);
		}

		packet["users"] = users;

		session->sendPacket(packet);
	}
}

void VoiceHandler::handleMute(std::shared_ptr<Session> session, const nlohmann::json& json) {
	if (!json.contains("muted"))
		return;
	session->setMuted(json["muted"].get<bool>());
	LOG_INFO("MUTE STATUS CHANGE: userId={}, muted={}", session->getUserId(), session->isMuted());
}

void VoiceHandler::handleDeafen(std::shared_ptr<Session> session, const nlohmann::json& json) {
	if (!json.contains("deafened"))
		return;
	session->setDeafened(json["deafened"].get<bool>());
	LOG_INFO("DEAFEN STATUS CHANGE: userId={}, deafened={}", session->getUserId(), session->isDeafened());
}