// status.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <filesystem_pch.hpp>
#include <filesystem.hpp>

#if !_HAS_WINDOWS
#pragma message("The contents of <status.cpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv
#pragma warning(push)
#pragma warning(disable : 4996) // C4996: using deprecated content

_FILESYSTEM_BEGIN
// FUNCTION file_status::file_status
file_status::file_status() noexcept {
    _Init();
    _Refresh();
}

file_status::file_status(const path& _Path) noexcept {
    _Init();
    _Mypath = _Path;
    _Refresh();
}

file_status::file_status(const path& _Target, const file_attributes _Attr, const file_permissions _Perms,
    const file_type _Type) noexcept : _Mypath(_Target), _Myattr(_Attr), _Myperms(_Perms), _Mytype(_Type) {}

// FUNCTION file_status::_Init
void file_status::_Init() noexcept {
    _Mypath  = path();
    _Myattr  = file_attributes::none;
    _Myperms = file_permissions::none;
    _Mytype  = file_type::none;
}

// FUNCTION file_status::_Refresh
void file_status::_Refresh() noexcept {
    _Update_attribute(file_attributes{GetFileAttributesW(_Mypath.generic_wstring().c_str())});

    // If _Mypath not found, GetFileAttributeW() will return INVALID_FILE_ATTRIBUTES.
    // Don't use GetLastError() to check if _Mypath exists, because some functions
    // will return ERROR_PATH_NOT_FOUND and others ERROR_FILE_NOT_FOUND when they cannot find target.
    if (_Myattr == file_attributes::unknown) { // file_attributes::unknown means, that target not found
        _Update_attribute(file_attributes::none);
        _Update_permissions(file_permissions::none);
        _Update_type(file_type::not_found);
        return; // don't check anything else
    }

    if ((_Myattr & file_attributes::readonly) == file_attributes::readonly) { // read-only
        _Update_permissions(file_permissions::readonly);
    } else { // full access
        _Update_permissions(file_permissions::all);
    }

    // Theoretically we could use FindFirstFile() with WIN32_FIND_DATAW, because it's a bit faster,
    // but when we use DeviceIoControl(), we are on lower level, where we can define buffer size etc.
    // It's safer as well, because if _Mypath is bad, there're only execptions from us.

    if ((_Myattr & file_attributes::reparse_point) == file_attributes::reparse_point) {
        // In some cases _Refresh() may using more bytes than defaule maximum (16384 bytes).
        // To avoid C6262 warning and potential threat, buffor size is set to 16284 bytes.
        // It shouldn't change result and it's safer. If your /analyse:stacksize is set to larger value,
        // you can change buffer size.
        unsigned char _Buff[MAXIMUM_REPARSE_DATA_BUFFER_SIZE - 100] = {};
        reparse_data_buffer& _Reparse_buff                          = reinterpret_cast<reparse_data_buffer&>(_Buff);
        unsigned long _Bytes                                        = 0; // returned bytes from DeviceIoControl()
        const HANDLE _Handle                                        = CreateFileW(_Mypath.generic_wstring().c_str(),
            static_cast<unsigned long>(file_access::all), static_cast<unsigned long>(file_share::read),
            nullptr, static_cast<unsigned long>(file_disposition::only_if_exists), static_cast<unsigned long>(
                file_flags::backup_semantics | file_flags::open_reparse_point), nullptr);
        _FILESYSTEM_VERIFY_HANDLE(_Handle);
        if (!DeviceIoControl(_Handle, FSCTL_GET_REPARSE_POINT, nullptr, 0,
            &_Reparse_buff, sizeof(_Buff), &_Bytes, nullptr)) { // failed to get informations
            CloseHandle(_Handle); // should be closed even if function will throw an exception
            _Throw_fs_error("failed to get informations", error_type::runtime_error, "_Refresh");
        }
        
        CloseHandle(_Handle);
        if (_Reparse_buff._Reparse_tag == static_cast<unsigned long>(file_reparse_tag::mount_point)) {
            _Update_type(file_type::junction);
            return;
        }

        if (_Reparse_buff._Reparse_tag == static_cast<unsigned long>(file_reparse_tag::symlink)) {
            _Update_type(file_type::symlink);
            return;
        }

        // all others are file or directory types
    }

    if ((_Myattr & file_attributes::directory) == file_attributes::directory) {
        _Update_type(file_type::directory);
    } else {
        _Update_type(file_type::regular);
    }
}

