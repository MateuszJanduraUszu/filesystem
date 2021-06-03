// path.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <pch.h>
#include <path.hpp>

#if !_HAS_WINDOWS
#pragma message("The contents of <path.cpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv
#pragma warning(push)
#pragma warning(disable : 4273 4297 4834 4996 26439) // C4273: inconsistent DLL binding
                                                     // C4297: function throwed exception after [nothrow] declaration
                                                     // C4834: discard function with [nodiscard] attribute
                                                     // C4996: using deprecated content
                                                     // C26439: function without [noexcept] attribute
_FS_EXP_BEGIN
// FUNCTION _Throw_fs_error
__declspec(noreturn) void __cdecl _Throw_fs_error(const char* const _Errm, const _Error_type _Errc) {
    switch (_Errc) {
    case _Error_type::_Generic_error:
        _THROW(exception(_Errm));
        break;
    case _Error_type::_Invalid_argument:
        _THROW(invalid_argument(_Errm));
        break;
    case _Error_type::_Length_error:
        _THROW(length_error(_Errm));
        break;
    case _Error_type::_Runtime_error:
        _THROW(runtime_error(_Errm));
        break;
    default: // without code or message
        _RERAISE;
        break;
    }
}

// FUNCTION _Convert_narrow_to_wide
_NODISCARD wstring __cdecl _Convert_narrow_to_wide(const string_view& _Input, const unsigned int _Code_page) {
    if (!_Input.empty()) {
        if (_Input.size() > static_cast<size_t>(INT_MAX)) {
            _Throw_fs_error("invalid length", _Error_type::_Length_error);
        }

        wstring _Output;
        const int _Input_size  = static_cast<int>(_Input.size());
        const int _Output_size = _Input_size;

        // MultiByteToWideChar() replaces multi byte to wide characters,
        // _Output size must be same as _Input size
        _Output.resize(_Output_size, L' ');

        const int _Result = MultiByteToWideChar(_Code_page, MB_ERR_INVALID_CHARS, _Input.data(), _Input_size,
            _Output.data(), _Output_size);

        if (_Result == 0) { // failed to convert multi byte to wide characters
            _Throw_fs_error("conversion failed", _Error_type::_Runtime_error);
        }

        if (_Output.size() > _Input_size) { // additional characters
            _Output.resize(_Input_size);
        }

        return _Output;
    } 

    return wstring();
}

// FUNCTION _Convert_wide_to_narrow
_NODISCARD string __cdecl _Convert_wide_to_narrow(const wstring_view& _Input, const unsigned int _Code_page) {
    if (!_Input.empty()) {
        if (_Input.size() > static_cast<size_t>(INT_MAX)) {
            _Throw_fs_error("invalid length", _Error_type::_Length_error);
        }

        string _Output;
        const int _Input_size  = static_cast<int>(_Input.size());
        const int _Output_size = _Input_size;

        // WideCharToMultiByte() replaces multi byte to wide characters.
        // _Output size must be same as _Input size
        _Output.resize(_Output_size, ' ');

        const int _Result = WideCharToMultiByte(_Code_page, WC_ERR_INVALID_CHARS, _Input.data(), _Input_size,
            _Output.data(), _Output_size, nullptr, nullptr);
        
        if (_Result == 0) { // failed to convert wide to multi byte characters
            _Throw_fs_error("conversion failed", _Error_type::_Runtime_error);
        }

        if (_Output.size() > _Input.size()) { // additional characters
            _Output.resize(_Input_size);
        }

        return _Output;
    }

    return string();
}

