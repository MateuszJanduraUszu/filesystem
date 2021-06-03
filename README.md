# filesystem

Simple library for path, file and directory operations used in UCFG file extension.

# <path.hpp>:

# _Expected_slash 
- type: inline constexpr char
- use: Defines excepted slash in path, in Windows 10. 
- attributes: none
- calling convention: none
- exceptions: none

# _Utf8
- type: inline constexpr unsigned int
- use: Default code page in conversion.
- attributes: none
- calling convention: none
- exceptions: none
  
# template<class>
# _Is_CharT
- type: template inline constexpr bool
- use: Checks if type in template is character.
- attributes: none
- calling convention: none
- exceptions: none
  
# template<class>
# _Is_Src
- type: template inline constexpr bool
- use: Checks if type in template is basic_string.
- attributes: none
- calling convention: none
- exceptions: none
  
# template<class>
# _Is_SrcView
- type: template inline constexpr bool
- use: Checks if type in template is basic_string_view.
- attributes: none
- calling convention: none
- exceptions: none
  
# _Error_type
- type: enum class
- use: Defines filesystem error type.
- attributes: none
- calling convention: none
- exceptions: none
  
# _Throw_fs_error(const char*, const _Error_type)
- type: void
- use: Throws _Error_type error.
- attributes: noreturn
- calling convention: __cdecl
- exceptions: _Error_type::_Generic_error, _Error_type::_Length_error, _Error_type::_Runtime_error, _Error_type::_Invalid_argument
  
# _Convert_narrow_to_wide(const string_view&, const unsigned int)
- type: wstring
- use: Converts string to wstring.
- attributes: nodiscard
- calling convention: __cdecl
- exceptions: _Error_type::_Length_error, _Error_type::_Runtime_error
  
# _Convert_wide_to_narrow(const wstring_view&, const unsigned int)
- type: string
- use: Converts wstring to string.  
- attributes: nodiscard
- calling convention: __cdecl
- exceptions: _Error_type::_Length_error, _Error_type::_Runtime_error
  
# template<class, class>
# _Convert_utf_to_narrow(const basic_string_view<class, class>&)
- type: template string
- use: Converts UTFx to string.
- attributes: nodiscard
- calling convention: __cdecl
- exceptions: _Error_type::_Length_error, _Error_type::_Runtime_error
   
# template<class, class, class>
# _Convert_narrow_to_utf(const string_view&)
- type: template basic_string<class, class, class>
- use: Converts string to UTFx.
- attributes: nodiscard
- calling convention: __cdecl
- exceptions: _Error_type::_Length_error, _Error_type::_Runtime_error
  
# template<class, class>
# operator>>(basic_istream<class, class>&, path&)
- type: template basic_istream<class, class>&
- use: Passes stream to path::_Text.
- attributes: nodiscard
- calling convention: __cdecl
- exceptions: none

# template<class, class>
# operator<<(basic_ostream<class, class>&, path&)
- type: template basic_ostream<class, class>&
- use: Passes path::_Text to the stream.
- attributes: nodiscard
- calling convention: __cdecl
- exceptions: none
  
# template<class, class>
# path::path(const class* const)
  
# template<class, class>
# path::path(const class&)
  
# path::_Check_size()
  
# path::operator=(const path&)
  
# path::assign(const path&)  
  
# template<class, class>
# path::operator=(const class* const)
  
# template<class, class>
# path::assign(const calss* const)
  
# template<class, class>
# path::operator=(const class&)

# template<class, class>
# path::assign(const class&)
  
# path::operator+=(const path&)
  
# path::append(const path&)
  
# template<class, class>
# path::operator+=(const class* const)

# template<class, class>
# path::append(const class* const)
  
# template<class, class>
# path::operator+=(const calss&)
  
# template<class, class>
# path::append(const class&)
  
# path::operator==(const path&)
  
# template<class, class>
# path::operator==(const class* const)
  
# template<class, class>
# path::operator==(const class&)
  
