// read_write.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <filesystem_pch.hpp>
#include <filesystem.hpp>

#if !_HAS_WINDOWS
#pragma message("The contents of <read_write.cpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv

_FILESYSTEM_BEGIN
// FUNCTION clear
_NODISCARD bool clear(const path& _Target) { // if directory, removes everything inside _Target, otherwise clears file
    _FILESYSTEM_VERIFY(exists(_Target), "target not found", error_type::runtime_error);
    if (!is_empty(_Target)) {
        if (_Is_directory(_Target)) {
            // don't use remove_all(), because it will remove _Target as well
            const directory_data _Dir(_Target);
            for (const auto& _Elem : _Dir.total()) { // remove one by one if _Target is directory
                if (_Is_directory(_Target + R"(\)" + _Elem) && !is_empty(_Target + R"(\)" + _Elem)) { // non-empty directory
                    // If we don't check if directory is empty and won't be, remove() will throw an exception.
                    (void) remove_all(_Target + R"(\)" + _Elem);
                } else { // regular file or empty directory
                    (void) remove(_Target + R"(\)" + _Elem);
                }
            }

            _FILESYSTEM_VERIFY(is_empty(_Target), "failed to clear the directory", error_type::runtime_error);
            return true;
        } else { // file, symlink or other
            (void) resize_file(_Target, 0);
            _FILESYSTEM_VERIFY(is_empty(_Target), "failed to clear the file", error_type::runtime_error);
            return true;
        }
    }

    // if is empty, don't do anything
    return true;
}

// FUNCTION lines_count
_NODISCARD uintmax_t lines_count(const path& _Target) { // counts lines in _Target
    _FILESYSTEM_VERIFY(!_Is_directory(_Target), "expected the file", error_type::runtime_error);
    return read_all(_Target).size();
}

// FUNCTION read_all
_NODISCARD vector<string> read_all(const path& _Target) { // reads all lines in _Target (ignores last empty lines)
    _FILESYSTEM_VERIFY(exists(_Target), "file not found", error_type::runtime_error);
    _FILESYSTEM_VERIFY(!_Is_directory(_Target), "file not found", error_type::runtime_error);

    if (!is_empty(_Target)) {
        ifstream _Stream;
        vector<string> _All;
        string _Buff;

        _Stream.open(_Target.generic_wstring());
        _FILESYSTEM_VERIFY_FILE_STREAM(_Stream);

        while (!_Stream.eof()) {
            if (_All.size() + 1 >= _All.max_size()) { // don't use max size
                _Throw_fs_error("file is too big", error_type::runtime_error, "read_all");
            }

            _STD getline(_Stream, _Buff);
            _All.push_back(_Buff);
            _Buff.clear();
        }

        _Stream.close();
        while (_All.back().empty()) { // ignore last empty lines
            _All.pop_back();
        }

        return _All;
    }

    return vector<string>();
}

// FUNCTION read_back
_NODISCARD string read_back(const path& _Target) { // reads last line in _Target
    _FILESYSTEM_VERIFY(!_Is_directory(_Target), "expected a file", error_type::runtime_error);

    if (is_empty(_Target)) {
        return string();
    }
    
    return read_all(_Target).back();
}

// FUNCTION read_first
_NODISCARD string read_front(const path& _Target) { // reads first line in _Target
    _FILESYSTEM_VERIFY(exists(_Target), "file not found", error_type::runtime_error);
    _FILESYSTEM_VERIFY(!_Is_directory(_Target), "expected a file", error_type::runtime_error);

    if (!is_empty(_Target)) {
        ifstream _Stream;
        string _Buff;

        _Stream.open(_Target.generic_wstring());
        _FILESYSTEM_VERIFY_FILE_STREAM(_Stream);

        _STD getline(_Stream, _Buff);
        _Stream.close();
        return _Buff;
    }

    return string();
}

// FUNCTION read_inside
_NODISCARD string read_inside(const path& _Target, const uintmax_t _Line) { // reads _Line line from _Target
    _FILESYSTEM_VERIFY(!_Is_directory(_Target), "expected a file", error_type::runtime_error);

    const auto& _All{read_all(_Target)};
    _FILESYSTEM_VERIFY(_Line <= _All.size() && _Line > 0, "invalid line", error_type::runtime_error);  
#ifdef _M_X64
    return _All[_Line - 1];
#else // ^^^ _M_X64 ^^^ / vvv _M_IX86 vvv
    return _All[static_cast<uint32_t>(_Line) - 1];
#endif // _M_X64
}

