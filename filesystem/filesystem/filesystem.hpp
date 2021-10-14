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

// Filesystem Api
#ifndef _FILESYSTEM_API
#ifdef FILESYSTEM_EXPORTS
#define _FILESYSTEM_API __declspec(dllexport)
#else // ^^^ FILESYSTEM_EXPORTS ^^^ / vvv !FILESYSTEM_EXPORTS vvv
#define _FILESYSTEM_API __declspec(dllimport)
#endif // FILESYSTEM_EXPORTS
#endif // _FILESYSTEM_API

// Some macros may be defined in <yvals_core.h> and <yvals.h>.
#include <yvals_core.h>
#include <yvals.h>

// Attributes
#ifndef _NODISCARD
#define _NODISCARD [[nodiscard]]
#endif // _NODISCARD

// Exceptions
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

// Namespace
#ifndef _FILESYSTEM_BEGIN
#define _FILESYSTEM_BEGIN namespace filesystem {
#endif // _FILESYSTEM_BEGIN

#ifndef _FILESYSTEM_END
#define _FILESYSTEM_END }
#endif // _FILESYSTEM_END

#ifndef _FILESYSTEM
#define _FILESYSTEM ::filesystem::
#endif // _FILESYSTEM

// Deprecated Messages
#ifndef _FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS
#define _FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS                           \
    __declspec(deprecated("The filesystem::shortcut_parameters() is unsafe " \
        "and will be removed or redefined in future version."))
#endif // _FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS

// Operators (No Unnecessary)
#ifndef _BITOPS
#define _BITOPS(_Bitmask)                                                                       \
_NODISCARD constexpr _Bitmask operator&(const _Bitmask _Left, const _Bitmask _Right) noexcept { \
    using _IntTy = _STD underlying_type_t<_Bitmask>;                                            \
    return static_cast<_Bitmask>(static_cast<_IntTy>(_Left) & static_cast<_IntTy>(_Right));     \
}                                                                                               \
                                                                                                \
_NODISCARD constexpr _Bitmask operator|(const _Bitmask _Left, const _Bitmask _Right) noexcept { \
    using _IntTy = _STD underlying_type_t<_Bitmask>;                                            \
    return static_cast<_Bitmask>(static_cast<_IntTy>(_Left) | static_cast<_IntTy>(_Right));     \
}                                                                                               \
                                                                                                \
_NODISCARD constexpr _Bitmask operator^(const _Bitmask _Left, const _Bitmask _Right) noexcept { \
    using _IntTy = _STD underlying_type_t<_Bitmask>;                                            \
    return static_cast<_Bitmask>(static_cast<_IntTy>(_Left) ^ static_cast<_IntTy>(_Right));     \
}                                                                                               \
                                                                                                \
constexpr _Bitmask& operator^=(_Bitmask& _Left, const _Bitmask _Right) noexcept {               \
    _Left = _Left ^ _Right;                                                                     \
    return _Left;                                                                               \
}
#endif // _BITOPS

#if !_HAS_WINDOWS
#pragma message("The contents of <filesystem.hpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv
#pragma warning(push)
#pragma warning(disable : 4996) // C4996: using deprecated content
#pragma warning(disable : 4251) // C4251: some STL classes requires dll library

// These libraries contains each function/macro/type that filesystem.dll uses.
// <Windows.h> must be included before C-libraries.
#include <Windows.h>
#include <array>
#include <codecvt>
#include <combaseapi.h>
#include <coml2api.h>
#include <CommCtrl.h>
#include <corecrt_wstring.h>
#include <errhandlingapi.h>
#include <fileapi.h>
#include <fstream>
#include <handleapi.h>
#include <ioapiset.h>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <libloaderapi.h>
#include <limits.h>
#include <locale>
#include <minwinbase.h>
#include <objbase.h>
#include <objidl.h>
#include <ostream>
#include <processenv.h>
#include <ShlGuid.h>
#include <shellapi.h>
#include <ShObjIdl.h>
#include <ShObjIdl_core.h>
#include <stdexcept>
#include <string>
#include <stringapiset.h>
#include <timezoneapi.h>
#include <vcruntime.h>
#include <vcruntime_string.h>
#include <vector>
#include <WinBase.h>
#include <winerror.h>
#include <winioctl.h>
#include <WinNls.h>
#include <winnt.h>
#include <xiosbase>
#include <xlocbuf>
#include <xutility>
#include <xtr1common>

