// conversion.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <filesystem_pch.hpp>
#include <filesystem.hpp>

#if !_HAS_WINDOWS
#pragma message("The contents of <conversion.cpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv
#pragma warning(push)
#pragma warning(disable : 4996) // C4996: using deprecated content (<codecvt>)

_FILESYSTEM_BEGIN
// FUNCTION _Convert_narrow_to_wide
_NODISCARD wstring _Convert_narrow_to_wide(const code_page _Cp, const string_view _Input) {
    if (!_Input.empty()) {
        if (_Input.size() > static_cast<size_t>(INT_MAX)) {
            _Throw_system_error("_Convert_narrow_to_wide", "invalid length", error_type::length_error);
        }

        wstring _Output;
        const int _Input_size  = static_cast<int>(_Input.size());
        const int _Output_size = _Input_size; // the same size
    
        // The MultiByteToWideChar() replaces multi byte to wide characters,
        // so _Output size must be same as _Input size.
        _Output.resize(_Output_size, L' ');
        if (!MultiByteToWideChar(static_cast<uint32_t>(_Cp), MB_ERR_INVALID_CHARS,
            _Input.data(), _Input_size, _Output.data(), _Output_size)) { 
            _Throw_system_error("_Convert_narrow_to_wide", "conversion failed", error_type::runtime_error);
        }

        if (_Output.size() > _Input.size()) { // additional characters
            _Output.resize(_Output_size);
        }

        return _Output;
    }

    return wstring();
}

// FUNCTION _Convert_wide_to_narrow
_NODISCARD string _Convert_wide_to_narrow(const code_page _Cp, const wstring_view _Input) {
    if (!_Input.empty()) {
        if (_Input.size() > static_cast<size_t>(INT_MAX)) {
            _Throw_system_error("_Convert_wide_to_narrow", "invalid length", error_type::length_error);
        }

        string _Output;
        const int _Input_size  = static_cast<int>(_Input.size());
        const int _Output_size = _Input_size;

        // The WideCharToMultiByte() replaces multi byte to wide characters.
        // _Output size must be same as _Input size.
        _Output.resize(_Output_size, ' ');
        if (!WideCharToMultiByte(static_cast<uint32_t>(_Cp), WC_ERR_INVALID_CHARS,
            _Input.data(), _Input_size, _Output.data(), _Output_size, nullptr, nullptr)) { 
            _Throw_system_error("_Convert_wide_to_narrow", "conversion failed", error_type::runtime_error);
        }

        if (_Output.size() > _Input.size()) { // additional characters
            _Output.resize(_Output_size);
        }

        return _Output;
    }

    return string();
}

// FUNCTION TEMPLATE _Convert_utf_to_wide
template <class _Elem, class _Traits>
_NODISCARD string _Convert_utf_to_narrow(const basic_string_view<_Elem, _Traits> _Input) noexcept(_Is_narrow_char_t<_Elem>) {
    if (!_Input.empty()) {
        if (_Input.size() > static_cast<size_t>(INT_MAX)) {
            _Throw_system_error("_Convert_utf_to_wide", "invalid length", error_type::length_error);
        }

        // check that _Elem is the default character types
        if constexpr (_Is_narrow_char_t<_Elem>) {
            return string(_Input);
        } else if constexpr (_STD is_same_v<_Elem, wchar_t>) {
            return _Convert_wide_to_narrow(code_page::utf8, _Input);
        }

        string _Output;
        _Output.reserve(_Input.size()); // reserve place for new data

        // If _Elem isn't char or wchar_t, use wstring_convert,
        // instead of Windows functions (MultiByteToWideChar() and WideCharToMultiByte()).
        if constexpr (_STD is_same_v<_Elem, char8_t>) { // from char8_t
            _Output = wstring_convert<codecvt_utf8<char8_t>, char8_t>().to_bytes(_Input.data());
        } else if constexpr (_STD is_same_v<_Elem, char16_t>) { // from char16_t
            _Output = wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t>().to_bytes(_Input.data());
        } else if constexpr (_STD is_same_v<_Elem, char32_t>) { // from char32_t
            _Output = wstring_convert<codecvt_utf8<char32_t>, char32_t>().to_bytes(_Input.data());
        }
        
        return _Output;
    }

    return string();
}

