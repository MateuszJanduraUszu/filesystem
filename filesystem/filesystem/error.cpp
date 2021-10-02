// error.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <filesystem_pch.hpp>
#include <filesystem.hpp>

#if !_HAS_WINDOWS
#pragma message("The contents of <error.cpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv

_FILESYSTEM_BEGIN
// FUNCTION _Throw_system_error
__declspec(noreturn) void _Throw_system_error(const char* const _Src, const char* const _Msg, const error_type _Code) {
    // construct message in 'src: error' format
    string _Full_msg = _Src; // append error source
    _Full_msg       += ": ";
    _Full_msg       += _Msg; // append error message
    switch (_Code) {
    case error_type::invalid_argument:
        _THROW(invalid_argument(_Full_msg));
        break;
    case error_type::length_error:
        _THROW(length_error(_Full_msg));
        break;
    case error_type::runtime_error:
        _THROW(runtime_error(_Full_msg));
        break;
    default: // without any type
        _RERAISE;
        break;
    }
}

// FUNCTION filesystem_error constructors/destructor
filesystem_error::filesystem_error() noexcept
    : _Mysrc(path{}), _Mycat(error_type{}), _Mywhat(nullptr) {}

template <class _CharTy>
filesystem_error::filesystem_error(const _CharTy* const _Msg) noexcept(_Is_narrow_char_t<_CharTy>) {
    // _CharTy must be an character (char/char8_t/char16_t/char32_t/wchar_t) type
    static_assert(_Is_char_t<_CharTy>, "invalid character type");
    _Mysrc           = path();
    _Mycat           = error_type();
    const auto& _Tmp = _Convert_to_narrow<_CharTy, char_traits<_CharTy>>(_Msg);
    _Mywhat          = _Tmp.c_str();
}

template _FILESYSTEM_API filesystem_error::filesystem_error(const char* const) noexcept;
template _FILESYSTEM_API filesystem_error::filesystem_error(const char8_t* const);
template _FILESYSTEM_API filesystem_error::filesystem_error(const char16_t* const);
template _FILESYSTEM_API filesystem_error::filesystem_error(const char32_t* const);
template _FILESYSTEM_API filesystem_error::filesystem_error(const wchar_t* const);

template <class _CharTy>
filesystem_error::filesystem_error(const _CharTy* const _Msg, const error_type _Code) noexcept(_Is_narrow_char_t<_CharTy>) {
    // _CharTy must be an character (char/char8_t/char16_t/char32_t/wchar_t) type
    static_assert(_Is_char_t<_CharTy>, "invalid character type");
    _Mysrc           = path();
    _Mycat           = _Code;
    const auto& _Tmp = _Convert_to_narrow<_CharTy, char_traits<_CharTy>>(_Msg);
    _Mywhat          = _Tmp.c_str();
}

template _FILESYSTEM_API filesystem_error::filesystem_error(const char* const, const error_type) noexcept;
template _FILESYSTEM_API filesystem_error::filesystem_error(const char8_t* const, const error_type);
template _FILESYSTEM_API filesystem_error::filesystem_error(const char16_t* const, const error_type);
template _FILESYSTEM_API filesystem_error::filesystem_error(const char32_t* const, const error_type);
template _FILESYSTEM_API filesystem_error::filesystem_error(const wchar_t* const, const error_type);

template <class _CharTy>
filesystem_error::filesystem_error(const _CharTy* const _Msg, const error_type _Code, const path& _Src) noexcept(_Is_narrow_char_t<_CharTy>) {
    // _CharTy must be an character (char/char8_t/char16_t/char32_t/wchar_t) type
    static_assert(_Is_char_t<_CharTy>, "invalid character type");
    _Mysrc           = _Src;
    _Mycat           = _Code;
    const auto& _Tmp = _Convert_to_narrow<_CharTy, char_traits<_CharTy>>(_Msg);
    _Mywhat          = _Tmp.c_str();
}

template _FILESYSTEM_API filesystem_error::filesystem_error(const char* const, const error_type, const path&) noexcept;
template _FILESYSTEM_API filesystem_error::filesystem_error(const char8_t* const, const error_type, const path&);
template _FILESYSTEM_API filesystem_error::filesystem_error(const char16_t* const, const error_type, const path&);
template _FILESYSTEM_API filesystem_error::filesystem_error(const char32_t* const, const error_type, const path&);
template _FILESYSTEM_API filesystem_error::filesystem_error(const wchar_t* const, const error_type, const path&);

filesystem_error::~filesystem_error() noexcept {}

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
__declspec(noreturn) void _Throw_fs_error(const char* const _Msg) {
    _THROW(filesystem_error(_Msg));
}

__declspec(noreturn) void _Throw_fs_error(const char* const _Msg, const error_type _Code) {
    _THROW(filesystem_error(_Msg, _Code));
}

__declspec(noreturn) void _Throw_fs_error(const char* const _Msg, const error_type _Code, const path& _Src) {
    _THROW(filesystem_error(_Msg, _Code, _Src));
}
_FILESYSTEM_END

#endif // !_HAS_WINDOWS