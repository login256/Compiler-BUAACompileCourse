//
// Created by lyt on 2019/11/2.
//

#include "../../include/homework/error_homework.h"

#include <set>

using namespace ucc;

static std::map<ErrorType, std::string> ErrorToString = {
		{ErrorType::bad_char,          "a"},
		{ErrorType::id_redefine,       "b"},
		{ErrorType::undefined_id,      "c"},
		{ErrorType::func_arg_bad,      "d"},
		{ErrorType::func_type_bad,     "e"},
		{ErrorType::bad_in_cond,       "f"},
		{ErrorType::void_bad_return,   "g"},
		{ErrorType::no_bad_return,     "h"},
		{ErrorType::index_bad,         "i"},
		{ErrorType::const_var_changed, "j"},
		{ErrorType::should_be_semi,    "k"},
		{ErrorType::should_be_rpar,    "l"},
		{ErrorType::should_be_rbac,    "m"},
		{ErrorType::no_while,          "n"},
		{ErrorType::const_define_bad,  "o"},
		{ErrorType::other_error,       "?"},
};

bool ucc::operator<(const Error &fi, const Error &se)
{
	return fi.line < se.line;
}

std::ofstream &ucc::operator<<(std::ofstream &os, const Error &error)
{
	os << error.line << " " << ErrorToString[error.error_type];
	return os;
}

static std::set<Error> errors;

void ucc::error_handler(const Error &error)
{
	errors.insert(error);
}

void ucc::print_errors(std::ofstream &error_file)
{
	for (auto e : errors)
	{
		error_file << e;
	}
}
