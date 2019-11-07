//
// Created by lyt on 2019/11/2.
//

#ifndef UNIVERSALCOMPILER_ERROR_HOMEWORK_H
#define UNIVERSALCOMPILER_ERROR_HOMEWORK_H

#include <map>
#include <string>
#include <vector>
#include <stack>
#include "../SymbolTable.h"
#include "../Error.h"

namespace ucc
{
	class ErrorData
	{
	public:
		static std::stack<bool> is_char_exp;
		static bool has_void_return;
		static bool has_char_return;
		static bool has_int_return;
		static std::vector<SymbolData> cur_func_args;
		static SymbolData cur_func_data_type;
	};

	std::ofstream &operator<<(std::ofstream &os, const Error &error);

	void error_handler(Error error);

	void print_errors(std::ofstream &error_file);
}

int error_main();


#endif //UNIVERSALCOMPILER_ERROR_HOMEWORK_H
