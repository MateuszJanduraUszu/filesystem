// entry_point.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <cstdlib>
#include <filesystem.hpp>
#include <iostream>
#include <xstring>
#include <xtr1common>
#ifdef _M_X64
#ifdef _DEBUG
#pragma comment(lib, R"(x64\Debug\filesystem.lib)")
#else // ^^^ _DEBUG ^^^ / vvv NDEBUG vvv
#pragma comment(lib, R"(x64\Release\filesystem.lib)")
#endif // _DEBUG
#else // ^^^ _M_X64 ^^^ / vvv _M_IX86 vvv
#ifdef _DEBUG
#pragma comment(lib, R"(Debug\filesystem.lib)")
#else // ^^^ _DEBUG ^^^ / vvv NDEBUG vvv
#pragma comment(lib, R"(Release\filesystem.lib)")
#endif // _DEBUG
#endif // _M_X64

// TYPES AND CONSTANTS USED IN TESTS
using _FILESYSTEM code_page;
using _FILESYSTEM _Is_char_t;
using _STD is_same_v;

// CLASS TEMPLATE _Convert_to
template <class _In, class _Out>
class _Convert_to;

// CLASS TEMPLATE _Convert_to WITH _In AND char TYPES
template <class _In>
class _Convert_to<_In, char> { // convert any character type to char
private:
    static_assert(_Is_char_t<_In>, "invalid input character type");
    static_assert(!is_same_v<_In, char>, "cannot do conversion between the same types");

public:
    using _Elem   = _In;
    using _Traits = char_traits<_In>;

    _NODISCARD string operator()(const _Elem* const _Input) {
        return _FILESYSTEM _Convert_to_narrow<_Elem, _Traits>(_Input);
    }
};

// CLASS TEMPLATE _Convert_to WITH _In AND char8_t TYPES
template <class _In>
class _Convert_to<_In, char8_t> { // convert any character type to char8_t
private:
    static_assert(_Is_char_t<_In>, "invalid input character type");
    static_assert(!is_same_v<_In, char8_t>, "cannot do conversion between the same types");

public:
    using _Elem   = _In;
    using _Traits = char_traits<_In>;
    
    _NODISCARD u8string operator()(const _Elem* const _Input) {
        if constexpr (is_same_v<_Elem, char>) {
            const size_t _Size = _Traits::length(_Input);
            u8string _Output;

            // sizeof char and char8_t is the same, so they can store the same charcters
            for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
                _Output.push_back(static_cast<char8_t>(_Input[_Idx]));
            }

            return _Output;
        } else { // convert to string and then to u8string
            const string& _Narrow = _FILESYSTEM _Convert_to_narrow<_Elem, _Traits>(_Input);
            u8string _Output;

            for (const auto& _Ch : _Narrow) {
                _Output.push_back(static_cast<char8_t>(_Ch));
            }

            return _Output;
        }
    }
};

// CLASS TEMPLATE _Convert_to WITH _In AND char16_t TYPES
template <class _In>
class _Convert_to<_In, char16_t> { // convert any character type to char16_t
private:
    static_assert(_Is_char_t<_In>, "invalid input character type");
    static_assert(!is_same_v<_In, char16_t>, "cannot do conversion between the same types");

public:
    using _Elem   = _In;
    using _Traits = char_traits<_In>;

    _NODISCARD u16string operator()(const _Elem* const _Input) {
        if constexpr (sizeof(_Elem) <= sizeof(char16_t)) { // use simple casting
            const size_t _Size = _Traits::length(_Input);
            u16string _Output;

            // push every character that sizeof it's is less or equal to char16_t
            for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
                _Output.push_back(static_cast<char16_t>(_Input[_Idx]));
            }
            
            return _Output;
        } else { // use codecvt etc.
            const string& _Narrow{_FILESYSTEM _Convert_to_narrow<_Elem, _Traits>(_Input)};
            return _FILESYSTEM _Convert_narrow_to_utf<char16_t, char_traits<char16_t>, allocator<char16_t>>(_Narrow.c_str());
        }
    }
};

