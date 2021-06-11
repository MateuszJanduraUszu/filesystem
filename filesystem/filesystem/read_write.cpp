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


// FUNCTION write_instead

_EXPERIMENTAL_END
_FILESYSTEM_END

#endif // !_HAS_WINDOWS