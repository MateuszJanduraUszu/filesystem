// path.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <pch.h>
#include <filesystem.hpp>

#if !_HAS_WINDOWS
#pragma message("The contents of <path.cpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv
#pragma warning(push)
#pragma warning(disable : 4834) // C4834: discard function with [nodiscard] attribute

_FILESYSTEM_BEGIN
_EXPERIMENTAL_BEGIN
// FUNCTION TEMPLATE path::operator+
_NODISCARD path __cdecl operator+(const path& _Left, const path& _Right) {
    const auto _Result = _Left._Text + _Right._Text;
    return path(_Result);
}

template<class _CharT>
_NODISCARD path __cdecl operator+(const path& _Left, const _CharT* const _Right) {
    auto _Result = _Left._Text;

    if constexpr (_STD is_same_v<_CharT, char>) {
        _Result = _Left._Text + _Right;
    } else if constexpr (_STD is_same_v<_CharT, wchar_t>) {
        _Result = _Left._Text + _Convert_wide_to_narrow(code_page::utf8, _Right);
    } else { // const char8_t*, const char16_t* and const char32_t*
        _Result = _Left._Text + _Convert_utf_to_narrow<_CharT, char_traits<_CharT>>(_Right);
    }

    return path(_Result);
}

template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const char* const);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const char8_t* const);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const char16_t* const);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const char32_t* const);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const wchar_t* const);

template<class _CharT>
_NODISCARD path __cdecl operator+(const _CharT* const _Left, const path& _Right) {
    auto _Result = _Right._Text;

    if constexpr (_STD is_same_v<_CharT, char>) {
        _Result = _Left + _Right._Text;
    } else if constexpr (_STD is_same_v<_CharT, wchar_t>) {
        _Result = _Convert_wide_to_narrow(_Left) + _Right._Text;
    } else { // const char8_t*, const char16_t* and const char32_t*
        _Result = _Convert_utf_to_narrow<_CharT, char_traits<_CharT>>(_Left) + _Right._Text;
    }

    return path(_Result);
}

template<class _Elem, class _Traits, class _Alloc>
_NODISCARD path __cdecl operator+(const path& _Left, const basic_string<_Elem, _Traits, _Alloc>& _Right) {
    auto _Result = _Left._Text;

    if constexpr (_STD is_same_v<_Elem, char>) {
        _Result = _Left._Text + _Right;
    } else if constexpr (_STD is_same_v<_Elem, wchar_t>) {
        _Result = _Left._Text + _Convert_wide_to_narrow(code_page::utf8, _Right);
    } else { // u8string, u16string and u32string
        _Result = _Left._Text + _Convert_utf_to_narrow<_Elem, _Traits>(_Right);
    }

    return path(_Result);
}

template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const string&);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const u8string&);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const u16string&);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const u32string&);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const path&, const wstring&);

template<class _Elem, class _Traits, class _Alloc>
_NODISCARD path __cdecl operator+(const basic_string<_Elem, _Traits, _Alloc>& _Left, const path& _Right) {
    auto _Result = _Right._Text;

    if constexpr (_STD is_same_v<_Elem, char>) {
        _Result = _Left + _Right._Text;
    } else if constexpr (_STD is_same_v<_Elem, wchar_t>) {
        _Result = _Convert_wide_to_narrow(code_page::utf8, _Left) + _Right._Text;
    } else { // u8string, u16string and u32string
        _Result = _Convert_utf_to_narrow<_Elem, _Traits>(_Left) + _Right._Text;
    }

    return path(_Result);
}

template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const string&, const path&);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const u8string&, const path&);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const u16string&, const path&);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const u32string&, const path&);
template _FILESYSTEM_API _NODISCARD path __cdecl operator+(const wstring&, const path&);

// FUNCTION path::operator>>
template<class _Elem, class _Traits = char_traits<_Elem>>
_NODISCARD basic_istream<_Elem, _Traits>& __cdecl operator>>(basic_istream<_Elem, _Traits>& _Stream, path& _Path) {
    basic_string<_Elem, _Traits, allocator<_Elem>> _Input;
    _Stream >> _Input;
    _Path = _STD move(_Input);
    return _Stream;
}

template _FILESYSTEM_API _NODISCARD istream& __cdecl operator>>(istream&, path&);
template _FILESYSTEM_API _NODISCARD wistream& __cdecl operator>>(wistream&, path&);

