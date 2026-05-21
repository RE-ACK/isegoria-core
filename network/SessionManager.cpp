#include "SessionManager.hpp"
#include "Session.hpp"

void SessionManager::add(std::shared_ptr<Session> session) {
	std::lock_guard<std::mutex> lock(_mutex);

	_sessionMap[session->getUserId()] = session;

	// sessionToken -> string key 변환
	const auto& token = session->getSessionToken();
	std::string tokenKey(token.begin(), token.end());
	_tokenMap[tokenKey] = session->getUserId();
}

void SessionManager::remove(int64_t userId) {
	std::lock_guard<std::mutex> lock(_mutex);

	auto it = _sessionMap.find(userId);
	if (it != _sessionMap.end()) {
		const auto& token = it->second->getSessionToken();
		std::string tokenKey(token.begin(), token.end());
		_tokenMap.erase(tokenKey);
		_sessionMap.erase(it);
	}
}

std::shared_ptr<Session> SessionManager::findByUserId(int64_t userId) {
	std::lock_guard<std::mutex> lock(_mutex);

	auto it = _sessionMap.find(userId);
	if (it != _sessionMap.end())
		return it->second;
	return nullptr;
}

std::shared_ptr<Session> SessionManager::findByToken(const std::array<uint8_t, 32>& token) {
	std::lock_guard<std::mutex> lock(_mutex);

	std::string tokenKey(token.begin(), token.end());
	auto it = _tokenMap.find(tokenKey);
	if (it != _tokenMap.end()) {
		auto sit = _sessionMap.find(it->second);
		if (sit != _sessionMap.end()) {
			return sit->second;
		}
	}
	return nullptr;
}

std::vector<std::shared_ptr<Session>> SessionManager::getAll() {
	std::lock_guard<std::mutex> lock(_mutex);

	std::vector<std::shared_ptr<Session>> result;
	
	for (auto& [id, session] : _sessionMap) {
		result.push_back(session);
	}

	return result;
}