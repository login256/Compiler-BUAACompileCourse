//
// Created by lyt on 2019/9/12.
//

#include <cctype>
#include <sstream>
#include "../include/Lexer.h"
#include <iostream>

namespace ucc
{
	Lexer::Lexer(
			const std::map<std::string, TokenType> &res_to_token,
			const std::map<std::string, TokenType> &sym_to_token,
			const std::string &input_file_string
	) :
			reserved_to_token(*(new auto(res_to_token))),
			symbol_to_token(*(new auto(sym_to_token))),
			input_string(*(new auto(input_file_string)))
	{

		cur_pos_it = input_string.cbegin();
		cur_line = 1;
		cur_col = 0;
	}

	Lexer::~Lexer()
	{
		delete (&reserved_to_token);
		delete (&symbol_to_token);
		delete (&input_string);
	}

	int Lexer::get_cur_pos() const
	{
		return cur_pos_it - input_string.cbegin();
	}

	int Lexer::get_cur_line() const
	{
		return cur_line;
	}

	int Lexer::get_cur_col() const
	{
		return cur_col;
	}

	static bool is_alpha(const char &c)
	{
		return isalpha(c) || c == '_';
	}

	static bool is_nozero_dig(const char &c)
	{
		return c > '0' && c <= '9';
	}

	static bool is_dig(const char &c)
	{
		return isdigit(c);
	}

	static bool is_char(const char &c)
	{
		return is_dig(c) || is_alpha(c) || c == '+' || c == '-' || c == '*' || c == '/';
	}

	inline void Lexer::next_pos()
	{
		if (*cur_pos_it == '\n')
		{
			cur_line++;
			cur_col = 0;
		}
		cur_col++;
		cur_pos_it++;
	}

	std::shared_ptr<Token> Lexer::get_next_token()
	{
		while (true)
		{
			if (cur_pos_it == input_string.cend())
			{
				return std::make_shared<Token>(TokenType::token_eof);
			}
			if (!std::isspace(*cur_pos_it))
			{
				break;
			}
			next_pos();
		}
		if (*cur_pos_it == '"')
		{
			auto begin_pos_it = cur_pos_it;
			next_pos();
			while (true)
			{
				if (cur_pos_it != input_string.cend() && *cur_pos_it == '"')
				{
					next_pos();
					break;
				}
				if (cur_pos_it == input_string.cend() ||
					!(*cur_pos_it == 32 || *cur_pos_it == 33 || (*cur_pos_it >= 33 && *cur_pos_it <= 126)))
				{
					std::cerr << "Wrong!1";
					while (1) {}
					//wrong
				}
				next_pos();
			}
			return std::make_shared<StringToken>(std::string(begin_pos_it + 1, cur_pos_it - 1));
		}
		else if (*cur_pos_it == '\'')
		{
			auto begin_pos_it = cur_pos_it;
			next_pos();
			if (cur_pos_it != input_string.cend() && is_char(*cur_pos_it))
			{
				next_pos();
			}
			else
			{
				//wrong
			}
			if (cur_pos_it != input_string.cend() && *cur_pos_it == '\'')
			{
				next_pos();
			}
			else
			{
				std::cerr << "Wrong!2";
				//wrong
			}
			return std::make_shared<CharToken>(std::string(begin_pos_it + 1, cur_pos_it - 1));
		}
		else if (is_alpha(*cur_pos_it))
		{
			auto begin_pos_it = cur_pos_it;
			next_pos();
			while (true)
			{
				if (cur_pos_it == input_string.cend() || !(is_alpha(*cur_pos_it) || is_dig(*cur_pos_it)))
				{
					break;
				}
				next_pos();
			}
			std::string str(begin_pos_it, cur_pos_it);
			//std::cout << "// " << str << std::endl;
			if (reserved_to_token.find(str) != reserved_to_token.end())
			{
				return std::make_shared<NormalToken>(reserved_to_token.find(str)->second, str);
			}
			else
			{
				return std::make_shared<IdToken>(str);
			}
		}
		else if (is_nozero_dig(*cur_pos_it))
		{
			auto begin_pos_it = cur_pos_it;
			next_pos();
			while (true)
			{
				if ( cur_pos_it == input_string.cend() || !is_dig(*cur_pos_it))
				{
					break;
				}
				next_pos();
			}
			std::string str(begin_pos_it, cur_pos_it);
			std::istringstream ist(str);
			int v;
			ist >> v;
			return std::make_shared<NumToken>(v);
		}
		else if (*cur_pos_it == '0')
		{
			auto begin_pos_it = cur_pos_it;
			next_pos();
			std::string str(begin_pos_it, cur_pos_it);
			std::istringstream ist(str);
			int v;
			ist >> v;
			return std::make_shared<NumToken>(v);
		}
		else
		{
			auto begin_pos_it = cur_pos_it;
			next_pos();
			if (cur_pos_it != input_string.cend() &&
				symbol_to_token.find(std::string(begin_pos_it, cur_pos_it + 1)) != symbol_to_token.end())
			{
				next_pos();
				std::string str(std::string(begin_pos_it, cur_pos_it));
				return std::make_shared<NormalToken>(symbol_to_token.find(str)->second, str);
			}
			else if (symbol_to_token.find(std::string(begin_pos_it, cur_pos_it)) != symbol_to_token.end())
			{
				std::string str(std::string(begin_pos_it, cur_pos_it));
				return std::make_shared<NormalToken>(symbol_to_token.find(str)->second, str);
			}
		}
		std::cerr << "Wrong!3";
		while (1) {}
		//wrong
	}

	bool Lexer::is_eof()
	{
		return cur_pos_it == input_string.cend();
	}

}
