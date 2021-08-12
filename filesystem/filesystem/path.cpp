// path.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <filesystem_pch.hpp>
#include <filesystem.hpp>

#if !_HAS_WINDOWS
#pragma message("The contents of <path.cpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv

_FILESYSTEM_BEGIN
// FUNCTION TEMPLATE operator>>
template <class _Elem, class _Traits>
_NODISCARD basic_istream<_Elem, _Traits>& operator>>(basic_istream<_Elem, _Traits>& _Stream, path& _Path) {
    basic_string<_Elem, _Traits, allocator<_Elem>> _Input;
    _Stream >> _Input;
    _Path = _STD move(_Input);
    return _Stream;
}

template _FILESYSTEM_API _NODISCARD istream& operator>>(istream&, path&);
template _FILESYSTEM_API _NODISCARD wistream& operator>>(wistream&, path&);

// FUNCTION TEMPLATE operator<<
template <class _Elem, class _Traits>
_NODISCARD basic_ostream<_Elem, _Traits>& operator<<(basic_ostream<_Elem, _Traits>& _Stream, const path& _Path) {
    // current C++ standard supports only char and wchar_t streams
    if constexpr (_STD is_same_v<_Elem, char>) {
        _Stream << _Path.generic_string();
    } else if constexpr (_STD is_same_v<_Elem, wchar_t>) {
        _Stream << _Path.generic_wstring();
    }

    return _Stream;
}

template _FILESYSTEM_API _NODISCARD ostream& operator<<(ostream&, const path&);
template _FILESYSTEM_API _NODISCARD wostream& operator<<(wostream&, const path&);

// FUNCTION TEMPLATE operator+
_NODISCARD path operator+(const path& _Left, const path& _Right) {
    return path(_Left.generic_string() + _Right.generic_string());
}

template <class _CharTy>
_NODISCARD path operator+(const path& _Left, const _CharTy* const _Right) {
    return path(_Left.generic_string() + _Convert_to_narrow<_CharTy, char_traits<_CharTy>>(_Right));
}

template _FILESYSTEM_API _NODISCARD path operator+(const path&, const char* const);
template _FILESYSTEM_API _NODISCARD path operator+(const path&, const char8_t* const);
template _FILESYSTEM_API _NODISCARD path operator+(const path&, const char16_t* const);
template _FILESYSTEM_API _NODISCARD path operator+(const path&, const char32_t* const);
template _FILESYSTEM_API _NODISCARD path operator+(const path&, const wchar_t* const);

template <class _CharTy>
_NODISCARD path operator+(const _CharTy* const _Left, const path& _Right) {
    return path(_Convert_to_narrow<_CharTy, char_traits<_CharTy>>(_Left) + _Right.generic_string());
}

template _FILESYSTEM_API _NODISCARD path operator+(const char* const, const path&);
template _FILESYSTEM_API _NODISCARD path operator+(const char8_t* const, const path&);
template _FILESYSTEM_API _NODISCARD path operator+(const char16_t* const, const path&);
template _FILESYSTEM_API _NODISCARD path operator+(const char32_t* const, const path&);
template _FILESYSTEM_API _NODISCARD path operator+(const wchar_t* const, const path&);

template <class _Elem, class _Traits, class _Alloc>
_NODISCARD path operator+(const path& _Left, const basic_string<_Elem, _Traits, _Alloc>& _Right) {
    return path(_Left.generic_string() + _Convert_to_narrow<_Elem, _Traits>(_Right.data()));
}

template _FILESYSTEM_API _NODISCARD path operator+(const path&, const string&);
template _FILESYSTEM_API _NODISCARD path operator+(const path&, const u8string&);
template _FILESYSTEM_API _NODISCARD path operator+(const path&, const u16string&);
template _FILESYSTEM_API _NODISCARD path operator+(const path&, const u32string&);
template _FILESYSTEM_API _NODISCARD path operator+(const path&, const wstring&);

template <class _Elem, class _Traits, class _Alloc>
_NODISCARD path operator+(const basic_string<_Elem, _Traits, _Alloc>& _Left, const path& _Right) {
    return path(_Convert_to_narrow<_Elem, _Traits>(_Left.data()) + _Right.generic_string());
}

