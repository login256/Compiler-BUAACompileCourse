//
// Created by lyt on 2019/9/27.
//

#include "../include/Automaton.h"

namespace ucc
{
	void Automaton::add_edge(int u, char c, int v)
	{
		check_node(u);
		check_node(v);
		id_to_node[u]->next[c] = id_to_node[v];
	}

	Automaton::node *Automaton::check_node(int id)
	{
		if (id_to_node.find(id) == id_to_node.end())
		{
			id_to_node[id] = new node;
			id_to_node[id]->id = id;
		}
		return id_to_node[id];
	}

	void Automaton::set_start()
	{
		cur_node = root;
	}

	Automaton::node *Automaton::walk(const char c)
	{
		if (cur_node == nullptr)
		{
			return nullptr;
		}
		if (cur_node->next.find(c) != cur_node->next.end())
		{
			cur_node = cur_node->next[c];
		}
		else
		{
			cur_node = nullptr;
		}
		return cur_node;
	}

	Automaton::Automaton()
	{
		root = check_node(0);
		cur_node = nullptr;
	}

	Automaton::~Automaton()
	{
		for (auto e : id_to_node)
		{
			delete (e.second);
		}
	}

}