//
// Created by lyt on 2019/11/7.
//

#ifndef UNIVERSALCOMPILER_ERROR_H
#define UNIVERSALCOMPILER_ERROR_H

#include <fstream>
#include <vector>
#include "SymbolTable.h"

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
		should_be_rbrac,    //"m"
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
}

#endif //UNIVERSALCOMPILER_ERROR_H