// STD allocators
using _STD allocator;
using _STD array;
using _STD vector;

// STD characters
using _STD char_traits;

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

// CONSTANT _Is_char_t
template <class _Ty>
inline constexpr bool _Is_char_t = _STD _Is_any_of_v<_Ty, char, char8_t, char16_t, char32_t, wchar_t>;

// CONSTANT _Is_narrow_char_t
template <class _Ty>
inline constexpr bool _Is_narrow_char_t = _STD is_same_v<_Ty, char>;

// CONSTANT _Is_src_t
template <class _Ty>
inline constexpr bool _Is_src_t = _STD _Is_any_of_v<_Ty, string, u8string, u16string, u32string, wstring,
    string_view, u8string_view, u16string_view, u32string_view, wstring_view>;

// CONSTANT _Is_narrow_src_t
template <class _Ty>
inline constexpr bool _Is_narrow_src_t = _STD _Is_any_of_v<_Ty, string, string_view>;

// CONSTANT _Max_path
inline constexpr size_t _Max_path = 260;

// ENUM CLASS error_type
enum class _FILESYSTEM_API error_type : unsigned int { // error type for _Throw_system_error
    invalid_argument,
    length_error,
    runtime_error
};

// ENUM CLASS code_page
enum class _FILESYSTEM_API code_page {
    acp  = 0, // ansi code page
    utf8 = 65001 // default code page
};

// FUNCTION _Throw_system_error
_FILESYSTEM_API __declspec(noreturn) void _Throw_system_error(const char* const _Src, const char* const _Msg, const error_type _Code);

// FUNCTION _Convert_narrow_to_wide
_FILESYSTEM_API _NODISCARD wstring _Convert_narrow_to_wide(const code_page _Cp, const string_view _Input);

// FUNCTION _Convert_wide_to_narrow
_FILESYSTEM_API _NODISCARD string _Convert_wide_to_narrow(const code_page _Cp, const wstring_view _Input);

// FUNCTION TEMPLATE _Convert_utf_to_wide
template <class _Elem, class _Traits = char_traits<_Elem>>
_FILESYSTEM_API _NODISCARD constexpr string _Convert_utf_to_narrow(const basic_string_view<_Elem, _Traits> _Input) noexcept(_Is_narrow_char_t<_Elem>);

// FUNCTION TEMPLATE _Convert_narrow_to_utf
template <class _Elem, class _Traits = char_traits<_Elem>, class _Alloc = allocator<_Elem>>
_FILESYSTEM_API _NODISCARD constexpr basic_string<_Elem, _Traits, _Alloc> _Convert_narrow_to_utf(
    const string_view _Input) noexcept(_Is_narrow_char_t<_Elem>);

// FUNCTION TEMPLATE _Convert_to_narrow
template <class _Elem, class _Traits = char_traits<_Elem>>
_FILESYSTEM_API _NODISCARD constexpr string _Convert_to_narrow(const basic_string_view<_Elem, _Traits> _Input) noexcept(_Is_narrow_char_t<_Elem>);

// PREDEFINED CLASS path
class path;

// FUNCTION TEMPLATE operator>>
template <class _Elem, class _Traits = char_traits<_Elem>>
_FILESYSTEM_API _NODISCARD constexpr basic_istream<_Elem, _Traits>& operator>>(basic_istream<_Elem, _Traits>& _Istr, path& _Path);

// FUNCTION TEMPLATE operator<<
template <class _Elem, class _Traits = char_traits<_Elem>>
_FILESYSTEM_API _NODISCARD constexpr basic_ostream<_Elem, _Traits>& operator<<(basic_ostream<_Elem, _Traits>& _Ostr, const path& _Path);

// FUNCTION TEMPLATE operator+
_FILESYSTEM_API _NODISCARD path operator+(const path& _Left, const path& _Right);
template <class _CharTy>
_FILESYSTEM_API _NODISCARD constexpr path operator+(const path& _Left, const _CharTy* const _Right);
template <class _CharTy>
_FILESYSTEM_API _NODISCARD constexpr path operator+(const _CharTy* const _Left, const path& _Right);
template <class _Elem, class _Traits = char_traits<_Elem>, class _Alloc = allocator<_Elem>>
_FILESYSTEM_API _NODISCARD constexpr path operator+(const path& _Left, const basic_string<_Elem, _Traits, _Alloc>& _Right);
template <class _Elem, class _Traits = char_traits<_Elem>, class _Alloc = allocator<_Elem>>
_FILESYSTEM_API _NODISCARD constexpr path operator+(const basic_string<_Elem, _Traits, _Alloc>& _Left, const path& _Right);

