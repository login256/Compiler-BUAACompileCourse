//
// Created by lyt on 2019/11/15.
//

#include "include/IR.h"

namespace ucc
{
	IrCode::IrCode(IrType ir_type) : ir_type(ir_type) {}

	IrJump::IrJump(std::shared_ptr<Label> label) : IrCode(IrType::ir_jump), label(label) {}

	void IrJump::print(std::ostream &stream) const
	{
		stream << "jump " << *label;
	}

	int Label::cnt;

	Label::Label()
	{
		id = ++cnt;
	}

	std::ostream &operator<<(std::ostream &os, const Label &label)
	{
		os << "label$" << label.id;
		return os;
	}

	IrBranch::IrBranch(bool is_true, const std::shared_ptr<Var> &var, std::shared_ptr<Label> label) : IrCode(IrType::ir_branch), is_true(is_true),
																									  var(var),
																									  label(label) {}

	void IrBranch::print(std::ostream &os) const
	{
		if (is_true)
		{
			os << "beq " << *var << " " << *label;
		}
		else
		{
			os << "bne " << *var << " " << *label;
		}
	}

	IrAssign::IrAssign(IrOp op, const std::shared_ptr<Var> &l, const std::shared_ptr<Var> &r, const std::shared_ptr<Var> &aim) :
			IrCode(IrType::ir_assign), op(op), l(l), r(r), aim(aim) {}

	IrAssign::IrAssign(IrOp op, const std::shared_ptr<Var> &l, const std::shared_ptr<Var> &aim) : IrCode(IrType::ir_assign), op(op), l(l), aim(aim) {}

	void IrAssign::print(std::ostream &os) const
	{
		if (r == nullptr)
		{
			switch (op)
			{
				case IrOp::op_add:
					os << *aim << " = " << *l;
					break;
				case IrOp::op_sub:
					os << *aim << " = -" << *l;
					break;
				default:
					while (1);//error;
			}
		}
		else
		{
			std::string op_c;
			switch (op)
			{
				case IrOp::op_add :
					op_c = "+";
					break;
				case IrOp::op_sub :
					op_c = "-";
					break;
				case IrOp::op_mul :
					op_c = "*";
					break;
				case IrOp::op_div :
					op_c = "/";
					break;
				case IrOp::op_ge  :
					op_c = ">=";
					break;
				case IrOp::op_gt  :
					op_c = ">";
					break;
				case IrOp::op_le  :
					op_c = "<=";
					break;
				case IrOp::op_lt  :
					op_c = "<";
					break;
				case IrOp::op_eq  :
					op_c = "==";
					break;
				case IrOp::op_ne  :
					op_c = "!=";
					break;
			}
			os << *aim << " = " << *l << " " << op_c << " " << *r;
		}
	}

	IrRet::IrRet(const std::shared_ptr<Var> &var) : IrCode(IrType::ir_ret), is_void(false), var(var) {}

	IrRet::IrRet() : IrCode(IrType::ir_ret), is_void(true) {}

	void IrRet::print(std::ostream &os) const
	{
		os << "return";
		if (!is_void)
		{
			os << " " << *var;
		}
	}


	IrLable::IrLable(std::shared_ptr<Label> label) : IrCode(IrType::ir_label), label(label) {}

	void IrLable::print(std::ostream &os) const
	{
		os << *label << ":";
	}

	IrCall::IrCall(const std::string &func) : IrCode(IrType::ir_call), func(func) {}

	void IrCall::print(std::ostream &os) const
	{
		os << func << "(";
		bool is_first = true;
		for (auto e : this->vars)
		{
			if (!is_first)
			{
				os << ", ";
			}
			os << *e;
			is_first = false;
		}
		os << ")";
	}

	IrFunc::IrFunc(const std::string &id, std::shared_ptr<SymbolTable> symbol_table, std::shared_ptr<std::vector<std::string>> par_list)
			: IrCode(IrType::ir_func), id(id), symbol_table(symbol_table), par_list(par_list) {}

