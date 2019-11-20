//
// Created by lyt on 2019/9/27.
//

#ifndef UNIVERSALCOMPILER_AUTOMATON_H
#define UNIVERSALCOMPILER_AUTOMATON_H

#include <map>
#include "Token.h"

namespace ucc
{
	class Automaton
	{
	public:
		struct node
		{
			int id;
			std::map<int, node *> next;
			bool can_end = 0;
			TokenType token_type;
		};

		void add_edge(int u, char c, int v);

		Automaton();

		~Automaton();

		void set_start();

		node *walk(const char c);

		node *root;
		node *cur_node;

	private:
		node *check_node(int id);
		std::map<int, node *> id_to_node;
	};
}


#endif //UNIVERSALCOMPILER_AUTOMATON_H
