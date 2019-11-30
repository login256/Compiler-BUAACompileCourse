//
// Created by lyt on 2019/10/12.
//
#include <memory>
#include <iostream>
#include <set>
#include "include/Parser.h"
#include "include/Token.h"
#include "include/homework/error_homework.h"


#define SEMANTICERROR

namespace ucc
{
	TokenBuffer::TokenBuffer(Lexer &lexer) : lexer(lexer)
	{
		last = std::make_shared<Token>(TokenType::token_eof, -1);
	}

	std::shared_ptr<Token> TokenBuffer::at(int pos)
	{
		while (que.size() <= pos)
		{
			que.push_back(lexer.get_next_token());
		}
		return que.at(pos);
	}

	std::shared_ptr<Token> TokenBuffer::front()
	{
		return at(0);
	}

	std::shared_ptr<Token> TokenBuffer::pop()
	{
		if (que.empty())
		{
			que.push_back(lexer.get_next_token());
		}
		auto re = que.front();
		last = re;
		que.pop_front();
		return re;
	}

	void TokenBuffer::back_up(const std::shared_ptr<Token> &token)
	{
		que.push_front(token);
	}

	std::shared_ptr<Token> TokenBuffer::get_last() const
	{
		return last;
	}

	Parser::Parser(GrammerOutputer &outputer, Lexer &lexer) : outputer(outputer), buffer(lexer) {}

	Parser::Parser(Lexer &lexer) : outputer(*(new DefaultGrammerOutputer())), buffer(lexer) {}

	void Parser::output(SyntaxType type)
	{
		outputer.syntax_unit_output(type);
	}

	void Parser::output(std::shared_ptr<Token> token)
	{
		outputer.token_output(token);
	}

	void Parser::output_func(bool is_void)
	{
		outputer.syntax_func_fucking_output(is_void);
	}

	void Parser::get_next()
	{
		auto re = buffer.pop();
		output(re);
	}

