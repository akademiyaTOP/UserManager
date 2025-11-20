cmake_minimum_required(VERSION 3.16)
project(UserManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Widgets Sql)

add_subdirectory(Server)
add_subdirectory(Client)