// N lang.cpp: 定义应用程序的入口点。
//

#include "BinaryStructureLanguage.h"
#include "Compiler/Lexer.hpp"
#include "Compiler/AST.hpp"
using namespace std;

int main()
{
	locale::global(locale("chs"));

	BinaryStructureLanguage::DebugOutput("Hello CMake Debug.");
	BinaryStructureLanguage::Lexer lexer;
	lstring code = R(R"A(
/*
BinaryStructureLanguage测试代码
*/
student:={
	name:string = "unknown", //姓名
	age:int = 18, //年龄
	class:{ //班级
		name:string,
		teacher:string
	}[] = [
		{
			"unknown", //班级名
			"unknown" //老师名
		}
	]
},
DataBase:student[] //数据库
	= [
		{
			"Tom",
			15,
			{
				"Class1",
				"Emma"
			}
		},
		{
			"Jerry",
			16,
			{
				"Class2",
				"Jack"
			}
		}

	]
)A");
	auto tokens = lexer.tokenize(code);
	//for (auto& token : tokens) {
	//	wcout << "Token: " << token.type << " Value: " << token.token_str << " Position: " << token.position << endl;
	//}

	BinaryStructureLanguage::TokenAnaylizer anaylizer(lexer.reject_comments(tokens), tokens);

	if (anaylizer.parse()) {
		std_lcout << "Parse Success!" << endl;

		std_lcout << "AST:" << endl;
		anaylizer.get_ast().print(
			[](BinaryStructureLanguage::token& token, size_t depth) -> void {
				for (size_t i = 0; i < depth; i++) {
					std_lcout << "  ";
				}
				std_lcout << token.token_str << ":" << BinaryStructureLanguage::TokenTypeToString(token.type) << endl;
			}
		);
	}
	else {
		std_lcout << "Parse Failed!" << endl;
	}


	if (anaylizer.is_failed()) {
		std::vector <bool> err_text{};
		err_text.resize(code.length());
		std_lcout << RED << "TokenAnaylizer Error: " << RESET << endl;
		for (const auto& token : tokens) {
			if (token.error) {
				for (size_t i = 0; i < token.orginal_str_length; i++) {
					err_text[token.position + i] = true;
				}
			}
		}

		for (size_t i = 0; i < code.length(); i++) {
			if (err_text[i]) {
				std_lcout << RED << code[i] << RESET;
			}
			else {
				std_lcout << code[i];
			}
		}
		std_lcout << RESET << endl;
	}

	return 0;
}
