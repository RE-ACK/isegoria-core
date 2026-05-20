#pragma once

#include <memory>
#include <nlohmann/json.hpp>

class Session;

class StatusHandler {
public:
	static void handleStatus(std::shared_ptr<Session> session, const nlohmann::json& json);
	static void handlePing(std::shared_ptr<Session> session, const nlohmann::json& json);
	static void handleSubscribe(std::shared_ptr<Session> session, const nlohmann::json& json);
};