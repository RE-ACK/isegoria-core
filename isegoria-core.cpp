// isegoria-core.cpp : 애플리케이션의 진입점을 정의합니다.
//

#include <iostream>
#include "utils/Logger.hpp"


using namespace std;

int main() {
    Logger::init();

    LOG_INFO("Server Start");
    LOG_WARN("WARNING TEST");
    LOG_ERROR("ERROR TEST");

    Logger::shutdown();
    return 0;
}