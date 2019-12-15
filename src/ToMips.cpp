//
// Created by lyt on 2019/11/19.
//


#include "include/ToMips.h"
#include <iostream>
#include <vector>
#include <deque>

namespace ucc
{
	int cur_stack_size = 0;
	static std::shared_ptr<SymbolTable> global_table;
	std::ofstream mips_output_stream;

	static bool busy[32];
	static std::shared_ptr<Var> occ_var[32];
	static std::string cur_func_id;

	std::shared_ptr<FuncMipsCodes> cur_func_codes;
	std::vector<int> cur_func_use_s;

	int need(std::shared_ptr<Var> var, bool load);

	void get_to(std::shared_ptr<Var> var_t, int reg)
	{
		switch (var_t->var_type)
		{
			case var_temp:
			{
				auto var = std::static_pointer_cast<TempVar>(var_t);
				if (var->offset == 0)
				{
					while (1);
					//wrong;
				}
				long long to_sp = cur_stack_size + var->offset;
				cur_func_codes->codes << "lw $" << reg << ", " << to_sp << "($sp)" << std::endl;
				break;
			}
			case var_normal:
			{
				auto var = std::static_pointer_cast<NorVar>(var_t);
				if (var->table_entry->scope == SymbolScope::scope_local)
				{
					int to_sp = cur_stack_size - var->table_entry->address;
					cur_func_codes->codes << "lw $" << reg << ", " << to_sp << "($sp)" << std::endl;
				}
				else
				{
					cur_func_codes->codes << "lw $" << reg << ", " << var->table_entry->id << "($0)" << std::endl;
				}
				break;
			}
			case var_ret:
			{
				cur_func_codes->codes << "move $" << reg << ", $v0" << std::endl;
				break;
			}
			case var_const:
			{
				auto var = std::static_pointer_cast<ConstVar>(var_t);
				cur_func_codes->codes << "li $" << reg << ", " << var->value << std::endl;
				break;
			}
			case var_array:
			{
				auto var = std::static_pointer_cast<ArrayVar>(var_t);
				if (var->index->var_type != VarType::var_temp)
				{
					while (1);
					//wrong
				}
				int cur_state = busy[reg];
				busy[reg] = true;
				int tr = need(var->index, true);
				busy[reg] = cur_state;
				auto v = global_table->find(var->table_entry->id);
				cur_func_codes->codes << "sll $" << $v1 << ", $" << tr << ", 2" << std::endl;
				if (v == nullptr)
				{
					int to_sp = cur_stack_size - var->table_entry->address;
					cur_func_codes->codes << "add $" << $v1 << ", $sp, $" << $v1 << std::endl;
					cur_func_codes->codes << "lw $" << reg << ", " << to_sp << "($" << $v1 << ")" << std::endl;
				}
				else
				{
					cur_func_codes->codes << "lw $" << reg << ", " << var->table_entry->id << "($" << $v1 << ")" << std::endl;
				}
				break;
			}
			default:
			{
				while (1);
			}
		}
	}

	int cur_ir_line;

	void save_to(int reg, std::shared_ptr<Var> var_t)
	{
		switch (var_t->var_type)
		{
			case var_temp:
			{
				auto var = std::static_pointer_cast<TempVar>(var_t);
				long long to_sp = cur_stack_size + var->offset;
				cur_func_codes->codes << "sw $" << reg << ", " << to_sp << "($sp)" << std::endl;
				break;
			}
			case var_normal:
			{
				auto var = std::static_pointer_cast<NorVar>(var_t);
				if (var->table_entry->scope == SymbolScope::scope_local)
				{
					int to_sp = cur_stack_size - var->table_entry->address;
					cur_func_codes->codes << "sw $" << reg << ", " << to_sp << "($sp)" << std::endl;
				}
				else
				{
					cur_func_codes->codes << "sw $" << reg << ", " << var->table_entry->id << "($0)" << std::endl;
				}
				break;
			}
			case var_ret:
			{
				while (1);
				//wrong
			}
			case var_const:
			{
				while (1);
				//wrong
			}
			case var_array:
			{
				auto var = std::static_pointer_cast<ArrayVar>(var_t);
				if (var->index->var_type != VarType::var_temp)
				{
					while (1);
					//wrong
				}
				int cur_state = busy[reg];
				busy[reg] = true;
				int tr = need(var->index, true);
				busy[reg] = cur_state;
				auto v = global_table->find(var->table_entry->id);
				cur_func_codes->codes << "sll $" << $v1 << ", $" << tr << ", 2" << std::endl;
				if (v == nullptr)
				{
					int to_sp = cur_stack_size - var->table_entry->address;
					cur_func_codes->codes << "addu $" << $v1 << ", $sp, $" << $v1 << std::endl;
					cur_func_codes->codes << "sw $" << reg << ", " << to_sp << "($" << $v1 << ")" << std::endl;
				}
				else
				{
					cur_func_codes->codes << "sw $" << reg << ", " << var->table_entry->id << "($" << $v1 << ")" << std::endl;
				}
				break;
			}
			default:
			{
				while (1);
			}
		}
	}