	void IrFunc::print(std::ostream &os) const
	{
		os << id << "(";
		bool is_first = true;
		for (auto e : *par_list)
		{
			if (!is_first)
			{
				os << ", ";
			}
			os << e;
			is_first = false;
		}
		os << ")" << std::endl;
		for (auto e : symbol_table->get_table())
		{
			if (e.second->attributes.find(SymbolAttribute::att_const) != e.second->attributes.end())
			{
				os << "var const " << e.first << " = " << e.second->init_value << std::endl;
			}
			else
			{
				os << "var " << e.first << std::endl;
			}
		}
		os << "{";
	}

	/*
	Var::Var(bool is_temp, bool is_ret_value, std::shared_ptr<SymbolTableEntry> table_entry,std::shared_ptr<Var> index)
			: is_temp(is_temp), table_entry(table_entry), is_ret_value(is_ret_value), index(index)
	{
		if (is_temp)
		{
			this->id = ++cnt_temp;
		}
	}
	*/

	Var::Var(VarType var_type) : var_type(var_type) {}

	std::ostream &operator<<(std::ostream &os, const Var &var)
	{
		var.print(os);
		return os;
	}

	ConstVar::ConstVar(int value) : Var(VarType::var_const), value(value) {}

	void ConstVar::print(std::ostream &os) const
	{
		os << value;
	}

	int TempVar::cnt_temp;

	TempVar::TempVar() : Var(VarType::var_temp)
	{
		this->id = ++cnt_temp;
		offset = 0;
	}

	void TempVar::print(std::ostream &os) const
	{
		os << "$temp" << id;
	}

	RetVar::RetVar() : Var(VarType::var_ret) {}

	void RetVar::print(std::ostream &os) const
	{
		os << "$ret_value";
	}

	NorVar::NorVar(const std::shared_ptr<SymbolTableEntry> &table_entry) : Var(VarType::var_normal), table_entry(table_entry) {}

	void NorVar::print(std::ostream &os) const
	{
		os << table_entry->id;
	}

	ArrayVar::ArrayVar(const std::shared_ptr<SymbolTableEntry> &table_entry, const std::shared_ptr<Var> &index)
			: Var(VarType::var_array), table_entry(table_entry), index(index) {}

	void ArrayVar::print(std::ostream &os) const
	{
		os << table_entry->id << "[" << *index << "]";
	}

	std::ostream &operator<<(std::ostream &os, const IrCode &code)
	{
		code.print(os);
		return os;
	}

	IrRead::IrRead(const std::shared_ptr<NorVar> &var) : IrCode(IrType::ir_read), var(var) {}

	void IrRead::print(std::ostream &os) const
	{
		os << "read " << *var;
	}

	IrWrite::IrWrite(const std::shared_ptr<Var> &var, bool as_char) : IrCode(IrType::ir_write), var(var), as_char(as_char) {}

	void IrWrite::print(std::ostream &os) const
	{
		os << "print " << *var << " as " << (as_char ? "char" : "int");
	}

	IrFuncEnd::IrFuncEnd() : IrCode(IrType::ir_func_end) {}

	void IrFuncEnd::print(std::ostream &os) const
	{
		os << "}";
	}


	std::ostream &operator<<(std::ostream &os, const IrList &ir_list)
	{
		for (auto code : ir_list)
		{
			os << *code << std::endl;
		}
		return os;
	}

	bool is_same(std::shared_ptr<Var> fi_t, std::shared_ptr<Var> se_t)
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

	void small_opt(std::shared_ptr<IrList> ir_list)
	{
		for (int i = 0; i < ir_list->size(); i++)
		{
			auto cur_c = (*ir_list)[i];
			if (cur_c->ir_type == IrType::ir_assign)
			{
				auto cur = std::static_pointer_cast<IrAssign>(cur_c);
				if (!cur->r && cur->op == IrOp::op_add)
				{
					if (i > 0 && (*ir_list)[i - 1]->ir_type == ir_assign)
					{
						auto last = std::static_pointer_cast<IrAssign>((*ir_list)[i - 1]);
						if (is_same(last->aim, cur->l))
						{
							if (last->aim->var_type == VarType::var_temp && cur->aim->var_type == VarType::var_normal)
							{
								last->aim = cur->aim;
								ir_list->erase(ir_list->begin() + i);
								i--;
							}
							/*
							for (int j = i + 1; j < ir_list->size(); j++)
							{
								auto code_t = (*ir_list)[j];
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
							 */
						}
					}
				}
			}
		}
	}

}