cmake_minimum_required(VERSION 3.16)

project(ByteFlux)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(byteflux
    src/main.cpp
    src/validator.cpp
)
