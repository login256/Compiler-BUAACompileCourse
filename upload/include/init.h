//
// Created by lyt on 2019/9/24.
//

#ifndef UNIVERSALCOMPILER_INIT_H
#define UNIVERSALCOMPILER_INIT_H

#include "Lexer.h"
#include "Automaton.h"

namespace ucc
{
	namespace data
	{
		extern std::map<std::string, TokenType> res_to_token;
		extern std::map<std::string, TokenType> sym_to_token;
		void init_automaton(Automaton &automaton);
	}
}

#endif //UNIVERSALCOMPILER_INIT_H
