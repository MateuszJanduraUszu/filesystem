// create_remove.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <filesystem_pch.hpp>
#include <filesystem.hpp>

#if !_HAS_WINDOWS
#pragma message("The contents of <create_remove.cpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv

_FILESYSTEM_BEGIN
// FUNCTION copy
_NODISCARD bool copy(const path& _From, const path& _To, const copy_options _Options) {
    // should be checked before any operation
    _FILESYSTEM_VERIFY(exists(_From), "target not found", error_type::runtime_error);
    _FILESYSTEM_VERIFY(is_other(_From) || is_other(_To), "operation not supported", error_type::invalid_argument);
    _FILESYSTEM_VERIFY(_Is_directory(_From) && is_regular_file(_To), "invalid operation", error_type::invalid_argument);
    _FILESYSTEM_VERIFY((_Options & copy_options::cannot_exists) == copy_options::cannot_exists
        && exists(_To), "target already exists", error_type::runtime_error);
    _FILESYSTEM_VERIFY((_Options & copy_options::cannot_be_link) == copy_options::cannot_be_link
        && (is_junction(_From) || is_symlink(_From)), "target is a link", error_type::runtime_error);
    if (_Options == copy_options::none) { // try to do it with default CopyFileW() or SHFileOperationW()
        if (!CopyFileW(_From.generic_wstring().c_str(), _To.generic_wstring().c_str(), true)) { // failed to copy target
            if (GetLastError() == ERROR_ACCESS_DENIED) { // _From is directory
                const auto& _Src  = _From.generic_wstring();
                const auto& _Dest = _To.generic_wstring();

                // Last character from source must be 0. Without it SHFileOperationW() will create empty directory in target path
                const_cast<wstring&>(_Src).push_back(L'\0');

                SHFILEOPSTRUCTW _Ops = SHFILEOPSTRUCTW();
                _Ops.fFlags          = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;
                _Ops.hwnd            = nullptr; // never used in this case
                _Ops.pFrom           = _Src.c_str();
                _Ops.pTo             = _Dest.c_str();
                _Ops.wFunc           = FO_COPY;
                _FILESYSTEM_VERIFY(SHFileOperationW(&_Ops) == 0, "failed to copy the directory", error_type::runtime_error);
            } else { // unknown error
                _Throw_fs_error("failed to copy file", error_type::runtime_error, "copy");
            }
        }

        return true;
    }

    // check for copying links
    if ((_Options & copy_options::copy_junction) == copy_options::copy_junction && is_junction(_From)) { // copy _From junction to _To
        // if contains copy_options::replace flag, remove _To, if exists and copy expected
        if ((_Options & copy_options::replace) == copy_options::replace && exists(_To)) {
            (void) remove_all(_To);
        }
        
        return copy_junction(_From, _To);
    }

    if ((_Options & copy_options::copy_symlink) == copy_options::copy_symlink && is_symlink(_From)) { // copy _From symbolic link to _To
        // if contains copy_options::replace flag, remove _To, if exists and copy expected, check type
        if (_Is_directory(_From)) {
            if ((_Options & copy_options::replace) == copy_options::replace && exists(_To)) {
                (void) remove_all(_To);
            }
        } else { // regular file
            if ((_Options & copy_options::overwrite) == copy_options::overwrite && exists(_To)
                && (_Options & copy_options::replace) != copy_options::replace) { // clear symlink and overwrite
                (void) clear(_To);
                for (const auto& _Elem : read_all(_From)) {
                    (void) write_back(_To, _Elem.c_str());
                }

                _FILESYSTEM_VERIFY(read_all(_From) == read_all(_To), "failed to copy the file", error_type::runtime_error);
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
            
            SHFILEOPSTRUCTW _Ops = SHFILEOPSTRUCTW();
            _Ops.fFlags          = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;
            _Ops.hwnd            = nullptr; // never used in this case
            _Ops.pFrom           = _Src.c_str();
            _Ops.pTo             = _Dest.c_str();
            _Ops.wFunc           = FO_COPY;
            _FILESYSTEM_VERIFY(SHFileOperationW(&_Ops) == 0, "failed to copy the directory", error_type::runtime_error);
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
                    (void) write_back(_To, _Elem.c_str());
                }

                _FILESYSTEM_VERIFY(read_all(_From) == read_all(_To), "failed to copy the file", error_type::runtime_error);
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
                    (void) write_back(_To, _Elem.c_str());
                }

                _FILESYSTEM_VERIFY(read_all(_From) == read_all(_To), "failed to copy the file", error_type::runtime_error);
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
                (void) write_back(_To, _Elem.c_str());
            }

            _FILESYSTEM_VERIFY(read_all(_From) == read_all(_To), "failed to copy the file", error_type::runtime_error);
            return true;
        }

        if ((_Options & copy_options::replace) == copy_options::replace && exists(_To)) { // remove old file and copy from source path
            _FILESYSTEM_VERIFY(CopyFileW(_From.generic_wstring().c_str(), _To.generic_wstring().c_str(),
                false), "failed to copy the file", error_type::runtime_error);
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

_NODISCARD bool copy(const path& _From, const path& _To) {
    return copy(_From, _To, copy_options::none);
}

// FUNCTION copy_file
_NODISCARD bool copy_file(const path& _From, const path& _To, const bool _Replace) { // if _Replace is true, clears file
    _FILESYSTEM_VERIFY(exists(_From), "file not found", error_type::runtime_error);
    _FILESYSTEM_VERIFY(!_Is_directory(_From), "expected a file", error_type::runtime_error);
    if (is_empty(_From)) { // nothing to do
        return true;
    }

    if (!exists(_To)) { // if not found _To, create new file
        (void) create_file(_To);
    }

    if (_Replace) { // clear _To and write to him content from _From
        (void) clear(_To);
        for (const auto& _Elem : read_all(_From)) {
            (void) write_back(_To, _Elem.c_str());
        }

        _FILESYSTEM_VERIFY(read_all(_From) == read_all(_To), "failed to copy the file", error_type::runtime_error);
        return true;
    } else { // don't touch old content
        const auto& _Src    = read_all(_From);
        const auto& _Result = read_all(_To); // content from _From and _To
        
        const_cast<vector<string>&>(_Result).insert(_Result.end(), _Src.begin(), _Src.end());
        for (const auto& _Elem : read_all(_From)) {
            (void) write_back(_To, _Elem.c_str());
        }

        _FILESYSTEM_VERIFY(read_all(_To) == _Result, "failed to copy the file", error_type::runtime_error);
        return true;
    }
}

// FUNCTION copy_junction
_NODISCARD bool copy_junction(const path& _Junction, const path& _Newjunction) {
    _FILESYSTEM_VERIFY(is_junction(_Junction), "junction not found", error_type::runtime_error);
    return create_junction(read_junction(_Junction), _Newjunction);
}

// FUNCTION copy_symlink
_NODISCARD bool copy_symlink(const path& _Symlink, const path& _Newsymlink) {
    _FILESYSTEM_VERIFY(is_symlink(_Symlink), "symbolic link not found", error_type::runtime_error);
    return create_symlink(read_symlink(_Symlink), _Newsymlink);
}

// FUNCTION create_directory
_NODISCARD bool create_directory(const path& _Path) {
    _FILESYSTEM_VERIFY(CreateDirectoryW(_Path.generic_wstring().c_str(), nullptr),
        "failed to create the directory", error_type::runtime_error);
    return true;
}

// FUNCTION create_file
_NODISCARD bool create_file(const path& _Path, const file_attributes _Attributes) {
    _FILESYSTEM_VERIFY(!exists(_Path), "file already exists", error_type::runtime_error);
    const HANDLE _Handle{CreateFileW(_Path.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::all), static_cast<unsigned long>(file_share::all),
        nullptr, static_cast<unsigned long>(file_disposition::only_new), static_cast<unsigned long>(_Attributes), nullptr)};
    _FILESYSTEM_VERIFY_HANDLE(_Handle);

    CloseHandle(_Handle);
    _FILESYSTEM_VERIFY(exists(_Path), "failed to create the file", error_type::runtime_error);
    return true;
}