// FUNCTION path::operator<<
template<class _Elem, class _Traits = char_traits<_Elem>>
_NODISCARD basic_ostream<_Elem, _Traits>& __cdecl operator<<(basic_ostream<_Elem, _Traits>& _Stream, const path& _Path) {
    // current C++ standard supports only char and wchar_t streams
    if constexpr (_STD is_same_v<_Elem, char>) {
        _Stream << _Path._Text;
    } else if constexpr (_STD is_same_v<_Elem, wchar_t>) {
        _Stream << _Path.generic_wstring();
    }

    return _Stream;
}

template _FILESYSTEM_API _NODISCARD ostream& __cdecl operator<<(ostream&, const path&);
template _FILESYSTEM_API _NODISCARD wostream& __cdecl operator<<(wostream&, const path&);

// FUNCTION TEMPLATE path::path
template<class _CharT, class>
__cdecl path::path(const _CharT* const _Source) {
    if constexpr (_STD is_same_v<_CharT, char>) {
        this->_Text = _Source;
    } else if constexpr (_STD is_same_v<_CharT, wchar_t>) {
        this->_Text = _Convert_wide_to_narrow(code_page::utf8, _Source);
    } else { // const char8_t*, const char16_t* and const char32_t*
        this->_Text = _Convert_utf_to_narrow<_CharT, char_traits<_CharT>>(_Source);
    }

    this->_Check_size();
}

template _FILESYSTEM_API __cdecl path::path(const char* const);
template _FILESYSTEM_API __cdecl path::path(const char8_t* const);
template _FILESYSTEM_API __cdecl path::path(const char16_t* const);
template _FILESYSTEM_API __cdecl path::path(const char32_t* const);
template _FILESYSTEM_API __cdecl path::path(const wchar_t* const);

template<class _Src, class>
__cdecl path::path(const _Src& _Source) {
    if constexpr (_STD is_same_v<typename _Src::value_type, char>) {
        this->_Text = _Source.data();
    } else if constexpr (_STD is_same_v<typename _Src::value_type, wchar_t>) {
        this->_Text = _Convert_wide_to_narrow(code_page::utf8, _Source.data());
    } else { // u8string / u8string_view, u16string / u16string_view and u32string / u32string_view
        this->_Text = _Convert_utf_to_narrow<typename _Src::value_type,
            typename _Src::traits_type>(_Source.data());
    }

    this->_Check_size();
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
    if (this->_Text.size() > static_cast<size_t>(MAX_PATH)) {
        _Throw_system_error("_Check_size", "invalid length", error_type::length_error);
    }
}

// FUNCTION path::operator=
path& __cdecl path::operator=(const path& _Source) {
    if (this != __builtin_addressof(_Source)) { // avoid assigning own value
        this->_Text = _Source._Text;
        this->_Check_size();
    }
    
    return *this;
}

template<class _CharT, class>
path& __cdecl path::operator=(const _CharT* const _Source) {
    if constexpr (_STD is_same_v<_CharT, char>) {
        this->_Text = _Source;
    } else if constexpr (_STD is_same_v<_CharT, wchar_t>) {
        this->_Text = _Convert_wide_to_narrow(code_page::utf8, _Source);
    } else { // const char8_t*, const char16_t* and const char32_t*
        this->_Text = _Convert_utf_to_narrow<_CharT, char_traits<_CharT>>(_Source);
    }

    this->_Check_size();
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
        this->_Text = _Source.data();
    } else if constexpr (_STD is_same_v<typename _Src::value_type, wchar_t>) {
        this->_Text = _Convert_wide_to_narrow(code_page::utf8, _Source.data());
    } else { // u8string / u8string_view, u16string / u16string_view and u32string / u32string_view
        this->_Text = _Convert_utf_to_narrow<typename _Src::value_type,
            typename _Src::traits_type>(_Source.data());
    }

    this->_Check_size();
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
    return this->operator=(_Source);
}

template<class _CharT, class>
path& __cdecl path::assign(const _CharT* const _Source) {
    return this->operator=(_Source);
}

template<class _Src, class>
path& __cdecl path::assign(const _Src& _Source) {
    return this->operator=(_Source);
}

// FUNCTION path::operator+=
path& __cdecl path::operator+=(const path& _Added) {
    if (this != __builtin_addressof(_Added)) { // avoid appending own value
        this->_Text += _Added._Text;
        this->_Check_size();
    }

    this->_Check_size();
    return *this;
}

