// path.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <pch.h>
#include <filesystem.hpp>

#if !_HAS_WINDOWS
#pragma message("The contents of <path.cpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv

_FILESYSTEM_BEGIN
// FUNCTION TEMPLATE operator>>
template <class _Elem, class _Traits = char_traits<_Elem>>
_NODISCARD basic_istream<_Elem, _Traits>& __cdecl operator>>(basic_istream<_Elem, _Traits>& _Stream, path& _Path) {
    basic_string<_Elem, _Traits, allocator<_Elem>> _Input;
    _Stream >> _Input;
    _Path = _STD move(_Input);
    return _Stream;
}

template _FILESYSTEM_API _NODISCARD istream& __cdecl operator>>(istream&, path&);
template _FILESYSTEM_API _NODISCARD wistream& __cdecl operator>>(wistream&, path&);

// FUNCTION TEMPLATE operator<<
template <class _Elem, class _Traits = char_traits<_Elem>>
_NODISCARD basic_ostream<_Elem, _Traits>& __cdecl operator<<(basic_ostream<_Elem, _Traits>& _Stream, const path& _Path) {
    // current C++ standard supports only char and wchar_t streams
    if constexpr (_STD is_same_v<_Elem, char>) {
        _Stream << _Path.generic_string();
    } else if constexpr (_STD is_same_v<_Elem, wchar_t>) {
        _Stream << _Path.generic_wstring();
    }

    return _Stream;
}

template _FILESYSTEM_API _NODISCARD ostream& __cdecl operator<<(ostream&, const path&);
template _FILESYSTEM_API _NODISCARD wostream& __cdecl operator<<(wostream&, const path&);

// FUNCTION TEMPLATE operator+
_NODISCARD path __cdecl operator+(const path& _Left, const path& _Right) {
    return path(_Left.generic_string() + _Right.generic_string());
}

template <class _CharT>
_NODISCARD path __cdecl operator+(const path& _Left, const _CharT* const _Right) {
    typename path::string_type _Result;

    if constexpr (_STD is_same_v<_CharT, char>) {
        _Result = _Left.generic_string() + _Right;
    } else if constexpr (_STD is_same_v<_CharT, wchar_t>) {
        _Result = _Left.generic_string() + _Convert_wide_to_narrow(code_page::utf8, _Right);
    } else { // const char8_t*, const char16_t* and const char32_t*
        _Result = _Left.generic_string() + _Convert_utf_to_narrow<_CharT, char_traits<_CharT>>(_Right);
    }

    return path(_Result);
}

template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const char* const);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const char8_t* const);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const char16_t* const);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const char32_t* const);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const wchar_t* const);

template <class _CharT>
_NODISCARD path __cdecl operator+(const _CharT* const _Left, const path& _Right) {
    typename path::string_type _Result;

    if constexpr (_STD is_same_v<_CharT, char>) {
        _Result = _Left + _Right.generic_string();
    } else if constexpr (_STD is_same_v<_CharT, wchar_t>) {
        _Result = _Convert_wide_to_narrow(_Left) + _Right.generic_string();
    } else { // const char8_t*, const char16_t* and const char32_t*
        _Result = _Convert_utf_to_narrow<_CharT, char_traits<_CharT>>(_Left) + _Right.generic_string();
    }

    return path(_Result);
}

template <class _Elem, class _Traits, class _Alloc>
_NODISCARD path __cdecl operator+(const path& _Left, const basic_string<_Elem, _Traits, _Alloc>& _Right) {
    typename path::string_type _Result;

    if constexpr (_STD is_same_v<_Elem, char>) {
        _Result = _Left.generic_string() + _Right;
    } else if constexpr (_STD is_same_v<_Elem, wchar_t>) {
        _Result = _Left.generic_string() + _Convert_wide_to_narrow(code_page::utf8, _Right);
    } else { // u8string, u16string and u32string
        _Result = _Left.generic_string() + _Convert_utf_to_narrow<_Elem, _Traits>(_Right);
    }

    return path(_Result);
}

