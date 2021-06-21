// filesystem.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _FILESYSTEM_HPP_
#define _FILESYSTEM_HPP_

#ifndef __cplusplus
#error The contents of <filesystem.hpp> are available only with C++.
#endif // __cplusplus

#ifdef _WIN32
#define _HAS_WINDOWS 1
#else // ^^^ _WIN32 ^^^ / vvv !_WIN32 vvv
#define _HAS_WINDOWS 0
#endif // _WIN32

// deprecated content
#ifndef _DEPRECATED
#define _DEPRECATED(_Depr) __declspec(deprecated("The " #_Depr " will be removed soon. Try to avoid using it."))
#endif // _DEPRECATED

#ifndef _DEPRECATED_WITH_REPLACEMENT
#define _DEPRECATED_WITH_REPLACEMENT(_Depr, _Repl) __declspec(deprecated("The " #_Depr " will be removed soon. " \
    "Use " #_Repl " instead of it."))
#endif // _DEPRECATED_WITH_REPLACEMENT

// filesystem api
#ifndef _FILESYSTEM_API
#ifdef _FILESYSTEM_EXPORT
#define _FILESYSTEM_API __declspec(dllexport)
#else // ^^^ _FILESYSTEM_EXPORT ^^^ / vvv _FILESYSTEM_EXPORT vvv
#define _FILESYSTEM_API __declspec(dllimport)
#endif // _FILESYSTEM_EXPORT
#endif // _FILESYSTEM_API

// attributes
#ifndef _NODISCARD
#define _NODISCARD [[nodiscard]]
#endif // _NODISCARD

// exceptions
#ifndef _THROW
#define _THROW(_Errm) throw _Errm
#endif // _THROW

#ifndef _TRY_BEGIN
#define _TRY_BEGIN try {
#endif // _TRY_BEGIN

#ifndef _CATCH
#define _CATCH(_Exc) } catch (_Exc) {
#endif // _CATCH

#ifndef _CATCH_ALL
#define _CATCH_ALL } catch (...) {
#endif // _CATCH_ALL

#ifndef _CATCH_END
#define _CATCH_END }
#endif // _CATCH_END

// namespace
#ifndef _FILESYSTEM_BEGIN
#define _FILESYSTEM_BEGIN namespace filesystem {
#endif // _FILESYSTEM_BEGIN

#ifndef _FILESYSTEM_END
#define _FILESYSTEM_END }
#endif // _FILESYSTEM_END

#ifndef _EXPERIMENTAL_BEGIN
#define _EXPERIMENTAL_BEGIN namespace experimental {
#endif // _EXPERIMENTAL_BEGIN

#ifndef _EXPERIMENTAL_END
#define _EXPERIMENTAL_END }
#endif // _EXPERIMENTAL_END

#ifndef _FILESYSTEM
#define _FILESYSTEM ::filesystem::
#endif // _FILESYSTEM

#ifndef _FILESYSTEM_EXPERIMENTAL
#define _FILESYSTEM_EXPERIMENTAL ::filesystem::experimental::
#endif // _FILESYSTEM_EXPERIMENTAL

// operators
#ifndef _BITOPS
#define _BITOPS(_Bitsrc)                                                                             \
_NODISCARD constexpr _Bitsrc __cdecl operator&(const _Bitsrc _Left, const _Bitsrc _Right) noexcept { \
    using _IntT = _STD underlying_type_t<_Bitsrc>;                                                   \
    return static_cast<_Bitsrc>(static_cast<_IntT>(_Left) & static_cast<_IntT>(_Right));             \
}                                                                                                    \
                                                                                                     \
_NODISCARD constexpr _Bitsrc __cdecl operator|(const _Bitsrc _Left, const _Bitsrc _Right) noexcept { \
    using _IntT = _STD underlying_type_t<_Bitsrc>;                                                   \
    return static_cast<_Bitsrc>(static_cast<_IntT>(_Left) | static_cast<_IntT>(_Right));             \
}                                                                                                    \
                                                                                                     \
_NODISCARD constexpr _Bitsrc __cdecl operator^(const _Bitsrc _Left, const _Bitsrc _Right) noexcept { \
    using _IntT = _STD underlying_type_t<_Bitsrc>;                                                   \
    return static_cast<_Bitsrc>(static_cast<_IntT>(_Left) ^ static_cast<_IntT>(_Right));             \
}                                                                                                    \
                                                                                                     \
