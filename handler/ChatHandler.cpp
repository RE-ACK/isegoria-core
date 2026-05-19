#include "ChatHandler.hpp"
#include "network/Session.hpp"
#include "room/RoomManager.hpp"
#include "protocol/PacketTypes.hpp"
#include "utils/Logger.hpp"
#include "infra/SpringClient.hpp"

void ChatHandler::handleJoin(std::shared_ptr<Session> session, const nlohmann::json& json) {
    if (!json.contains("channelId")) return;

    uint64_t channelId = json["channelId"].get<uint64_t>();
    session->setTextChannelId(channelId);
    RoomManager::getInstance().joinTextChannel(session->getUserId(), channelId);

    LOG_INFO("ENTER TEXT CHANNEL userId={}, channelId={}", session->getUserId(), channelId);
}

void ChatHandler::handleLeave(std::shared_ptr<Session> session, const nlohmann::json& json) {
    uint64_t channelId = session->getTextChannelId();
    if (channelId == 0) return;

    RoomManager::getInstance().leaveTextChannel(session->getUserId(), channelId);
    session->setTextChannelId(0);

    LOG_INFO("LEAVE TEXT CHANNEL userId={}, channelId={}", session->getUserId(), channelId);
}

void ChatHandler::handleMessage(std::shared_ptr<Session> session, const nlohmann::json& json) {
    if (!json.contains("content")) return;

    std::string content = json["content"].get<std::string>();

    // 4KB 초과 시 세션 강제 종료
    if (content.size() > MAX_MESSAGE_SIZE) {
        LOG_ERROR("RECEIVED MESSAGES EXCEEDING 4KB : userId={}", session->getUserId());
        session->sendPacket({
            {"type", "ERROR"},
            {"message", "Message too large"}
            });
        return;
    }

    uint64_t channelId = session->getTextChannelId();
    if (channelId == 0) return;

    nlohmann::json broadcast;
    broadcast["type"] = PacketTypes::CHAT_MSG;
    broadcast["userId"] = session->getUserId();
    broadcast["channelId"] = channelId;
    broadcast["content"] = content;

    auto members = RoomManager::getInstance().getTextChannelMembers(channelId);
    for (auto& member : members) {
        member->sendPacket(broadcast);
    }


    // SpringClient로 메시지 저장 콜백
    SpringClient::getInstance().postMessage(channelId, session->getUserId(), content);

    LOG_INFO("MESSAGE BROADCAST : userId={}, channelId={}", session->getUserId(), channelId);
}