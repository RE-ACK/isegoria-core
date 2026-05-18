# =====================================================================
# 1단계: 빌드 스테이지 (GCC 컴파일러와 vcpkg로 빌드 진행)
# =====================================================================
FROM ubuntu:22.04 AS builder

# 우분투 패키지 설치 중 인터랙티브 팝업 뜨는 것 방지
ENV DEBIAN_FRONTEND=noninteractive

# 1. 리눅스 필수 빌드 도구 및 GCC 컴파일러 패키지 설치
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# 2. 컨테이너 내부 전용 공간(/opt/vcpkg)에 vcpkg 설치 및 리눅스용 부트스트랩 실행
RUN git clone https://github.com/microsoft/vcpkg.git /opt/vcpkg \
    && /opt/vcpkg/bootstrap-vcpkg.sh

# 3. 작업 디렉터리를 프로젝트 루트(/app)로 지정
WORKDIR /app

# 4. 내 컴퓨터의 프로젝트 소스 코드 및 vcpkg.json 전체 복사
COPY . .

# 5. 빌드 폴더 생성 및 이동
WORKDIR /app/build

# 6. CMake 구성 (vcpkg 툴체인 및 크로스 플랫폼 최적화 Release 빌드 설정)
# 이 과정에서 CMakeLists.txt의 PROJECT_ROOT_DIR 매크로에 "/app" 경로가 주입됩니다.
RUN cmake .. -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DCMAKE_BUILD_TYPE=Release

# 7. 컴파일 시작 -> 리눅스용 바이너리 실행 파일 생성
RUN cmake --build .

# =====================================================================
# 2단계: 실행 스테이지 (용량을 줄이기 위해 실행 파일만 남김)
# =====================================================================
FROM ubuntu:22.04

# 실행 환경의 작업 디렉터리 설정
WORKDIR /app

# 1단계 빌드 컨테이너의 결과물 폴더에서 컴파일 완료된 실행 파일만 쏙 빼서 복사
COPY --from=builder /app/build/isegoria-core /app/isegoria-core

# 서버가 사용할 포트 설정 (예시: 8080)
EXPOSE 8080

# 컨테이너가 켜지면 C++ 서버 실행
CMD ["./isegoria-core"]