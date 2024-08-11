#pragma once
#include "../common/Common.hpp"
#include "Structure.hpp"
namespace BinaryStructureLanguage {
	class Lexer {
		public:
		std::vector<token> tokenize(const lstring& str);
		std::vector<token> reject_comments(const std::vector<token>& tokens);
	};
}