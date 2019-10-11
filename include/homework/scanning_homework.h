//
// Created by lyt on 2019/9/24.
//

#ifndef UNIVERSALCOMPILER_SCANNING_HOMEWORK_H
#define UNIVERSALCOMPILER_SCANNING_HOMEWORK_H

#include "../Token.h"
#include "../Matcher.h"
#include <ostream>

void do_first_pass(const std::string &input_file, std::ostream &output_file_stream, ucc::Matcher &matcher);

void token_output(ucc::Token *, std::ostream &output_file_stream);

#endif //UNIVERSALCOMPILER_SCANNING_HOMEWORK_H
