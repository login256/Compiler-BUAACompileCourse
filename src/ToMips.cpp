//
// Created by lyt on 2019/11/19.
//


#include "../include/ToMips.h"
#include <iostream>

namespace ucc
{
	int cur_stack_size = 0;
	static std::shared_ptr<SymbolTable> global_table;
	std::ofstream mips_output_stream;
#define $s0 16
#define $s1 17
#define $v0 2
#define $a0 4
#define $ra 31
#define $sp 29

	void get_to(std::shared_ptr<Var> tvar, int reg)
	{
		switch (tvar->var_type)
		{
			case var_temp:
			{
				auto var = std::static_pointer_cast<TempVar>(tvar);
				if (var->offset == 0)
				{
					while (1);
					//wrong;
				}
				long long to_sp = cur_stack_size + var->offset;
				mips_output_stream << "lw $" << reg << ", " << to_sp << "($sp)" << std::endl;
				break;
			}
			case var_normal:
			{
				auto var = std::static_pointer_cast<NorVar>(tvar);
				auto v = global_table->find(var->table_entry->id);
				if (v == nullptr)
				{
					int to_sp = cur_stack_size - var->table_entry->address;
					mips_output_stream << "lw $" << reg << ", " << to_sp << "($sp)" << std::endl;
				}
				else
				{
					mips_output_stream << "lw $" << reg << ", " << var->table_entry->id << "($0)" << std::endl;
				}
				break;
			}
			case var_ret:
			{
				mips_output_stream << "move $" << reg << ", $v0" << std::endl;
				break;
			}
			case var_const:
			{
				auto var = std::static_pointer_cast<ConstVar>(tvar);
				mips_output_stream << "li $" << reg << ", " << var->value << std::endl;
				break;
			}
			case var_array:
			{
				auto var = std::static_pointer_cast<ArrayVar>(tvar);
				if (var->index->var_type != VarType::var_temp)
				{
					while (1);
					//wrong
				}
				get_to(var->index, 8);
				auto v = global_table->find(var->table_entry->id);
				mips_output_stream << "sll $t0, $t0, 2" << std::endl;
				if (v == nullptr)
				{
					int to_sp = cur_stack_size - var->table_entry->address;
					mips_output_stream << "add $t0, $sp, $t0" << std::endl;
					mips_output_stream << "lw $" << reg << ", " << to_sp << "($t0)" << std::endl;
				}
				else
				{
					mips_output_stream << "lw $" << reg << ", " << var->table_entry->id << "($t0)" << std::endl;
				}
				break;
			}
			default:
			{
				while (1);
			}
		}
	}

