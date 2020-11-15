//
// Created by lyt on 2019/9/12.
//

#include "../include/Token.h"

namespace ucc
{
	Token::Token(TokenType type, int line) : type(type), line(line) {}

	TokenType Token::get_type() const
	{
		return type;
	}

	int Token::get_line() const
	{
		return line;
	}

	NumToken::NumToken(int line, int value) : Token(TokenType::token_number, line), value(value) {}

	int NumToken::get_value() const
	{
		return value;
	}

	void NumToken::set_value(int value)
	{
		NumToken::value = value;
	}

	CharToken::CharToken(int line, std::string value) : Token(TokenType::token_cchar, line)
	{
		CharToken::value = std::move(value);
	}

	const std::string &CharToken::get_value() const
	{
		return value;
	}

	void CharToken::set_value(const std::string &value)
	{
		CharToken::value = value;
	}

	IdToken::IdToken(int line, std::string value) : Token(TokenType::token_id, line)
	{
		IdToken::value = std::move(value);
	}

	const std::string &IdToken::get_value() const
	{
		return value;
	}

	void IdToken::set_value(const std::string &value)
	{
		IdToken::value = value;
	}

	StringToken::StringToken(int line, std::string value) : Token(TokenType::token_cstring, line)
	{
		StringToken::value = std::move(value);
	}

	const std::string &StringToken::get_value() const
	{
		return value;
	}

	void StringToken::set_value(const std::string &value)
	{
		StringToken::value = value;
	}

	NormalToken::NormalToken(TokenType token_type, int line, std::string value) : Token(token_type, line)
	{
		NormalToken::value = std::move(value);
	}

	const std::string &NormalToken::get_value() const
	{
		return value;
	}

	void NormalToken::set_value(const std::string &value)
	{
		NormalToken::value = value;
	}
}