// FUNCTION file_status::_Update_attribute
void file_status::_Update_attribute(const file_attributes _Newattrib) noexcept {
    _Myattr = _Newattrib;
}

// FUNCTION file_status::_Update_permissions
void file_status::_Update_permissions(const file_permissions _Newperms) noexcept {
    _Myperms = _Newperms;
}

// FUNCTION file_status::_Update_type
void file_status::_Update_type(const file_type _Newtype) noexcept {
    _Mytype = _Newtype;
}

// FUNCTION file_status::attribute
_NODISCARD const file_attributes file_status::attribute() const noexcept {
    return _Myattr;
}

// FUNCTION file_status::permissions
_NODISCARD const file_permissions file_status::permissions() const noexcept {
    return _Myperms;
}

// FUNCTION file_status::type
_NODISCARD const file_type file_status::type() const noexcept {
    return _Mytype;
}

// FUNCTION directory_data::directory_data
directory_data::directory_data() noexcept {
    _Init();
    _Refresh(); // get the latest informaions
}

directory_data::directory_data(const path& _Path) noexcept {
    _Init();
    _Mypath = _Path; // update current working path
    _Refresh(); // get the latest informaions
}

// FUNCTION directory_data::_Init
void directory_data::_Init() noexcept {
    _Mypath = path();
    for (size_t _Idx = 0; _Idx < _Mycount.size(); ++_Idx) {
        _Mycount[_Idx] = 0;
        _Myname[_Idx]  = vector<path>();
    }
}

// FUNCTION directory_data::_Refresh
void directory_data::_Refresh() noexcept {
    _FILESYSTEM_VERIFY(exists(_Mypath), "directory not found", error_type::runtime_error);
    _FILESYSTEM_VERIFY(_Is_directory(_Mypath), "expected a directory", error_type::runtime_error);

    WIN32_FIND_DATAW _Data = WIN32_FIND_DATAW();
    const HANDLE _Handle   = FindFirstFileExW(path(_Mypath + LR"(\*)").generic_wstring().c_str(),
        FindExInfoBasic, &_Data, FindExSearchNameMatch, nullptr, 0);
    _FILESYSTEM_VERIFY_HANDLE(_Handle);

    vector<path> _All;
    do { // get all types, they will be separated later
        _All.push_back(static_cast<const wchar_t*>(_Data.cFileName));
    } while (FindNextFileW(_Handle, &_Data));

    FindClose(_Handle);
    _Reset(); // clear everything
    if (_All.empty()) { // nothing to do if directory is empty
        return;
    }

    path _Precise; // creates full path to _All[_Idx]
    for (const auto& _Elem : _All) {
        if (_Elem != "." && _Elem != "..") { // skip dots
            _Precise = _Mypath + LR"(\)" + _Elem;
            file_status _Status(_Precise);
            _Myname[5].push_back(_Elem); // each type
            ++_Mycount[5];
            switch (_Status.type()) {
            case file_type::none: // never happens
                continue;
            case file_type::not_found: // never happens
                continue;
            case file_type::directory:
                _Myname[0].push_back(_Elem);
                ++_Mycount[0];
                continue;
            case file_type::regular:
                _Myname[3].push_back(_Elem);
                ++_Mycount[3];
                continue;
            case file_type::symlink:
                _Myname[4].push_back(_Elem);
                ++_Mycount[4];
                continue;
            case file_type::junction:
                _Myname[1].push_back(_Elem);
                ++_Mycount[1];
                continue;
            default: // other
                _Myname[2].push_back(_Elem);
                ++_Mycount[2];
                continue;
            }
        }
    }
}

// FUNCTION directory_data::_Reset
void directory_data::_Reset() noexcept {
    // _Init() cannot be used here, because removes value from _Mypath
    for (size_t _Idx = 0; _Idx < _Mycount.size(); ++_Idx) {
        _Mycount[_Idx] = 0;
        _Myname[_Idx].clear();
    }
}

// FUNCTION directory_data::directories
_NODISCARD const vector<path>& directory_data::directories() const noexcept {
    return _Myname[0];
}

// FUNCTION directory_data::directories_count
_NODISCARD const uintmax_t directory_data::directories_count() const noexcept {
    return _Mycount[0];
}