// FUNCTION TEMPLATE _Convert_utf_to_narrow
template<class _Elem, class _Traits>
_NODISCARD string __cdecl _Convert_utf_to_narrow(const basic_string_view<_Elem, _Traits>& _Input) {
    if (!_Input.empty()) {
        if (_Input.size() > static_cast<size_t>(INT_MAX)) {
            _Throw_fs_error("invalid length", _Error_type::_Length_error);
        }

        string _Output;

        _Output.reserve(_Input.size()); // reserve place for new data

        if constexpr (_STD is_same_v<_Elem, char8_t>) {
            for (const auto& _Idx_elem : _Input) {
                _Output.push_back(static_cast<char>(_Idx_elem));
            }

            if (_Output.size() < _Input.size()) { // lost characters
                _Throw_fs_error("conversion failed", _Error_type::_Runtime_error);
            }

            if (_Output.size() > _Input.size()) { // additional characters
                _Output.resize(_Input.size());
            }
        } else if constexpr (_STD is_same_v<_Elem, char16_t>) {
            wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> _Cvt;
            _Output = _Cvt.to_bytes(_Input.data());
        } else if constexpr (_STD is_same_v<_Elem, char32_t>) {
            wstring_convert<codecvt_utf8<char32_t>, char32_t> _Cvt;
            _Output = _Cvt.to_bytes(_Input.data());
        }

        return _Output;
    }

    return string();
}

template _FS_API _NODISCARD string __cdecl _Convert_utf_to_narrow(const u8string_view&);
template _FS_API _NODISCARD string __cdecl _Convert_utf_to_narrow(const u16string_view&);
template _FS_API _NODISCARD string __cdecl _Convert_utf_to_narrow(const u32string_view&);

// FUNCTION TEMPLATE _Convert_narrow_to_utf
template<class _Elem, class _Traits, class _Alloc>
_NODISCARD basic_string<_Elem, _Traits, _Alloc> __cdecl _Convert_narrow_to_utf(const string_view& _Input) {
    using _OutTy = basic_string<_Elem, _Traits, _Alloc>;

    if (!_Input.empty()) {
        if (_Input.size() > static_cast<size_t>(INT_MAX)) {
            _Throw_fs_error("invalid length", _Error_type::_Length_error);
        }

        _OutTy _Output;

        _Output.reserve(_Input.size()); // reserve place for new data

        if constexpr (_STD is_same_v<_Elem, char8_t>) {
            for (const auto& _Idx_elem : _Input) {
                _Output.push_back(static_cast<_Elem>(_Idx_elem));
            }

            if (_Output.size() < _Input.size()) { // lost characters
                _Throw_fs_error("conversion failed", _Error_type::_Runtime_error);
            }

            if (_Output.size() > _Input.size()) { // additional characters
                _Output.resize(_Input.size());
            }
        } else if constexpr (_STD is_same_v<_Elem, char16_t>) {
            wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> _Cvt;
            _Output = _Cvt.from_bytes(_Input.data());
        } else if constexpr (_STD is_same_v<_Elem, char32_t>) {
            wstring_convert<codecvt_utf8<char32_t>, char32_t> _Cvt;
            _Output = _Cvt.from_bytes(_Input.data());
        }

        return _Output;
    }

    return _OutTy();
}

template _FS_API _NODISCARD u8string __cdecl _Convert_narrow_to_utf(const string_view&);
template _FS_API _NODISCARD u16string __cdecl _Convert_narrow_to_utf(const string_view&);
template _FS_API _NODISCARD u32string __cdecl _Convert_narrow_to_utf(const string_view&);

// FUNCTION TEMPLATE path::path
template<class _Elem, class>
__cdecl path::path(const _Elem* const _Source) {
    if constexpr (_STD is_same_v<_Elem, char>) {
        this->_Text = _Source;
    } else if constexpr (_STD is_same_v<_Elem, wchar_t>) {
        this->_Text = _Convert_wide_to_narrow(_Source);
    } else { // const char8_t*, const char16_t* and const char32_t*
        this->_Text = _Convert_utf_to_narrow<_Elem, char_traits<_Elem>>(_Source);
    }

    this->_Check_size();
}

template _FS_API __cdecl path::path(const char* const);
template _FS_API __cdecl path::path(const char8_t* const);
template _FS_API __cdecl path::path(const char16_t* const);
template _FS_API __cdecl path::path(const char32_t* const);
template _FS_API __cdecl path::path(const wchar_t* const);