template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const string&);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const u8string&);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const u16string&);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const u32string&);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const wstring&);

template <class _Elem, class _Traits, class _Alloc>
_NODISCARD path __cdecl operator+(const basic_string<_Elem, _Traits, _Alloc>& _Left, const path& _Right) {
    typename path::string_type _Result;

    if constexpr (_STD is_same_v<_Elem, char>) {
        _Result = _Left + _Right.generic_string();
    } else if constexpr (_STD is_same_v<_Elem, wchar_t>) {
        _Result = _Convert_wide_to_narrow(code_page::utf8, _Left) + _Right.generic_string();
    } else { // u8string, u16string and u32string
        _Result = _Convert_utf_to_narrow<_Elem, _Traits>(_Left) + _Right.generic_string();
    }

    return path(_Result);
}

template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const string&, const path&);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const u8string&, const path&);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const u16string&, const path&);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const u32string&, const path&);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const wstring&, const path&);

// FUNCTION TEMPLATE path::path
template <class _CharT, class>
__cdecl path::path(const _CharT* const _Source) {
    if constexpr (_STD is_same_v<_CharT, char>) {
        _Mytext = _Source;
    } else if constexpr (_STD is_same_v<_CharT, wchar_t>) {
        _Mytext = _Convert_wide_to_narrow(code_page::utf8, _Source);
    } else { // const char8_t*, const char16_t* and const char32_t*
        _Mytext = _Convert_utf_to_narrow<_CharT, char_traits<_CharT>>(_Source);
    }

    _Check_size();
}

template _FILESYSTEM_API __cdecl path::path(const char* const);
template _FILESYSTEM_API __cdecl path::path(const char8_t* const);
template _FILESYSTEM_API __cdecl path::path(const char16_t* const);
template _FILESYSTEM_API __cdecl path::path(const char32_t* const);
template _FILESYSTEM_API __cdecl path::path(const wchar_t* const);

template <class _Src, class>
__cdecl path::path(const _Src& _Source) {
    if constexpr (_STD is_same_v<typename _Src::value_type, char>) {
        _Mytext = _Source.data();
    } else if constexpr (_STD is_same_v<typename _Src::value_type, wchar_t>) {
        _Mytext = _Convert_wide_to_narrow(code_page::utf8, _Source.data());
    } else { // u8string / u8string_view, u16string / u16string_view and u32string / u32string_view
        _Mytext = _Convert_utf_to_narrow<typename _Src::value_type, typename _Src::traits_type>(_Source.data());
    }

    _Check_size();
} 

template _FILESYSTEM_API __cdecl path::path(const string&);
template _FILESYSTEM_API __cdecl path::path(const u8string&);
template _FILESYSTEM_API __cdecl path::path(const u16string&);
template _FILESYSTEM_API __cdecl path::path(const u32string&);
template _FILESYSTEM_API __cdecl path::path(const wstring&);

template _FILESYSTEM_API __cdecl path::path(const string_view&);
template _FILESYSTEM_API __cdecl path::path(const u8string_view&);
template _FILESYSTEM_API __cdecl path::path(const u16string_view&);
template _FILESYSTEM_API __cdecl path::path(const u32string_view&);
template _FILESYSTEM_API __cdecl path::path(const wstring_view&);

// FUNCTION path::_Check_size
void __thiscall path::_Check_size() const {
    // path cannot be longer than 260 characters
    if (_Mytext.size() > static_cast<size_t>(MAX_PATH)) {
        _Throw_system_error("_Check_size", "invalid length", error_type::length_error);
    }
}

// FUNCTION path::operator=
path& __cdecl path::operator=(const path& _Source) {
    if (this != __builtin_addressof(_Source)) { // avoid assigning own value
        _Mytext = _Source._Mytext;
        _Check_size();
    }
    
    return *this;
}