// FUNCTION directory_data::junctions
_NODISCARD const vector<path>& directory_data::junctions() const noexcept {
    return _Myname[1];
}

// FUNCTION directory_data::junctions_count
_NODISCARD const uintmax_t directory_data::junctions_count() const noexcept {
    return _Mycount[1];
}

// FUNCTION directory_data::others
_NODISCARD const vector<path>& directory_data::others() const noexcept {
    return _Myname[2];
}

// FUNCTION directory_data::others_count
_NODISCARD const uintmax_t directory_data::others_count() const noexcept {
    return _Mycount[2];
}

// FUNCTION directory_data::regular_files
_NODISCARD const vector<path>& directory_data::regular_files() const noexcept {
    return _Myname[3];
}

// FUNCTION directory_data::regular_files_count
_NODISCARD const uintmax_t directory_data::regular_files_count() const noexcept {
    return _Mycount[3];
}

// FUNCTION directory_data::symlinks
_NODISCARD const vector<path>& directory_data::symlinks() const noexcept {
    return _Myname[4];
}

// FUNCTION directory_data::symlinks_count
_NODISCARD const uintmax_t directory_data::symlinks_count() const noexcept {
    return _Mycount[4];
}

// FUNCTION directory_data::total
_NODISCARD const vector<path>& directory_data::total() const noexcept {
    return _Myname[5];
}

// FUNCTION directory_data::total_count
_NODISCARD const uintmax_t directory_data::total_count() const noexcept {
    return _Mycount[5];
}

// FUNCTION change_attributes
_NODISCARD bool change_attributes(const path& _Target, const file_attributes _Newattr) {
    _FILESYSTEM_VERIFY(exists(_Target), "target not found", error_type::runtime_error);
    _FILESYSTEM_VERIFY(SetFileAttributesW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(_Newattr)), "failed to change attributes", error_type::runtime_error);
    return true;
}

// FUNCTION change_permissions
_NODISCARD bool change_permissions(const path& _Target, const file_permissions _Newperms,
    const bool _Old, const bool _Links) { // _Old = include old attributes, _Links = follow links (symlinks and junctions)
    _FILESYSTEM_VERIFY(exists(_Target), "target not found", error_type::runtime_error);
    file_status _Status(_Target);
    if (_Status.permissions() == _Newperms) { // old permissions are same as new, nothing to do
        return true;
    }

    _FILESYSTEM_VERIFY(_Status.type() == file_type::symlink || _Status.type() == file_type::junction
        && !_Links, "target is a link", error_type::runtime_error);
    const auto _Attr{_Newperms == file_permissions::readonly ? file_attributes::readonly : file_attributes::none};
    if (_Old) { // in this cast, user adds new attributes to existing
        const_cast<file_attributes&>(_Attr) ^= _Status.attribute();
    }

    _FILESYSTEM_VERIFY(SetFileAttributesW(_Target.generic_wstring().c_str(), static_cast<unsigned long>(_Attr)),
        "failed to set new permissions", error_type::runtime_error);
    return true;
}

_NODISCARD bool change_permissions(const path& _Target, const file_permissions _Newperms) {
    return change_permissions(_Target, _Newperms, true, true);
}

// FUNCTION creation_time
_NODISCARD file_time creation_time(const path& _Target) {
    _FILESYSTEM_VERIFY(exists(_Target), "target not found", error_type::runtime_error);
    const HANDLE _Handle{CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::readonly), static_cast<unsigned long>(file_share::read),
        nullptr, static_cast<unsigned long>(file_disposition::only_if_exists), static_cast<unsigned long>(
            file_flags::backup_semantics | file_flags::open_reparse_point), nullptr)};
    _FILESYSTEM_VERIFY_HANDLE(_Handle);

    FILETIME _File_time;
    if (!GetFileTime(_Handle, &_File_time, nullptr, nullptr)) { // failed to get file time
        CloseHandle(_Handle); // should be closed even if function will throw an exception
        _Throw_fs_error("failed to get file time", error_type::runtime_error, "creation_time");
    }

    CloseHandle(_Handle);

    // convert file time to system time and then to user time zone
    SYSTEMTIME _Sys_gen_time; // general system time
    SYSTEMTIME _Sys_exact_time; // system time in user region
    _FILESYSTEM_VERIFY(FileTimeToSystemTime(&_File_time, &_Sys_gen_time),
        "failed to convert file time to system time", error_type::runtime_error);
    _FILESYSTEM_VERIFY(SystemTimeToTzSpecificLocalTimeEx(nullptr, &_Sys_gen_time, &_Sys_exact_time),
        "failed to convert general system time to exact system time", error_type::runtime_error);
    return {_Sys_exact_time.wYear, _Sys_exact_time.wMonth, _Sys_exact_time.wDay,
        _Sys_exact_time.wHour, _Sys_exact_time.wMinute, _Sys_exact_time.wSecond};
}

