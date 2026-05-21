#pragma once

#include <cstdint>

#pragma pack(push, 1)
struct VoiceHeader {
	uint8_t sessionToken[32]; // UDP 인증
	uint64_t channelId; // 8 byte
	uint32_t sequenceNum; // 4 byte (Jitter Buffer 정렬용)
	uint32_t timestamp; // 4 byte (밀리초)
};
#pragma pack(pop)

constexpr size_t VOICE_HEADER_SIZE = sizeof(VoiceHeader); // 48 byte
constexpr size_t MAX_VOICE_PACKET_SIZE = 256; // 최대 패킷 크기
constexpr size_t MAX_OPUS_DATA_SIZE = MAX_VOICE_PACKET_SIZE - VOICE_HEADER_SIZE;


// 직렬화: VoiceHeader를 송신 버퍼에 씀
inline bool serializeVoiceHeader(uint8_t* buffer, size_t bufferSize, const VoiceHeader& header) {
	if (bufferSize < VOICE_HEADER_SIZE) return false;
	memcpy(buffer, &header, VOICE_HEADER_SIZE);
	return true;
}

// 역직렬화 : 수신 버퍼에서 VoiceHeader 추출
inline bool parseVoiceHeader(const uint8_t* buffer, size_t bufferSize, VoiceHeader& header) {
	if (bufferSize < VOICE_HEADER_SIZE)
		return false;

	memcpy(&header, buffer, VOICE_HEADER_SIZE);
	return true;
}

// Opus 데이터 포인터 및 크기 추출
inline const uint8_t* getOpusData(const uint8_t* buffer) {
	return buffer + VOICE_HEADER_SIZE;
}

inline size_t getOpusDataSize(size_t totalPacketSize) {
	return totalPacketSize - VOICE_HEADER_SIZE;
}