// CLASS path
class _FILESYSTEM_API path { // takes any string of characters
public:
    using value_type      = char;
    using string_type     = string;
    using size_type       = typename string_type::size_type;
    using pointer         = typename string_type::pointer;
    using const_pointer   = typename string_type::const_pointer;
    using reference       = typename string_type::reference;
    using const_reference = typename string_type::const_reference;

    using iterator       = typename string_type::iterator;
    using const_iterator = typename string_type::const_iterator;

    using reverse_iterator       = typename string_type::reverse_iterator;
    using const_reverse_iterator = typename string_type::const_reverse_iterator;

    static constexpr auto npos{static_cast<size_type>(-1)};

    path()            = default;
    path(const path&) = default;
    path(path&&)      = default;
    ~path()           = default;

    template <class _CharTy>
    path(const _CharTy* const _Source); // all character types

    template <class _Src>
    path(const _Src& _Source); // all string types

    path& operator=(const path& _Source);
    path& assign(const path& _Source);

    template <class _CharTy>
    path& operator=(const _CharTy* const _Source); // all character types

    template <class _CharTy>
    path& assign(const _CharTy* const _Source); // all character types

    template <class _Src>
    path& operator=(const _Src& _Source); // all string types

    template <class _Src>
    path& assign(const _Src& _Source); // all string types

    path& operator+=(const path& _Added);
    path& append(const path& _Added);

    template <class _CharTy>
    path& operator+=(const _CharTy* const _Added); // all character types

    template <class _CharTy>
    path& append(const _CharTy* const _Added); // all character types

    template <class _Src>
    path& operator+=(const _Src& _Added); // all string types

    template <class _Src>
    path& append(const _Src& _Added); // all string types

    bool operator==(const path& _Compare) const noexcept;

    template <class _CharTy>
    bool operator==(const _CharTy* const _Compare) const;

    template <class _Src>
    bool operator==(const _Src& _Compare) const;

    bool operator!=(const path& _Compare) const noexcept;

    template <class _CharTy>
    bool operator!=(const _CharTy* const _Compare) const;

    template <class _Src>
    bool operator!=(const _Src& _Compare) const;

    _NODISCARD constexpr reference operator[](const size_type _Pos);
    _NODISCARD constexpr const_reference operator[](const size_type _Pos) const;

    // returns element at _Pos position
    _NODISCARD constexpr reference at(const size_type _Pos);
    _NODISCARD constexpr const_reference at(const size_type _Pos) const;

    // returns iterator with first element
    _NODISCARD constexpr iterator begin() noexcept;
    _NODISCARD constexpr const_iterator begin() const noexcept;

    // clears the current working path
    constexpr void clear() noexcept;

    // returns the directory from the current working path (if has)
    _NODISCARD path directory() const noexcept;

    // returns the drive from the current working path (if has)
    _NODISCARD path drive() const noexcept;

    // checks if the current working path is empty
    _NODISCARD constexpr bool empty() const noexcept;

    // returns iterator with last element
    _NODISCARD constexpr iterator end() noexcept;
    _NODISCARD constexpr const_iterator end() const noexcept;

    // returns the extension from the current working path (if has)
    _NODISCARD path extension() const noexcept;

    // returns the filename from current the working path (if has)
    _NODISCARD path file() const noexcept;

    // removes unnecessary slashes from the current working path
    // and converts to the Windows 10 standard
    _NODISCARD path& fix() noexcept;

    // returns the current working path as string
    _NODISCARD const string generic_string() const noexcept;

    // returns the current working path as u8string
    _NODISCARD const u8string generic_u8string() const;

    // returns the current working path as u16string
    _NODISCARD const u16string generic_u16string() const;

    // returns the current working path as u32string
    _NODISCARD const u32string generic_u32string() const;

    // returns the current working path as wstring
    _NODISCARD const wstring generic_wstring() const;

    // checks if the current working path has the directory
    _NODISCARD bool has_directory() const noexcept;

    // checks if the current working path has the drive
    _NODISCARD bool has_drive() const noexcept;

    // checks if the current working path has the extension
    _NODISCARD bool has_extension() const noexcept;
    
    // checks if the current working path has the file
    _NODISCARD bool has_file() const noexcept;

