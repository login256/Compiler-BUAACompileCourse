//
// Created by lyt on 2019/11/15.
//

#ifndef UNIVERSALCOMPILER_IR_H
#define UNIVERSALCOMPILER_IR_H

#include <string>
#include <vector>
#include "SymbolTable.h"

namespace ucc
{
	enum VarType
	{
		var_temp,
		var_normal,
		var_ret,
		var_const,
		var_array,
	};

	class Var
	{
	public:
		VarType var_type;

		Var(VarType var_type);
		/*
		static int cnt_temp;

		bool is_temp;
		int id;

		bool is_ret_value;

		std::shared_ptr<SymbolTableEntry> table_entry;
		std::shared_ptr<Var> index;

		Var(bool is_temp, bool is_ret_value = false, std::shared_ptr<SymbolTableEntry> table_entry = nullptr, std::shared_ptr<Var> index = nullptr);
		 */
	};

	class ConstVar : public Var
	{
	public:
		int value;

		ConstVar(int value);
	};

	class TempVar : public Var
	{
	public:
		static int cnt_temp;
		int id;

		int offset;

		TempVar();
	};

	class RetVar : public Var
	{
	public:
		RetVar();
	};

	class NorVar : public Var
	{
	public:
		std::shared_ptr<SymbolTableEntry> table_entry;

		NorVar(const std::shared_ptr<SymbolTableEntry> &table_entry);
	};

	class ArrayVar : public Var
	{
	public:
		std::shared_ptr<SymbolTableEntry> table_entry;
		std::shared_ptr<Var> index;

		ArrayVar(const std::shared_ptr<SymbolTableEntry> &table_entry, const std::shared_ptr<Var> &index);
	};

	struct Label
	{
		Label();

		static int cnt;
		int id;
	};

	enum IrType
	{
		IR_assign,
		IR_call,
		IR_ret,
		IR_branch,
		IR_jump,
		IR_label,
		IR_func,
		IR_write,
		IR_read,
		IR_func_end,
	};

	class IrCode
	{
	public:
		IrCode(IrType ir_type);

	public:
		IrType ir_type;
	};

	enum IrOp
	{
		op_add,
		op_sub,
		op_mul,
		op_div,
		op_ge,
		op_gt,
		op_le,
		op_lt,
		op_eq,
		op_ne,
	};

	class IrAssign : public IrCode
	{
	public:
		IrAssign(IrOp op, const std::shared_ptr<Var> &l, const std::shared_ptr<Var> &r, const std::shared_ptr<Var> &aim);

		IrAssign(IrOp op, const std::shared_ptr<Var> &l, const std::shared_ptr<Var> &aim);

	public:
		IrOp op;
		std::shared_ptr<Var> l, r, aim;
	};

	class IrCall : public IrCode
	{
	public:
		IrCall(const std::string &func);

	public:
		std::string func;
		std::vector<std::shared_ptr<Var>> vars;
		std::shared_ptr<Var> re_var;
	};

	class IrRet : public IrCode
	{
	public:
		IrRet(const std::shared_ptr<Var> &var);

		IrRet();

	public:
		bool is_void;
		std::shared_ptr<Var> var;
	};

	class IrBranch : public IrCode
	{
	public:
		IrBranch(bool is_true, const std::shared_ptr<Var> &var, std::shared_ptr<Label> label);

	public:
		bool is_true;
		std::shared_ptr<Var> var;
		std::shared_ptr<Label> label;
	};

	class IrJump : public IrCode
	{
	public:
		IrJump(std::shared_ptr<Label> label);

	public:
		std::shared_ptr<Label> label;
	};

	class IrLable : public IrCode
	{
	public:
		std::shared_ptr<Label> label;

		IrLable(std::shared_ptr<Label> label);
	};


	class IrFunc : public IrCode
	{
	public:
		std::string id;

		std::shared_ptr<SymbolTable> symbol_table;

		std::shared_ptr<std::vector<std::string>> par_list;

		IrFunc(const std::string &id, std::shared_ptr<SymbolTable> symbol_table, std::shared_ptr<std::vector<std::string>> par_list);
	};

	class IrRead : public IrCode
	{
	public:
		std::shared_ptr<NorVar> var;

		IrRead(const std::shared_ptr<NorVar> &var);
	};

	class IrWrite : public IrCode
	{
	public:
		std::shared_ptr<Var> var;
		bool as_char;

		IrWrite(const std::shared_ptr<Var> &var, bool as_char);
	};

	class IrFuncEnd : public IrCode
	{
	public:
		IrFuncEnd();
	};

	typedef std::vector<std::shared_ptr<IrCode>> IrList;
	/*
	class IrList : public std::vector<IrCode>
	{
	};
	 */

}

#endif //UNIVERSALCOMPILER_IR_H