// FUNCTION read_junction
_NODISCARD path read_junction(const path& _Target) {
    _FILESYSTEM_VERIFY(is_junction(_Target), "expected a junction", error_type::runtime_error);

    const HANDLE _Handle{CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::readonly | file_access::writeonly), 0, nullptr,
        static_cast<unsigned long>(file_disposition::only_if_exists), static_cast<unsigned long>(
            file_flags::backup_semantics | file_flags::open_reparse_point), nullptr)};
    _FILESYSTEM_VERIFY_HANDLE(_Handle);

    // In some cases read_junction() may using more bytes than defaule maximum (16384 bytes).
    // To avoid C6262 warning and potential threat, buffor size is set to 16284 bytes.
    // It shouldn't change result and it's safer. If your /analyse:stacksize is set to larger value,
    // you can change buffer size.
    unsigned char _Buff[MAXIMUM_REPARSE_DATA_BUFFER_SIZE - 100] = {};
    reparse_data_buffer& _Reparse_buff                          = reinterpret_cast<reparse_data_buffer&>(_Buff);
    unsigned long _Bytes                                        = 0; // returned bytes from DeviceIoControl()

    if (!DeviceIoControl(_Handle, FSCTL_GET_REPARSE_POINT, nullptr, 0,
        &_Reparse_buff, sizeof(_Buff), &_Bytes, nullptr)) { // failed to read junction
        CloseHandle(_Handle); // close handle even if function will throw an exception
        _Throw_fs_error("failed to read junction", error_type::runtime_error, "read_junction");
    }

    CloseHandle(_Handle);

    // check if result contains unnecessary content (prefix and sufix)
    wstring _Reparse{_Reparse_buff._Mount_point_reparse_buffer._Path_buffer};

    if (wstring(_Reparse, 0, 4) == LR"(\??\)") { // remove prefix
        _Reparse.erase(0, 4);
    }

    if (_Reparse.back() == L'\\') { // remove sufix
        _Reparse.pop_back();
    }

    return path(_Reparse);
}

// FUNCTION read_shortcut
_NODISCARD path read_shortcut(const path& _Target) {
    _FILESYSTEM_VERIFY(exists(_Target), "shortcut not found", error_type::runtime_error);
    _FILESYSTEM_VERIFY(_Target.extension() == "lnk", "expected a shortcut", error_type::runtime_error);

    IShellLinkW* _Link     = {};
    WIN32_FIND_DATAW _Data = {}; // warning C6001 if not defined
    _FILESYSTEM_VERIFY(CoInitialize(nullptr) == S_OK, "failed to initialize COM library", error_type::runtime_error);
    _FILESYSTEM_VERIFY(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_ALL, IID_IShellLinkW,
        reinterpret_cast<void**>(&_Link)) == S_OK, "failed to create COM object instance", error_type::runtime_error);
    IPersistFile* _File      = {};
    wchar_t _Buff[_Max_path] = {}; // buffer for shortcut target path
    _FILESYSTEM_VERIFY_COM_RESULT(_Link->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&_File)), _Link);
    _FILESYSTEM_VERIFY_COM_RESULT(_File->Load(_Target.generic_wstring().c_str(), STGM_READ), _Link);
    _FILESYSTEM_VERIFY_COM_RESULT(_Link->Resolve(nullptr, 0), _Link);
    _FILESYSTEM_VERIFY_COM_RESULT(_Link->GetPath(_Buff, _Max_path, &_Data, SLGP_SHORTPATH), _Link);
    _File->Release();
    _Link->Release();
    return path(static_cast<const wchar_t*>(_Buff));
}