    // checks if the current working path has the parent directory
    _NODISCARD bool has_parent_directory() const noexcept;

    // checks if the current working path has the root directory
    _NODISCARD bool has_root_directory() const noexcept;

    // checks if the current working path has the stem (filename without extension)
    _NODISCARD bool has_stem() const noexcept;

    // checks if the current working path is absolute
    _NODISCARD bool is_absolute() const noexcept;
    
    // checks if the current working path is relative
    _NODISCARD bool is_relative() const noexcept;

    // converts the current working path to the Windows 10 standard
    _NODISCARD path& make_preferred() noexcept;

    // returns the parent directory from the current working path
    _NODISCARD path parent_directory() const noexcept;

    // returns the parent path from the current working path
    _NODISCARD path parent_path() const noexcept;

    // returns reverse iterator with first element
    _NODISCARD constexpr reverse_iterator rend() noexcept;
    _NODISCARD constexpr const_reverse_iterator rend() const noexcept;

    // removes the directory from the current working path (if has)
    _NODISCARD path& remove_directory(const bool _With_slash = true) noexcept;

    // removes the extension from the current working path (if has)
    _NODISCARD path& remove_extension() noexcept;

    // removes the filename from the current working path (if has) (removes slash, if has and _With_slash is true)
    _NODISCARD path& remove_file(const bool _With_slash = true) noexcept;

    // replaces the directory from the current working path with the replacement (if has)
    _NODISCARD path& replace_directory(const path& _Replacement);

    // replaces the extension from the current working path with the replacement (if has)
    _NODISCARD path& replace_extension(const path& _Replacement);

    // replaces the filename from the current working path with the replacement (if has)
    _NODISCARD path& replace_file(const path& _Replacement);

    // replaces the stem from the current working path with the replacement (if has)
    _NODISCARD path& replace_stem(const path& _Replacement);

    // resizes the path to _Newsize
    constexpr void resize(const size_type _Newsize, const value_type _Ch = value_type(0));

    // returns the root directory from the current working path (if has)
    _NODISCARD path root_directory() const noexcept;

    // returns the root path from the current working path (if has)
    _NODISCARD path root_path() const noexcept;

    // returns the size of the current working path
    _NODISCARD constexpr size_t size() const noexcept;

    // returns the stem (file name without the extension) from the current working path (if has)
    _NODISCARD path stem() const noexcept;

private:
    // verifies path size
    constexpr void _Check_size() const;
    
    string_type _Mytext; // current working path
};

// CLASS filesystem_error
class _FILESYSTEM_API filesystem_error { // base of all filesystem errors
public:
    filesystem_error() noexcept;
    ~filesystem_error() noexcept;

    template <class _CharTy>
    filesystem_error(const _CharTy* const _Msg) noexcept(_Is_narrow_char_t<_CharTy>);

    template <class _CharTy>
    filesystem_error(const _CharTy* const _Msg, const error_type _Code) noexcept(_Is_narrow_char_t<_CharTy>);

    template <class _CharTy>
    filesystem_error(const _CharTy* const _Msg, const error_type _Code, const path& _Src) noexcept(_Is_narrow_char_t<_CharTy>);

    // error category from error_type enum
    _NODISCARD const error_type category() const noexcept;

    // error source
    _NODISCARD const path& source() const noexcept;
    
    // informations about error
    _NODISCARD const string& what() const noexcept;

private:
    path _Mysrc; // source of the error
    error_type _Mycat; // error category
    string _Mywhat; // error message
};

#pragma warning(push)
#pragma warning(disable : 4455) // C4455: reserved name
namespace path_literals {
    // FUNCTION operator""p
    _FILESYSTEM_API _NODISCARD path operator""p(const char* const _Str, const size_t _Size) noexcept;
    _FILESYSTEM_API _NODISCARD path operator""p(const char8_t* const _Str, const size_t _Size) noexcept;
    _FILESYSTEM_API _NODISCARD path operator""p(const char16_t* const _Str, const size_t _Size) noexcept;
    _FILESYSTEM_API _NODISCARD path operator""p(const char32_t* const _Str, const size_t _Size) noexcept;
    _FILESYSTEM_API _NODISCARD path operator""p(const wchar_t* const _Str, const size_t _Size) noexcept;
} // path_literals
#pragma warning(pop)