#pragma warning(push, 1)
#pragma warning(disable : 4067) // C4067: token after preprocessor directive (?)
// FUNCTION equivalent
_NODISCARD bool equivalent(const path& _Left, const path& _Right) {
    _FILESYSTEM_VERIFY(exists(_Left) && exists(_Right), "target not found", error_type::runtime_error);

    file_id _Left_id{file_id()};
    {
        const HANDLE _Handle{CreateFileW(_Left.generic_wstring().c_str(),
            static_cast<unsigned long>(file_access::readonly), static_cast<unsigned long>(
            file_share::read), nullptr, static_cast<unsigned long>(file_disposition::only_if_exists),
            static_cast<unsigned long>(file_flags::backup_semantics), nullptr)}; // don't use file_flags::open_reparse_point
        _FILESYSTEM_VERIFY_HANDLE(_Handle);

        if (!GetFileInformationByHandleEx(_Handle, FileIdInfo, &_Left_id,
            sizeof(_Left_id))) { // failed to get informations
            CloseHandle(_Handle); // should be closed even if function will throw an exception
            _Throw_fs_error("failed to get informations", error_type::runtime_error, "equivalent");
        }

        CloseHandle(_Handle);
    }

    file_id _Right_id{file_id()};
    {
        const HANDLE _Handle{CreateFileW(_Right.generic_wstring().c_str(),
            static_cast<unsigned long>(file_access::readonly), static_cast<unsigned long>(
            file_share::read), nullptr, static_cast<unsigned long>(file_disposition::only_if_exists),
            static_cast<unsigned long>(file_flags::backup_semantics), nullptr)}; // don't use file_flags::open_reparse_point
        _FILESYSTEM_VERIFY_HANDLE(_Handle);

        if (!GetFileInformationByHandleEx(_Handle, FileIdInfo, &_Right_id,
            sizeof(_Right_id))) { // failed to get informations
            CloseHandle(_Handle); // should be closed even if function will throw an exception
            _Throw_fs_error("failed to get informations", error_type::runtime_error, "equivalent");
        }
        
        CloseHandle(_Handle);
    }

#if __has_builtin(__builtin_memcmp)
    return __builtin_memcmp(&_Left_id, &_Right_id, sizeof(file_id)) == 0;
#else // ^^^ __has_builtin(__builtin_memcmp) ^^^ / vvv !__has_builtin(__builtin_memcmp) vvv
    return _CSTD memcmp(&_Left_id, &_Right_id, sizeof(file_id)) == 0;
#endif // __has_builtin(__builtin_memcmp)
}
#pragma warning(default : 4067)
#pragma warning(pop)

// FUNCTION exists
_NODISCARD bool exists(const file_status _Status) noexcept {
    return _Status.type() != file_type::none && _Status.type() != file_type::not_found;
}

_NODISCARD bool exists(const path& _Target) noexcept {
    return exists(file_status(_Target));
}

// FUNCTION file_size
_NODISCARD size_t file_size(const path& _Target) {
    _FILESYSTEM_VERIFY(exists(_Target), "file not found", error_type::runtime_error);
    _FILESYSTEM_VERIFY(!_Is_directory(_Target), "expected a file", error_type::runtime_error);

    const HANDLE _Handle{CreateFileW(_Target.generic_wstring().c_str(), static_cast<unsigned long>(file_access::readonly),
        static_cast<unsigned long>(file_share::read), nullptr, static_cast<unsigned long>(file_disposition::only_if_exists),
        static_cast<unsigned long>(file_flags::open_reparse_point), nullptr)};
    _FILESYSTEM_VERIFY_HANDLE(_Handle);

    const size_t _Size{static_cast<size_t>(GetFileSize(_Handle, nullptr))};
    CloseHandle(_Handle);
    return _Size;
}