template<class _Src, class>
__cdecl path::path(const _Src& _Source) {
    if constexpr (_STD is_same_v<_Src, string> || _STD is_same_v<_Src, string_view>) {
        this->_Text = _Source.data();
    } else if constexpr (_STD is_same_v<_Src, wstring> || _STD is_same_v<_Src, wstring_view>) {
        this->_Text = _Convert_wide_to_narrow(_Source.data());
    } else { // u8string / u8string_view, u16string / u16string_view and u32string / u32string_view
        this->_Text = _Convert_utf_to_narrow<typename _Src::value_type,
            char_traits<typename _Src::value_type>>(_Source.data());
    }

    this->_Check_size();
}

template _FS_API __cdecl path::path(const string&);
template _FS_API __cdecl path::path(const u8string&);
template _FS_API __cdecl path::path(const u16string&);
template _FS_API __cdecl path::path(const u32string&);
template _FS_API __cdecl path::path(const wstring&);

template _FS_API __cdecl path::path(const string_view&);
template _FS_API __cdecl path::path(const u8string_view&);
template _FS_API __cdecl path::path(const u16string_view&);
template _FS_API __cdecl path::path(const u32string_view&);
template _FS_API __cdecl path::path(const wstring_view&);

// FUNCTION _Check_size
void __thiscall path::_Check_size() const {
    // _Text cannot be longer than 260 characters
    if (this->_Text.size() > static_cast<size_t>(MAX_PATH)) { // too long
        _Throw_fs_error("invalid path, expected maximum 260 characters", _Error_type::_Length_error);
    }
}

// FUNCTION path::operator=
path& __cdecl path::operator=(const path& _Source) {
    if (this != __builtin_addressof(_Source)) { // avoid assigning own value
        this->_Text = _Source._Text;
        this->_Check_size();
    }

    return *this;
}

template<class _Elem, class>
path& __cdecl path::operator=(const _Elem* const _Source) {
    if constexpr (_STD is_same_v<_Elem, char>) {
        this->_Text = _Source;
    } else if constexpr (_STD is_same_v<_Elem, wchar_t>) {
        this->_Text = _Convert_wide_to_narrow(_Source);
    } else { // const char8_t*, const char16_t* and const char32_t*
        this->_Text = _Convert_utf_to_narrow<_Elem, char_traits<_Elem>>(_Source);
    }

    this->_Check_size();
    return *this;
}

template _FS_API path& __cdecl path::operator=(const char* const);
template _FS_API path& __cdecl path::operator=(const char8_t* const);
template _FS_API path& __cdecl path::operator=(const char16_t* const);
template _FS_API path& __cdecl path::operator=(const char32_t* const);
template _FS_API path& __cdecl path::operator=(const wchar_t* const);

template<class _Src, class>
path& __cdecl path::operator=(const _Src& _Source) {
    if constexpr (_STD is_same_v<_Src, string> || _STD is_same_v<_Src, string_view>) {
        this->_Text = _Source.data();
    } else if constexpr (_STD is_same_v<_Src, wstring> || _STD is_same_v<_Src, wstring_view>) {
        this->_Text = _Convert_wide_to_narrow(_Source.data());
    } else { // u8string / u8string_view, u16string / u16string_view and u32string / u32string_view
        this->_Text = _Convert_utf_to_narrow<typename _Src::value_type,
            char_traits<typename _Src::value_type>>(_Source.data());
    }

    this->_Check_size();
    return *this;
}

template _FS_API path& __cdecl path::operator=(const string&);
template _FS_API path& __cdecl path::operator=(const u8string&);
template _FS_API path& __cdecl path::operator=(const u16string&);
template _FS_API path& __cdecl path::operator=(const u32string&);
template _FS_API path& __cdecl path::operator=(const wstring&);

template _FS_API path& __cdecl path::operator=(const string_view&);
template _FS_API path& __cdecl path::operator=(const u8string_view&);
template _FS_API path& __cdecl path::operator=(const u16string_view&);
template _FS_API path& __cdecl path::operator=(const u32string_view&);
template _FS_API path& __cdecl path::operator=(const wstring_view&);