# path::clear()
- type: void
- use: Clears current working path.
- attributes: noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::current_directory()
- type: path
- use: Returns current system directory.
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::current_path()
- type: path
- use: Returns current system path.
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::drive()
- type: path
- use: Return drive from current working path (if has).
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::empty()
- type: bool
- use: Checks if current working path is empty.
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::extension()
- type: path
- use: Returns file extension from current working path (if has).
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::file()
- type: path
- use: Returns filename from current working path (if has).
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::fix()
- type: path&
- use: Removes unnecessary slashes and changes unexpected slashes to expected (on Windows 10 from "/" to "\").
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::generic_string()
- type: string
- use: Returns path::_Text.
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::generic_u8string()
- type: u8string
- use: Returns path::_Text as u8string.
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none  
  
# path::generic_u16string()
- type: u16string
- use: Returns path::_Text as u16string.
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::generic_u32string()
- type: u32string
- use: Returns path::_Text as u32string;
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::generic_wstring()
- type: wstring
- use: Returns path::_Text as wstring;
- attributes: nodiscard, noexcept;
- calling convention: __thiscall
- exceptions: none
  
# path::has_drive()
- type: bool
- use: Checks if current working path has drive.
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::has_extension()
- type: bool
- use: Checks if current working path has extension.
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::has_file()
- type: bool
- use: Checks if current working path has file.
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::has_parent_directory()
- type: bool
- use: Checks if current working path has parent directory.
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::has_root_directory()
- type: bool
- use: Checks if current working path has root directory.
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::has_stem()
- type: bool
- use: Checks if current working path has stem (filename without extension).
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::is_absolute()
- type: bool
- use: Checks if current working path is absolute. See https://docs.microsoft.com/xx-xx/dotnet/standard/io/file-path-formats,
- (where xx-xx is your localization) for more informations.
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none  
  
# path::is_relative()
- type: bool
- use: Checks if current working path is relative. See https://docs.microsoft.com/xx-xx/dotnet/standard/io/file-path-formats,
- (where xx-xx is your localization) for more informations.
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::make_preferred()
- type: path&
- use: Returns converted to Windows 10 standard path.
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::parent_directory()
- type: path
- use: Returns parent directory from current working path (if has).
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::parent_path()
- type: path
- use: Returns parent path from current working path (if has).
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::remove_extension()
- type: path&
- use: Removes extension from file, from current working path (if has).
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::remove_file(const bool)
- type: path&
- use: Removes filename (if parametr is true, removes also slash before filename (if has)).
- attributes: nodiscard, noexcept
- calling convention: __cdecl
- exceptions: none
  
# path::replace_extension(const path&)
- type: path&
- use: Replaces current extension by other.
- attributes: nodiscard
- calling convention: __cdecl
- exceptions: _Error_type::_Length_error
  
# path::replace_file(const path&)
- type: path&
- use: Replaces current filename by other.
- attributes: nodiscard
- calling convention: __cdecl
- exceptions: _Error_type::_Length_error
  
# path::replace_stem(const path&)
- type: path&
- use: Replaces current stem (filename without extension) by other.
- attributes: nodiscard
- calling convention: __cdecl
- exceptions: _Error_type::_Length_error
  
# path::root_directory()
- type: path
- use: Returns root directory from current working path (if has).
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::root_path()
- type: path
- use: Returns root path from current working path (if has).
- attributes: nodiscard. noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::stem()
- type: path
- use: Returns stem (filename without extension) from current working path (if has).
- attributes: nodiscard, noexcept
- calling convention: __thiscall
- exceptions: none
  
# path::swap(path&)
- type: void
- use: Swaps path::_Text with other path::_Text.
- attributes: none
- calling convention: __cdecl
- exceptions: _Error_type::_Length_error
  
# path::_Text
- type: string
- use: Current working path.
- attributes: none
- calling convention: none
- exceptions: none

# <file.hpp>:
  # add a description of the file class here

# <directory.hpp>:
  # add a description of the directory class here
