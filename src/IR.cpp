//
// Created by lyt on 2019/11/15.
//

#include "include/IR.h"

namespace ucc
{
	IrCode::IrCode(IrType ir_type) : ir_type(ir_type) {}

	IrJump::IrJump(std::shared_ptr<Label> label) : IrCode(IrType::IR_jump), label(label) {}

	void IrJump::print(std::ostream &stream) const
	{
		stream << "jump " << label;
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

	IrBranch::IrBranch(bool is_true, const std::shared_ptr<Var> &var, std::shared_ptr<Label> label) : IrCode(IrType::IR_branch), is_true(is_true),
																									  var(var),
																									  label(label) {}

	void IrBranch::print(std::ostream &os) const
	{
		if (is_true)
		{
			os << "beq " << var << " " << label;
		}
		else
		{
			os << "bne " << var << " " << label;
		}
	}

	IrAssign::IrAssign(IrOp op, const std::shared_ptr<Var> &l, const std::shared_ptr<Var> &r, const std::shared_ptr<Var> &aim) :
			IrCode(IrType::IR_assign), op(op), l(l), r(r), aim(aim) {}

	IrAssign::IrAssign(IrOp op, const std::shared_ptr<Var> &l, const std::shared_ptr<Var> &aim) : IrCode(IrType::IR_assign), op(op), l(l), aim(aim) {}

	void IrAssign::print(std::ostream &os) const
	{
		if (r == nullptr)
		{
			switch (op)
			{
				case IrOp::op_add:
					os << aim << " = " << l;
					break;
				case IrOp::op_sub:
					os << aim << " = -" << l;
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
			os << aim << " = " << l << " " << op_c << " " << r;
		}
	}

	IrRet::IrRet(const std::shared_ptr<Var> &var) : IrCode(IrType::IR_ret), is_void(false), var(var) {}

	IrRet::IrRet() : IrCode(IrType::IR_ret), is_void(true) {}

	void IrRet::print(std::ostream &os) const
	{
		os << "return";
		if (!is_void)
		{
			os << " " << var;
		}
	}


	IrLable::IrLable(std::shared_ptr<Label> label) : IrCode(IrType::IR_label), label(label) {}

	void IrLable::print(std::ostream &os) const
	{
		os << label << ":";
	}

	IrCall::IrCall(const std::string &func) : IrCode(IrType::IR_call), func(func) {}

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
			os << e;
			is_first = false;
		}
		os << ")";
	}

	IrFunc::IrFunc(const std::string &id, std::shared_ptr<SymbolTable> symbol_table, std::shared_ptr<std::vector<std::string>> par_list)
			: IrCode(IrType::IR_func), id(id), symbol_table(symbol_table), par_list(par_list) {}

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
			if (e.second->attributes.size() > 0) //?
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
		offset = 4;
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
		os << table_entry->id << "[" << index << "]";
	}

	std::ostream &operator<<(std::ostream &os, const IrCode &code)
	{
		code.print(os);
		return os;
	}

	IrRead::IrRead(const std::shared_ptr<NorVar> &var) : IrCode(IrType::IR_read), var(var) {}

	void IrRead::print(std::ostream &os) const
	{
		os << "read " << var;
	}

	IrWrite::IrWrite(const std::shared_ptr<Var> &var, bool as_char) : IrCode(IrType::IR_write), var(var), as_char(as_char) {}

	void IrWrite::print(std::ostream &os) const
	{
		os << "print " << var << " as " << (as_char ? "char" : "int");
	}

	IrFuncEnd::IrFuncEnd() : IrCode(IrType::IR_func_end) {}

	void IrFuncEnd::print(std::ostream &os) const
	{
		os << "}";
	}


	std::ostream &operator<<(std::ostream &os, const IrList &ir_list)
	{
		for (auto code : ir_list)
		{
			os << code << std::endl;
		}
		return os;
	}
}