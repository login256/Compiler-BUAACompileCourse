//
// Created by lyt on 2019/10/12.
//

#include <queue>
#include <memory>
#include "../include/Parser.h"
#include "../include/Token.h"

namespace ucc
{
	TokenBuffer::TokenBuffer(Lexer &lexer) : lexer(lexer)
	{}

	std::shared_ptr<Token> TokenBuffer::get_next()
	{
		if (que.empty())
		{
			que.push_back(lexer.get_next_token());
		}
		auto re = que.front();
		que.pop_front();
		return re;
	}

	void TokenBuffer::back_up(const std::shared_ptr<Token> &token)
	{
		que.push_front(token);
	}
}