// FUNCTION hard_link_count
_NODISCARD uintmax_t hard_link_count(const path& _Target, const file_flags _Flags) { // counts hard links to _Target
    const HANDLE _Handle{CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::readonly), static_cast<unsigned long>(file_share::read), nullptr,
        static_cast<unsigned long>(file_disposition::only_if_exists), static_cast<unsigned long>(_Flags), nullptr)};
    _FILESYSTEM_VERIFY_HANDLE(_Handle);

    FILE_STANDARD_INFO _Info{FILE_STANDARD_INFO()};
    if (!GetFileInformationByHandleEx(_Handle, FileStandardInfo, &_Info, sizeof(_Info))) {
        CloseHandle(_Handle); // should be closed even if function will throw an exception
        _Throw_fs_error("failed to get informations", error_type::runtime_error, "hard_link_count");
    }
    
    CloseHandle(_Handle);
    return static_cast<uintmax_t>(_Info.NumberOfLinks);
}

_NODISCARD uintmax_t hard_link_count(const path& _Target) {
    return hard_link_count(_Target, file_flags::backup_semantics | file_flags::open_reparse_point);
}

// FUNCTION _Is_directory
_NODISCARD bool _Is_directory(const file_status _Status) noexcept {
    return (_Status.attribute() & file_attributes::directory) == file_attributes::directory;
}

_NODISCARD bool _Is_directory(const path& _Target) noexcept {
    return _Is_directory(file_status(_Target));
}

// FUNCTION is_directory
_NODISCARD bool is_directory(const file_status _Status) noexcept {
    return _Status.type() == file_type::directory;
}

_NODISCARD bool is_directory(const path& _Target) noexcept {
    return is_directory(file_status(_Target));
}

// FUNCTION is_empty
_NODISCARD bool is_empty(const path& _Target) {
    _FILESYSTEM_VERIFY(exists(_Target), "target not found", error_type::runtime_error);
    return _Is_directory(_Target) ? directory_data(_Target).total_count() == 0 : file_size(_Target) == 0;
}

// FUNCTION is_junction
_NODISCARD bool is_junction(const file_status _Status) noexcept {
    return _Status.type() == file_type::junction;
}

_NODISCARD bool is_junction(const path& _Target) noexcept {
    return is_junction(file_status(_Target));
}

// FUNCTION is_other
_NODISCARD bool is_other(const file_status _Status) noexcept {
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

_NODISCARD bool is_other(const path& _Target) noexcept {
    return is_other(file_status(_Target));
}
 
// FUNCTION is_regular_file
_NODISCARD bool is_regular_file(const file_status _Status) noexcept { 
    return _Status.type() == file_type::regular;
}

_NODISCARD bool is_regular_file(const path& _Target) noexcept { 
    return is_regular_file(file_status(_Target));
}

// FUNCTION is_symlink
_NODISCARD bool is_symlink(const file_status _Status) noexcept {
    return _Status.type() == file_type::symlink;
}

_NODISCARD bool is_symlink(const path& _Target) noexcept {
    return is_symlink(file_status(_Target));
}

// FUNCTION junction_status
_NODISCARD file_status junction_status(const path& _Target) noexcept {
    const auto _Status{file_status(_Target)};

    // don't use is_junction(), because it will refresh status twice
    if (_Status.type() != file_type::junction) { // junction_status() is reserved for junctions only
        return file_status(_Target, file_attributes::unknown, file_permissions::unknown, file_type::not_found);
    }

    return _Status;
}

// FUNCTION last_access_time
_NODISCARD file_time last_access_time(const path& _Target) {
    _FILESYSTEM_VERIFY(exists(_Target), "target not found", error_type::runtime_error);

    const HANDLE _Handle{CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::readonly), static_cast<unsigned long>(file_share::read),
        nullptr, static_cast<unsigned long>(file_disposition::only_if_exists), static_cast<unsigned long>(
            file_flags::backup_semantics | file_flags::open_reparse_point), nullptr)};
    _FILESYSTEM_VERIFY_HANDLE(_Handle);

    FILETIME _File_time;
    if (!GetFileTime(_Handle, nullptr, &_File_time, nullptr)) { // failed to get file time
        CloseHandle(_Handle); // should be closed even if function will throw an exception
        _Throw_fs_error("failed to get file time", error_type::runtime_error, "last_access_time");
    }

    CloseHandle(_Handle);

    // convert file time to system time and then to user time zone
    SYSTEMTIME _Sys_gen_time; // general system time
    SYSTEMTIME _Sys_exact_time; // system time in user region
    _FILESYSTEM_VERIFY(FileTimeToSystemTime(&_File_time, &_Sys_gen_time),
        "failed to convert file time to system time", error_type::runtime_error);
    _FILESYSTEM_VERIFY(SystemTimeToTzSpecificLocalTimeEx(nullptr, &_Sys_gen_time, &_Sys_exact_time),
        "failed to convert general system time to exact system time", error_type::runtime_error);
    return {_Sys_exact_time.wYear, _Sys_exact_time.wMonth, _Sys_exact_time.wDay,
        _Sys_exact_time.wHour, _Sys_exact_time.wMinute, _Sys_exact_time.wSecond};
}

