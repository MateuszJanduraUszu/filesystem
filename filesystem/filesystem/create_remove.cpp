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
// FUNCTION copy
_NODISCARD bool __cdecl copy(const path& _From, const path& _To, const copy_options _Options) {
    if (!exists(_From)) { // path not found
        _Throw_fs_error("path not found", error_type::runtime_error, "copy");
    }

    // error cases should be checked as first
    if (is_other(_From) || is_other(_To)) { // not supported operation
        _Throw_fs_error("operation not supported", error_type::invalid_argument, "copy");
    }

    // _From and _To must be the same type (directory/not directory)
    if (is_directory(_From) && is_regular_file(_To)) { // cannot copy directory to file
        _Throw_fs_error("invalid operation", error_type::invalid_argument, "copy");
    }

    if ((_Options & copy_options::cannot_exists) == copy_options::cannot_exists && exists(_To)) { // _To shouldn't exists, but exists
        _Throw_fs_error("already exists", error_type::runtime_error, "copy");
    }

    if ((_Options & copy_options::cannot_be_link) == copy_options::cannot_be_link
        && (is_junction(_From) || is_symlink(_From))) { // _From shouldn't be a link, but it is
        _Throw_fs_error("source is a link", error_type::runtime_error, "copy");
    }

    if (_Options == copy_options::none) { // try to do it with default CopyFileW() or SHFileOperationW()
        if (!_CSTD CopyFileW(_From.generic_wstring().c_str(), _To.generic_wstring().c_str(), true)) { // failed to copy target
            if (_CSTD GetLastError() == ERROR_ACCESS_DENIED) { // _From is directory
                const auto& _Src  = _From.generic_wstring();
                const auto& _Dest = _To.generic_wstring();

                // Last character from source must be 0. Without it SHFileOperationW() will create empty directory in target path
                const_cast<wstring&>(_Src).push_back(L'\0');

                SHFILEOPSTRUCTW _Ops;
                _Ops.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;
                _Ops.hwnd   = nullptr; // never used in this case
                _Ops.pFrom  = _Src.c_str();
                _Ops.pTo    = _Dest.c_str();
                _Ops.wFunc  = FO_COPY;

                if (_CSTD SHFileOperationW(&_Ops)) { // failed to copy directory replacing existing
                    _Throw_fs_error("failed to copy directory", error_type::runtime_error, "copy");
                }                
            } else { // unknown error
                _Throw_fs_error("failed to copy file", error_type::runtime_error, "copy");
            }
        }

        // if won't throw an exception, will be able to return true;
        return true;
    }

    // check for copying links
    if ((_Options & copy_options::copy_junction) == copy_options::copy_junction && is_junction(_From)) { // copy _From junction to _To
        // if contains copy_options::replace flag, remove _To, if exists and copy expected
        if ((_Options & copy_options::replace) == copy_options::replace) {
            (void) remove_all(_To);
        }
        
        return copy_junction(_From, _To);
    }

    if ((_Options & copy_options::copy_symlink) == copy_options::copy_symlink && is_symlink(_From)) { // copy _From symbolic link to _To
        // if contains copy_options::replace flag, remove _To, if exists and copy expected, check type
        if (_CSTD PathIsDirectoryW(_From.generic_wstring().c_str())) {
            if ((_Options & copy_options::replace) == copy_options::replace && exists(_To)) {
                (void) remove_all(_To);
            }
        } else { // regular file
            if ((_Options & copy_options::overwrite) == copy_options::overwrite && exists(_To)
                && (_Options & copy_options::replace) != copy_options::replace) { // clear symlink and overwrite
                (void) clear(_To);

                for (const auto& _Elem : read_all(_From)) {
                    (void) write_back(_To, _Elem);
                }

                if (read_all(_From) != read_all(_To)) { // failed to copy file
                    _Throw_fs_error("failed to copy file", error_type::runtime_error, "copy");
                }
                
                // if won't throw an exception, will be able to return true
                return true;
            }

            if ((_Options & copy_options::replace) == copy_options::replace && exists(_To)) { // remove existing
                (void) remove(_To);
            }
        }
        
        return copy_symlink(_From, _To);
    }

    // check for defined types
    if (is_directory(_From)) { // directory cases
        if ((_Options & copy_options::create_junction) == copy_options::create_junction
            && (_Options & copy_options::create_symlink) != copy_options::create_symlink) {
            // if contains copy_options::replace flag, remove _To if exists and replace with expected
            if ((_Options & copy_options::replace) == copy_options::replace && exists(_To)) {
                (void) remove_all(_To);
            }

            return create_junction(_From, _To);
        }

        if ((_Options & copy_options::create_symlink) == copy_options::create_symlink) {
            // if contains copy_options::replace flag, remove _To if exists and replace with expected
            if ((_Options & copy_options::replace) == copy_options::replace && exists(_To)) {
                (void) remove_all(_To);
            }

            return create_symlink(_From, _To);
        }

        if ((_Options & copy_options::replace) == copy_options::replace
            && exists(_To)) { // remove existing and copy from source to target
            (void) remove_all(_To); // remove _To as well
            
            const auto& _Src  = _From.generic_wstring();
            const auto& _Dest = _To.generic_wstring();

            // Last char from source must be 0. Without it SHFileOperationW() will create empty directory in target path.
            const_cast<wstring&>(_Src).push_back(L'\0');
            
            SHFILEOPSTRUCTW _Ops;
            _Ops.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;
            _Ops.hwnd   = nullptr; // never used in this case
            _Ops.pFrom  = _Src.c_str();
            _Ops.pTo    = _Dest.c_str();
            _Ops.wFunc  = FO_COPY;

            if (_CSTD SHFileOperationW(&_Ops)) { // failed to copy directory replacing existing
                _Throw_fs_error("failed to copy directory", error_type::runtime_error, "copy");
            }

            // if won't throw an exception, will be able to return true
            return true;
        }

        // there's no more supported operations
        _Throw_fs_error("operation not supported", error_type::invalid_argument, "copy");
    }

    if (is_regular_file(_From)) { // regular file cases
        if ((_Options & copy_options::create_hard_link) == copy_options::create_hard_link
            && (_Options & copy_options::create_symlink) != copy_options::create_symlink) { // without second instruction, creates only hard link
            // The copy_options::overwrite flag is difference case.
            // We have to clear existing hard link and write to him content from _From.
            if ((_Options & copy_options::overwrite) == copy_options::overwrite && exists(_To)
                && (_Options & copy_options::replace) != copy_options::replace) {
                (void) clear(_To);

                for (const auto& _Elem : read_all(_To)) {
                    (void) write_back(_To, _Elem);
                }

                if (read_all(_From) != read_all(_To)) { // failed to copy file creating hard link 
                    _Throw_fs_error("failed to copy file", error_type::runtime_error, "copy");
                }

                // if won't throw an exception, will be able to return true
                return true;
            }

            // if contains copy_options::replace flag, remove _To, if exists and replace with excepted
            if ((_Options & copy_options::replace) == copy_options::replace && exists(_To)) {
                (void) remove(_To);
            }

            return create_hard_link(_From, _To);
        }

        if ((_Options & copy_options::create_symlink) == copy_options::create_symlink) {
            // The copy_options::overwrite flag is difference case.
            // We have to clear existing symbolic link and write to him content from _From.
            if ((_Options & copy_options::overwrite) == copy_options::overwrite && exists(_To)
                && (_Options & copy_options::replace) != copy_options::replace) {
                (void) clear(_To);

                for (const auto& _Elem : read_all(_To)) {
                    (void) write_back(_To, _Elem);
                }

                if (read_all(_From) != read_all(_To)) { // failed to copy file creating hard link 
                    _Throw_fs_error("failed to copy file", error_type::runtime_error, "copy");
                }

                // if won't throw an exception, will be able to return true
                return true;
            }

            // if contains copy_options::replace flag, remove _To, if exists and replace with excepted
            if ((_Options & copy_options::replace) == copy_options::replace && exists(_To)) {
                (void) remove(_To);
            }

            return create_symlink(_From, _To);
        }

        if ((_Options & copy_options::overwrite) == copy_options::overwrite && exists(_To)
            && (_Options & copy_options::replace) != copy_options::replace) { // replace old _To content
            (void) clear(_To);

            for (const auto& _Elem : read_all(_From)) {
                (void) write_back(_To, _Elem);
            }

            if (read_all(_To) != read_all(_From)) { // failed to copy file with replace attribute
                _Throw_fs_error("failed to copy file", error_type::runtime_error, "copy");
            }

            // if won't throw an exception, will be able to return true
            return true;
        }

        if ((_Options & copy_options::replace) == copy_options::replace && exists(_To)) { // remove old file and copy from source path
            if (!_CSTD CopyFileW(_From.generic_wstring().c_str(), _To.generic_wstring().c_str(), false)) { // failed to copy file
                _Throw_fs_error("failed to copy file", error_type::runtime_error, "copy");
            }

            // if won't throw an exception, will be able to return true
            return true;
        }

        // there's no more supported operations
        _Throw_fs_error("operation not supported", error_type::invalid_argument, "copy");
    }

    // there's no more supported operations,
    _Throw_fs_error("operation not supported", error_type::invalid_argument, "copy");
    
    // if everything went correctly this block won't be executed
    return false;
}

