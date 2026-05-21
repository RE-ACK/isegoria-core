#pragma once

#include <string>
#include <vector>

struct ChannelInfo {
	int64_t channelId;
	std::string channelName;
	int64_t guildId;
};

class SpringClient {
public:
	static SpringClient& getInstance() {
		static SpringClient instance;
		return instance;
	}

	void init();

	void postMessage(int64_t serverid, int64_t channelId, int64_t userId, const std::string& content);
	void postKick(int64_t userId, int64_t guildId);
	void postMessageDelete(int64_t messageId, int64_t channelId);
	std::vector<ChannelInfo> getChannels();

private:
	SpringClient() = default;
	SpringClient(const SpringClient&) = delete;
	SpringClient& operator=(const SpringClient&) = delete;

	std::string post(const std::string& endpoint, const std::string& body);
	std::string get(const std::string& endpoint);

	std::string _baseUrl;
	std::string _apiKey;
};