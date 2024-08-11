#pragma once
#include "../common/Common.hpp"

namespace BinaryStructureLanguage {
	enum TokenType
	{
		// 保留字
		TokenType_RESERVED,
		// 标识符
		TokenType_IDENTIFIER,
		// 数字
		TokenType_NUMBER,
		// 字符串
		TokenType_STRING,
		// 符号
		TokenType_SYMBOL,
		// 注释
		TokenType_COMMENT,
		// 未知
		TokenType_UNKNOWN,

		ASTNodeType_EMPTY, // 空
		ASTNodeType_OPERATOR, // 操作符
		ASTNodeType_NUMBER, // 数字
		ASTNodeType_STRING, // 字符串
		ASTNodeType_MEMBER, // 成员
		ASTNodeType_IDENTIFIER, // 标识符
		ASTNodeType_VARIABLE, // 变量
		ASTNodeType_TUPLE, // 元组
		ASTNodeType_LIST, // 列表
		ASTNodeType_DICT, // 字典
		ASTNodeType_SENTENCES, // 语句
		ASTNodeType_DEFINTION, // 定义
	};
	inline lchar* TokenTypeToString(TokenType type) {
		const lchar* strs[] = {
			R("TokenType_RESERVED"),
			R("TokenType_IDENTIFIER"),
			R("TokenType_NUMBER"),
			R("TokenType_STRING"),
			R("TokenType_SYMBOL"),
			R("TokenType_COMMENT"),
			R("TokenType_UNKNOWN"),
			R("ASTNodeType_EMPTY"),
			R("ASTNodeType_OPERATOR"),
			R("ASTNodeType_NUMBER"),
			R("ASTNodeType_STRING"),
			R("ASTNodeType_MEMBER"),
			R("ASTNodeType_IDENTIFIER"),
			R("ASTNodeType_VARIABLE"),
			R("ASTNodeType_TUPLE"),
			R("ASTNodeType_LIST"),
			R("ASTNodeType_DICT"),
			R("ASTNodeType_SENTENCES"),
			R("ASTNodeType_DEFINTION"),
		};
		if (type >= 0 && type < sizeof(strs) / sizeof(strs[0])) {
			return (lchar*)strs[type];
		}
		return (lchar*)R("Unknown");
	}
	struct token
	{
		lstring token_str; // token字符串
		TokenType type; // token类型
		size_t position{}; // token在原始字符串中的位置
		size_t token_position{}; // token在token列表中的位置
		bool error{}; // 是否有错误
		size_t orginal_str_length{}; // token原始字符串长度
	};
}