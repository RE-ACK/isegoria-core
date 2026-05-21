#include "AuthHandler.hpp"
#include "network/Session.hpp"
#include "network/SessionManager.hpp"
#include "auth/JwtVerifier.hpp"
#include "protocol/PacketTypes.hpp"
#include "utils/Logger.hpp"

#include <random>
#include <cstdlib>

std::array<uint8_t, 32> AuthHandler::generateSessionToken() {
	std::array<uint8_t, 32> token{};
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<uint16_t> dist(0, 255);
	for (auto& byte : token) {
		byte = static_cast<uint8_t>(dist(gen));
	}

	return token;
}

void AuthHandler::handle(std::shared_ptr<Session> session, const nlohmann::json& json) {
	// JWT 토큰 추출
	if (!json.contains("token")) {
		session->sendPacket({
			{"type", PacketTypes::AUTH_FAIL}
		});
		return;
	}

	std::string token = json["token"].get<std::string>();

	// JWT 검증
	std::string secret = std::getenv("JWT_SECRET") ? std::getenv("JWT_SECRET") : "";
	if (secret.empty()) {
		LOG_ERROR("JWT_SECRET ENVIRONMENT SETTING ERROR");
		session->sendPacket({
			{"type", PacketTypes::AUTH_FAIL}
		});
		return;
	}

	JwtVerifier verifier(secret);
	JwtClaims claims;
	if (!verifier.verify(token, claims)) {
		LOG_WARN("JWT VALIDATION FAILED : INVALID TOKEN");
		session->sendPacket({
			{"type", PacketTypes::AUTH_FAIL}
		});
		return;
	}
	
	// sessionToken 생성
	auto sessionToken = generateSessionToken();
	session->setUserId(claims.userId);
	session->setSessionToken(sessionToken);

	// sessionManager 등록
	SessionManager::getInstance().add(session);

	// AUTH_OK 응답 (sessionToken 포함)
	nlohmann::json response;
	response["type"] = PacketTypes::AUTH_OK;
	response["sessionToken"] = std::vector<uint8_t>(sessionToken.begin(), sessionToken.end());
	session->sendPacket(response);


	LOG_INFO("AUTH SUCCESS : userId = {}", claims.userId);

	broadcastOnline(session);
}

void AuthHandler::broadcastOnline(std::shared_ptr<Session> session) {
	nlohmann::json stateMsg;
	stateMsg["type"] = PacketTypes::USER_STATE;
	stateMsg["userId"] = session->getUserId();
	stateMsg["status"] = "ONLINE";

	auto all = SessionManager::getInstance().getAll();

	for (auto& s : all) {
		if (s->getUserId() == session->getUserId())
			continue;
		if (s->isWatching(session->getUserId())) {
			s->sendPacket(stateMsg);
		}
	}
}