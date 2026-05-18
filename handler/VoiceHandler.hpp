#pragma once

#include <memory>
#include <nlohmann/json.hpp>

class Session;

class VoiceHandler {
public:
    static void handleJoin(std::shared_ptr<Session> session, const nlohmann::json& json);
    static void handleLeave(std::shared_ptr<Session> session, const nlohmann::json& json);
    static void handleMute(std::shared_ptr<Session> session, const nlohmann::json& json);
    static void handleDeafen(std::shared_ptr<Session> session, const nlohmann::json& json);
};