_NODISCARD bool create_file(const path& _Path) { // creates new file
    return create_file(_Path, file_attributes::normal);
}

// FUNCTION create_hard_link
_NODISCARD bool create_hard_link(const path& _To, const path& _Hardlink) { // creates hard link _Hardlink to _To
    _FILESYSTEM_VERIFY(CreateHardLinkW(_Hardlink.generic_wstring().c_str(), _To.generic_wstring().c_str(),
        nullptr), "failed to create the hard link", error_type::runtime_error);
    return true;
}

#pragma warning(push)
#pragma warning(disable : 6385) // C6385: reading incorrect data
// FUNCTION create_junction
_NODISCARD bool create_junction(const path& _To, const path& _Junction) {
    _FILESYSTEM_VERIFY(exists(_To), "directory not found", error_type::runtime_error);
    _FILESYSTEM_VERIFY(is_directory(_To), "expected a directory", error_type::runtime_error);

    // at the beginning _Junction must be created as default directory 
    (void) create_directory(_Junction);
    const HANDLE _Handle{CreateFileW(_Junction.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::readonly | file_access::writeonly), 0, nullptr,
        static_cast<unsigned long>(file_disposition::only_if_exists), static_cast<unsigned long>(
            file_flags::backup_semantics | file_flags::open_reparse_point), nullptr)};
    _FILESYSTEM_VERIFY_HANDLE(_Handle);

    unsigned char _Buff[sizeof(reparse_data_buffer) + _Max_path * sizeof(wchar_t)] = {};
    reparse_mountpoint_data_buffer& _Reparse_buff                                  = reinterpret_cast<reparse_mountpoint_data_buffer&>(_Buff);

    // set informations about reparse point
    _CSTD memset(_Buff, 0, sizeof(_Buff));
    
    // _Junction must containts prefix ("\??\") and sufix ("\"), to create junction successfully
    _Reparse_buff._Reparse_tag                   = static_cast<unsigned long>(file_reparse_tag::mount_point);
    int _Len                                     = MultiByteToWideChar(static_cast<uint32_t>(code_page::acp), 0,
        path(R"(\??\)" + _To.generic_string() + R"(\)").generic_string().c_str(), -1, _Reparse_buff._Reparse_target, _MAX_PATH);
    _Reparse_buff._Reparse_target_maximum_length = static_cast<uint16_t>((_Len--) * sizeof(wchar_t));
    _Reparse_buff._Reparse_target_length         = static_cast<uint16_t>(_Len * sizeof(wchar_t));
    _Reparse_buff._Reparse_data_length           = _Reparse_buff._Reparse_target_length + 12;

    // attach reparse point to _Junction
    unsigned long _Bytes; // returned bytes from DeviceIoControl()

    // don't check result of DeviceIoControl(), because next function will check it better
    DeviceIoControl(_Handle, FSCTL_SET_REPARSE_POINT, &_Reparse_buff,
        _Reparse_buff._Reparse_data_length + 8 /* REPARSE_MOUNTPOINT_HEADER_SIZE */, nullptr, 0, &_Bytes, nullptr);
    CloseHandle(_Handle);
    _FILESYSTEM_VERIFY(is_junction(_Junction), "failed to create the junction", error_type::runtime_error);
    return true;
}
#pragma warning(pop)