_NODISCARD constexpr _Bitsrc& __cdecl operator^=(_Bitsrc& _Left, const _Bitsrc _Right) noexcept {    \
    _Left = _Left ^ _Right;                                                                          \
    return _Left;                                                                                    \
}
#endif // _BITOPS

#if !_HAS_WINDOWS
#pragma message("The contents of <filesystem.hpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv
#pragma warning(push)
#pragma warning(disable : 4996) // C4996: using deprecated content
#pragma warning(disable : 4251) // C4251: requires dll library

#include <algorithm>
#include <array>
#include <codecvt>
#include <fstream>
#include <iostream>
#include <locale>
#include <shellapi.h>
#pragma comment(lib, "Shell32.lib")
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <stdexcept>
#include <string>
#include <vector>
#include <Windows.h>
#include <winioctl.h>

// STD allocators
using _STD allocator;
using _STD array;
using _STD vector;

// STD characters
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
using _STD system_error;

// STD file streams
using _STD fstream;
using _STD ifstream;
using _STD ofstream;

// STD streams
using _STD basic_istream;
using _STD basic_ostream;
using _STD ios;
using _STD istream;
using _STD ostream;
using _STD wistream;
using _STD wostream;

// STD strings
using _STD basic_string;
using _STD string;
using _STD u8string;
using _STD u16string;
using _STD u32string;
using _STD wstring;

// STD strings for view
using _STD basic_string_view;
using _STD string_view;
using _STD u8string_view;
using _STD u16string_view;
using _STD u32string_view;
using _STD wstring_view;

_FILESYSTEM_BEGIN
// expected slash on Windows 10
inline constexpr char _Expected_slash   = '\\';
inline constexpr char _Unexpected_slash = '/'; // default on Linux

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
inline constexpr bool _Is_Src<path> = false; // to avoid errors in copy constructors, in path

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
inline constexpr bool _Is_SrcView<path> = false; // to avoid errors in copy constructors, in path

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

// ENUM CLASS error_type
enum class error_type : unsigned int { // error type for _Throw_system_error
    invalid_argument,
    length_error,
    runtime_error
};

// ENUM CLASS code_page
enum class code_page {
    acp  = 0, // ansi code page
    utf8 = 65001 // default code page
};

// FUNCTION _Throw_system_error
_FILESYSTEM_API __declspec(noreturn) void __cdecl _Throw_system_error(const char* const _Errpos, const char* const _Errm, const error_type _Errc);

// FUNCTION _Convert_narrow_to_wide
_FILESYSTEM_API _NODISCARD wstring __cdecl _Convert_narrow_to_wide(const code_page _Code_page, const string_view _Input);

// FUNCTION _Convert_wide_to_narrow
_FILESYSTEM_API _NODISCARD string __cdecl _Convert_wide_to_narrow(const code_page _Code_page, const wstring_view _Input);

// FUNCTION TEMPLATE _Convert_utf_to_wide
template<class _Elem, class _Traits = char_traits<_Elem>>
_FILESYSTEM_API _NODISCARD string __cdecl _Convert_utf_to_narrow(const basic_string_view<_Elem, _Traits> _Input);

// FUNCTION TEMPLATE _Convert_narrow_to_utf
template<class _Elem, class _Traits = char_traits<_Elem>, class _Alloc = allocator<_Elem>>
_FILESYSTEM_API _NODISCARD basic_string<_Elem, _Traits, _Alloc> __cdecl _Convert_narrow_to_utf(const string_view _Input);

// FUNCTION TEMPLATE operator+
// enables path concatenation with C/C++ strings
_FILESYSTEM_API _NODISCARD path __cdecl operator+(const path& _Left, const path& _Right);
template<class _CharT>
_FILESYSTEM_API _NODISCARD path __cdecl operator+(const path& _Left, const _CharT* const _Right);
template<class _CharT>
_FILESYSTEM_API _NODISCARD path __cdecl operator+(const _CharT* const _Left, const path& _Right);
template<class _Elem, class _Traits = char_traits<_Elem>, class _Alloc = allocator<_Elem>>
_FILESYSTEM_API _NODISCARD path __cdecl operator+(const path& _Left, const basic_string<_Elem, _Traits, _Alloc>& _Right);
template<class _Elem, class _Traits = char_traits<_Elem>, class _Alloc = allocator<_Elem>>
_FILESYSTEM_API _NODISCARD path __cdecl operator+(const basic_string<_Elem, _Traits, _Alloc>& _Left, const path& _Right);