template<class _CharT, class>
path& __cdecl path::operator+=(const _CharT* const _Added) {
    if constexpr (_STD is_same_v<_CharT, char>) {
        this->_Text += _Added;
    } else if constexpr (_STD is_same_v<_CharT, wchar_t>) {
        this->_Text += _Convert_wide_to_narrow(code_page::utf8, _Added);
    } else { // const char8_t*, const char16_t* and const char32_t*
        this->_Text += _Convert_utf_to_narrow<_CharT, char_traits<_CharT>>(_Added);
    }

    this->_Check_size();
    return *this;
}

template _FILESYSTEM_API path& __cdecl path::operator+=(const char* const);
template _FILESYSTEM_API path& __cdecl path::operator+=(const char8_t* const);
template _FILESYSTEM_API path& __cdecl path::operator+=(const char16_t* const);
template _FILESYSTEM_API path& __cdecl path::operator+=(const char32_t* const);
template _FILESYSTEM_API path& __cdecl path::operator+=(const wchar_t* const);

template<class _Src, class>
path& __cdecl path::operator+=(const _Src& _Added) {
    if constexpr (_STD is_same_v<typename _Src::value_type, char>) {
        this->_Text += _Added.data();
    } else if constexpr (_STD is_same_v<typename _Src::value_type, wchar_t>) {
        this->_Text += _Convert_wide_to_narrow(code_page::utf8, _Added.data());
    } else { // u8string / u8string_view, u16string / u16string_view and u32string / u32string_view
        this->_Text += _Convert_utf_to_narrow<typename _Src::value_type,
            typename _Src::traits_type>(_Added.data());
    }

    this->_Check_size();
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
    return this->operator+=(_Added);
}

template<class _CharT, class>
path& __cdecl path::append(const _CharT* const _Added) {
    return this->operator+=(_Added);
}

template<class _Src, class>
path& __cdecl path::append(const _Src& _Added) {
    return this->operator+=(_Added);
}

// FUNCTION path::operator==
bool __cdecl path::operator==(const path& _Compare) const noexcept {
    bool _Result = true; // if _Compare._Text won't be compared with _Test, result will be true

    if (this != __builtin_addressof(_Compare)) { // avoid comparing with own value
        _Result = this->_Text == _Compare._Text;
    }

    return _Result;
}

template<class _CharT, class>
bool __cdecl path::operator==(const _CharT* const _Compare) const {
    bool _Result = false;

    if constexpr (_STD is_same_v<_CharT, char>) {
        _Result = this->_Text == _Compare;
    } else if constexpr (_STD is_same_v<_CharT, wchar_t>) {
        _Result = this->_Text == _Convert_wide_to_narrow(code_page::utf8, _Compare);
    } else { // const char8_t*, const char16_t* and const char32_t*
        _Result = this->_Text == _Convert_utf_to_narrow<_CharT, char_traits<_CharT>>(_Compare);
    }

    return _Result;
}

template _FILESYSTEM_API bool __cdecl path::operator==(const char* const) const;
template _FILESYSTEM_API bool __cdecl path::operator==(const char8_t* const) const;
template _FILESYSTEM_API bool __cdecl path::operator==(const char16_t* const) const;
template _FILESYSTEM_API bool __cdecl path::operator==(const char32_t* const) const;
template _FILESYSTEM_API bool __cdecl path::operator==(const wchar_t* const) const;

template<class _Src, class>
bool __cdecl path::operator==(const _Src& _Compare) const {
    bool _Result = false;

    if constexpr (_STD is_same_v<typename _Src::value_type, char>) {
        _Result = this->_Text == _Compare.data();
    } else if constexpr (_STD is_same_v<typename _Src::value_type, wchar_t>) {
        _Result = this->_Text == _Convert_wide_to_narrow(code_page::utf8, _Compare.data());
    } else { // u8string / u8string_view, u16string / u16string_view and u32string / u32string_view
        _Result = this->_Text == _Convert_utf_to_narrow<typename _Src::value_type,
            typename _Src::traits_type>(_Compare.data());
    } 
    
    return _Result;
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
    bool _Result = false; // // if _Compare._Text won't be compared with _Test, result will be false

    if (this != __builtin_addressof(_Compare)) { // avoid comparing with own value
        _Result = this->_Text != _Compare._Text;
    }

    return _Result;
}

