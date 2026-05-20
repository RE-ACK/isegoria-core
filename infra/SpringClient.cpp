#include "SpringClient.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "utils/Logger.hpp"
#include <cstdlib>

static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
	output->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void SpringClient::init() {
	const char* url = std::getenv("SPRING_API_URL");
	const char* apiKey = std::getenv("INTERNAL_API_KEY");

	_baseUrl = url ? url : "http://localhost:8000";
	_apiKey = apiKey ? apiKey : "";

	if (_apiKey.empty()) {
		LOG_WARN("[SPRINGCLIENT] INTERNAL_API_KEY IS NOT SET");
	}

	LOG_INFO("[SPRINGCLIENT] INITALIZED WITH BASE URL : {}", _baseUrl);
}

std::string SpringClient::post(const std::string& endpoint, const std::string& body) {
	std::string response;
	std::string url = _baseUrl + endpoint;

	CURL* curl = curl_easy_init();
	if (!curl)
		return "";

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	std::string apiKeyHeader = "X-Internal-API-Key: " + _apiKey;
	headers = curl_slist_append(headers, apiKeyHeader.c_str());

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		LOG_ERROR("[SPRINGCLIENT] POST {} FAILED: {}", endpoint, curl_easy_strerror(res));
	}

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	return response;
}

std::string SpringClient::get(const std::string& endpoint) {
	std::string response;
	std::string url = _baseUrl + endpoint;

	CURL* curl = curl_easy_init();
	if (!curl) return "";

	struct curl_slist* headers = nullptr;
	std::string apiKeyHeader = "X-Internal-API-Key: " + _apiKey;
	headers = curl_slist_append(headers, apiKeyHeader.c_str());

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		LOG_ERROR("[SpringClient] GET {} FAILED: {}", endpoint, curl_easy_strerror(res));
	}

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	return response;
}

void SpringClient::postMessage(int64_t serverId, int64_t channelId, int64_t userId, const std::string& content) {
	nlohmann::json body;
	//body["serverId"] = serverId;
	body["channelId"] = channelId;
	body["senderId"] = userId;
	body["content"] = content;
	post("/api/messages/create", body.dump());
}

void SpringClient::postKick(int64_t userId, int64_t guildId) {
	nlohmann::json body;
	body["userId"] = userId;
	body["guildId"] = guildId;
	post("/api/internal/kick", body.dump());
}

void SpringClient::postMessageDelete(int64_t messageId, int64_t channelId) {
	nlohmann::json body;
	body["messageId"] = messageId;
	body["channelId"] = channelId;
	post("/api/internal/messages/delete", body.dump());
}

std::vector<ChannelInfo> SpringClient::getChannels() {
	std::string response = get("/api/internal/channels");
	std::vector<ChannelInfo> channels;

	if (response.empty()) return channels;

	try {
		auto json = nlohmann::json::parse(response);
		for (auto& ch : json) {
			ChannelInfo info;
			info.channelId = ch["channelId"].get<int64_t>();
			info.channelName = ch["channelName"].get<std::string>();
			info.guildId = ch["guildId"].get<int64_t>();
			channels.push_back(info);
		}
	}
	catch (const std::exception& e) {
		LOG_ERROR("[SpringClient] getChannels parse failed: {}", e.what());
	}

	return channels;
}