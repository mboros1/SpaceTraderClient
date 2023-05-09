cmake_minimum_required(VERSION 3.24)
project(SpaceTraderClient VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenSSL REQUIRED)
find_package(Boost 1.81.0 REQUIRED COMPONENTS system thread json)
find_package(SQLite3 REQUIRED)


add_executable(SpaceTraderClient main.cpp register_db.cpp)

target_include_directories(SpaceTraderClient PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(SpaceTraderClient Boost::system Boost::thread Boost::json OpenSSL::SSL OpenSSL::Crypto SQLite::SQLite3)

