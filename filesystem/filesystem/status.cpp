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
// FUNCTION file_status::file_status
__thiscall file_status::file_status() noexcept {
    _Init();
    _Refresh();
}

__cdecl file_status::file_status(const path& _Path) noexcept {
    _Init();
    _Mypath = _Path;
    _Refresh();
}

// FUNCTION file_status::_Init
void __thiscall file_status::_Init() noexcept {
    _Mypath  = path();
    _Myattr  = file_attributes::none;
    _Myperms = file_permissions::none;
    _Mytype  = file_type::none;
}

// FUNCTION file_status::_Refresh
void __thiscall file_status::_Refresh() noexcept {
    _Update_attribute(static_cast<file_attributes>(_CSTD GetFileAttributesW(_Mypath.generic_wstring().c_str())));

    // If _Mypath not found, GetFileAttributeW() will return INVALID_FILE_ATTRIBUTES.
    // Don't use GetLastError() to check if _Path exists, because some functions
    // will return ERROR_PATH_NOT_FOUND and others ERROR_FILE_NOT_FOUND when they cannot find target.
    if (_Myattr == file_attributes::unknown) { // file_attributes::unknown means, that target not found
        _Update_attribute(file_attributes::none);
        _Update_permissions(file_permissions::none);
        _Update_type(file_type::not_found);
        return; // don't check anything else
    }

    // check _Mypath attributes
    if ((_Myattr & file_attributes::readonly) == file_attributes::readonly) { // read-only
        _Update_permissions(file_permissions::readonly);
    } else { // full access
        _Update_permissions(file_permissions::all);
    }

    // Theoretically we could use FindFirstFile() with WIN32_FIND_DATAW, because it's a bit faster,
    // but when we use DeviceIoControl(), we are on lower level, where we can define buffer size etc.
    // It's safer as well, because if _Mypath is bad, there're only execptions from us.
    
    // checks if _Path is a reparse point, if is check type
    if ((_Myattr & file_attributes::reparse_point) == file_attributes::reparse_point) {
        // In some cases _Refresh() may using more bytes than defaule maximum (16384 bytes).
        // To avoid C6262 warning and potential threat, buffor size is set to 16284 bytes.
        // It shouldn't change result and it's safer. If your /analyse:stacksize is set to larger value,
        // you can change buffer size.
        unsigned char _Buff[MAXIMUM_REPARSE_DATA_BUFFER_SIZE - 100];
        reparse_data_buffer& _Reparse_buff = reinterpret_cast<reparse_data_buffer&>(_Buff);
        unsigned long _Bytes; // returned bytes from DeviceIoControl()

        const HANDLE _Handle = _CSTD CreateFileW(_Mypath.generic_wstring().c_str(),
            static_cast<unsigned long>(file_access::all), static_cast<unsigned long>(file_share::read),
            nullptr, static_cast<unsigned long>(file_disposition::only_if_exists), static_cast<unsigned long>(
                file_flags::backup_semantics | file_flags::open_reparse_point), nullptr);

        if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
            _Throw_fs_error("failed to get handle", error_type::runtime_error, "_Refresh");
        }

        if (!_CSTD DeviceIoControl(_Handle, FSCTL_GET_REPARSE_POINT, nullptr, 0,
            &_Reparse_buff, sizeof(_Buff), &_Bytes, nullptr)) { // failed to get informations
            _Throw_fs_error("failed to get informations", error_type::runtime_error, "_Refresh");
        }
        
        _CSTD CloseHandle(_Handle);

        if (_Reparse_buff._Reparse_tag == static_cast<unsigned long>(file_reparse_tag::mount_point)) { // junction
            _Update_type(file_type::junction);
            return;
        }

        if (_Reparse_buff._Reparse_tag == static_cast<unsigned long>(file_reparse_tag::symlink)) {
            _Update_type(file_type::symlink);
            return;
        }

        // all others are file or directory types
    }

    if ((_Myattr & file_attributes::directory) == file_attributes::directory) { // directory
        _Update_type(file_type::directory);
    } else { // regular file
        _Update_type(file_type::regular);
    }
}

