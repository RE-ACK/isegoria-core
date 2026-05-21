#include "HeartbeatManager.hpp"
#include "network/Session.hpp"
#include "network/SessionManager.hpp"
#include "room/RoomManager.hpp"
#include "protocol/PacketTypes.hpp"
#include "utils/Logger.hpp"
#include <chrono>
#include <vector>

void HeartbeatManager::start() {
	_running = true;
	_thread = std::thread(&HeartbeatManager::run, this);
	LOG_INFO("[HEARTBEATMANAGER] STARTED");
}

void HeartbeatManager::stop() {
	_running = false;
	if (_thread.joinable())
		_thread.join();
	LOG_INFO("[HEARTBEATMANAGER] STOPPED");
}

void HeartbeatManager::run() {
	while (_running) {
		std::this_thread::sleep_for(std::chrono::seconds(PING_INTERVAL_SEC));
		if (!_running)
			break;
		checkSessions();
	}
}

void HeartbeatManager::checkSessions() {
	auto now = std::chrono::steady_clock::now();
	auto sessions = SessionManager::getInstance().getAll();

	for (auto& session : sessions) {
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
			now - session->getLastPingAt()
		).count();

		if (elapsed >= TIMEOUT_SEC) {
			LOG_WARN("[HEARTBEATMANAGER] SESSION TIMEOUT: userId={}", session->getUserId());

			RoomManager::getInstance().removeUserFromAll(session->getUserId());

			nlohmann::json offline;
			offline["type"] = PacketTypes::USER_STATE;
			offline["userId"] = session->getUserId();
			offline["status"] = "OFFLINE";

			auto all = SessionManager::getInstance().getAll();
			for (auto& s : all) {
				if (s->getUserId() == session->getUserId())
					continue;
				s->sendPacket(offline);
			}

			SessionManager::getInstance().remove(session->getUserId());
		}
		else {
			session->sendPacket({
				{"type", PacketTypes::PING}
			});
		}
	}
}