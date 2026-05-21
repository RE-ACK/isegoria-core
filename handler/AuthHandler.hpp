#pragma once

#include <memory>
#include<nlohmann/json.hpp>

class Session;

class AuthHandler {
public:
	static void handle(std::shared_ptr<Session> session, const nlohmann::json& json);
private:
	static std::array<uint8_t, 32> generateSessionToken();
	static void broadcastOnline(std::shared_ptr<Session> session);
};