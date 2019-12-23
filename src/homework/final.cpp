//
// Created by lyt on 2019/11/16.
//

#include <fstream>
#include <iostream>
#include "include/FlowGraph.h"
#include "include/ToMips.h"
#include "include/homework/error_homework.h"
#include "include/homework/syntactic_homework.h"
#include "include/init.h"

using ucc::mips_output_stream;

namespace final_homework
{
	static int process_file(std::ifstream &input_stream, std::ofstream &output_stream, std::ofstream &log_stream, std::ofstream &pre_ir_stream,
							std::ofstream &back_ir_stream, std::ofstream &error_stream)
	{
		std::string input_file_name;
		input_file_name = "testfile.txt";
		std::string output_file_name;
		output_file_name = "mips.txt";
		std::string log_file_name;
		log_file_name = "output.txt";

		std::string pre_ir_file_name;
		pre_ir_file_name = "pre_ir.txt";
		std::string back_ir_file_name;
		back_ir_file_name = "back_ir.txt";


		std::string error_file_name;
		error_file_name = "error.txt";

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
		log_stream.open(log_file_name);
		if (!log_stream.is_open())
		{
			std::cerr << "Can't open log file!" << std::endl;
			return -1;
		}
		pre_ir_stream.open(pre_ir_file_name);
		if (!pre_ir_stream.is_open())
		{
			std::cerr << "Can't open pre_ir file!" << std::endl;
			return -1;
		}
		back_ir_stream.open(back_ir_file_name);
		if (!back_ir_stream.is_open())
		{
			std::cerr << "Can't open back_ir file!" << std::endl;
			return -1;
		}

		error_stream.open(error_file_name);
		if (!error_stream.is_open())
		{
			std::cerr << "Can't open error file!" << std::endl;
			return -1;
		}
		return 0;
	}

	int final_main()
	{
		std::ifstream input_stream;
		std::ofstream log_stream;
		std::ofstream pre_ir_stream;
		std::ofstream back_ir_stream;
		std::ofstream error_file_stream;
		if (process_file(input_stream, mips_output_stream, log_stream, pre_ir_stream, back_ir_stream, error_file_stream) != 0)
		{
			return -1;
		}
		std::string input_string((std::istreambuf_iterator<char>(input_stream)), std::istreambuf_iterator<char>());
		ucc::Lexer lexer(ucc::data::res_to_token, ucc::data::sym_to_token, input_string);
		//HomeworkGrammerOutputer homework_grammer_outputer(log_stream);
		auto *parser = new ucc::Parser(/*homework_grammer_outputer, */lexer);
		parser->parse();
		if (ucc::has_error())
		{
			ucc::print_errors(error_file_stream);
			return 0;
		}
		pre_ir_stream << *parser->get_ir_list();
		bool opt = true;
		if (opt)
		{
			//auto graph = std::make_shared<ucc::FlowGraph>(parser->get_ir_list());
			ucc::small_opt(parser->get_ir_list());
			back_ir_stream << *parser->get_ir_list();
			to_mips(parser->get_ir_list(), parser->get_cur_symbol_table());
		}
		else
		{
			to_mips(parser->get_ir_list(), parser->get_cur_symbol_table());
		}
		return 0;
	}
}