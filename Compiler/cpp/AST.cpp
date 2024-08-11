#include "../AST.hpp"

namespace BinaryStructureLanguage {
	bool step_bracket_counter(const token& tk, intptr_t& brackets_depth_counter, intptr_t& square_brackets_depth_counter, intptr_t& curly_brackets_depth_counter) {
		if (tk.type != TokenType_SYMBOL) return false;
		if (tk.token_str == R("(")) ++brackets_depth_counter;
		else if (tk.token_str == R(")")) --brackets_depth_counter;
		else if (tk.token_str == R("[")) ++square_brackets_depth_counter;
		else if (tk.token_str == R("]")) --square_brackets_depth_counter;
		else if (tk.token_str == R("{")) ++curly_brackets_depth_counter;
		else if (tk.token_str == R("}")) --curly_brackets_depth_counter;
		return true;
	}
	void TokenAnaylizer::throw_error(size_t err_token_start_idx, size_t err_token_end_idx, const lstring& error) {
		std_lcout << RED << error << R(": ") << RESET;
		for (size_t i = err_token_start_idx; i < err_token_end_idx; ++i) {
			std_lcout << RED << (*original_tokens)[tokens[i].token_position].token_str << R(" ") << RESET;
		}
		std_lcout << std::endl;
		//标红错误的token并输出
		for(size_t i = err_token_start_idx; i < err_token_end_idx; ++i){
			(*original_tokens)[tokens[i].token_position].error = true;
		}
		Error = true;
		if(ParseFailed) *ParseFailed = true;
		else original_parse_failed = true;
	}
	TokenAnaylizer::~TokenAnaylizer() {
		
	}
	TokenAnaylizer::TokenAnaylizer(const std::vector<token>& tokens, std::vector<token>& original_tokens) {
		this->tokens = tokens;
		this->original_tokens = &original_tokens;
	}
	TokenAnaylizer::TokenAnaylizer(const std::vector<token>& tokens, std::vector<token>& original_tokens, bool& parseFailed) {
		this->tokens = tokens;
		this->original_tokens = &original_tokens;
		this->ParseFailed = &parseFailed;
	}