// CLASS TEMPLATE _Convert_to WITH _In AND char32_t TYPES
template <class _In>
class _Convert_to<_In, char32_t> { // convert any character type to char32_t
private:
    static_assert(_Is_char_t<_In>, "invalid input character type");
    static_assert(!is_same_v<_In, char32_t>, "cannot do conversion between the same types");

public:
    using _Elem   = _In;
    using _Traits = char_traits<_In>;

    _NODISCARD u32string operator()(const _Elem* const _Input) {
        const size_t _Size = _Traits::length(_Input);
        u32string _Output;

        // char32_t can store any other character type
        for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
            _Output.push_back(static_cast<char32_t>(_Input[_Idx]));
        }

        return _Output;
    }
};

// CLASS TEMPLATE _Convert_to WITH _In AND wchar_t TYPES
template <class _In>
class _Convert_to<_In, wchar_t> { // convert any character to wchar_t
private:
    static_assert(_Is_char_t<_In>, "invalid input character type");
    static_assert(!is_same_v<_In, char32_t>, "cannot do conversion between the same types");

public:
    using _Elem   = _In;
    using _Traits = char_traits<_In>;

    _NODISCARD wstring operator()(const _Elem* const _Input) {
        if constexpr (sizeof(_Elem) <= sizeof(wchar_t)) { // use simple casting
            const size_t _Size = _Traits::length(_Input);
            wstring _Output;

            for (size_t _Idx = 0; _Idx < _Size; ++_Idx) {
                _Output.push_back(static_cast<wchar_t>(_Input[_Idx]));
            }

            return _Output;
        } else { // use codecvt etc.
            const string& _Narrow{_FILESYSTEM _Convert_to_narrow<_Elem, _Traits>(_Input)};
            return _FILESYSTEM _Convert_narrow_to_wide(code_page::utf8, _Narrow.c_str());
        }
    }
};

// To avoid warning C4007: main() must be __cdecl
#ifndef _CDECL_OR_STDCALL
#ifdef _M_IX86
#define _CDECL_OR_STDCALL __cdecl
#else // ^^^ _M_IX86 ^^^ / vvv _M_X64 vvv
#define _CDECL_OR_STDCALL __stdcall
#endif // _M_IX86
#endif // _CDECL_OR_STDCALL

int _CDECL_OR_STDCALL main(int _Count, char** _Params) {
    string _Out; // output character
    string _Input; // converted text

    _STD cout << "Output character: ";
    _STD cin >> _Out;
    _STD cout << "Converted text: ";
    _STD cin >> _Input;

    if (_Out == "char") { // unsupported operation
        _CSTD system("cls");
        _STD cout << "Cannot do conversion between the same types.";
        _STD cin.get();
        return EXIT_FAILURE;
    }

    if (_Out == "char8_t") { // convert char to char8_t
        const u8string& _Result{_Convert_to<char, char8_t>{}(_Input.c_str())};
        return EXIT_SUCCESS;
    } else if (_Out == "char16_t") { // convert char to char16_t
        const u16string& _Result{_Convert_to<char, char16_t>{}(_Input.c_str())};
        return EXIT_SUCCESS;
    } else if (_Out == "char32_t") { // convert char to char32_t
        const u32string& _Result{_Convert_to<char, char32_t>{}(_Input.c_str())};
        return EXIT_SUCCESS;
    } else if (_Out == "wchar_t") { // convert char to wchar_t (available to see)
        const wstring& _Result{_Convert_to<char, wchar_t>{}(_Input.c_str())};
        _STD wcout << _Result << "\n";
        _STD cin.get();
        return EXIT_SUCCESS;
    } else { // invalid character type
        _CSTD system("cls");
        _STD cout << "Invalid character type.\n";
        _STD cin.get();
        return EXIT_FAILURE;
    }
}

#undef _CDECL_OR_STDCALL