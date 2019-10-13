//
// Created by lyt on 2019/10/12.
//

#ifndef UNIVERSALCOMPILER_SYNTACTIC_HOMEWORK_H
#define UNIVERSALCOMPILER_SYNTACTIC_HOMEWORK_H


#include "../Parser.h"
#include <fstream>
#include <map>
#include <string>

class HomeworkGrammerOutputer : public ucc::GrammerOutputer
{
private:
	static std::map<ucc::SyntaxType, std::string> type_to_output;
	std::ofstream &output_stream;
public:
	HomeworkGrammerOutputer(std::ofstream &output_stream);

	virtual void syntax_unit_output(ucc::SyntaxType type);

	void token_output(std::shared_ptr<ucc::Token> token) override;
};

#endif //UNIVERSALCOMPILER_SYNTACTIC_HOMEWORK_H