// CLASS path
class _FILESYSTEM_API path {
private:
    template<class _Elem, class _Traits>
    friend basic_istream<_Elem, _Traits>& __cdecl operator>>(basic_istream<_Elem, _Traits>&, path&);
    template<class _Elem, class _Traits>
    friend basic_ostream<_Elem, _Traits>& __cdecl operator<<(basic_ostream<_Elem, _Traits>&, const path&);
    friend path __cdecl operator+(const path&, const path&);
    template<class _CharT>
    friend path __cdecl operator+(const path&, const _CharT* const);
    template<class _CharT>
    friend path __cdecl operator+(const _CharT* const, const path&);
    template<class _Elem, class _Traits, class _Alloc>
    friend path __cdecl operator+(const path&, const basic_string<_Elem, _Traits, _Alloc>&);
    template<class _Elem, class _Traits, class _Alloc>
    friend path __cdecl operator+(const basic_string<_Elem, _Traits, _Alloc>&, const path&);

public:
    using value_type  = char;
    using string_type = string;

    __thiscall path()          = default;
    __cdecl path(const path&)  = default;
    __cdecl path(path&&)       = default;
    virtual __thiscall ~path() = default;

    template<class _CharT, class = enable_if<_Is_CharT<_CharT>, void>>
    __cdecl path(const _CharT* const _Source); // all character types

    template<class _Src, class = enable_if<_Is_Src<_Src> || _Is_SrcView<_Src>, void>>
    __cdecl path(const _Src& _Source); // all string types

private:
    void __thiscall _Check_size() const; // verifies path size

public:
    path& __cdecl operator=(const path& _Source);
    path& __cdecl assign(const path& _Source);

    template<class _CharT, class = enable_if<_Is_CharT<_CharT>, void>>
    path& __cdecl operator=(const _CharT* const _Source); // all character types

    template<class _CharT, class = enable_if<_Is_CharT<_CharT>, void>>
    path& __cdecl assign(const _CharT* const _Source); // all character types

    template<class _Src, class = enable_if<_Is_Src<_Src> || _Is_SrcView<_Src>, void>>
    path& __cdecl operator=(const _Src& _Source); // all string types

    template<class _Src, class = enable_if<_Is_Src<_Src> || _Is_SrcView<_Src>, void>>
    path& __cdecl assign(const _Src& _Source); // all string types

public:
    path& __cdecl operator+=(const path& _Added);
    path& __cdecl append(const path& _Added);

    template<class _CharT, class = enable_if<_Is_CharT<_CharT>, void>>
    path& __cdecl operator+=(const _CharT* const _Added); // all character types

    template<class _CharT, class = enable_if<_Is_CharT<_CharT>, void>>
    path& __cdecl append(const _CharT* const _Added); // all character types

    template<class _Src, class = enable_if<_Is_Src<_Src> || _Is_SrcView<_Src>, void>>
    path& __cdecl operator+=(const _Src& _Added); // all string types

    template<class _Src, class = enable_if<_Is_Src<_Src> || _Is_SrcView<_Src>, void>>
    path& __cdecl append(const _Src& _Added); // all string types

public:
    bool __cdecl operator==(const path& _Compare) const noexcept;

    template<class _CharT, class = enable_if<_Is_CharT<_CharT>, void>>
    bool __cdecl operator==(const _CharT* const _Compare) const;

    template<class _Src, class = enable_if<_Is_Src<_Src> || _Is_SrcView<_Src>, void>>
    bool __cdecl operator==(const _Src& _Compare) const;

public:
    bool __cdecl operator!=(const path& _Compare) const noexcept;

    template<class _CharT, class = enable_if<_Is_CharT<_CharT>, void>>
    bool __cdecl operator!=(const _CharT* const _Compare) const;

    template<class _Src, class = enable_if<_Is_Src<_Src> || _Is_SrcView<_Src>, void>>
    bool __cdecl operator!=(const _Src& _Compare) const;

public:
    // clears current working path
    void __thiscall clear() noexcept;

    // returns drive from current working path (if has)
    _NODISCARD path __thiscall drive() const noexcept;

    // checks if current working path is empty
    _NODISCARD bool __thiscall empty() const noexcept;

    // returns extension from current working path (if has)
    _NODISCARD path __thiscall extension() const noexcept;

    // returns filename from current working path (if has)
    _NODISCARD path __thiscall file() const noexcept;

    // removes unnecessary slashes from current working path
    // and converts to Windows 10 standard
    _NODISCARD path& __thiscall fix() noexcept;

    // returns current working path as string
    _NODISCARD const string __thiscall generic_string() const noexcept;

    // returns current working path as u8string
    _NODISCARD const u8string __thiscall generic_u8string() const;