_NODISCARD bool __cdecl copy(const path& _From, const path& _To) {
    return copy(_From, _To, copy_options::none);
}

// FUNCTION copy_file
_NODISCARD bool __cdecl copy_file(const path& _From, const path& _To, const bool _Replace) { // if _Replace is true, clears file
    if (!exists(_From)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "copy_file");
    }

    if (is_empty(_From)) { // nothing to do
        return true;
    }

    if (!exists(_To)) { // if not found _To, create new file
        (void) create_file(_To);
    }

    if (_Replace) { // clear _To and write to him content from _From
        (void) clear(_To);

        for (const auto _Elem : read_all(_From)) {
            (void) write_back(_To, _Elem);
        }

        if (read_all(_From) != read_all(_To)) { // failed to copy file
            _Throw_fs_error("failed to copy file", error_type::runtime_error, "copy_file");
        }

        // if won't throw an exception, will be able to return true
        return true;
    } else { // don't touch old content
        const auto& _Src(read_all(_From));
        const auto& _Result(read_all(_To)); // content from _From and _To
        
        const_cast<vector<path>&>(_Result).insert(_Result.end(), _Src.begin(), _Src.end());

        for (const auto& _Elem : read_all(_From)) {
            (void) write_back(_To, _Elem);
        }

        if (read_all(_To) != _Result) { // failed to copy file
            _Throw_fs_error("failed to copy file", error_type::runtime_error, "copy_file");
        }

        // if won't throw an exception, will be able to return true
        return true;
    }
}