	static void wrong_in_parser(std::string str)
	{
		std::cerr << str << std::endl;
		while (1) {}
		//exit(0);
	}

#define MUST_HAVE(v_v) \
{\
    if (buffer.front()->get_type() != (v_v))\
    {\
        wrong_in_parser(std::string("no ").append(#v_v));\
    }\
}

#define MUST_BE(v_v) \
{\
    MUST_HAVE(v_v);\
    get_next();\
}

#define IS_TOKEN(v_v) (buffer.front()->get_type() == (v_v))

#define CHARSIZE 4

	void Parser::must_and_error(TokenType token_type, ErrorType error_type, bool front)
	{
		if (buffer.front()->get_type() != token_type)
		{
			int line;
			if (front)
			{
				line = buffer.get_last()->get_line();
			}
			else
			{
				line = buffer.front()->get_line();
			}
			error_handler(Error(error_type, line));
		}
		else
		{
			get_next();
		}
	}

	IrOp Parser::parse_add()
	{
		IrOp re;
		if (IS_TOKEN(TokenType::token_plus))
		{
			get_next();
			re = IrOp::op_add;
		}
		else if (IS_TOKEN(TokenType::token_minus))
		{
			get_next();
			re = IrOp::op_sub;
		}
		else
		{
			wrong_in_parser("no + -");
		}
		output(SyntaxType::syntax_add);
		return re;
	}

	IrOp Parser::parse_mul()
	{
		IrOp re;
		if (IS_TOKEN(TokenType::token_mul))
		{
			get_next();
			re = IrOp::op_mul;
		}
		else if (IS_TOKEN(TokenType::token_div))
		{
			get_next();
			re = IrOp::op_div;
		}
		else
		{
			wrong_in_parser("no * /");
		}
		output(SyntaxType::syntax_mul);
		return re;
	}

	IrOp Parser::parse_rel()
	{
		IrOp re;
		switch (buffer.front()->get_type())
		{
			case TokenType::token_les:
			{
				re = IrOp::op_lt;
				get_next();
				break;
			}
			case TokenType::token_leq:
			{
				re = IrOp::op_le;
				get_next();
				break;
			}
			case TokenType::token_grt:
			{
				re = IrOp::op_gt;
				get_next();
				break;
			}
			case TokenType::token_geq:
			{
				re = IrOp::op_ge;
				get_next();
				break;
			}
			case TokenType::token_neq:
			{
				re = IrOp::op_ne;
				get_next();
				break;
			}
			case TokenType::token_eql:
			{
				re = IrOp::op_eq;
				get_next();
				break;
			}
			default:
				wrong_in_parser("no rel");
		}
		output(syntax_rel);
		return re;
	}

	char Parser::parse_char()
	{
		MUST_HAVE(TokenType::token_cchar);
		auto cur_char_token = std::static_pointer_cast<CharToken>(buffer.front());
		get_next();
		output(SyntaxType::syntax_char);
		return cur_char_token->get_value()[0];
	}

	std::string Parser::parse_string()
	{
		MUST_HAVE(TokenType::token_cstring);
		auto cur_str_token = std::static_pointer_cast<StringToken>(buffer.front());
		get_next();
		output(SyntaxType::syntax_string);
		return cur_str_token->get_value();
	}

	void Parser::parse_program()
	{
		cur_symbol_table = std::make_shared<SymbolTable>();
		if (buffer.front()->get_type() == TokenType::token_const)
		{
			parse_const_declare(SymbolScope::scope_global);
		}
		if (buffer.at(2)->get_type() == TokenType::token_comma ||
			buffer.at(2)->get_type() == TokenType::token_lbrackets ||
			buffer.at(2)->get_type() == TokenType::token_semi)
		{
			parse_var_declare(SymbolScope::scope_global);
		}
		while (buffer.at(1)->get_type() == TokenType::token_id)
		{
			if (buffer.front()->get_type() == TokenType::token_void)
			{
				parse_void_func();
			}
			else if (IS_TOKEN(TokenType::token_int) || IS_TOKEN(TokenType::token_char))
			{
				parse_func();
			}
			else
			{
				wrong_in_parser("no type for func");
			}
		}
		parse_main_func();
		output(SyntaxType::syntax_program);
	}

	void Parser::parse_const_declare(SymbolScope scope)
	{
		MUST_BE(TokenType::token_const);
		parse_const_define(scope);
		MUST_BE(TokenType::token_semi);
		while (IS_TOKEN(TokenType::token_const))
		{
			get_next();
			parse_const_define(scope);
			MUST_BE(TokenType::token_semi);
		}
		output(SyntaxType::syntax_const_declare);
	}

	void Parser::parse_const_define(SymbolScope scope)
	{
		SymbolData data_type;
		int size = 0;
		switch (buffer.front()->get_type())
		{
			case TokenType::token_int:
			{
				get_next();
				data_type = SymbolData::data_int;
				size = 4;
				break;
			}
			case TokenType::token_char:
			{
				get_next();
				data_type = SymbolData::data_char;
				size = CHARSIZE;
				break;
			}
			default:
				wrong_in_parser("no int/char int const define");
		}
		auto cur_entry = SymbolTableEntry(parse_id(), SymbolType::varible, data_type, size, {SymbolAttribute::att_const});
		int init_value;
		MUST_BE(TokenType::token_ass);
		if (data_type == data_int)
		{
			if (buffer.front()->get_type() != TokenType::token_number &&
				!((buffer.front()->get_type() == TokenType::token_plus || buffer.front()->get_type() == TokenType::token_minus) &&
				  buffer.at(1)->get_type() == TokenType::token_number)
					)
			{
				error_handler(Error(ErrorType::const_define_bad, buffer.front()->get_line()));
				while (buffer.front()->get_type() != TokenType::token_comma && buffer.front()->get_type() != TokenType::token_semi)
				{
					get_next();
				}
			}
			else
			{
				init_value = parse_int();
			}
		}
		else
		{
			if (buffer.front()->get_type() != TokenType::token_cchar)
			{
				error_handler(Error(ErrorType::const_define_bad, buffer.front()->get_line()));
				while (buffer.front()->get_type() != TokenType::token_comma && buffer.front()->get_type() != TokenType::token_semi)
				{
					get_next();
				}
			}
			else
			{
				init_value = parse_char();
			}
		}
		cur_entry.init_value = init_value;
		if (!cur_symbol_table->add(cur_entry))
		{
			error_handler(Error(ErrorType::id_redefine, buffer.get_last()->get_line()));
		}
		while (IS_TOKEN(TokenType::token_comma))
		{
			get_next();
			cur_entry = SymbolTableEntry(parse_id(), SymbolType::varible, data_type, size, {SymbolAttribute::att_const});
			MUST_BE(TokenType::token_ass);
			if (data_type == data_int)
			{
				if (buffer.front()->get_type() != TokenType::token_number &&
					!((buffer.front()->get_type() == TokenType::token_plus || buffer.front()->get_type() == TokenType::token_minus) &&
					  buffer.at(1)->get_type() == TokenType::token_number)
						)
				{
					error_handler(Error(ErrorType::const_define_bad, buffer.front()->get_line()));
					while (buffer.front()->get_type() != TokenType::token_comma && buffer.front()->get_type() != TokenType::token_semi)
					{
						get_next();
					}
				}
				else
				{
					init_value = parse_int();
				}
			}
			else
			{
				if (buffer.front()->get_type() != TokenType::token_cchar)
				{
					error_handler(Error(ErrorType::const_define_bad, buffer.front()->get_line()));
					while (buffer.front()->get_type() != TokenType::token_comma && buffer.front()->get_type() != TokenType::token_semi)
					{
						get_next();
					}
				}
				else
				{
					init_value = parse_char();
				}
			}
			cur_entry.init_value = init_value;
			if (!cur_symbol_table->add(cur_entry))
			{
				error_handler(Error(ErrorType::id_redefine, buffer.get_last()->get_line()));
			}
		}
		output(SyntaxType::syntax_const_define);
	}

	int Parser::parse_uint()
	{
		MUST_HAVE(TokenType::token_number);
		auto cur_num_token = std::static_pointer_cast<NumToken>(buffer.front());
		get_next();
		output(SyntaxType::syntax_uint);
		return cur_num_token->get_value();
	}

	int Parser::parse_int()
	{
		int neg = 1;
		if (IS_TOKEN(TokenType::token_plus))
		{
			get_next();
		}
		else if (IS_TOKEN(TokenType::token_minus))
		{
			neg = -1;
			get_next();
		}
		int v = parse_uint();
		output(SyntaxType::syntax_int);
		return neg * v;
	}

	std::string Parser::parse_id()
	{
		MUST_HAVE(TokenType::token_id);
		auto cur_token = std::static_pointer_cast<IdToken>(buffer.front());
		get_next();
		output(SyntaxType::syntax_id);
		return cur_token->get_value();
	}

	std::string Parser::parse_declare_header()
	{
		SymbolData data;
		if (IS_TOKEN(TokenType::token_int))
		{
			data = SymbolData::data_int;
		}
		else if (IS_TOKEN(TokenType::token_char))
		{
			data = SymbolData::data_char;
		}
		else
		{
			wrong_in_parser("no int/char");
		}
		get_next();
		auto id = parse_id();
		if (!cur_symbol_table->add(SymbolTableEntry(id, SymbolType::func, data, 0)))
		{
			error_handler(Error(ErrorType::id_redefine, buffer.get_last()->get_line()));
			id = id + "$$wrong_func";
		}
		output(SyntaxType::syntax_declare_header);
		return id;
	}

	void Parser::parse_var_declare(SymbolScope scope)
	{
		parse_var_define(scope);
		MUST_BE(TokenType::token_semi);
		while ((IS_TOKEN(TokenType::token_int) || IS_TOKEN(TokenType::token_char)) &&
			   buffer.at(2)->get_type() != TokenType::token_lpar)
		{
			parse_var_define(scope);
			MUST_BE(TokenType::token_semi);
		}
		output(SyntaxType::syntax_var_declare);
	}

	void Parser::parse_var_define(SymbolScope scope)
	{
		SymbolData data;
		int size = 0;
		data = parse_var_type();
		if (data == data_int)
		{
			size = 4;
		}
		else
		{
			size = CHARSIZE;
		}
		std::string cur_id = parse_id();
		int tsize = size;
		SymbolType type = SymbolType::varible;
		if (IS_TOKEN(TokenType::token_lbrackets))
		{
			get_next();
			type = SymbolType::array;
			tsize = size * parse_uint();
			if (tsize == 0)
			{
				wrong_in_parser("array size 0");
			}
			must_and_error(TokenType::token_rbrackets, ErrorType::should_be_rbrac, false);
		}
		if (!cur_symbol_table->add(SymbolTableEntry(cur_id, type, data, tsize)))
		{
			error_handler(Error(ErrorType::id_redefine, buffer.get_last()->get_line()));
		}
		while (IS_TOKEN(TokenType::token_comma))
		{
			get_next();
			cur_id = parse_id();
			tsize = size;
			type = SymbolType::varible;
			if (IS_TOKEN(TokenType::token_lbrackets))
			{
				get_next();
				type = SymbolType::array;
				tsize = size * parse_uint();
				if (tsize == 0)
				{
					wrong_in_parser("array size 0");
				}
				must_and_error(TokenType::token_rbrackets, ErrorType::should_be_rbrac, false);
			}
			if (!cur_symbol_table->add(SymbolTableEntry(cur_id, type, data, tsize)))
			{
				error_handler(Error(ErrorType::id_redefine, buffer.get_last()->get_line()));
			}
		}
		output(SyntaxType::syntax_var_define);
	}

	SymbolData Parser::parse_var_type()
	{
		SymbolData re;
		if (IS_TOKEN(TokenType::token_int))
		{
			re = SymbolData::data_int;
		}
		else if (IS_TOKEN(TokenType::token_char))
		{
			re = SymbolData::data_char;
		}
		else
		{
			wrong_in_parser("no int/char int const define");
		}
		get_next();
		output(SyntaxType::syntax_var_type);
		return re;
	}

	void Parser::parse_func()
	{
#ifdef SEMANTICERROR
		ErrorData::has_void_return = false;
		ErrorData::has_char_return = false;
		ErrorData::has_int_return = false;
		int line = buffer.front()->get_line();
#endif
		std::string id = parse_declare_header();
#ifdef SEMANTICERROR
		ErrorData::cur_func_data_type = cur_symbol_table->find(id)->data;
#endif
		cur_symbol_table = std::make_shared<SymbolTable>(cur_symbol_table);
		MUST_BE(TokenType::token_lpar);
		parse_par_list(func_args[id], func_args_id[id]);
		must_and_error(TokenType::token_rpar, ErrorType::should_be_rpar, false);
		ir_list->push_back(std::make_shared<IrFunc>(id, cur_symbol_table, std::make_shared<std::vector<std::string>>(func_args_id[id])));
		MUST_BE(TokenType::token_lbrace);
		parse_compound(false);
		MUST_BE(TokenType::token_rbrace);
		ir_list->push_back(std::make_shared<IrFuncEnd>());
		cur_symbol_table = cur_symbol_table->get_par();
		output(SyntaxType::syntax_func);
#ifdef SEMANTICERROR
		auto data_type = cur_symbol_table->find(id)->data;
		if (!ErrorData::has_void_return && ErrorData::has_int_return && ErrorData::has_char_return)
		{
			error_handler(Error(ErrorType::no_bad_return, line));
		}
#endif
	}

	void Parser::parse_void_func()
	{
#ifdef SEMANTICERROR
		ErrorData::has_void_return = false;
		ErrorData::has_char_return = false;
		ErrorData::has_int_return = false;
		int line = buffer.front()->get_line();
		ErrorData::cur_func_data_type = SymbolData::data_void;
#endif
		MUST_BE(TokenType::token_void);
		std::string cur_id = parse_id();
		if (!cur_symbol_table->add(SymbolTableEntry(cur_id, SymbolType::func, SymbolData::data_void, 0)))
		{
			error_handler(Error(ErrorType::id_redefine, buffer.get_last()->get_line()));
			cur_id = cur_id + "$$wrong_func";
		}
		cur_symbol_table = std::make_shared<SymbolTable>(cur_symbol_table);
		MUST_BE(TokenType::token_lpar);
		parse_par_list(func_args[cur_id], func_args_id[cur_id]);
		must_and_error(TokenType::token_rpar, ErrorType::should_be_rpar, false);
		ir_list->push_back(std::make_shared<IrFunc>(cur_id, cur_symbol_table, std::make_shared<std::vector<std::string>>(func_args_id[cur_id])));
		MUST_BE(TokenType::token_lbrace);
		parse_compound(false);
		MUST_BE(TokenType::token_rbrace);
		ir_list->push_back(std::make_shared<IrFuncEnd>());
		cur_symbol_table = cur_symbol_table->get_par();
		output(SyntaxType::syntax_void_func);
#ifdef SEMANTICERROR
		/*
		if (ErrorData::has_char_return || ErrorData::has_int_return)
		{
			error_handler(Error(ErrorType::void_bad_return, line));
		}
		*/
#endif
	}

	void Parser::parse_compound(bool new_symbol_table)
	{
		if (new_symbol_table)
		{
			cur_symbol_table = std::make_shared<SymbolTable>(cur_symbol_table);
		}
		if (IS_TOKEN(TokenType::token_const))
		{
			parse_const_declare(SymbolScope::scope_local);
		}
		if (IS_TOKEN(TokenType::token_int) || IS_TOKEN(TokenType::token_char))
		{
			parse_var_declare(SymbolScope::scope_local);
		}
		parse_state_list();
		if (new_symbol_table)
		{
			cur_symbol_table = cur_symbol_table->get_par();
		}
		output(SyntaxType::syntax_compound);
	}

	void Parser::parse_par_list(std::vector<SymbolData> &par_list, std::vector<std::string> &par_id)
	{
		if (IS_TOKEN(TokenType::token_int) || IS_TOKEN(TokenType::token_char))
		{
			auto data_type = parse_var_type();
			auto cur_id = parse_id();
			int size = 0;
			if (data_type == data_int)
			{
				size = 4;
			}
			else
			{
				size = CHARSIZE;
			}
			if (!cur_symbol_table->add(SymbolTableEntry(cur_id, SymbolType::varible, data_type, size)))
			{
				error_handler(Error(ErrorType::id_redefine, buffer.get_last()->get_line()));
			}
			par_list.push_back(data_type);
			par_id.push_back(cur_id);
			while (IS_TOKEN(TokenType::token_comma))
			{
				get_next();
				if (!(IS_TOKEN(TokenType::token_int) || IS_TOKEN(TokenType::token_char)))
				{
					wrong_in_parser("no int/char int par list");
				}
				data_type = parse_var_type();
				cur_id = parse_id();
				if (data_type == data_int)
				{
					size = 4;
				}
				else
				{
					size = CHARSIZE;
				}
				if (!cur_symbol_table->add(SymbolTableEntry(cur_id, SymbolType::varible, data_type, size)))
				{
					error_handler(Error(ErrorType::id_redefine, buffer.get_last()->get_line()));
				}
				par_list.push_back(data_type);
				par_id.push_back(cur_id);
			}
		}
		output(SyntaxType::syntax_par_list);
	}

	void Parser::parse_main_func()
	{
		MUST_BE(TokenType::token_void);
		MUST_BE(TokenType::token_main);
		cur_symbol_table = std::make_shared<SymbolTable>(cur_symbol_table);
		MUST_BE(TokenType::token_lpar);
		must_and_error(TokenType::token_rpar, ErrorType::should_be_rpar, false);
		MUST_BE(TokenType::token_lbrace);
		ir_list->push_back(std::make_shared<IrFunc>("main", cur_symbol_table, std::make_shared<std::vector<std::string>>()));
		parse_compound(false);
		MUST_BE(TokenType::token_rbrace);
		ir_list->push_back(std::make_shared<IrFuncEnd>());
		cur_symbol_table = cur_symbol_table->get_par();
		output(SyntaxType::syntax_main_func);
	}

	SymbolData Parser::parse_exp(std::shared_ptr<Var> &var)
	{
#ifdef SEMANTICERROR
		ErrorData::is_char_exp.push(true);
#endif
		int neg = 1;
		if (IS_TOKEN(TokenType::token_plus))
		{
#ifdef SEMANTICERROR
			ErrorData::is_char_exp.top() = false;
#endif
			get_next();
		}
		else if (IS_TOKEN(TokenType::token_minus))
		{
#ifdef SEMANTICERROR
			ErrorData::is_char_exp.top() = false;
#endif
			neg = -1;
			get_next();
		}
		std::shared_ptr<Var> curt;
		parse_term(curt);
		if (neg == -1)
		{
			std::shared_ptr<Var> tempt = std::make_shared<TempVar>();
			ir_list->push_back(std::make_shared<IrAssign>(IrOp::op_sub, curt, tempt));
			curt = tempt;
		}
		while (IS_TOKEN(TokenType::token_plus) || IS_TOKEN(TokenType::token_minus))
		{
#ifdef SEMANTICERROR
			ErrorData::is_char_exp.top() = false;
#endif
			IrOp curop = parse_add();
			std::shared_ptr<Var> rt;
			parse_term(rt);
			std::shared_ptr<Var> tempt = std::make_shared<TempVar>();
			ir_list->push_back(std::make_shared<IrAssign>(curop, curt, rt, tempt));
			curt = tempt;
		}
		output(SyntaxType::syntax_exp);
		var = curt;
#ifdef SEMANTICERROR
		bool v = ErrorData::is_char_exp.top();
		ErrorData::is_char_exp.pop();
		if (v)
		{
			return SymbolData::data_char;
		}
		else
		{
			return SymbolData::data_int;
		}
#endif
		return SymbolData::data_void;
	}

	void Parser::parse_term(std::shared_ptr<Var> &var)
	{
		std::shared_ptr<Var> curt;
		parse_fact(curt);
		while (IS_TOKEN(TokenType::token_mul) || IS_TOKEN(TokenType::token_div))
		{
#ifdef SEMANTICERROR
			ErrorData::is_char_exp.top() = false;
#endif
			auto cur_op = parse_mul();
			std::shared_ptr<Var> rt;
			parse_fact(rt);
			std::shared_ptr<Var> tempt = std::make_shared<TempVar>();
			ir_list->push_back(std::make_shared<IrAssign>(cur_op, curt, rt, tempt));
			curt = tempt;
		}
		output(SyntaxType::syntax_term);
		var = curt;
	}

	void Parser::parse_fact(std::shared_ptr<Var> &var)
	{
		switch (buffer.front()->get_type())
		{
			case TokenType::token_id :
			{
				if (buffer.at(1)->get_type() == TokenType::token_lpar)
				{
					parse_func_call(false);
					auto ret_var = std::make_shared<RetVar>();
					var = std::make_shared<TempVar>();
					ir_list->push_back(std::make_shared<IrAssign>(IrOp::op_add, ret_var, var));
				}
				else
				{
					std::string id = parse_id();
					auto entry = cur_symbol_table->find(id);
#ifdef SEMANTICERROR
					if (!entry)
					{
						error_handler(Error(ErrorType::undefined_id, buffer.get_last()->get_line()));
					}
					else if (entry->data == SymbolData::data_int)
					{
						ErrorData::is_char_exp.top() = false;
					}
#endif
					if (IS_TOKEN(TokenType::token_lbrackets))
					{
						get_next();
						std::shared_ptr<Var> index;
#ifdef SEMANTICERROR
						auto v =
#endif
						parse_exp(index);
						auto index_t = std::make_shared<TempVar>();
						ir_list->push_back(std::make_shared<IrAssign>(IrOp::op_add, index, index_t));
						var = std::make_shared<ArrayVar>(entry, index_t);
#ifdef SEMANTICERROR
						if (v == SymbolData::data_char)
						{
							error_handler(Error(ErrorType::index_bad, buffer.get_last()->get_line()));
						}
#endif
						must_and_error(TokenType::token_rbrackets, ErrorType::should_be_rbrac, false);
					}
					else
					{
						var = std::make_shared<NorVar>(entry);
					}
				}
				break;
			}
			case TokenType::token_lpar :
			{
#ifdef SEMANTICERROR
				ErrorData::is_char_exp.top() = false;
#endif
				get_next();
				std::shared_ptr<Var> son_var;
				parse_exp(son_var);
				var = std::make_shared<TempVar>();
				ir_list->push_back(std::make_shared<IrAssign>(IrOp::op_add, son_var, var));
				must_and_error(TokenType::token_rpar, ErrorType::should_be_rpar, false);
				break;
			}
			case TokenType::token_plus :
			case TokenType::token_minus :
			case TokenType::token_number:
			{
#ifdef SEMANTICERROR
				ErrorData::is_char_exp.top() = false;
#endif
				int v = parse_int();
				var = std::make_shared<ConstVar>(v);
				break;
			}
			case TokenType::token_cchar :
			{
				char c = parse_char();
				var = std::make_shared<ConstVar>(c);
				break;
			}
			default:
				wrong_in_parser("wrong fact");
		}
		output(SyntaxType::syntax_fact);
	}

	void Parser::parse_state()
	{
		switch (buffer.front()->get_type())
		{
			case TokenType::token_if :
			{
				parse_con_state();
				break;
			}
			case TokenType::token_for :
			case TokenType::token_while :
			case TokenType::token_do :
			{
				parse_loop_state();
				break;
			}
			case TokenType::token_lbrace :
			{
				get_next();
				parse_state_list();
				MUST_BE(TokenType::token_rbrace);
				break;
			}
			case TokenType::token_id :
			{
				if (buffer.at(1)->get_type() == TokenType::token_lpar)
				{
#ifdef SEMANTICERROR
					ErrorData::is_char_exp.push(true);
#endif
					parse_func_call();
#ifdef SEMANTICERROR
					ErrorData::is_char_exp.pop();
#endif
					//MUST_BE(TokenType::token_semi);
					must_and_error(TokenType::token_semi, ErrorType::should_be_semi, true);
				}
				else //if (1 is =)
				{
					parse_ass_state();
					//MUST_BE(TokenType::token_semi);
					must_and_error(TokenType::token_semi, ErrorType::should_be_semi, true);
				}
				break;
			}
			case TokenType::token_scanf :
			{
				parse_read_state();
				//MUST_BE(TokenType::token_semi);
				must_and_error(TokenType::token_semi, ErrorType::should_be_semi, true);
				break;
			}
			case TokenType::token_printf :
			{
				parse_write_state();
				//MUST_BE(TokenType::token_semi);
				must_and_error(TokenType::token_semi, ErrorType::should_be_semi, true);
				break;
			}
			case TokenType::token_semi :
			{
				get_next();
				break;
			}
			case TokenType::token_return :
			{
				parse_return_state();
				//MUST_BE(TokenType::token_semi);
				must_and_error(TokenType::token_semi, ErrorType::should_be_semi, true);
				break;
			}
			default:
				wrong_in_parser("wrong token in state");
		}
		output(SyntaxType::syntax_state);
	}

	void Parser::parse_ass_state()
	{
		std::string id = parse_id();
		auto entry = cur_symbol_table->find(id);
		std::shared_ptr<Var> l_val;
#ifdef SEMANTICERROR
		if (!entry)
		{
			error_handler(Error(ErrorType::undefined_id, buffer.get_last()->get_line()));
		}
		else
		{
			auto att_set = std::set<SymbolAttribute>(entry->attributes.begin(), entry->attributes.end());
			if (att_set.find(SymbolAttribute::att_const) != att_set.end())
			{
				error_handler(Error(ErrorType::const_var_changed, buffer.get_last()->get_line()));
			}
		}
#endif
		if (IS_TOKEN(TokenType::token_lbrackets))
		{
			get_next();
			std::shared_ptr<Var> index;
#ifdef SEMANTICERROR
			auto v =
#endif
			parse_exp(index);
			auto index_t = std::make_shared<TempVar>();
			ir_list->push_back(std::make_shared<IrAssign>(IrOp::op_add, index, index_t));
			l_val = std::make_shared<ArrayVar>(entry, index_t);
#ifdef SEMANTICERROR
			if (v == SymbolData::data_char)
			{
				error_handler(Error(ErrorType::index_bad, buffer.get_last()->get_line()));
			}
#endif
			must_and_error(TokenType::token_rbrackets, ErrorType::should_be_rbrac, false);
		}
		else
		{
			l_val = std::make_shared<NorVar>(entry);
		}
		MUST_BE(TokenType::token_ass);
		std::shared_ptr<Var> r_val;
		parse_exp(r_val);
		ir_list->push_back(std::make_shared<IrAssign>(IrOp::op_add, r_val, l_val));
		output(SyntaxType::syntax_ass_state);
	}

	void Parser::parse_con_state()
	{
		auto label_else = std::make_shared<Label>();
		auto label_end = std::make_shared<Label>();
		MUST_BE(TokenType::token_if);
		MUST_BE(TokenType::token_lpar);
		std::shared_ptr<Var> con_var;
		parse_con(con_var);
		ir_list->push_back(std::make_shared<IrBranch>(false, con_var, label_else));
		must_and_error(TokenType::token_rpar, ErrorType::should_be_rpar, false);
		parse_state();
		ir_list->push_back(std::make_shared<IrJump>(label_end));
		ir_list->push_back(std::make_shared<IrLable>(label_else));
		if (IS_TOKEN(TokenType::token_else))
		{
			get_next();
			parse_state();
		}
		ir_list->push_back(std::make_shared<IrLable>(label_end));
		output(SyntaxType::syntax_con_state);
	}

	void Parser::parse_con(std::shared_ptr<Var> &var)
	{
#ifdef SEMANTICERROR
		auto exp_type1 =
#endif
		parse_exp(var);
#ifdef SEMANTICERROR
		if (exp_type1 != SymbolData::data_int)
		{
			error_handler(Error(ErrorType::bad_in_cond, buffer.get_last()->get_line()));
		}
#endif
		switch (buffer.front()->get_type())
		{
			case TokenType::token_les:
			case TokenType::token_leq:
			case TokenType::token_grt:
			case TokenType::token_geq:
			case TokenType::token_neq:
			case TokenType::token_eql:
			{
				IrOp op = parse_rel();
				std::shared_ptr<Var> r_var;
#ifdef SEMANTICERROR
				auto exp_type2 =
#endif
				parse_exp(r_var);
				auto tempt = std::make_shared<TempVar>();
				ir_list->push_back(std::make_shared<IrAssign>(op, var, r_var, tempt));
				var = tempt;
#ifdef SEMANTICERROR
				if (exp_type2 != data_int)
				{
					error_handler(Error(ErrorType::bad_in_cond, buffer.get_last()->get_line()));
				}
#endif
				break;
			}
			default:
			{
				break;
			}
		}
		output(SyntaxType::syntax_con);
	}

	void Parser::parse_loop_state()
	{
		switch (buffer.front()->get_type())
		{
			case TokenType::token_while :
			{
				auto label_begin = std::make_shared<Label>();
				auto label_end = std::make_shared<Label>();
				ir_list->push_back(std::make_shared<IrLable>(label_begin));
				get_next();
				MUST_BE(TokenType::token_lpar);
				std::shared_ptr<Var> con_var;
				parse_con(con_var);
				ir_list->push_back(std::make_shared<IrBranch>(false, con_var, label_end));
				must_and_error(TokenType::token_rpar, ErrorType::should_be_rpar, false);
				parse_state();
				ir_list->push_back(std::make_shared<IrJump>(label_begin));
				ir_list->push_back(std::make_shared<IrLable>(label_end));
				break;
			}
			case TokenType::token_do :
			{
				auto label_begin = std::make_shared<Label>();
				ir_list->push_back(std::make_shared<IrLable>(label_begin));
				get_next();
				parse_state();
				must_and_error(TokenType::token_while, ErrorType::no_while, 0);
				MUST_BE(TokenType::token_lpar);
				std::shared_ptr<Var> con_var;
				parse_con(con_var);
				ir_list->push_back(std::make_shared<IrBranch>(true, con_var, label_begin));
				must_and_error(TokenType::token_rpar, ErrorType::should_be_rpar, false);
				break;
			}
			case TokenType::token_for :
			{
				auto label_begin = std::make_shared<Label>();
				auto label_end = std::make_shared<Label>();
				get_next();
				MUST_BE(TokenType::token_lpar);
				auto id = parse_id();
				auto entry = cur_symbol_table->find(id);
				std::shared_ptr<Var> l_val;
				l_val = std::make_shared<NorVar>(entry);
				MUST_BE(TokenType::token_ass);
				std::shared_ptr<Var> r_val;
				parse_exp(r_val);
				ir_list->push_back(std::make_shared<IrAssign>(IrOp::op_add, r_val, l_val));
				MUST_BE(TokenType::token_semi);
				ir_list->push_back(std::make_shared<IrLable>(label_begin));
				std::shared_ptr<Var> con_var;
				parse_con(con_var);
				ir_list->push_back(std::make_shared<IrBranch>(false, con_var, label_end));
				MUST_BE(TokenType::token_semi);
				auto id1 = parse_id();
				MUST_BE(TokenType::token_ass);
				auto id2 = parse_id();
				IrOp neg;
				if (IS_TOKEN(TokenType::token_plus))
				{
					neg = IrOp::op_add;
				}
				else if (IS_TOKEN(TokenType::token_minus))
				{
					neg = IrOp::op_sub;
				}
				else
				{
					wrong_in_parser("no +/- in for");
				}
				get_next();
				int step = parse_step();
				must_and_error(TokenType::token_rpar, ErrorType::should_be_rpar, false);
				parse_state();
				auto entry1 = cur_symbol_table->find(id1);
				auto entry2 = cur_symbol_table->find(id2);
				auto var1 = std::make_shared<NorVar>(entry1);
				auto var2 = std::make_shared<NorVar>(entry2);
				auto step_var = std::make_shared<ConstVar>(step);
				ir_list->push_back(std::make_shared<IrAssign>(neg, var2, step_var, var1));
				ir_list->push_back(std::make_shared<IrJump>(label_begin));
				ir_list->push_back(std::make_shared<IrLable>(label_end));
				break;
			}
			default:
				wrong_in_parser("no while for do");
		}
		output(SyntaxType::syntax_loop_state);
	}

	int Parser::parse_step()
	{
		int re = parse_uint();
		output(SyntaxType::syntax_step);
		return re;
	}

	void Parser::parse_func_call(bool can_be_void)
	{
		auto cur_id = parse_id();
		auto entryp = cur_symbol_table->find(cur_id);
		auto cur_ir = std::make_shared<IrCall>(entryp->id);
		if (!entryp)
		{
			error_handler(Error(ErrorType::undefined_id, buffer.get_last()->get_line()));
		}
		else if (!can_be_void && entryp->data == SymbolData::data_void)
		{
			wrong_in_parser("can't be void");
		}
#ifdef SEMANTICERROR
			else if (entryp->data == SymbolData::data_int)
			{
				ErrorData::is_char_exp.top() = false;
			}
			ErrorData::cur_func_args = func_args[cur_id];
#endif
		MUST_BE(TokenType::token_lpar);
		parse_par_value_list(cur_ir);
		ir_list->push_back(cur_ir);
		must_and_error(TokenType::token_rpar, ErrorType::should_be_rpar, false);
		if (entryp->data == SymbolData::data_void)
		{
			output_func(true);
		}
		else
		{
			output_func(false);
		}
	}

	void Parser::parse_par_value_list(std::shared_ptr<IrCall> ir)
	{
		int cnt = 0;
		switch (buffer.front()->get_type())
		{
			case TokenType::token_plus :
			case TokenType::token_minus :
			case TokenType::token_id :
			case TokenType::token_lpar :
			case TokenType::token_number :
			case TokenType::token_cchar :
			{
				std::shared_ptr<Var> curt;
#ifdef SEMANTICERROR
				auto exp_type =
#endif
				parse_exp(curt);
				ir->vars.push_back(curt);
#ifdef SEMANTICERROR
				if (cnt < ErrorData::cur_func_args.size() && exp_type != ErrorData::cur_func_args[cnt])
				{
					error_handler(Error(ErrorType::func_type_bad, buffer.get_last()->get_line()));
				}
#endif
				cnt++;
				while (IS_TOKEN(TokenType::token_comma))
				{
					get_next();
#ifdef SEMANTICERROR
					exp_type =
#endif
					parse_exp(curt);
					ir->vars.push_back(curt);
#ifdef SEMANTICERROR
					if (cnt < ErrorData::cur_func_args.size() && exp_type != ErrorData::cur_func_args[cnt])
					{
						error_handler(Error(ErrorType::func_type_bad, buffer.get_last()->get_line()));
					}
#endif
					cnt++;
				}
				break;
			}
			default:
				break;
		}
		if (cnt != ErrorData::cur_func_args.size())
		{
			error_handler(Error(ErrorType::func_arg_bad, buffer.get_last()->get_line()));
		}
		output(SyntaxType::syntax_par_value_list);
	}

	void Parser::parse_state_list() // must have a '}'
	{
		while (!IS_TOKEN(TokenType::token_rbrace))
		{
			parse_state();
		}
		output(SyntaxType::syntax_state_list);
	}

	void Parser::parse_read_state()
	{
		MUST_BE(TokenType::token_scanf);
		MUST_BE(TokenType::token_lpar);
		MUST_HAVE(TokenType::token_id);
		std::string id = parse_id();
		auto entry = cur_symbol_table->find(id);
		auto var = std::make_shared<NorVar>(entry);
		ir_list->push_back(std::make_shared<IrRead>(var));
		while (IS_TOKEN(TokenType::token_comma))
		{
			get_next();
			id = parse_id();
			entry = cur_symbol_table->find(id);
			var = std::make_shared<NorVar>(entry);
			ir_list->push_back(std::make_shared<IrRead>(var));
		}
		must_and_error(TokenType::token_rpar, ErrorType::should_be_rpar, false);
		output(SyntaxType::syntax_read_state);
	}

	void Parser::parse_write_state()
	{
		if (buffer.front()->get_type() != TokenType::token_printf)
		{
			wrong_in_parser("no printf");
		}
		get_next();
		if (buffer.front()->get_type() != TokenType::token_lpar)
		{
			wrong_in_parser("no (");
		}
		get_next();
		if (buffer.front()->get_type() == TokenType::token_cstring)
		{
			std::string str = parse_string();
			for (auto i = str.cbegin(); i != str.cend(); i++)
			{
				auto v = std::make_shared<ConstVar>(*i);
				ir_list->push_back(std::make_shared<IrWrite>(v, true));
			}
			if (buffer.front()->get_type() == TokenType::token_comma)
			{
				get_next();
				std::shared_ptr<Var> var;
				auto sd = parse_exp(var);
				bool is_char = false;
				/*
				if (var->var_type == VarType::var_normal)
				{
					auto v = std::static_pointer_cast<NorVar>(var);
					if (v->table_entry->data == SymbolData::data_char)
					{
						is_char = true;
					}
				}
				// 有 bug, 数组，函数调用
				 */
				if (sd == SymbolData::data_char)
				{
					is_char = true;
				}
				ir_list->push_back(std::make_shared<IrWrite>(var, is_char));
			}
		}
		else
		{
			std::shared_ptr<Var> var;
			auto sd = parse_exp(var);
			bool is_char = false;
			/*
			if (var->var_type == VarType::var_normal)
			{
				auto v = std::static_pointer_cast<NorVar>(var);
				if (v->table_entry->data == SymbolData::data_char)
				{
					is_char = true;
				}
			}
			*/
			if (sd == SymbolData::data_char)
			{
				is_char = true;
			}
			ir_list->push_back(std::make_shared<IrWrite>(var, is_char));
		}
		auto v = std::make_shared<ConstVar>(10);
		ir_list->push_back(std::make_shared<IrWrite>(v, true));
		must_and_error(TokenType::token_rpar, ErrorType::should_be_rpar, false);
		output(SyntaxType::syntax_write_state);
	}

	void Parser::parse_return_state()
	{
		if (buffer.front()->get_type() != TokenType::token_return)
		{
			wrong_in_parser("no return");
		}
		get_next();
		if (buffer.front()->get_type() == TokenType::token_lpar)
		{
			get_next();
			std::shared_ptr<Var> var;
#ifdef SEMANTICERROR
			auto exp_type =
#endif
			parse_exp(var);
			ir_list->push_back(std::make_shared<IrRet>(var));
#ifdef SEMANTICERROR
			if (exp_type == SymbolData::data_char)
			{
				ErrorData::has_char_return = true;
			}
			else
			{
				ErrorData::has_int_return = true;
			}
			if (ErrorData::cur_func_data_type == SymbolData::data_void)
			{
				error_handler(Error(ErrorType::void_bad_return, buffer.get_last()->get_line()));
			}
			else if (ErrorData::cur_func_data_type != exp_type)
			{
				error_handler(Error(ErrorType::no_bad_return, buffer.get_last()->get_line()));
			}
#endif
			must_and_error(TokenType::token_rpar, ErrorType::should_be_rpar, false);
		}
		else
		{
			ir_list->push_back(std::make_shared<IrRet>());
#ifdef SEMANTICERROR
			ErrorData::has_void_return = true;
			if (ErrorData::cur_func_data_type != SymbolData::data_void)
			{
				error_handler(Error(ErrorType::void_bad_return, buffer.get_last()->get_line()));
			}
#endif
		}
		output(SyntaxType::syntax_return_state);
	}

	void Parser::parse()
	{
		ir_list = std::make_shared<IrList>();
		parse_program();
	}

	std::shared_ptr<IrList> Parser::get_ir_list() const
	{
		return ir_list;
	}

	const std::shared_ptr<SymbolTable> &Parser::get_cur_symbol_table() const
	{
		return cur_symbol_table;
	}

	void DefaultGrammerOutputer::syntax_unit_output(ucc::SyntaxType type)
	{
	}

	void DefaultGrammerOutputer::syntax_func_fucking_output(bool is_void)
	{
	}

	void DefaultGrammerOutputer::token_output(std::shared_ptr<Token> token)
	{
	}
}
#undef SEMANTICERROR