// FUNCTION last_write_time
_NODISCARD file_time last_write_time(const path& _Target) {
    _FILESYSTEM_VERIFY(exists(_Target), "target not found", error_type::runtime_error);

    const HANDLE _Handle{CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::readonly), static_cast<unsigned long>(file_share::read),
        nullptr, static_cast<unsigned long>(file_disposition::only_if_exists), static_cast<unsigned long>(
            file_flags::backup_semantics | file_flags::open_reparse_point), nullptr)};
    _FILESYSTEM_VERIFY_HANDLE(_Handle);

    FILETIME _File_time;
    if (!GetFileTime(_Handle, nullptr, nullptr, &_File_time)) { // failed to get file time
        CloseHandle(_Handle); // should be closed even if function will throw an exception
        _Throw_fs_error("failed to get file time", error_type::runtime_error, "last_write_time");
    }

    CloseHandle(_Handle);

    // convert file time to system time and then to user time zone
    SYSTEMTIME _Sys_gen_time; // general system time
    SYSTEMTIME _Sys_exact_time; // system time in user region
    _FILESYSTEM_VERIFY(FileTimeToSystemTime(&_File_time, &_Sys_gen_time),
        "failed to convert file time to system time", error_type::runtime_error);
    _FILESYSTEM_VERIFY(SystemTimeToTzSpecificLocalTimeEx(nullptr, &_Sys_gen_time, &_Sys_exact_time),
        "failed to convert general system time to exact system time", error_type::runtime_error);
    return {_Sys_exact_time.wYear, _Sys_exact_time.wMonth, _Sys_exact_time.wDay,
        _Sys_exact_time.wHour, _Sys_exact_time.wMinute, _Sys_exact_time.wSecond};
}

namespace experimental {
    // FUNCTION shortcut_parameters
    _NODISCARD shortcut_data shortcut_parameters(const path& _Target) {
        _FILESYSTEM_VERIFY(exists(_Target), "shortcut not found", error_type::runtime_error);
        _FILESYSTEM_VERIFY(_Target.extension() == "lnk", "expected a shortcut", error_type::runtime_error);

        IShellLinkW* _Link = {};
        WIN32_FIND_DATAW _Data = {}; // warning 6001 if not defined
        shortcut_data _Result = {};
        // this functions will replace characters; size must be defined before calling them
        _Result.arguments.resize(INFOTIPSIZE);
        _Result.description.resize(INFOTIPSIZE);
        _Result.directory.resize(_Max_path);
        _FILESYSTEM_VERIFY(CoInitialize(nullptr) == S_OK, "failed to initialize COM library", error_type::runtime_error);
        _FILESYSTEM_VERIFY(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_ALL, IID_IShellLinkW,
            reinterpret_cast<void**>(&_Link)) == S_OK, "failed to create COM object instance", error_type::runtime_error);

