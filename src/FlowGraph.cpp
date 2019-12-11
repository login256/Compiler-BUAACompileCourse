//
// Created by lyt on 2019/12/11.
//

#include <map>
#include "include/FlowGraph.h"

namespace ucc
{
	bool Block::is_same(std::shared_ptr<Var> fi_t, std::shared_ptr<Var> se_t)
	{
		if (fi_t->var_type != se_t->var_type)
		{
			return false;
		}
		if (fi_t->var_type == VarType::var_normal)
		{
			auto fi = std::static_pointer_cast<NorVar>(fi_t);
			auto se = std::static_pointer_cast<NorVar>(se_t);
			return fi->table_entry == se->table_entry;
		}
		else if (fi_t->var_type == VarType::var_temp)
		{
			auto fi = std::static_pointer_cast<TempVar>(fi_t);
			auto se = std::static_pointer_cast<TempVar>(se_t);
			return fi->id == se->id;
		}
		else if (fi_t->var_type == VarType::var_const)
		{
			auto fi = std::static_pointer_cast<ConstVar>(fi_t);
			auto se = std::static_pointer_cast<ConstVar>(se_t);
			return fi->value == se->value;
		}
			/*
			else if (fi_t->var_type == VarType::var_array)
			{
				auto fi = std::static_pointer_cast<ArrayVar>(fi_t);
				auto se = std::static_pointer_cast<ArrayVar>(se_t);
				return fi->table_entry == se->table_entry && is_same(fi->index, se->index);
			}
			*/
		else
		{
			return false;
		}
	}

	void Block::add_def(std::shared_ptr<Var> var)
	{
		if (var->var_type == VarType::var_ret)
		{
			return;
		}
		if (var->var_type == VarType::var_array)
		{
			auto var_t = std::static_pointer_cast<ArrayVar>(var);
			add_use(var_t->index);
		}
		for (auto v : use)
		{
			if (is_same(var, v))
			{
				return;
			}
		}
		for (auto v : def)
		{
			if (is_same(var, v))
			{
				return;
			}
		}
		def.push_back(var);
	}

	void Block::add_use(std::shared_ptr<Var> var)
	{
		if (var->var_type == VarType::var_ret)
		{
			return;
		}
		if (var->var_type == VarType::var_array)
		{
			auto var_t = std::static_pointer_cast<ArrayVar>(var);
			var = var_t->index;
		}
		for (auto v : def)
		{
			if (is_same(var, v))
			{
				return;
			}
		}
		for (auto v : use)
		{
			if (is_same(var, v))
			{
				return;
			}
		}
		use.push_back(var);
	}

	Block::Block(const IrList::iterator &it_begin, const IrList::iterator &it_end)
	{
		for (auto it = it_begin; it != it_end; it++)
		{
			ir_list.push_back(*it);
		}
		for (auto it = ir_list.begin(); it != ir_list.end(); it++)
		{
			auto code_t = *it;
			switch (code_t->ir_type)
			{
				case ir_assign:
				{
					auto code = std::static_pointer_cast<IrAssign>(code_t);
					add_use(code->l);
					if (code->r)
					{
						add_use(code->r);
					}
					add_def(code->aim);
					break;
				}
				case ir_call:
				{
					auto code = std::static_pointer_cast<IrCall>(code_t);
					for (auto var : code->vars)
					{
						add_use(var);
					}
					break;
				}
				case ir_ret:
				{
					auto code = std::static_pointer_cast<IrRet>(code_t);
					if (!code->is_void)
					{
						add_use(code->var);
					}
					break;
				}
				case ir_branch:
				{
					auto code = std::static_pointer_cast<IrBranch>(code_t);
					add_use(code->var);
					break;
				}
				case ir_jump:
				{
					auto code = std::static_pointer_cast<IrJump>(code_t);
					break;
				}
				case ir_label:
				{
					auto code = std::static_pointer_cast<IrLable>(code_t);
					break;
				}
				case ir_func:
				{
					auto code = std::static_pointer_cast<IrFunc>(code_t);
					for (auto var_s : *(code->par_list))
					{
						add_def(std::make_shared<NorVar>(code->symbol_table->find(var_s)));
					}
					break;
				}
				case ir_write:
				{
					auto code = std::static_pointer_cast<IrWrite>(code_t);
					add_use(code->var);
					break;
				}
				case ir_read:
				{
					auto code = std::static_pointer_cast<IrRead>(code_t);
					add_def(code->var);
					break;
				}
				case ir_func_end:
				{
					auto code = std::static_pointer_cast<IrFuncEnd>(code_t);
					break;
				}
			}
		}
	}

	/*
	static std::vector<std::pair<std::shared_ptr<Var>, std::shared_ptr<Var>>>::iterator
	find(std::vector<std::pair<std::shared_ptr<Var>, std::shared_ptr<Var>>> &table, std::shared_ptr<Var> var)
	{
		for (auto it = table.begin(); it != table.end(); it++)
		{
			if (Block::is_same(it->first, var))
			{
				return it;
			}
		}
		return table.end();
	}
	*/