template <class _CharT, class>
path& __cdecl path::operator=(const _CharT* const _Source) {
    if constexpr (_STD is_same_v<_CharT, char>) {
        _Mytext = _Source;
    } else if constexpr (_STD is_same_v<_CharT, wchar_t>) {
        _Mytext = _Convert_wide_to_narrow(code_page::utf8, _Source);
    } else { // const char8_t*, const char16_t* and const char32_t*
        _Mytext = _Convert_utf_to_narrow<_CharT, char_traits<_CharT>>(_Source);
    }

    _Check_size();
    return *this;
}

template _FILESYSTEM_API path& __cdecl path::operator=(const char* const);
template _FILESYSTEM_API path& __cdecl path::operator=(const char8_t* const);
template _FILESYSTEM_API path& __cdecl path::operator=(const char16_t* const);
template _FILESYSTEM_API path& __cdecl path::operator=(const char32_t* const);
template _FILESYSTEM_API path& __cdecl path::operator=(const wchar_t* const);

template<class _Src, class>
path& __cdecl path::operator=(const _Src& _Source) {
    if constexpr (_STD is_same_v<typename _Src::value_type, char>) {
        _Mytext = _Source.data();
    } else if constexpr (_STD is_same_v<typename _Src::value_type, wchar_t>) {
        _Mytext = _Convert_wide_to_narrow(code_page::utf8, _Source.data());
    } else { // u8string / u8string_view, u16string / u16string_view and u32string / u32string_view
        _Mytext = _Convert_utf_to_narrow<typename _Src::value_type, typename _Src::traits_type>(_Source.data());
    }

    _Check_size();
    return *this;
}

template _FILESYSTEM_API path& __cdecl path::operator=(const string&);
template _FILESYSTEM_API path& __cdecl path::operator=(const u8string&);
template _FILESYSTEM_API path& __cdecl path::operator=(const u16string&);
template _FILESYSTEM_API path& __cdecl path::operator=(const u32string&);
template _FILESYSTEM_API path& __cdecl path::operator=(const wstring&);

template _FILESYSTEM_API path& __cdecl path::operator=(const string_view&);
template _FILESYSTEM_API path& __cdecl path::operator=(const u8string_view&);
template _FILESYSTEM_API path& __cdecl path::operator=(const u16string_view&);
template _FILESYSTEM_API path& __cdecl path::operator=(const u32string_view&);
template _FILESYSTEM_API path& __cdecl path::operator=(const wstring_view&);

// FUNCTION path::assign
path& __cdecl path::assign(const path& _Source) {
    return operator=(_Source);
}

template <class _CharT, class>
path& __cdecl path::assign(const _CharT* const _Source) {
    return operator=(_Source);
}

template <class _Src, class>
path& __cdecl path::assign(const _Src& _Source) {
    return operator=(_Source);
}

// FUNCTION path::operator+=
path& __cdecl path::operator+=(const path& _Added) {
    if (this != __builtin_addressof(_Added)) { // avoid appending own value
        _Mytext += _Added._Mytext;
        _Check_size();
    }

    return *this;
}

template <class _CharT, class>
path& __cdecl path::operator+=(const _CharT* const _Added) {
    if constexpr (_STD is_same_v<_CharT, char>) {
        _Mytext += _Added;
    } else if constexpr (_STD is_same_v<_CharT, wchar_t>) {
        _Mytext += _Convert_wide_to_narrow(code_page::utf8, _Added);
    } else { // const char8_t*, const char16_t* and const char32_t*
        _Mytext += _Convert_utf_to_narrow<_CharT, char_traits<_CharT>>(_Added);
    }

    _Check_size();
    return *this;
}

template _FILESYSTEM_API path& __cdecl path::operator+=(const char* const);
template _FILESYSTEM_API path& __cdecl path::operator+=(const char8_t* const);
template _FILESYSTEM_API path& __cdecl path::operator+=(const char16_t* const);
template _FILESYSTEM_API path& __cdecl path::operator+=(const char32_t* const);
template _FILESYSTEM_API path& __cdecl path::operator+=(const wchar_t* const);

