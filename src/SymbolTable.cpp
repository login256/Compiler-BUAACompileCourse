//
// Created by lyt on 2019/10/12.
//

#include <iostream>
#include "include/SymbolTable.h"
#include "include/homework/error_homework.h"

namespace ucc
{
	SymbolTableEntry::SymbolTableEntry(std::string id, SymbolType type, SymbolData data, int size, const std::set<SymbolAttribute> &attributes,
									   SymbolScope scope) : id(std::move(id)), type(type), data(data),
															size(size),
															attributes(attributes), scope(scope) {}

	SymbolTableEntry::SymbolTableEntry(std::string id, SymbolType type, SymbolData data, int size, SymbolScope scope) : id(std::move(id)),
																														type(type),
																														data(data),
																														size(size), scope(scope) {}


	SymbolTable::SymbolTable() {}

	SymbolTable::SymbolTable(const std::shared_ptr<SymbolTable> &par) : par(par) {}

	bool SymbolTable::is_root()
	{
		return par == nullptr;
	}

	std::shared_ptr<SymbolTableEntry> SymbolTable::find(const std::string &id)
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
			table[entry.id] = std::make_shared<SymbolTableEntry>(entry);
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
			table[id] = std::make_shared<SymbolTableEntry>(std::move(entry));
		}
		else
		{
			return false;
			// wrong;
			//std::cerr << "Add SymbolTable repeated!" << std::endl;
		}
		return true;
	}

	const std::shared_ptr<SymbolTable> &SymbolTable::get_par() const
	{
		return par;
	}

	const SymbolTable::SymbolMap &SymbolTable::get_table() const
	{
		return table;
	}


}