// read_write.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <pch.h>
#include <filesystem.hpp>

#if !_HAS_WINDOWS
#pragma message("The contents of <read_write.cpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv

_FILESYSTEM_BEGIN
_EXPERIMENTAL_BEGIN
// FUNCTION clear
_NODISCARD bool __cdecl clear(const path& _Target) { // if directory, removes everything inside _Target, otherwise clears file
    if (!exists(_Target)) { // path not found
        _Throw_fs_error("path not found", error_type::runtime_error, "clear");
    }

    if (!is_empty(_Target)) {
        if (is_directory(_Target) || is_junction(_Target)
            || _CSTD PathIsDirectoryW(_Target.generic_wstring().c_str())) {
            // don't use remove_all(), because it will remove _Target as well
            const auto _All = directory_data(_Target).total();

            for (const auto& _Elem : _All) { // remove one by one if _Target is directory
                remove(_Target + R"(\)" + _Elem); // requires full path
            }

            if (!is_empty(_Target)) { // failed to clear the directory
                _Throw_fs_error("failed to clear the directory", error_type::runtime_error, "clear");
            }

            // if won't throw an exception, will be able to return true
            return true;
        } else { // file, symlink or other
            ofstream _File;
            _File.open(_Target.generic_wstring());

            if (!_File) { // bad file
                _Throw_fs_error("bad file", error_type::runtime_error, "clear");
            }

            _File.clear();
            _File.close();

            if (!is_empty(_Target)) { // failed to clear the file
                _Throw_fs_error("failed to clear the file", error_type::runtime_error, "clear");
            }

            // same return as above
            return true;
        }
    }

    // if is empty, don't do anything
    return true;
}

// FUNCTION lines_count
_NODISCARD size_t __cdecl lines_count(const path& _Target) { // counts lines in _Target
    return read_all(_Target).size();
}

// FUNCTION read_all
_NODISCARD vector<path> __cdecl read_all(const path& _Target) { // reads all lines in _Target (ignores last empty lines)
    if (!exists(_Target)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "read_all");
    }

    if (!is_empty(_Target)) {
        ifstream _File;
        vector<path> _All;
        path::string_type _Single;

        _File.open(_Target.generic_wstring());
        if (!_File) { // bad file
            _Throw_fs_error("bad file", error_type::runtime_error, "read_all");
        }

        while (!_File.eof()) {
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
    if (is_empty(_Target)) {
        return path();
    }
    
    const auto _All  = read_all(_Target);
    const auto _Last = _All.size() - 1;
    return _All[_Last];
}

// FUNCTION read_first
_NODISCARD path __cdecl read_front(const path& _Target) { // reads first line in _Target
    if (!exists(_Target)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "read_first");
    }

    if (!is_empty(_Target)) {
        ifstream _File;
        path::string_type _First;

        _File.open(_Target.generic_wstring());
        if (!_File) { // bad file
            _Throw_fs_error("bad", error_type::runtime_error, "read_first");
        }

        _STD getline(_File, _First);
        _File.close();
        return path(_First);
    }

    return path();
}

// FUNCTION read_inside
_NODISCARD path __cdecl read_inside(const path& _Target, const size_t _Line) { // reads _Line line from _Target
    const auto _All   = read_all(_Target);
    const auto _Count = _All.size();

    if (_Line > _Count || _Line < 1) { // _Line grater than lines count or less than 1 (count starts from 1)
        _Throw_fs_error("invalid line", error_type::invalid_argument, "read_inside");
    }
    
    return _All[_Line - 1];
}

// FUNCTION read_junction
_NODISCARD path __cdecl read_junction(const path& _Target) {
    if (!is_junction(_Target)) { // _Target must be a junction
        _Throw_fs_error("expected junction", error_type::runtime_error, "read_junction");
    }

    const HANDLE _Handle = CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<DWORD>(file_access::readonly | file_access::writeonly), 0, nullptr,
        static_cast<DWORD>(file_disposition::only_if_exists), static_cast<DWORD>(
            file_flags::backup_semantics | file_flags::open_reparse_point), nullptr);

    if (_Handle == INVALID_HANDLE_VALUE) { // failed to get handle
        _Throw_fs_error("failed to get handle", error_type::runtime_error, "read_junction");
    }

    // in some cases buffer size may be larger than expeceted (16 * 1024)
    unsigned char _Buff[MAXIMUM_REPARSE_DATA_BUFFER_SIZE - 100]; 
    reparse_data_buffer& _Reparse_buff = reinterpret_cast<reparse_data_buffer&>(_Buff);
    unsigned long _Returned; // returned bytes

    _CSTD DeviceIoControl(_Handle, FSCTL_GET_REPARSE_POINT, nullptr, 0,
        &_Reparse_buff, sizeof(_Buff), &_Returned, nullptr);
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
_NODISCARD path __cdecl read_symlink(const path& _Target) {
    return path();
}