template _FILESYSTEM_API _NODISCARD path operator+(const string&, const path&);
template _FILESYSTEM_API _NODISCARD path operator+(const u8string&, const path&);
template _FILESYSTEM_API _NODISCARD path operator+(const u16string&, const path&);
template _FILESYSTEM_API _NODISCARD path operator+(const u32string&, const path&);
template _FILESYSTEM_API _NODISCARD path operator+(const wstring&, const path&);

// FUNCTION TEMPLATE path::path
template <class _CharTy>
path::path(const _CharTy* const _Source) {
    // _CharTy must be an character (char/char8_t/char16_t/char32_t/wchar_t) type
    static_assert(_Is_char_t<_CharTy>, "invalid character type");
    _Mytext = _Convert_to_narrow<_CharTy, char_traits<_CharTy>>(_Source);
    _Check_size();
}

template _FILESYSTEM_API path::path(const char* const);
template _FILESYSTEM_API path::path(const char8_t* const);
template _FILESYSTEM_API path::path(const char16_t* const);
template _FILESYSTEM_API path::path(const char32_t* const);
template _FILESYSTEM_API path::path(const wchar_t* const);

template <class _Src>
path::path(const _Src& _Source) {
    // _Src must be an string (basic_string/basic_string_view) type
    static_assert(_Is_src_t<_Src>, "invalid string type");
    _Mytext = _Convert_to_narrow<typename _Src::value_type, typename _Src::traits_type>(_Source.data());
    _Check_size();
} 

template _FILESYSTEM_API path::path(const string&);
template _FILESYSTEM_API path::path(const u8string&);
template _FILESYSTEM_API path::path(const u16string&);
template _FILESYSTEM_API path::path(const u32string&);
template _FILESYSTEM_API path::path(const wstring&);

template _FILESYSTEM_API path::path(const string_view&);
template _FILESYSTEM_API path::path(const u8string_view&);
template _FILESYSTEM_API path::path(const u16string_view&);
template _FILESYSTEM_API path::path(const u32string_view&);
template _FILESYSTEM_API path::path(const wstring_view&);

// FUNCTION path::_Check_size
constexpr void path::_Check_size() const {
    // path cannot be longer than 260 characters
    if (_Mytext.size() > static_cast<size_t>(_MAX_PATH)) {
        _Throw_system_error("_Check_size", "invalid length", error_type::length_error);
    }
}

// FUNCTION path::operator=
path& path::operator=(const path& _Source) {
    if (this != __builtin_addressof(_Source)) { // avoid assigning own value
        _Mytext = _Source._Mytext;
        _Check_size();
    }
    
    return *this;
}

template <class _CharTy>
path& path::operator=(const _CharTy* const _Source) {
    // _CharTy must be an character (char/char8_t/char16_t/char32_t/wchar_t) type
    static_assert(_Is_char_t<_CharTy>, "invalid character type");
    _Mytext = _Convert_to_narrow<_CharTy, char_traits<_CharTy>>(_Source);
    _Check_size();
    return *this;
}

template _FILESYSTEM_API path& path::operator=(const char* const);
template _FILESYSTEM_API path& path::operator=(const char8_t* const);
template _FILESYSTEM_API path& path::operator=(const char16_t* const);
template _FILESYSTEM_API path& path::operator=(const char32_t* const);
template _FILESYSTEM_API path& path::operator=(const wchar_t* const);

template<class _Src>
path& path::operator=(const _Src& _Source) {
    // _Src must be an string (basic_string/basic_string_view) type
    static_assert(_Is_src_t<_Src>, "invalid string type");
    _Mytext = _Convert_to_narrow<typename _Src::value_type, typename _Src::traits_type>(_Source.data());
    _Check_size();
    return *this;
}

template _FILESYSTEM_API path& path::operator=(const string&);
template _FILESYSTEM_API path& path::operator=(const u8string&);
template _FILESYSTEM_API path& path::operator=(const u16string&);
template _FILESYSTEM_API path& path::operator=(const u32string&);
template _FILESYSTEM_API path& path::operator=(const wstring&);

