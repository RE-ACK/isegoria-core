#include "Session.hpp"
#include "SessionManager.hpp"
#include "room/RoomManager.hpp"
#include "utils/Logger.hpp"
#include "protocol/PacketTypes.hpp"
#include "handler/AuthHandler.hpp"
#include "handler/ChatHandler.hpp"
#include "handler/VoiceHandler.hpp"
#include "handler/StatusHandler.hpp"

#include <unordered_map>

using PacketHandler = std::function<void(std::shared_ptr<Session>, const nlohmann::json&)>;

const std::unordered_map<std::string, PacketHandler> handlers = {
	{ PacketTypes::AUTH,        AuthHandler::handle },
	{ PacketTypes::JOIN_TEXT,   ChatHandler::handleJoin },
	{ PacketTypes::LEAVE_TEXT,  ChatHandler::handleLeave },
	{ PacketTypes::CHAT_MSG,    ChatHandler::handleMessage },
	{ PacketTypes::VOICE_JOIN,  VoiceHandler::handleJoin },
	{ PacketTypes::VOICE_LEAVE, VoiceHandler::handleLeave },
	{ PacketTypes::SET_MUTE,    VoiceHandler::handleMute },
	{ PacketTypes::SET_DEAFEN,  VoiceHandler::handleDeafen },
	{ PacketTypes::SET_STATUS,  StatusHandler::handleStatus },
	{ PacketTypes::PING,        StatusHandler::handlePing }
};

Session::Session(asio::ip::tcp::socket socket)
	: _socket(std::move(socket))
	, _lastPingAt(std::chrono::steady_clock::now()) {
}

void Session::start() {
	readLength();
}

void Session::readLength() {
	auto self = shared_from_this();
	auto lengthBuf = std::make_shared<std::array<uint8_t, 4>>();

	asio::async_read(_socket, asio::buffer(*lengthBuf),
		[this, self, lengthBuf](std::error_code ec, std::size_t) {
			if (ec) {
				disconnect();
				return;
			}

			uint32_t length = 0;
			std::memcpy(&length, lengthBuf->data(), 4);
			// 네트워크 바이트 오더 변환
			// 네트워크 통신은 빅엔디안 방식
			length = ntohl(length); 

			if (length == 0 || length > 4096) {
				LOG_WARN("Abnormal packet length : {}", length);
				disconnect();
				return;
			}

			readBody(length);
		});
}

void Session::readBody(uint32_t length) {
	auto self = shared_from_this();
	_readBuf.resize(length);

	asio::async_read(_socket, asio::buffer(_readBuf),
		[this, self](std::error_code ec, std::size_t) {
			if (ec) {
				disconnect();
				return;
			}

			std::string raw(_readBuf.begin(), _readBuf.end());
			handlePacket(raw);
			readLength();
		});
}

void Session::handlePacket(const std::string& raw) {
	try {
		auto json = nlohmann::json::parse(raw);
		std::string type = json.at("type").get<std::string>();

		// @TODO 기존방식
		//if (type == PacketTypes::AUTH)       AuthHandler::handle(shared_from_this(), json);
		//else if (type == PacketTypes::JOIN_TEXT)  ChatHandler::handleJoin(shared_from_this(), json);
		//else if (type == PacketTypes::LEAVE_TEXT) ChatHandler::handleLeave(shared_from_this(), json);
		//else if (type == PacketTypes::CHAT_MSG)   ChatHandler::handleMessage(shared_from_this(), json);
		//else if (type == PacketTypes::VOICE_JOIN) VoiceHandler::handleJoin(shared_from_this(), json);
		//else if (type == PacketTypes::VOICE_LEAVE)VoiceHandler::handleLeave(shared_from_this(), json);
		//else if (type == PacketTypes::SET_MUTE)   VoiceHandler::handleMute(shared_from_this(), json);
		//else if (type == PacketTypes::SET_DEAFEN) VoiceHandler::handleDeafen(shared_from_this(), json);
		//else if (type == PacketTypes::SET_STATUS) StatusHandler::handleStatus(shared_from_this(), json);
		//else if (type == PacketTypes::PING)       StatusHandler::handlePing(shared_from_this(), json);
		//else LOG_WARN("알 수 없는 패킷 타입: {}", type);

		// @TODO 이방법이 더좋은듯
		auto it = handlers.find(type);
		if (it != handlers.end()) {
			it->second(shared_from_this(), json);
		}
		else {
			LOG_WARN("알 수 없는 패킷 타입: {}", type);
		}

	}
	catch (const std::exception& e) {
		LOG_ERROR("Packet Parsing Failure : {}", e.what());
	}
}

void Session::sendPacket(const nlohmann::json& json) {
	auto self = shared_from_this();
	std::string body = json.dump(); // 객체 -> json 문자열 변환
	// 엔디언 변환
	uint32_t length = htonl(static_cast<uint32_t>(body.size()));

	auto buf = std::make_shared<std::vector<uint8_t>>();
	buf->resize(4 + body.size());
	std::memcpy(buf->data(), &length, 4);
	std::memcpy(buf->data() + 4, body.data(), body.size());

	asio::async_write(_socket, asio::buffer(*buf),
		[this, self, buf](std::error_code ec, std::size_t) {
			if (ec) {
				LOG_ERROR("SendPacket Fail : {}", ec.message());
				disconnect();
			}
		});
}

void Session::disconnect() {
	LOG_INFO("Session Exit: userId = {}", _userId);
	_socket.close();
	SessionManager::getInstance().remove(_userId);
	RoomManager::getInstance().removeUserFromAll(_userId);
}