#pragma warning(push, 1)
#pragma warning(disable : 4067) // C4067: token after preprocessor directive (?)
// FUNCTION read_symlink
_NODISCARD path read_symlink(const path& _Target) { // returns full path to target of symbolic link
    _FILESYSTEM_VERIFY(is_symlink(_Target), "expected a symbolic link", error_type::runtime_error);

    const HANDLE _Handle{CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::readonly), static_cast<unsigned long>(file_share::read), nullptr,
        static_cast<unsigned long>(file_disposition::only_if_exists), static_cast<unsigned long>(
            file_flags::backup_semantics | file_flags::open_reparse_point), nullptr)};
    _FILESYSTEM_VERIFY_HANDLE(_Handle);

    // In some cases read_symlink() may using more bytes than defaule maximum (16384 bytes).
    // To avoid C6262 warning and potential threat, buffor size is set to 15734 bytes.
    // It shouldn't change result and it's safer. If your /analyse:stacksize is set to larger value,
    // you can change buffer size.
    unsigned char _Buff[MAXIMUM_REPARSE_DATA_BUFFER_SIZE - 650] = {};
    reparse_data_buffer& _Reparse_buff                          = reinterpret_cast<reparse_data_buffer&>(_Buff);
    unsigned long _Bytes                                        = 0; // returnd bytes from DeviceIoControl()

    if (!DeviceIoControl(_Handle, FSCTL_GET_REPARSE_POINT, nullptr, 0,
        &_Reparse_buff, sizeof(_Buff), &_Bytes, nullptr)) { // failed to read symlink
        CloseHandle(_Handle); // should be closed even if function will throw an exception
        _Throw_fs_error("failed to read sybmolic link", error_type::runtime_error, "read_symlink");
    }

    CloseHandle(_Handle);
    const size_t _Length = _Reparse_buff._Symbolic_link_reparse_buffer._Substitute_name_length / sizeof(wchar_t) + 1;
    wchar_t* _Sub_name   = new wchar_t[_Length + 1];

    _CSTD wcsncpy_s(_Sub_name, _Length + 1, &_Reparse_buff._Symbolic_link_reparse_buffer._Path_buffer[
        _Reparse_buff._Symbolic_link_reparse_buffer._Substitute_name_offset / sizeof(wchar_t) + 1], _Length);
    _Sub_name[_Length] = L'\0'; // C string must ends with 0

    // first char is copy of last and must be skiped, because with him result will be incorrect
#if __has_builtin(__builtin_wcslen)
    wstring _Reparse(_Sub_name, 1, __builtin_wcslen(_Sub_name) - 1);
#else // ^^^ __has_builtin(__builtin_wcslen) ^^^ / vvv !__has_builtin(__builtin_wcslen) vvv
    wstring _Reparse(_Sub_name, 1, _CSTD wcslen(_Sub_name) - 1);
#endif // __has_builtin(__builtin_wcslen)
    delete[] _Sub_name; // free memory from _Sum_name, won't be used any more

    // in some cases, first 4 characters may be a prefix, they're unnecessary
    if (wstring(_Reparse, 0, 4) == LR"(\??\)") { // remove prefix
        _Reparse.erase(0, 4);
    }

    return path(_Reparse);
}
#pragma warning(default : 4067)
#pragma warning(pop)

// FUNCTION resize_file
_NODISCARD bool resize_file(const path& _Target, const size_t _Newsize) {
    _FILESYSTEM_VERIFY(exists(_Target), "file not found", error_type::runtime_error);
    _FILESYSTEM_VERIFY(!_Is_directory(_Target), "expected a file", error_type::runtime_error);

    const HANDLE _Handle{CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::writeonly), static_cast<unsigned long>(file_share::read
            | file_share::write | file_share::remove), nullptr, static_cast<unsigned long>(file_disposition::only_if_exists),
        static_cast<unsigned long>(file_attributes::none), nullptr)};
    _FILESYSTEM_VERIFY_HANDLE(_Handle);

    LARGE_INTEGER _Large = LARGE_INTEGER();
    _Large.QuadPart = static_cast<long long>(_Newsize);
    if (!SetFilePointerEx(_Handle, _Large, nullptr, FILE_BEGIN)
        || !SetEndOfFile(_Handle)) { // failed to resize file
        CloseHandle(_Handle); // close handle even if function will throw an exception
        _Throw_fs_error("failed to resize file", error_type::runtime_error, "resize_file");
    }

    CloseHandle(_Handle);
    return true;
}

