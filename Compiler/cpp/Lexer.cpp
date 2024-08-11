#include "../Lexer.hpp"

#include <regex>


std::vector<BinaryStructureLanguage::token> BinaryStructureLanguage::Lexer::tokenize(const lstring& str) {
	std::vector<BinaryStructureLanguage::token> tokens;
	size_t currpos = 0;
	auto skip_space = [&]() {
		while (currpos < str.length() && (str[currpos] == ' ' || str[currpos] == '\t' || str[currpos] == '\n' || str[currpos] == '\r')) currpos++;
	};
	auto next_line = [&]() {
		//跳到下一行，并确保能处理\r\n、\n、\r
		while (currpos < str.length() && str[currpos] != '\n' && str[currpos] != '\r') currpos++;
	};
	auto test_string = [&](lstring str_, size_t pos) {
		//测试字符串是否为str
		if (currpos + str_.length() > str.length()) return false;
		for (size_t i = 0; i < str_.length(); i++) {
			if (str[currpos + i] != str_[i]) return false;
		}
		return true;
	};
	auto test_number = [](const lstring& str, size_t pos) -> size_t {
		// Regular expression to match valid numbers including integers, floats, and scientific notation
		
		// Attempt to match a number starting exactly at the given position
		#if G_UNICODE_
		// unicode
		std::wregex number_pattern(LR"(^\d*\.?\d+([eE][-+]?\d+)?)", std::regex_constants::ECMAScript | std::regex_constants::icase);
		std::wsmatch match;
		if (std::regex_search(str.begin() + pos, str.end(), match, number_pattern)) {
			// If a match was found, return its length
			return match[0].length();
		}
		#else
		std::regex number_pattern(R"(^\d*\.?\d+([eE][-+]?\d+)?)", std::regex_constants::ECMAScript | std::regex_constants::icase);
		std::smatch match;
		if (std::regex_search(str.begin() + pos, str.end(), match, number_pattern)) {
			// If a match was found, return its length
			return match[0].length();
		}
		#endif // G_UNICODE_
		else {
			// If no match was found, return 0
			return 0;
		}
	};
	auto read_number = [&](lstring& current_token) {
		//读取数字
		size_t len = test_number(str, currpos);
		if (len) {
			current_token = str.substr(currpos, len);
			currpos += len;
			return true;
		}
		return false;
	};
	auto read_string = [&](lstring& current_token) {
		// Read a string
		if (test_string(R("R\""), currpos)) {
			lstring divider;
			currpos += 2;
			while (currpos < str.length() && str[currpos] != '(') {
				divider += str[currpos++];
			}
			if (currpos < str.length()) {
				currpos++; // skip '('
				lstring end_divider = R(")") + divider + R("\"");
				while (currpos < str.length() && !test_string(end_divider, currpos)) {
					if (str[currpos] == '\\') { // Check for escape sequence
						if (++currpos < str.length()) { // Skip the backslash and move to the next character
							switch (str[currpos]) {
							case 'n': current_token += '\n'; currpos++; break;
							case 't': current_token += '\t'; currpos++; break;
							case '\"': current_token += '\"'; currpos++; break;
							case '\\': current_token += '\\'; currpos++; break;
							default:    current_token += '\\'; current_token += str[currpos++]; break; // Unrecognized escape sequence, keep the character
							}
						}
						else {
							// Handle error: unexpected end of string
							return false;
						}
					}
					else {
						current_token += str[currpos++];
					}
				}
				if (currpos < str.length()) {
					currpos += end_divider.length(); // Skip the closing delimiter
					return true;
				}
			}
		}
		else if (str[currpos] == '\"') {
			currpos++;
			while (currpos < str.length()) {
				if (str[currpos] == '\\') { // Check for escape sequence
					if (++currpos < str.length()) { // Skip the backslash and move to the next character
						switch (str[currpos]) {
						case 'n': current_token += '\n'; currpos++; break;
						case 't': current_token += '\t'; currpos++; break;
						case '\"': current_token += '\"'; currpos++; break;
						case '\\': current_token += '\\'; currpos++; break;
						default:   current_token += '\\'; current_token += str[currpos++]; break; // Unrecognized escape sequence, keep the character
						}
					}
					else {
						// Handle error: unexpected end of string
						return false;
					}
				}
				else if (str[currpos] == '\"') {
					currpos++;
					return true;
				}
				else {
					current_token += str[currpos++];
				}
			}
		}
		return false;
	};
	auto read_char = [&](lstring& current_token) {
		//读取字符
		if (str[currpos] == '\'') {
			currpos++;
			while (currpos < str.length() && str[currpos] != '\'') {
				current_token += str[currpos];
				currpos++;
			}
			if (currpos < str.length()) {
				currpos++;
				return true;
			}
		}
		return false;
	};
	auto read_token = [&](lstring& current_token) {
		//读取标识符
		while (currpos<str.length())
		{
			if (str[currpos] == ' ' || str[currpos] == '\t' || str[currpos] == '\n' || str[currpos] == '\r') break;
			if (str[currpos] == '\'' || str[currpos] == '\"') break;
			if (currpos < str.length() - 1) {
				lstring two_char = { str[currpos],str[currpos + 1]};
				if (BinaryStructureLanguage::IsOperator(two_char,0)) break;
			}
			lstring one_char = { str[currpos] };
			if (BinaryStructureLanguage::IsOperator(one_char,0)) break;
			current_token += str[currpos];
			currpos++;
		}
	};
	auto read_operator = [&](lstring& current_token) {
		//读取操作符
		if (currpos < str.length() - 1) {
			lstring two_char = { str[currpos],str[currpos + 1] };
			if (BinaryStructureLanguage::IsOperator(two_char, 0)) {
				current_token = two_char;
				currpos += 2;
				return true;
			}
		}
		if (currpos < str.length()) {
			lstring one_char = { str[currpos] };
			if (BinaryStructureLanguage::IsOperator(one_char, 0)) {
				current_token = one_char;
				currpos++;
				return true;
			}
		}
		return false;
	};
	auto read_comment = [&](lstring& current_token, size_t& comment_length) {
		//读取注释
		if (test_string(R("//"), currpos)) {
			currpos += 2;
			while (currpos < str.length() && str[currpos] != '\n' && str[currpos] != '\r') {
				current_token += str[currpos];
				currpos++;
			}
			comment_length = current_token.length() + 2;
			return true;
		}
		if (test_string(R("/*"), currpos)) {
			currpos+= 2;
			while (currpos < str.length() && !test_string(R("*/"), currpos)) {
				current_token += str[currpos];
				currpos++;
			}
			if (currpos < str.length()) {
				currpos += 2;
			}
			comment_length = current_token.length() + 4;
			return true;
		}
		return false;
	};
	//下面的变量指示是否为字符串、注释
	bool is_char = false;
	bool is_string = false;
	bool is_line_comment = false;
	bool is_block_comment = false;

	lstring current_token;
	size_t token_position = 0;
	while (true)
	{
		skip_space();
		if (currpos >= str.length()) break;
		
		lstring token;
		size_t position = currpos;
		size_t comment_length = 0;
		if (read_comment(token, comment_length)) {
			tokens.push_back({ token, BinaryStructureLanguage::TokenType::TokenType_COMMENT, position ,token_position++, false, comment_length });
		}
		else if (read_number(token)) {
			tokens.push_back({ token, BinaryStructureLanguage::TokenType::TokenType_NUMBER, position, token_position++, false, token.length() });
		}
		else if (read_string(token)) {
			tokens.push_back({ token, BinaryStructureLanguage::TokenType::TokenType_STRING, position, token_position++, false, token.length() + 2 });
		}
		else if (read_char(token)) {
			tokens.push_back({ token, BinaryStructureLanguage::TokenType::TokenType_STRING, position, token_position++, false, token.length() });
		}
		else if (read_operator(token)) {
			tokens.push_back({ token, BinaryStructureLanguage::TokenType::TokenType_SYMBOL, position, token_position++, false, token.length() });
		}
		else {
			read_token(token);
			tokens.push_back({ token, BinaryStructureLanguage::TokenType::TokenType_IDENTIFIER, position, token_position++, false, token.length() });
		}

	}
	return tokens;
}

std::vector<BinaryStructureLanguage::token> BinaryStructureLanguage::Lexer::reject_comments(const std::vector<BinaryStructureLanguage::token>& tokens) {
	std::vector<BinaryStructureLanguage::token> result;
	for (const auto& token : tokens) {
		if (token.type != BinaryStructureLanguage::TokenType::TokenType_COMMENT) {
			result.push_back(token);
		}
	}
	return result;
}