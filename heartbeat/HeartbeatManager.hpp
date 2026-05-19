#pragma once

#include <thread>
#include <atomic>

class HeartbeatManager {
public:
	static HeartbeatManager& getInstance() {
		static HeartbeatManager instance;
		return instance;
	}

	void start();
	void stop();

private:
	HeartbeatManager() = default;
	HeartbeatManager(const HeartbeatManager&) = delete;
	HeartbeatManager& operator=(const HeartbeatManager&) = delete;

	void run();
	void checkSessions();

	std::thread _thread;
	std::atomic<bool> _running{ false };

	static constexpr int PING_INTERVAL_SEC = 30;
	static constexpr int TIMEOUT_SEC = 60;
};