// FUNCTION TEMPLATE write_back
template <class _CharTy>
_NODISCARD constexpr bool write_back(const path& _Target, const _CharTy* const _Writable) {
    _FILESYSTEM_VERIFY(exists(_Target), "file not found", error_type::runtime_error);
    _FILESYSTEM_VERIFY(!_Is_directory(_Target), "expected a file", error_type::runtime_error);

    // without "\n" on first position, _Writable will be added to the existing line (if exists)
    const auto& _Correct = is_empty(_Target) ?
        _Convert_to_narrow<_CharTy, char_traits<_CharTy>>(_Writable)
        : "\n" + _Convert_to_narrow<_CharTy, char_traits<_CharTy>>(_Writable);
    ofstream _Stream;
    _Stream.open(_Target.generic_string(), ios::ate | ios::in | ios::out); // without ios::in, all content will be removed
    _FILESYSTEM_VERIFY_FILE_STREAM(_Stream);

    _Stream.write(_Correct.c_str(), _Correct.size());
    _Stream.close();

    // The _FILESYSTEM_VERIFY() don't accepts commas in template,
    // so result of _Convert_to_narrow() must be as constant variable.
    const string& _Narrow{_Convert_to_narrow<_CharTy, char_traits<_CharTy>>(_Writable)};
    _FILESYSTEM_VERIFY(read_back(_Target) == _Narrow, "failed to overwrite the file", error_type::runtime_error);
    return true;
}

template _FILESYSTEM_API _NODISCARD bool write_back(const path&, const char* const);
template _FILESYSTEM_API _NODISCARD bool write_back(const path&, const char8_t* const);
template _FILESYSTEM_API _NODISCARD bool write_back(const path&, const char16_t* const);
template _FILESYSTEM_API _NODISCARD bool write_back(const path&, const char32_t* const);
template _FILESYSTEM_API _NODISCARD bool write_back(const path&, const wchar_t* const);

// FUNCTION TEMPLATE write_front
template <class _CharTy>
_NODISCARD constexpr bool write_front(const path& _Target, const _CharTy* const _Writable) {
    _FILESYSTEM_VERIFY(exists(_Target), "file not found", error_type::runtime_error);
    _FILESYSTEM_VERIFY(!_Is_directory(_Target), "expected a file", error_type::runtime_error);

    // _Narrow_writable will be used few times
    const string& _Narrow_writable{_Convert_to_narrow<_CharTy, char_traits<_CharTy>>(_Writable)};

    // if is empty, write _Writable using write_back() and finish here
    if (is_empty(_Target)) {
        (void) write_back(_Target, _Narrow_writable.c_str()); // no needs to convert
        _FILESYSTEM_VERIFY(read_front(_Target) == _Narrow_writable, "failed to overwrite the file", error_type::runtime_error);
        return true;
    }

    // If isn't empty, the file must be overwritten with good content,
    // otherwise the file will contains unnecessary empty lines
    // and sometimes contents from different lines will be in the same line.
    vector<string> _All{_Narrow_writable};
    for (const auto& _Line : read_all(_Target)) {
        _All.push_back(_Line); // after added _Narrow_writable, add all content from read_all()
    }

    // use resize_file() instead of clear(), because we knows that _Target is file
    (void) resize_file(_Target, 0);

    // don't add end of line, because write_back() will do it for us
    for (const auto& _Line : _All) {
        (void) write_back(_Target, _Line.c_str());
    }

    _FILESYSTEM_VERIFY(read_front(_Target) == _Narrow_writable, "failed to overwrite the file", error_type::runtime_error);
    return true;
}

template _FILESYSTEM_API _NODISCARD bool write_front(const path&, const char* const);
template _FILESYSTEM_API _NODISCARD bool write_front(const path&, const char8_t* const);
template _FILESYSTEM_API _NODISCARD bool write_front(const path&, const char16_t* const);
template _FILESYSTEM_API _NODISCARD bool write_front(const path&, const char32_t* const);
template _FILESYSTEM_API _NODISCARD bool write_front(const path&, const wchar_t* const);