    // returns current working path as u16string
    _NODISCARD const u16string __thiscall generic_u16string() const;

    // returns current working path as u32string
    _NODISCARD const u32string __thiscall generic_u32string() const;

    // returns current working path as wstring
    _NODISCARD const wstring __thiscall generic_wstring() const;

    // checks if current working path has drive
    _NODISCARD bool __thiscall has_drive() const noexcept;

    // checks if current working path has extension
    _NODISCARD bool __thiscall has_extension() const noexcept;
    
    // checks if current working path has file
    _NODISCARD bool __thiscall has_file() const noexcept;

    // checks if current working path has parent directory
    _NODISCARD bool __thiscall has_parent_directory() const noexcept;

    // checks if current working path has root directory
    _NODISCARD bool __thiscall has_root_directory() const noexcept;

    // checks if current working path has stem (filename without extension)
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

    // removes extension from current working path (if has)
    _NODISCARD path& __thiscall remove_extension() noexcept;

    // removes filename from current working path (if has) (removes slash, if has and _With_slash is true)
    _NODISCARD path& __cdecl remove_file(const bool _With_slash) noexcept;

    // replaces extension from current working path with replacement (if has extension)
    _NODISCARD path& __cdecl replace_extension(const path& _Replacement);

    // replaces filename from current working path with replacement (if has filename)
    _NODISCARD path& __cdecl replace_file(const path& _Replacement);

    // replaces stem from current working path with replacement (if has stem)
    _NODISCARD path& __cdecl replace_stem(const path& _Replacement);

    // returns root directory from current working path (if has)
    _NODISCARD path __thiscall root_directory() const noexcept;

    // returns root path from current working path
    _NODISCARD path __thiscall root_path() const noexcept;

    // returns stem (file name without extension) from current working path (if has)
    _NODISCARD path __thiscall stem() const noexcept;

private:
    string_type _Text; // current working path
};

// CLASS filesystem_error
class _FILESYSTEM_API filesystem_error { // base of all filesystem errors
public:
    __thiscall filesystem_error() noexcept : _Src(), _Cat(), _What() {}

    template<class _CharT, class = enable_if<_Is_CharT<_CharT>, void>>
    __cdecl filesystem_error(const _CharT* const _Errm);

    template<class _CharT, class = enable_if<_Is_CharT<_CharT>, void>>
    __cdecl filesystem_error(const _CharT* const _Errm, const error_type _Errc);

    template<class _CharT, class = enable_if<_Is_CharT<_CharT>, void>>
    __cdecl filesystem_error(const _CharT* const _Errm, const error_type _Errc, const path& _Errpos);

public:
    _NODISCARD const error_type __thiscall category() const noexcept; // error category

    _NODISCARD const path& __thiscall source() const noexcept; // error source
    
    _NODISCARD const char* __thiscall what() const noexcept; // informations about error

private:
    path _Src; // source of the error
    error_type _Cat; // error category
    const char* _What; // error message
};

// FUNCTION _Throw_fs_error
_FILESYSTEM_API __declspec(noreturn) void __cdecl _Throw_fs_error(const char* const _Errm);
_FILESYSTEM_API __declspec(noreturn) void __cdecl _Throw_fs_error(const char* const _Errm, const error_type _Errc);
_FILESYSTEM_API __declspec(noreturn) void __cdecl _Throw_fs_error(const char* const _Errm, const error_type _Errc, const path& _Errpos);

// FUNCTION current_path
_FILESYSTEM_API _NODISCARD path __stdcall current_path() noexcept;
_FILESYSTEM_API _NODISCARD bool __cdecl current_path(const path& _Path);

// ENUM CLASS file_access
enum class _FILESYSTEM_API file_access : unsigned long {
    readonly  = 0x80000000, // GENERIC_READ, file can be only readed
    writeonly = 0x40000000, // GENERIC_WRITE, file can be readed and overwritten
    all       = 0x10000000 // GENERIC_ALL, contains read, all and execute (it's redundant in this case)
};

_BITOPS(file_access)

