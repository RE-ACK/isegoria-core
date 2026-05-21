#pragma once

#include <memory>
#include <nlohmann/json.hpp>

class Session;

class ChatHandler {
public:
	static void handleJoin(std::shared_ptr<Session> session, const nlohmann::json& json);
	static void handleLeave(std::shared_ptr<Session> session, const nlohmann::json& json);
	static void handleMessage(std::shared_ptr<Session> session, const nlohmann::json& json);

private:
	static constexpr size_t MAX_MESSAGE_SIZE = 4096;
};