// FUNCTION path::assign
path& __cdecl path::assign(const path& _Source) {
    return this->operator=(_Source);
}

template<class _Elem, class>
path& __cdecl path::assign(const _Elem* const _Source) {
    return this->operator=(_Source);
}

template<class _Src, class>
path& __cdecl path::assign(const _Src& _Source) {
    return this->operator=(_Source);
}

// FUNCTION path::operator+=
path& __cdecl path::operator+=(const path& _Added) {
    if (this != __builtin_addressof(_Added)) { // avoid appending own value
        this->_Text += _Added._Text;
        this->_Check_size();
    }

    return *this;
}

template<class _Elem, class>
path& __cdecl path::operator+=(const _Elem* const _Added) {
    if constexpr (_STD is_same_v<_Elem, char>) {
        this->_Text += _Added;
    } else if constexpr (_STD is_same_v<_Elem, wchar_t>) {
        this->_Text += _Convert_wide_to_narrow(_Added);
    } else { // const char8_t* const char16_t* and const char32_t
        this->_Text += _Convert_utf_to_narrow<_Elem, char_traits<_Elem>>(_Added);
    }

    this->_Check_size();
    return *this;
}

template _FS_API path& __cdecl path::operator+=(const char* const);
template _FS_API path& __cdecl path::operator+=(const char8_t* const);
template _FS_API path& __cdecl path::operator+=(const char16_t* const);
template _FS_API path& __cdecl path::operator+=(const char32_t* const);
template _FS_API path& __cdecl path::operator+=(const wchar_t* const);

template<class _Src, class>
path& __cdecl path::operator+=(const _Src& _Added) {
    if constexpr (_STD is_same_v<_Src, string> || _STD is_same_v<_Src, string_view>) {
        this->_Text += _Added.data();
    } else if constexpr (_STD is_same_v<_Src, wstring> || _STD is_same_v<_Src, wstring_view>) {
        this->_Text += _Convert_wide_to_narrow(_Added.data());
    } else { // u8string / u8string_view, u16string / u16string_view and u32string / u32string_view
        this->_Text += _Convert_utf_to_narrow<typename _Src::value_type,
            char_traits<typename _Src::value_type>>(_Added.data());
    }
    
    this->_Check_size();
    return *this;
}

template _FS_API path& __cdecl path::operator+=(const string&);
template _FS_API path& __cdecl path::operator+=(const u8string&);
template _FS_API path& __cdecl path::operator+=(const u16string&);
template _FS_API path& __cdecl path::operator+=(const u32string&);
template _FS_API path& __cdecl path::operator+=(const wstring&);

template _FS_API path& __cdecl path::operator+=(const string_view&);
template _FS_API path& __cdecl path::operator+=(const u8string_view&);
template _FS_API path& __cdecl path::operator+=(const u16string_view&);
template _FS_API path& __cdecl path::operator+=(const u32string_view&);
template _FS_API path& __cdecl path::operator+=(const wstring_view&);

// FUNCTION path::append
path& __cdecl path::append(const path& _Added) {
    return this->operator+=(_Added);
}

template<class _Elem, class>
path& __cdecl path::append(const _Elem* const _Added) {
    return this->operator+=(_Added);
}

template<class _Src, class>
path& __cdecl path::append(const _Src& _Added) {
    return this->operator+=(_Added);
}

// FUNCTION path::operator==
bool __cdecl path::operator==(const path& _Compare) const noexcept {
    // if _Compared._Text won't be compared with _Text, result will be true
    bool _Comp = true; 

    if (this != __builtin_addressof(_Compare)) { // avoid comparing with own value
        _Comp = this->_Text == _Compare._Text;
    }

    return _Comp;
}

