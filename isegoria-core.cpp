#include <asio.hpp>
#include "utils/Env.hpp"
#include "utils/Logger.hpp"
#include "infra/SpringClient.hpp"
#include "network/TCPListener.hpp"
#include "network/UDPListener.hpp"
#include "heartbeat/HeartbeatManager.hpp"

int main() {
    // 1. 환경변수 로드
    Env::load();

    // 2. 로거 초기화
    Logger::init();
    LOG_INFO("Server Starting...");

    // 3. SpringClient 초기화
    SpringClient::getInstance().init();

    // 4. io_context 생성
    asio::io_context ioContext;

    // 5. TCP/UDP 리스너 시작
    TCPListener tcpListener(ioContext, 9000);
    tcpListener.start();

    UDPListener udpListener(ioContext, 9001);
    udpListener.start();

    // 6. HeartbeatManager 시작
    HeartbeatManager::getInstance().start();

    LOG_INFO("Server Started. TCP:9000, UDP:9001");

    // 7. 이벤트 루프 시작
    ioContext.run();

    // 8. 종료
    HeartbeatManager::getInstance().stop();
    Logger::shutdown();

    return 0;
}