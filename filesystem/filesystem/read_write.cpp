// read_write.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <pch.h>
#include <filesystem.hpp>

#if !_HAS_WINDOWS
#pragma message("The contents of <read_write.cpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv

_FILESYSTEM_BEGIN
// FUNCTION clear
_NODISCARD bool __cdecl clear(const path& _Target) { // if directory, removes everything inside _Target, otherwise clears file
    if (!exists(_Target)) { // path not found
        _Throw_fs_error("path not found", error_type::runtime_error, "clear");
    }

    if (!is_empty(_Target)) {
        if (_Is_directory(_Target)) {
            // don't use remove_all(), because it will remove _Target as well
            const auto _All = directory_data(_Target).total();

            for (const auto& _Elem : _All) { // remove one by one if _Target is directory
                if (_Is_directory(_Target + R"(\)" + _Elem) && !is_empty(_Target + R"(\)" + _Elem)) { // non-empty directory
                    // If we don't check if directory is empty and won't be, remove() will throw an exception.
                    (void) remove_all(_Target + R"(\)" + _Elem);
                } else { // regular file or empty directory
                    (void) remove(_Target + R"(\)" + _Elem);
                }
            }

            if (!is_empty(_Target)) { // failed to clear the directory
                _Throw_fs_error("failed to clear the directory", error_type::runtime_error, "clear");
            }

            return true;
        } else { // file, symlink or other
            (void) resize_file(_Target, 0);

            if (!is_empty(_Target)) { // failed to clear the file
                _Throw_fs_error("failed to clear the file", error_type::runtime_error, "clear");
            }

            return true;
        }
    }

    // if is empty, don't do anything
    return true;
}

// FUNCTION lines_count
_NODISCARD size_t __cdecl lines_count(const path& _Target) { // counts lines in _Target
    if (_Is_directory(_Target)) { // lines_count() is reserved for files only
        _Throw_fs_error("expected file", error_type::runtime_error, "lines_count");
    }
    
    return read_all(_Target).size();
}

// FUNCTION read_all
_NODISCARD vector<path> __cdecl read_all(const path& _Target) { // reads all lines in _Target (ignores last empty lines)
    if (!exists(_Target)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "read_all");
    }

    if (_Is_directory(_Target)) { // read_all() is reserved for files only
        _Throw_fs_error("expected file", error_type::runtime_error, "read_all");
    }

    if (!is_empty(_Target)) {
        ifstream _File;
        vector<path> _All;
        typename path::string_type _Single;

        _File.open(_Target.generic_wstring());
        if (!_File) { // bad file
            _Throw_fs_error("bad file", error_type::runtime_error, "read_all");
        }

        while (!_File.eof()) {
            if (_All.size() + 1 >= _All.max_size()) { // don't use max size
                _Throw_fs_error("file is too big", error_type::runtime_error, "read_all");
            }

            _STD getline(_File, _Single);
            _All.push_back(_Single);
            _Single.clear();
        }

        _File.close();

        while (_All.back().empty()) { // ignore last empty lines
            _All.pop_back();
        }

        return _All;
    }

    return vector<path>();
}

// FUNCTION read_back
_NODISCARD path __cdecl read_back(const path& _Target) { // reads last line in _Target
    if (_Is_directory(_Target)) { // read_back() is reserved for files only
        _Throw_fs_error("expected file", error_type::runtime_error, "read_back");
    }
    
    if (is_empty(_Target)) {
        return path();
    }
    
    const auto& _All = read_all(_Target);
    return _All[_All.size() - 1];
}

// FUNCTION read_first
_NODISCARD path __cdecl read_front(const path& _Target) { // reads first line in _Target
    if (!exists(_Target)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "read_front");
    }

    if (_Is_directory(_Target)) { // read_front() is reserved for files only
        _Throw_fs_error("expected file", error_type::runtime_error, "read_front");
    }

    if (!is_empty(_Target)) {
        ifstream _File;
        typename path::string_type _First;

        _File.open(_Target.generic_wstring());
        if (!_File) { // bad file
            _Throw_fs_error("bad", error_type::runtime_error, "read_front");
        }

        _STD getline(_File, _First);
        _File.close();
        return path(_First);
    }

    return path();
}

