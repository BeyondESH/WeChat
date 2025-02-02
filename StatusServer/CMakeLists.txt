cmake_minimum_required(VERSION 3.30)
project(StatusServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
add_definitions(-D_WIN32_WINNT=0x0601)
# 设置vcpkg工具链文件路径
set(CMAKE_TOOLCHAIN_FILE "D:/C++library/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_PREFIX_PATH "D:/C++library/vcpkg/installed/x64-windows")
set(Protobuf_PROTOC_EXECUTABLE "${CMAKE_PREFIX_PATH}/tools/protobuf/protoc.exe")
set(GRPC_CPP_PLUGIN_EXECUTABLE "${CMAKE_PREFIX_PATH}/tools/grpc/grpc_cpp_plugin.exe")
# 设置策略 CMP0167 为 NEW
cmake_policy(SET CMP0167 NEW)

add_executable(StatusServer main.cpp
        AsioIOcontextPool.h
        AsioIOcontextPool.cpp
        ConfigMgr.hpp
        const.h
        Crypto.h
        Crypto.cpp
        Defer.hpp
        message.pb.cc
        message.pb.h
        message.grpc.pb.cc
        message.grpc.pb.h
        MySQLMgr.h
        MySQLMgr.cpp
        RedisMgr.h
        RedisMgr.cpp
        singleton.hpp
        StatusServiceImpl.cpp
        StatusServiceImpl.h
)
# absl库
find_package(absl CONFIG REQUIRED)
target_link_libraries(StatusServer PRIVATE absl::any absl::log absl::base absl::bits)

# protobuf库
set(Protobuf_PROTOC_EXECUTABLE "${CMAKE_PREFIX_PATH}/bin/protoc.exe")
find_package(Protobuf CONFIG REQUIRED)
target_link_libraries(StatusServer PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)

# OpenSSL库
find_package(OpenSSL REQUIRED)
target_link_libraries(StatusServer PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# gRPC库
find_package(gRPC CONFIG REQUIRED)
target_link_libraries(StatusServer PRIVATE gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc++_alts)

# Boost库
find_package(Boost REQUIRED COMPONENTS system filesystem json)
target_link_libraries(StatusServer PRIVATE Boost::system Boost::filesystem)

# Winsock和Mswsock库
target_link_libraries(StatusServer PRIVATE ws2_32 Mswsock)

# json库
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(StatusServer PRIVATE nlohmann_json::nlohmann_json)

# redis++库
find_package(redis++ CONFIG REQUIRED)
target_link_libraries(StatusServer PRIVATE redis++::redis++)

# mysql-connector-c++库
find_package(unofficial-mysql-connector-cpp CONFIG REQUIRED)
target_link_libraries(StatusServer PRIVATE unofficial::mysql-connector-cpp::connector)
