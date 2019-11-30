//
// Created by lyt on 2019/10/12.
//

#ifndef UNIVERSALCOMPILER_PARSER_H
#define UNIVERSALCOMPILER_PARSER_H

#include <queue>
#include <fstream>
#include <memory>
#include "Token.h"
#include "Lexer.h"
#include "SymbolTable.h"
#include "Error.h"
#include "IR.h"

namespace ucc
{
	enum SyntaxType
	{
		syntax_add, //<加法运算符>
		syntax_mul, //<乘法运算符>
		syntax_rel, //<关系运算符>
		syntax_char,    //<字符>
		syntax_string,  //<字符串>
		syntax_program, //<程序>
		syntax_const_declare, //<常量说明>
		syntax_const_define,    //<常量定义>
		syntax_uint,    //<无符号整数>
		syntax_int, //<整数>
		syntax_id, //<标识符>
		syntax_declare_header,  //<声明头部>
		syntax_var_declare,   //<变量说明>
		syntax_var_define,  //<变量定义>
		syntax_var_type,    //<类型标识符>
		syntax_func,    //<有返回值函数定义>
		syntax_void_func,   //<无返回值函数定义>
		syntax_compound,    //<复合语句>
		syntax_par_list,    //<参数表>
		syntax_main_func,   //<主函数>
		syntax_exp, //<表达式>
		syntax_term,    //<项>
		syntax_fact,    //<因子>
		syntax_state,    //<语句>
		syntax_ass_state,   //<赋值语句>
		syntax_con_state,   //<条件语句>
		syntax_con, //<条件>
		syntax_loop_state,  //<循环语句>
		syntax_step,    //<步长>
		syntax_func_call,   //<有返回值函数调用语句> <无返回值函数调用语句>
		syntax_par_value_list,  //<值参数表>
		syntax_state_list,  //<语句列>
		syntax_read_state,  //<读语句>
		syntax_write_state, //<写语句>
		syntax_return_state,    //<返回语句>
	};

	class TokenBuffer
	{
	private:
		std::deque<std::shared_ptr<Token>> que;
		Lexer &lexer;
		std::shared_ptr<Token> last;
	public:
		TokenBuffer(Lexer &lexer);

		std::shared_ptr<Token> at(int pos);

		std::shared_ptr<Token> front();

		std::shared_ptr<Token> pop();

		std::shared_ptr<Token> get_last() const;

		void back_up(const std::shared_ptr<Token> &token);
	};

	class GrammerOutputer
	{
	public:
		virtual void syntax_unit_output(ucc::SyntaxType type) = 0;

		virtual void syntax_func_fucking_output(bool is_void) = 0;

		virtual void token_output(std::shared_ptr<Token> token) = 0;
	};

	class DefaultGrammerOutputer : public GrammerOutputer
	{
	public:
		void syntax_unit_output(ucc::SyntaxType type) override;

		void syntax_func_fucking_output(bool is_void) override;

		void token_output(std::shared_ptr<Token> token) override;
	};

	class Parser
	{
	private:
		TokenBuffer buffer;

		GrammerOutputer &outputer;

		std::shared_ptr<SymbolTable> cur_symbol_table;

		std::map<std::string, std::vector<SymbolData>> func_args;
		std::map<std::string, std::vector<std::string>> func_args_id;

		void output(SyntaxType type);

		void output(std::shared_ptr<Token> token);

		void output_func(bool is_void);

		void get_next();

		IrOp parse_add(); //<加法运算符>
		IrOp parse_mul(); //<乘法运算符>
		IrOp parse_rel(); //<关系运算符>
		char parse_char();    //<字符>
		std::string parse_string();  //<字符串>
		void parse_program(); //<程序>
		void parse_const_declare(SymbolScope scope); //<常量说明>
		void parse_const_define(SymbolScope scope);    //<常量定义>
		int parse_uint();    //<无符号整数>
		int parse_int(); //<整数>
		std::string parse_id();

		std::string parse_declare_header();  //<声明头部>
		void parse_var_declare(SymbolScope scope);   //<变量说明>
		void parse_var_define(SymbolScope scope);  //<变量定义>
		SymbolData parse_var_type();    //<类型标识符>
		void parse_func();    //<有返回值函数定义>
		void parse_void_func();   //<无返回值函数定义>
		void parse_compound(bool new_symbol_table);    //<复合语句>
		void parse_par_list(std::vector<SymbolData> &par_list, std::vector<std::string> &par_id);    //<参数表>
		void parse_main_func();   //<主函数>
		SymbolData parse_exp(std::shared_ptr<Var> &var); //<表达式>
		void parse_term(std::shared_ptr<Var> &var);    //<项>
		void parse_fact(std::shared_ptr<Var> &var);    //<因子>
		void parse_state();    //<语句>
		void parse_ass_state();   //<赋值语句>
		void parse_con_state();   //<条件语句>
		void parse_con(std::shared_ptr<Var> &var); //<条件>
		void parse_loop_state();  //<循环语句>
		int parse_step();    //<步长>
		void parse_func_call(bool can_be_void = true);   //<有返回值函数调用语句> <无返回值函数调用语句>
		void parse_par_value_list(std::shared_ptr<IrCall> ir);  //<值参数表>
		void parse_state_list();  //<语句列>
		void parse_read_state();  //<读语句>
		void parse_write_state(); //<写语句>
		void parse_return_state();    //<返回语句>

		std::map<std::string, std::vector<SymbolData>> func_para;

		inline void must_and_error(TokenType token_type, ErrorType error_type, bool front);

		std::shared_ptr<IrList> ir_list;

	public:
		Parser(GrammerOutputer &outputer, Lexer &lexer);

		Parser(Lexer &lexer);

		void parse();

		std::shared_ptr<IrList> get_ir_list() const;

		const std::shared_ptr<SymbolTable> &get_cur_symbol_table() const;
	};
}
#endif //UNIVERSALCOMPILER_PARSER_H