// FUNCTION resize_file
_NODISCARD bool __cdecl resize_file(const path& _Target, const size_t _Newsize) {
    if (!exists(_Target)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "resize_file");
    }

    const HANDLE _Handle = _CSTD CreateFileW(_Target.generic_wstring().c_str(),
        static_cast<DWORD>(file_access::writeonly), static_cast<DWORD>(file_share::read
            | file_share::write | file_share::remove), nullptr, static_cast<DWORD>(file_disposition::only_if_exists),
        static_cast<DWORD>(file_attributes::none), nullptr);

    if (_Handle == INVALID_HANDLE_VALUE) { // failed to open handle
        _Throw_fs_error("failed to get handle", error_type::runtime_error, "resize_file");
    }

    LARGE_INTEGER _Large;
    _Large.QuadPart = static_cast<LONGLONG>(_Newsize);

    const bool _Result = _CSTD SetFilePointerEx(_Handle, _Large, nullptr, FILE_BEGIN)
        && _CSTD SetEndOfFile(_Handle);

    if (!_Result) { // failed to resize file
        _Throw_fs_error("failed to resize file", error_type::runtime_error, "resize_file");
    }

    _CSTD CloseHandle(_Handle);
    return _Result;
}

// FUNCTION write_back
_NODISCARD bool __cdecl write_back(const path& _Target, const path& _Writable) {
    if (!exists(_Target)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "write_back");
    }

    // without "\n" on first position, _Writable will be added to existing line (if exists)
    const path::string_type _Safe = is_empty(_Target) ?
        _Writable.generic_string() : "\n" + _Writable.generic_string();

    ofstream _File;	
    _File.open(_Target.generic_string(), ios::ate | ios::in | ios::out); // without ios::in, all content will be removed
    _File.write(_Safe.c_str(), _Safe.size());
    _File.close();

    if (read_back(_Target) != _Writable) { // failed to overwritte the file
        _Throw_fs_error("failed to overwritte the file", error_type::runtime_error, "write_back");
    }

    // if won't throw exception, will be able to return true
    return true;
}

// FUNCTION write_front
_NODISCARD bool __cdecl write_front(const path& _Target, const path& _Writable) {
    if (!exists(_Target)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "write_back");
    }

    // If is empty, write _Writable using default ofstream::write() and close file.
    // It's important to call this function here, because it opens and closes handle to _Target.
    // If ofstream would try to open the same handle, system will throw exception.
    if (is_empty(_Target)) {
        ofstream _Quick_write;
        _Quick_write.open(_Target.generic_string());
        _Quick_write.write(_Writable.generic_string().c_str(), _Writable.generic_string().size());
        _Quick_write.close();

        if (read_front(_Target) != _Writable) { // failed to overrite file
            _Throw_fs_error("failed to overwrite file", error_type::runtime_error, "write_end");
        }

        // if won't throw exception, will be able to return true
        return true;
    }

    // If isn't empty, the file must be overwritten with good content,
    // otherwise the file will contains unnecessary empty lines
    // and sometimes contents from different lines will be in the same line.
    vector<path> _All{ _Writable };
    
    for (const auto& _Line : read_all(_Target)) {
        _All.push_back(_Line); // after added _Writable, add all content from read_all()
    }

    if (_All.size() <= 2) { // only 2 lines (added and existing)
        _All[0] += "\n";
    } else {
        for (size_t _Idx = 0; _Idx < _All.size(); ++_Idx) { // add end of line
            if (_Idx < _All.size() - 1) { // last line cannot contains end of line
                _All[_Idx] += "\n";
            }
        }
    }

    ofstream _File;
    _File.open(_Target.generic_string());
    
    for (const auto& _Line : _All) {
        _File.write(_Line.generic_string().c_str(), _Line.generic_string().size());
    }

    _File.close();

    if (read_front(_Target) != _Writable) { // failed to overrite file
        _Throw_fs_error("failed to overwrite file", error_type::runtime_error, "write_front");
    }

    return true; // same as if _Target is empty
}

// FUNCTION write_inside
_NODISCARD bool __cdecl write_inside(const path& _Target, const path& _Writable, const size_t  _Line) {
    if (!exists(_Target)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "write_inside");
    }

    const auto _All   = read_all(_Target);
    const auto _Count = _All.size();

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

    vector<path> _Write                      = _All;
    const vector<path>::const_iterator _Iter = _Write.begin();

    _Write.insert(_Iter + (_Line - 1), _Writable);

    // clear file and write the newest content
    ofstream _File;
    _File.open(_Target.generic_string());
    _File.clear();
    _File.close();

    for (const auto& _Elem : _Write) {
        write_back(_Target, _Elem);
    }

    if (read_inside(_Target, _Line) != _Writable) { // failed to overwrite file
        _Throw_fs_error("failed to overwrite the file", error_type::runtime_error, "write_inside");
    }

    // if won't throw an exception, will be able to return true
    return true;
}

// FUNCTION write_instead
_NODISCARD bool __cdecl write_instead(const path& _Target, const path& _Writable, const size_t _Line) {
    if (!exists(_Target)) { // file not found
        _Throw_fs_error("file not found", error_type::runtime_error, "write_instead");
    }

    auto _All         = read_all(_Target);
    const auto _Count = _All.size();

    if (_Line < 1 || _Line > _Count) { // invalid line
        _Throw_fs_error("invalid line", error_type::invalid_argument, "write_instead");
    }

    _All[_Line - 1] = _Writable; // swap content

    // clear _Target and write to him the newest content
    ofstream _File;
    _File.open(_Target.generic_string());
    _File.clear();
    _File.close();

    for (const auto& _Elem : _All) {
        write_back(_Target, _Elem);
    }

    if (read_inside(_Target, _Line) != _Writable) { // failed to replace line
        _Throw_fs_error("failed to overwrite the  file", error_type::runtime_error, "write_instead");
    }

    // if won't throw an exception, will be able to return true
    return true;
}

_EXPERIMENTAL_END
_FILESYSTEM_END

#endif // !_HAS_WINDOWS