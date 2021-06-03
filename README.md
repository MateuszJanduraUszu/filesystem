# filesystem

Simple library for path, file and directory operations used in UCFG file extension.

<path.hpp>:
  _Expected_slash 
  # type: inline constexpr char
  # use: Defines excepted slash in path, in Windows 10. 
  # attributes: none
  # calling convention: none
  # exceptions: none

  _Utf8
  # type: inline constexpr unsigned int
  # use: Default code page in conversion.
  # attributes: none
  # calling convention: none
  # exceptions: none
  
  template<class>
  _Is_CharT
  # type: template inline constexpr bool
  # use: Checks if type in template is character.
  # attributes: none
  # calling convention: none
  # exceptions: none
  
  template<class>
  _Is_Src
  # type: template inline constexpr bool
  # use: Checks if type in template is basic_string.
  # attributes: none
  # calling convention: none
  # exceptions: none
  
  template<class>
  _Is_SrcView
  # type: template inline constexpr bool
  # use: Checks if type in template is basic_string_view.
  # attributes: none
  # calling convention: none
  # exceptions: none
  
  _Error_type
  # type: enum class
  # use: Defines filesystem error type.
  # attributes: none
  # calling convention: none
  # exceptions: none
  
  _Throw_fs_error(const char*, const _Error_type)
  # type: void
  # use: Throws _Error_type error.
  # attributes: noreturn
  # calling convention: __cdecl
  # exceptions: _Error_type::_Generic_error, _Error_type::_Length_error, _Error_type::_Runtime_error, _Error_type::_Invalid_argument
  
  _Convert_narrow_to_wide(const string_view&, const unsigned int)
  # type: wstring
  # use: Converts string to wstring.
  # attributes: nodiscard
  # calling convention: __cdecl
  # exceptions: _Error_type::_Length_error, _Error_type::_Runtime_error
  
  _Convert_wide_to_narrow(const wstring_view&, const unsigned int)
  # type: string
  # use: Converts wstring to string.  
  # attributes: nodiscard
  # calling convention: __cdecl
  # exceptions: _Error_type::_Length_error, _Error_type::_Runtime_error
  
  template<class, class>
  _Convert_utf_to_narrow(const basic_string_view<class, class>&)
  # type: template string
  # use: Converts UTFx to string.
  # attributes: nodiscard
  # calling convention: __cdecl
  # exceptions: _Error_type::_Length_error, _Error_type::_Runtime_error
   
  template<class, class, class>
  _Convert_narrow_to_utf(const string_view&)
  # type: template basic_string<class, class, class>
  # use: Converts string to UTFx.
  # attributes: nodiscard
  # calling convention: __cdecl
  # exceptions: _Error_type::_Length_error, _Error_type::_Runtime_error
  
  template<class, class>
  operator>>(basic_istream<class, class>&, path&)
  # type: template basic_istream<class, class>&
  # use: Passes stream to path::_Text.
  # attributes: nodiscard
  # calling convention: __cdecl
  # exceptions: none

  template<class, class>
  operator<<(basic_ostream<class, class>&, path&)
  # type: template basic_ostream<class, class>&
  # use: Passes path::_Text to the stream.
  # attributes: nodiscard
  # calling convention: __cdecl
  # exceptions: none
  
  
  
  
  
  
# path::replace_file(const path&)
- type: path&
- use: Replaces current filename by other.
- attributes: nodiscard
- calling convention: __cdecl
- exceptions: _Error_type::_Length_error
  
  path::replace_stem(const path&)
  # type: path&
  # use: Replaces current stem (filename without extension) by other.
  # attributes: nodiscard
  # calling convention: __cdecl
  # exceptions: _Error_type::_Length_error
  
  path::root_directory()
  # type: path
  # use: Returns root directory from current working path (if has).
  # attributes: nodiscard
  # calling convention: __thiscall
  # exceptions: none
  
  path::root_path()
  # type: path
  # use: Returns root path from current working path (if has).
  # attributes: nodiscard
  # calling convention: __thiscall
  # exceptions: none
  
  path::stem()
  # type: path
  # use: Returns stem (filename without extension) from current working path (if has).
  # attributes: nodiscard
  # calling convention: __thiscall
  # exceptions: none
  
  path::swap(path&)
  # type: void
  # use: Swaps path::_Text with other path::_Text.
  # attributes: none
  # calling convention: __cdecl
  # exceptions: _Error_type::_Length_error
  
  path::_Text
  # type: string
  # use: Current working path.
  # attributes: none
  # calling convention: none
  # exceptions: none

<file.hpp>:
  # add a description of the file class here

<directory.hpp>:
  # add a description of the directory class here