template<class _CharT, class>
bool __cdecl path::operator!=(const _CharT* const _Compare) const {
    bool _Result = false;

    if constexpr (_STD is_same_v<_CharT, char>) {
        _Result = this->_Text != _Compare;
    } else if constexpr (_STD is_same_v<_CharT, wchar_t>) {
        _Result = this->_Text != _Convert_wide_to_narrow(code_page::utf8, _Compare);
    } else { // const char8_t*, const char16_t* and const char32_t*
        _Result = this->_Text != _Convert_utf_to_narrow<_CharT, char_traits<_CharT>>(_Compare);
    }

    return _Result;
}

template _FILESYSTEM_API bool __cdecl path::operator!=(const char* const) const;
template _FILESYSTEM_API bool __cdecl path::operator!=(const char8_t* const) const;
template _FILESYSTEM_API bool __cdecl path::operator!=(const char16_t* const) const;
template _FILESYSTEM_API bool __cdecl path::operator!=(const char32_t* const) const;
template _FILESYSTEM_API bool __cdecl path::operator!=(const wchar_t* const) const;

template<class _Src, class>
bool __cdecl path::operator!=(const _Src& _Compare) const {
    bool _Result = false;

    if constexpr (_STD is_same_v<typename _Src::value_type, char>) {
        _Result = this->_Text != _Compare.data();
    } else if constexpr (_STD is_same_v<typename _Src::value_type, wchar_t>) {
        _Result = this->_Text != _Convert_wide_to_narrow(code_page::utf8, _Compare.data());
    } else { // u8string / u8string_view, u16string / u16string_view and u32string / u32string_view
        _Result = this->_Text != _Convert_utf_to_narrow<typename _Src::value_type,
            typename _Src::traits_type>(_Compare.data());
    }

    return _Result;
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
    this->_Text.clear();
}

// FUNCTION path::drive
_NODISCARD path __thiscall path::drive() const noexcept {
    if (this->has_drive()) {
        return string_type(1, this->_Text[0]); // first character is drive
    }

    return *this;
}

// FUNCTION path::empty
_NODISCARD bool __thiscall path::empty() const noexcept {
    return this->_Text.empty();
}

// FUNCTION path::extension
_NODISCARD path __thiscall path::extension() const noexcept {
    if (this->has_extension()) {
        return string_type(this->_Text, this->_Text.find_last_of(".") + 1, this->_Text.size());
    }

    return *this;
}

// FUNCTION path::file
_NODISCARD path __thiscall path::file() const noexcept {
    if (this->has_file()) {
        return this->_Text.find(_Expected_slash) < this->_Text.size() ?
            string_type(this->_Text, this->_Text.find_last_of(_Expected_slash) + 1, this->_Text.size())
            : string_type(this->_Text);
    }

    return *this;
}

// FUNCTION path::fix
_NODISCARD path& __thiscall path::fix() noexcept {
    string_type _Fixed;

    for (size_t _Idx = 0; _Idx < this->_Text.size(); ++_Idx) { // leave only single slashs
        if (_Fixed.size() > 0) {
            if ((this->_Text[_Idx] == _Expected_slash || this->_Text[_Idx] == _Unexpected_slash)
                && (_Fixed.back() == _Expected_slash || _Fixed.back() == _Unexpected_slash)) {
                continue; // skip slash if is next in the row
            }
        }

        _Fixed.push_back(this->_Text[_Idx]);
    }

    this->_Text = _Fixed;
    this->make_preferred();
    return *this;
}

// FUNCTION path::generic_string
_NODISCARD const string __thiscall path::generic_string() const noexcept {
    return this->_Text;
}

// FUNCTION path::generic_u8string
_NODISCARD const u8string __thiscall path::generic_u8string() const {
    return _Convert_narrow_to_utf<char8_t, char_traits<char8_t>>(this->_Text.data());
}

// FUNCTION path::generic_u16string
_NODISCARD const u16string __thiscall path::generic_u16string() const {
    return _Convert_narrow_to_utf<char16_t, char_traits<char16_t>>(this->_Text.data());
}

// FUNCTION path::generic_u32string
_NODISCARD const u32string __thiscall path::generic_u32string() const {
    return _Convert_narrow_to_utf<char32_t, char_traits<char32_t>>(this->_Text.data());
}

// FUNCTION path::generic_wstring
_NODISCARD const wstring __thiscall path::generic_wstring() const {
    return _Convert_narrow_to_wide(code_page::utf8, this->_Text.data());
}