template <class _Src, class>
path& __cdecl path::operator+=(const _Src& _Added) {
    if constexpr (_STD is_same_v<typename _Src::value_type, char>) {
        _Mytext += _Added.data();
    } else if constexpr (_STD is_same_v<typename _Src::value_type, wchar_t>) {
        _Mytext += _Convert_wide_to_narrow(code_page::utf8, _Added.data());
    } else { // u8string / u8string_view, u16string / u16string_view and u32string / u32string_view
        _Mytext += _Convert_utf_to_narrow<typename _Src::value_type, typename _Src::traits_type>(_Added.data());
    }

    _Check_size();
    return *this;
}

template _FILESYSTEM_API path& __cdecl path::operator+=(const string&);
template _FILESYSTEM_API path& __cdecl path::operator+=(const u8string&);
template _FILESYSTEM_API path& __cdecl path::operator+=(const u16string&);
template _FILESYSTEM_API path& __cdecl path::operator+=(const u32string&);
template _FILESYSTEM_API path& __cdecl path::operator+=(const wstring&);

template _FILESYSTEM_API path& __cdecl path::operator+=(const string_view&);
template _FILESYSTEM_API path& __cdecl path::operator+=(const u8string_view&);
template _FILESYSTEM_API path& __cdecl path::operator+=(const u16string_view&);
template _FILESYSTEM_API path& __cdecl path::operator+=(const u32string_view&);
template _FILESYSTEM_API path& __cdecl path::operator+=(const wstring_view&);

// FUNCTION path::append
path& __cdecl path::append(const path& _Added) {
    return operator+=(_Added);
}

template <class _CharT, class>
path& __cdecl path::append(const _CharT* const _Added) {
    return operator+=(_Added);
}

template <class _Src, class>
path& __cdecl path::append(const _Src& _Added) {
    return operator+=(_Added);
}

// FUNCTION path::operator==
bool __cdecl path::operator==(const path& _Compare) const noexcept {
    // avoid comparing with own value
    return this != __builtin_addressof(_Compare) ? _Mytext == _Compare._Mytext : true;
}

template <class _CharT, class>
bool __cdecl path::operator==(const _CharT* const _Compare) const {
    if constexpr (_STD is_same_v<_CharT, char>) {
        return _Mytext == _Compare;
    } else if constexpr (_STD is_same_v<_CharT, wchar_t>) {
        return _Mytext == _Convert_wide_to_narrow(code_page::utf8, _Compare);
    } else { // const char8_t*, const char16_t* and const char32_t*
        return _Mytext == _Convert_utf_to_narrow<_CharT, char_traits<_CharT>>(_Compare);
    }
}

template _FILESYSTEM_API bool __cdecl path::operator==(const char* const) const;
template _FILESYSTEM_API bool __cdecl path::operator==(const char8_t* const) const;
template _FILESYSTEM_API bool __cdecl path::operator==(const char16_t* const) const;
template _FILESYSTEM_API bool __cdecl path::operator==(const char32_t* const) const;
template _FILESYSTEM_API bool __cdecl path::operator==(const wchar_t* const) const;

template <class _Src, class>
bool __cdecl path::operator==(const _Src& _Compare) const {
    if constexpr (_STD is_same_v<typename _Src::value_type, char>) {
        return _Mytext == _Compare.data();
    } else if constexpr (_STD is_same_v<typename _Src::value_type, wchar_t>) {
        return _Mytext == _Convert_wide_to_narrow(code_page::utf8, _Compare.data());
    } else { // u8string / u8string_view, u16string / u16string_view and u32string / u32string_view
        return _Mytext == _Convert_utf_to_narrow<typename _Src::value_type, typename _Src::traits_type>(_Compare.data());
    } 
}