// FUNCTION file_status::_Update_attribute
void __cdecl file_status::_Update_attribute(const file_attributes _Newattrib) noexcept {
    _Myattr = _Newattrib;
}

// FUNCTION file_status::_Update_permissions
void __cdecl file_status::_Update_permissions(const file_permissions _Newperms) noexcept {
    _Myperms = _Newperms;
}

// FUNCTION file_status::_Update_type
void __cdecl file_status::_Update_type(const file_type _Newtype) noexcept {
    _Mytype = _Newtype;
}

// FUNCTION file_status::attribute
_NODISCARD const file_attributes __thiscall file_status::attribute() const noexcept {
    return _Myattr;
}

// FUNCTION file_status::permissions
_NODISCARD const file_permissions __thiscall file_status::permissions() const noexcept {
    return _Myperms;
}

// FUNCTION file_status::type
_NODISCARD const file_type __thiscall file_status::type() const noexcept {
    return _Mytype;
}

// FUNCTION directory_data::directory_data
__thiscall directory_data::directory_data() noexcept {
    _Init();
    _Refresh(); // get the latest informaions
}

__cdecl directory_data::directory_data(const path& _Path) noexcept {
    _Init();
    _Mypath = _Path; // update current working path
    _Refresh(); // get the latest informaions
}

// FUNCTION directory_data::_Init
void __thiscall directory_data::_Init() noexcept {
    _Mypath = path();

    for (size_t _Idx = 0; _Idx < _Mycount.size(); ++_Idx) {
        _Mycount[_Idx] = 0;
        _Myname[_Idx]  = {};
    }
}

// FUNCTION directory_data::_Refresh
void __thiscall directory_data::_Refresh() noexcept {
    if (!exists(_Mypath)) { // directory not found
        _Throw_fs_error("directory not found", error_type::runtime_error, "_Refresh");
    }

    // _Mypath must be directory
    if (!is_directory(_Mypath) && !is_junction(_Mypath)
        && !_CSTD PathIsDirectoryW(_Mypath.generic_wstring().c_str())) { // expected directory
        _Throw_fs_error("expected directory", error_type::runtime_error, "_Refresh");
    }

    if (!is_empty(_Mypath)) {
        WIN32_FIND_DATAW _Data;
        const HANDLE _Handle = _CSTD FindFirstFileExW(path(_Mypath + LR"(\*)").generic_wstring().c_str(),
            FindExInfoBasic, &_Data, FindExSearchNameMatch, nullptr, 0);

        if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
            _Throw_fs_error("failed to get handle", error_type::runtime_error, "_Refresh");
        }

        vector<path> _All;

        do { // get all types, they will be separated later
            _All.push_back(static_cast<const wchar_t*>(_Data.cFileName));
        } while (_CSTD FindNextFileW(_Handle, &_Data));
        _CSTD FindClose(_Handle);

        _Reset(); // clear everything
        path _Precise; // creates full path to _All[_Idx]

        for (const auto& _Elem : _All) {
            if (_Elem != "." && _Elem != "..") { // skip dots
                _Precise = _Mypath + LR"(\)" + _Elem;

                if (is_directory(_Precise)) { // directory
                    _Myname[0].push_back(_Elem);
                } else if (is_junction(_Precise)) { // junction (as directory)
                    _Myname[1].push_back(_Elem);
                } else if (is_other(_Precise)) { // other
                    _Myname[2].push_back(_Elem);
                } else if (is_regular_file(_Precise)) { // regular file
                    _Myname[3].push_back(_Elem);
                } else if (is_symlink(_Precise)) { // symlink (directory or file)
                    _Myname[4].push_back(_Elem);
                }

                _Myname[5].push_back(_Elem); // _Myname[5] (total) takes every type
            }
        }

        for (size_t _Idx = 0; _Idx < _Myname.size(); ++_Idx) {
            _Mycount[_Idx] = _Myname[_Idx].size();
        }

        return; // that's all if _Mypath isn't empty
    }

    // if _Mypath is empty, leave variables without value
    _Reset();
}