        IPersistFile* _File = {};
        wstring _Buff = wstring(); // buffer for shortcut icon/target path
        _Buff.resize(_Max_path); // resize before calling GetIconLocation()
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&_File)), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_File->Load(_Target.generic_wstring().c_str(), STGM_READ), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->Resolve(nullptr, 0), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->GetArguments(_Result.arguments.data(), INFOTIPSIZE), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->GetDescription(_Result.description.data(), INFOTIPSIZE), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->GetHotkey(&_Result.hotkey), _Link);
        _Buff.resize(_Max_path); // set default buffer size
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->GetIconLocation(_Buff.data(), _Max_path, &_Result.icon), _Link);
        _Result.icon_path = _Buff;
        _Buff.resize(_Max_path); // restore buffer size
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->GetIDList(&_Result.id_list), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->GetPath(_Buff.data(), _Max_path, &_Data, STGM_READ), _Link);
        _Result.target_path = _Buff;
        _Buff.resize(_Max_path); // restore buffer size
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->GetShowCmd(&_Result.show_cmd), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->GetWorkingDirectory(_Buff.data(), _Max_path), _Link);
        _Result.directory = _Buff;
        _File->Release();
        _Link->Release();
        return _Result;
    }

    _NODISCARD bool shortcut_parameters(const path& _Target, shortcut_data* const _Params) {
        _FILESYSTEM_VERIFY(exists(_Target), "shortcut not found", error_type::runtime_error);
        _FILESYSTEM_VERIFY(_Target.extension() == "lnk", "expected a shortcut", error_type::runtime_error);

        IShellLinkW* _Link{};
        _FILESYSTEM_VERIFY(CoInitialize(nullptr) == S_OK, "failed to initialize COM library", error_type::runtime_error);
        _FILESYSTEM_VERIFY(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_ALL, IID_IShellLinkW,
            reinterpret_cast<void**>(&_Link)) == S_OK, "failed to create COM object instance", error_type::runtime_error);

        IPersistFile* _File{};
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&_File)), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_File->Load(_Target.generic_wstring().c_str(), STGM_READ), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->Resolve(nullptr, 0), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->SetArguments(_Params->arguments.c_str()), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->SetDescription(_Params->description.c_str()), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->SetHotkey(_Params->hotkey), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->SetIconLocation(_Params->icon_path.generic_wstring().c_str(), _Params->icon), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->SetIDList(_Params->id_list), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->SetPath(_Params->target_path.generic_wstring().c_str()), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->SetShowCmd(_Params->show_cmd), _Link);
        _FILESYSTEM_VERIFY_COM_RESULT(_Link->SetWorkingDirectory(_Params->directory.generic_wstring().c_str()), _Link);
        _File->Release();
        _Link->Release();
        return true;
    }
} // experimental

// FUNCTION space
_NODISCARD disk_space space(const path& _Target) {
    disk_space _Result    = disk_space();
    const auto _Available = reinterpret_cast<PULARGE_INTEGER>(&_Result.available);
    const auto _Capacity  = reinterpret_cast<PULARGE_INTEGER>(&_Result.capacity);
    const auto _Free      = reinterpret_cast<PULARGE_INTEGER>(&_Result.free);
    _FILESYSTEM_VERIFY(GetDiskFreeSpaceExW(_Target.generic_wstring().c_str(), _Available,
        _Capacity, _Free), "failed to get informations", error_type::runtime_error);
    return _Result;
}

// FUNCTION status
_NODISCARD file_status status(const path& _Target) noexcept {
    // The status() is reserved for real files/directories,
    // so if _Target isn't one of them, read symbolic link/junction and return status of real one.
    const auto _Status{file_status(_Target)};
    if (_Status.type() == file_type::junction) {
        return file_status(read_junction(_Target));
    }

    if (_Status.type() == file_type::symlink) {
        return file_status(read_symlink(_Target));
    }

    return _Status;
}

// FUNCTION status_known
_NODISCARD bool status_known(const file_status _Status) noexcept {
    return _Status.type() != file_type::none;
}

_NODISCARD bool status_known(const path& _Target) noexcept {
    return status_known(file_status(_Target));
}

// FUNCTION symlink_status
_NODISCARD file_status symlink_status(const path& _Target) noexcept {
    const auto _Status{file_status(_Target)};

    // don't use is_symlink(), because it will refresh status twice
    if (_Status.type() != file_type::symlink) { // symlink_status() is reserved for symbolic links only
        return file_status(_Target, file_attributes::unknown, file_permissions::unknown, file_type::not_found);
    }

    return _Status;
}
_FILESYSTEM_END

#pragma warning(pop)
#endif // !_HAS_WINDOWS