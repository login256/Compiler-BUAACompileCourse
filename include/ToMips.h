//
// Created by lyt on 2019/11/19.
//

#ifndef UNIVERSALCOMPILER_TOMIPS_H
#define UNIVERSALCOMPILER_TOMIPS_H

#include <fstream>
#include <memory>
#include "IR.h"

namespace ucc
{
	extern std::ofstream mips_output_stream;
	void to_mips(std::shared_ptr<IrList> ir_list, std::shared_ptr<SymbolTable> global_table_t);
}

#endif //UNIVERSALCOMPILER_TOMIPS_H