// FUNCTION directory_data::_Reset
void __thiscall directory_data::_Reset() noexcept {
    // _Init() cannot be used here, because removes value from _Path
    for (size_t _Idx = 0; _Idx < _Mycount.size(); ++_Idx) {
        _Mycount[_Idx] = 0;
        _Myname[_Idx].clear();
    }
}

// FUNCTION directory_data::directories
_NODISCARD const vector<path>& __thiscall directory_data::directories() const noexcept {
    return _Myname[0];
}

// FUNCTION directory_data::directories_count
_NODISCARD const size_t __thiscall directory_data::directories_count() const noexcept {
    return _Mycount[0];
}

// FUNCTION directory_data::junctions
_NODISCARD const vector<path>& __thiscall directory_data::junctions() const noexcept {
    return _Myname[1];
}

// FUNCTION directory_data::junctions_count
_NODISCARD const size_t __thiscall directory_data::junctions_count() const noexcept {
    return _Mycount[1];
}

// FUNCTION directory_data::others
_NODISCARD const vector<path>& __thiscall directory_data::others() const noexcept {
    return _Myname[2];
}

// FUNCTION directory_data::others_count
_NODISCARD const size_t __thiscall directory_data::others_count() const noexcept {
    return _Mycount[2];
}

// FUNCTION directory_data::regular_files
_NODISCARD const vector<path>& __thiscall directory_data::regular_files() const noexcept {
    return _Myname[3];
}

// FUNCTION directory_data::regular_files_count
_NODISCARD const size_t __thiscall directory_data::regular_files_count() const noexcept {
    return _Mycount[3];
}

// FUNCTION directory_data::symlinks
_NODISCARD const vector<path>& __thiscall directory_data::symlinks() const noexcept {
    return _Myname[4];
}

// FUNCTION directory_data::symlinks_count
_NODISCARD const size_t __thiscall directory_data::symlinks_count() const noexcept {
    return _Mycount[4];
}

// FUNCTION directory_data::total
_NODISCARD const vector<path>& __thiscall directory_data::total() const noexcept {
    return _Myname[5];
}

// FUNCTION directory_data::total_count
_NODISCARD const size_t __thiscall directory_data::total_count() const noexcept {
    return _Mycount[5];
}

// FUNCTION change_attributes
_NODISCARD bool __cdecl change_attributes(const path& _Target, const file_attributes _Newattr) {
    if (!exists(_Target)) { // file/directory not found
        _Throw_fs_error("path not found", error_type::runtime_error, "change_attributes");
    }

    if (!_CSTD SetFileAttributesW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(_Newattr))) { // failed to change attributes
        _Throw_fs_error("failed to change attributes", error_type::runtime_error, "change_attributes");
    }

    return true;
}

#pragma warning(push)
#pragma warning(disable : 4834) // C4834: discard function with [nodiscard] attribute (operator^=())
// FUNCTION change_permissions
_NODISCARD bool __cdecl change_permissions(const path& _Target, const file_permissions _Newperms,
    const bool _Inc_old, const bool _Links) { // _Inc_old = include old attributes, _Links = follow links (symlinks and junctions)
    if (!exists(_Target)) { // path not found
        _Throw_fs_error("path not found", error_type::runtime_error, "change_permissions");
    }

    file_status _Status(_Target);
    if (_Status.permissions() == _Newperms) { // old permissions are same as new, nothing to do
        return true;
    }

    if (_Status.type() == file_type::symlink || _Status.type() == file_type::junction
        && !_Links) { // user don't want to change permissions to links, but _Target is one of them
        _Throw_fs_error("target is a link", error_type::runtime_error, "change_permissions");
    }

    const auto& _Attr = _Newperms == file_permissions::readonly ? file_attributes::readonly : file_attributes::none;
    if (_Inc_old) { // in this cast, user adds new attributes to existing
        const_cast<file_attributes&>(_Attr) ^= _Status.attribute();
    }

    // if _Inc_old is true, add new attribute to existing
    if (!_CSTD SetFileAttributesW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(_Attr))) { // failed to set new permissions
        _Throw_fs_error("failed to set new permissions", error_type::runtime_error, "change_permissions");
    }

    return true;
}
#pragma warning(pop)

