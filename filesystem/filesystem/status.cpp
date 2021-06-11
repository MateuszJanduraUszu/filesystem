// status.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <pch.h>
#include <filesystem.hpp>

#if !_HAS_WINDOWS
#pragma message("The contents of <status.cpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv
#pragma warning(push)
#pragma warning(disable : 4996) // C4996: using deprecated content

_FILESYSTEM_BEGIN
_EXPERIMENTAL_BEGIN
// FUNCTION file_status::file_status
__thiscall file_status::file_status() noexcept {
    this->_Init();
    this->_Refresh();
}

__cdecl file_status::file_status(const path& _Path) noexcept {
    this->_Init();
    this->_Path = _Path;
    this->_Refresh();
}

// FUNCTION file_status::_Init
void __thiscall file_status::_Init() noexcept {
    this->_Stats     = struct stat();
    this->_Path      = path();
    this->_Attribute = file_attributes::none;
    this->_Perms     = file_permissions::none;
    this->_Type      = file_type::none;
}

// FUNCTION file_status::_Refresh
void __thiscall file_status::_Refresh() noexcept {
    if (_CSTD stat(this->_Path.generic_string().c_str(), &this->_Stats) == 0) { // found file
        if (_CSTD _waccess_s(this->_Path.generic_wstring().c_str(),
            static_cast<int>(file_permissions::readonly) == EACCES)) { // read-only
            this->_Update_permissions(file_permissions::readonly);
        } else { // full access
            this->_Update_permissions(file_permissions::all);
        }

        this->_Update_attribute(file_attributes{ static_cast<file_attributes>(
            _CSTD GetFileAttributesW(this->_Path.generic_wstring().c_str())) });
        

        if ((this->_Attribute & file_attributes::reparse_point) == file_attributes::reparse_point) {
            WIN32_FIND_DATAW _Data;
            const HANDLE _Handle = _CSTD FindFirstFileW(this->_Path.generic_wstring().c_str(), &_Data);
            
            if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
                _Throw_fs_error("failed to get handle", error_type::runtime_error, "_Refresh");
            }

            if (_Data.dwReserved0 == static_cast<DWORD>(file_reparse_tag::mount_point)) { // junction
                this->_Update_type(file_type::junction);
                return;
            }

            if (_Data.dwReserved0 == static_cast<DWORD>(file_reparse_tag::symlink)) {
                this->_Update_type(file_type::symlink);
                return;
            }

            // all others are not supported
        }

        if ((this->_Attribute & file_attributes::directory) == file_attributes::directory) {
            this->_Update_type(file_type::directory);
        } else {
            this->_Update_type(file_type::regular);
        }

        return; // successfully updated status
    } 

    // assign default values if path not found
    this->_Update_attribute(file_attributes::unknown);
    this->_Update_permissions(file_permissions::unknown);
    this->_Update_type(file_type::not_found);
}

// FUNCTION file_status::_Update_attribute
void __cdecl file_status::_Update_attribute(const file_attributes _Newattrib) noexcept {
    this->_Attribute = _Newattrib;
}

// FUNCTION file_status::_Update_access
void __cdecl file_status::_Update_permissions(const file_permissions _Newperms) noexcept {
    this->_Perms = _Newperms;
}

// FUNCTION file_status::_Update_type
void __cdecl file_status::_Update_type(const file_type _Newtype) noexcept {
    this->_Type = _Newtype;
}

// FUNCTION file_status::access
_NODISCARD const file_permissions __thiscall file_status::permissions() const noexcept {
    return this->_Perms;
}

// FUNCTION file_status::attribute
_NODISCARD const file_attributes __thiscall file_status::attribute() const noexcept {
    return this->_Attribute;
}

// FUNCTION file_status::type
_NODISCARD const file_type __thiscall file_status::type() const noexcept {
    return this->_Type;
}

// FUNCTION creation_time
_NODISCARD file_time __cdecl creation_time(const path& _Target) {
    if (!exists(_Target)) { // file/directory not found
        _Throw_fs_error("path not found", error_type::runtime_error, "creation_time");
    }

    struct stat _Stats;
    _CSTD stat(_Target.generic_string().c_str(), &_Stats); // refresh stats

    const tm* _Time = _CSTD localtime(&_Stats.st_ctime);
    return { _Time->tm_year + 1900, _Time->tm_mon + 1, _Time->tm_mday,
        _Time->tm_hour, _Time->tm_min, _Time->tm_sec };
}

// FUNCTION file_size
_NODISCARD size_t __cdecl file_size(const path& _Target,
    const file_attributes _Attributes, const file_flags _Flags) {
    if (!exists(_Target)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "file_size");
    }

    const DWORD _Attr_or_flags = is_directory(_Target) || is_junction(_Target) ?
        static_cast<DWORD>(_Flags) : static_cast<DWORD>(_Attributes);

    const HANDLE _Handle = _CSTD CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<DWORD>(file_access::readonly), static_cast<DWORD>(file_share::read),
        nullptr, static_cast<DWORD>(file_disposition::only_if_exists), _Attr_or_flags, nullptr);

    if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
        _Throw_fs_error("failed to get handle", error_type::runtime_error, "file_size");
    }

    const size_t _Size = static_cast<size_t>(_CSTD GetFileSize(_Handle, nullptr));
    _CSTD CloseHandle(_Handle);
    return _Size;
}