// ENUM CLASS file_attributes
enum class _FILESYSTEM_API file_attributes : unsigned long {
    none          = 0, 
    readonly      = 0x0001, // FILE_ATTRIBUTE_READONLY
    hidden        = 0x0002, // FILE_ATTRIBUTE_HIDDEN
    system        = 0x0004, // FILE_ATTRIBUTE_SYSTEM, used only by system
    directory     = 0x0010, // FILE_ATTRIBUTE_DIRECTORY, is directory
    archive       = 0x0020, // FILE_ATTRIBUTE_ARCHIVE
    normal        = 0x0080, // FILE_ATTRIBUTE_NORMAL, without any attribute
    reparse_point = 0x0400, // FILE_ATTRIBUTE_REPARSE_POINT, symbolic link or mount point
    compressed    = 0x0800, // FILE_ATTRIBUTE_COMPRESSED
    encrypted     = 0x4000, // FILE_ATTRIBUTE_ENCRYPTED
    unknown       = (unsigned long) - 1 // INVALID_FILE_ATTRIBUTES, target not found
};

_BITOPS(file_attributes)

// ENUM CLASS file_disposition
enum class _FILESYSTEM_API file_disposition : unsigned int {
    only_new       = 0x1, // CREATE_NEW, creates only if not exists
    only_if_exists = 0x3, // OPEN_EXISTING, opens only if exists

    force_create = 0x2, // CREATE_ALWAYS, if exists, overwrites the file
    force_open   = 0x4 // OPEN_ALWAYS, if not exists, creates new
};

// ENUM CLASS file_flags
enum class _FILESYSTEM_API file_flags { // mainly for GetFileInformationByHandleEx()
    open_reparse_point = 0x00200000, // FILE_FLAG_OPEN_REPARSE_POINT
    backup_semantics   = 0x02000000 // FILE_FLAG_BACKUP_SEMANTICS
};

_BITOPS(file_flags)

// ENUM CLASS file_permissions
enum class _FILESYSTEM_API file_permissions {
    none,
    readonly,
    writeonly,
    all,
    unknown
};

// ENUM CLASS file_reparse_tag
enum class _FILESYSTEM_API file_reparse_tag : unsigned long {
    mount_point = 0xA0000003L, // IO_REPARSE_TAG_MOUNT_POINT
    symlink     = 0xA000000CL // IO_REPARSE_TAG_SYMLINK
};

// ENUM CLASS file_type
enum class _FILESYSTEM_API file_type : unsigned int {
    none,
    not_found,
    directory,
    regular, 

    // there could be also block, character, fifo and socket,
    // but there's no reason to implement it,
    // because Windows 10 don't supports it
    symlink,
    junction
};

// ENUM CLASS file_share
enum class _FILESYSTEM_API file_share : unsigned int {
    read   = 0x1, // FILE_SHARE_READ
    write  = 0x2, // FILE_SHARE_WRITE
    remove = 0x4, // FILE_SHARE_DELETE, is able to remove file/directory
    all    = read | write | remove
};

_BITOPS(file_share)

// CLASS status
class _FILESYSTEM_API file_status {
public:
    __thiscall file_status() noexcept;
    __cdecl file_status(const file_status&) = default;
    __cdecl file_status(file_status&&)      = default;
    virtual __thiscall ~file_status()       = default;
    file_status& __cdecl operator=(const file_status&) = delete;
    file_status& __cdecl operator=(file_status&&) = delete;

    explicit __cdecl file_status(const path& _Path) noexcept;

public:
    // returns attributes to current working path
    _NODISCARD const file_attributes __thiscall attribute() const noexcept;

    // returns permissions to current working path
    _NODISCARD const file_permissions __thiscall permissions() const noexcept;

    // returns current working path type
    _NODISCARD const file_type __thiscall type() const noexcept;

private:
    // inits all
    void __thiscall _Init() noexcept;

    // refreshs current working path type
    void __thiscall _Refresh() noexcept;

    // sets new attribute to current working path
    void __cdecl _Update_attribute(const file_attributes _Newattrib) noexcept;

    // sets new permissions to current working path
    void __cdecl _Update_permissions(const file_permissions _Newperms) noexcept;

    // sets new type to current working path
    void __cdecl _Update_type(const file_type _Newtype) noexcept;

private:
    path _Path; // current working path
    file_attributes _Attribute; // current working path attribute
    file_permissions _Perms; // current working path right access
    file_type _Type; // current working path type
};

// ENUM CLASS rename_options
enum class _FILESYSTEM_API rename_options : unsigned int {
    replace       = 0x1, // MOVEFILE_REPLACE_EXISTING, replaces existing
    copy          = 0x2, // MOVEFILE_COPY_ALLOWED, if moving to another volumine, copies and removes file
    write_through = 0x8 // MOVEFILE_WRITE_THROUGH, no returns if wasn't moved on drive
};

_BITOPS(rename_options)

