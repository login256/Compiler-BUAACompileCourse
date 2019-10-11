//
// Created by lyt on 2019/9/12.
//

#include "../include/Token.h"

namespace ucc
{
	Token::Token(TokenType type) : type(type) {}

	void Token::set_type(TokenType type)
	{
		Token::type = type;
	}


	TokenType Token::get_type() const
	{
		return type;
	}

	NumToken::NumToken(int value) : Token(TokenType::token_number), value(value) {}

	int NumToken::get_value() const
	{
		return value;
	}

	void NumToken::set_value(int value)
	{
		NumToken::value = value;
	}

	CharToken::CharToken(std::string value) : Token(TokenType::token_cchar)
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

	IdToken::IdToken(std::string value) : Token(TokenType::token_id)
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

	StringToken::StringToken(std::string value) : Token(TokenType::token_cstring)
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

	NormalToken::NormalToken(TokenType token_type, std::string value) : Token(token_type)
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
