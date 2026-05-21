#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <mutex>
#include <cstdint>

class Session;

class RoomManager {
public:
	static RoomManager& getInstance() {
		static RoomManager instance;
		return instance;
	}

	void joinTextChannel(int64_t userId, uint64_t channelId);
	void leaveTextChannel(int64_t userId, uint64_t channelId);

	bool joinVoiceChannel(int64_t userId, uint64_t channelId);
	void leaveVoiceChannel(int64_t userId, uint64_t channelId);

	std::vector<std::shared_ptr<Session>> getTextChannelMembers(uint64_t channelId);
	std::vector<std::shared_ptr<Session>> getVoiceChannelMembers(uint64_t channelId);
	std::unordered_map<uint64_t, std::unordered_set<uint64_t>> getAllVoiceChannels();

	// 연결 끊길때 호출
	void removeUserFromAll(int64_t userId);

private:
	RoomManager() = default;
	RoomManager(const RoomManager&) = delete;
	RoomManager& operator=(const RoomManager&) = delete;

	static constexpr int MAX_VOICE_MEMBERS = 8;

	std::mutex _mutex;
	std::unordered_map<uint64_t, std::unordered_set<uint64_t>> _textChannelMap;
	std::unordered_map<uint64_t, std::unordered_set<uint64_t>> _voiceChannelMap;
};