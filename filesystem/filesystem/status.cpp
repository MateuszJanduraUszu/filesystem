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

// FUNCTION directory_data::directory_data
__thiscall directory_data::directory_data() noexcept {
    this->_Init();
    this->_Refresh(); // get the latest informaions
}

__cdecl directory_data::directory_data(const path& _Path) noexcept {
    this->_Init();
    this->_Path = _Path; // update current working path
    this->_Refresh(); // get the latest informaions
}

// FUNCTION directory_data::_Init
void __thiscall directory_data::_Init() noexcept {
    this->_Path = {};

    for (size_t _Idx = 0; _Idx < this->_Counts.size(); ++_Idx) {
        this->_Counts[_Idx] = 0;
        this->_Names[_Idx]  = {};
    }
}

// FUNCTION directory_data::_Refresh
void __thiscall directory_data::_Refresh() noexcept {
    if (!exists(this->_Path)) { // directory not found
        _Throw_fs_error("directory not found", error_type::runtime_error, "_Refresh");
    }

    // _Path must be directory
    if (!is_directory(this->_Path) && !is_junction(this->_Path)
        && !_CSTD PathIsDirectoryW(this->_Path.generic_wstring().c_str())) { // expected directory
        _Throw_fs_error("expected directory", error_type::runtime_error, "_Refresh");
    }

    if (!is_empty(this->_Path)) {
        WIN32_FIND_DATAW _Data;
        const HANDLE _Handle = _CSTD FindFirstFileExW(path(this->_Path + LR"(\*)").generic_wstring().c_str(),
            FindExInfoBasic, &_Data, FindExSearchNameMatch, nullptr, 0);

        if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
            _Throw_fs_error("failed to get handle", error_type::runtime_error, "_Refresh");
        }

        vector<path> _All;

        do { // get all types, they will be separated later
            _All.push_back(static_cast<const wchar_t*>(_Data.cFileName));
        } while (_CSTD FindNextFileW(_Handle, &_Data));
        _CSTD FindClose(_Handle);

        this->_Reset(); // clear everything
        path _Precise; // creates full path to _All[_Idx]

        for (const auto& _Elem : _All) {
            if (_Elem != "." && _Elem != "..") { // skip dots
                _Precise = this->_Path + LR"(\)" + _Elem;

                if (is_directory(_Precise)) { // directory
                    this->_Names[0].push_back(_Elem);
                } else if (is_junction(_Precise)) { // junction (as directory)
                    this->_Names[1].push_back(_Elem);
                } else if (is_other(_Precise)) { // other
                    this->_Names[2].push_back(_Elem);
                } else if (is_regular_file(_Precise)) { // regular file
                    this->_Names[3].push_back(_Elem);
                } else if (is_symlink(_Precise)) { // symlink (directory or file)
                    this->_Names[4].push_back(_Elem);
                }

                this->_Names[5].push_back(_Elem); // _Names[5] (total) takes every type
            }
        }

        for (size_t _Idx = 0; _Idx < this->_Names.size(); ++_Idx) {
            this->_Counts[_Idx] = this->_Names[_Idx].size();
        }

        return; // that's all if _Path isn't empty
    }

    // if _Path is empty, leave variables without value
    this->_Reset();
}

// FUNCTION directory_data::_Reset
void __thiscall directory_data::_Reset() noexcept {
    // _Init() cannot be used here, because removes value from _Path
    for (size_t _Idx = 0; _Idx < this->_Counts.size(); ++_Idx) {
        this->_Counts[_Idx] = 0;
        this->_Names[_Idx].clear();
    }
}

// FUNCTION directory_data::directories
_NODISCARD const vector<path>& __thiscall directory_data::directories() const noexcept {
    return this->_Names[0];
}

// FUNCTION directory_data::directories_count
_NODISCARD const size_t __thiscall directory_data::directories_count() const noexcept {
    return this->_Counts[0];
}

// FUNCTION directory_data::junctions
_NODISCARD const vector<path>& __thiscall directory_data::junctions() const noexcept {
    return this->_Names[1];
}

// FUNCTION directory_data::junctions_count
_NODISCARD const size_t __thiscall directory_data::junctions_count() const noexcept {
    return this->_Counts[1];
}

// FUNCTION directory_data::others
_NODISCARD const vector<path>& __thiscall directory_data::others() const noexcept {
    return this->_Names[2];
}

// FUNCTION directory_data::others_count
_NODISCARD const size_t __thiscall directory_data::others_count() const noexcept {
    return this->_Counts[2];
}

// FUNCTION directory_data::regular_files
_NODISCARD const vector<path>& __thiscall directory_data::regular_files() const noexcept {
    return this->_Names[3];
}

// FUNCTION directory_data::regular_files_count
_NODISCARD const size_t __thiscall directory_data::regular_files_count() const noexcept {
    return this->_Counts[3];
}

// FUNCTION directory_data::symlinks
_NODISCARD const vector<path>& __thiscall directory_data::symlinks() const noexcept {
    return this->_Names[4];
}

// FUNCTION directory_data::symlinks_count
_NODISCARD const size_t __thiscall directory_data::symlinks_count() const noexcept {
    return this->_Counts[4];
}

// FUNCTION directory_data::total
_NODISCARD const vector<path>& __thiscall directory_data::total() const noexcept {
    return this->_Names[5];
}

// FUNCTION directory_data::total_count
_NODISCARD const size_t __thiscall directory_data::total_count() const noexcept {
    return this->_Counts[5];
}

// FUNCTION change_attributes
_NODISCARD bool __cdecl change_attributes(const path& _Target, const file_attributes _Newattr) {
    if (!exists(_Target)) { // file/directory not found
        _Throw_fs_error("path not found", error_type::runtime_error, "change_attributes");
    }

    const bool _Result = _CSTD SetFileAttributesW(_Target.generic_wstring().c_str(),
        static_cast<DWORD>(_Newattr)) != 0;

    if (!_Result) { // failed to change attributes
        _Throw_fs_error("failed to change attributes", error_type::runtime_error, "change_attributes");
    }

    return _Result;
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
        || _CSTD PathIsDirectoryW(_Target.generic_wstring().c_str()) ?
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

    return { is_directory(_Target) || is_junction(_Target) || _CSTD PathIsDirectoryW(_Target.generic_wstring().c_str())
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
    _CSTD stat(_Target.generic_string().c_str(), &_Stats);

    const tm* _Time = _CSTD localtime(&_Stats.st_mtime);
    return { _Time->tm_yday + 1900, _Time->tm_mon + 1, _Time->tm_mday,
        _Time->tm_hour, _Time->tm_min, _Time->tm_sec };
}

_EXPERIMENTAL_END
_FILESYSTEM_END

#pragma warning(pop)
#endif // !_HAS_WINDOWS