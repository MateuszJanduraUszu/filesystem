// build_filesystem_dll.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <filesystem_pch.hpp>

#ifndef _MAYBE_UNUSED
#if _HAS_CXX20
#define _MAYBE_UNUSED [[maybe_unused]]
#else // ^^^ _HAS_CXX20 ^^^ / vvv !_HAS_CXX20 vvv
#define _MAYBE_UNUSED
#endif // _HAS_CXX20
#endif // _MAYBE_UNUSED

int __stdcall DllMain(HMODULE, unsigned long _Reason, _MAYBE_UNUSED void*) {
    switch (_Reason) {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }

    return 1;
}

// won't be used anymore
#ifdef _MAYBE_UNUSED
#undef _MAYBE_UNUSED
#endif // _MAYBE_UNUSED