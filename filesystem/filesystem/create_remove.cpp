// create_remove.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <pch.h>
#include <filesystem.hpp>

#if !_HAS_WINDOWS
#pragma message("The contents of <create_remove.cpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv
#pragma warning(push)
#pragma warning(disable : 6385) // C6385: reading incorrect data

_FILESYSTEM_BEGIN
_EXPERIMENTAL_BEGIN
// FUNCTION create_directory
_NODISCARD bool __cdecl create_directory(const path& _Path) {
    const bool _Result = _CSTD CreateDirectoryW(_Path.generic_wstring().c_str(), nullptr) != 0;

    if (!_Result) { // failed to create directory
        _Throw_fs_error("failed to create directory", error_type::runtime_error, "create_directory");
    }

    return _Result;
}

// FUNCTION create_file
_NODISCARD bool __cdecl create_file(const path& _Path) {
    if (exists(_Path)) { // already exists
        _Throw_fs_error("file already exists", error_type::runtime_error, "create_file");
    }

    ofstream _File(_Path.generic_wstring());
    
    if (_File.is_open()) { // close file if is open
        _File.close();
    }

    const bool _Result = exists(_Path);

    if (!_Result) { // failed to create file
        _Throw_fs_error("failed to create file", error_type::runtime_error, "create_file");
    }

    return _Result;
}

// FUNCTION create_hard_link
_NODISCARD bool __cdecl create_hard_link(const path& _To, const path& _Hardlink) {
    const bool _Result = _CSTD CreateHardLinkW(_Hardlink.generic_wstring().c_str(),
        _To.generic_wstring().c_str(), nullptr) != 0;

    if (!_Result) { // failed to create hard link
        _Throw_fs_error("failed to create hard link", error_type::runtime_error, "create_hard_link");
    }

    return _Result;
}

// FUNCTION create_junction
_NODISCARD bool __cdecl create_junction(const path& _To, const path& _Junction) {
    if (!exists(_To)) { // directory not found
        _Throw_fs_error("directory not found", error_type::runtime_error, "create_junction");
    }

    // at the beginning _Junction must be created as default directory 
    create_directory(_Junction);

    const HANDLE _Handle = _CSTD CreateFileW(_Junction.generic_wstring().c_str(),
        static_cast<DWORD>(file_access::readonly | file_access::writeonly), 0, nullptr,
        static_cast<DWORD>(file_disposition::only_if_exists), static_cast<DWORD>(
            file_flags::backup_semantics | file_flags::open_reparse_point), nullptr);

    if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
        _Throw_fs_error("failed to get handle", error_type::runtime_error, "create_junction");
    }

    unsigned char _Buff[sizeof(reparse_data_buffer) + MAX_PATH * sizeof(wchar_t)];
    reparse_mountpoint_data_buffer& _Reparse_buff = reinterpret_cast<reparse_mountpoint_data_buffer&>(_Buff);

    // set informations about reparse point
    _CSTD memset(_Buff, 0, sizeof(_Buff));

    // _Junction must containts prefix ("\??\") and sufix ("\"), to create junction successfully
    _Reparse_buff._Reparse_tag                   = static_cast<unsigned long>(file_reparse_tag::mount_point);
    int _Len                                     = _CSTD MultiByteToWideChar(static_cast<UINT>(code_page::acp), 0,
        path(R"(\??\)" + _To.generic_string() + R"(\)").generic_string().c_str(), -1, _Reparse_buff._Reparse_target, MAX_PATH);
    _Reparse_buff._Reparse_target_maximum_length = static_cast<uint16_t>((_Len--) * sizeof(wchar_t));
    _Reparse_buff._Reparse_target_length         = static_cast<uint16_t>(_Len * sizeof(wchar_t));
    _Reparse_buff._Reparse_data_length           = _Reparse_buff._Reparse_target_length + 12;

    // attach reparse point to _Junction
    unsigned long _Returned; // returned bytes
    _CSTD DeviceIoControl(_Handle, FSCTL_SET_REPARSE_POINT, &_Reparse_buff,
        _Reparse_buff._Reparse_data_length + 8, nullptr, 0, &_Returned, nullptr);
    _CSTD CloseHandle(_Handle);

    if (!is_junction(_Junction)) { // failed to create junction
        _Throw_fs_error("failed to create junction", error_type::runtime_error, "create_junction");
    }

    // if won't throw an exception, will be able to return true
    return true;
}

// FUNCTION create_symlink
_NODISCARD bool __cdecl create_symlink(const path& _To, const path& _Symlink, const symlink_flags _Flags) {
    const bool _Result = _CSTD CreateSymbolicLinkW(_Symlink.generic_wstring().c_str(),
        _To.generic_wstring().c_str(), static_cast<DWORD>(_Flags)) != 0;

    if (!_Result) { // failed to create symbolic link
        _Throw_fs_error("failed to create symlink", error_type::runtime_error, "create_symlink");
    }

    return _Result;
}

