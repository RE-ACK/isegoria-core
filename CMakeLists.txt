cmake_minimum_required (VERSION 3.10)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("isegoria-core")

find_package(asio CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "*.cpp"
    "*.h"
    "*.hpp"
)

list(FILTER SOURCES EXCLUDE REGEX ".*/out/.*")

add_executable (isegoria-core ${SOURCES} "auth/JwtVerifier.cpp" "network/Session.cpp" "network/SessionManager.cpp")

target_include_directories(isegoria-core PRIVATE .)

target_link_libraries(isegoria-core PRIVATE
    asio::asio
    CURL::libcurl
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    jwt-cpp::jwt-cpp
    ws2_32
    wsock32
)

target_compile_definitions(isegoria-core PRIVATE
    _WIN32_WINNT=0x0601
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET isegoria-core PROPERTY CXX_STANDARD 20)
endif()