// FUNCTION read_inside
_NODISCARD path __cdecl read_inside(const path& _Target, const size_t _Line) { // reads _Line line from _Target
    if (_Is_directory(_Target)) { // read_inside() is reserved for files only
        _Throw_fs_error("expected file", error_type::runtime_error, "read_inside");
    }
    
    const auto& _All = read_all(_Target);

    if (_Line > _All.size() || _Line < 1) { // _Line grater than lines count or less than 1 (count starts from 1)
        _Throw_fs_error("invalid line", error_type::invalid_argument, "read_inside");
    }
    
    return _All[_Line - 1];
}

// FUNCTION read_junction
_NODISCARD path __cdecl read_junction(const path& _Target) {
    if (!is_junction(_Target)) { // _Target must be a junction
        _Throw_fs_error("expected junction", error_type::runtime_error, "read_junction");
    }

    const HANDLE _Handle = _CSTD CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::readonly | file_access::writeonly), 0, nullptr,
        static_cast<unsigned long>(file_disposition::only_if_exists), static_cast<unsigned long>(
            file_flags::backup_semantics | file_flags::open_reparse_point), nullptr);

    if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
        _Throw_fs_error("failed to get handle", error_type::runtime_error, "read_junction");
    }

    // In some cases read_junction() may using more bytes than defaule maximum (16384 bytes).
    // To avoid C6262 warning and potential threat, buffor size is set to 16284 bytes.
    // It shouldn't change result and it's safer. If your /analyse:stacksize is set to larger value,
    // you can change buffer size.
    unsigned char _Buff[MAXIMUM_REPARSE_DATA_BUFFER_SIZE - 100]; 
    reparse_data_buffer& _Reparse_buff = reinterpret_cast<reparse_data_buffer&>(_Buff);
    unsigned long _Bytes; // returned bytes from DeviceIoControl()

    if (!_CSTD DeviceIoControl(_Handle, FSCTL_GET_REPARSE_POINT, nullptr, 0,
        &_Reparse_buff, sizeof(_Buff), &_Bytes, nullptr)) { // failed to read junction
        _CSTD CloseHandle(_Handle); // close handle even if function will throw an exception
        _Throw_fs_error("failed to read junction", error_type::runtime_error, "read_junction");
    }

    _CSTD CloseHandle(_Handle);

    // check if result contains unnecessary content (prefix and sufix)
    wstring _Reparse(_Reparse_buff._Mount_point_reparse_buffer._Path_buffer);

    if (wstring(_Reparse, 0, 4) == LR"(\??\)") { // remove prefix
        _Reparse.erase(0, 4);
    }

    if (_Reparse.back() == L'\\') { // remove sufix
        _Reparse.pop_back();
    }

    return path(_Reparse);
}

