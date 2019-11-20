//
// Created by lyt on 2019/9/12.
//

#ifndef UNIVERSALCOMPILER_LEXER_H
#define UNIVERSALCOMPILER_LEXER_H

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <memory>
#include "Token.h"

namespace ucc
{
	class Lexer
	{

	private:
		const std::map<std::string, TokenType> &reserved_to_token;
		const std::map<std::string, TokenType> &symbol_to_token;
		const std::string &input_string;

		std::string::const_iterator cur_pos_it;
		int cur_line;
		int cur_col;
		inline void next_pos();

	public:
		Lexer(
				const std::map<std::string, TokenType> &res_to_token,
				const std::map<std::string, TokenType> &sym_to_token,
				const std::string &input_file_string
		);

		virtual ~Lexer();

		int get_cur_pos() const;

		int get_cur_line() const;

		int get_cur_col() const;

		std::shared_ptr<Token> get_next_token();

		bool is_eof();

	};
}

#endif //UNIVERSALCOMPILER_LEXER_H