_NODISCARD bool __cdecl change_permissions(const path& _Target, const file_permissions _Newperms) {
    return change_permissions(_Target, _Newperms, true, true);
}

// FUNCTION creation_time
_NODISCARD file_time __cdecl creation_time(const path& _Target) {
    if (!exists(_Target)) { // file/directory not found
        _Throw_fs_error("path not found", error_type::runtime_error, "creation_time");
    }

    const HANDLE _Handle = _CSTD CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::readonly), static_cast<unsigned long>(file_share::read),
        nullptr, static_cast<unsigned long>(file_disposition::only_if_exists), static_cast<unsigned long>(
            file_flags::backup_semantics | file_flags::open_reparse_point), nullptr);

    if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
        _Throw_fs_error("failed to get handle", error_type::runtime_error, "creation_time");
    }

    FILETIME _File_time;
    if (!_CSTD GetFileTime(_Handle, &_File_time, nullptr, nullptr)) { // failed to get file time
        _CSTD CloseHandle(_Handle); // should be cosed even if function will throw an exception
        _Throw_fs_error("failed to get file time", error_type::runtime_error, "creation_time");
    }

    _CSTD CloseHandle(_Handle);

    SYSTEMTIME _Sys_gen_time; // general system time
    SYSTEMTIME _Sys_exact_time; // system time in user region

    if (!_CSTD FileTimeToSystemTime(&_File_time, &_Sys_gen_time)) { // failed to convert time
        _Throw_fs_error("failed to convert file time to system time", error_type::runtime_error, "creation_time");
    }

    // now we have to convert general system time to system time in user region
    if (!_CSTD SystemTimeToTzSpecificLocalTimeEx(nullptr, &_Sys_gen_time, &_Sys_exact_time)) { // failed to convert time
        _Throw_fs_error("failed to convert general system time to exact system time",
            error_type::runtime_error, "creation_time");
    }

    return {_Sys_exact_time.wYear, _Sys_exact_time.wMonth, _Sys_exact_time.wDay,
        _Sys_exact_time.wHour, _Sys_exact_time.wMinute, _Sys_exact_time.wSecond};
}

// FUNCTION equivalent
_NODISCARD bool __cdecl equivalent(const path& _Left, const path& _Right) {
    if (!exists(_Left) || !exists(_Right)) { // path not found
        _Throw_fs_error("path not found", error_type::runtime_error, "equivalent");
    }

    file_id _Left_id;
    {
        const HANDLE _Handle = _CSTD CreateFileW(_Left.generic_wstring().c_str(),
            static_cast<unsigned long>(file_access::readonly), static_cast<unsigned long>(
            file_share::read), nullptr, static_cast<unsigned long>(file_disposition::only_if_exists),
            static_cast<unsigned long>(file_flags::backup_semantics), nullptr); // don't use file_flags::open_reparse_point

        if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
            _Throw_fs_error("failed to get handle", error_type::runtime_error, "equivalent");
        }

        if (!_CSTD GetFileInformationByHandleEx(_Handle, FileIdInfo, &_Left_id,
            sizeof(_Left_id))) { // failed to get informations
            _Throw_fs_error("failed to get informations", error_type::runtime_error, "equivalent");
        }
        _CSTD CloseHandle(_Handle);
    }

    file_id _Right_id;
    {
        const HANDLE _Handle = _CSTD CreateFileW(_Right.generic_wstring().c_str(),
            static_cast<unsigned long>(file_access::readonly), static_cast<unsigned long>(
            file_share::read), nullptr, static_cast<unsigned long>(file_disposition::only_if_exists),
            static_cast<unsigned long>(file_flags::backup_semantics), nullptr); // don't use file_flags::open_reparse_point

        if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
            _Throw_fs_error("failed to get handle", error_type::runtime_error, "equivalent");
        }

        if (!_CSTD GetFileInformationByHandleEx(_Handle, FileIdInfo, &_Right_id,
            sizeof(_Right_id))) { // failed to get informations
            _Throw_fs_error("failed to get informations", error_type::runtime_error, "equivalent");
        }
        _CSTD CloseHandle(_Handle);
    }

    return _CSTD memcmp(&_Left_id, &_Right_id, sizeof(file_id)) == 0;
}

