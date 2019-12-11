cmake_minimum_required(VERSION 3.10)
project(UniversalCompiler)

set(CMAKE_CXX_STANDARD 11)

include_directories(./)

add_executable(UniversalCompiler main.cpp include/Token.h src/Lexer.cpp include/Lexer.h src/Token.cpp src/homework/scanning_homework.cpp include/homework/scanning_homework.h src/init.cpp include/init.h src/Automaton.cpp include/Automaton.h src/homework/syntactic_homework.cpp include/homework/syntactic_homework.h src/SymbolTable.cpp include/SymbolTable.h src/Parser.cpp include/Parser.h src/homework/error_homework.cpp include/homework/error_homework.h src/Error.cpp include/Error.h src/AST.cpp include/AST.h src/IR.cpp include/IR.h src/homework/generate_homework.cpp include/homework/generate_homework.h src/ToMips.cpp include/ToMips.h src/FlowGraph.cpp include/FlowGraph.h)