// FUNCTION _Throw_fs_error
_FILESYSTEM_API __declspec(noreturn) void _Throw_fs_error(const char* const _Msg);
_FILESYSTEM_API __declspec(noreturn) void _Throw_fs_error(const char* const _Msg, const error_type _Code);
_FILESYSTEM_API __declspec(noreturn) void _Throw_fs_error(const char* const _Msg, const error_type _Code, const path& _Src);

// verification macros
#ifndef _FILESYSTEM_VERIFY
#define _FILESYSTEM_VERIFY(_Cond, _Msg, _Err)                                \
    do {                                                                     \
        if (_Cond) {                                                         \
            /* nothing to do */                                              \
        } else {                                                             \
            _Throw_fs_error(_Msg, _Err, static_cast<const char*>(__func__)); \
        }                                                                    \
    } while (false);
#endif // _FILESYSTEM_VERIFY

#ifndef _FILESYSTEM_VERIFY_COM_RESULT
#define _FILESYSTEM_VERIFY_COM_RESULT(_Result, _Obj)                                                          \
    do {                                                                                                      \
        if (_Result == S_OK) {                                                                                \
            /* nothing to do */                                                                               \
        } else {                                                                                              \
            _Obj->Release();                                                                                  \
            _Throw_fs_error("bad COM result", error_type::runtime_error, static_cast<const char*>(__func__)); \
        }                                                                                                     \
    } while (false);
#endif // _FILESYSTEM_VERIFY_COM_RESULT

#ifndef _FILESYSTEM_VERIFY_FILE_STREAM
#define _FILESYSTEM_VERIFY_FILE_STREAM(_Stream)                                                         \
    do {                                                                                                \
        if (_Stream) {                                                                                  \
            /* nothing to do */                                                                         \
        } else {                                                                                        \
            if (_Stream.is_open()) {                                                                    \
                _Stream.close();                                                                        \
            }                                                                                           \
                                                                                                        \
            _Throw_fs_error("bad file", error_type::runtime_error, static_cast<const char*>(__func__)); \
        }                                                                                               \
    } while (false);
#endif // _FILESYSTEM_VERIFY_FILE_STREAM

#ifndef _FILESYSTEM_VERIFY_HANDLE
#define _FILESYSTEM_VERIFY_HANDLE(_Handle) \
    _FILESYSTEM_VERIFY(_Handle != INVALID_HANDLE_VALUE, "failed to get handle", error_type::runtime_error)
#endif // _FILESYSTEM_VERIFY_HANDLE

// FUNCTION current_path
_FILESYSTEM_API _NODISCARD path current_path() noexcept;
_FILESYSTEM_API _NODISCARD bool current_path(const path& _Path);

// FUNCTION make_path
_FILESYSTEM_API _NODISCARD path make_path(const path& _Path, const bool _Module);

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

// CLASS file_status
class _FILESYSTEM_API file_status {
public:
    file_status() noexcept;
    file_status(const file_status&) = default;
    file_status(file_status&&)      = default;
    virtual ~file_status()          = default;

    file_status& operator=(const file_status&) = delete;
    file_status& operator=(file_status&&) = delete;

    explicit file_status(const path& _Path) noexcept;

    explicit file_status(const path& _Target, const file_attributes _Attr,
        const file_permissions _Perms, const file_type _Type) noexcept;

    // returns attributes to current working path
    _NODISCARD const file_attributes attribute() const noexcept;

    // returns permissions to current working path
    _NODISCARD const file_permissions permissions() const noexcept;

    // returns current working path type
    _NODISCARD const file_type type() const noexcept;

private:
    // inits all
    void _Init() noexcept;

    // refreshs current working path type
    void _Refresh() noexcept;

    // sets new attribute to current working path
    void _Update_attribute(const file_attributes _Newattrib) noexcept;

    // sets new permissions to current working path
    void _Update_permissions(const file_permissions _Newperms) noexcept;

    // sets new type to current working path
    void _Update_type(const file_type _Newtype) noexcept;

    path _Mypath; // current working path
    file_attributes _Myattr; // current working path attribute
    file_permissions _Myperms; // current working path right access
    file_type _Mytype; // current working path type
};

// ENUM CLASS rename_options
enum class _FILESYSTEM_API rename_options : unsigned int {
    replace       = 0x1, // MOVEFILE_REPLACE_EXISTING, replaces existing
    copy          = 0x2, // MOVEFILE_COPY_ALLOWED, if moving to another volumine, copies and removes file
    write_through = 0x8 // MOVEFILE_WRITE_THROUGH, no returns if wasn't moved on drive
};

