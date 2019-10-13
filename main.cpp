#include <iostream>
#include <fstream>
#include <string>
#include "include/Lexer.h"
#include "include/init.h"
#include "include/homework/scanning_homework.h"

int process_file(std::ifstream &input_stream, std::ofstream &output_stream)
{
	std::string input_file_name;
	input_file_name = "testfile.txt";
	std::string output_file_name;
	output_file_name = "output.txt";
	input_stream.open(input_file_name);
	if (!input_stream.is_open())
	{
		std::cerr << "Can't open input file!" << std::endl;
		return -1;
	}
	output_stream.open(output_file_name);
	if (!output_stream.is_open())
	{
		std::cerr << "Can't open output file!" << std::endl;
		return -1;
	}
	return 0;
}


int main(int argc, char **args)
{
	std::ifstream input_stream;
	std::ofstream output_stream;
	if (process_file(input_stream, output_stream) != 0)
	{
		return -1;
	}
	std::string input_string((std::istreambuf_iterator<char>(input_stream)), std::istreambuf_iterator<char>());
	ucc::Lexer lexer(ucc::data::res_to_token, ucc::data::sym_to_token, input_string);
	return 0;
}