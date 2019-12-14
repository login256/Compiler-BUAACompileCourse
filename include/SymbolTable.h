//
// Created by lyt on 2019/10/12.
//

#ifndef UNIVERSALCOMPILER_SYMBOLTABLE_H
#define UNIVERSALCOMPILER_SYMBOLTABLE_H

#include <string>
#include <vector>
#include <set>
#include <memory>
#include <unordered_map>

namespace ucc
{
	enum SymbolType
	{
		varible,
		array,
		func,
	};

	enum SymbolData
	{
		data_int,
		data_char,
		data_void,
	};

	enum SymbolAttribute
	{
		att_const,
		att_register,
	};

	enum SymbolScope
	{
		scope_global,
		scope_local,
	};

	struct SymbolTableEntry
	{
		std::string id;
		SymbolType type;
		SymbolData data;
		SymbolScope scope;
		int size;
		std::set<SymbolAttribute> attributes;

		int init_value;

		unsigned int address = 0;

		SymbolTableEntry(std::string id, SymbolType type, SymbolData data, int size, SymbolScope scope);

		SymbolTableEntry(std::string id, SymbolType type, SymbolData data, int size, const std::set<SymbolAttribute> &attributes,
						 SymbolScope scope);
	};


	class SymbolTable
	{
		typedef std::unordered_map<std::string, std::shared_ptr<SymbolTableEntry>> SymbolMap;
	private:
		SymbolMap table;
		std::shared_ptr<SymbolTable> par;
	public:
		SymbolTable();

		SymbolTable(const std::shared_ptr<SymbolTable> &par);

		bool is_root();

		std::shared_ptr<SymbolTableEntry> find(const std::string &id);

		bool add(const SymbolTableEntry &entry);

		bool add(SymbolTableEntry &&entry);

		const std::shared_ptr<SymbolTable> &get_par() const;

		const SymbolMap &get_table() const;
	};
}

#endif //UNIVERSALCOMPILER_SYMBOLTABLE_H