template _FILESYSTEM_API path& path::operator=(const string_view&);
template _FILESYSTEM_API path& path::operator=(const u8string_view&);
template _FILESYSTEM_API path& path::operator=(const u16string_view&);
template _FILESYSTEM_API path& path::operator=(const u32string_view&);
template _FILESYSTEM_API path& path::operator=(const wstring_view&);

// FUNCTION path::assign
path& path::assign(const path& _Source) {
    return operator=(_Source);
}

template <class _CharTy>
path& path::assign(const _CharTy* const _Source) {
    // don't check _CharTy type, because operator=() will check it
    return operator=(_Source);
}

template _FILESYSTEM_API path& path::assign(const char* const);
template _FILESYSTEM_API path& path::assign(const char8_t* const);
template _FILESYSTEM_API path& path::assign(const char16_t* const);
template _FILESYSTEM_API path& path::assign(const char32_t* const);
template _FILESYSTEM_API path& path::assign(const wchar_t* const);

template <class _Src>
path& path::assign(const _Src& _Source) {
    // don't check _Src type, because operator=() will check it
    return operator=(_Source);
}

template _FILESYSTEM_API path& path::assign(const string&);
template _FILESYSTEM_API path& path::assign(const u8string&);
template _FILESYSTEM_API path& path::assign(const u16string&);
template _FILESYSTEM_API path& path::assign(const u32string&);
template _FILESYSTEM_API path& path::assign(const wstring&);

template _FILESYSTEM_API path& path::assign(const string_view&);
template _FILESYSTEM_API path& path::assign(const u8string_view&);
template _FILESYSTEM_API path& path::assign(const u16string_view&);
template _FILESYSTEM_API path& path::assign(const u32string_view&);
template _FILESYSTEM_API path& path::assign(const wstring_view&);

// FUNCTION path::operator+=
path& path::operator+=(const path& _Added) {
    if (this != __builtin_addressof(_Added)) { // avoid appending own value
        _Mytext += _Added._Mytext;
        _Check_size();
    }

    return *this;
}

template <class _CharTy>
path& path::operator+=(const _CharTy* const _Added) {
    // _CharTy must be an character (char/char8_t/char16_t/char32_t/wchar_t) type
    static_assert(_Is_char_t<_CharTy>, "invalid character type");
    _Mytext += _Convert_to_narrow<_CharTy, char_traits<_CharTy>>(_Added);
    _Check_size();
    return *this;
}

template _FILESYSTEM_API path& path::operator+=(const char* const);
template _FILESYSTEM_API path& path::operator+=(const char8_t* const);
template _FILESYSTEM_API path& path::operator+=(const char16_t* const);
template _FILESYSTEM_API path& path::operator+=(const char32_t* const);
template _FILESYSTEM_API path& path::operator+=(const wchar_t* const);

template <class _Src>
path& path::operator+=(const _Src& _Added) {
    // _Src must be an string (basic_string/basic_string_view) type
    static_assert(_Is_src_t<_Src>, "invalid string type");
    _Mytext += _Convert_to_narrow<typename _Src::value_type, typename _Src::traits_type>(_Added.data());
    _Check_size();
    return *this;
}

template _FILESYSTEM_API path& path::operator+=(const string&);
template _FILESYSTEM_API path& path::operator+=(const u8string&);
template _FILESYSTEM_API path& path::operator+=(const u16string&);
template _FILESYSTEM_API path& path::operator+=(const u32string&);
template _FILESYSTEM_API path& path::operator+=(const wstring&);

template _FILESYSTEM_API path& path::operator+=(const string_view&);
template _FILESYSTEM_API path& path::operator+=(const u8string_view&);
template _FILESYSTEM_API path& path::operator+=(const u16string_view&);
template _FILESYSTEM_API path& path::operator+=(const u32string_view&);
template _FILESYSTEM_API path& path::operator+=(const wstring_view&);

// FUNCTION path::append
path& path::append(const path& _Added) {
    return operator+=(_Added);
}

template <class _CharTy>
path& path::append(const _CharTy* const _Added) {
    // don't check _CharTy type, because operator+=() will check it
    return operator+=(_Added);
}

template _FILESYSTEM_API path& path::append(const char* const);
template _FILESYSTEM_API path& path::append(const char8_t* const);
template _FILESYSTEM_API path& path::append(const char16_t* const);
template _FILESYSTEM_API path& path::append(const char32_t* const);
template _FILESYSTEM_API path& path::append(const wchar_t* const);