	bool TokenAnaylizer::match_sentences(size_t start, size_t end) {

		// 语句列表
		// 语句列表的形式为 xxx;xxx;xxx
		// 先得到分号分割的子序列

		std::vector<size_t> semicolon_positions;
		intptr_t brackets_depth_counter = 0, square_brackets_depth_counter = 0, curly_brackets_depth_counter = 0;
		for (size_t i = start; i < end; ++i) {
			if (tokens[i].type == TokenType_SYMBOL && tokens[i].token_str == R(";") && brackets_depth_counter == 0 && square_brackets_depth_counter == 0 && curly_brackets_depth_counter == 0) {
				semicolon_positions.push_back(i);
			}
			else {
				step_bracket_counter(tokens[i], brackets_depth_counter, square_brackets_depth_counter, curly_brackets_depth_counter);
			}
		}
		if (brackets_depth_counter != 0 || square_brackets_depth_counter != 0 || curly_brackets_depth_counter != 0) {
			throw_error(0, tokens.size(), R("括号不匹配"));
			return false;
		}
		if (semicolon_positions.size() == 0) {
			// 没有分号
			return false;
		}
		// 逐个分析子序列
		token ast_root;
		ast_root.type = ASTNodeType_SENTENCES;
		ast_root.token_str = R("SENTENCES");
		ast_root.position = tokens[start].position;
		ast_root.token_position = tokens[start].token_position;
		ast_root.orginal_str_length = tokens[start].orginal_str_length;
		ast_root.error = false;

		ast.push_back(ast_root);
		ast.ToChildrenEnd();
		size_t left = start;
		for (size_t i = 0; i < semicolon_positions.size(); ++i) {
			TokenAnaylizer sub = subset(left, semicolon_positions[i]);
			if (!sub.parse()) return false;
			ast.push_back(sub.ast);
			left = semicolon_positions[i] + 1;
		}
		TokenAnaylizer last = subset(left, end);
		if (!last.parse()) return false;
		ast.push_back(last.ast);
		ast.parent();
		return true;
	}
	bool TokenAnaylizer::match_arguments(size_t start, size_t end) {
		// 参数列表
		// 参数列表的形式为 xxx,xxx,xxx
		// 先得到逗号分割的子序列
		std::vector<size_t> comma_positions;
		intptr_t brackets_depth_counter = 0, square_brackets_depth_counter = 0, curly_brackets_depth_counter = 0;
		for (size_t i = start; i < end; ++i) {
			if (tokens[i].type == TokenType_SYMBOL && tokens[i].token_str == R(",") && brackets_depth_counter == 0 && square_brackets_depth_counter == 0 && curly_brackets_depth_counter == 0) {
				comma_positions.push_back(i);
			}
			else {
				step_bracket_counter(tokens[i], brackets_depth_counter, square_brackets_depth_counter, curly_brackets_depth_counter);
			}
		}
		if (brackets_depth_counter != 0 || square_brackets_depth_counter != 0 || curly_brackets_depth_counter != 0) {
			throw_error(0, tokens.size(), R("括号不匹配"));
			return false;
		}
		if (comma_positions.size() == 0) {
			// 没有逗号
			return false;
		}
		// 逐个分析子序列
		token ast_root;
		ast_root.type = ASTNodeType_TUPLE;
		ast_root.token_str = R("TUPLE");
		ast_root.position = tokens[start].position;
		ast_root.token_position = tokens[start].token_position;
		ast_root.orginal_str_length = tokens[start].orginal_str_length;
		ast_root.error = false;

		ast.push_back(ast_root);
		ast.ToChildrenEnd();
		size_t left = start;
		for (size_t i = 0; i < comma_positions.size(); ++i) {
			TokenAnaylizer sub = subset(left, comma_positions[i]);			
			if (!sub.parse()) return false;
			ast.push_back(sub.ast);
			left = comma_positions[i] + 1;
		}
		TokenAnaylizer last = subset(left, end);
		if (!last.parse()) return false;
		ast.push_back(last.ast);
		ast.parent();
		return true;
	}
	bool TokenAnaylizer::match_variable_declaration_and_struct_declaration(size_t start, size_t end) {
		// 变量声明和结构体声明
		// 变量声明的形式为 xxx:type
		// 结构体声明的形式为 xxx:=dict
		// 先得到冒号分割的子序列
		size_t operator_position{};
		bool got_operator = false;
		intptr_t brackets_depth_counter = 0, square_brackets_depth_counter = 0, curly_brackets_depth_counter = 0;
		for (size_t i = start; i < end; ++i) {
			if (!got_operator && tokens[i].type == TokenType_SYMBOL && (tokens[i].token_str == R(":") || tokens[i].token_str == R(":=")) && brackets_depth_counter == 0 && square_brackets_depth_counter == 0 && curly_brackets_depth_counter == 0) {
				operator_position = i;
				got_operator = true;
			}
			else {
				step_bracket_counter(tokens[i], brackets_depth_counter, square_brackets_depth_counter, curly_brackets_depth_counter);
			}
		}
		if (brackets_depth_counter != 0 || square_brackets_depth_counter != 0 || curly_brackets_depth_counter != 0) {
			throw_error(0, tokens.size(), R("括号不匹配"));
			return false;
		}
		if (!got_operator) {
			return false;
		}

		if (tokens[operator_position].token_str == R(":")) {
			// 向后检查是否有等于号
			bool got_equal = false;
			size_t equal_position = 0;
			for (size_t i = operator_position + 1; i < end; ++i) {
				if (tokens[i].type == TokenType_SYMBOL && tokens[i].token_str == R("=") && brackets_depth_counter == 0 && square_brackets_depth_counter == 0 && curly_brackets_depth_counter == 0) {
					got_equal = true;
					equal_position = i;
					break;
				}
				else {
					step_bracket_counter(tokens[i], brackets_depth_counter, square_brackets_depth_counter, curly_brackets_depth_counter);
				}
			}
			if (brackets_depth_counter != 0 || square_brackets_depth_counter != 0 || curly_brackets_depth_counter != 0) {
				throw_error(0, tokens.size(), R("括号不匹配"));
				return false;
			}
			token ast_root;
			ast_root.type = ASTNodeType_VARIABLE;
			ast_root.token_str = R("VARIABLE");
			ast_root.position = tokens[start].position;
			ast_root.token_position = tokens[start].token_position;
			ast_root.orginal_str_length = tokens[start].orginal_str_length;
			ast_root.error = false;

			ast.push_back(ast_root);
			ast.ToChildrenEnd();
			TokenAnaylizer sub = subset(start, operator_position);
			if (!sub.parse()) return false;
			ast.push_back(sub.ast);
			if (!got_equal) {
				// 变量声明
				TokenAnaylizer sub2 = subset(operator_position + 1, end);
				if (!sub2.parse()) return false;
				ast.push_back(sub2.ast);
			}
			else {
				// 变量声明
				TokenAnaylizer sub2 = subset(operator_position + 1, equal_position);
				if (!sub2.parse()) return false;
				ast.push_back(sub2.ast);
				TokenAnaylizer sub3 = subset(equal_position + 1, end);
				if (!sub3.parse()) return false;
				ast.push_back(sub3.ast);
			}
			ast.parent();
		}
		else if (tokens[operator_position].token_str == R(":=")) {
			// 结构体声明
			token ast_root;
			ast_root.type = ASTNodeType_DEFINTION;
			ast_root.token_str = R("DEFINTION");
			ast_root.position = tokens[start].position;
			ast_root.token_position = tokens[start].token_position;
			ast_root.orginal_str_length = tokens[start].orginal_str_length;
			ast_root.error = false;

			ast.push_back(ast_root);
			ast.ToChildrenEnd();
			TokenAnaylizer sub = subset(start, operator_position);
			if (!sub.parse()) return false;
			ast.push_back(sub.ast);
			TokenAnaylizer sub2 = subset(operator_position + 1, end);
			if (!sub2.parse()) return false;
			ast.push_back(sub2.ast);
			ast.parent();
		}
	}
	bool TokenAnaylizer::match_dict_declaration(size_t start, size_t end) {
		// 字典声明
		// 字典声明的形式为 {xxx,xxx,xxx}
		// 先得到逗号分割的子序列
		if (tokens[start].type != TokenType_SYMBOL || tokens[start].token_str != R("{")) return false;
		if (tokens[end - 1].type != TokenType_SYMBOL || tokens[end - 1].token_str != R("}")) return false;

		std::vector<size_t> comma_positions;
		intptr_t brackets_depth_counter = 0, square_brackets_depth_counter = 0, curly_brackets_depth_counter = 0;
		for (size_t i = start + 1; i < end - 1; ++i) {
			if (tokens[i].type == TokenType_SYMBOL && tokens[i].token_str == R(",") && brackets_depth_counter == 0 && square_brackets_depth_counter == 0 && curly_brackets_depth_counter == 0) {
				comma_positions.push_back(i);
			}
			else {
				step_bracket_counter(tokens[i], brackets_depth_counter, square_brackets_depth_counter, curly_brackets_depth_counter);
			}
		}
		if (brackets_depth_counter != 0 || square_brackets_depth_counter != 0 || curly_brackets_depth_counter != 0) {
			throw_error(0, tokens.size(), R("括号不匹配"));
			return false;
		}

		// 逐个分析子序列
		token ast_root;
		ast_root.type = ASTNodeType_DICT;
		ast_root.token_str = R("DICT");
		ast_root.position = tokens[start].position;
		ast_root.token_position = tokens[start].token_position;
		ast_root.orginal_str_length = tokens[start].orginal_str_length;
		ast_root.error = false;

		ast.push_back(ast_root);
		ast.ToChildrenEnd();
		size_t left = start + 1;
		for (size_t i = 0; i < comma_positions.size(); ++i) {
			TokenAnaylizer sub = subset(left, comma_positions[i]);
			if (!sub.parse()) return false;
			ast.push_back(sub.ast);
			left = comma_positions[i] + 1;
		}
		TokenAnaylizer last = subset(left, end - 1);
		if (!last.parse()) return false;
		ast.push_back(last.ast);
		ast.parent();
		return true;		
	}
	bool TokenAnaylizer::match_list_declaration(size_t start, size_t end) {
		// 列表声明
		// 列表声明的形式为 xxx[]
		// 先得到方括号分割的子序列
		if (tokens[end - 1].type != TokenType_SYMBOL || tokens[end - 1].token_str != R("]")) return false;
		intptr_t brackets_depth_counter = 0, square_brackets_depth_counter = 0, curly_brackets_depth_counter = 0;
		// 反向查找[
		size_t left = 0;
		TokenAnaylizer* sub{};
		for (size_t i = end - 2; i >= start; --i) {
			if (tokens[i].type == TokenType_SYMBOL && tokens[i].token_str == R("[") && brackets_depth_counter == 0 && square_brackets_depth_counter == 0 && curly_brackets_depth_counter == 0) {
				// 找到了
				std::vector<token> subtokens;
				for (size_t j = i + 1; j < end - 1; ++j) {
					subtokens.push_back(tokens[j]);
				}
				sub = new TokenAnaylizer(subtokens, *original_tokens, ParseFailed ? *ParseFailed : original_parse_failed);
				if (!sub->parse()) return false;
				left = i;
				break;
			}
			else {
				step_bracket_counter(tokens[i], brackets_depth_counter, square_brackets_depth_counter, curly_brackets_depth_counter);
			}
		}
		
		// 逐个分析子序列
		token ast_root;
		ast_root.type = ASTNodeType_LIST;
		ast_root.token_str = R("LIST");
		ast_root.position = tokens[start].position;
		ast_root.token_position = tokens[start].token_position;
		ast_root.orginal_str_length = tokens[start].orginal_str_length;
		ast_root.error = false;

		ast.push_back(ast_root);
		ast.ToChildrenEnd();
		TokenAnaylizer sub0 = subset(start, left);
		if (!sub0.parse()) return false;
		ast.push_back(sub0.ast);
		if (sub) {
			ast.push_back(sub->ast);
			delete sub;
		}
		ast.parent();
		return true;
	}