// ENUM CLASS symlink_flag
enum class _FILESYSTEM_API symlink_flags {
    file               = 0x0, // symbolic link to file
    directory          = 0x1, // SYMBOLIC_LING_FLAG_DIRECTORY, symbolic link to directory
    allow_unprivileged = 0x2 // SYMBOLIC_LINK_FLAG_ALLWO_UNPRIVILEGED_CREATE, no need to have full access to target
};

_BITOPS(symlink_flags)

// CLASS directory_data
class _FILESYSTEM_API directory_data { // basic informations about files and directories inside directory
public:
    __thiscall directory_data() noexcept;
    explicit __cdecl directory_data(const directory_data&) noexcept = delete;
    explicit __cdecl directory_data(directory_data&&) noexcept      = delete;
    virtual __thiscall ~directory_data() noexcept                   = default;
    directory_data& __cdecl operator=(const directory_data&) noexcept = delete;
    directory_data& __cdecl operator=(directory_data&&) noexcept = delete;

    explicit __cdecl directory_data(const path& _Path) noexcept;

public:
    // returns names of directories inside _Path
    _NODISCARD const vector<path>& __thiscall directories() const noexcept;

    // returns count of directories inside _Path
    _NODISCARD const size_t __thiscall directories_count() const noexcept;

    // returns names of junctions inside _Path
    _NODISCARD const vector<path>& __thiscall junctions() const noexcept;

    // returns count of junctions inside _Path
    _NODISCARD const size_t __thiscall junctions_count() const noexcept;
    
    // returns names of other types inside _Path
    _NODISCARD const vector<path>& __thiscall others() const noexcept;

    // returns count of other types inside _Path
    _NODISCARD const size_t __thiscall others_count() const noexcept;
  
    // returns names of regular files inside _Path
    _NODISCARD const vector<path>& __thiscall regular_files() const noexcept;

    // returns count of regular files inside _Path
    _NODISCARD const size_t __thiscall regular_files_count() const noexcept;
    
    // returns names of symbolic links inside _Path
    _NODISCARD const vector<path>& __thiscall symlinks() const noexcept;

    // returns count of symbolic links inside _Path
    _NODISCARD const size_t __thiscall symlinks_count() const noexcept;
    
    // returns names of every type inside _Path
    _NODISCARD const vector<path>& __thiscall total() const noexcept;

    // returns count of every type inside _Path
    _NODISCARD const size_t __thiscall total_count() const noexcept;

private:
    // inits all
    void __thiscall _Init() noexcept;

    // sets the newest informations
    void __thiscall _Refresh() noexcept;

    // backs variables to original state
    void __thiscall _Reset() noexcept;

private:
    path _Path; // current working path
    array<vector<path>, 6> _Names; // directory, junction, other, regular, symlink and total
    array<size_t, 6> _Counts; // count of directorires, junctions, others, regulars, symlinks and total
};

// STRUCT reparse_data_buffer
struct _FILESYSTEM_API reparse_data_buffer final { // copy of _REPARSE_DATA_BUFFER
    unsigned long _Reparse_tag; // dwReparseTag
    uint16_t _Reparse_data_length; // wReparseDataLength
    uint16_t _Reserved; // wReserved

    union {
        struct {
            uint16_t _Substitute_name_offset; // SubstituteNameOffset
            uint16_t _Substitute_name_length; // SubstituteNameLenfth
            uint16_t _Print_name_offset; // wPrintNameOffset
            uint16_t _Print_name_length; // wPrintNameLength
            wchar_t _Path_buffer[1]; // cPathBuffer
        } _Symbolic_link_reparse_buffer; // SymbolicLinkReparseBuffer

        struct {
            uint16_t _Substitute_name_offset; // SubstituteNameOffset
            uint16_t _Substitute_name_length; // SubstituteNameLenfth
            uint16_t _Print_name_offset; // wPrintNameOffset
            uint16_t _Print_name_length; // wPrintNameLength
            wchar_t _Path_buffer[1]; // cPathBuffer
        } _Mount_point_reparse_buffer; // MountPointReparseBuffer
    
        struct {
            unsigned char _Data_buffer[1]; // DataBuffer[1]
        } _Generic_reparse_buffer; // GenericReparseBuffer
    };
};

// STRUCT reparse_mountpoint_data_buffer
struct _FILESYSTEM_API reparse_mountpoint_data_buffer final { // copy of _REPARSE_MOUNTPOINT_DATA_BUFFER
    unsigned long _Reparse_tag; // dwReparseTag
    unsigned long _Reparse_data_length; // dwReparseDataLength
    uint16_t _Reserved; // wReserved
    uint16_t _Reparse_target_length; // wReparseTargetLength
    uint16_t _Reparse_target_maximum_length; // wReparseTargetMaximumLength
    uint16_t _Reserved1; // wReserved1
    wchar_t _Reparse_target[1]; // cReparseTarget
};