	bool MyLess::operator()(const std::shared_ptr<Var> &fi_t, const std::shared_ptr<Var> &se_t)
	{
		if (fi_t->var_type != se_t->var_type)
		{
			return fi_t->var_type < se_t->var_type;
		}
		if (fi_t->var_type == VarType::var_normal)
		{
			auto fi = std::static_pointer_cast<NorVar>(fi_t);
			auto se = std::static_pointer_cast<NorVar>(se_t);
			return fi->table_entry < se->table_entry;
		}
		else if (fi_t->var_type == VarType::var_temp)
		{
			auto fi = std::static_pointer_cast<TempVar>(fi_t);
			auto se = std::static_pointer_cast<TempVar>(se_t);
			return fi->id < se->id;
		}
		else if (fi_t->var_type == VarType::var_const)
		{
			auto fi = std::static_pointer_cast<ConstVar>(fi_t);
			auto se = std::static_pointer_cast<ConstVar>(se_t);
			return fi->value < se->value;
		}
		else
		{
			return false;
		}
	}

	static void correct_var(std::shared_ptr<Var> &var, std::map<std::shared_ptr<Var>, std::shared_ptr<Var>> &table)
	{
		if (var->var_type == VarType::var_ret)
		{
			return;
		}
		if (var->var_type == VarType::var_array)
		{
			auto var_t = std::static_pointer_cast<ArrayVar>(var);
			correct_var(var_t->index, table);
			return;
		}
		if (table.find(var) != table.end())
		{
			var = table[var];
		}
	}

	void Block::remove_common_exp()
	{
		std::map<std::shared_ptr<Var>, std::shared_ptr<Var>> table;
		for (auto it = ir_list.begin(); it != ir_list.end(); it++)
		{
			auto code_t = *it;
			switch (code_t->ir_type)
			{
				case ir_assign:
				{
					auto code = std::static_pointer_cast<IrAssign>(code_t);
					if (!code->r)
					{
						correct_var(code->l, table);
					}
					else
					{
						correct_var(code->l, table);
						correct_var(code->r, table);
					}
					add_def(code->aim);
					break;
				}
				case ir_call:
				{
					auto code = std::static_pointer_cast<IrCall>(code_t);
					for (auto var : code->vars)
					{
						add_use(var);
					}
					break;
				}
				case ir_ret:
				{
					auto code = std::static_pointer_cast<IrRet>(code_t);
					if (!code->is_void)
					{
						add_use(code->var);
					}
					break;
				}
				case ir_branch:
				{
					auto code = std::static_pointer_cast<IrBranch>(code_t);
					add_use(code->var);
					break;
				}
				case ir_jump:
				{
					auto code = std::static_pointer_cast<IrJump>(code_t);
					break;
				}
				case ir_label:
				{
					auto code = std::static_pointer_cast<IrLable>(code_t);
					break;
				}
				case ir_func:
				{
					auto code = std::static_pointer_cast<IrFunc>(code_t);
					for (auto var_s : *(code->par_list))
					{
						add_def(std::make_shared<NorVar>(code->symbol_table->find(var_s)));
					}
					break;
				}
				case ir_write:
				{
					auto code = std::static_pointer_cast<IrWrite>(code_t);
					add_use(code->var);
					break;
				}
				case ir_read:
				{
					auto code = std::static_pointer_cast<IrRead>(code_t);
					add_def(code->var);
					break;
				}
				case ir_func_end:
				{
					auto code = std::static_pointer_cast<IrFuncEnd>(code_t);
					break;
				}
			}
		}
	}

	FlowGraph::FlowGraph(std::shared_ptr<IrList> ir_list)
	{
		int cnt_block = 1;
		std::vector<int> block_no;
		auto it_begin = ir_list->begin();
		std::shared_ptr<Block> last_block = nullptr;
		for (auto it = ir_list->begin(); it != ir_list->end(); it++)
		{
			switch ((*it)->ir_type)
			{
				case ir_label:
				case ir_func:
				{
					blocks.push_back(std::make_shared<Block>(it_begin, it));
					if (last_block)
					{
						last_block->next = blocks.back();
					}
					last_block = blocks.back();
					cnt_block++;
					block_no.push_back(cnt_block);
					it_begin = it;
					break;
				}
				case ir_call:
				case ir_ret:
				case ir_branch:
				case ir_jump:
				case ir_func_end:
				{
					block_no.push_back(cnt_block);
					blocks.push_back(std::make_shared<Block>(it_begin, it + 1));
					if (last_block)
					{
						last_block->next = blocks.back();
					}
					last_block = blocks.back();
					cnt_block++;
					it_begin = it + 1;
					break;
				}
				default:
				{
					block_no.push_back(cnt_block);
				}
			}
		}
	}
}