// FUNCTION copy_junction
_NODISCARD bool __cdecl copy_junction(const path& _Junction, const path& _Newjunction) {
    if (!is_junction(_Junction)) { // junction not found
        _Throw_fs_error("junction not found", error_type::runtime_error, "copy_junction");
    }

    return create_junction(read_junction(_Junction), _Newjunction);
}

// FUNCTION copy_symlink
_NODISCARD bool __cdecl copy_symlink(const path& _Symlink, const path& _Newsymlink) {
    if (!is_symlink(_Symlink)) { // symbolic link not found
        _Throw_fs_error("symbolic link not found", error_type::runtime_error, "copy_symlink");
    }

    return create_symlink(read_symlink(_Symlink), _Newsymlink);
}

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

    if (!is_directory(_To)) { // junction applies only to directories
        _Throw_fs_error("source path must be a directory", error_type::runtime_error, "create_junction");
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
    if (!is_directory(_Path) && !is_junction(_Path)
        && !_CSTD PathIsDirectoryW(_Path.generic_wstring().c_str())) { // directory not found
        _Throw_fs_error("directory not found", error_type::runtime_error, "remove_all");
    }

    if (is_empty(_Path)) { // if empty, remove _Path and don't do anything else
        return remove(_Path);
    }

    const auto& _Src = _Path.generic_wstring();

    // without 0 on last position, SHFileOperationW won't work correctly
    const_cast<wstring&>(_Src).push_back(L'\0');

    SHFILEOPSTRUCTW _Ops;
    _Ops.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    _Ops.hwnd   = nullptr; // never used in this case
    _Ops.pFrom  = _Src.c_str();
    _Ops.wFunc  = FO_DELETE;

    if (_CSTD SHFileOperationW(&_Ops) != 0) { // failed to remove directory
        _Throw_fs_error("failed to remove directory", error_type::runtime_error, "remove_all");
    }

    // SHFileOperationW() removes base directory as well, so there's nothing to do more
    return true;
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