_BITOPS(rename_options)

// ENUM CLASS symlink_flags
enum class _FILESYSTEM_API symlink_flags {
    file               = 0x0, // symbolic link to file
    directory          = 0x1, // SYMBOLIC_LING_FLAG_DIRECTORY, symbolic link to directory
    allow_unprivileged = 0x2 // SYMBOLIC_LINK_FLAG_ALLWO_UNPRIVILEGED_CREATE, no need to have full access to target
};

_BITOPS(symlink_flags)

// CLASS directory_data
class _FILESYSTEM_API directory_data { // basic informations about files and directories inside directory
public:
    directory_data() noexcept;
    explicit directory_data(const directory_data&) noexcept = delete;
    explicit directory_data(directory_data&&) noexcept      = delete;
    ~directory_data() noexcept                              = default;

    directory_data& operator=(const directory_data&) noexcept = delete;
    directory_data& operator=(directory_data&&) noexcept = delete;

    explicit directory_data(const path& _Path) noexcept;

    // returns names of directories inside _Path
    _NODISCARD const vector<path>& directories() const noexcept;

    // returns count of directories inside _Path
    _NODISCARD const uintmax_t directories_count() const noexcept;

    // returns names of junctions inside _Path
    _NODISCARD const vector<path>& junctions() const noexcept;

    // returns count of junctions inside _Path
    _NODISCARD const uintmax_t junctions_count() const noexcept;
    
    // returns names of other types inside _Path
    _NODISCARD const vector<path>& others() const noexcept;

    // returns count of other types inside _Path
    _NODISCARD const uintmax_t others_count() const noexcept;
  
    // returns names of regular files inside _Path
    _NODISCARD const vector<path>& regular_files() const noexcept;

    // returns count of regular files inside _Path
    _NODISCARD const uintmax_t regular_files_count() const noexcept;
    
    // returns names of symbolic links inside _Path
    _NODISCARD const vector<path>& symlinks() const noexcept;

    // returns count of symbolic links inside _Path
    _NODISCARD const uintmax_t symlinks_count() const noexcept;
    
    // returns names of every type inside _Path
    _NODISCARD const vector<path>& total() const noexcept;

    // returns count of every type inside _Path
    _NODISCARD const uintmax_t total_count() const noexcept;

private:
    // inits all
    void _Init() noexcept;

    // sets the newest informations
    void _Refresh() noexcept;

    // backs variables to original state
    void _Reset() noexcept;

private:
    path _Mypath; // current working path
    array<vector<path>, 6> _Myname; // directory, junction, other, regular, symlink and total
    array<size_t, 6> _Mycount; // count of directorires, junctions, others, regulars, symlinks and total
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
_FILESYSTEM_API _NODISCARD bool change_attributes(const path& _Target, const file_attributes _Newattr);

// FUNCTION change_permissions
_FILESYSTEM_API _NODISCARD bool change_permissions(const path& _Target, const file_permissions _Newperms,
    const bool _Old, const bool _Links);
_FILESYSTEM_API _NODISCARD bool change_permissions(const path& _Target, const file_permissions _Newperms);

// FUNCTION clear
_FILESYSTEM_API _NODISCARD bool clear(const path& _Target);

// ENUM CLASS copy_options
enum class _FILESYSTEM_API copy_options : unsigned int {
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
_FILESYSTEM_API _NODISCARD bool copy(const path& _From, const path& _To, const copy_options _Options);
_FILESYSTEM_API _NODISCARD bool copy(const path& _From, const path& _To);

// FUNCTION copy_file
_FILESYSTEM_API _NODISCARD bool copy_file(const path& _From, const path& _To, const bool _Replace);

// FUNCTION copy_junction
_FILESYSTEM_API _NODISCARD bool copy_junction(const path& _Junction, const path& _Newjunction);

// FUNCTION copy_symlink
_FILESYSTEM_API _NODISCARD bool copy_symlink(const path& _Symlink, const path& _Newsymlink);

// STRUCT file_time
struct _FILESYSTEM_API file_time final {
    // calendar
    uint16_t year;
    uint16_t month;
    uint16_t day;

