// conversion.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <pch.h>
#include <filesystem.hpp>

#if !_HAS_WINDOWS
#pragma message("The contents of <conversion.cpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv
#pragma warning(push)
#pragma warning(disable : 4996) // C4996: using deprecated content (<codecvt>)

_FILESYSTEM_BEGIN
_EXPERIMENTAL_BEGIN
// FUNCTION _Convert_narrow_to_wide
_NODISCARD wstring __cdecl _Convert_narrow_to_wide(const code_page _Code_page, const string_view _Input) {
    if (!_Input.empty()) {
        if (_Input.size() > static_cast<size_t>(INT_MAX)) {
            _Throw_system_error("_Convert_narrow_to_wide", "invalid length", error_type::length_error);
        }

        wstring _Output;
        const int _Input_size  = static_cast<int>(_Input.size());
        const int _Output_size = _Input_size;
    
        // MultiByteToWideChar() replaces multi byte to wide characters,
        // _Output size must be same as _Input size
        _Output.resize(_Output_size, L' ');

        if (!_CSTD MultiByteToWideChar(static_cast<uint32_t>(_Code_page), MB_ERR_INVALID_CHARS,
            _Input.data(), _Input_size, _Output.data(), _Output_size)) { // failed to convert multi byte to wide characters
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
_NODISCARD string __cdecl _Convert_wide_to_narrow(const code_page _Code_page, const wstring_view _Input) {
    if (!_Input.empty()) {
        if (_Input.size() > static_cast<size_t>(INT_MAX)) {
            _Throw_system_error("_Convert_wide_to_narrow", "invalid length", error_type::length_error);
        }

        string _Output;
        const int _Input_size  = static_cast<int>(_Input.size());
        const int _Output_size = _Input_size;

        // WideCharToMultiByte() replaces multi byte to wide characters.
        // _Output size must be same as _Input size
        _Output.resize(_Output_size, ' ');

        if (!_CSTD WideCharToMultiByte(static_cast<uint32_t>(_Code_page), WC_ERR_INVALID_CHARS,
            _Input.data(), _Input_size, _Output.data(), _Output_size, nullptr, nullptr)) { 
            // failed to convert wide to multi byte charracters 
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
template<class _Elem, class _Traits>
_NODISCARD string __cdecl _Convert_utf_to_narrow(const basic_string_view<_Elem, _Traits> _Input) {
    if (!_Input.empty()) {
        if (_Input.size() > static_cast<size_t>(INT_MAX)) {
            _Throw_system_error("_Convert_utf_to_wide", "invalid length", error_type::length_error);
        }

        string _Output;

        _Output.reserve(_Input.size()); // reserve place for new data

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

template _FILESYSTEM_API _NODISCARD string __cdecl _Convert_utf_to_narrow(const u8string_view);
template _FILESYSTEM_API _NODISCARD string __cdecl _Convert_utf_to_narrow(const u16string_view);
template _FILESYSTEM_API _NODISCARD string __cdecl _Convert_utf_to_narrow(const u32string_view);

// FUNCTION TEMPLATE _Convert_narrow_to_utf
template<class _Elem, class _Traits, class _Alloc>
_NODISCARD basic_string<_Elem, _Traits, _Alloc> __cdecl  _Convert_narrow_to_utf(const string_view _Input) {
    using _Mystr = basic_string<_Elem, _Traits, _Alloc>;

    if (!_Input.empty()) {
        if (_Input.size() > static_cast<size_t>(INT_MAX)) {
            _Throw_system_error("_Convert_narrow_to_utf", "invalid length", error_type::runtime_error);
        }

        _Mystr _Output;

        _Output.reserve(_Input.size()); // reserve place for new data

        if constexpr (_STD is_same_v<_Elem, char8_t>) { // to char8_t
            _Output = wstring_convert<codecvt_utf8<char8_t>, char8_t>().from_bytes(_Input.data());
        } else if constexpr (_STD is_same_v<_Elem, char16_t>) { // to char16_t
            _Output = wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t>().from_bytes(_Input.data());
        } else if constexpr (_STD is_same_v<_Elem, char32_t>) { // to char32_t
            _Output = wstring_convert<codecvt_utf8<char32_t>, char32_t>().from_bytes(_Input.data());
        }

        return _Output;
    }

    return _Mystr();
}

template _FILESYSTEM_API _NODISCARD u8string __cdecl _Convert_narrow_to_utf(const string_view);
template _FILESYSTEM_API _NODISCARD u16string __cdecl _Convert_narrow_to_utf(const string_view);
template _FILESYSTEM_API _NODISCARD u32string __cdecl _Convert_narrow_to_utf(const string_view);
_EXPERIMENTAL_END
_FILESYSTEM_END

#pragma warning(pop)
#endif // !_HAS_WINDOWS