template<class _Elem, class>
bool __cdecl path::operator==(const _Elem* const _Compare) const {
    bool _Comp = false;
    
    if constexpr (_STD is_same_v<_Elem, char>) {
        _Comp = this->_Text == _Compare;
    } else if constexpr (_STD is_same_v<_Elem, wchar_t>) {
        _Comp = this->_Text == _Convert_wide_to_narrow(_Compare);
    } else { // const char8_t*, const char16_t* and const char32_t*
        _Comp = this->_Text == _Convert_utf_to_narrow<_Elem, char_traits<_Elem>>(_Compare);
    }

    return _Comp;
}

template _FS_API bool __cdecl path::operator==(const char* const) const;
template _FS_API bool __cdecl path::operator==(const char8_t* const) const;
template _FS_API bool __cdecl path::operator==(const char16_t* const) const;
template _FS_API bool __cdecl path::operator==(const char32_t* const) const;
template _FS_API bool __cdecl path::operator==(const wchar_t* const) const;

template<class _Src, class>
bool __cdecl path::operator==(const _Src& _Compare) const {
    bool _Comp = false;

    if constexpr (_STD is_same_v<_Src, string> || _STD is_same_v<_Src, string_view>) {
        _Comp = this->_Text == _Compare;
    } else if constexpr (_STD is_same_v<_Src, wstring> || _STD is_same_v<_Src, wstring_view>) {
        _Comp = this->_Text == _Convert_wide_to_narrow(_Compare.data());
    } else { // u8string / u8string_view, u16string / u16string_view and u32string / u32string_view
        _Comp = this->_Text == _Convert_utf_to_narrow<typename _Src::value_type,
            char_traits<typename _Src::value_type>>(_Compare.data());
    }

    return _Comp;
}

template _FS_API bool __cdecl path::operator==(const string&) const;
template _FS_API bool __cdecl path::operator==(const u8string&) const;
template _FS_API bool __cdecl path::operator==(const u16string&) const;
template _FS_API bool __cdecl path::operator==(const u32string&) const;
template _FS_API bool __cdecl path::operator==(const wstring&) const;

template _FS_API bool __cdecl path::operator==(const string_view&) const;
template _FS_API bool __cdecl path::operator==(const u8string_view&) const;
template _FS_API bool __cdecl path::operator==(const u16string_view&) const;
template _FS_API bool __cdecl path::operator==(const u32string_view&) const;
template _FS_API bool __cdecl path::operator==(const wstring_view&) const;

// FUNCTION TEMPLATE operator>>
template<class _Elem, class _Traits = char_traits<_Elem>>
_NODISCARD basic_istream<_Elem, _Traits>& __cdecl operator>>(basic_istream<_Elem, _Traits>& _Stream, path& _Path) {
    basic_string<_Elem, _Traits, allocator<_Elem>> _Input;
    _Stream >> _Input;
    _Path = _Input;
    return _Stream;
}

template _FS_API basic_istream<char, char_traits<char>>& __cdecl operator>>(basic_istream<char, char_traits<char>>&, path&);
template _FS_API basic_istream<wchar_t, char_traits<wchar_t>>& __cdecl operator>>(basic_istream<wchar_t, char_traits<wchar_t>>&, path&);

// FUNCTION TEMPLATE operator<<
template<class _Elem, class _Traits = char_traits<_Elem>>
_NODISCARD basic_ostream<_Elem, _Traits>& __cdecl operator<<(basic_ostream<_Elem, _Traits>& _Stream, path& _Path) {
    // current C++ standard supports only char and wchar_t streaming
    if constexpr (_STD is_same_v<_Elem, char>) {
        _Stream << _Path._Text;
    } else if constexpr (_STD is_same_v<_Elem, wchar_t>) {
        _Stream << _Path.generic_wstring();
    }
    
    return _Stream;
}

template _FS_API basic_ostream<char, char_traits<char>>& __cdecl operator<<(basic_ostream<char, char_traits<char>>&, path&);
template _FS_API basic_ostream<wchar_t, char_traits<wchar_t>>& __cdecl operator<<(basic_ostream<wchar_t, char_traits<wchar_t>>&, path&);