// FUNCTION read_symlink
_NODISCARD path __cdecl read_symlink(const path& _Target) { // returns full path to target of symbolic link
    if (!is_symlink(_Target)) { // _Target must be a symbolic link
        _Throw_fs_error("symbolic link not found", error_type::runtime_error, "read_symlink");
    }

    const HANDLE _Handle = _CSTD CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::readonly), static_cast<unsigned long>(file_share::read), nullptr,
        static_cast<unsigned long>(file_disposition::only_if_exists), static_cast<unsigned long>(
            file_flags::backup_semantics | file_flags::open_reparse_point), nullptr);

    if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
        _Throw_fs_error("failed to get handle", error_type::runtime_error, "read_symlink");
    }

    // In some cases read_symlink() may using more bytes than defaule maximum (16384 bytes).
    // To avoid C6262 warning and potential threat, buffor size is set to 15734 bytes.
    // It shouldn't change result and it's safer. If your /analyse:stacksize is set to larger value,
    // you can change buffer size.
    unsigned char _Buff[MAXIMUM_REPARSE_DATA_BUFFER_SIZE - 650];
    reparse_data_buffer& _Reparse_buff = reinterpret_cast<reparse_data_buffer&>(_Buff);
    unsigned long _Bytes; // returnd bytes from DeviceIoControl()

    if (!_CSTD DeviceIoControl(_Handle, FSCTL_GET_REPARSE_POINT, nullptr, 0,
        &_Reparse_buff, sizeof(_Buff), &_Bytes, nullptr)) { // failed to read symlink
        _CSTD CloseHandle(_Handle); // should be closed even if function will throw an exception
        _Throw_fs_error("failed to read sybmolic link", error_type::runtime_error, "read_symlink");
    }

    _CSTD CloseHandle(_Handle);

    const size_t _Len  = _Reparse_buff._Symbolic_link_reparse_buffer._Substitute_name_length / sizeof(wchar_t) + 1;
    wchar_t* _Sub_name = new wchar_t[_Len + 1];

    _CSTD wcsncpy_s(_Sub_name, _Len + 1, &_Reparse_buff._Symbolic_link_reparse_buffer._Path_buffer[
        _Reparse_buff._Symbolic_link_reparse_buffer._Substitute_name_offset / sizeof(wchar_t) + 1], _Len);
    _Sub_name[_Len] = L'\0'; // C string must ends with 0

    // first char is copy of last and must be skiped, because with him result will be incorrect
    wstring _Reparse(_Sub_name, 1, _CSTD wcslen(_Sub_name) - 1);
    delete[] _Sub_name; // free memory from _Sum_name, won't be used any more

    // in some cases, first 4 characters may be a prefix, they're unnecessary
    if (wstring(_Reparse, 0, 4) == LR"(\??\)") { // remove prefix
        _Reparse.erase(0, 4);
    }

    return path(_Reparse);
}

// FUNCTION resize_file
_NODISCARD bool __cdecl resize_file(const path& _Target, const size_t _Newsize) {
    if (!exists(_Target)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "resize_file");
    }

    if (_Is_directory(_Target)) { // resize_file() is reserved for files only
        _Throw_fs_error("expected file", error_type::runtime_error, "resize_file");
    }

    const HANDLE _Handle = _CSTD CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<unsigned long>(file_access::writeonly), static_cast<unsigned long>(file_share::read
            | file_share::write | file_share::remove), nullptr, static_cast<unsigned long>(file_disposition::only_if_exists),
        static_cast<unsigned long>(file_attributes::none), nullptr);

    if (_Handle == INVALID_HANDLE_VALUE) { // failed to open handle
        _Throw_fs_error("failed to get handle", error_type::runtime_error, "resize_file");
    }

    LARGE_INTEGER _Large;
    _Large.QuadPart = static_cast<long long>(_Newsize);

    if (!_CSTD SetFilePointerEx(_Handle, _Large, nullptr, FILE_BEGIN)
        || !_CSTD SetEndOfFile(_Handle)) { // failed to resize file
        _CSTD CloseHandle(_Handle); // close handle even if function will throw an exception
        _Throw_fs_error("failed to resize file", error_type::runtime_error, "resize_file");
    }

    _CSTD CloseHandle(_Handle);

    return true;
}

// FUNCTION write_back
_NODISCARD bool __cdecl write_back(const path& _Target, const path& _Writable) {
    if (!exists(_Target)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "write_back");
    }

    if (_Is_directory(_Target)) { // write_back() is reserved for files only
        _Throw_fs_error("expected file", error_type::runtime_error, "write_back");
    }

    // without "\n" on first position, _Writable will be added to existing line (if exists)
    const auto&& _Safe = is_empty(_Target) ? _Writable.generic_string() : "\n" + _Writable.generic_string();

    ofstream _File;	
    _File.open(_Target.generic_string(), ios::ate | ios::in | ios::out); // without ios::in, all content will be removed

    if (!_File) {
        if (_File.is_open()) { // if is open, close
            _File.close();
        }

        _Throw_fs_error("bad file", error_type::runtime_error, "write_back");
    }

    _File.write(_Safe.c_str(), _Safe.size());
    _File.close();

    if (read_back(_Target) != _Writable) { // failed to overwritte the file
        _Throw_fs_error("failed to overwritte the file", error_type::runtime_error, "write_back");
    }

    return true;
}

