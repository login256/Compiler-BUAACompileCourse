//
// Created by lyt on 2019/9/24.
//

#include "include/init.h"
#include <map>

namespace ucc
{
	namespace data
	{
		std::map<std::string, TokenType> res_to_token(
				{
						{"int",    TokenType::token_int},
						{"char",   TokenType::token_char},
						{"void",   TokenType::token_void},
						{"const",  TokenType::token_const},
						{"main",   TokenType::token_main},
						{"scanf",  TokenType::token_scanf},
						{"printf", TokenType::token_printf},
						{"if",     TokenType::token_if},
						{"else",   TokenType::token_else},
						{"while",  TokenType::token_while},
						{"for",    TokenType::token_for},
						{"do",     TokenType::token_do},
						{"return", TokenType::token_return}
				});
		std::map<std::string, TokenType> sym_to_token(
				{
						{"{",  TokenType::token_lbrace},
						{"}",  TokenType::token_rbrace},
						{"[",  TokenType::token_lbrackets},
						{"]",  TokenType::token_rbrackets},
						{"(",  TokenType::token_lpar},
						{")",  TokenType::token_rpar},
						{"'",  TokenType::token_squote},
						{"\"", TokenType::token_dquote},
						{";",  TokenType::token_semi},
						{",",  TokenType::token_comma},
						{"+",  TokenType::token_plus},
						{"-",  TokenType::token_minus},
						{"*",  TokenType::token_mul},
						{"/",  TokenType::token_div},
						{"<",  TokenType::token_les},
						{"<=", TokenType::token_leq},
						{">",  TokenType::token_grt},
						{">=", TokenType::token_geq},
						{"!=", TokenType::token_neq},
						{"==", TokenType::token_eql},
						{"=",  TokenType::token_ass},
				});

		void init_automaton(Automaton &automaton)
		{
		}
	}
}