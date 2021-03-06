//
// Created by lyt on 2019/10/12.
//

#ifndef UNIVERSALCOMPILER_SYMBOLTABLE_H
#define UNIVERSALCOMPILER_SYMBOLTABLE_H

#include <string>
#include <vector>
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
	};

	struct SymbolTableEntry
	{
		std::string id;
		SymbolType type;
		SymbolData data;
		int size;
		std::vector<SymbolAttribute> attributes;

		SymbolTableEntry(std::string id, SymbolType type, SymbolData data, int size);

		SymbolTableEntry(std::string id, SymbolType type, SymbolData data, int size,
						 const std::vector<SymbolAttribute> &attributes);
	};


	class SymbolTable
	{
		typedef std::unordered_map<std::string, SymbolTableEntry *> SymbolMap;
	private:
		SymbolMap table;
		std::shared_ptr<SymbolTable> par;
	public:
		SymbolTable();

		SymbolTable(const std::shared_ptr<SymbolTable> &par);

		virtual ~SymbolTable();

		bool is_root();

		SymbolTableEntry *find(const std::string &id);

		void add(const SymbolTableEntry &entry);

		void add(SymbolTableEntry &&entry);

		const std::shared_ptr<SymbolTable> &get_par() const;
	};
}

#endif //UNIVERSALCOMPILER_SYMBOLTABLE_H
