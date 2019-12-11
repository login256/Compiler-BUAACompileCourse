//
// Created by lyt on 2019/12/11.
//

#ifndef UNIVERSALCOMPILER_FLOWGRAPH_H
#define UNIVERSALCOMPILER_FLOWGRAPH_H

#include <vector>
#include <memory>
#include <set>
#include "IR.h"

namespace ucc
{
	class MyLess
	{
		bool operator()(const std::shared_ptr<Var> &fi_t, const std::shared_ptr<Var> &se_t);
	};
	class Block
	{
	public:
		std::shared_ptr<Block> next;
		std::vector<std::shared_ptr<Block>> can_arrive;

		std::vector<std::shared_ptr<Var>> in;
		std::vector<std::shared_ptr<Var>> out;

		std::vector<std::shared_ptr<Var>> def;
		std::vector<std::shared_ptr<Var>> use;

		IrList ir_list;

		Block(const IrList::iterator &it_begin, const IrList::iterator &it_end);
		static bool is_same(std::shared_ptr<Var> fi_t, std::shared_ptr<Var> se_t);
		void add_def(std::shared_ptr<Var> var);
		void add_use(std::shared_ptr<Var> var);

		void remove_common_exp();
	};

	class FlowGraph
	{
	public:
		std::vector<std::shared_ptr<Block>> blocks;
		FlowGraph(std::shared_ptr<IrList> ir_list);
	};

}

#endif //UNIVERSALCOMPILER_FLOWGRAPH_H