namespace experimental {
    // FUNCTION TEMPLATE create_shortcut
#ifdef _FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS
    _NODISCARD bool create_shortcut(const path& _To, const path& _Shortcut) {
#else // ^^^ _FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS ^^^ / vvv !_FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS vvv
    template <class _CharTy>
    _NODISCARD bool create_shortcut(const path & _To, const path & _Shortcut, const _CharTy* const _Description) {
#endif // _FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS
        _FILESYSTEM_VERIFY(exists(_To), "file not found", error_type::runtime_error);
        _FILESYSTEM_VERIFY(!_Is_directory(_To), "expected a file", error_type::runtime_error);
        IShellLinkW* _Link{};
        _FILESYSTEM_VERIFY(CoInitialize(nullptr) == S_OK, "failed to initialize COM library", error_type::runtime_error);
        _FILESYSTEM_VERIFY(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_ALL, IID_IShellLinkW,
            reinterpret_cast<void**>(&_Link)) == S_OK, "failed to create COM object instance", error_type::runtime_error);
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->SetPath(_To.generic_wstring().c_str()), _Link);
#ifndef _FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS
        // Current version don't offerts conversion between char[16/32]_t and wchar_t.
        // First convert to narrow, and then from narrow to wide.
        const auto& _Narrow = _Convert_to_narrow<_CharTy, char_traits<_CharTy>>(_Description);
        const auto& _Wide = _Convert_narrow_to_wide(code_page::utf8, _Narrow.c_str());
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->SetDescription(_Wide.c_str()), _Link);
#endif // _FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS
        IPersistFile* _File = {};
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&_File)), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_File->Save(_Shortcut.generic_wstring().c_str(), true), _Link);
        _File->Release();
        _Link->Release();
        return true;
    }

