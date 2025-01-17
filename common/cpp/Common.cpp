﻿#include "../Common.hpp"

namespace BinaryStructureLanguage {
    void output(lstring str) {
        printf((char*)(str + R("\n")).c_str());
    }
    size_t max_(size_t A, size_t B) {
        return A > B ? A : B;
    }
    size_t min_(size_t A, size_t B) {
        return A < B ? A : B;
    }
    lstring subreplace(lstring resource_str, lstring sub_str, lstring new_str)
    {
        lstring dst_str = resource_str;
        intptr_t pos = 0;
        while ((pos = dst_str.find(sub_str)) != lstring::npos)   //替换所有指定子串
        {
            dst_str.replace(pos, sub_str.length(), new_str);
        }
        return dst_str;
    }
    bool IsOperator(lstring t, int type) {
        
        bool l{};
        l = t == R("+") || t == R("-") || t == R("*") || t == R("/") || t == R("\\") || t == R("%") || t == R("&") || t == R("!") || t == R("^") || t == R("~") || t == R("=") || t == R("==") || t == R(">") || t == R("<") || t == R("<=") || t == R(">=") || t == R("!=") || t == R("?=") || t == R("|") ||
            t == R("&&") || t == R(",") || t == R(".") || t == R("\n") || t == R(":") || t == R("->") || t == R("<<") || t == R(">>") || t == R("/*") || t == R("*/") || t == R(";") || t == R(" ") || t == R(":=") || t == R("|>") || t == R("<|") || t == R("::");
        if (type == 0) {
            l = l || t == R("(") || t == R(")") || t == R("[") || t == R("]") || t == R("{") || t == R("}");
        }
        return l;
    }
    lstring RemoveSpaceLR(lstring str) {
        int i = 0, j = str.length() - 1;
        while (i < j && (str.substr(i, 1) == R(" ") || str.substr(i, 1) == R("\t"))) i++;
        while (j > i && (str.substr(j, 1) == R(" ") || str.substr(j, 1) == R("\t"))) j--;
        return str.substr(i, j - i + 1);
    }
    bool isNum(lstring str)
    {
        lstringstream sin(str);
        double d;
        lchar c;
        if (!(sin >> d)) return false;
        if (sin >> c) return false;
        return true;
    }

    bool isNum_(const lstring &s)
    {
        try{
            std::stold(s);
        }
        catch (std::invalid_argument e) {
            return false;
        }
        return true;
    }

    void SubTokens(std::vector<lstring>& tks, std::vector<lstring>& output, intptr_t start, intptr_t end) {
        // 从start到end-1
        // 用于截取tks的子集
        // 例如SubTokens(tks, output, 1, 3)将tks的第1个到第3个元素截取出来
        std::vector<lstring> t;
        if (end < start) {
            output.clear(); return;
        }
        if (end > tks.size()) {
            end = tks.size();
        }
        if (start < 1) start = 1;
        t = tks;
        t.erase(t.begin() + end, t.end());
        if (start > 1) t.erase(t.begin(), t.begin() + start - 1);
        output = t;
    }
    bool iftk(std::vector<lstring>& tks, lstring tk, intptr_t i) {
        return i >= 0 && i < tks.size() && tks[i] == tk;
    }
    lstring process_quotation_mark(lstring tk) {
        lstring t = tk;
        if (t.substr(0, 1) == R("\"")) t = t.substr(1, t.length() - 1);
        if (t.substr(t.length() - 1, 1) == R("\"")) t = t.substr(0, t.length() - 1);
        return t;
    }

    lstring readFileString(const lstring& path) {
#if  G_UNICODE_
        std::wifstream fin{};
        LOCALE_FIN
        std::wstring buf;
#else
        std::ifstream fin{};
        std::string buf;
#endif
        fin.open(path.c_str(), std::ios::in);
        if (!fin.is_open()) return R("");
#if G_UNICODE_
        // Check for BOM
        wchar_t bom[1];
        fin.read(bom, 1);
        if (bom[0] != 65279) {
            // If it's not a BOM, put the bytes back into the stream
            fin.seekg(0);
        }
#endif
        lstring buf2{};
        while (std::getline(fin, buf, R('\0')))
            buf2 += buf;
        fin.close();
        DebugOutput(buf2);
        return buf2;
    }

    std::vector<lstring> split(lstring str, lstring str_0) {
        std::vector<lstring> tmp{};
        size_t p{};
        size_t lp{};
        size_t off = str_0.length();
        while ((p = str.find(str_0, p)) != std::string::npos)
        {
            tmp.push_back(str.substr(lp, p - lp));
            p += off;
            lp = p;
        }
        tmp.push_back(str.substr(lp, str.size() - lp + 1));
        return tmp;
    }
    bool writeFileString(const lstring &path, lstring str)
    {
#if G_UNICODE_
        std::wofstream fout{};
#else
        std::ofstream fout{};
#endif // UNICODE
        fout.open(path.c_str(), std::ios::out);
        if (!fout.is_open()) return false;
        fout << str;
        fout.close();
        return true;
    }
    lstring getDictionary(lstring path) {
		size_t p = path.find_last_of(R("/\\"));
        	if (p == std::string::npos) return R(".");
        		return path.substr(0, p);
        return R(".");
    }
}
