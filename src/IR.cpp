//
// Created by lyt on 2019/11/15.
//

#include "include/IR.h"

namespace ucc
{
	IrCode::IrCode(IrType ir_type) : ir_type(ir_type) {}

	IrJump::IrJump(std::shared_ptr<Label> label) : IrCode(IrType::IR_jump), label(label) {}

	int Label::cnt;

	Label::Label()
	{
		id = ++cnt;
	}

	IrBranch::IrBranch(bool is_true, const std::shared_ptr<Var> &var, std::shared_ptr<Label> label) : IrCode(IrType::IR_branch), is_true(is_true),
																									  var(var),
																									  label(label) {}

	IrAssign::IrAssign(IrOp op, const std::shared_ptr<Var> &l, const std::shared_ptr<Var> &r, const std::shared_ptr<Var> &aim) :
			IrCode(IrType::IR_assign), op(op), l(l), r(r), aim(aim) {}

	IrAssign::IrAssign(IrOp op, const std::shared_ptr<Var> &l, const std::shared_ptr<Var> &aim) : IrCode(IrType::IR_assign), op(op), l(l), aim(aim) {}

	IrRet::IrRet(const std::shared_ptr<Var> &var) : IrCode(IrType::IR_ret), is_void(false), var(var) {}

	IrRet::IrRet() : IrCode(IrType::IR_ret), is_void(true) {}


	IrLable::IrLable(std::shared_ptr<Label> label) : IrCode(IrType::IR_label), label(label) {}

	IrCall::IrCall(const std::string &func) : IrCode(IrType::IR_call), func(func) {}

	IrFunc::IrFunc(const std::string &id, std::shared_ptr<SymbolTable> symbol_table) : IrCode(IrType::IR_func), id(id),
																							   symbol_table(symbol_table) {}

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

	ConstVar::ConstVar(int value) : Var(VarType::var_const), value(value) {}

	int TempVar::cnt_temp;

	TempVar::TempVar() : Var(VarType::var_temp)
	{
		this->id = ++cnt_temp;
		offset = 0;
	}

	RetVar::RetVar() : Var(VarType::var_ret) {}

	NorVar::NorVar(const std::shared_ptr<SymbolTableEntry> &table_entry) : Var(VarType::var_normal), table_entry(table_entry) {}

	ArrayVar::ArrayVar(const std::shared_ptr<SymbolTableEntry> &table_entry, const std::shared_ptr<Var> &index)
			: Var(VarType::var_array), table_entry(table_entry), index(index) {}

	IrRead::IrRead(const std::shared_ptr<NorVar> &var) : IrCode(IrType::IR_read), var(var) {}

	IrWrite::IrWrite(const std::shared_ptr<Var> &var, bool as_char) : IrCode(IrType::IR_write), var(var), as_char(as_char) {}

	IrFuncEnd::IrFuncEnd() : IrCode(IrType::IR_func_end) {}
}