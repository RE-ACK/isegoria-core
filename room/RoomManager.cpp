#include "RoomManager.hpp"
#include "network/Session.hpp"
#include "network/SessionManager.hpp"

void RoomManager::joinTextChannel(int64_t userId, uint64_t channelId) {
	std::lock_guard<std::mutex> lock(_mutex);

	_textChannelMap[channelId].insert(userId);
}

void RoomManager::leaveTextChannel(int64_t userId, uint64_t channelId) {
	std::lock_guard<std::mutex> lock(_mutex);

	auto it = _textChannelMap.find(channelId);
	if (it != _textChannelMap.end()) {
		it->second.erase(userId);
	}
}

bool RoomManager::joinVoiceChannel(int64_t userId, uint64_t channelId) {
	std::lock_guard<std::mutex> lock(_mutex);

	auto& members = _voiceChannelMap[channelId];
	if (members.size() >= MAX_VOICE_MEMBERS)
		return false;
	members.insert(userId);
	return true;
}

void RoomManager::leaveVoiceChannel(int64_t userId, uint64_t channelId) {
	std::lock_guard<std::mutex> lock(_mutex);

	auto it = _voiceChannelMap.find(channelId);
	if (it != _voiceChannelMap.end()) {
		it->second.erase(userId);
	}
}


std::vector<std::shared_ptr<Session>> RoomManager::getTextChannelMembers(uint64_t channelId) {
	std::lock_guard<std::mutex> lock(_mutex);

	std::vector<std::shared_ptr<Session>> result;

	auto it = _textChannelMap.find(channelId);
	if (it == _textChannelMap.end())
		return result;

	auto& sm = SessionManager::getInstance();

	for (int64_t userId : it->second) {
		auto session = sm.findByUserId(userId);
		if (session)
			result.push_back(session);
	}

	return result;
}

std::vector<std::shared_ptr<Session>> RoomManager::getVoiceChannelMembers(uint64_t channelId) {
	std::lock_guard<std::mutex> lock(_mutex);

	std::vector<std::shared_ptr<Session>> result;

	auto it = _voiceChannelMap.find(channelId);
	if (it == _voiceChannelMap.end())
		return result;

	auto& sm = SessionManager::getInstance();

	for (int64_t userId : it->second) {
		auto session = sm.findByUserId(userId);
		if (session)
			result.push_back(session);
	}

	return result;
}

void RoomManager::removeUserFromAll(int64_t userId) {
	std::lock_guard<std::mutex> lock(_mutex);

	for (auto& [channelId, members] : _textChannelMap) {
		members.erase(userId);
	}

	for (auto& [channelId, members] : _voiceChannelMap) {
		members.erase(userId);
	}
}