template _FILESYSTEM_API bool __cdecl path::operator==(const string&) const;
template _FILESYSTEM_API bool __cdecl path::operator==(const u8string&) const;
template _FILESYSTEM_API bool __cdecl path::operator==(const u16string&) const;
template _FILESYSTEM_API bool __cdecl path::operator==(const u32string&) const;
template _FILESYSTEM_API bool __cdecl path::operator==(const wstring&) const;

template _FILESYSTEM_API bool __cdecl path::operator==(const string_view&) const;
template _FILESYSTEM_API bool __cdecl path::operator==(const u8string_view&) const;
template _FILESYSTEM_API bool __cdecl path::operator==(const u16string_view&) const;
template _FILESYSTEM_API bool __cdecl path::operator==(const u32string_view&) const;
template _FILESYSTEM_API bool __cdecl path::operator==(const wstring_view&) const;

// FUNCTION path::operator!=
bool __cdecl path::operator!=(const path& _Compare) const noexcept {
    // avoid comparing with own value
    return this != __builtin_addressof(_Compare) ? _Mytext != _Compare._Mytext : false;
}

template <class _CharT, class>
bool __cdecl path::operator!=(const _CharT* const _Compare) const {
    if constexpr (_STD is_same_v<_CharT, char>) {
        return _Mytext != _Compare;
    } else if constexpr (_STD is_same_v<_CharT, wchar_t>) {
        return _Mytext != _Convert_wide_to_narrow(code_page::utf8, _Compare);
    } else { // const char8_t*, const char16_t* and const char32_t*
        return _Mytext != _Convert_utf_to_narrow<_CharT, char_traits<_CharT>>(_Compare);
    }
}

template _FILESYSTEM_API bool __cdecl path::operator!=(const char* const) const;
template _FILESYSTEM_API bool __cdecl path::operator!=(const char8_t* const) const;
template _FILESYSTEM_API bool __cdecl path::operator!=(const char16_t* const) const;
template _FILESYSTEM_API bool __cdecl path::operator!=(const char32_t* const) const;
template _FILESYSTEM_API bool __cdecl path::operator!=(const wchar_t* const) const;

template <class _Src, class>
bool __cdecl path::operator!=(const _Src& _Compare) const {
    if constexpr (_STD is_same_v<typename _Src::value_type, char>) {
        return _Mytext != _Compare.data();
    } else if constexpr (_STD is_same_v<typename _Src::value_type, wchar_t>) {
        return _Mytext != _Convert_wide_to_narrow(code_page::utf8, _Compare.data());
    } else { // u8string / u8string_view, u16string / u16string_view and u32string / u32string_view
        return _Mytext != _Convert_utf_to_narrow<typename _Src::value_type, typename _Src::traits_type>(_Compare.data());
    }
}

template _FILESYSTEM_API bool __cdecl path::operator!=(const string&) const;
template _FILESYSTEM_API bool __cdecl path::operator!=(const u8string&) const;
template _FILESYSTEM_API bool __cdecl path::operator!=(const u16string&) const;
template _FILESYSTEM_API bool __cdecl path::operator!=(const u32string&) const;
template _FILESYSTEM_API bool __cdecl path::operator!=(const wstring&) const;

template _FILESYSTEM_API bool __cdecl path::operator!=(const string_view&) const;
template _FILESYSTEM_API bool __cdecl path::operator!=(const u8string_view&) const;
template _FILESYSTEM_API bool __cdecl path::operator!=(const u16string_view&) const;
template _FILESYSTEM_API bool __cdecl path::operator!=(const u32string_view&) const;
template _FILESYSTEM_API bool __cdecl path::operator!=(const wstring_view&) const;

// FUNCTION path::clear
void __thiscall path::clear() noexcept {
    _Mytext.clear();
}

// FUNCTION path::drive
_NODISCARD path __thiscall path::drive() const noexcept {
    // if has drive then first letter is drive
    return has_drive() ? string_type(1, _Mytext[0]) : string_type();
}

