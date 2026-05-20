#pragma once

#include <asio.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <array>
#include <chrono>
#include <unordered_set>

class Session : public std::enable_shared_from_this<Session> {
public:
	explicit Session(asio::ip::tcp::socket socket);

	void start();
	void sendPacket(const nlohmann::json& json);

    // Getter
    int64_t getUserId() const { return _userId; }
    std::string getUsername() const { return _username; }
    uint64_t getTextChannelId() const { return _textChannelId; }
    uint64_t getVoiceChannelId() const { return _voiceChannelId; }
    bool isMuted() const { return _isMuted; }
    bool isDeafened() const { return _isDeafened; }
    const std::array<uint8_t, 32>& getSessionToken() const { return _sessionToken; }
    asio::ip::udp::endpoint getUdpEndpoint() const { return _udpEndPoint; }
    std::chrono::steady_clock::time_point getLastPingAt() const { return _lastPingAt; }
    asio::ip::tcp::socket& getSocket() { return _socket; }

    // Setter
    void setUserId(int64_t userId) { _userId = userId; }
    void setUsername(const std::string& username) { _username = username; }
    void setTextChannelId(uint64_t id) { _textChannelId = id; }
    void setVoiceChannelId(uint64_t id) { _voiceChannelId = id; }
    void setMuted(bool muted) { _isMuted = muted; }
    void setDeafened(bool deafened) { _isDeafened = deafened; }
    void setSessionToken(const std::array<uint8_t, 32>& token) { _sessionToken = token; }
    void setUdpEndpoint(const asio::ip::udp::endpoint& ep) { _udpEndPoint = ep; }
    void updateLastPingAt() { _lastPingAt = std::chrono::steady_clock::now(); }
    void addWatchedUser(int64_t userId) { _watchedUserIds.insert(userId); }
    void clearWatchedUsers() { _watchedUserIds.clear(); }
    bool isWatching(int64_t userId) const { return _watchedUserIds.count(userId) > 0; }


private:
	void readLength();
	void readBody(const uint32_t length);
	void handlePacket(const std::string& raw);
	void disconnect();

	asio::ip::tcp::socket _socket;

	int64_t _userId{ 0 };
	std::string _username;
	uint64_t _textChannelId{ 0 };
	uint64_t _voiceChannelId{ 0 };
	bool _isMuted{ false };
	bool _isDeafened{ false };

	std::array<uint8_t, 32> _sessionToken{};
	asio::ip::udp::endpoint _udpEndPoint;
	std::chrono::steady_clock::time_point _lastPingAt;
	std::unordered_set<int64_t> _watchedUserIds;

	std::vector<uint8_t> _readBuf;
};