//
// Created by lyt on 2019/9/10.
//

#ifndef UNIVERSALCOMPILER_TOKEN_H
#define UNIVERSALCOMPILER_TOKEN_H

#include <string>

namespace ucc
{
	enum TokenType
	{
		token_number, // unsigned
		token_cstring, // "{32,33,35-126}*"
		token_cchar, // '+-*/a-zA-Z0-9'
		token_id, // identify
		token_lbrace, // {
		token_rbrace, // }
		token_lbrackets, // [
		token_rbrackets, // ]
		token_lpar, // (
		token_rpar, // )
		token_squote, // '
		token_dquote, // "
		token_semi, // ;
		token_comma, // ,
		token_plus, // +
		token_minus, // -
		token_mul, // *
		token_div, // /
		token_les, // <
		token_leq, // <=
		token_grt, // >
		token_geq, // >=
		token_neq, // !=
		token_eql, // ==
		token_ass, // =
		token_int, // int
		token_char, // char
		token_void,    // void
		token_const,    // const
		token_main,    // main
		token_scanf,    // scanf
		token_printf,    // printf
		token_if,        // if
		token_else,    // else
		token_while,    // while
		token_for,        // for
		token_do,        // do
		token_return,    // return
		token_eof        // end of file
	};

	class Token
	{
	private:
		TokenType type;
	public:
		explicit Token(TokenType type);

		void set_type(TokenType type);

		TokenType get_type() const;

	};

	class NumToken : public Token
	{
	private:
		int value;
	public:
		NumToken(int value);

		int get_value() const;

		void set_value(int value);
	};

	class CharToken : public Token
	{
	private:
		std::string value;
	public:
		CharToken(std::string value);

		const std::string &get_value() const;

		void set_value(const std::string &value);
	};

	class IdToken : public Token
	{
	private:
		std::string value;
	public:
		IdToken(std::string value);

		const std::string &get_value() const;

		void set_value(const std::string &value);
	};

	class StringToken : public Token
	{
	private:
		std::string value;
	public:
		StringToken(std::string value);

		const std::string &get_value() const;

		void set_value(const std::string &value);
	};

	class NormalToken : public Token
	{
	private:
		std::string value;
	public:
		NormalToken(TokenType token_type, std::string value);

		const std::string &get_value() const;

		void set_value(const std::string &value);
	};
}

#endif //UNIVERSALCOMPILER_TOKEN_H