// FUNCTION path::empty
_NODISCARD bool __thiscall path::empty() const noexcept {
    return _Mytext.empty();
}

// FUNCTION path::extension
_NODISCARD path __thiscall path::extension() const noexcept {
    // if has extension, then everything after last dot is extension
    return has_extension() ? string_type(_Mytext, _Mytext.find_last_of(".") + 1, _Mytext.size()) : string_type();
}

// FUNCTION path::file
_NODISCARD path __thiscall path::file() const noexcept {
    if (has_file()) { // if has file, then everything after last slash is filename
        return _Mytext.find(_Expected_slash) != string_type::npos ?
            string_type(_Mytext, _Mytext.find_last_of(_Expected_slash) + 1, _Mytext.size()) : string_type(_Mytext);
    }

    return string_type();
}

// FUNCTION path::fix
_NODISCARD path& __thiscall path::fix() noexcept {
    string_type _Fixed;

    for (size_t _Idx = 0; _Idx < _Mytext.size(); ++_Idx) { // leave only single slashs
        if (_Fixed.size() > 0) {
            if ((_Mytext[_Idx] == _Expected_slash || _Mytext[_Idx] == _Unexpected_slash)
                && (_Fixed.back() == _Expected_slash || _Fixed.back() == _Unexpected_slash)) {
                continue; // skip slash if is next in the row
            }
        }

        _Fixed.push_back(_Mytext[_Idx]);
    }

    _Mytext = _Fixed;
    (void) make_preferred();
    return *this;
}

// FUNCTION path::generic_string
_NODISCARD const string __thiscall path::generic_string() const noexcept {
    return _Mytext;
}

// FUNCTION path::generic_u8string
_NODISCARD const u8string __thiscall path::generic_u8string() const noexcept {
    return _Convert_narrow_to_utf<char8_t, char_traits<char8_t>>(_Mytext.data());
}

// FUNCTION path::generic_u16string
_NODISCARD const u16string __thiscall path::generic_u16string() const noexcept {
    return _Convert_narrow_to_utf<char16_t, char_traits<char16_t>>(_Mytext.data());
}

// FUNCTION path::generic_u32string
_NODISCARD const u32string __thiscall path::generic_u32string() const noexcept {
    return _Convert_narrow_to_utf<char32_t, char_traits<char32_t>>(_Mytext.data());
}

// FUNCTION path::generic_wstring
_NODISCARD const wstring __thiscall path::generic_wstring() const noexcept {
    return _Convert_narrow_to_wide(code_page::utf8, _Mytext.data());
}

// FUNCTION path::has_drive
_NODISCARD bool __thiscall path::has_drive() const noexcept {
    if (_Mytext.size() < 3) { // requires minimum 3 characters
        return false;
    }

    if (_Mytext[1] != ':') { // second character must be ":"
        return false;
    }

    // third character must be expected or unexpected slash
    if (_Mytext[2] != _Expected_slash && _Mytext[2] != _Unexpected_slash) {
        return false;
    }

    if (static_cast<int>(_Mytext[0]) >= 65 && static_cast<int>(_Mytext[0]) <= 90) { // only big letters
        return true;
    } else if (static_cast<int>(_Mytext[0]) >= 97 && static_cast<int>(_Mytext[0]) <= 122) { // only small letters
        return true;
    } else { // other characters
        return false;
    }

    return false;
}

// same as string[_view]::npos
#define _NPOS static_cast<size_t>(-1)

// FUNCTION path::has_extension
_NODISCARD bool __thiscall path::has_extension() const noexcept {
    if (_Mytext.find(".") != _NPOS) {
        const size_t _Dot_pos = _Mytext.find_last_of(".");

        // example of this case:
        // "Disk:\Directory\Subdirectory."
        if (_Dot_pos < _Mytext.size()) {
            if (const size_t _Last = _Mytext.find_last_of(_Expected_slash);
                _Last < _Mytext.size() && _Last > _Dot_pos) { // for example: "Disk:\File.Extension\"
                return false;
            }

            // for example: "Disk:\File."
            return _Dot_pos != _Mytext.size() - 1;
        } else {
            return false;
        }
    }

    return false;
}