// FUNCTION file_size
_NODISCARD size_t __cdecl file_size(const path& _Target) {
    if (!exists(_Target)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "file_size");
    }

    if (_CSTD PathIsDirectoryW(_Target.generic_wstring().c_str())) { // file size is only for files
        _Throw_fs_error("expected file", error_type::runtime_error, "file_size");
    }

    const HANDLE _Handle = _CSTD CreateFileW(_Target.generic_wstring().c_str(), static_cast<unsigned long>(file_access::readonly),
        static_cast<unsigned long>(file_share::read), nullptr, static_cast<unsigned long>(file_disposition::only_if_exists),
        static_cast<unsigned long>(file_flags::open_reparse_point), nullptr);

    if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
        _Throw_fs_error("failed to get handle", error_type::runtime_error, "file_size");
    }

    const size_t _Size = static_cast<size_t>(_CSTD GetFileSize(_Handle, nullptr));
    _CSTD CloseHandle(_Handle);

    return _Size;
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
    const auto _Attr_or_flags = is_directory(_Target) || is_junction(_Target)
        || _CSTD PathIsDirectoryW(_Target.generic_wstring().c_str()) ?
        static_cast<unsigned long>(_Flags) : static_cast<unsigned long>(_Attributes);
    
    const HANDLE _Handle = _CSTD CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::readonly), static_cast<unsigned long>(file_share::read),
        nullptr, static_cast<unsigned long>(file_disposition::only_if_exists), _Attr_or_flags, nullptr);

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
    return hard_link_count(_Target, file_attributes::none, file_flags::backup_semantics | file_flags::open_reparse_point);
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

    return {is_directory(_Target) || is_junction(_Target) || _CSTD PathIsDirectoryW(_Target.generic_wstring().c_str())
        ? _CSTD PathIsDirectoryEmptyW(_Target.generic_wstring().c_str()) == 1 : file_size(_Target) == 0};
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
    case file_type::none:
    case file_type::not_found:
    case file_type::directory:
    case file_type::regular:
    case file_type::symlink:
        return false;
    case file_type::junction:
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


