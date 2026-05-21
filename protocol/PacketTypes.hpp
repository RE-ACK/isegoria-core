#pragma once

namespace PacketTypes {
    // 인증
    constexpr const char* AUTH = "AUTH";
    constexpr const char* AUTH_OK = "AUTH_OK";
    constexpr const char* AUTH_FAIL = "AUTH_FAIL";

    // 텍스트 채널
    constexpr const char* JOIN_TEXT = "JOIN_TEXT";
    constexpr const char* LEAVE_TEXT = "LEAVE_TEXT";
    constexpr const char* CHAT_MSG = "CHAT_MSG";

    // 음성 채널
    constexpr const char* VOICE_JOIN = "VOICE_JOIN";
    constexpr const char* VOICE_LEAVE = "VOICE_LEAVE";
    constexpr const char* VOICE_FULL = "VOICE_FULL";
    constexpr const char* VOICE_STATE = "VOICE_STATE";

    // 음소거 / 귀막기
    constexpr const char* SET_MUTE = "SET_MUTE";
    constexpr const char* SET_DEAFEN = "SET_DEAFEN";

    // 유저 상태
    constexpr const char* USER_STATE = "USER_STATE";
    constexpr const char* SET_STATUS = "SET_STATUS";
    constexpr const char* SUBSCRIBE_STATUS = "SUBSCRIBE_STATUS";
    constexpr const char* SUBSCRIBE_STATUS_OK = "SUBSCRIBE_STATUS_OK";

    // 메시지
    constexpr const char* MSG_DELETED = "MSG_DELETED";

    // 멤버
    constexpr const char* MEMBER_JOIN = "MEMBER_JOIN";
    constexpr const char* MEMBER_LEAVE = "MEMBER_LEAVE";
    constexpr const char* KICKED = "KICKED";

    // 하트비트
    constexpr const char* PING = "PING";
    constexpr const char* PONG = "PONG";
}