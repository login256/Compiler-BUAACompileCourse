//
// Created by lyt on 2019/10/12.
//

#ifndef UNIVERSALCOMPILER_SYMBOLTABLE_H
#define UNIVERSALCOMPILER_SYMBOLTABLE_H

#include <string>
#include <vector>
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
	};

	struct SymbolTableEntry
	{
		std::string id;
		SymbolType type;
		SymbolData data;
		int size;
		std::vector<SymbolAttribute> attributes;

		SymbolTableEntry(const std::string &id, SymbolType type, SymbolData data, int size,
		                 const std::vector<SymbolAttribute> &attributes);

		SymbolTableEntry(const std::string &id, SymbolType type, SymbolData data, int size);
	};


	class SymbolTable
	{
		typedef std::unordered_map<std::string, SymbolTableEntry *> SymbolMap;
	private:
		SymbolMap table;
		SymbolTable *par;
	public:
		SymbolTable();

		SymbolTable(SymbolTable *par);

		virtual ~SymbolTable();

		bool is_root();

		SymbolTableEntry *find(std::string id);

		void add(const SymbolTableEntry &entry);
	};
}

#endif //UNIVERSALCOMPILER_SYMBOLTABLE_H
