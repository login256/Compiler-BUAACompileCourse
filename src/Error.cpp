//
// Created by lyt on 2019/11/7.
//

#include "../include/Error.h"

namespace ucc
{
	bool operator<(const Error &fi, const Error &se)
	{
		return fi.line < se.line;
	}

	Error::Error(ErrorType error_type, int line) : error_type(error_type), line(line) {}
}