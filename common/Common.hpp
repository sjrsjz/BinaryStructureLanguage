#pragma once

#define ENABLE_DEBUG_OUTPUT

#ifndef _COMMON_HEAD_
#define _COMMON_HEAD_
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <cstring>
#include <optional>
#include <vector>
#include <sstream>
#include <fstream>
#include <codecvt>
#include <unordered_set>
#include <tuple>

#ifdef _MSC_VER
#define LOCALE(x) imbue(std::locale(x));
#define LOCALE_WCOUT std::wcout.LOCALE("zh-CN")
#define LOCALE_FIN fin.LOCALE("zh-CN.utf8")
#else
#define LOCALE(x) ;
#define LOCALE_WCOUT ;
#define LOCALE_FIN ;
#endif

#define G_X64_ _WIN64
#define G_UNICODE_ 1

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif // _WIN32 || _WIN64


#if G_UNICODE_
typedef std::wstring lstring;
typedef std::wstringstream lstringstream;
typedef wchar_t lchar;
#define R(x) L##x
#define to_lstring(x) std::to_wstring(x)
#define std_lcout std::wcout

#else
typedef std::string lstring;
typedef char lchar;
typedef std::stringstream lstringstream;
#define R(x) x
#define to_lstring(x) std::to_string(x)
#define std_lcout std::cout
#endif //

#if G_X64_
#define st_size_t(x) std::stoull(x)
#define st_intptr_t(x) std::stoll(x)
#else
#define st_size_t(x) std::stoull(x)
#define st_intptr_t(x) std::stoll(x)
#endif // X64
#endif // !_COMMON_HEAD_

#include "../util/ByteArray.hpp"


#if defined _MSC_VER
#define _stdcall __stdcall
#define _cdecl __cdecl

#elif defined __GNUC__
#define _stdcall __attribute__((__stdcall__))
#define _cdecl __attribute__((__cdecl__))

#endif // _MSC_VER

#define INDEX(x,idx) x[size_t(idx % x.size())]

#define RESET   R("\033[0m")
#define BLACK   R("\033[30m")      /* Black */
#define RED     R("\033[31m")      /* Red */
#define GREEN   R("\033[32m")      /* Green */
#define YELLOW  R("\033[33m")      /* Yellow */
#define BLUE    R("\033[34m")      /* Blue */
#define MAGENTA R("\033[35m")      /* Magenta */
#define CYAN    R("\033[36m")      /* Cyan */
#define WHITE   R("\033[37m")      /* White */
#define BOLDBLACK   R("\033[1m\033[30m")      /* Bold Black */
#define BOLDRED     R("\033[1m\033[31m")      /* Bold Red */
#define BOLDGREEN   R("\033[1m\033[32m")      /* Bold Green */
#define BOLDYELLOW  R("\033[1m\033[33m")      /* Bold Yellow */
#define BOLDBLUE    R("\033[1m\033[34m")      /* Bold Blue */
#define BOLDMAGENTA R("\033[1m\033[35m")      /* Bold Magenta */
#define BOLDCYAN    R("\033[1m\033[36m")      /* Bold Cyan */
#define BOLDWHITE   R("\033[1m\033[37m")      /* Bold White */


extern lstring workPath;
extern lstring BinaryStructureLanguageError;

namespace BinaryStructureLanguage {
    size_t max_(size_t A, size_t B);
    size_t min_(size_t A, size_t B);
    const lstring DIVISION = R("$");
    void SubTokens(std::vector<lstring>& tks, std::vector<lstring>& output, intptr_t start, intptr_t end);
    bool iftk(std::vector<lstring>& tks, lstring tk, intptr_t i);
    lstring process_quotation_mark(lstring tk);
    bool IsOperator(lstring t, int type);
    lstring RemoveSpaceLR(lstring str);
    lstring subreplace(lstring resource_str, lstring sub_str, lstring new_str);
    bool isNum(lstring str);
    bool isNum_(const lstring& s);
    //void output(std::vector<lstring> tk, lstring str, intptr_t pos);
    lstring readFileString(const lstring& path);
    lstring getDictionary(lstring path);
    template <typename T>
    inline ByteArray<T> readFileByteArray(const lstring& path)
    {
        std::ifstream fin{};
        fin.open(path.c_str(), std::ios::in | std::ios::binary);
        if (!fin.is_open()) return ByteArray<T>{};
        fin.seekg(0, std::ios::end);
        size_t size = fin.tellg();
        fin.seekg(0, std::ios::beg);
        ByteArray<T> buf(size / sizeof(T));
        fin.read(reinterpret_cast<char*>(buf.ptr), size);
        fin.close();
        return buf;
    }

