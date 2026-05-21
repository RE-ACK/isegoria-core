#include "StatusHandler.hpp"
#include "network/Session.hpp"
#include "network/SessionManager.hpp"
#include "protocol/PacketTypes.hpp"
#include "utils/Logger.hpp"

void StatusHandler::handleStatus(std::shared_ptr<Session> session, const nlohmann::json& json) {
    if (!json.contains("status")) return;

    std::string status = json["status"].get<std::string>();

    // TODO: SpringClient로 유저가 속한 서버 목록 조회 후 해당 멤버들에게만 브로드캐스트
    // 현재는 전체 온라인 유저에게 브로드캐스트
    nlohmann::json broadcast;
    broadcast["type"] = PacketTypes::USER_STATE;
    broadcast["userId"] = session->getUserId();
    broadcast["status"] = status;

    auto all = SessionManager::getInstance().getAll();
    for (auto& s : all) {
        if (s->getUserId() == session->getUserId()) continue;
        if (s->isWatching(session->getUserId())) {
            s->sendPacket(broadcast);
        }
    }

    LOG_INFO("유저 상태 변경: userId={}, status={}", session->getUserId(), status);
}

void StatusHandler::handlePing(std::shared_ptr<Session> session, const nlohmann::json& json) {
    session->updateLastPingAt();
    session->sendPacket({ {"type", PacketTypes::PONG} });
}

void StatusHandler::handleSubscribe(std::shared_ptr<Session> session, const nlohmann::json& json) {
    if (!json.contains("userIds") || !json["userIds"].is_array()) return;

    session->clearWatchedUsers();
    for (auto& id_json : json["userIds"]) {
        if (id_json.is_number()) {
            session->addWatchedUser(id_json.get<int64_t>());
        }
    }

    nlohmann::json response;
    response["type"] = "SUBSCRIBE_STATUS_OK";

    std::vector<int64_t> onlineWatched;
    auto all = SessionManager::getInstance().getAll();
    for (auto& s : all) {
        if (session->isWatching(s->getUserId())) {
            onlineWatched.push_back(s->getUserId());
        }
    }
    response["onlineUserIds"] = onlineWatched;
    session->sendPacket(response);
}