#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include "Logger.hpp"

class Env {
private:
    static std::string trim(const std::string& str) {
        if (str.empty()) return str;
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

public:
    // 매개변수 기본값을 CMake가 준 경로 기반으로 설정하도록 변경
    static void load() {
        // 1. CMake가 꽂아준 프로젝트 루트 절대 경로 가져오기
        std::filesystem::path rootPath = PROJECT_ROOT_DIR;

        // 2. 루트 경로 바로 아래의 .env 파일 위치 계산 (OS 맞춰 슬래시 자동 처리)
        std::filesystem::path envPath = rootPath / ".env";

        std::ifstream file(envPath);

        if (!file.is_open()) {
            LOG_ERROR("[Env] .env Cannot open file! Please check the path. Entered path: {}", envPath.string());
            return;
        }

        LOG_INFO("[Env] .env The file has been successfully opened from root: {}", envPath.string());

        std::string line;
        int lineCount = 0;
        while (std::getline(file, line)) {
            lineCount++;

            line = trim(line);

            if (line.empty() || line[0] == '#') continue;

            auto pos = line.find('=');
            if (pos == std::string::npos) {
                LOG_WARN("[Env] There is no '=' on the {}th line. Skip.: {}", lineCount, line);
                continue;
            }

            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));

            if (!key.empty()) {
                LOG_INFO("[Env] Registering environment variables -> KEY: [{}], VALUE: [{}]", key, value);

                // 3. 윈도우와 리눅스/도커 환경 분기 처리
#if defined(_WIN32)
                _putenv_s(key.c_str(), value.c_str());
#else
                setenv(key.c_str(), value.c_str(), 1); // 리눅스 표준 환경변수 주입 함수
#endif
            }
        }
        LOG_INFO("[Env] .env File parsing complete (total {} lines processed)", lineCount);
    }
};