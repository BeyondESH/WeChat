cmake_minimum_required(VERSION 3.29)
project(GateServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(GateServer main.cpp
        CServer.cpp
        CServer.h
        const.h
        HttpConnection.cpp
        HttpConnection.h
        singleton.hpp
        LogicSystem.cpp
        LogicSystem.h)

# 设置boost路径
set(BOOST_ROOT "D:\\C++library\\boost")
# 设置boost库文件
find_package(Boost 1.86.0 REQUIRED)
# 设置boost头文件
target_include_directories(GateServer PRIVATE ${Boost_INCLUDE_DIRS})
# 设置boost库文件
target_link_libraries(GateServer PRIVATE ${Boost_LIBRARIES} ws2_32 Mswsock)

set(CMAKE_TOOLCHAIN_FILE "C:\\Users\\Beyond\\.vcpkg-clion\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake")
set(CMAKE_PREFIX_PATH "C:\\Users\\Beyond\\.vcpkg-clion\\vcpkg\\installed\\x64-windows")
find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(GateServer PRIVATE JsonCpp::JsonCpp)
find_package(Protobuf CONFIG REQUIRED)
target_link_libraries(GateServer PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)