    // clock
    uint16_t hour;
    uint16_t minute;
    uint16_t second;
};

// FUNCTION creation_data
_FILESYSTEM_API _NODISCARD file_time creation_time(const path& _Target);

// FUNCTION create_directory
_FILESYSTEM_API _NODISCARD bool create_directory(const path& _Path);

// FUNCTION create_file
_FILESYSTEM_API _NODISCARD bool create_file(const path& _Path, const file_attributes _Attributes);
_FILESYSTEM_API _NODISCARD bool create_file(const path& _Path);

// FUNCTION create_hard_link
_FILESYSTEM_API _NODISCARD bool create_hard_link(const path& _To, const path& _Hardlink);

// FUNCTION create_junction
_FILESYSTEM_API _NODISCARD bool create_junction(const path& _To, const path& _Junction);

namespace experimental {
    // FUNCTION TEMPLATE create_shortcut
#ifdef _FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS
    _FILESYSTEM_API _NODISCARD bool create_shortcut(const path& _To, const path& _Shortcut);
#else // ^^^ _FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS ^^^ / vvv !_FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS vvv
    template <class _CharTy>
    _FILESYSTEM_API _NODISCARD bool create_shortcut(const path& _To, const path& _Shortcut, const _CharTy* const _Description = nullptr);
#endif // _FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS
} // experimental

// FUNCTION create_symlink
_FILESYSTEM_API _NODISCARD bool create_symlink(const path& _To, const path& _Symlink, const symlink_flags _Flags);
_FILESYSTEM_API _NODISCARD bool create_symlink(const path& _To, const path& _Symlink);

// STRUCT file_id
struct _FILESYSTEM_API file_id final { // copy of _FILE_ID_INFO
    uint64_t _Volume_serial_number; // VolumeSerialNumber
    unsigned char _Id[16]; // FileId
};

// FUNCTION equivalent
_FILESYSTEM_API _NODISCARD bool equivalent(const path& _Left, const path& _Right);

// FUNCTION exists
_FILESYSTEM_API _NODISCARD bool exists(const file_status _Status) noexcept;
_FILESYSTEM_API _NODISCARD bool exists(const path& _Target) noexcept;

// FUNCTION file_size
_FILESYSTEM_API _NODISCARD size_t file_size(const path& _Target);

// FUNCTION hard_link_count
_FILESYSTEM_API _NODISCARD uintmax_t hard_link_count(const path& _Target, const file_flags _Flags);
_FILESYSTEM_API _NODISCARD uintmax_t hard_link_count(const path& _Target);

// FUNCTION _Is_directory
// returns true if _Target is directory/symlink/junction
_FILESYSTEM_API _NODISCARD bool _Is_directory(const file_status _Status) noexcept;
_FILESYSTEM_API _NODISCARD bool _Is_directory(const path& _Target) noexcept;

// FUNCTION is_directory
_FILESYSTEM_API _NODISCARD bool is_directory(const file_status _Status) noexcept;
_FILESYSTEM_API _NODISCARD bool is_directory(const path& _Target) noexcept;

// FUNCTION is_empty
_FILESYSTEM_API _NODISCARD bool is_empty(const path& _Target);

// FUNCTION is_junction
_FILESYSTEM_API _NODISCARD bool is_junction(const file_status _Status) noexcept;
_FILESYSTEM_API _NODISCARD bool is_junction(const path& _Target) noexcept;

// FUNCTION is_other
_FILESYSTEM_API _NODISCARD bool is_other(const file_status _Status) noexcept;
_FILESYSTEM_API _NODISCARD bool is_other(const path& _Target) noexcept;

// FUNCTION is_regular_file
_FILESYSTEM_API _NODISCARD bool is_regular_file(const file_status _Status) noexcept;
_FILESYSTEM_API _NODISCARD bool is_regular_file(const path& _Target) noexcept;

// FUNCTION is_symlink
_FILESYSTEM_API _NODISCARD bool is_symlink(const file_status _Status) noexcept;
_FILESYSTEM_API _NODISCARD bool is_symlink(const path& _Target) noexcept;

// FUNCTION junction_status
_FILESYSTEM_API _NODISCARD file_status junction_status(const path& _Target) noexcept;

// FUNCTION last_access_time
_FILESYSTEM_API _NODISCARD file_time last_access_time(const path& _Target);

// FUNCTION last_write_time
_FILESYSTEM_API _NODISCARD file_time last_write_time(const path& _Target);

// FUNCTION lines_count
_FILESYSTEM_API _NODISCARD uintmax_t lines_count(const path& _Target);

// Functions that reads content from file (read_all(), read_back(), read_front() and read_inside())
// are using string as return type. Don't use path because it accepts only 260 characters.
// If you want to use they in other basic_string return type, just use _Convert_narrow_to_wide() or _Convert_narrow_to_utf().

// FUNCTION read_all
_FILESYSTEM_API _NODISCARD vector<string> read_all(const path& _Target);

// FUNCTION read_back
_FILESYSTEM_API _NODISCARD string read_back(const path& _Target);

// FUNCTION read_front
_FILESYSTEM_API _NODISCARD string read_front(const path& _Target);

// FUNCTION read_inside
_FILESYSTEM_API _NODISCARD string read_inside(const path& _Target, const uintmax_t _Line);

// FUNCTION read_junction
_FILESYSTEM_API _NODISCARD path read_junction(const path& _Target);

// FUNCTION read_shortcut
_FILESYSTEM_API _NODISCARD path read_shortcut(const path& _Target);

// FUNCTION read_symlink
_FILESYSTEM_API _NODISCARD path read_symlink(const path& _Target);

// FUNCTION remove
_FILESYSTEM_API _NODISCARD bool remove(const path& _Path);

// FUNCTION remove_all
_FILESYSTEM_API _NODISCARD bool remove_all(const path& _Path);

// FUNCTION remove_junction
_FILESYSTEM_API _NODISCARD bool remove_junction(const path& _Target);

// FUNCTION remove_line
_FILESYSTEM_API _NODISCARD bool remove_line(const path& _Target, const uintmax_t _Line);

// FUNCTION remove_lines
_FILESYSTEM_API _NODISCARD bool remove_lines(const path& _Target, const uintmax_t _First, uintmax_t _Count);

// FUNCTION rename
_FILESYSTEM_API _NODISCARD bool rename(const path& _Old, const path& _New, const rename_options _Flags);
_FILESYSTEM_API _NODISCARD bool rename(const path& _Old, const path& _New);

// FUNCTION resize_file
_FILESYSTEM_API _NODISCARD bool resize_file(const path& _Target, const size_t _Newsize);

namespace experimental {
    // STRUCT shortcut_data
    struct _FILESYSTEM_API _FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS shortcut_data final { // warinig C6001 if not defined
        wstring arguments{};
        wstring description{};
        uint16_t hotkey{};
        path icon_path{};
        int icon{};
        ITEMIDLIST* id_list{};
        path target_path{};
        int show_cmd{};
        path directory{};
    };