// FUNCTION path::has_file
_NODISCARD bool __thiscall path::has_file() const noexcept {
    // file cannot exists without extension
    return has_extension();
}

// FUNCTION path::has_parent_directory
_NODISCARD bool __thiscall path::has_parent_directory() const noexcept {
    // if has root directory, then must be longer than root path, otherwise all path is parent path
    return has_root_directory() ? _Mytext.size() > root_path().generic_string().size() : true;
}

// FUNCTION path::has_root_directory
_NODISCARD bool __thiscall path::has_root_directory() const noexcept {
    // slash on first place where there's no drive means, that it's root directory
    if (!has_drive() && (_Mytext[0] == _Expected_slash || _Mytext[0] == _Unexpected_slash)) {
        return true;
    }

    // directory after drive is root directory
    if (has_drive() && _Mytext.size() > 3) {
        return true;
    }

    return false;
}

// FUNCTION path::has_stem
_NODISCARD bool __thiscall path::has_stem() const noexcept {
    // stem is just filename without extension
    return has_file();
}

// FUNCTION path::is_absolute
_NODISCARD bool __thiscall path::is_absolute() const noexcept {
    if (_Mytext.find(_Expected_slash) == _NPOS
        && _Mytext.find(_Unexpected_slash) == _NPOS) { // path without any slash
        return false;
    }

    // path with drive and/or root directory must be absolute
    if (has_drive() || has_root_directory()) {
        return true;
    }

    return false;
}

// FUNCTION path::is_relative
_NODISCARD bool __thiscall path::is_relative() const noexcept {
    return !is_absolute();
}

// FUNCTION path::make_preferred
_NODISCARD path& __thiscall path::make_preferred() noexcept {
    if (!empty()) {
        _STD replace(_Mytext.begin(), _Mytext.end(), _Unexpected_slash, _Expected_slash);
    }
    
    return *this;
}

// FUNCTION path::parent_directory
_NODISCARD path __thiscall path::parent_directory() const noexcept {
    // if has parent directory, then return only first directory from parent path
    return has_parent_directory() ? string_type(parent_path().generic_string(), 0,
        parent_path().generic_string().find_first_of(_Expected_slash)) : string_type();
}

// FUNCTION path::parent_path
_NODISCARD path __thiscall path::parent_path() const noexcept {
    if (has_parent_directory()) { // parent path is everything after root directory
        if (has_drive()) { // remove drive and root directory (with ":" and 2 slashes)
            return string_type(_Mytext, drive().generic_string().size()
                + root_directory().generic_string().size() + 3, _Mytext.size());
        } else if (!has_drive() && has_root_directory()) { // remove only root directory (with 2 slashes)
            return string_type(_Mytext, root_directory().generic_string().size() + 2, _Mytext.size());
        } else { // nothing to do, because current working path is parent path
            return _Mytext;
        }
    }

    return string_type();
}

// FUNCTION path::remove_extension
_NODISCARD path& __thiscall path::remove_extension() noexcept {
    if (has_extension()) {
        _Mytext.resize(_Mytext.find_last_of("."));
    }

    return *this;
}

// FUNCTION path::remove_file
_NODISCARD path& __cdecl path::remove_file(const bool _With_slash) noexcept {
    if (has_file()) {
        // if path contains only filename, then clear it
        _Mytext.find(_Expected_slash) != _NPOS ?
            _Mytext.resize(_With_slash ? _Mytext.find_last_of(_Expected_slash)
                : _Mytext.find_last_of(_Expected_slash) + 1) : this->clear();
    }

    return *this;
}