	bool TokenAnaylizer::parse() {
		if (tokens.size() == 0) {
			// 空
			token ast_root;
			ast_root.type = ASTNodeType_EMPTY;
			ast_root.token_str = R("EMPTY");
			ast_root.position = 0;
			ast_root.token_position = 0;
			ast_root.orginal_str_length = 0;
			ast_root.error = false;

			ast.push_back(ast_root);
			return true;
		}
		if (is_failed())
			return false;
		if (!is_failed() && match_sentences(0, tokens.size())) return true;
		if (!is_failed() && match_arguments(0, tokens.size())) return true;
		if (!is_failed() && match_variable_declaration_and_struct_declaration(0, tokens.size())) return true;
		if (!is_failed() && match_dict_declaration(0, tokens.size())) return true;
		if (!is_failed() && match_list_declaration(0, tokens.size())) return true;
		if (!is_failed() && tokens.size() == 1) {
			// 单个token
			token ast_root;
			ast_root.type = tokens[0].type;
			ast_root.token_str = tokens[0].token_str;
			ast_root.position = tokens[0].position;
			ast_root.token_position = tokens[0].token_position;
			ast_root.orginal_str_length = tokens[0].orginal_str_length;
			ast_root.error = false;

			ast.push_back(ast_root);
			return true;		
		}
		if (is_failed())
			return false;
		throw_error(0, tokens.size(), R("无法解析"));
		return false;
	}
	TokenAnaylizer TokenAnaylizer::subset(size_t left, size_t right) {
		// 截获子序列 [left, right)
		std::vector<token> subtokens;
		for (size_t i = left; i < right; ++i) {
			subtokens.push_back(tokens[i]);
		}
		return TokenAnaylizer(subtokens, *original_tokens,ParseFailed ? *ParseFailed : original_parse_failed);
	}
}