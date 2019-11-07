//
// Created by lyt on 2019/11/2.
//

#include "../../include/homework/error_homework.h"
#include "../../include/Lexer.h"
#include "../../include/init.h"
#include <iostream>
#include "../../include/homework/syntactic_homework.h"

#include <set>

using namespace ucc;


std::stack<bool> ErrorData::is_char_exp;
bool ErrorData::has_void_return;
bool ErrorData::has_char_return;
bool ErrorData::has_int_return;
std::vector<SymbolData> ErrorData::cur_func_args;
SymbolData ErrorData::cur_func_data_type;

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
		{ErrorType::should_be_rbrac,   "m"},
		{ErrorType::no_while,          "n"},
		{ErrorType::const_define_bad,  "o"},
		{ErrorType::other_error,       "?"},
};

static std::set<Error> errors;

class res_to_token;

std::ofstream &ucc::operator<<(std::ofstream &os, const Error &error)
{
	os << error.line << " " << ErrorToString[error.error_type];
	return os;
}

void ucc::error_handler(Error error)
{
	errors.insert(std::move(error));
}

void ucc::print_errors(std::ofstream &error_file)
{
	for (auto e : errors)
	{
		error_file << e << std::endl;
	}
}


static int process_file(std::ifstream &input_stream, std::ofstream &error_file_stream)
{
	std::string input_file_name;
	input_file_name = "testfile.txt";
	std::string output_file_name;
	output_file_name = "error.txt";
	input_stream.open(input_file_name);
	if (!input_stream.is_open())
	{
		std::cerr << "Can't open input file!" << std::endl;
		return -1;
	}
	error_file_stream.open(output_file_name);
	if (!error_file_stream.is_open())
	{
		std::cerr << "Can't open output file!" << std::endl;
		return -1;
	}
	return 0;
}


int error_main()
{
	std::ifstream input_stream;
	std::ofstream error_file_stream;
	if (process_file(input_stream, error_file_stream) != 0)
	{
		return -1;
	}
	std::string input_string((std::istreambuf_iterator<char>(input_stream)), std::istreambuf_iterator<char>());
	ucc::Lexer lexer(ucc::data::res_to_token, ucc::data::sym_to_token, input_string);


	std::string output_file_name;
	output_file_name = "output.txt";
	std::ofstream output(output_file_name);

	HomeworkGrammerOutputer homework_grammer_outputer(output);
	ucc::Parser parser(homework_grammer_outputer, lexer);
	parser.parse();
	print_errors(error_file_stream);
	return 0;
}