#ifndef _FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS
    template _FILESYSTEM_API _NODISCARD bool create_shortcut(const path&, const path&, const char* const);
    template _FILESYSTEM_API _NODISCARD bool create_shortcut(const path&, const path&, const char8_t* const);
    template _FILESYSTEM_API _NODISCARD bool create_shortcut(const path&, const path&, const char16_t* const);
    template _FILESYSTEM_API _NODISCARD bool create_shortcut(const path&, const path&, const char32_t* const);
    template _FILESYSTEM_API _NODISCARD bool create_shortcut(const path&, const path&, const wchar_t* const);
#endif // _FILESYSTEM_DEPRECATED_SHORTCUT_PARAMETERS
} // experimental

// FUNCTION create_symlink
_NODISCARD bool create_symlink(const path& _To, const path& _Symlink, const symlink_flags _Flags) {
    _FILESYSTEM_VERIFY(CreateSymbolicLinkW(_Symlink.generic_wstring().c_str(), _To.generic_wstring().c_str(),
        static_cast<unsigned long>(_Flags)), "failed to create the symlink", error_type::runtime_error);
    return true;
}

_NODISCARD bool create_symlink(const path& _To, const path& _Symlink) {
    return create_symlink(_To, _Symlink, is_directory(_To) ? symlink_flags::directory | symlink_flags::allow_unprivileged
        : symlink_flags::file | symlink_flags::allow_unprivileged);
}

// FUNCTION remove
_NODISCARD bool remove(const path& _Path) { // removes files and directories
    _FILESYSTEM_VERIFY(_Is_directory(_Path) ? RemoveDirectoryW(_Path.generic_wstring().c_str())
        : DeleteFileW(_Path.generic_wstring().c_str()), "failed to remove the target", error_type::runtime_error);
    return true;
}

// FUNCTION remove_all
_NODISCARD bool remove_all(const path& _Path) { // removes directory with all content
    _FILESYSTEM_VERIFY(_Is_directory(_Path), "expected a directory", error_type::runtime_error);
    if (is_empty(_Path)) { // if empty, remove _Path and don't do anything else
        return remove(_Path);
    }

    const auto& _Src{_Path.generic_wstring()};
    if (_Src.back() != _Expected_slash) { // must contains slash before null-char
        const_cast<wstring&>(_Src).push_back(static_cast<wchar_t>(_Expected_slash));
    }

    // without 0 on last position, SHFileOperationW() may not work correctly
    const_cast<wstring&>(_Src).push_back(L'\0');

    // In this case we use only fFlags, pFrom and wFunc.
    // Rest are unnecessary and they can have any value, but they must be defined.
    // Without it SHFileOperationW() will throw an exception.
    SHFILEOPSTRUCTW _Ops       = SHFILEOPSTRUCTW();
    _Ops.fAnyOperationsAborted = false;
    _Ops.fFlags                = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    _Ops.hNameMappings         = nullptr;
    _Ops.hwnd                  = nullptr; // never used in this case
    _Ops.lpszProgressTitle     = nullptr;
    _Ops.pFrom                 = _Src.c_str();
    _Ops.pTo                   = nullptr;
    _Ops.wFunc                 = FO_DELETE;
    _FILESYSTEM_VERIFY(SHFileOperationW(&_Ops) == 0, "failed to remove the directory", error_type::runtime_error);

    // SHFileOperationW() removes base directory as well, so there's nothing to do more
    return true;
}

