// error.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <pch.h>
#include <filesystem.hpp>

#if !_HAS_WINDOWS
#pragma message("The contents of <error.cpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv

_FILESYSTEM_BEGIN
// FUNCTION _Throw_system_error
__declspec(noreturn) void _Throw_system_error(const char* const _Errpos, const char* const _Errm, const error_type _Errc) {
    switch (_Errc) {
    case error_type::invalid_argument:
        _THROW(invalid_argument(_Errm));
        break;
    case error_type::length_error:
        _THROW(length_error(_Errm));
        break;
    case error_type::runtime_error:
        _THROW(runtime_error(_Errm));
        break;
    default: // without any type
        _RERAISE;
        break;
    }
}

// FUNCTION filesystem_error::filesystem_error
template <class _CharTy, class>
filesystem_error::filesystem_error(const _CharTy* const _Errm) {
    _Mysrc = path();
    _Mycat = error_type();
    
    if constexpr (_STD is_same_v<_CharTy, char>) {
        _Mywhat = _Errm;
    } else if constexpr (_STD is_same_v<_CharTy, wchar_t>) {
        const auto& _Cvt = _Convert_wide_to_narrow(code_page::utf8, _Errm);
        _Mywhat          = _Cvt.c_str(); 
    } else { // const char8_t*, const char16_t* and const char32_t*
        const auto& _Cvt = _Convert_utf_to_narrow<_CharTy, char_traits<_CharTy>>(_Errm);
        _Mywhat          = _Cvt.c_str();
    }
}

template _FILESYSTEM_API filesystem_error::filesystem_error(const char* const);
template _FILESYSTEM_API filesystem_error::filesystem_error(const char8_t* const);
template _FILESYSTEM_API filesystem_error::filesystem_error(const char16_t* const);
template _FILESYSTEM_API filesystem_error::filesystem_error(const char32_t* const);
template _FILESYSTEM_API filesystem_error::filesystem_error(const wchar_t* const);

template <class _CharTy, class>
filesystem_error::filesystem_error(const _CharTy* const _Errm, const error_type _Errc) {
    _Mysrc = path();
    _Mycat = _Errc;

    if constexpr (_STD is_same_v<_CharTy, char>) {
        _Mywhat = _Errm;
    } else if constexpr (_STD is_same_v<_CharTy, wchar_t>) {
        const auto& _Cvt = _Convert_wide_to_narrow(code_page::utf8, _Errm);
        _Mywhat          = _Cvt.c_str();
    } else { // const char8_t*, const char16_t* and const char32_t*
        const auto& _Cvt = _Convert_utf_to_narrow<_CharTy, char_traits<_CharTy>>(_Errm);
        _Mywhat          = _Cvt.c_str();
    }
}

template _FILESYSTEM_API filesystem_error::filesystem_error(const char* const, const error_type);
template _FILESYSTEM_API filesystem_error::filesystem_error(const char8_t* const, const error_type);
template _FILESYSTEM_API filesystem_error::filesystem_error(const char16_t* const, const error_type);
template _FILESYSTEM_API filesystem_error::filesystem_error(const char32_t* const, const error_type);
template _FILESYSTEM_API filesystem_error::filesystem_error(const wchar_t* const, const error_type);

template <class _CharTy, class>
filesystem_error::filesystem_error(const _CharTy* const _Errm, const error_type _Errc, const path& _Errpos) {
    _Mysrc = _Errpos;
    _Mycat = _Errc;

    if constexpr (_STD is_same_v<_CharTy, char>) {
        _Mywhat = _Errm;
    } else if constexpr (_STD is_same_v<_CharTy, wchar_t>) {
        const auto& _Cvt = _Convert_wide_to_narrow(code_page::utf8, _Errm);
        _Mywhat          = _Cvt.c_str();
    } else { // const char8_t*, const char16_t* and const char32_t*
        const auto& _Cvt = _Convert_utf_to_narrow<_CharTy, char_traits<_CharTy>>(_Errm);
        _Mywhat          = _Cvt.c_str();
    }
}

template _FILESYSTEM_API filesystem_error::filesystem_error(const char* const, const error_type, const path&);
template _FILESYSTEM_API filesystem_error::filesystem_error(const char8_t* const, const error_type, const path&);
template _FILESYSTEM_API filesystem_error::filesystem_error(const char16_t* const, const error_type, const path&);
template _FILESYSTEM_API filesystem_error::filesystem_error(const char32_t* const, const error_type, const path&);
template _FILESYSTEM_API filesystem_error::filesystem_error(const wchar_t* const, const error_type, const path&);

// FUNCTION filesystem_error::category
_NODISCARD const error_type filesystem_error::category() const noexcept {
    return _Mycat;
}

// FUNCTION filesystem_error::source
_NODISCARD const path& filesystem_error::source() const noexcept {
    return _Mysrc;
}

// FUNCTION filesystem_error::what
_NODISCARD const char* filesystem_error::what() const noexcept {
    return _Mywhat;
}

// FUNCTION _Throw_fs_error
__declspec(noreturn) void _Throw_fs_error(const char* const _Errm) {
    _THROW(filesystem_error(_Errm));
}

__declspec(noreturn) void _Throw_fs_error(const char* const _Errm, const error_type _Errc) {
    _THROW(filesystem_error(_Errm, _Errc));
}

__declspec(noreturn) void _Throw_fs_error(const char* const _Errm, const error_type _Errc, const path& _Errpos) {
    _THROW(filesystem_error(_Errm, _Errc, _Errpos));
}
_FILESYSTEM_END

#endif // !_HAS_WINDOWS