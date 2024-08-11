#pragma once
#include "../common/Common.hpp"
#include "../Compiler/Structure.hpp"
#include "../util/Tree.h"
namespace BinaryStructureLanguage {
	class TokenAnaylizer {
		/*
			用于分析token的类
		*/
public:
	bool Error = false;
	bool* ParseFailed = nullptr;
	bool original_parse_failed = false;
	std::vector<BinaryStructureLanguage::token> tokens;
	std::vector<BinaryStructureLanguage::token>* original_tokens;

	TokenAnaylizer(const std::vector<token>& tokens, std::vector<token>& original_tokens);
	TokenAnaylizer(const std::vector<token>& tokens, std::vector<token>& original_tokens, bool& parseFailed);
	//move
	inline TokenAnaylizer(TokenAnaylizer&& other) noexcept {
		tokens = std::move(other.tokens);
		original_tokens = std::move(other.original_tokens);
		ast = std::move(other.ast);
		ParseFailed = other.ParseFailed;
		original_parse_failed = other.original_parse_failed;
		other.ParseFailed = nullptr;
		other.original_parse_failed = false;
	}
	inline TokenAnaylizer& operator=(TokenAnaylizer&& other) noexcept {
		tokens = std::move(other.tokens);
		original_tokens = std::move(other.original_tokens);
		ast = std::move(other.ast);
		ParseFailed = other.ParseFailed;
		original_parse_failed = other.original_parse_failed;
		other.ParseFailed = nullptr;
		other.original_parse_failed = false;
		return *this;
	}
	//copy
	inline TokenAnaylizer(const TokenAnaylizer& other) {
		tokens = other.tokens;
		original_tokens = other.original_tokens;
		ast = other.ast;
		ParseFailed = other.ParseFailed;
		original_parse_failed = other.original_parse_failed;
	}
	inline TokenAnaylizer& operator=(const TokenAnaylizer& other) {
		tokens = other.tokens;
		original_tokens = other.original_tokens;
		ast = other.ast;
		ParseFailed = other.ParseFailed;
		original_parse_failed = other.original_parse_failed;
		return *this;
	}
	~TokenAnaylizer();

	// [start, end)
	bool match_arguments(size_t start, size_t end); //匹配参数列表(xxx,xxx,xxx)
	bool match_sentences(size_t start, size_t end); //匹配语句列表(xxx;xxx;xxx)
	bool match_dict_declaration(size_t start, size_t end); //匹配字典声明
	bool match_list_declaration(size_t start, size_t end); //匹配列表声明
	bool match_variable_declaration_and_struct_declaration(size_t start, size_t end); //匹配变量声明和结构体声明
	bool match_type(size_t start, size_t end); //匹配类型
	/*
	variable_declaration:
		name:type

	list和dict都属于type
	list:
		type[]

	dict:
		{
			variable_declaration1,variable_declaration2, variable_declaration3,variable_declaration4
		}

	struct_declaration:
		name:=dict

	*/

	inline Tree<token>& get_ast() { return ast; } //获取ast
	inline bool is_failed() {
		if (ParseFailed) {
			return *ParseFailed;
		}
		return original_parse_failed;
	}

	TokenAnaylizer subset(size_t start, size_t end); //返回一个子集
	bool parse(); //解析
private:
	Tree<token> ast;
	void throw_error(size_t err_token_start_idx, size_t err_token_end_idx, const lstring& error);
	};
}