// FUNCTION path::replace_extension
_NODISCARD path& __cdecl path::replace_extension(const path& _Replacement) {
    if (has_extension()) {
        (void) remove_extension();
        *this += _Replacement.generic_string()[0] == '.' ?
            _Replacement : "." + _Replacement.generic_string();
        _Check_size();
    }

    return *this;
}

// FUNCTION path::replace_file
_NODISCARD path& __cdecl path::replace_file(const path& _Replacement) {
    if (has_file()) {
        (void) remove_file(_Replacement.generic_string()[0] == _Expected_slash
            || _Replacement.generic_string()[0] == _Unexpected_slash);
        *this += _Replacement;
        _Check_size();
    }

    return *this;
}

// FUNCTION path::replace_stem
_NODISCARD path& __cdecl path::replace_stem(const path& _Replacement) {
    if (has_stem()) {
        const auto& _Ext = extension();

        (void) remove_file(false);
        *this += _Replacement;
        *this += "." + _Ext.generic_string();
        _Check_size();
    }

    return *this;
}

// FUNCTION path::root_directory
_NODISCARD path __thiscall path::root_directory() const noexcept {
    // if has root directory, then return only first directory from root path
    return has_root_directory() ? string_type(root_path().generic_string(), 0,
        root_path().generic_string().find_first_of(_Expected_slash)) : string_type();
}

// FUNCTION path::root_path
_NODISCARD path __thiscall path::root_path() const noexcept {
    // if has root directory, then return everything after first slash
    return has_root_directory() ? string_type(_Mytext, _Mytext.find_first_of(_Expected_slash), _Mytext.size()) : string_type();
}

// FUNCTION path::stem
_NODISCARD path __thiscall path::stem() const noexcept {
    if (has_stem()) {
        auto _Stem = _Mytext; // don't change original text

        if (_Stem.find_last_of(_Expected_slash) != _NPOS) {
            _Stem.replace(0, _Stem.find_last_of(_Expected_slash) + 1, string_type()); // leave only filename with extension
            _Stem.resize(_Stem.find_last_of(".")); // remove extension
            return _Stem; // break here
        }
        
        // path contains only filename
        _Stem.resize(_Stem.find_first_of(".")); // remove extension
        return _Stem;
    }
    
    return string_type();
}

// FUNCTION current_path
_NODISCARD path __stdcall current_path() noexcept {
    wchar_t _Buff[MAX_PATH];
    if (!_CSTD GetCurrentDirectoryW(MAX_PATH, _Buff)) { // failed to get current path
        _Throw_fs_error("failed to get current path", error_type::runtime_error, "current_path");
    }

    return path(wstring(_Buff));
}

_NODISCARD bool __cdecl current_path(const path& _Path) { // sets new current path
    // path cannot be longer than 260 characters
    if (_Path.generic_string().size() > static_cast<size_t>(MAX_PATH)) {
        _Throw_system_error("set_path", "invalid length", error_type::length_error);
    }

    if (!_CSTD SetCurrentDirectoryW(_Path.generic_wstring().c_str())) { // failed to set new path
        _Throw_fs_error("failed to set new path", error_type::runtime_error, "set_path");
    }

    return true;
}

namespace path_literals {
    // FUNCTION operator""p
    _NODISCARD path __cdecl operator""__p(const char* const _Str, const size_t _Size) noexcept {
        return path(_Str);
    }

    _NODISCARD path __cdecl operator""__p(const char8_t* const _Str, const size_t _Size) noexcept {
        return path(_Str);
    }

    _NODISCARD path __cdecl operator""__p(const char16_t* const _Str, const size_t _Size) noexcept {
        return path(_Str);
    }

    _NODISCARD path __cdecl operator""__p(const char32_t* const _Str, const size_t _Size) noexcept {
        return path(_Str);
    }

    _NODISCARD path __cdecl operator""__p(const wchar_t* const _Str, const size_t _Size) noexcept {
        return path(_Str);
    }
} // path_literals
_FILESYSTEM_END

#endif // !_HAS_WINDOWS