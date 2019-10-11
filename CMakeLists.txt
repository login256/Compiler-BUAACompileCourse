cmake_minimum_required(VERSION 3.10)
project(UniversalCompiler)

set(CMAKE_CXX_STANDARD 11)

add_executable(UniversalCompiler main.cpp include/Token.h src/Matcher.cpp include/Matcher.h src/Token.cpp src/homework/scanning_homework.cpp include/homework/scanning_homework.h src/init.cpp include/init.h src/Automaton.cpp include/Automaton.h)