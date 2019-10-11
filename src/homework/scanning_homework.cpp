//
// Created by lyt on 2019/9/24.
//
#include <string>
#include "../../include/homework/scanning_homework.h"
#include "../../include/Matcher.h"

using namespace ucc;

static std::map<int, std::string> token_map = {{TokenType::token_const,     "CONSTTK"},
											   {TokenType::token_int,       "INTTK"},
											   {TokenType::token_char,      "CHARTK"},
											   {TokenType::token_void,      "VOIDTK"},
											   {TokenType::token_main,      "MAINTK"},
											   {TokenType::token_if,        "IFTK"},
											   {TokenType::token_else,      "ELSETK"},
											   {TokenType::token_do,        "DOTK"},
											   {TokenType::token_while,     "WHILETK"},
											   {TokenType::token_for,       "FORTK"},
											   {TokenType::token_scanf,     "SCANFTK"},
											   {TokenType::token_printf,    "PRINTFTK"},
											   {TokenType::token_return,    "RETURNTK"},
											   {TokenType::token_plus,      "PLUS"},
											   {TokenType::token_minus,     "MINU"},
											   {TokenType::token_mul,       "MULT"},
											   {TokenType::token_div,       "DIV"},
											   {TokenType::token_les,       "LSS"},
											   {TokenType::token_leq,       "LEQ"},
											   {TokenType::token_grt,       "GRE"},
											   {TokenType::token_geq,       "GEQ"},
											   {TokenType::token_eql,       "EQL"},
											   {TokenType::token_neq,       "NEQ"},
											   {TokenType::token_ass,       "ASSIGN"},
											   {TokenType::token_semi,      "SEMICN"},
											   {TokenType::token_comma,     "COMMA"},
											   {TokenType::token_lpar,      "LPARENT"},
											   {TokenType::token_rpar,      "RPARENT"},
											   {TokenType::token_lbrackets, "LBRACK"},
											   {TokenType::token_rbrackets, "RBRACK"},
											   {TokenType::token_lbrace,    "LBRACE"},
											   {TokenType::token_rbrace,    "RBRACE"}
};

void do_first_pass(const std::string &input_file, std::ostream &output_file_stream, ucc::Matcher &matcher)
{
	while (!matcher.is_eof())
	{
		Token *cur_token = matcher.get_next_token();
		token_output(cur_token, output_file_stream);
		delete (cur_token);
	}
}


void token_output(ucc::Token *token, std::ostream &output_file_stream)
{
	switch (token->get_type())
	{
		case TokenType::token_eof:
			return;
		case TokenType::token_id :
			output_file_stream << "IDENFR " << ((IdToken *) token)->get_value() << std::endl;
			break;
		case TokenType::token_number :
			output_file_stream << "INTCON " << ((NumToken *) token)->get_value() << std::endl;
			break;
		case TokenType::token_cchar :
			output_file_stream << "CHARCON " << ((CharToken *) token)->get_value() << std::endl;
			break;
		case TokenType::token_cstring :
			output_file_stream << "STRCON " << ((StringToken *) token)->get_value() << std::endl;
			break;
		default:
			output_file_stream << token_map[token->get_type()] << " " << ((NormalToken *) token)->get_value() << std::endl;
	}
}