    bool writeFileString(const lstring& path, lstring str);
    bool inline writeFileByteArray(lstring path, ByteArray<> data) {
        std::ofstream fout{};
        fout.open(path.c_str(), std::ios::out | std::ios::binary);
        if (!fout.is_open()) return false;
        fout.write(reinterpret_cast<char*>(data.ptr), data.size * sizeof(data.ptr[0]));
        fout.close();
        return true;
    }

    std::vector<lstring> split(lstring str, lstring str_0);

    //convert string to wstring
    inline std::wstring to_wide_string(const std::string& input)
    {

        std::mbstate_t state = std::mbstate_t();
        std::wstring ret(input.size(), 0);
        const char* data = input.data();
        wchar_t* ptr = &ret[0];
        std::locale loc;
        size_t res = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc).in(state, data, data + input.size(), data, ptr, ptr + ret.size(), ptr);
        ret.resize(ptr - &ret[0]);
        return ret;

    }
    inline std::wstring to_wide_string(const std::wstring& input) {
        return input;
    }
    //convert wstring to string
    inline std::string to_byte_string(const std::wstring& input)
    {
        std::mbstate_t state = std::mbstate_t();
        std::string ret(input.size(), 0);
        const wchar_t* data = input.data();
        char* ptr = &ret[0];
        std::locale loc;
        size_t res = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc).out(state, data, data + input.size(), data, ptr, ptr + ret.size(), ptr);
        ret.resize(ptr - &ret[0]);
        return ret;
    }
    inline std::string to_byte_string(const std::string& input) {
        return input;
    }

    // 默认情况：is_vector为false
    template<typename T, typename... Types>
    struct is_vector : std::false_type {};

    // 特化：为所有vector类型，is_vector为true
    template<typename T, typename... Types>
    struct is_vector<const std::vector<T, Types...>&> : std::true_type {};

    template<typename T, typename... Types>
    lstringstream DebugOutputString(const T& data, const Types&... args) {
        lstringstream ss;
        //ss << "Type:" << typeid(data).name();
        if constexpr (std::is_same<decltype(data), const ByteArray<unsigned char>&>::value) {
            ss << "size:" << data.size << "{";
            for (size_t i = 0; i < data.size; i++) {
                ss << (i ? R(", ") : R("")) << to_lstring((unsigned char)data.ptr[i]);
            }
            ss << "}";
        }
        else if constexpr (is_vector<decltype(data)>::value) {
            ss << "size:" << data.size() << "{";
            for (size_t i = 0; i < data.size(); i++) {
                ss << (i ? R(", ") : R("")) << DebugOutputString(data[i]).str();
            }
            ss << "}";
        }
        else {
            if constexpr (std::is_same<decltype(data), const std::string&>::value) {
#if G_UNICODE_
                ss << R("\"") << to_wide_string(data) << R("\"");
#else
                ss << R("\"") << data << R("\"");
#endif // G_UNICODE_
            }
            else if constexpr (std::is_same<decltype(data), const std::wstring&>::value) {
#ifdef G_UNICODE_
                ss << R("\"") << data << R("\"");
#else
                ss << R("\"") << to_byte_string(data) << R("\"");
#endif // G_UNICODE_
            }
            else {
                ss << data;
            }
        }
        if constexpr (sizeof...(args)) {
            ss << R(" | ") << DebugOutputString(args...).str();
        }
        return ss;
    }

    template<typename T, typename... Types>
    void DebugOutput(const T& data, const Types&... args) {
#if _DEBUG && defined(ENABLE_DEBUG_OUTPUT)
        lstring tmp = DebugOutputString(data, args...).str();
#if (defined(_WIN32) || defined(_WIN64)) //&& defined(_MSC_VER)
#if G_UNICODE_
        OutputDebugStringW(tmp.c_str());
        OutputDebugStringW(R("\n"));
#else
        OutputDebugStringA(tmp.c_str());
        OutputDebugStringA(R("\n"));
#endif // G_UNICODE_
#else
        std::cout << tmp << std::endl;
#endif
#endif
    }
}