_NODISCARD size_t __cdecl file_size(const path& _Target) {
    return file_size(_Target, file_attributes{ 0 },
        file_flags::backup_semantics | file_flags::open_reparse_point);
}

// FUNCTION exists
_NODISCARD bool __cdecl exists(const file_status _Status) noexcept {
    return _Status.type() != file_type::none && _Status.type() != file_type::not_found;
}

_NODISCARD bool __cdecl exists(const path& _Target) noexcept {
    return exists(file_status(_Target));
}

// FUNCTION hard_link_count
_NODISCARD size_t __cdecl hard_link_count(const path& _Target,
    const file_attributes _Attributes, const file_flags _Flags) { // counts hard links to _Target
    const DWORD _Attr_or_flags = is_directory(_Target) || is_junction(_Target)
        || PathIsDirectoryW(_Target.generic_wstring().c_str()) ?
        static_cast<DWORD>(_Flags) : static_cast<DWORD>(_Attributes);
    
    const HANDLE _Handle = _CSTD CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<DWORD>(file_access::readonly), static_cast<DWORD>(file_share::read),
        nullptr, static_cast<DWORD>(file_disposition::only_if_exists), _Attr_or_flags, nullptr);

    if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
        _Throw_fs_error("failed to get handle", error_type::runtime_error, "hard_link_count");
    }

    FILE_STANDARD_INFO _Info;
    if (!_CSTD GetFileInformationByHandleEx(_Handle, FileStandardInfo, &_Info, sizeof(_Info))) {
        _Throw_fs_error("failed to get informations", error_type::runtime_error, "hard_link_count");
    }
    
    _CSTD CloseHandle(_Handle);
    return static_cast<size_t>(_Info.NumberOfLinks);
}

_NODISCARD size_t __cdecl hard_link_count(const path& _Target) {
    return hard_link_count(_Target, file_attributes::none,
        file_flags::backup_semantics | file_flags::open_reparse_point);
}

// FUNCTION is_directory
_NODISCARD bool __cdecl is_directory(const file_status _Status) noexcept {
    return _Status.type() == file_type::directory;
}

_NODISCARD bool __cdecl is_directory(const path& _Target) noexcept {
    return is_directory(file_status(_Target));
}

// FUNCTION is_empty
_NODISCARD bool __cdecl is_empty(const path& _Target) {
    if (!exists(_Target)) { // path not found
        _Throw_fs_error("path not found", error_type::runtime_error, "is_empty");
    }

    return { is_directory(_Target) || is_junction(_Target) || PathIsDirectoryW(_Target.generic_wstring().c_str())
        ? _CSTD PathIsDirectoryEmptyW(_Target.generic_wstring().c_str()) == 1 : file_size(_Target) == 0 };
}

// FUNCTION is_junction
_NODISCARD bool __cdecl is_junction(const file_status _Status) noexcept {
    return _Status.type() == file_type::junction;
}

_NODISCARD bool __cdecl is_junction(const path& _Target) noexcept {
    return is_junction(file_status(_Target));
}

// FUNCTION is_other
_NODISCARD bool __cdecl is_other(const file_status _Status) noexcept {
    switch (_Status.type()) {
    case file_type::directory:
    case file_type::junction:
    case file_type::not_found:
    case file_type::regular:
    case file_type::symlink:
        return false;
    default:
        return true;
    }
}

_NODISCARD bool __cdecl is_other(const path& _Target) noexcept {
    return is_other(file_status(_Target));
}
 
// FUNCTION is_regular_file
_NODISCARD bool __cdecl is_regular_file(const file_status _Status) noexcept { 
    return _Status.type() == file_type::regular;
}

_NODISCARD bool __cdecl is_regular_file(const path& _Target) noexcept { 
    return is_regular_file(file_status(_Target));
}

// FUNCTION is_symlink
_NODISCARD bool __cdecl is_symlink(const file_status _Status) noexcept {
    return _Status.type() == file_type::symlink;
}

_NODISCARD bool __cdecl is_symlink(const path& _Target) noexcept {
    return is_symlink(file_status(_Target));
}

// FUNCTION last_write_time
_NODISCARD file_time __cdecl last_write_time(const path& _Target) {
    if (!exists(_Target)) { // file/directory not found
        _Throw_fs_error("path not found", error_type::runtime_error, "last_write_time");
    }

    struct stat _Stats;
    stat(_Target.generic_string().c_str(), &_Stats);

    const tm* _Time = _CSTD localtime(&_Stats.st_mtime);
    return { _Time->tm_yday + 1900, _Time->tm_mon + 1, _Time->tm_mday,
        _Time->tm_hour, _Time->tm_min, _Time->tm_sec };
}

_EXPERIMENTAL_END
_FILESYSTEM_END

#pragma warning(pop)
#endif // !_HAS_WINDOWS