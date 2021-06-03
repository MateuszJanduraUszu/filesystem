// path.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _PATH_HPP_
#define _PATH_HPP_
#include <core.hpp>

#if !_HAS_WINDOWS
#pragma message("The contents of <path.hpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv
#pragma warning(push)
#pragma warning(disable : 4996) // C4996: using deprecated content

#include <codecvt>
#include <iostream>
#include <locale>
#include <stdexcept>
#include <string>

// STD allocators
using _STD allocator;

// STD basic_stream
using _STD basic_istream;
using _STD basic_ostream;

// STD basic_string;
using _STD basic_string;
using _STD string;
using _STD u8string;
using _STD u16string;
using _STD u32string;
using _STD wstring;

// STD basic_string_view
using _STD basic_string_view;
using _STD string_view;
using _STD u8string_view;
using _STD u16string_view;
using _STD u32string_view;
using _STD wstring_view;

// STD chars
using _STD char_traits;

// STD conditions
using _STD enable_if;

// STD conversion
using _STD codecvt_utf8;
using _STD codecvt_utf8_utf16;
using _STD wstring_convert;

// STD exceptions
using _STD exception;
using _STD invalid_argument;
using _STD length_error;
using _STD runtime_error;

_FS_EXP_BEGIN
// expected slash on Windows 10
inline constexpr char _Expected_slash = '\\';
inline constexpr unsigned int _Utf8   = 65001; // default code page

// CONSTANT _Is_CharT
template<class>
inline constexpr bool _Is_CharT = false;

template<>
inline constexpr bool _Is_CharT<char> = true;
template<>
inline constexpr bool _Is_CharT<char8_t> = true;
template<>
inline constexpr bool _Is_CharT<char16_t> = true;
template<>
inline constexpr bool _Is_CharT<char32_t> = true;
template<>
inline constexpr bool _Is_CharT<wchar_t> = true;

// CLASS path
class path;

// CONSTANT _Is_Src
template<class>
inline constexpr bool _Is_Src = false;

template<>
inline constexpr bool _Is_Src<path> = false; // to avoid error in copy constructors in path

template<>
inline constexpr bool _Is_Src<string> = true;
template<>
inline constexpr bool _Is_Src<u8string> = true;
template<>
inline constexpr bool _Is_Src<u16string> = true;
template<>
inline constexpr bool _Is_Src<u32string> = true;
template<>
inline constexpr bool _Is_Src<wstring> = true;

// CONSTANT _Is_SrcView
template<class>
inline constexpr bool _Is_SrcView = false;

template<>
inline constexpr bool _Is_SrcView<string_view> = true;
template<>
inline constexpr bool _Is_SrcView<u8string_view> = true;
template<>
inline constexpr bool _Is_SrcView<u16string_view> = true;
template<>
inline constexpr bool _Is_SrcView<u32string_view> = true;
template<>
inline constexpr bool _Is_SrcView<wstring_view> = true;

#ifndef MAX_PATH 
#define MAX_PATH 260
#endif // MAX_PATH

// CLASS ENUM _Error_type
enum class _Error_type : unsigned int { // error type for _Throw_fs_error
    _Generic_error,
    _Length_error,
    _Runtime_error,
    _Invalid_argument
};

// FUNCTION _Throw_fs_error
_FS_API __declspec(noreturn) void __cdecl _Throw_fs_error(const char* const _Errm, const _Error_type _Errc);

// FUNCTION _Convert_narrow_to_wide
_FS_API _NODISCARD wstring __cdecl _Convert_narrow_to_wide(const string_view& _Input, const unsigned int _Code_page = _Utf8);

// FUNCTION _Convert_wide_to_narrow
_FS_API _NODISCARD string __cdecl _Convert_wide_to_narrow(const wstring_view& _Input, const unsigned int _Code_page = _Utf8);

// FUNCTION TEMPLATE _Convert_utf_to_narrow
template<class _Elem, class _Traits = char_traits<_Elem>>
_FS_API _NODISCARD string __cdecl _Convert_utf_to_narrow(const basic_string_view<_Elem, _Traits>& _Input);

// FUNCTION TEMPLATE _Convert_narrow_to_utf
template<class _Elem, class _Traits = char_traits<_Elem>, class _Alloc = allocator<_Elem>>
_FS_API _NODISCARD basic_string<_Elem, _Traits, _Alloc> __cdecl _Convert_narrow_to_utf(const string_view& _Input);

// CLASS path
class _FS_API path { // used in all filesystem files
private:
    template<class _Elem, class _Traits>
    friend basic_istream<_Elem, _Traits>& __cdecl operator>>(basic_istream<_Elem, _Traits>&, path&);
    template<class _Elem, class _Traits>
    friend basic_ostream<_Elem, _Traits>& __cdecl operator<<(basic_ostream<_Elem, _Traits>&, path&);

public:
    using value_type  = char;
    using string_type = string;

    __thiscall path()          = default;
    __cdecl path(const path&)  = default;
    __cdecl path(path&&)       = default;
    virtual __thiscall ~path() = default;

    template<class _Elem, class = enable_if<_Is_CharT<_Elem>, void>>
    __cdecl path(const _Elem* const _Source); // all characters types

    template<class _Src, class = enable_if<_Is_Src<_Src> || _Is_SrcView<_Src>, void>>
    __cdecl path(const _Src& _Source); // all basic_string and basic_string_view types

private:
    // checks if path size is good
    void __thiscall _Check_size() const;

public:
    path& __cdecl operator=(const path& _Source);
    path& __cdecl assign(const path& _Source);