template <class _Src>
path& path::append(const _Src& _Added) {
    // don't check _Src type, because operator+=() will check it
    return operator+=(_Added);
}

template _FILESYSTEM_API path& path::append(const string&);
template _FILESYSTEM_API path& path::append(const u8string&);
template _FILESYSTEM_API path& path::append(const u16string&);
template _FILESYSTEM_API path& path::append(const u32string&);
template _FILESYSTEM_API path& path::append(const wstring&);

template _FILESYSTEM_API path& path::append(const string_view&);
template _FILESYSTEM_API path& path::append(const u8string_view&);
template _FILESYSTEM_API path& path::append(const u16string_view&);
template _FILESYSTEM_API path& path::append(const u32string_view&);
template _FILESYSTEM_API path& path::append(const wstring_view&);

// FUNCTION path::operator==
bool path::operator==(const path& _Compare) const noexcept {
    // avoid comparing with own value
    return this != __builtin_addressof(_Compare) ? _Mytext == _Compare._Mytext : true;
}

template <class _CharTy>
bool path::operator==(const _CharTy* const _Compare) const {
    // _CharTy must be an chararcter (char/char8_t/char16_t/char32_t/wchar_t) type
    static_assert(_Is_char_t<_CharTy>, "invalid character type");
    
    return _Mytext == _Convert_to_narrow<_CharTy, char_traits<_CharTy>>(_Compare);
}

template _FILESYSTEM_API bool path::operator==(const char* const) const;
template _FILESYSTEM_API bool path::operator==(const char8_t* const) const;
template _FILESYSTEM_API bool path::operator==(const char16_t* const) const;
template _FILESYSTEM_API bool path::operator==(const char32_t* const) const;
template _FILESYSTEM_API bool path::operator==(const wchar_t* const) const;

template <class _Src>
bool path::operator==(const _Src& _Compare) const {
    // _Src must be an string (basic_string/basic_string_view) type
    static_assert(_Is_src_t<_Src>, "invalid string type");
    
    return _Mytext == _Convert_to_narrow<typename _Src::value_type, typename _Src::traits_type>(_Compare.data());
}

template _FILESYSTEM_API bool path::operator==(const string&) const;
template _FILESYSTEM_API bool path::operator==(const u8string&) const;
template _FILESYSTEM_API bool path::operator==(const u16string&) const;
template _FILESYSTEM_API bool path::operator==(const u32string&) const;
template _FILESYSTEM_API bool path::operator==(const wstring&) const;

template _FILESYSTEM_API bool path::operator==(const string_view&) const;
template _FILESYSTEM_API bool path::operator==(const u8string_view&) const;
template _FILESYSTEM_API bool path::operator==(const u16string_view&) const;
template _FILESYSTEM_API bool path::operator==(const u32string_view&) const;
template _FILESYSTEM_API bool path::operator==(const wstring_view&) const;

// FUNCTION path::operator!=
bool path::operator!=(const path& _Compare) const noexcept {
    // avoid comparing with own value
    return this != __builtin_addressof(_Compare) ? _Mytext != _Compare._Mytext : false;
}

template <class _CharTy>
bool path::operator!=(const _CharTy* const _Compare) const {
    // _CharTy must be an chararcter (char/char8_t/char16_t/char32_t/wchar_t) type
    static_assert(_Is_char_t<_CharTy>, "invalid character type");
    
    return _Mytext != _Convert_to_narrow<_CharTy, char_traits<_CharTy>>(_Compare);
}

template _FILESYSTEM_API bool path::operator!=(const char* const) const;
template _FILESYSTEM_API bool path::operator!=(const char8_t* const) const;
template _FILESYSTEM_API bool path::operator!=(const char16_t* const) const;
template _FILESYSTEM_API bool path::operator!=(const char32_t* const) const;
template _FILESYSTEM_API bool path::operator!=(const wchar_t* const) const;

template <class _Src>
bool path::operator!=(const _Src& _Compare) const {
    // _Src must be an string (basic_string/basic_string_view) type
    static_assert(_Is_src_t<_Src>, "invalid string type");
    
    return _Mytext != _Convert_to_narrow<typename _Src::value_type, typename _Src::traits_type>(_Compare.data());
}