// FUNCTION remove_junction
_NODISCARD bool remove_junction(const path& _Target) {
    _FILESYSTEM_VERIFY(is_junction(_Target), "expected a junction", error_type::runtime_error);
    const HANDLE _Handle{CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::readonly | file_access::writeonly), 0, nullptr,
        static_cast<unsigned long>(file_disposition::only_if_exists), static_cast<unsigned long>(
            file_flags::backup_semantics | file_flags::open_reparse_point), nullptr)};
    _FILESYSTEM_VERIFY_HANDLE(_Handle);

    unsigned char _Buff[8 /* REPARSE_MOUNTPOINT_HEADER_SIZE */] = {};
    reparse_mountpoint_data_buffer& _Reparse_buff               = reinterpret_cast<reparse_mountpoint_data_buffer&>(_Buff);
    unsigned long _Bytes                                        = 0; // returned bytes from DeviceIoControl()

    // set new information to _Target
    _CSTD memset(_Buff, 0, sizeof(_Buff));
    _Reparse_buff._Reparse_tag = static_cast<unsigned long>(file_reparse_tag::mount_point);
    if (!DeviceIoControl(_Handle, FSCTL_DELETE_REPARSE_POINT, &_Reparse_buff, 8 /* REPARSE_MOUNTPOINT_HEADER_SIZE */,
        nullptr, 0, &_Bytes, nullptr) || is_junction(_Target)) { // failed to remove junction
        CloseHandle(_Handle); // should be closed even if function will throw an exception
        _Throw_fs_error("failed to remove junction", error_type::runtime_error, "remove_junction");
    }

    CloseHandle(_Handle);
    return true;
}

// FUNCTION remove_line
_NODISCARD bool remove_line(const path& _Target, const uintmax_t _Line) { // removes _Line line from _Target
    _FILESYSTEM_VERIFY(exists(_Target), "file not found", error_type::runtime_error);
    _FILESYSTEM_VERIFY(!_Is_directory(_Target), "expected a file", error_type::runtime_error);
    const auto& _All       = read_all(_Target);
    const size_t _Count    = _All.size();
    const size_t _Expected = _Count - 1; // count of lines after removed
    _FILESYSTEM_VERIFY(_Line > 0 && _Line <= _Count && _Expected >= 0, "invalid line", error_type::runtime_error);
    // Clear _Target and write to him the newest content.
    // Use resize_file() instead of clear(), because we know that _Target is a file.
    (void) resize_file(_Target, 0);
    for (size_t _Idx = 0; _Idx < _Count; ++_Idx) {
        if (_Idx + 1 != _Line) { // skip removed line
            (void) write_back(_Target, _All[_Idx].c_str());
        }
    }

    _FILESYSTEM_VERIFY(lines_count(_Target) == _Expected, "failed to remove the line", error_type::runtime_error);
    return true;
}

// FUNCTION remove_lines
_NODISCARD bool remove_lines(const path& _Target, const uintmax_t _First, uintmax_t _Count) {
    _FILESYSTEM_VERIFY(exists(_Target), "file not found", error_type::runtime_error);
    _FILESYSTEM_VERIFY(!_Is_directory(_Target), "expected a file", error_type::runtime_error);
    const uintmax_t _Old_count = lines_count(_Target);
    const uintmax_t _New_count = _Old_count - _Count;
    for (; _Count > 0; --_Count) { // use the same line
        (void) remove_line(_Target, _First);
    }

    _FILESYSTEM_VERIFY(lines_count(_Target) == _New_count, "failed to removed lines", error_type::runtime_error);
    return true;
}

// FUNCTION rename
_NODISCARD bool rename(const path& _Old, const path& _New, const rename_options _Flags) { // renames _Old to _New
    _FILESYSTEM_VERIFY(MoveFileExW(_Old.generic_wstring().c_str(), _New.generic_wstring().c_str(),
        static_cast<unsigned long>(_Flags)), "failed to rename the target", error_type::runtime_error);
    return true;
}

_NODISCARD bool rename(const path& _Old, const path& _New) {
    return rename(_Old, _New, rename_options::copy | rename_options::replace);
}
_FILESYSTEM_END

#endif // !_HAS_WINDOWS