_NODISCARD bool __cdecl create_symlink(const path& _To, const path& _Symlink) {
    const auto _Flags = is_directory(_To) ? symlink_flags::directory | symlink_flags::allow_unprivileged
        : symlink_flags::file | symlink_flags::allow_unprivileged;

    return create_symlink(_To, _Symlink, _Flags);
}

// FUNCTION remove
_NODISCARD bool __cdecl remove(const path& _Path) { // removes files and directories
    const bool _Result = is_directory(_Path) ? _CSTD RemoveDirectoryW(_Path.generic_wstring().c_str()) != 0
        : _CSTD DeleteFileW(_Path.generic_wstring().c_str()) != 0;

    if (!_Result) { // failed to remove file
        _Throw_fs_error("failed to remove file", error_type::runtime_error, "remove");
    }

    return _Result;
}

// FUNCTION remove_all
_NODISCARD bool __cdecl remove_all(const path& _Path) {
    if (!is_directory(_Path) && !PathIsDirectoryW(_Path.generic_wstring().c_str())) { // directory not found
        _Throw_fs_error("directory not found", error_type::runtime_error, "remove_all");
    }

    if (is_empty(_Path)) { // if empty, remove _Path and don't do anything else
        return remove(_Path);
    }

    const auto _All = directory_data(_Path).total(); // total count of files, directories and others in _Path

    for (const auto& _Elem : _All) { // remove one by one
        remove(_Path + R"(\)" + _Elem); // requires full path
    }

    // remove this directory as well and don't check remove result, because remove() will check it
    return remove(_Path);
}

// FUNCTION remove_junction
_NODISCARD bool __cdecl remove_junction(const path& _Target) {
    if (!is_junction(_Target)) { // _Target must be a junction
        _Throw_fs_error("expected junction", error_type::runtime_error, "remove_junction");
    }

    const HANDLE _Handle = _CSTD CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<DWORD>(file_access::readonly | file_access::writeonly), 0, nullptr,
        static_cast<DWORD>(file_disposition::only_if_exists), static_cast<DWORD>(
            file_flags::backup_semantics | file_flags::open_reparse_point), nullptr);

    if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
        _Throw_fs_error("failed to get handle", error_type::runtime_error, "remove_junction");
    }

    unsigned char _Buff[8];
    reparse_mountpoint_data_buffer& _Reparse_buff = reinterpret_cast<reparse_mountpoint_data_buffer&>(_Buff);
    unsigned long _Returned; // returned bytes

    // set new information to _Target
    _CSTD memset(_Buff, 0, sizeof(_Buff));
    _Reparse_buff._Reparse_tag = static_cast<unsigned long>(file_reparse_tag::mount_point);

    if (!_CSTD DeviceIoControl(_Handle, FSCTL_DELETE_REPARSE_POINT, &_Reparse_buff, 8,
        nullptr, 0, &_Returned, nullptr) || is_junction(_Target)) { // failed to remove junction
        _Throw_fs_error("failed to remove junction", error_type::runtime_error, "remove_junction");
    }
    _CSTD CloseHandle(_Handle);

    // if won't throw an exception, will be able to return true
    return true;
}

// FUNCTION remove_line
_NODISCARD bool __cdecl remove_line(const path& _Target, const size_t _Line) {
    if (!exists(_Target)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "remove_line");
    }

    const auto _All      = read_all(_Target);
    const auto _Count    = _All.size();
    const auto _Expected = _Count - 1; // count of lines after removed

    if (_Line < 1 || _Line > _Count || _Expected < 0) { // invalid line
        _Throw_fs_error("invalid line", error_type::invalid_argument, "remove_line");
    }

    // clear _Target and write to him the newest content
    ofstream _File;
    _File.open(_Target.generic_string());
    _File.clear();
    _File.close();

    for (size_t _Idx = 0; _Idx < _Count; ++_Idx) {
        if (_Idx + 1 != _Line) { // skip removed line
            write_back(_Target, _All[_Idx]);
        }
    }

    if (lines_count(_Target) != _Expected) { // failed to remove line
        _Throw_fs_error("failed to remove line", error_type::runtime_error, "remove_line");
    }

    // if won't throw an exception, will be able to return true
    return true;
}

// FUNCTION rename
_NODISCARD bool __cdecl rename(const path& _Old, const path& _New, const rename_options _Flags) {
    const bool _Result = MoveFileExW(_Old.generic_wstring().c_str(),
        _New.generic_wstring().c_str(), static_cast<DWORD>(_Flags)) != 0;

    if (!_Result) {
        _Throw_fs_error("failed to rename", error_type::runtime_error, "rename");
    }

    return _Result;
}

_NODISCARD bool __cdecl rename(const path& _Old, const path& _New) {
    constexpr auto _Flags = rename_options::copy | rename_options::replace;
    return rename(_Old, _New, _Flags);
}

_EXPERIMENTAL_END
_FILESYSTEM_END

#pragma warning(pop)
#endif // !_HAS_WINDOWS