// FUNCTION TEMPLATE write_inside
template <class _CharTy>
_NODISCARD constexpr bool write_inside(const path& _Target, const _CharTy* const _Writable, const uintmax_t _Line) {
    _FILESYSTEM_VERIFY(exists(_Target), "file not found", error_type::runtime_error);
    _FILESYSTEM_VERIFY(!_Is_directory(_Target), "expected a file", error_type::runtime_error);
    const auto& _All    = read_all(_Target);
    const size_t _Count = _All.size();

    // Second condition is important, because user may want to write 
    // _Writable in the first line, in empty file. If it won't be checked,
    // write_inside() will throw exception.
    _FILESYSTEM_VERIFY(_Line > 0 && (_Count > 0 && _Line <= _Count), "invalid line", error_type::runtime_error);

    // _Narrow_writable will be used few times
    const string& _Narrow_writable{_Convert_to_narrow<_CharTy, char_traits<_CharTy>>(_Writable)};

    // if _Target is an empty file, write _Writable at the end of the file
    // and return write_back() result
    if (_Count == 0) { // empty file
        return write_back(_Target, _Narrow_writable.c_str()); // no needs to convert
    }

    if (_Line == 1) { // write in first line, in _Target and return write_front() result
        return write_front(_Target, _Narrow_writable.c_str()); // no needs to convert
    }

    auto _Write{_All};
#ifdef _M_X64
    _Write.insert(_Write.begin() + (_Line - 1), _Narrow_writable);
#else // ^^^ _M_X64 ^^^ / vvv _M_IX86 vvv
    _Write.insert(_Write.begin() + (static_cast<uint32_t>(_Line) - 1), _Narrow_writable);
#endif // _M_X64

    // clear file and write the newest content,
    // use resize_file() instead of clear(), because we knows that _Target is file
    (void) resize_file(_Target, 0);

    for (const auto& _Elem : _Write) {
        (void) write_back(_Target, _Elem.c_str());
    }

    _FILESYSTEM_VERIFY(read_inside(_Target, _Line) == _Narrow_writable, "failed to overwrite the file", error_type::runtime_error);
    return true;
}

template _FILESYSTEM_API _NODISCARD bool write_inside(const path& _Target, const char* const, const uintmax_t);
template _FILESYSTEM_API _NODISCARD bool write_inside(const path& _Target, const char8_t* const, const uintmax_t);
template _FILESYSTEM_API _NODISCARD bool write_inside(const path& _Target, const char16_t* const, const uintmax_t);
template _FILESYSTEM_API _NODISCARD bool write_inside(const path& _Target, const char32_t* const, const uintmax_t);
template _FILESYSTEM_API _NODISCARD bool write_inside(const path& _Target, const wchar_t* const, const uintmax_t);

// FUNCTION TEMPLATE write_instead
template <class _CharTy>
_NODISCARD constexpr bool write_instead(const path& _Target, const _CharTy* const _Writable, const uintmax_t _Line) {
    _FILESYSTEM_VERIFY(exists(_Target), "file not found", error_type::runtime_error);
    _FILESYSTEM_VERIFY(!_Is_directory(_Target), "expected a file", error_type::runtime_error);
    const string& _Narrow_writable = _Convert_to_narrow<_CharTy, char_traits<_CharTy>>(_Writable);
    auto _All                      = read_all(_Target);
    const size_t _Count            = _All.size();

    _FILESYSTEM_VERIFY(_Line > 0 && _Line <= _Count, "invalid line", error_type::runtime_error);
#ifdef _M_X64
    _All[_Line - 1] = _Narrow_writable; // swap content
#else // ^^^ _M_X64 ^^^ / vvv _M_IX86 vvv
    _All[static_cast<uint32_t>(_Line) - 1] = _Narrow_writable; // swap content
#endif // _M_X64

    // clear _Target and write to him the newest content,
    // use resize_file() instead of clear(), because we knows that _Target is file
    (void) resize_file(_Target, 0);

    for (const auto& _Elem : _All) {
        (void) write_back(_Target, _Elem.c_str());
    }

    _FILESYSTEM_VERIFY(read_inside(_Target, _Line) == _Narrow_writable, "failed to overwrite the file", error_type::runtime_error);
    return true;
}

template _FILESYSTEM_API _NODISCARD bool write_instead(const path& _Target, const char* const, const uintmax_t);
template _FILESYSTEM_API _NODISCARD bool write_instead(const path& _Target, const char8_t* const, const uintmax_t);
template _FILESYSTEM_API _NODISCARD bool write_instead(const path& _Target, const char16_t* const, const uintmax_t);
template _FILESYSTEM_API _NODISCARD bool write_instead(const path& _Target, const char32_t* const, const uintmax_t);
template _FILESYSTEM_API _NODISCARD bool write_instead(const path& _Target, const wchar_t* const, const uintmax_t);
_FILESYSTEM_END

#endif // !_HAS_WINDOWS