// FUNCTION last_access_time
_NODISCARD file_time __cdecl last_access_time(const path& _Target) {
    if (!exists(_Target)) { // file/directory not found
        _Throw_fs_error("path not found", error_type::runtime_error, "last_access_time");
    }

    const HANDLE _Handle = _CSTD CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::readonly), static_cast<unsigned long>(file_share::read),
        nullptr, static_cast<unsigned long>(file_disposition::only_if_exists), static_cast<unsigned long>(
            file_flags::backup_semantics | file_flags::open_reparse_point), nullptr);

    if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
        _Throw_fs_error("failed to get handle", error_type::runtime_error, "last_access_time");
    }

    FILETIME _File_time;
    if (!_CSTD GetFileTime(_Handle, nullptr, &_File_time, nullptr)) { // failed to get file time
        _CSTD CloseHandle(_Handle); // should be closed even if function will throw an exception
        _Throw_fs_error("failed to get file time", error_type::runtime_error, "last_access_time");
    }

    _CSTD CloseHandle(_Handle);

    SYSTEMTIME _Sys_gen_time; // general system time
    SYSTEMTIME _Sys_exact_time; // system time in user region

    if (!_CSTD FileTimeToSystemTime(&_File_time, &_Sys_gen_time)) { // failed to convert time
        _Throw_fs_error("failed to convert file time to system time", error_type::runtime_error, "last_access_time");
    }

    // now we have to convert general system time to system time in user region
    if (!_CSTD SystemTimeToTzSpecificLocalTimeEx(nullptr, &_Sys_gen_time, &_Sys_exact_time)) {
        // failed to convert general system time to exact system time
        _Throw_fs_error("failed to convert general system time to exact system time",
            error_type::runtime_error, "last_access_time");
    }

    return {_Sys_exact_time.wYear, _Sys_exact_time.wMonth, _Sys_exact_time.wDay,
        _Sys_exact_time.wHour, _Sys_exact_time.wMinute, _Sys_exact_time.wSecond};
}

// FUNCTION last_write_time
_NODISCARD file_time __cdecl last_write_time(const path& _Target) {
    if (!exists(_Target)) { // file/directory not found
        _Throw_fs_error("path not found", error_type::runtime_error, "last_write_time");
    }

    const HANDLE _Handle = _CSTD CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::readonly), static_cast<unsigned long>(file_share::read),
        nullptr, static_cast<unsigned long>(file_disposition::only_if_exists), static_cast<unsigned long>(
            file_flags::backup_semantics | file_flags::open_reparse_point), nullptr);

    if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
        _Throw_fs_error("failed to get handle", error_type::runtime_error, "last_write_time");
    }

    FILETIME _File_time;
    if (!_CSTD GetFileTime(_Handle, nullptr, nullptr, &_File_time)) { // failed to get file time
        _CSTD CloseHandle(_Handle); // should be closed even if function will throw an exception
        _Throw_fs_error("failed to get file time", error_type::runtime_error, "last_write_time");
    }

    _CSTD CloseHandle(_Handle);

    SYSTEMTIME _Sys_gen_time; // general system time
    SYSTEMTIME _Sys_exact_time; // system time in user region

    if (!_CSTD FileTimeToSystemTime(&_File_time, &_Sys_gen_time)) { // failed to convert time
        _Throw_fs_error("failed to convert file time to system time", error_type::runtime_error, "last_write_time");
    }

    // now we have to convert general system time to system time in user region
    if (!_CSTD SystemTimeToTzSpecificLocalTimeEx(nullptr, &_Sys_gen_time, &_Sys_exact_time)) {
        // failed to convert general system time to exact system time
        _Throw_fs_error("failed to convert general system time to exact system time",
            error_type::runtime_error, "last_write_time");
    }

    return {_Sys_exact_time.wYear, _Sys_exact_time.wMonth, _Sys_exact_time.wDay,
        _Sys_exact_time.wHour, _Sys_exact_time.wMinute, _Sys_exact_time.wSecond};
}

// FUNCTION space
_NODISCARD disk_space __cdecl space(const path& _Target) {
    disk_space _Result;
    const auto _Available = reinterpret_cast<PULARGE_INTEGER>(&_Result._Available);
    const auto _Capacity  = reinterpret_cast<PULARGE_INTEGER>(&_Result._Capacity);
    const auto _Free      = reinterpret_cast<PULARGE_INTEGER>(&_Result._Free);

    if (!_CSTD GetDiskFreeSpaceExW(_Target.generic_wstring().c_str(), _Available,
        _Capacity, _Free)) { // failed to get informations
        _Throw_fs_error("failed to get informations", error_type::runtime_error, "space");
    }

    return _Result;
}
_FILESYSTEM_END

#pragma warning(pop)
#endif // !_HAS_WINDOWS