// FUNCTION path::has_drive
_NODISCARD bool __thiscall path::has_drive() const noexcept {
    if (this->_Text.size() < 3) { // requires minimum 3 characters
        return false;
    }

    if (this->_Text[1] != ':') { // second character must be ":"
        return false;
    }

    // third character must be expected or unexpected slash
    if (this->_Text[2] != _Expected_slash && this->_Text[2] != _Unexpected_slash) {
        return false;
    }

    if (static_cast<int>(this->_Text[0]) >= 65
        && static_cast<int>(this->_Text[0]) <= 90) { // only big letters
        return true;
    } else if (static_cast<int>(this->_Text[0]) >- 97
        && static_cast<int>(this->_Text[0]) <= 122) { // only small letters
        return true;
    } else { // other characters
        return false;
    }

    return false;
}

// FUNCTION path::has_extension
_NODISCARD bool __thiscall path::has_extension() const noexcept {
    if (this->_Text.find(".") < this->_Text.size()) {
        const size_t _Dot_pos = this->_Text.find_last_of(".");

        // example of this case:
        // "Disk:\Directory\Subdirectory."
        if (_Dot_pos < this->_Text.size()) {
            if (const size_t _Last = this->_Text.find_last_of(_Expected_slash);
                _Last < this->_Text.size() && _Last > _Dot_pos) { // for example: "Disk:\File.Extension\"
                return false;
            }

            // for example: "Disk:\File."
            return _Dot_pos != this->_Text.size() - 1;
        } else { 
            return false;
        }
    }

    return false;
}

// FUNCTION path::has_file
_NODISCARD bool __thiscall path::has_file() const noexcept {
    // file cannot exists without extension
    return this->has_extension();
}

// FUNCTION path::has_parent_directory
_NODISCARD bool __thiscall path::has_parent_directory() const noexcept {
    // parent directory is directory after root directory
    if (this->has_root_directory()) {
        // if false, then path contains only parent path (no drive or root directory)
        return this->_Text.size() > this->root_path().generic_string().size();
    } else { // path without drive and root directory
        return true;
    }
}

// FUNCTION path::has_root_directory
_NODISCARD bool __thiscall path::has_root_directory() const noexcept {
    // slash on first place where there's no drive means, that it's root directory
    if (!this->has_drive() && (this->_Text[0] == _Expected_slash
        || this->_Text[0] == _Unexpected_slash)) {
        return true;
    }

    // directory after drive is root directory
    if (this->has_drive() && this->_Text.size() > 3) {
        return true;
    }

    return false;
}

// FUNCTION path::has_stem
_NODISCARD bool __thiscall path::has_stem() const noexcept {
    // stem is just filename without extension
    return this->has_file();
}

// FUNCTION path::is_absolute
_NODISCARD bool __thiscall path::is_absolute() const noexcept {
    if (this->_Text.find(_Expected_slash) > this->_Text.size()
        && this->_Text.find(_Unexpected_slash) > this->_Text.size()) { // path without any slash
        return false;
    }

    // path with drive and/or root directory must be absolute
    if (this->has_drive() || this->has_root_directory()) {
        return true;
    } else { // path with some directories (no root)
        return false;
    }

    return false;
}

// FUNCTION path::is_relative
_NODISCARD bool __thiscall path::is_relative() const noexcept {
    return !this->is_absolute();
}

// FUNCTION path::make_preferred
_NODISCARD path& __thiscall path::make_preferred() noexcept {
    if (!this->empty()) {
        _STD replace(this->_Text.begin(), this->_Text.end(), _Unexpected_slash, _Expected_slash);
    }
    
    return *this;
}

// FUNCTION path::parent_directory
_NODISCARD path __thiscall path::parent_directory() const noexcept {
    if (this->has_parent_directory()) { // get only first directory from parent path
        return string_type(this->parent_path().generic_string(), 0,
            this->parent_path().generic_string().find_first_of(_Expected_slash));
    }

    return *this;
}

// FUNCTION path::parent_path
_NODISCARD path __thiscall path::parent_path() const noexcept {
    // parent path is everything after root directory
    if (this->has_parent_directory()) {
        if (this->has_drive()) { // remove drive and root directory (with ":" and 2 slashes)
            return string_type(this->_Text, this->drive().generic_string().size()
                + this->root_directory().generic_string().size() + 3, this->_Text.size());
        } else if (!this->has_drive() && this->has_root_directory()) { // remove only root directory (with 2 slashes)
            return string_type(this->_Text, this->root_directory().generic_string().size() + 2,
                this->_Text.size());
        } else { // nothing to do, because current working path is parent path
            return this->_Text;
        }
    }

    return *this;
}