	void save_to(int reg, std::shared_ptr<Var> var_t)
	{
		switch (var_t->var_type)
		{
			case var_temp:
			{
				auto var = std::static_pointer_cast<TempVar>(var_t);
				if (var->offset == 0)
				{
					cur_stack_size += 4;
					var->offset = -cur_stack_size;
					mips_output_stream << "addiu $sp, $sp, -4" << std::endl;
				}
				long long to_sp = cur_stack_size + var->offset;
				mips_output_stream << "sw $" << reg << ", " << to_sp << "($sp)" << std::endl;
				break;
			}
			case var_normal:
			{
				auto var = std::static_pointer_cast<NorVar>(var_t);
				auto v = global_table->find(var->table_entry->id);
				if (v == nullptr)
				{
					int to_sp = cur_stack_size - var->table_entry->address;
					mips_output_stream << "sw $" << reg << ", " << to_sp << "($sp)" << std::endl;
				}
				else
				{
					mips_output_stream << "sw $" << reg << ", " << var->table_entry->id << "($0)" << std::endl;
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
				get_to(var->index, 8);
				auto v = global_table->find(var->table_entry->id);
				mips_output_stream << "sll $t0, $t0, 2" << std::endl;
				if (v == nullptr)
				{
					int to_sp = cur_stack_size - var->table_entry->address;
					mips_output_stream << "add $t0, $sp, $t0" << std::endl;
					mips_output_stream << "sw $" << reg << ", " << to_sp << "($t0)" << std::endl;
				}
				else
				{
					mips_output_stream << "sw $" << reg << ", " << var->table_entry->id << "($t0)" << std::endl;
				}
				break;
			}
			default:
			{
				while (1);
			}
		}
	}

	void to_mips(std::shared_ptr<IrList> ir_list, std::shared_ptr<SymbolTable> global_table_t)
	{
		global_table = global_table_t;
		mips_output_stream << ".data" << std::endl;
		for (auto e : global_table->get_table())
		{
			auto entry = e.second;
			if (entry->type == SymbolType::varible)
			{
				mips_output_stream << entry->id << ": .word 0" << std::endl;
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
				mips_output_stream << entry->id << ": space " << size << std::endl;
			}
		}
		mips_output_stream << ".text" << std::endl;
		mips_output_stream << "jal main" << std::endl;
		mips_output_stream << "li $v0 0" << std::endl;
		mips_output_stream << "syscall" << std::endl;
		for (auto ir_t : *ir_list)
		{
			switch (ir_t->ir_type)
			{
				case IrType::IR_assign :
				{
					auto ir = std::static_pointer_cast<IrAssign>(ir_t);
					if (ir->r == nullptr)
					{
						get_to(ir->l, $s0);
						switch (ir->op)
						{
							case op_add:
							{
								break;
							}
							case op_sub:
							{
								mips_output_stream << "sub $s0, $0, $s0" << std::endl;
								break;
							}
							default:
							{
								while (1);
								//wrong;
							}
						}
						save_to($s0, ir->aim);
					}
					else
					{
						get_to(ir->l, $s0);
						get_to(ir->r, $s1);
						switch (ir->op)
						{
							case op_add:
							{
								mips_output_stream << "add $s0, $s0, $s1" << std::endl;
								break;
							}
							case op_sub:
							{
								mips_output_stream << "sub $s0, $s0, $s1" << std::endl;
								break;
							}
							case op_mul:
							{
								mips_output_stream << "mul $s0, $s0, $s1" << std::endl;
								break;
							}
							case op_div:
							{
								mips_output_stream << "div $s0, $s0, $s1" << std::endl;
								break;
							}
							case op_ge:
							{
								mips_output_stream << "sge $s0, $s0, $s1" << std::endl;
								break;
							}
							case op_gt:
							{
								mips_output_stream << "sgt $s0, $s0, $s1" << std::endl;
								break;
							}
							case op_le:
							{
								mips_output_stream << "sle $s0, $s0, $s1" << std::endl;
								break;
							}
							case op_lt:
							{
								mips_output_stream << "slt $s0, $s0, $s1" << std::endl;
								break;
							}
							case op_eq:
							{
								mips_output_stream << "seq $s0, $s0, $s1" << std::endl;
								break;
							}
							case op_ne:
							{
								mips_output_stream << "sne $s0, $s0, $s1" << std::endl;
								break;
							}
						}
						save_to($s0, ir->aim);
					}
					break;
				}
				case IR_call:
				{
					break;
				}
				case IR_ret:
				{
					//not finished
					mips_output_stream << "addiu $sp, $sp, " << cur_stack_size << std::endl;
					mips_output_stream << "sw $ra, -4($sp)" << std::endl;
					break;
				}
				case IR_branch:
				{
					auto ir = std::static_pointer_cast<IrBranch>(ir_t);
					get_to(ir->var, $s0);
					if (ir->is_true)
					{
						mips_output_stream << "bnez $s0, __LABEL$" << ir->label->id << std::endl;
					}
					else
					{
						mips_output_stream << "beqz $s0, __LABEL$" << ir->label->id << std::endl;
					}
					break;
				}
				case IR_jump:
				{
					auto ir = std::static_pointer_cast<IrJump>(ir_t);
					mips_output_stream << "j __LABEL$" << ir->label->id << std::endl;
					break;
				}
				case IR_label:
				{
					auto ir = std::static_pointer_cast<IrLable>(ir_t);
					mips_output_stream << "__LABEL$" << ir->label->id << ":" << std::endl;
					break;
				}
				case IR_func:
				{
					auto ir = std::static_pointer_cast<IrFunc>(ir_t);
					mips_output_stream << ir->id << ":" << std::endl;
					cur_stack_size = 0;
					cur_stack_size += 4;
					mips_output_stream << "addiu $sp, $sp, -4" << std::endl;
					mips_output_stream << "sw $ra, 0($sp)" << std::endl;
					//unfinished
					break;
				}
				case IR_write:
				{
					auto ir = std::static_pointer_cast<IrWrite>(ir_t);
					if (ir->as_char)
					{
						mips_output_stream << "li $v0, 11" << std::endl;
					}
					else
					{
						mips_output_stream << "li $v0, 1" << std::endl;
					}
					get_to(ir->var, $a0);
					mips_output_stream << "syscall" << std::endl;
					break;
				}
				case IR_read:
				{
					auto ir = std::static_pointer_cast<IrRead>(ir_t);
					if (ir->var->table_entry->data == SymbolData::data_char)
					{
						mips_output_stream << "li $v0, 12" << std::endl;
					}
					else
					{

						mips_output_stream << "li $v0, 5" << std::endl;
					}
					mips_output_stream << "syscall" << std::endl;
					save_to($v0, ir->var);
					break;
				}
				case IR_func_end:
				{
					mips_output_stream << "addiu $sp, $sp, " << cur_stack_size << std::endl;
					mips_output_stream << "lw $ra, -4($sp)" << std::endl;
					mips_output_stream << "jr $ra" << std::endl;
					break;
				}
			}
		}
	}
}