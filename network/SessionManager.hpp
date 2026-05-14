#pragma once

#include <unordered_map>
#include <memory>
#include <mutex>
#include <vector>
#include <array>
#include <cstdint>

class Session;

class SessionManager {
public:
	static SessionManager& getInstance() {
		static SessionManager instance;
		return instance;
	}

	void add(std::shared_ptr<Session> session);
	void remove(int64_t userId);
	std::shared_ptr<Session> findByUserId(int64_t userId);
	std::shared_ptr<Session> findByToken(const std::array<uint8_t, 32>& token);
	std::vector<std::shared_ptr<Session>> getAll();

private:
	SessionManager() = default;
	SessionManager(const SessionManager&) = delete;
	SessionManager& operator=(const SessionManager&) = delete;

	std::mutex _mutex;
	std::unordered_map<int64_t, std::shared_ptr<Session>> _sessionMap;
	std::unordered_map<std::string, int64_t> _tokenMap;
};