    template<class _Elem, class = enable_if<_Is_CharT<_Elem>, void>>
    path& __cdecl operator=(const _Elem* const _Source); // all character types

    template<class _Elem, class = enable_if<_Is_CharT<_Elem>, void>>
    path& __cdecl assign(const _Elem* const _Source); // all character types

    template<class _Src, class = enable_if<_Is_Src<_Src> || _Is_SrcView<_Src>, void>>
    path& __cdecl operator=(const _Src& _Source); // all basic_string and basic_string_view types

    template<class _Src, class = enable_if<_Is_Src<_Src> || _Is_SrcView<_Src>, void>>
    path& __cdecl assign(const _Src& _Source); // all basic_string and basic_string_view types

public:
    path& __cdecl operator+=(const path& _Added);
    path& __cdecl append(const path& _Added);

    template<class _Elem, class = enable_if<_Is_CharT<_Elem>, void>>
    path& __cdecl operator+=(const _Elem* const _Added); // all character types

    template<class _Elem, class = enable_if<_Is_CharT<_Elem>, void>>
    path& __cdecl append(const _Elem* const _Added); // all character types

    template<class _Src, class = enable_if<_Is_Src<_Src> || _Is_SrcView<_Src>, void>>
    path& __cdecl operator+=(const _Src& _Added); // all basic_string and basic_string_view types

    template<class _Src, class = enable_if<_Is_Src<_Src> || _Is_SrcView<_Src>, void>>
    path& __cdecl append(const _Src& _Added); // all basic_string and basic_string_view types 

public:
    bool __cdecl operator==(const path& _Compare) const noexcept;

    template<class _Elem, class = enable_if<_Is_CharT<_Elem>, void>>
    bool __cdecl operator==(const _Elem* const _Compare) const;

    template<class _Src, class = enable_if<_Is_Src<_Src> || _Is_SrcView<_Src>, void>>
    bool __cdecl operator==(const _Src& _Compare) const;

public:
    // clears current working path
    void __thiscall clear() noexcept;

    // returns current directory
    _NODISCARD path __thiscall current_directory() noexcept;

    // returns current path
    _NODISCARD path __thiscall current_path() noexcept;

    // returns drive from current working path
    _NODISCARD path __thiscall drive() const noexcept;

    // checks if current working path is empty
    _NODISCARD bool __thiscall empty() const noexcept;

    // returns extension from current working path, if has
    _NODISCARD path __thiscall extension() const noexcept;

    // returns file name from current working path, if has
    _NODISCARD path __thiscall file() const noexcept;

    // removes unnecessary slashes from current working path
    // and converts to Windows 10 standard
    _NODISCARD path& __thiscall fix() noexcept;

    // returns current working path as string
    _NODISCARD const string __thiscall generic_string() const noexcept;

    // returns current working path as u8string
    _NODISCARD const u8string __thiscall generic_u8string() const noexcept;

    // returns current working path as u16string
    _NODISCARD const u16string __thiscall generic_u16string() const noexcept;

    // returns current working path as u32string
    _NODISCARD const u32string __thiscall generic_u32string() const noexcept;

    // returns current working path as wstring
    _NODISCARD const wstring __thiscall generic_wstring() const noexcept;

    // checks if current working path has drive
    _NODISCARD bool __thiscall has_drive() const noexcept;

    // checks if current working path has extension
    _NODISCARD bool __thiscall has_extension() const noexcept;

    // checks if current working path has file name
    _NODISCARD bool __thiscall has_file() const noexcept;

    // checks if current working path has parent directory
    _NODISCARD bool __thiscall has_parent_directory() const noexcept;

    // checks if current working path has root directory
    _NODISCARD bool __thiscall has_root_directory() const noexcept;

    // checks if current working path has stem (file name without extension)
    _NODISCARD bool __thiscall has_stem() const noexcept;

    // checks if current working path is absolute
    _NODISCARD bool __thiscall is_absolute() const noexcept;

    // checks if current working path is relative
    _NODISCARD bool __thiscall is_relative() const noexcept;

    // converts current working path to Windows 10 standard
    _NODISCARD path& __thiscall make_preferred() noexcept;

    // returns parent directory from current working path
    _NODISCARD path __thiscall parent_directory() const noexcept;

    // returns parent path from current working path
    _NODISCARD path __thiscall parent_path() const noexcept;

    // removes extension from current working path (removes slash, if _Slash is true)
    _NODISCARD path& __thiscall remove_extension() noexcept;

    // removes file name from current working path (removes slash, if _Slash is true)
    _NODISCARD path& __cdecl remove_file(const bool _With_slash) noexcept;

    // replaces extension from current working path with replacement
    _NODISCARD path& __cdecl replace_extension(const path& _Replacement);

    // replaces file name from current working path with replacement
    _NODISCARD path& __cdecl replace_file(const path& _Replacement);

    // replaces stem from current working path with replacement
    _NODISCARD path& __cdecl replace_stem(const path& _Replacement);

    // returns root directory from current working path
    _NODISCARD path __thiscall root_directory() const noexcept;

    // returns root path from current woring path
    _NODISCARD path __thiscall root_path() const noexcept;

    // returns stem from current working path (file name without extension)
    _NODISCARD path __thiscall stem() const noexcept;

    // swaps current working path to current working path from other
    void __cdecl swap(path& _Other);

private:
    string_type _Text; // current working path
};

_FS_EXP_END

#pragma warning(pop)
#endif // !_HAS_WINDOWS
#endif // _PATH_HPP_