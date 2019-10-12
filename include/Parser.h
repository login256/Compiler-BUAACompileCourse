//
// Created by lyt on 2019/10/12.
//

#ifndef UNIVERSALCOMPILER_PARSER_H
#define UNIVERSALCOMPILER_PARSER_H

#include "Token.h"
#include "Lexer.h"

namespace ucc
{
	class TokenBuffer
	{
	private:
		std::deque<std::shared_ptr<Token>> que;
		Lexer &lexer;
	public:
		TokenBuffer(Lexer &lexer);

		std::shared_ptr<Token> get_next();

		void back_up(const std::shared_ptr<Token> &token);
	};
}
#endif //UNIVERSALCOMPILER_PARSER_H
