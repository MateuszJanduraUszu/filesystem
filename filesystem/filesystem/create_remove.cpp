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
_NODISCARD bool __cdecl remove_all(const path& _Path, size_t _Count) {
    return false;
}

_NODISCARD bool __cdecl remove_all(const path& _Path) {
    return false;
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

#endif // !_HAS_WINDOWS