// create_remove.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <pch.h>
#include <filesystem.hpp>

#if !_HAS_WINDOWS
#pragma message("The contents of <create_remove.cpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv

_FILESYSTEM_BEGIN
_EXPERIMENTAL_BEGIN
// FUNCTION create_directory
_NODISCARD bool __cdecl create_directory(const path& _Path) { // creates new directory
    if (!_CSTD CreateDirectoryW(_Path.generic_wstring().c_str(), nullptr)) { // failed to create directory 
        _Throw_fs_error("failed to create directory", error_type::runtime_error, "create_directory");
    }

    // if won't throw an exception, will be able to return true
    return true;
}

// FUNCTION create_file
_NODISCARD bool __cdecl create_file(const path& _Path) { // creates new file
    if (exists(_Path)) { // already exists
        _Throw_fs_error("file already exists", error_type::runtime_error, "create_file");
    }

    ofstream _File(_Path.generic_wstring());
    
    if (_File.is_open()) { // close file if is open
        _File.close();
    }

    if (!exists(_Path)) { // failed to create file
        _Throw_fs_error("failed to create file", error_type::runtime_error, "create_file");
    }

    // if won't throw an exception, will be able to return true
    return true;
}

// FUNCTION create_hard_link
_NODISCARD bool __cdecl create_hard_link(const path& _To, const path& _Hardlink) { // creates hard link _Hardlink to _To
    if (!_CSTD CreateHardLinkW(_Hardlink.generic_wstring().c_str(),
        _To.generic_wstring().c_str(), nullptr)) { // failed to create hard link
        _Throw_fs_error("failed to create hard link", error_type::runtime_error, "create_hard_link");
    }

    // if won't throw an exception, will be able to return true
    return true;
}