// FUNCTION change_attributes
_FILESYSTEM_API _NODISCARD bool __cdecl change_attributes(const path& _Target, const file_attributes _Newattr);

// FUNCTION change_permissions
_FILESYSTEM_API _NODISCARD bool __cdecl change_permissions(const path& _Target, const file_permissions _Newperms,
    const bool _Inc_old, const bool _Symlinks);
_FILESYSTEM_API _NODISCARD bool __cdecl change_permissions(const path& _Target, const file_permissions _Newperms);

// FUNCTION clear
_FILESYSTEM_API _NODISCARD bool __cdecl clear(const path& _Target);

// ENUM CLASS copy_options
enum class _FILESYSTEM_API copy_options {
    none,

    overwrite, // replaces content of target with source
    replace, // removes existing and copies source to target path

    copy_symlink, // copies symbolic link to target path (by default creates new file/directory)
    copy_junction, // copies junction to target path (by default creates new directory)

    create_hard_link, // creates hard link in target path to source
    create_junction, // creates junction in target path to source
    create_symlink, // creates symbolic link in target to source

    cannot_exists, // error if already exists
    cannot_be_link // error if is a symbolic link or junction
};

_BITOPS(copy_options)

// FUNCTION copy
_FILESYSTEM_API _NODISCARD bool __cdecl copy(const path& _From, const path& _To, const copy_options _Options);
_FILESYSTEM_API _NODISCARD bool __cdecl copy(const path& _From, const path& _To);

// FUNCTION copy_file
_FILESYSTEM_API _NODISCARD bool __cdecl copy_file(const path& _From, const path& _To, const bool _Replace);

// FUNCTION copy_junction
_FILESYSTEM_API _NODISCARD bool __cdecl copy_junction(const path& _Junction, const path& _Newjunction);

// FUNCTION copy_symlink
_FILESYSTEM_API _NODISCARD bool __cdecl copy_symlink(const path& _Symlink, const path& _Newsymlink);

// STRUCT file_time
struct _FILESYSTEM_API file_time final {
    // calendar
    uint16_t _Year;
    uint16_t _Month;
    uint16_t _Day;

    // clock
    uint16_t _Hour;
    uint16_t _Minute;
    uint16_t _Second;
};

// FUNCTION creation_data
_FILESYSTEM_API _NODISCARD file_time __cdecl creation_time(const path& _Target);

// FUNCTION create_directory
_FILESYSTEM_API _NODISCARD bool __cdecl create_directory(const path& _Path);

// FUNCTION create_file
_FILESYSTEM_API _NODISCARD bool __cdecl create_file(const path& _Path, const file_attributes _Attributes);
_FILESYSTEM_API _NODISCARD bool __cdecl create_file(const path& _Path);

// FUNCTION create_hard_link
_FILESYSTEM_API _NODISCARD bool __cdecl create_hard_link(const path& _To, const path& _Hardlink);

// FUNCTION create_junction
_FILESYSTEM_API _NODISCARD bool __cdecl create_junction(const path& _To, const path& _Junction);

// FUNCTION create_symlink
_FILESYSTEM_API _NODISCARD bool __cdecl create_symlink(const path& _To, const path& _Symlink, const symlink_flags _Flags);
_FILESYSTEM_API _NODISCARD bool __cdecl create_symlink(const path& _To, const path& _Symlink);

// STRUCT file_id
struct _FILESYSTEM_API file_id final { // copy of _FILE_ID_INFO
    uint64_t _Volume_serial_number; // VolumeSerialNumber
    unsigned char _Id[16]; // FileId
};

// FUNCTION equivalent
_FILESYSTEM_API _NODISCARD bool __cdecl equivalent(const path& _Left, const path& _Right);

// FUNCTION file_size
_FILESYSTEM_API _NODISCARD size_t __cdecl file_size(const path& _Target);

// FUNCTION exists
_FILESYSTEM_API _NODISCARD bool __cdecl exists(const file_status _Status) noexcept;
_FILESYSTEM_API _NODISCARD bool __cdecl exists(const path& _Target) noexcept;

// FUNCTION hard_link_count
_FILESYSTEM_API _NODISCARD size_t __cdecl hard_link_count(const path& _Target,
    const file_attributes _Attributes, const file_flags _Flags);
_FILESYSTEM_API _NODISCARD size_t __cdecl hard_link_count(const path& _Target);

