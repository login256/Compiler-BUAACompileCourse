//
// Created by lyt on 2019/11/2.
//

#ifndef UNIVERSALCOMPILER_ERROR_HOMEWORK_H
#define UNIVERSALCOMPILER_ERROR_HOMEWORK_H

#include <map>
#include <string>
#include <fstream>

namespace ucc
{
	enum ErrorType
	{
		bad_char,    //"a"
		id_redefine,    //"b"
		undefined_id,    //"c"
		func_arg_bad,    //"d"
		func_type_bad,    //"e"
		bad_in_cond,    //"f"
		void_bad_return,    //"g"
		no_bad_return,    //"h"
		index_bad,    //"i"
		const_var_changed,    //"j"
		should_be_semi,    //"k"
		should_be_rpar,    //"l"
		should_be_rbac,    //"m"
		no_while,    //"n"
		const_define_bad,    //"o"
		other_error,    //"?"
	};
	struct Error
	{
		ErrorType error_type;
		int line;
		int col;

		Error(ErrorType error_type, int line);
	};

	bool operator<(const Error &fi, const Error &se);

	std::ofstream &operator<<(std::ofstream &os, const Error &error);

	void error_handler(Error error);

	void print_errors(std::ofstream &error_file);
}


#endif //UNIVERSALCOMPILER_ERROR_HOMEWORK_H