template _FILESYSTEM_API bool path::operator!=(const string&) const;
template _FILESYSTEM_API bool path::operator!=(const u8string&) const;
template _FILESYSTEM_API bool path::operator!=(const u16string&) const;
template _FILESYSTEM_API bool path::operator!=(const u32string&) const;
template _FILESYSTEM_API bool path::operator!=(const wstring&) const;

template _FILESYSTEM_API bool path::operator!=(const string_view&) const;
template _FILESYSTEM_API bool path::operator!=(const u8string_view&) const;
template _FILESYSTEM_API bool path::operator!=(const u16string_view&) const;
template _FILESYSTEM_API bool path::operator!=(const u32string_view&) const;
template _FILESYSTEM_API bool path::operator!=(const wstring_view&) const;

// FUNCTION path::clear
constexpr void path::clear() noexcept {
    _Mytext.clear();
}

// same as string[_view]::npos
#ifndef _NPOS
#define _NPOS static_cast<size_t>(-1)
#endif // _NPOS

// FUNCTION path::directory
_NODISCARD path path::directory() const noexcept {
    if (has_directory()) {
        const size_t _Pos{_Mytext.find_last_of(_Expected_slash)};
        if (_Pos == _NPOS) { // the path contain only one directory
            return _Mytext;
        }

        if (_Pos == _Mytext.size() - 1) { // slash on the last position
            return R"(\)";
        }

        return string_type(_Mytext, _Pos + 1, _Mytext.size() - 1);
    }

    return string_type();
}

// FUNCTION path::drive
_NODISCARD path path::drive() const noexcept {
    // if has drive then first letter is drive
    return has_drive() ? string_type(1, _Mytext[0]) : string_type();
}

// FUNCTION path::empty
_NODISCARD constexpr bool path::empty() const noexcept {
    return _Mytext.empty();
}

// FUNCTION path::extension
_NODISCARD path path::extension() const noexcept {
    // if has extension, then everything after last dot is extension
    return has_extension() ? string_type(_Mytext, _Mytext.find_last_of(".") + 1, _Mytext.size()) : string_type();
}

// FUNCTION path::file
_NODISCARD path path::file() const noexcept {
    if (has_file()) { // if has file, then everything after last slash is filename
        return _Mytext.find(_Expected_slash) != _NPOS ?
            string_type(_Mytext, _Mytext.find_last_of(_Expected_slash) + 1, _Mytext.size()) : string_type(_Mytext);
    }

    return string_type();
}

