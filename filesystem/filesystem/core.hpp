// core.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _CORE_HPP_
#define _CORE_HPP_

// v1.0.0: Add <core.hpp>, <path.hpp> and <path.cpp>.

#ifndef __cplusplus
#error Filesystem requires C++.
#endif // !__cplusplus

#ifdef _WIN32
#define _HAS_WINDOWS 1
#else // ^^^ _WIN32 ^^^ / vvv !_WIN32 vvv
#define _HAS_WINDOWS 0
#endif // _WIN32

#if !_HAS_WINDOWS
#pragma message("The contents of <core.hpp> are available only with Windows 10.")
#else // ^^^ !_HAS_WINDOWS ^^^ / vvv _HAS_WINDOWS vvv
// deprecated content
#ifndef _DEPRECATED
#define _DEPRECATED(_Depr) __declspec(deprecated(#_Depr " will be removed soon, try to avoid using it."))
#endif // _DEPRECATED

#ifndef _DEPRECATED_WITH_REPLACEMENT
#define _DEPRECATED_WITH_REPLACEMENT(_Depr, _Repl)                                        \
    __declspec(deprecated(#_Depr " will be removed soon, use " #_Repl " instead of it.")) 
#endif // _DEPRECATED_WITH_REPLACEMENT

// filesystem api
#ifdef _FS_EXPORT
#define _FS_API __declspec(dllexport)
#else // ^^^ _FS_EXPORT ^^^ / vvv !_FS_EXPORT vvv
#define _FS_API __declspec(dllimport)
#endif // _FS_EXPORT

// attributes
#ifndef _NODISCARD
#define _NODISCARD [[nodiscard]]
#endif // _NODISCARD

// exceptions
#ifndef _THROW
#define _THROW(_Errm) throw _Errm
#endif // _THROW

#ifndef _TRY_BEGIN
#define _TRY_BEGIN try {
#endif // _TRY_BEGIN

#ifndef _CATCH
#define _CATCH(_Errc) } catch (_Errc) {
#endif // _CATCH

#ifndef _CATCH_ALL
#define _CATCH_ALL } catch (...) {
#endif // _CATCH_ALL

#ifndef _CATCH_END
#define _CATCH_END }
#endif // _CATCH_END

// external
#ifndef _EXTERN_BEGIN
#define _EXTERN_BEGIN extern "C" {
#endif // _EXTERN_BEGIN

#ifndef _EXTERN_END
#define _EXTERN_END }
#endif // _EXTERN_END

// namespace
#ifndef _FS_BEGIN
#define _FS_BEGIN namespace fs {
#endif // _FS_BEGIN

#ifndef _FS_END
#define _FS_END }
#endif // _FS_END

#ifndef _FS_EXP_BEGIN
#define _FS_EXP_BEGIN \
    _FS_BEGIN         \
    namespace exp {
#endif // _FS_EXP_BEGIN

#ifndef _FS_EXP_END
#define _FS_EXP_END \
    }               \
    }
#endif // _FS_EXP_END

#ifndef _FS
#define _FS ::fs::
#endif // _FS

#ifndef _FS_EXP
#define _FS_EXP ::fs::exp::
#endif //_FS_EXP
#endif // !_HAS_WINDOWS
#endif // _CORE_HPP_