// FUNCTION path::clear
void __thiscall path::clear() noexcept {
    this->_Text.clear();
}

// FUNCTION path::current_directory
_NODISCARD path __thiscall path::current_directory() noexcept {
    this->current_path(); // assign current path
    this->remove_file(true);
    return *this;
}

// FUNCTION path::current_path
_NODISCARD path __thiscall path::current_path() noexcept {
    char _Buff[MAX_PATH];
    this->assign(string_type(_Buff, GetModuleFileNameA(nullptr, _Buff, MAX_PATH)));
    return *this;
}

// FUNCTION path::drive
_NODISCARD path __thiscall path::drive() const noexcept {
    if (this->has_drive()) {
        return string_type(1, this->_Text[0]); // first character is drive
    }

    return *this;
}

// FUNCTION path::empty
_NODISCARD bool __thiscall path::empty() const noexcept {
    return this->_Text.empty();
}

// FUNCTION path::extension
_NODISCARD path __thiscall path::extension() const noexcept {
    if (this->has_extension()) {
        auto _Extension = this->_Text; // use copy to remove unnecessary content
        _Extension.replace(0, _Extension.find_last_of(".") + 1, string_type());
        return _Extension;
    }

    return *this;
}

// FUNCTION path::file
_NODISCARD path __thiscall path::file() const noexcept {
    if (this->has_file()) {
        auto _File = this->_Text;
        
        if (_File.find_last_of(R"(\)") < _File.size()) { // if false, only file in the path
            _File.replace(0, _File.find_last_of(R"(\)") + 1, string_type());
        }
        
        return _File;
    }

    return *this;
}

// FUNCTION path::fix
_NODISCARD path& __thiscall path::fix() noexcept {
    string_type _Fixed;

    for (size_t _Idx = 0; _Idx < this->_Text.size(); ++_Idx) {
        if (_Fixed.empty() && (this->_Text[_Idx] == _Expected_slash
            || this->_Text[_Idx] == '/')) { // skip slash while is at the first position
            continue;
        }

        if ((this->_Text[_Idx] == _Expected_slash || this->_Text[_Idx] == '/')
            && (_Fixed.back() == _Expected_slash || _Fixed.back() == '/')) { // skip slash if is next in the row
            continue;
        }

        _Fixed.push_back(this->_Text[_Idx]);
    }

    while (_Fixed.back() == _Expected_slash || _Fixed.back() == '/') {
        _Fixed.pop_back(); // remove slash from last position
    }

    this->assign(_Fixed);
    this->make_preferred();
    return *this;
}

// FUNCTION path::generic_string
_NODISCARD const string __thiscall path::generic_string() const noexcept {
    return this->_Text;
}

// FUNCTION path::generic_u8string
_NODISCARD const u8string __thiscall path::generic_u8string() const noexcept {
    return _Convert_narrow_to_utf<char8_t, char_traits<char8_t>>(this->_Text);
}

// FUNCTION path::generic_u16string
_NODISCARD const u16string __thiscall path::generic_u16string() const noexcept {
    return _Convert_narrow_to_utf<char16_t, char_traits<char16_t>>(this->_Text);
}

// FUNCTION path::generic_u32string
_NODISCARD const u32string __thiscall path::generic_u32string() const noexcept {
    return _Convert_narrow_to_utf<char32_t, char_traits<char32_t>>(this->_Text);
}

// FUNCTION path::generic_wstring
_NODISCARD const wstring __thiscall path::generic_wstring() const noexcept {
    return _Convert_narrow_to_wide(this->_Text);
}

// FUNCTION path::has_drive
_NODISCARD bool __thiscall path::has_drive() const noexcept {
    if (this->_Text.size() < 3) { // requires minimum 3 characters
        return false;
    }

    if (this->_Text[1] != ':') { // second character must be ":"
        return false;
    }

    // third character must be expected or unexpected slash
    if (this->_Text[2] != _Expected_slash && this->_Text[2] != '/') {
        return false;
    }

    if (static_cast<int>(this->_Text[0]) >= 65
        && static_cast<int>(this->_Text[0]) <= 90) { // only big letters
        return true;
    } else if (static_cast<int>(this->_Text[0]) >= 97
        && static_cast<int>(this->_Text[0]) <= 122) { // only small letters
        return true;
    } else { // other characters
        return false;
    }

    return false;
}