// FUNCTION path::fix
_NODISCARD path& path::fix() noexcept {
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
_NODISCARD const string path::generic_string() const noexcept {
    return _Mytext;
}

// FUNCTION path::generic_u8string
_NODISCARD const u8string path::generic_u8string() const {
    return _Convert_narrow_to_utf<char8_t, char_traits<char8_t>>(_Mytext.data());
}

// FUNCTION path::generic_u16string
_NODISCARD const u16string path::generic_u16string() const {
    return _Convert_narrow_to_utf<char16_t, char_traits<char16_t>>(_Mytext.data());
}

// FUNCTION path::generic_u32string
_NODISCARD const u32string path::generic_u32string() const {
    return _Convert_narrow_to_utf<char32_t, char_traits<char32_t>>(_Mytext.data());
}

// FUNCTION path::generic_wstring
_NODISCARD const wstring path::generic_wstring() const {
    return _Convert_narrow_to_wide(code_page::utf8, _Mytext.data());
}

// FUNCTION path::has_directory
_NODISCARD bool path::has_directory() const noexcept {
    if (empty() || has_file()) { // the directory must on the last position
        return false;
    } else { // if has the drive, then must be longer than 3 characters ("D:\")
        return has_drive() ? _Mytext.size() > 3 : true;
    }
}

// FUNCTION path::has_drive
_NODISCARD bool path::has_drive() const noexcept {
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

// FUNCTION path::has_extension
_NODISCARD bool path::has_extension() const noexcept {
    if (_Mytext.find(".") != _NPOS) {
        const size_t _Dot_pos{_Mytext.find_last_of(".")};

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
_NODISCARD bool path::has_file() const noexcept {
    // file cannot exists without extension
    return has_extension();
}

// FUNCTION path::has_parent_directory
_NODISCARD bool path::has_parent_directory() const noexcept {
    // if has root directory, then must be longer than root path, otherwise all path is parent path
    return has_root_directory() ? _Mytext.size() > root_path()._Mytext.size() : true;
}

// FUNCTION path::has_root_directory
_NODISCARD bool path::has_root_directory() const noexcept {
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
_NODISCARD bool path::has_stem() const noexcept {
    // stem is just filename without extension
    return has_file();
}

// FUNCTION path::is_absolute
_NODISCARD bool path::is_absolute() const noexcept {
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
_NODISCARD bool path::is_relative() const noexcept {
    return !is_absolute();
}

// FUNCTION path::make_preferred
_NODISCARD path& path::make_preferred() noexcept {
    if (!empty()) {
        for (auto& _Ch : _Mytext) {
            if (_Ch == _Unexpected_slash) {
                _Ch = _Expected_slash;
            }
        }
    }
    
    return *this;
}

// FUNCTION path::parent_directory
_NODISCARD path path::parent_directory() const noexcept {
    // if has parent directory, then return only first directory from parent path
    return has_parent_directory() ? string_type(parent_path()._Mytext, 0,
        parent_path()._Mytext.find_first_of(_Expected_slash)) : string_type();
}

// FUNCTION path::parent_path
_NODISCARD path path::parent_path() const noexcept {
    if (has_parent_directory()) { // parent path is everything after root directory
        if (has_drive()) { // remove drive and root directory (with ":" and 2 slashes)
            return string_type(_Mytext, drive()._Mytext.size()
                + root_directory()._Mytext.size() + 3, _Mytext.size());
        } else if (!has_drive() && has_root_directory()) { // remove only root directory (with 2 slashes)
            return string_type(_Mytext, root_directory()._Mytext.size() + 2, _Mytext.size());
        } else { // nothing to do, because current working path is parent path
            return _Mytext;
        }
    }

    return string_type();
}

// FUNCTION path::remove_directory
_NODISCARD path& path::remove_directory(const bool _With_slash) noexcept {
    if (has_directory()) {
        if (_Mytext.find(_Expected_slash) == _NPOS) {
            _Mytext.clear();
        } else {
            _Mytext.resize(_Mytext.find_last_of(_Expected_slash) + (_With_slash ? 0 : 1));
        }
    }

    return *this;
}

// FUNCTION path::remove_extension
_NODISCARD path& path::remove_extension() noexcept {
    if (has_extension()) {
        _Mytext.resize(_Mytext.find_last_of("."));
    }

    return *this;
}

// FUNCTION path::remove_file
_NODISCARD path& path::remove_file(const bool _With_slash) noexcept {
    if (has_file()) {
        // if path contains only filename, then clear it
        _Mytext.find(_Expected_slash) != _NPOS ?
            _Mytext.resize(_With_slash ? _Mytext.find_last_of(_Expected_slash)
                : _Mytext.find_last_of(_Expected_slash) + 1) : clear();
    }

    return *this;
}

// FUNCTION path::replace_directory
_NODISCARD path& path::replace_directory(const path& _Replacement) {
    if (has_directory()) {
        (void) remove_directory(false);
        _Mytext += _Replacement._Mytext;
        _Check_size();
    }

    return *this;
}

// FUNCTION path::replace_extension
_NODISCARD path& path::replace_extension(const path& _Replacement) {
    if (has_extension()) {
        (void) remove_extension();
        _Mytext += _Replacement._Mytext[0] == '.' ?
            _Replacement._Mytext : "." + _Replacement._Mytext;
        _Check_size();
    }

    return *this;
}

// FUNCTION path::replace_file
_NODISCARD path& path::replace_file(const path& _Replacement) {
    if (has_file()) {
        (void) remove_file(_Replacement._Mytext[0] == _Expected_slash
            || _Replacement._Mytext[0] == _Unexpected_slash);
        _Mytext += _Replacement._Mytext;
        _Check_size();
    }

    return *this;
}

// FUNCTION path::replace_stem
_NODISCARD path& path::replace_stem(const path& _Replacement) {
    if (has_stem()) {
        const auto& _Ext{extension()};
        (void) remove_file(false);
        _Mytext += _Replacement._Mytext;
        _Mytext += "." + _Ext._Mytext;
        _Check_size();
    }

    return *this;
}

// FUNCTION path::resize
constexpr void path::resize(const size_t _Newsize, const value_type _Ch) {
    _Mytext.resize(_Newsize, _Ch);
    _Check_size();
}

// FUNCTION path::root_directory
_NODISCARD path path::root_directory() const noexcept {
    // if has root directory, then return only first directory from root path
    return has_root_directory() ? string_type(root_path()._Mytext, 0,
        root_path()._Mytext.find_first_of(_Expected_slash)) : string_type();
}

// FUNCTION path::root_path
_NODISCARD path path::root_path() const noexcept {
    // if has root directory, then return everything after first slash
    return has_root_directory() ? string_type(_Mytext, _Mytext.find_first_of(_Expected_slash), _Mytext.size()) : string_type();
}

// FUNCTION path::size
_NODISCARD constexpr size_t path::size() const noexcept {
    return _Mytext.size();
}

// FUNCTION path::stem
_NODISCARD path path::stem() const noexcept {
    if (has_stem()) {
        auto _Stem{_Mytext}; // don't change original text
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
_NODISCARD path current_path() noexcept {
    wchar_t _Buff[_MAX_PATH];
    if (!GetModuleFileNameW(nullptr, _Buff, _MAX_PATH)) { // failed to get current path
        _Throw_fs_error("failed to get current path", error_type::runtime_error, "current_path");
    }
    
    path _Path{static_cast<const wchar_t*>(_Buff)};
    (void) _Path.remove_file(true);
    return _Path;
}

_NODISCARD bool current_path(const path& _Path) { // sets new current path
    // path cannot be longer than 260 characters
    if (_Path.generic_string().size() > static_cast<size_t>(_MAX_PATH)) {
        _Throw_system_error("set_path", "invalid length", error_type::length_error);
    }

    if (!SetCurrentDirectoryW(_Path.generic_wstring().c_str())) { // failed to set new path
        _Throw_fs_error("failed to set new path", error_type::runtime_error, "set_path");
    }

    return true;
}

// FUNCTION temp_directory_path
_NODISCARD path temp_directory_path() {
    wchar_t _Buff[_MAX_PATH];
    const unsigned long _Size{GetTempPathW(_MAX_PATH, _Buff)};

    _FILESYSTEM_VERIFY(_Size > 0, "failed to get temporary path", error_type::runtime_error);
    path _Tmp{static_cast<const wchar_t*>(_Buff)};
    if (_Tmp.generic_string().back() == _Expected_slash) { // unnecessary slash on last position
        _Tmp.resize(_Tmp.size() - 1);
    
        if (!exists(_Tmp)) { // sometimes slash on last position can be important, we should check it
            _Tmp += R"(\)";

            if (!exists(_Tmp)) { // not found temp directory
                _Throw_fs_error("temporary directory path not found", error_type::runtime_error, "temp_directory_path");
            }

            // if won't throw an exception, return will contain slash on last position
        }
    }

    _FILESYSTEM_VERIFY(_Is_directory(_Tmp), "temporary directory path not found", error_type::runtime_error);
    return _Tmp;
}

#pragma warning(push)
#pragma warning(disable : 4455) // C4455: reserved name
namespace path_literals {
    // FUNCTION operator""p
    _NODISCARD path operator""p(const char* const _Str, const size_t _Size) noexcept {
        return path(_Str);
    }

    _NODISCARD path operator""p(const char8_t* const _Str, const size_t _Size) noexcept {
        return path(_Str);
    }

    _NODISCARD path operator""p(const char16_t* const _Str, const size_t _Size) noexcept {
        return path(_Str);
    }

    _NODISCARD path operator""p(const char32_t* const _Str, const size_t _Size) noexcept {
        return path(_Str);
    }

    _NODISCARD path operator""p(const wchar_t* const _Str, const size_t _Size) noexcept {
        return path(_Str);
    }
} // path_literals
#pragma warning(pop)
_FILESYSTEM_END

#endif // !_HAS_WINDOWS