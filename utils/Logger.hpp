

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <memory>
#include <vector>

class Logger {
public:
	static void init() {
		std::vector<spdlog::sink_ptr> sinks;

		//콘솔 출력
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_level(spdlog::level::debug);
		console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
		sinks.push_back(console_sink);

		//일별 롤링 파일 출력 (매일 자정 교체)
		auto file_sink = std::make_shared<spdlog::sinks::daily_file_format_sink_mt>("logs/isegoria.log", 0, 0);
		file_sink->set_level(spdlog::level::info);
		file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");
		sinks.push_back(file_sink);

		auto logger = std::make_shared<spdlog::logger>("isegoria", sinks.begin(), sinks.end());
		logger->set_level(spdlog::level::debug);
		logger->flush_on(spdlog::level::warn);

		spdlog::set_default_logger(logger);
		spdlog::info("Logger Initialized");
	}

	static void shutdown() {
		spdlog::shutdown();
	}
};

//spdlog 래핑.콘솔 + 일별 롤링 파일 동시 출력.
//INFO : 접속, 채널 입장 / 퇴장 등 정상 동작.
//WARN : UDP 패킷 연속 손실, 재연결 시도.
//ERROR : JWT 위조, 4KB 초과 패킷, 토큰 불일치.

#define LOG_INFO(...)  spdlog::info(__VA_ARGS__)
#define LOG_WARN(...)  spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)