// FUNCTION path::has_extension
_NODISCARD bool __thiscall path::has_extension() const noexcept {
    if (this->_Text.find(".") < this->_Text.size()) {
        const size_t _Dot_pos = this->_Text.find_last_of(".");
        
        // example of this case:
        // "Disk:\Directory\Subdorectory."
        if (_Dot_pos < this->_Text.size()) {
            if (const size_t _Last = this->_Text.find_last_of(R"(\)");
                _Last < this->_Text.size() && _Last > _Dot_pos) { // for example "Disk:\File.extension\"
                return false;
            }
            
            // for example: "Disk:\File."
            return _Dot_pos != this->_Text.size() - 1;
        } else {
            return false;
        }
    }
    
    return false;
}

// FUNCTION path::has_file
_NODISCARD bool __thiscall path::has_file() const noexcept {
    // file cannot exists without extension
    return this->has_extension();
}

// FUNCTION path::has_parent_directory
_NODISCARD bool __thiscall path::has_parent_directory() const noexcept {
    // parent directory is directory after root directory
    if (this->has_root_directory()) {
        // if false, then path contains only parent path (no drive or root directory)
        return this->_Text.size() > this->root_directory().generic_string().size();
    } else { // path without drive and root directory
        return true;
    }
}

// FUNCTION path::has_root_directory
_NODISCARD bool __thiscall path::has_root_directory() const noexcept {
    // slash on first place where there's no drive means, that it's root directory
    if (!this->has_drive() && (this->_Text[0] == _Expected_slash
        || this->_Text[0] == '/')) {
        return true;
    }

    // directory after drive is root directory
    if (this->has_drive() && this->_Text.size() > 3) {
        return true;
    }

    return false;
}

// FUNCTION path::has_stem
_NODISCARD bool __thiscall path::has_stem() const noexcept {
    // stem is just file name without extension
    return this->has_file();
}

// FUNCTION path::is_absolute
_NODISCARD bool __thiscall path::is_absolute() const noexcept {
    if (this->_Text.find(_Expected_slash) > this->_Text.size()
        && this->_Text.find("/") > this->_Text.size()) { // path without slashes (relative)
        return false;
    }

    // path with drive and/or root directory must be absolute
    if (this->has_drive() || this->has_root_directory()) { 
        return true;
    } else { // path with some directories
        return false;
    }

    return false;
}

// FUNCTION path::is_relative
_NODISCARD bool __thiscall path::is_relative() const noexcept {
    return !this->is_absolute();
}

// FUNCTION path::make_preferred
_NODISCARD path& __thiscall path::make_preferred() noexcept {
    for (auto& _Elem : this->_Text) {
        if (_Elem == '/') { // found bad slash
            _Elem = _Expected_slash;
        }
    }

    return *this;
}

// FUNCTION path::parent_directory
_NODISCARD path __thiscall path::parent_directory() const noexcept {
    if (this->has_parent_directory()) { // return only first directory from parent_path()
        return string_type(this->parent_path().generic_string(), 0,
            this->parent_path().generic_string().find_first_of(_Expected_slash));
    }
    
    return *this;
}

// FUNCTION path::parent_path
_NODISCARD path __thiscall path::parent_path() const noexcept {
    // parent path is everything after root directory
    if (this->has_parent_directory()) { 
        string_type _Parent_path(this->_Text);

        if (this->has_drive()) { // remove drive with root directory, contains ":" and 2 slashes
            string_type _Without_drive(_Parent_path, this->drive().generic_string().size() +
                this->root_directory().generic_string().size() + 3, _Parent_path.size());
            _Parent_path = _Without_drive; // now _Parent_path is without drive and root directory
        } else if (!this->has_drive() && this->has_root_directory()) { // remove root directory
            string_type _Without_root_dir(_Parent_path,
                this->root_directory().generic_string().size() + 2, _Parent_path.size());
            _Parent_path = _Without_root_dir; // now _Parent_path is without root_directory
        } else { // path without drive and root directory
            ; // nothing to do here, because _Text is parent path
        }

        return _Parent_path;
    }

    return *this;
}

