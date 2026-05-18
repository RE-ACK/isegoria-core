#include <iostream>
#include <cstdlib>
#include "utils/Env.hpp" // 직접 만든 헤더 포함
#include "utils/Logger.hpp"

int main() {
    Logger::init();
    LOG_INFO("Server Start");

    try {
        Env::load();
        LOG_INFO(".env file loaded successfully via custom Env class!");
    }
    catch (const std::exception& e) {
        LOG_ERROR(".env load failed: {}", e.what());
    }

    const char* jwtSecret = std::getenv("JWT_SECRET");

    if (jwtSecret) {
        LOG_INFO("[SUCCESS] JWT_SECRET LOAD SUCCESS: {}", jwtSecret);
    }
    else {
        LOG_ERROR("[FAIL] JWT_SECRET NOT FOUND");
    }

    Logger::shutdown();
    return 0;
}