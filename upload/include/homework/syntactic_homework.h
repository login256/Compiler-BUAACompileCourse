//
// Created by lyt on 2019/10/12.
//

#ifndef UNIVERSALCOMPILER_SYNTACTIC_HOMEWORK_H
#define UNIVERSALCOMPILER_SYNTACTIC_HOMEWORK_H


#include "../Parser.h"
#include <fstream>
#include <ostream>
#include <map>
#include <string>

class HomeworkGrammerOutputer : public ucc::GrammerOutputer
{
private:
	static std::map<ucc::SyntaxType, std::string> type_to_output;
	std::ostream &output_stream;
public:
	HomeworkGrammerOutputer(std::ostream &output_stream);

	virtual void syntax_unit_output(ucc::SyntaxType type) override;

	virtual void syntax_func_fucking_output(bool is_void) override;

	virtual void token_output(std::shared_ptr<ucc::Token> token) override;
};

namespace syntactic_homework
{
	int syntactic_main();
}

#endif //UNIVERSALCOMPILER_SYNTACTIC_HOMEWORK_H