#pragma warning(push)
#pragma warning(disable : 6385) // C6385: reading incorrect data
// FUNCTION create_junction
_NODISCARD bool __cdecl create_junction(const path& _To, const path& _Junction) {
    if (!exists(_To)) { // directory not found
        _Throw_fs_error("directory not found", error_type::runtime_error, "create_junction");
    }

    // at the beginning _Junction must be created as default directory 
    (void) create_directory(_Junction);

    const HANDLE _Handle = _CSTD CreateFileW(_Junction.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::readonly | file_access::writeonly), 0, nullptr,
        static_cast<unsigned long>(file_disposition::only_if_exists), static_cast<unsigned long>(
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
    int _Len                                     = _CSTD MultiByteToWideChar(static_cast<uint32_t>(code_page::acp), 0,
        path(R"(\??\)" + _To.generic_string() + R"(\)").generic_string().c_str(), -1, _Reparse_buff._Reparse_target, MAX_PATH);
    _Reparse_buff._Reparse_target_maximum_length = static_cast<uint16_t>((_Len--) * sizeof(wchar_t));
    _Reparse_buff._Reparse_target_length         = static_cast<uint16_t>(_Len * sizeof(wchar_t));
    _Reparse_buff._Reparse_data_length           = _Reparse_buff._Reparse_target_length + 12;

    // attach reparse point to _Junction
    unsigned long _Bytes; // returned bytes from DeviceIoControl()

    // don't check result of DeviceIoControl(), because next function will check it better
    _CSTD DeviceIoControl(_Handle, FSCTL_SET_REPARSE_POINT, &_Reparse_buff,
        _Reparse_buff._Reparse_data_length + 8 /* REPARSE_MOUNTPOINT_HEADER_SIZE */, nullptr, 0, &_Bytes, nullptr);
    _CSTD CloseHandle(_Handle);

    if (!is_junction(_Junction)) { // failed to create junction
        _Throw_fs_error("failed to create junction", error_type::runtime_error, "create_junction");
    }

    // if won't throw an exception, will be able to return true
    return true;
}
#pragma warning(pop)

// FUNCTION create_symlink
_NODISCARD bool __cdecl create_symlink(const path& _To, const path& _Symlink, const symlink_flags _Flags) {
    if (!_CSTD CreateSymbolicLinkW(_Symlink.generic_wstring().c_str(),
        _To.generic_wstring().c_str(), static_cast<unsigned long>(_Flags))) { // failed to create symbolic link
        _Throw_fs_error("failed to create symlink", error_type::runtime_error, "create_symlink");
    }

    // if won't throw an exception, will be able to return true
    return true;
}

_NODISCARD bool __cdecl create_symlink(const path& _To, const path& _Symlink) {
    return create_symlink(_To, _Symlink, is_directory(_To) ? symlink_flags::directory | symlink_flags::allow_unprivileged
        : symlink_flags::file | symlink_flags::allow_unprivileged);
}

// FUNCTION remove
_NODISCARD bool __cdecl remove(const path& _Path) { // removes files and directories
    if (is_directory(_Path) ? !_CSTD RemoveDirectoryW(_Path.generic_wstring().c_str())
        : !_CSTD DeleteFileW(_Path.generic_wstring().c_str())) { // failed to remove target
        _Throw_fs_error("failed to remove target", error_type::runtime_error, "remove");
    }

    // if won't throw an exception, will be able to return true
    return true;
}

// FUNCTION remove_all
_NODISCARD bool __cdecl remove_all(const path& _Path) {
    if (!is_directory(_Path) && !is_junction(_Path) && !PathIsDirectoryW(_Path.generic_wstring().c_str())) { // directory not found
        _Throw_fs_error("directory not found", error_type::runtime_error, "remove_all");
    }

    if (is_empty(_Path)) { // if empty, remove _Path and don't do anything else
        return remove(_Path);
    }

    const auto _All = directory_data(_Path).total(); // total count of files, directories and others in _Path

    for (const auto& _Elem : _All) { // remove one by one
        (void) remove(_Path + R"(\)" + _Elem); // requires full path
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
        static_cast<unsigned long>(file_access::readonly | file_access::writeonly), 0, nullptr,
        static_cast<unsigned long>(file_disposition::only_if_exists), static_cast<unsigned long>(
            file_flags::backup_semantics | file_flags::open_reparse_point), nullptr);

    if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
        _Throw_fs_error("failed to get handle", error_type::runtime_error, "remove_junction");
    }

    unsigned char _Buff[8 /* REPARSE_MOUNTPOINT_HEADER_SIZE */];
    reparse_mountpoint_data_buffer& _Reparse_buff = reinterpret_cast<reparse_mountpoint_data_buffer&>(_Buff);
    unsigned long _Returned; // returned bytes from DeviceIoControl()

    // set new information to _Target
    _CSTD memset(_Buff, 0, sizeof(_Buff));
    _Reparse_buff._Reparse_tag = static_cast<unsigned long>(file_reparse_tag::mount_point);

    if (!_CSTD DeviceIoControl(_Handle, FSCTL_DELETE_REPARSE_POINT, &_Reparse_buff, 8 /* REPARSE_MOUNTPOINT_HEADER_SIZE */,
        nullptr, 0, &_Returned, nullptr) || is_junction(_Target)) { // failed to remove junction
        _Throw_fs_error("failed to remove junction", error_type::runtime_error, "remove_junction");
    }

    _CSTD CloseHandle(_Handle);

    // if won't throw an exception, will be able to return true
    return true;
}

// FUNCTION remove_line
_NODISCARD bool __cdecl remove_line(const path& _Target, const size_t _Line) { // removes _Line line from _Target
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
            (void) write_back(_Target, _All[_Idx]);
        }
    }

    if (lines_count(_Target) != _Expected) { // failed to remove line
        _Throw_fs_error("failed to remove line", error_type::runtime_error, "remove_line");
    }

    // if won't throw an exception, will be able to return true
    return true;
}

// FUNCTION rename
_NODISCARD bool __cdecl rename(const path& _Old, const path& _New, const rename_options _Flags) { // renames _Old to _New
    if (!_CSTD MoveFileExW(_Old.generic_wstring().c_str(), _New.generic_wstring().c_str(),
        static_cast<unsigned long>(_Flags))) { // failed to rename
        _Throw_fs_error("failed to rename", error_type::runtime_error, "rename");
    }

    // if won't throw an excepyion, will be able to return true
    return true;
}

_NODISCARD bool __cdecl rename(const path& _Old, const path& _New) {
    return rename(_Old, _New, rename_options::copy | rename_options::replace);
}

_EXPERIMENTAL_END
_FILESYSTEM_END

#endif // !_HAS_WINDOWS