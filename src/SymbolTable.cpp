//
// Created by lyt on 2019/10/12.
//

#include <iostream>
#include "../include/SymbolTable.h"

namespace ucc
{
	SymbolTableEntry::SymbolTableEntry(const std::string &id, SymbolType type, SymbolData data, int size,
	                                   const std::vector<SymbolAttribute> &attributes) : id(id), type(type), data(data),
	                                                                                     size(size),
	                                                                                     attributes(attributes)
	{}

	SymbolTableEntry::SymbolTableEntry(const std::string &id, SymbolType type, SymbolData data, int size) : id(id),
	                                                                                                        type(type),
	                                                                                                        data(data),
	                                                                                                        size(size)
	{}


	SymbolTable::SymbolTable()
	{}

	SymbolTable::SymbolTable(SymbolTable *par) : par(par)
	{}

	bool SymbolTable::is_root()
	{
		return par == nullptr;
	}

	SymbolTableEntry *SymbolTable::find(std::string id)
	{
		auto cur = table.find(id);
		if (cur != table.end())
		{
			return cur->second;
		}
		else
		{
			return is_root() ? nullptr : par->find(id);
		}
	}

	void SymbolTable::add(const SymbolTableEntry &entry)
	{
		if (table.find(entry.id) == table.end())
		{
			table[entry.id] = new SymbolTableEntry(entry);
		}
		else
		{
			// wrong;
			std::cerr << "Add SymbolTable repeated!" << std::endl;
		}
	}

}