// _Convert_utf_to_narrow() with string_view and wstring_view as parameter type is redutant,
// but it's important to define it, because of compilation success.
template _FILESYSTEM_API _NODISCARD string _Convert_utf_to_narrow(const string_view) noexcept;
template _FILESYSTEM_API _NODISCARD string _Convert_utf_to_narrow(const u8string_view);
template _FILESYSTEM_API _NODISCARD string _Convert_utf_to_narrow(const u16string_view);
template _FILESYSTEM_API _NODISCARD string _Convert_utf_to_narrow(const u32string_view);
template _FILESYSTEM_API _NODISCARD string _Convert_utf_to_narrow(const wstring_view);

// FUNCTION TEMPLATE _Convert_narrow_to_utf
template <class _Elem, class _Traits, class _Alloc>
_NODISCARD basic_string<_Elem, _Traits, _Alloc> _Convert_narrow_to_utf(const string_view _Input) noexcept(_Is_narrow_char_t<_Elem>) {
    using _Str_t = basic_string<_Elem, _Traits, _Alloc>;

    if (!_Input.empty()) {
        if (_Input.size() > static_cast<size_t>(INT_MAX)) {
            _Throw_system_error("_Convert_narrow_to_utf", "invalid length", error_type::runtime_error);
        }

        // check that _Elem is the default character types
        if constexpr (_Is_narrow_char_t<_Elem>) {
            return string(_Input);
        } else if constexpr (_STD is_same_v<_Elem, wchar_t>) {
            return _Convert_narrow_to_wide(code_page::utf8, _Input);
        }

        _Str_t _Output;
        _Output.reserve(_Input.size()); // reserve place for new data

        // If _Elem isn't char or wchar_t, use wstring_convert,
        // instead of Windows functions (MultiByteToWideChar() and WideCharToMultiByte()).
        if constexpr (_STD is_same_v<_Elem, char8_t>) { // to char8_t
            _Output = wstring_convert<codecvt_utf8<char8_t>, char8_t>().from_bytes(_Input.data());
        } else if constexpr (_STD is_same_v<_Elem, char16_t>) { // to char16_t
            _Output = wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t>().from_bytes(_Input.data());
        } else if constexpr (_STD is_same_v<_Elem, char32_t>) { // to char32_t
            _Output = wstring_convert<codecvt_utf8<char32_t>, char32_t>().from_bytes(_Input.data());
        }

        return _Output;
    }

    return _Str_t();
}

// _Convert_narrow_to_utf() with string and wstring return type is redutant,
// but it's important to define it, because of compilation success.
template _FILESYSTEM_API _NODISCARD string _Convert_narrow_to_utf(const string_view) noexcept;
template _FILESYSTEM_API _NODISCARD u8string _Convert_narrow_to_utf(const string_view);
template _FILESYSTEM_API _NODISCARD u16string _Convert_narrow_to_utf(const string_view);
template _FILESYSTEM_API _NODISCARD u32string _Convert_narrow_to_utf(const string_view);
template _FILESYSTEM_API _NODISCARD wstring _Convert_narrow_to_utf(const string_view);

// FUNCTION TEMPLATE _Convert_to_narrow
template <class _Elem, class _Traits>
_NODISCARD string _Convert_to_narrow(const basic_string_view<_Elem, _Traits> _Input) noexcept(_Is_narrow_char_t<_Elem>) {
    if constexpr (_Is_narrow_char_t<_Elem>) {
        return string(_Input);
    } else if constexpr (_STD is_same_v<_Elem, wchar_t>) {
        return _Convert_wide_to_narrow(code_page::utf8, _Input);
    } else { // char8_t/char16_t/char32_t/wchar_t
        return _Convert_utf_to_narrow<_Elem, _Traits>(_Input);
    }
}

// _Convert_to_narrow() with string_view parameter type is redutant,
// but it's important to define it because of compilation success.
template _FILESYSTEM_API _NODISCARD string _Convert_to_narrow(const string_view) noexcept;
template _FILESYSTEM_API _NODISCARD string _Convert_to_narrow(const u8string_view);
template _FILESYSTEM_API _NODISCARD string _Convert_to_narrow(const u16string_view);
template _FILESYSTEM_API _NODISCARD string _Convert_to_narrow(const u32string_view);
template _FILESYSTEM_API _NODISCARD string _Convert_to_narrow(const wstring_view);
_FILESYSTEM_END

#pragma warning(pop)
#endif // !_HAS_WINDOWS