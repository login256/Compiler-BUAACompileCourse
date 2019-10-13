//
// Created by lyt on 2019/10/12.
//
#include <memory>
#include <iostream>
#include "../include/Parser.h"
#include "../include/Token.h"

namespace ucc
{
	TokenBuffer::TokenBuffer(Lexer &lexer) : lexer(lexer)
	{}

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
		que.pop_front();
		return re;
	}

	void TokenBuffer::back_up(const std::shared_ptr<Token> &token)
	{
		que.push_front(token);
	}

	Parser::Parser(GrammerOutputer &outputer, Lexer &lexer) : outputer(outputer), buffer(lexer)
	{}

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
		while (1)
		{}
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

	void Parser::parse_add()
	{
		if (IS_TOKEN(TokenType::token_plus))
		{
			get_next();
		}
		else if (IS_TOKEN(TokenType::token_minus))
		{
			get_next();
		}
		else
		{
			wrong_in_parser("no + -");
		}
		output(SyntaxType::syntax_add);
	}

	void Parser::parse_mul()
	{
		if (IS_TOKEN(TokenType::token_mul))
		{
			get_next();
		}
		else if (IS_TOKEN(TokenType::token_div))
		{
			get_next();
		}
		else
		{
			wrong_in_parser("no * /");
		}
		output(SyntaxType::syntax_mul);
	}

	void Parser::parse_rel()
	{
		switch (buffer.front()->get_type())
		{
			case TokenType::token_les:
			case TokenType::token_leq:
			case TokenType::token_grt:
			case TokenType::token_geq:
			case TokenType::token_neq:
			case TokenType::token_eql:
			{
				get_next();
				break;
			}
			default: wrong_in_parser("no rel");
		}
		output(syntax_rel);
	}

	void Parser::parse_char()
	{
		MUST_BE(TokenType::token_cchar);
		output(SyntaxType::syntax_char);
	}

	void Parser::parse_string()
	{
		MUST_BE(TokenType::token_cstring);
		output(SyntaxType::syntax_string);
	}

	void Parser::parse_program()
	{
		cur_symbol_table = std::make_shared<SymbolTable>();
		if (buffer.front()->get_type() == TokenType::token_const)
		{
			parse_const_declare();
		}
		if (buffer.at(2)->get_type() == TokenType::token_comma ||
		    buffer.at(2)->get_type() == TokenType::token_lbrackets ||
		    buffer.at(2)->get_type() == TokenType::token_semi)
		{
			parse_var_declare();
		}
		while (buffer.at(1)->get_type() == TokenType::token_id)
		{
			if (buffer.front()->get_type() == TokenType::token_void)
			{
				parse_void_func();
			}
			else if (IS_TOKEN(TokenType::token_int) || IS_TOKEN(TokenType::token_void))
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

	void Parser::parse_const_declare()
	{
		MUST_BE(TokenType::token_const);
		parse_const_define();
		MUST_BE(TokenType::token_semi);
		while (IS_TOKEN(TokenType::token_const))
		{
			get_next();
			parse_const_define();
			MUST_BE(TokenType::token_semi);
		}
		output(SyntaxType::syntax_const_declare);
	}

	void Parser::parse_const_define()
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
				size = 1;
				break;
			}
			default: wrong_in_parser("no int/char int const define");
		}
		cur_symbol_table->add(SymbolTableEntry(parse_id(), SymbolType::varible, data_type, size,
		                                       {SymbolAttribute::att_const}));
		MUST_BE(TokenType::token_ass);
		if (data_type == data_int)
		{
			parse_int();
		}
		else
		{
			parse_char();
		}
		while (IS_TOKEN(TokenType::token_comma))
		{
			get_next();
			cur_symbol_table->add(SymbolTableEntry(parse_id(), SymbolType::varible, data_type, size,
			                                       {SymbolAttribute::att_const}));
			MUST_BE(TokenType::token_ass);
			if (data_type == data_int)
			{
				parse_int();
			}
			else
			{
				parse_char();
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

	void Parser::parse_declare_header()
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
		cur_symbol_table->add(SymbolTableEntry(parse_id(), SymbolType::func, data, 0));
		output(SyntaxType::syntax_declare_header);
	}

	void Parser::parse_var_declare()
	{
		parse_var_define();
		MUST_BE(TokenType::token_semi);
		while ((IS_TOKEN(TokenType::token_int) || IS_TOKEN(TokenType::token_char)) &&
		       buffer.at(2)->get_type() != TokenType::token_lpar)
		{
			parse_var_define();
			MUST_BE(TokenType::token_semi);
		}
		output(SyntaxType::syntax_var_declare);
	}

	void Parser::parse_var_define()
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
			size = 1;
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
			parse_uint();
			MUST_BE(TokenType::token_rbrackets);
		}
		cur_symbol_table->add(SymbolTableEntry(cur_id, type, data, tsize));
		while (IS_TOKEN(TokenType::token_comma))
		{
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
				parse_uint();
				MUST_BE(TokenType::token_rbrackets);
			}
			cur_symbol_table->add(SymbolTableEntry(cur_id, type, data, tsize));
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
		parse_declare_header();
		MUST_BE(TokenType::token_lpar);
		parse_par_list();
		MUST_BE(TokenType::token_rpar);
		MUST_BE(TokenType::token_lbrace);
		parse_compound();
		MUST_BE(TokenType::token_rbrace);
		output(SyntaxType::syntax_func);
	}

	void Parser::parse_void_func()
	{
		MUST_BE(TokenType::token_void);
		std::string cur_id = parse_id();
		cur_symbol_table->add(SymbolTableEntry(cur_id, SymbolType::func, SymbolData::data_void, 0));
		MUST_BE(TokenType::token_lpar);
		parse_par_list();
		MUST_BE(TokenType::token_rpar);
		MUST_BE(TokenType::token_lbrace);
		parse_compound();
		MUST_BE(TokenType::token_rbrace);
		output(SyntaxType::syntax_void_func);
	}

	void Parser::parse_compound()
	{
		cur_symbol_table = std::make_shared<SymbolTable>(cur_symbol_table);
		if (IS_TOKEN(TokenType::token_const))
		{
			parse_const_declare();
		}
		if (IS_TOKEN(TokenType::token_int) || IS_TOKEN(TokenType::token_char))
		{
			parse_var_declare();
		}
		parse_state_list();
		cur_symbol_table = cur_symbol_table->get_par();
		output(SyntaxType::syntax_compound);
	}

	void Parser::parse_par_list()
	{
		if (IS_TOKEN(TokenType::token_int) || IS_TOKEN(TokenType::token_char))
		{
			auto data = parse_var_type();
			auto cur_id = parse_id();
			while (IS_TOKEN(TokenType::token_comma))
			{
				get_next();
				if (!(IS_TOKEN(TokenType::token_int) || IS_TOKEN(TokenType::token_char)))
				{
					wrong_in_parser("no int/char int par list");
				}
				data = parse_var_type();
				cur_id = parse_id();
			}
		}
		output(SyntaxType::syntax_par_list);
	}

	void Parser::parse_main_func()
	{
		MUST_BE(TokenType::token_void);
		MUST_BE(TokenType::token_main);
		MUST_BE(TokenType::token_lpar);
		MUST_BE(TokenType::token_rpar);
		MUST_BE(TokenType::token_lbrace);
		parse_compound();
		MUST_BE(TokenType::token_rbrace);
		output(SyntaxType::syntax_main_func);
	}

	void Parser::parse_exp()
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
		parse_term();
		while (IS_TOKEN(TokenType::token_plus) || IS_TOKEN(TokenType::token_minus))
		{
			parse_add();
			parse_term();
		}
		output(SyntaxType::syntax_exp);
	}

	void Parser::parse_term()
	{
		parse_fact();
		while (IS_TOKEN(TokenType::token_mul) || IS_TOKEN(TokenType::token_div))
		{
			parse_mul();
			parse_fact();
		}

		output(SyntaxType::syntax_term);
	}

	void Parser::parse_fact()
	{
		switch (buffer.front()->get_type())
		{
			case TokenType::token_id :
			{
				if (buffer.at(1)->get_type() == TokenType::token_lpar)
				{
					parse_func_call(false);
				}
				else
				{
					parse_id();
					if (IS_TOKEN(TokenType::token_lbrackets))
					{
						get_next();
						parse_exp();
						MUST_BE(TokenType::token_rbrackets);
					}
				}
				break;
			}
			case TokenType::token_lpar :
			{
				get_next();
				parse_exp();
				MUST_BE(TokenType::token_rpar);
				break;
			}
			case TokenType::token_plus :
			case TokenType::token_minus :
			case TokenType::token_number:
			{
				parse_int();
				break;
			}
			case TokenType::token_cchar :
			{
				parse_char();
				break;
			}
			default: wrong_in_parser("wrong fact");
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
					parse_func_call();
					MUST_BE(TokenType::token_semi);
				}
				else //if (1 is =)
				{
					parse_ass_state();
					MUST_BE(TokenType::token_semi);
				}
				break;
			}
			case TokenType::token_scanf :
			{
				parse_read_state();
				MUST_BE(TokenType::token_semi);
				break;
			}
			case TokenType::token_printf :
			{
				parse_write_state();
				MUST_BE(TokenType::token_semi);
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
				MUST_BE(TokenType::token_semi);
				break;
			}
		}
		output(SyntaxType::syntax_state);
	}

	void Parser::parse_ass_state()
	{
		parse_id();
		if (IS_TOKEN(TokenType::token_lbrackets))
		{
			get_next();
			parse_exp();
			MUST_BE(TokenType::token_rbrackets);

		}
		MUST_BE(TokenType::token_ass);
		parse_exp();
		output(SyntaxType::syntax_ass_state);
	}

	void Parser::parse_con_state()
	{
		MUST_BE(TokenType::token_if);
		MUST_BE(TokenType::token_lpar);
		parse_con();
		MUST_BE(TokenType::token_rpar);
		parse_state();
		if (IS_TOKEN(TokenType::token_else))
		{
			get_next();
			parse_state();
		}
		output(SyntaxType::syntax_con_state);
	}

	void Parser::parse_con()
	{
		parse_exp();
		switch (buffer.front()->get_type())
		{
			case TokenType::token_les:
			case TokenType::token_leq:
			case TokenType::token_grt:
			case TokenType::token_geq:
			case TokenType::token_neq:
			case TokenType::token_eql:
			{
				parse_rel();
				parse_exp();
				break;
			}
			default: break;
		}
		output(SyntaxType::syntax_con);
	}

	void Parser::parse_loop_state()
	{
		switch (buffer.front()->get_type())
		{
			case TokenType::token_while :
			{
				get_next();
				MUST_BE(TokenType::token_lpar);
				parse_con();
				MUST_BE(TokenType::token_rpar);
				parse_state();
				break;
			}
			case TokenType::token_do :
			{
				get_next();
				parse_state();
				MUST_BE(TokenType::token_while);
				MUST_BE(TokenType::token_lpar);
				parse_con();
				MUST_BE(TokenType::token_rpar);
				break;
			}
			case TokenType::token_for :
			{
				MUST_BE(TokenType::token_lpar);
				parse_id();
				MUST_BE(TokenType::token_ass);
				parse_exp();
				MUST_BE(TokenType::token_semi);
				parse_con();
				MUST_BE(TokenType::token_semi);
				parse_id();
				MUST_BE(TokenType::token_ass);
				parse_id();
				int neg = 1;
				if (IS_TOKEN(TokenType::token_plus))
				{
					neg = 1;
				}
				else if (IS_TOKEN(TokenType::token_minus))
				{
					neg = -1;
				}
				else
				{
					wrong_in_parser("no +/- in for");
				}
				get_next();
				parse_step();
				MUST_BE(TokenType::token_rpar);
				parse_state();
				break;
			}
			default: wrong_in_parser("no while for do");
		}
		output(SyntaxType::syntax_loop_state);
	}

	void Parser::parse_step()
	{
		parse_uint();
		output(SyntaxType::syntax_step);
	}

	void Parser::parse_func_call(bool can_be_void)
	{
		auto cur_id = parse_id();
		auto entryp = cur_symbol_table->find(cur_id);
		if (entryp == nullptr)
		{
			wrong_in_parser("can't find func");
		}
		if (!can_be_void && entryp->data == SymbolData::data_void)
		{
			wrong_in_parser("can't be void");
		}
		MUST_BE(TokenType::token_lpar);
		parse_par_value_list();
		MUST_BE(TokenType::token_rpar);
		if (!can_be_void && entryp->data == SymbolData::data_void)
		{
			output_func(true);
		}
		else
		{
			output_func(false);
		}
	}

	void Parser::parse_par_value_list()
	{
		switch (buffer.front()->get_type())
		{
			case TokenType::token_plus :
			case TokenType::token_minus :
			case TokenType::token_id :
			case TokenType::token_lpar :
			case TokenType::token_number :
			case TokenType::token_cchar :
			{
				parse_exp();
				while (IS_TOKEN(TokenType::token_comma))
				{
					get_next();
					parse_exp();
				}
				break;
			}
			default: break;
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
		parse_id();
		while (IS_TOKEN(TokenType::token_id))
		{
			parse_id();
		}
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
			parse_string();
			if (buffer.front()->get_type() == TokenType::token_comma)
			{
				get_next();
				parse_exp();
			}
		}
		else
		{
			parse_exp();
		}
		if (buffer.front()->get_type() != TokenType::token_rpar)
		{
			wrong_in_parser("no )");
		}
		get_next();
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
			parse_exp();
			if (buffer.front()->get_type() != TokenType::token_rpar)
			{
				wrong_in_parser("no )");
			}
			buffer.pop();
		}
		output(SyntaxType::syntax_return_state);
	}

	void Parser::parse()
	{
		parse_program();
	}
}