	static std::vector<int> general_reg = {$s0, $s1, $s2, $s3, $s4, $s5, $s6, $s7};
	static std::vector<int> temp_reg = {$t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7, $t8, $t9};
	static std::vector<int> use_seq;
	static bool dirty[32];

	bool is_temp(int u)
	{
		for (int i : temp_reg)
		{
			if (i == u)
			{
				return true;
			}
		}
		return false;
	}

	std::vector<std::set<std::shared_ptr<Var>>> used_temp;

	void put_back(int w)
	{
		busy[w] = false;
		if (occ_var[w] != nullptr)
		{
			if (dirty[w])
			{
				if (occ_var[w]->var_type == VarType::var_temp)
				{
					bool is_using = false;
					for (auto var_t : used_temp[cur_ir_line])
					{
						if (is_same(var_t, occ_var[w]))
						{
							is_using = true;
							break;
						}
					}
					if (is_using)
					{
						save_to(w, occ_var[w]);
					}
				}
				else
				{
					save_to(w, occ_var[w]);
				}
			}
		}
		occ_var[w] = nullptr;
		dirty[w] = false;
		for (auto e = use_seq.begin(); e != use_seq.end(); e++)
		{
			if (*e == w)
			{
				use_seq.erase(e);
				break;
			}
		}
	}

	bool has_in(std::shared_ptr<Var> var)
	{
		for (int i = 0; i < 32; i++)
		{
			if (occ_var[i] != nullptr && is_same(occ_var[i], var))
			{
				return true;
			}
		}
		return false;
	}

	int need(std::shared_ptr<Var> var, bool load)
	{
		for (int i = 0; i < 32; i++)
		{
			if (occ_var[i] != nullptr && is_same(occ_var[i], var))
			{
				for (auto e = use_seq.begin(); e != use_seq.end(); e++)
				{
					if (*e == i)
					{
						use_seq.erase(e);
						break;
					}
				}
				bool in = false;
				for (int v : temp_reg)
				{
					if (i == v)
					{
						in = true;
						break;
					}
				}
				if (in)
				{
					use_seq.push_back(i);
				}
				return i;
			}
		}
		for (int i = 0; i < 32; i++)
		{
			if (!busy[i] && occ_var[i] && occ_var[i]->var_type == VarType::var_array)
			{
				put_back(i);
			}
		}
		for (int i : temp_reg)
		{
			if (!busy[i] && occ_var[i] == nullptr)
			{
				occ_var[i] = var;
				use_seq.push_back(i);
				dirty[i] = false;
				if (load)
				{
					get_to(var, i);
				}
				return i;
			}
		}
		int w = -1;
		for (auto e = use_seq.begin(); e != use_seq.end(); e++)
		{
			if (!busy[*e])
			{
				w = *e;
				break;
			}
		}
		if (w == -1)
		{
			std::cout << "reg all locked!!" << std::endl;
			while (1);
		}
		put_back(w);
		dirty[w] = false;
		occ_var[w] = var;
		use_seq.push_back(w);
		if (load)
		{
			get_to(var, w);
		}
		return w;
	}

	inline void temp_put_back()
	{
		for (int i : temp_reg)
		{
			if (occ_var[i] != nullptr && occ_var[i]->var_type == VarType::var_array)
			{
				put_back(i);
			}
		}
		for (int i : temp_reg)
		{
			if (occ_var[i] != nullptr)
			{
				put_back(i);
			}
		}
	}

	inline void all_put_back()
	{
		for (int i = 0; i < 32; i++)
		{
			if (occ_var[i] != nullptr && occ_var[i]->var_type == VarType::var_array)
			{
				put_back(i);
			}
		}
		for (int i = 0; i < 32; i++)
		{
			if (occ_var[i] != nullptr)
			{
				put_back(i);
			}
		}
	}


