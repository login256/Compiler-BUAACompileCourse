//
// Created by lyt on 2019/11/15.
//

#ifndef UNIVERSALCOMPILER_IR_H
#define UNIVERSALCOMPILER_IR_H

#include <string>
#include <vector>
#include <ostream>
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

		virtual void print(std::ostream &os) const = 0;
	};

	std::ostream &operator<<(std::ostream &os, const Var &var);

	class ConstVar : public Var
	{
	public:
		int value;

		ConstVar(int value);

		void print(std::ostream &os) const override;
	};

	class TempVar : public Var
	{
	public:
		static int cnt_temp;
		int id;

		int offset;

		TempVar();

		void print(std::ostream &os) const override;
	};

	class RetVar : public Var
	{
	public:
		RetVar();

		void print(std::ostream &os) const override;
	};

	class NorVar : public Var
	{
	public:
		std::shared_ptr<SymbolTableEntry> table_entry;

		NorVar(const std::shared_ptr<SymbolTableEntry> &table_entry);

		void print(std::ostream &os) const override;
	};

	class ArrayVar : public Var
	{
	public:
		std::shared_ptr<SymbolTableEntry> table_entry;
		std::shared_ptr<Var> index;

		ArrayVar(const std::shared_ptr<SymbolTableEntry> &table_entry, const std::shared_ptr<Var> &index);

		void print(std::ostream &os) const override;
	};

	struct Label
	{
		Label();

		static int cnt;
		int id;

		friend std::ostream &operator<<(std::ostream &os, const Label &label);
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

		virtual void print(std::ostream &os) const = 0;

		friend std::ostream &operator<<(std::ostream &os, const IrCode &code);
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

		void print(std::ostream &os) const override;
	};

	class IrCall : public IrCode
	{
	public:
		IrCall(const std::string &func);

		void print(std::ostream &os) const override;

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

		void print(std::ostream &os) const override;

	public:
		bool is_void;
		std::shared_ptr<Var> var;
	};

	class IrBranch : public IrCode
	{
	public:
		IrBranch(bool is_true, const std::shared_ptr<Var> &var, std::shared_ptr<Label> label);

		void print(std::ostream &os) const override;

	public:
		bool is_true;
		std::shared_ptr<Var> var;
		std::shared_ptr<Label> label;
	};

	class IrJump : public IrCode
	{
	public:
		IrJump(std::shared_ptr<Label> label);

		void print(std::ostream &stream) const override;

	public:
		std::shared_ptr<Label> label;
	};

	class IrLable : public IrCode
	{
	public:
		std::shared_ptr<Label> label;

		IrLable(std::shared_ptr<Label> label);

		void print(std::ostream &os) const override;
	};


	class IrFunc : public IrCode
	{
	public:
		std::string id;

		std::shared_ptr<SymbolTable> symbol_table;

		std::shared_ptr<std::vector<std::string>> par_list;

		IrFunc(const std::string &id, std::shared_ptr<SymbolTable> symbol_table, std::shared_ptr<std::vector<std::string>> par_list);

		void print(std::ostream &os) const override;
	};

	class IrRead : public IrCode
	{
	public:
		std::shared_ptr<NorVar> var;

		IrRead(const std::shared_ptr<NorVar> &var);

		void print(std::ostream &os) const override;
	};

	class IrWrite : public IrCode
	{
	public:
		std::shared_ptr<Var> var;
		bool as_char;

		IrWrite(const std::shared_ptr<Var> &var, bool as_char);

		void print(std::ostream &os) const override;
	};

	class IrFuncEnd : public IrCode
	{
	public:
		IrFuncEnd();

		void print(std::ostream &os) const override;
	};

	typedef std::vector<std::shared_ptr<IrCode>> IrList;

	std::ostream &operator<<(std::ostream &os, const IrList &ir_list);

	/*
	class IrList : public std::vector<IrCode>
	{
	};
	 */

}

#endif //UNIVERSALCOMPILER_IR_H