// FUNCTION path::remove_extension
_NODISCARD path& __thiscall path::remove_extension() noexcept {
    if (this->has_extension()) {
        this->_Text.resize(this->_Text.find_last_of("."));
    }

    return *this;
}

// FUNCTION path::remove_file
_NODISCARD path& __cdecl path::remove_file(const bool _With_slash) noexcept {
    if (this->has_file()) {
        this->_Text.find(_Expected_slash) < this->_Text.size() ?
            this->_Text.resize(_With_slash ? this->_Text.find_last_of(_Expected_slash)
                : this->_Text.find_last_of(_Expected_slash) + 1) : this->clear();
    }

    return *this;
}

// FUNCTION path::replace_extension
_NODISCARD path& __cdecl path::replace_extension(const path& _Replacement) {
    if (this->has_extension()) {
        this->remove_extension();
        this->_Text += _Replacement.generic_string()[0] == '.' ?
            _Replacement.generic_string() : "." + _Replacement.generic_string();
        this->_Check_size();
    }

    return *this;
}

// FUNCTION path::replace_file
_NODISCARD path& __cdecl path::replace_file(const path& _Replacement) {
    if (this->has_file()) {
        this->remove_file(_Replacement.generic_string()[0] == _Expected_slash
            || _Replacement.generic_string()[0] == _Unexpected_slash);
        this->_Text += _Replacement.generic_string();
        this->_Check_size();
    }

    return *this;
}

// FUNCTION path::replace_stem
_NODISCARD path& __cdecl path::replace_stem(const path& _Replacement) {
    if (this->has_stem()) {
        const auto _Ext = this->extension();

        this->remove_file(false);
        this->_Text += _Replacement.generic_string();
        this->_Text += "." + _Ext.generic_string();
        this->_Check_size();
    }

    return *this;
}

// FUNCTION path::root_directory
_NODISCARD path __thiscall path::root_directory() const noexcept {
    if (this->has_root_directory()) { // get only first directory from root directory
        return string_type(this->root_path().generic_string(), 0,
            this->root_path().generic_string().find_first_of(_Expected_slash));
    }

    return *this;
}

// FUNCTION path::root_path
_NODISCARD path __thiscall path::root_path() const noexcept {
    if (this->has_root_directory()) {
        // if has drive, skip 3 first characters ("D:\"), otherwise skip first character (slash)
        return this->has_drive() ? string_type(this->_Text, 3, this->_Text.size())
            : string_type(this->_Text, 1, this->_Text.size());
    }
    
    return *this;
}

// FUNCTION path::stem
_NODISCARD path __thiscall path::stem() const noexcept {
    if (this->has_stem()) {
        auto _Stem = this->_Text; // don't change original text

        if (_Stem.find_last_of(_Expected_slash) < _Stem.size()) {
            _Stem.replace(0, _Stem.find_last_of(_Expected_slash) + 1, string_type()); // leave only filename with extension
            _Stem.resize(_Stem.find_last_of(".")); // remove extension
            return _Stem; // break here
        }
        
        _Stem.resize(_Stem.find_first_of(".")); // remove extension
        return _Stem;
    }

    return *this;
}

// FUNCTION current_path
_NODISCARD path __stdcall current_path() noexcept {
    wchar_t _Buff[MAX_PATH] = {};
    const bool _Result      = _CSTD GetCurrentDirectoryW(MAX_PATH, _Buff) != 0;

    if (!_Result) { // failed to get current path
        _Throw_fs_error("failed to get current path", error_type::runtime_error, "current_path");
    }

    return path(wstring(_Buff));
}

_NODISCARD bool __cdecl current_path(const path& _Path) {
    // path cannot be longer than 260 characters
    if (_Path.generic_string().size() > static_cast<size_t>(MAX_PATH)) {
        _Throw_system_error("set_path", "invalid length", error_type::length_error);
    }

    const bool _Result = _CSTD SetCurrentDirectoryW(_Path.generic_wstring().c_str()) != 0;

    if (!_Result) { // failed to set new path
        _Throw_fs_error("failed to set new path", error_type::runtime_error, "set_path");
    }

    return _Result;
}
_EXPERIMENTAL_END
_FILESYSTEM_END

#pragma warning(pop)
#endif // !_HAS_WINDOWS