// FUNCTION is_directory
_FILESYSTEM_API _NODISCARD bool __cdecl is_directory(const file_status _Status) noexcept;
_FILESYSTEM_API _NODISCARD bool __cdecl is_directory(const path& _Target) noexcept;

// FUNCTION is_empty
_FILESYSTEM_API _NODISCARD bool __cdecl is_empty(const path& _Target);

// FUNCTION is_junction
_FILESYSTEM_API _NODISCARD bool __cdecl is_junction(const file_status _Status) noexcept;
_FILESYSTEM_API _NODISCARD bool __cdecl is_junction(const path& _Target) noexcept;

// FUNCTION is_other
_FILESYSTEM_API _NODISCARD bool __cdecl is_other(const file_status _Status) noexcept;
_FILESYSTEM_API _NODISCARD bool __cdecl is_other(const path& _Target) noexcept;

// FUNCTION is_regular_file
_FILESYSTEM_API _NODISCARD bool __cdecl is_regular_file(const file_status _Status) noexcept;
_FILESYSTEM_API _NODISCARD bool __cdecl is_regular_file(const path& _Target) noexcept;

// FUNCTION is_symlink
_FILESYSTEM_API _NODISCARD bool __cdecl is_symlink(const file_status _Status) noexcept;
_FILESYSTEM_API _NODISCARD bool __cdecl is_symlink(const path& _Target) noexcept;

// FUNCTION last_access_time
_FILESYSTEM_API _NODISCARD file_time __cdecl last_access_time(const path& _Target);

// FUNCTION last_write_time
_FILESYSTEM_API _NODISCARD file_time __cdecl last_write_time(const path& _Target);

// FUNCTION lines_count
_FILESYSTEM_API _NODISCARD size_t __cdecl lines_count(const path& _Target);

// FUNCTION read_all
_FILESYSTEM_API _NODISCARD vector<path> __cdecl read_all(const path& _Target);

// FUNCTION read_back
_FILESYSTEM_API _NODISCARD path __cdecl read_back(const path& _Target);

// FUNCTION read_front
_FILESYSTEM_API _NODISCARD path __cdecl read_front(const path& _Target);

// FUNCTION read_inside
_FILESYSTEM_API _NODISCARD path __cdecl read_inside(const path& _Target, const size_t _Line);

// FUNCTION read_junction
_FILESYSTEM_API _NODISCARD path __cdecl read_junction(const path& _Target);

// FUNCTION read_symlink
_FILESYSTEM_API _NODISCARD path __cdecl read_symlink(const path& _Target);

// FUNCTION remove
_FILESYSTEM_API _NODISCARD bool __cdecl remove(const path& _Path);

// FUNCTION remove_all
_FILESYSTEM_API _NODISCARD bool __cdecl remove_all(const path& _Path);

// FUNCTION remove_junction
_FILESYSTEM_API _NODISCARD bool __cdecl remove_junction(const path& _Target);

// FUNCTION remove_line
_FILESYSTEM_API _NODISCARD bool __cdecl remove_line(const path& _Target, const size_t _Line);

// FUNCTION rename
_FILESYSTEM_API _NODISCARD bool __cdecl rename(const path& _Old, const path& _New, const rename_options _Flags);
_FILESYSTEM_API _NODISCARD bool __cdecl rename(const path& _Old, const path& _New);

// FUNCTION resize_file
_FILESYSTEM_API _NODISCARD bool __cdecl resize_file(const path& _Target, const size_t _Newsize);

// STRUCT disk_space
struct _FILESYSTEM_API disk_space final {
    size_t _Available;
    size_t _Capacity;
    size_t _Free;
};

// FUNCTION space
_FILESYSTEM_API _NODISCARD disk_space __cdecl space(const path& _Path);

// FUNCTION write_back
_FILESYSTEM_API _NODISCARD bool __cdecl write_back(const path& _Target, const path& _Writable);

// FUNCTION write_front
_FILESYSTEM_API _NODISCARD bool __cdecl write_front(const path& _Target, const path& _Writable);

// FUNCTION write_inside
_FILESYSTEM_API _NODISCARD bool __cdecl write_inside(const path& _Target, const path& _Writable, const size_t _Line);

// FUNCTION write_instead
_FILESYSTEM_API _NODISCARD bool __cdecl write_instead(const path& _Target, const path& _Writable, const size_t _Line);
_FILESYSTEM_END

#pragma warning(pop)
#endif // !_HAS_WINDOWS
#endif // _FILESYSTEM_HPP_