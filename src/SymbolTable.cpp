//
// Created by lyt on 2019/10/12.
//

#include <iostream>
#include "../include/SymbolTable.h"
#include "../include/homework/error_homework.h"

namespace ucc
{
	SymbolTableEntry::SymbolTableEntry(std::string id, SymbolType type, SymbolData data, int size,
									   const std::vector<SymbolAttribute> &attributes) : id(std::move(id)), type(type), data(data),
																						 size(size),
																						 attributes(attributes) {}

	SymbolTableEntry::SymbolTableEntry(std::string id, SymbolType type, SymbolData data, int size) : id(std::move(id)),
																									 type(type),
																									 data(data),
																									 size(size) {}


	SymbolTable::SymbolTable() {}

	SymbolTable::SymbolTable(const std::shared_ptr<SymbolTable> &par) : par(par) {}

	bool SymbolTable::is_root()
	{
		return par == nullptr;
	}

	SymbolTableEntry *SymbolTable::find(const std::string &id)
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

	bool SymbolTable::add(const SymbolTableEntry &entry)
	{
		if (table.find(entry.id) == table.end())
		{
			table[entry.id] = new SymbolTableEntry(entry);
		}
		else
		{
			return false;
			// wrong;
			//std::cerr << "Add SymbolTable repeated!" << std::endl;
		}
		return true;
	}

	bool SymbolTable::add(SymbolTableEntry &&entry)
	{
		if (table.find(entry.id) == table.end())
		{
			std::string id = entry.id;
			table[id] = new SymbolTableEntry(std::move(entry));
		}
		else
		{
			return false;
			// wrong;
			//std::cerr << "Add SymbolTable repeated!" << std::endl;
		}
		return true;
	}

	SymbolTable::~SymbolTable()
	{
		for (auto &e : table)
		{
			delete e.second;
		}
	}

	const std::shared_ptr<SymbolTable> &SymbolTable::get_par() const
	{
		return par;
	}


}