// FUNCTION path::remove_extension
_NODISCARD path& __thiscall path::remove_extension() noexcept {
    if (this->has_extension()) {
        const size_t _Last = this->_Text.find_last_of(".");
        this->_Text.resize(_Last);
    }

    return *this;
}

// FUNCTION path::remove_file
_NODISCARD path& __cdecl path::remove_file(const bool _With_slash) noexcept {
    if (this->has_file()) {
        const size_t _Last = _With_slash ? this->_Text.find_last_of(R"(\)") : this->_Text.find_last_of(R"(\)") + 1;
        this->_Text.resize(_Last);
    }

    return *this;
}

// FUNCTION path::replace_extension
_NODISCARD path& __cdecl path::replace_extension(const path& _Replacement) {
    if (this->has_extension()) {
        this->remove_extension(); // remove old extension
        
        if (_Replacement.generic_string()[0] != '.') { // replacement without dot
            this->append(".");
        }

        this->append(_Replacement);
    }

    this->_Check_size();
    return *this;
}

// FUNCTION path::replace_file
_NODISCARD path& __cdecl path::replace_file(const path& _Replacement) {
    if (this->has_file()) {
        if (this->_Text.find_last_of(R"(\)") > this->_Text.size()) { // only file in the path
            this->_Text.clear();
            this->assign(_Replacement);
            return *this; // break here
        }

        // if replacement has already slash at first position, remove without slash
        this->remove_file(_Replacement.generic_string()[0] == '\\');
        this->append(_Replacement);
    }

    this->_Check_size();
    return *this;
}

// FUNCTION path::replace_stem
_NODISCARD path& __cdecl path::replace_stem(const path& _Replacement) {
    if (this->has_stem()) {
        const auto _Ext = this->extension().generic_string();
    
        this->remove_file(_Replacement.generic_string()[0] == '\\');
        this->append(_Replacement); // add stem to the path
        this->append(".");
        this->append(_Ext); // add extension to the path
    }

    this->_Check_size();
    return *this;
}

// FUNCTION path::root_directory
_NODISCARD path __thiscall path::root_directory() const noexcept {
    if (this->has_root_directory()) {
        return string_type(this->root_path().generic_string(), 0,
            this->root_path().generic_string().find_first_of(_Expected_slash));
    }

    return *this;
}

// FUNCTION path::root_path
_NODISCARD path __thiscall path::root_path() const noexcept {
    if (this->has_root_directory()) {
        // if has drive, skip 3 first characters ("D:\"), otherwise skip 1 first character (slash)
        return this->has_drive() ? string_type(this->_Text, 3, this->_Text.size())
            : string_type(this->_Text, 1, this->_Text.size());
    }

    return *this;
}

// FUNCTION path::stem
_NODISCARD path __thiscall path::stem() const noexcept {
    if (this->has_stem()) {
        auto _Stem = this->_Text; // don't change original text
        
        if (_Stem.find_last_of(R"(\)") < _Stem.size()) {
            _Stem.replace(0, _Stem.find_last_of(R"(\)") + 1, string_type()); // leave only file name with extension
            _Stem.resize(_Stem.find_last_of(".")); // remove extension
            return _Stem; // break here
        }

        _Stem.resize(_Stem.find_last_of(".")); // remove extension
        return _Stem;
    }

    return *this;
}

// FUNCTION path::swap
void __cdecl path::swap(path& _Other) {
    this->_Text.swap(_Other._Text);
    this->_Check_size();
}

_FS_EXP_END

#pragma warning(pop)
#endif // !_HAS_WINDOWS