// FUNCTION write_front
_NODISCARD bool __cdecl write_front(const path& _Target, const path& _Writable) {
    if (!exists(_Target)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "write_back");
    }

    if (_Is_directory(_Target)) { // write_front() is reserved for files only
        _Throw_fs_error("expected file", error_type::runtime_error, "write_back");
    }

    // if is empty, write _Writable using write_back() and finish here
    if (is_empty(_Target)) {
        (void) write_back(_Target, _Writable);

        if (read_front(_Target) != _Writable) { // failed to overrite file
            _Throw_fs_error("failed to overwrite file", error_type::runtime_error, "write_end");
        }

        return true;
    }

    // If isn't empty, the file must be overwritten with good content,
    // otherwise the file will contains unnecessary empty lines
    // and sometimes contents from different lines will be in the same line.
    vector<path> _All{_Writable};
    
    for (const auto& _Line : read_all(_Target)) {
        _All.push_back(_Line); // after added _Writable, add all content from read_all()
    }

    // use resize_file() instead of clear(), because we knows that _Target is file
    (void) resize_file(_Target, 0);

    // don't add end of line, because write_back() will do it for us
    for (const auto& _Line : _All) {
        (void) write_back(_Target, _Line);
    }

    if (read_front(_Target) != _Writable) { // failed to overwrite file
        _Throw_fs_error("failed to overwrite file", error_type::runtime_error, "write_front");
    }
    
    return true;
}

// FUNCTION write_inside
_NODISCARD bool __cdecl write_inside(const path& _Target, const path& _Writable, const size_t  _Line) {
    if (!exists(_Target)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "write_inside");
    }

    if (_Is_directory(_Target)) { // write_inside() is reserved for files only
        _Throw_fs_error("expected file", error_type::runtime_error, "write_inside");
    }

    const auto& _All   = read_all(_Target);
    const auto& _Count = _All.size();

    // Second condition is important, because user may want to write 
    // _Writable in the first line, in empty file. If it won't be checked,
    // write_inside() will throw exception.
    if (_Line < 1 || (_Count > 0 && _Line > _Count)) {
        _Throw_fs_error("invalid line", error_type::invalid_argument, "write_inside");
    }

    // if _Target is an empty file, write _Writable at the end of the file
    // and return write_back() result
    if (_Count == 0) { // empty file
        return write_back(_Target, _Writable);
    }

    if (_Line == 1) { // write in first line, in _Target and return write_front() result
        return write_front(_Target, _Writable);
    }

    auto _Write       = _All;
    const auto& _Iter = _Write.begin();

    _Write.insert(_Iter + (_Line - 1), _Writable);

    // clear file and write the newest content,
    // use resize_file() instead of clear(), because we knows that _Target is file
    (void) resize_file(_Target, 0);

    for (const auto& _Elem : _Write) {
        (void) write_back(_Target, _Elem);
    }

    if (read_inside(_Target, _Line) != _Writable) { // failed to overwrite file
        _Throw_fs_error("failed to overwrite the file", error_type::runtime_error, "write_inside");
    }

    return true;
}

// FUNCTION write_instead
_NODISCARD bool __cdecl write_instead(const path& _Target, const path& _Writable, const size_t _Line) {
    if (!exists(_Target)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "write_instead");
    }

    if (_Is_directory(_Target)) { // write_instead() is reserved for files only
        _Throw_fs_error("expected file", error_type::runtime_error, "write_instead");
    }

    auto _All          = read_all(_Target);
    const auto& _Count = _All.size();

    if (_Line < 1 || _Line > _Count) { // invalid line
        _Throw_fs_error("invalid line", error_type::invalid_argument, "write_instead");
    }

    _All[_Line - 1] = _Writable; // swap content

    // clear _Target and write to him the newest content,
    // use resize_file() instead of clear(), because we knows that _Target is file

    for (const auto& _Elem : _All) {
        (void) write_back(_Target, _Elem);
    }

    if (read_inside(_Target, _Line) != _Writable) { // failed to replace line
        _Throw_fs_error("failed to overwrite the  file", error_type::runtime_error, "write_instead");
    }

    return true;
}
_FILESYSTEM_END

#endif // !_HAS_WINDOWS