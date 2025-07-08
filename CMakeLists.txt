cmake_minimum_required(VERSION 3.30)
project(compi)

set(CMAKE_CXX_STANDARD 20)
include_directories(.)

add_executable(ProyectoCompi main.cpp
        token.h
        token.cpp
        scanner.h
        scanner.cpp
        parser.cpp
        parser.h
        expression.cpp
        expression.h
        visitor.cpp
        visitor.h
        environment.h
        gencode.cpp
)
