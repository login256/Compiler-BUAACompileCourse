//
// Created by lyt on 2019/9/12.
//

#ifndef UNIVERSALCOMPILER_MATCHER_H
#define UNIVERSALCOMPILER_MATCHER_H

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include "Token.h"

namespace ucc
{
	class Matcher
	{

	private:
		const std::map<std::string, TokenType> &reserved_to_token;
		const std::map<std::string, TokenType> &symbol_to_token;
		const std::string &input_string;

		std::string::const_iterator cur_pos_it;
		int cur_line;
		int cur_col;

	public:
		Matcher(
				const std::map<std::string, TokenType> &res_to_token,
				const std::map<std::string, TokenType> &sym_to_token,
				const std::string &input_file_string
		);

		virtual ~Matcher();

		int get_cur_pos() const;

		int get_cur_line() const;

		int get_cur_col() const;

		Token *get_next_token();

		bool is_eof();

	};
}

#endif //UNIVERSALCOMPILER_MATCHER_H
