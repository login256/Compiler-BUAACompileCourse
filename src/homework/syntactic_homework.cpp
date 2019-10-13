//
// Created by lyt on 2019/10/12.
//

#include "../../include/homework/syntactic_homework.h"
#include "../../include/homework/scanning_homework.h"
#include "../../include/Parser.h"

using namespace ucc;

std::map<SyntaxType, std::string> HomeworkGrammerOutputer::type_to_output = {
//		{SyntaxType::syntax_add,            "<加法运算符>"},
//		{SyntaxType::syntax_mul,            "<乘法运算符>"},
//		{SyntaxType::syntax_rel,            "<关系运算符>"},
//		{SyntaxType::syntax_char,           "<字符>"},
		{SyntaxType::syntax_string,         "<字符串>"},
		{SyntaxType::syntax_program,        "<程序>"},
		{SyntaxType::syntax_const_declare,  "<常量说明>"},
		{SyntaxType::syntax_const_define,   "<常量定义>"},
		{SyntaxType::syntax_uint,           "<无符号整数>"},
		{SyntaxType::syntax_int,            "<整数>"},
//		{SyntaxType::syntax_id,             "<标识符>"},
		{SyntaxType::syntax_declare_header, "<声明头部>"},
		{SyntaxType::syntax_var_declare,    "<变量说明>"},
		{SyntaxType::syntax_var_define,     "<变量定义>"},
//		{SyntaxType::syntax_var_type,       "<类型标识符>"},
		{SyntaxType::syntax_func,           "<有返回值函数定义>"},
		{SyntaxType::syntax_void_func,      "<无返回值函数定义>"},
		{SyntaxType::syntax_compound,       "<复合语句>"},
		{SyntaxType::syntax_par_list,       "<参数表>"},
		{SyntaxType::syntax_main_func,      "<主函数>"},
		{SyntaxType::syntax_exp,            "<表达式>"},
		{SyntaxType::syntax_term,           "<项>"},
		{SyntaxType::syntax_fact,           "<因子>"},
		{SyntaxType::syntax_state,          "<语句>"},
		{SyntaxType::syntax_ass_state,      "<赋值语句>"},
		{SyntaxType::syntax_con_state,      "<条件语句>"},
		{SyntaxType::syntax_con,            "<条件>"},
		{SyntaxType::syntax_loop_state,     "<循环语句>"},
		{SyntaxType::syntax_step,           "<步长>"},
		{SyntaxType::syntax_func_call,      "<有返回值函数调用语句> <无返回值函数调用语句>"},
		{SyntaxType::syntax_par_value_list, "<值参数表>"},
		{SyntaxType::syntax_state_list,     "<语句列>"},
		{SyntaxType::syntax_read_state,     "<读语句>"},
		{SyntaxType::syntax_write_state,    "<写语句>"},
		{SyntaxType::syntax_return_state,   "<返回语句>"},
};

HomeworkGrammerOutputer::HomeworkGrammerOutputer(std::ostream &output_stream) : output_stream(output_stream)
{}

void HomeworkGrammerOutputer::syntax_unit_output(ucc::SyntaxType type)
{
	if (type_to_output.find(type) != type_to_output.end())
	{
		output_stream << type_to_output[type] << std::endl;
	}
}

void HomeworkGrammerOutputer::syntax_func_fucking_output(bool is_void)
{
	if (is_void)
	{
		output_stream << "<有返回值函数调用语句>" << std::endl;
	}
	else
	{
		output_stream << "<无返回值函数调用语句>" << std::endl;
	}
}

void HomeworkGrammerOutputer::token_output(std::shared_ptr<Token> token)
{
	::token_output(token, output_stream);
}
