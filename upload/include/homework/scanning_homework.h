//
// Created by lyt on 2019/9/24.
//

#ifndef UNIVERSALCOMPILER_SCANNING_HOMEWORK_H
#define UNIVERSALCOMPILER_SCANNING_HOMEWORK_H

#include "../Token.h"
#include "../Lexer.h"
#include <ostream>

void do_first_pass(std::ostream &output_file_stream, ucc::Lexer &lexer);

void token_output(const std::shared_ptr<ucc::Token> &token, std::ostream &output_file_stream);

#endif //UNIVERSALCOMPILER_SCANNING_HOMEWORK_H
