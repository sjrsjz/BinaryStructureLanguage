#pragma once
#include "../common/Common.hpp"

namespace BinaryStructureLanguage {
	enum TokenType
	{
		// ������
		TokenType_RESERVED,
		// ��ʶ��
		TokenType_IDENTIFIER,
		// ����
		TokenType_NUMBER,
		// �ַ���
		TokenType_STRING,
		// ����
		TokenType_SYMBOL,
		// ע��
		TokenType_COMMENT,
		// δ֪
		TokenType_UNKNOWN,

		ASTNodeType_EMPTY, // ��
		ASTNodeType_OPERATOR, // ������
		ASTNodeType_NUMBER, // ����
		ASTNodeType_STRING, // �ַ���
		ASTNodeType_MEMBER, // ��Ա
		ASTNodeType_IDENTIFIER, // ��ʶ��
		ASTNodeType_VARIABLE, // ����
		ASTNodeType_TUPLE, // Ԫ��
		ASTNodeType_LIST, // �б�
		ASTNodeType_DICT, // �ֵ�
		ASTNodeType_SENTENCES, // ���
		ASTNodeType_DEFINTION, // ����
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
		lstring token_str; // token�ַ���
		TokenType type; // token����
		size_t position{}; // token��ԭʼ�ַ����е�λ��
		size_t token_position{}; // token��token�б��е�λ��
		bool error{}; // �Ƿ��д���
		size_t orginal_str_length{}; // tokenԭʼ�ַ�������
	};
}