    // FUNCTION shortcut_parameters
    _FILESYSTEM_API _NODISCARD _FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS shortcut_data shortcut_parameters(const path& _Target);
    _FILESYSTEM_API _NODISCARD _FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS bool shortcut_parameters(
        const path& _Target, shortcut_data* const _Params);
} // experimental

// STRUCT disk_space
struct _FILESYSTEM_API disk_space final {
    uintmax_t available;
    uintmax_t capacity;
    uintmax_t free;
};

// FUNCTION space
_FILESYSTEM_API _NODISCARD disk_space space(const path& _Path);

// FUNCTION status
_FILESYSTEM_API _NODISCARD file_status status(const path& _Target) noexcept;

// FUNCTION status_known
_FILESYSTEM_API _NODISCARD bool status_known(const file_status _Status) noexcept;
_FILESYSTEM_API _NODISCARD bool status_known(const path& _Target) noexcept;

// FUNCTION symlink_status
_FILESYSTEM_API _NODISCARD file_status symlink_status(const path& _Target) noexcept;

// FUNCTION temp_directory_path
_FILESYSTEM_API _NODISCARD path temp_directory_path();

// FUNCTION TEMPLATE write_back
template <class _CharTy>
_FILESYSTEM_API _NODISCARD constexpr bool write_back(const path& _Target, const _CharTy* const _Writable);

// FUNCTION TEMPLATE write_front
template <class _CharTy>
_FILESYSTEM_API _NODISCARD constexpr bool write_front(const path& _Target, const _CharTy* const _Writable);

// FUNCTION TEMPLATE write_inside
template <class _CharTy>
_FILESYSTEM_API _NODISCARD constexpr bool write_inside(const path& _Target, const _CharTy* const _Writable, const uintmax_t _Line);

// FUNCTION TEMPLATE write_instead
template <class _CharTy>
_FILESYSTEM_API _NODISCARD constexpr bool write_instead(const path& _Target, const _CharTy* const _Writable, const uintmax_t _Line);
_FILESYSTEM_END

#pragma warning(pop)
#endif // !_HAS_WINDOWS
#endif // _FILESYSTEM_HPP_