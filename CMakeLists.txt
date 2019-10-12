cmake_minimum_required(VERSION 3.10)
project(UniversalCompiler)

set(CMAKE_CXX_STANDARD 11)

add_executable(UniversalCompiler main.cpp include/Token.h src/Lexer.cpp include/Lexer.h src/Token.cpp src/homework/scanning_homework.cpp include/homework/scanning_homework.h src/init.cpp include/init.h src/Automaton.cpp include/Automaton.h src/homework/syntactic_homework.cpp include/homework/syntactic_homework.h src/SymbolTable.cpp include/SymbolTable.h src/Parser.cpp include/Parser.h)