	void get_use_temp(std::set<std::shared_ptr<Var>> &use, std::shared_ptr<Var> var)
	{
		if (var->var_type == VarType::var_array)
		{
			auto var_t = std::static_pointer_cast<ArrayVar>(var);
			var = var_t->index;
		}
		if (var->var_type != VarType::var_temp)
		{
			return;
		}
		use.insert(var);
	}

	void to_mips(std::shared_ptr<IrList> ir_list, std::shared_ptr<SymbolTable> global_table_t)
	{
		used_temp.resize(ir_list->size());
		for (int i = ir_list->size() - 1; i >= 0; i--)
		{
			auto &code_t = (*ir_list)[i];
			switch (code_t->ir_type)
			{
				case ir_assign:
				{
					auto code = std::static_pointer_cast<IrAssign>(code_t);
					get_use_temp(used_temp[i], code->l);
					if (code->r)
					{
						get_use_temp(used_temp[i], code->r);
					}
					get_use_temp(used_temp[i], code->aim);
					break;
				}
				case ir_call:
				{
					auto code = std::static_pointer_cast<IrCall>(code_t);
					for (auto var : code->vars)
					{
						get_use_temp(used_temp[i], var);
					}
					break;
				}
				case ir_ret:
				{
					auto code = std::static_pointer_cast<IrRet>(code_t);
					if (!code->is_void)
					{
						get_use_temp(used_temp[i], code->var);
					}
					break;
				}
				case ir_branch:
				{
					auto code = std::static_pointer_cast<IrBranch>(code_t);
					get_use_temp(used_temp[i], code->var);
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
						get_use_temp(used_temp[i], std::make_shared<NorVar>(code->symbol_table->find(var_s)));
					}
					break;
				}
				case ir_write:
				{
					auto code = std::static_pointer_cast<IrWrite>(code_t);
					get_use_temp(used_temp[i], code->var);
					break;
				}
				case ir_read:
				{
					auto code = std::static_pointer_cast<IrRead>(code_t);
					get_use_temp(used_temp[i], code->var);
					break;
				}
				case ir_func_end:
				{
					auto code = std::static_pointer_cast<IrFuncEnd>(code_t);
					break;
				}
			}
			if (i > 0)
			{
				used_temp[i - 1] = used_temp[i];
			}
		}
		global_table = global_table_t;
		mips_output_stream << ".data" << std::endl;
		for (auto e : global_table->get_table())
		{
			auto entry = e.second;
			if (entry->type == SymbolType::varible)
			{
				if (entry->attributes.find(SymbolAttribute::att_const) != entry->attributes.end())
				{
					mips_output_stream << entry->id << ": .word " << entry->init_value << std::endl;
				}
				else
				{
					mips_output_stream << entry->id << ": .word 0" << std::endl;
				}
			}
			else if (entry->type == SymbolType::array)
			{
				int size = entry->size;
				/*
				while ((size & 3) != 0)
				{
					size++;
				}
				*/
				mips_output_stream << entry->id << ": .space " << size << std::endl;
			}
		}
		mips_output_stream << ".text" << std::endl;
		mips_output_stream << "jal main" << std::endl;
		mips_output_stream << "li $v0 10" << std::endl;
		mips_output_stream << "syscall" << std::endl;
		for (auto ir_it = ir_list->begin(); ir_it != ir_list->end(); ir_it++)
		{
			cur_ir_line = ir_it - ir_list->begin();
			auto ir_t = *ir_it;
			switch (ir_t->ir_type)
			{
				case IrType::ir_assign :
				{
					auto ir = std::static_pointer_cast<IrAssign>(ir_t);
					if (ir->r == nullptr)
					{
						int l_reg = need(ir->l, true);
						busy[l_reg] = true;
						int aim_reg = need(ir->aim, false);
						busy[aim_reg] = true;
						switch (ir->op)
						{
							case op_add:
							{
								cur_func_codes->codes << "move $" << aim_reg << ", $" << l_reg << std::endl;
								break;
							}
							case op_sub:
							{
								cur_func_codes->codes << "sub $" << aim_reg << ", $0, $" << l_reg << std::endl;
								break;
							}
							default:
							{
								while (1);
								//wrong;
							}
						}
						busy[l_reg] = false;
						busy[aim_reg] = false;
						dirty[aim_reg] = true;
					}
					else
					{
						int l_reg = need(ir->l, true);
						busy[l_reg] = true;
						int r_reg = need(ir->r, true);
						busy[r_reg] = true;
						int aim_reg = need(ir->aim, false);
						busy[aim_reg] = true;
						switch (ir->op)
						{
							case op_add:
							{
								cur_func_codes->codes << "add $" << aim_reg << ", $" << l_reg << ", $" << r_reg << std::endl;
								break;
							}
							case op_sub:
							{
								cur_func_codes->codes << "sub $" << aim_reg << ", $" << l_reg << ", $" << r_reg << std::endl;
								break;
							}
							case op_mul:
							{
								cur_func_codes->codes << "mul $" << aim_reg << ", $" << l_reg << ", $" << r_reg << std::endl;
								break;
							}
							case op_div:
							{
								cur_func_codes->codes << "div $" << l_reg << ", $" << r_reg << std::endl;
								cur_func_codes->codes << "mflo $" << aim_reg << std::endl;
								break;
							}
							case op_ge:
							{
								cur_func_codes->codes << "sge $" << aim_reg << ", $" << l_reg << ", $" << r_reg << std::endl;
								break;
							}
							case op_gt:
							{
								cur_func_codes->codes << "sgt $" << aim_reg << ", $" << l_reg << ", $" << r_reg << std::endl;
								break;
							}
							case op_le:
							{
								cur_func_codes->codes << "sle $" << aim_reg << ", $" << l_reg << ", $" << r_reg << std::endl;
								break;
							}
							case op_lt:
							{
								cur_func_codes->codes << "slt $" << aim_reg << ", $" << l_reg << ", $" << r_reg << std::endl;
								break;
							}
							case op_eq:
							{
								cur_func_codes->codes << "seq $" << aim_reg << ", $" << l_reg << ", $" << r_reg << std::endl;
								break;
							}
							case op_ne:
							{
								cur_func_codes->codes << "sne $" << aim_reg << ", $" << l_reg << ", $" << r_reg << std::endl;
								break;
							}
						}
						busy[l_reg] = false;
						busy[r_reg] = false;
						busy[aim_reg] = false;
						dirty[aim_reg] = true;
					}
					break;
				}
				case ir_call:
				{
					auto ir = std::static_pointer_cast<IrCall>(ir_t);
					cur_func_codes->codes << "addiu $sp, $sp, -" << ir->vars.size() * 4 + 16 << std::endl;
					cur_stack_size += ir->vars.size() * 4 + 16;
					bool save_reg[4] = {false, false, false, false};
					for (int i = 0; i < ir->vars.size(); i++)
					{
						if (i < 4)
						{
							if (occ_var[$a0 + i])
							{
								save_reg[i] = true;
								cur_func_codes->codes << "sw $" << $a0 + i << ", " << ir->vars.size() * 4 + i * 4 << "($sp)" << std::endl;
							}
							int reg = need(ir->vars[i], true);
							if (reg >= $a0 && reg < $a0 + i)
							{
								cur_func_codes->codes << "lw $" << $a0 + i << ", " << ir->vars.size() * 4 + (reg - $a0) * 4 << "($sp)" << std::endl;
							}
							else
							{
								cur_func_codes->codes << "move $" << $a0 + i << ", $" << reg << std::endl;
							}
						}
						else
						{
							int reg = need(ir->vars[i], true);
							if (reg >= $a0 && reg <= $a0 + 3)
							{
								cur_func_codes->codes << "lw $" << $v1 << ", " << ir->vars.size() * 4 + (reg - $a0) * 4 << "($sp)" << std::endl;
								reg = $v1;
							}
							int to_sp = i * 4;
							cur_func_codes->codes << "sw $" << reg << ", " << to_sp << "($sp)" << std::endl;
						}
					}
					temp_put_back();
					cur_func_codes->codes << "jal " << ir->func << std::endl;
					for (int i = 0; i < 4; i++)
					{
						if (save_reg[i])
						{
							cur_func_codes->codes << "lw $" << $a0 + i << ", " << ir->vars.size() * 4 + i * 4 << "($sp)" << std::endl;
						}
					}
					cur_func_codes->codes << "addiu $sp, $sp, " << ir->vars.size() * 4 + 16 << std::endl;
					cur_stack_size -= ir->vars.size() * 4 + 16;
					break;
				}
				case ir_ret:
				{
					auto ir = std::static_pointer_cast<IrRet>(ir_t);
					if (!ir->is_void)
					{
						int reg = need(ir->var, true);
						cur_func_codes->codes << "move $v0, $" << reg << std::endl;
					}
					temp_put_back();
					for (int i = 0; i < cur_func_use_s.size(); i++)
					{
						cur_func_codes->codes << "lw $" << $s0 + i << ", " << i * 4 << "($sp)" << std::endl;
					}
					cur_func_codes->codes << "addiu $sp, $sp, " << cur_stack_size << std::endl;
					cur_func_codes->codes << "lw $ra, -4($sp)" << std::endl;
					cur_func_codes->codes << "jr $ra" << std::endl;
					//cur_func_codes->codes << "j " << cur_func_id << "_END$" << std::endl;
					break;
				}
				case ir_branch:
				{
					auto ir = std::static_pointer_cast<IrBranch>(ir_t);
					int reg = need(ir->var, true);
					if (is_temp(reg))
					{
						cur_func_codes->codes << "move $v1, $" << reg << std::endl;
						reg = $v1;
					}
					temp_put_back();
					if (ir->is_true)
					{
						cur_func_codes->codes << "bnez $" << reg << ", __LABEL$" << ir->label->id << std::endl;
					}
					else
					{
						cur_func_codes->codes << "beqz $" << reg << ", __LABEL$" << ir->label->id << std::endl;
					}
					break;
				}
				case ir_jump:
				{
					temp_put_back();
					auto ir = std::static_pointer_cast<IrJump>(ir_t);
					cur_func_codes->codes << "j __LABEL$" << ir->label->id << std::endl;
					break;
				}
				case ir_label:
				{
					temp_put_back();
					auto ir = std::static_pointer_cast<IrLable>(ir_t);
					cur_func_codes->codes << "__LABEL$" << ir->label->id << ":" << std::endl;
					break;
				}
				case ir_func:
				{
					for (int i = 0; i < 32; i++)
					{
						busy[i] = false;
						occ_var[i] = nullptr;
						dirty[i] = false;
					}
					use_seq.clear();
					cur_func_use_s.clear();
					cur_func_codes = std::make_shared<FuncMipsCodes>();
					auto ir = std::static_pointer_cast<IrFunc>(ir_t);
					cur_func_id = ir->id;
					cur_stack_size = 0;
					cur_stack_size += 4;
					cur_func_codes->codes << "sw $ra, -4($sp)" << std::endl;
					for (auto &e : ir->symbol_table->get_table())
					{
						auto entry = e.second;
						if (entry->type == SymbolType::varible)
						{
							cur_stack_size += 4;
							entry->address = cur_stack_size;
						}
						else if (entry->type == SymbolType::array)
						{
							int size = entry->size;
							/*
							while ((size & 3) != 0)
							{
								size++;
							}
							*/
							cur_stack_size += size;
							entry->address = cur_stack_size;
						}
					}
					for (auto sub_it = ir_it; (*sub_it)->ir_type != IrType::ir_func_end; sub_it++)
					{
						auto sub_ir_t = *sub_it;
						bool is = false;
						std::shared_ptr<Var> var_t;
						if (sub_ir_t->ir_type == IrType::ir_assign)
						{
							is = true;
							auto sub_ir = std::static_pointer_cast<IrAssign>(sub_ir_t);
							var_t = sub_ir->aim;
						}
						else if (sub_ir_t->ir_type == IrType::ir_read)
						{
							is = true;
							auto sub_ir = std::static_pointer_cast<IrRead>(sub_ir_t);
							var_t = sub_ir->var;
						}
						if (is && var_t->var_type == VarType::var_temp)
						{
							auto var = std::static_pointer_cast<TempVar>(var_t);
							if (var->offset == 0)
							{
								cur_stack_size += 4;
								var->offset = -cur_stack_size;
							}
						}
					}
					int cnt = 0;for (int i = 0; i < ir->par_list->size(); i++)
					{
						if (i < 4)
						{
							occ_var[$a0 + i] = std::make_shared<NorVar>(ir->symbol_table->find((*ir->par_list)[i]));
						}
					}
					for (auto e : ir->symbol_table->get_table())
					{
						auto &entry = e.second;
						if (cnt < general_reg.size() && entry->attributes.find(SymbolAttribute::att_register) != entry->attributes.end())
						{
							auto var = std::make_shared<NorVar>(entry);
							if (!has_in(var))
							{
								occ_var[$s0 + cnt] = var;
								cur_func_use_s.push_back(cnt);
								cnt++;
								cur_stack_size += 4;
							}
						}
					}
					for (auto e : ir->symbol_table->get_table())
					{
						auto &entry = e.second;
						if (cnt < general_reg.size() && entry->type == SymbolType::varible &&entry->attributes.find(SymbolAttribute::att_const) == entry->attributes.end())
						{
							auto var = std::make_shared<NorVar>(entry);
							if (!has_in(var))
							{
								occ_var[$s0 + cnt] = var;
								cur_func_use_s.push_back(cnt);
								cnt++;
								cur_stack_size += 4;
							}
						}
					}
					cur_func_codes->codes << "addiu $sp, $sp, -" << cur_stack_size << std::endl;
					for (int i = 0; i < cur_func_use_s.size(); i++)
					{
						cur_func_codes->codes << "sw $" << $s0 + i << ", " << i * 4 << "($sp)" << std::endl;
					}
					for (auto e : ir->symbol_table->get_table())
					{
						auto entry = e.second;
						if (entry->type == SymbolType::varible)
						{
							if (entry->attributes.find(SymbolAttribute::att_const) != entry->attributes.end())
							{
								int to_sp = cur_stack_size - entry->address;
								cur_func_codes->codes << "li $v1, " << entry->init_value << std::endl;
								cur_func_codes->codes << "sw $v1, " << to_sp << "($sp)" << std::endl;
							}
						}
					}
					for (int i = 0; i < ir->par_list->size(); i++)
					{
						if (i < 4)
						{
							//occ_var[$a0 + i] = std::make_shared<NorVar>(ir->symbol_table->find((*ir->par_list)[i]));
						}
						else
						{
							int to_sp = cur_stack_size + i * 4;
							cur_func_codes->codes << "lw $v1, " << to_sp << "($sp)" << std::endl;
							save_to($v1, std::make_shared<NorVar>(ir->symbol_table->find((*ir->par_list)[i])));
						}
					}
					break;
				}
				case ir_write:
				{
					auto ir = std::static_pointer_cast<IrWrite>(ir_t);
					if (ir->as_char)
					{
						cur_func_codes->codes << "li $v0, 11" << std::endl;
					}
					else
					{
						cur_func_codes->codes << "li $v0, 1" << std::endl;
					}
					if (occ_var[$a0] != nullptr)
					{
						cur_func_codes->codes << "move $v1, $a0" << std::endl;
					}
					int reg = need(ir->var, true);
					cur_func_codes->codes << "move $a0, $" << reg << std::endl;
					cur_func_codes->codes << "syscall" << std::endl;
					if (occ_var[$a0] != nullptr)
					{
						cur_func_codes->codes << "move $a0, $v1" << std::endl;
					}
					break;
				}
				case ir_read:
				{
					auto ir = std::static_pointer_cast<IrRead>(ir_t);
					if (ir->var->table_entry->data == SymbolData::data_char)
					{
						cur_func_codes->codes << "li $v0, 12" << std::endl;
					}
					else
					{

						cur_func_codes->codes << "li $v0, 5" << std::endl;
					}
					cur_func_codes->codes << "syscall" << std::endl;
					int reg = need(ir->var, false);
					cur_func_codes->codes << "move $" << reg << ",$v0" << std::endl;
					dirty[reg] = true;
					break;
				}
				case ir_func_end:
				{
					all_put_back();
					for (int i = 0; i < cur_func_use_s.size(); i++)
					{
						cur_func_codes->codes << "lw $" << $s0 + i << ", " << i * 4 << "($sp)" << std::endl;
					}
					cur_func_codes->codes << "addiu $sp, $sp, " << cur_stack_size << std::endl;
					cur_func_codes->codes << "lw $ra, -4($sp)" << std::endl;
					cur_func_codes->codes << "jr $ra" << std::endl;
					//cur_func_codes->codes << cur_func_id << "_END$:" << std::endl;

					mips_output_stream << cur_func_id << ":" << std::endl;
					std::string func_str((std::istreambuf_iterator<char>(cur_func_codes->codes)), std::istreambuf_iterator<char>());
					mips_output_stream << func_str;
					//mips_output_stream << "jr $ra" << std::endl;
					break;
				}
			}
		}
	}
}