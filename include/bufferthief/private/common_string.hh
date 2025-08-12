/*
 * common_string.hh
 *
 * Copyright (c) 2025 Dalton Messmer <messmer.dalton/at/gmail.com>
 * This file is part of the BufferThief library.
 *
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef BUFFER_THIEF_COMMON_STRING_H
#define BUFFER_THIEF_COMMON_STRING_H

#if !defined(__cplusplus) || __cplusplus < 201703L
#	error "BufferThief requires at least C++17. If using MSVC, be sure to set /Zc:__cplusplus"
#endif

#include <memory>
#include <string>

#if (__cpp_lib_constexpr_string >= 201907L)
#	define BT_STRING_CONSTEXPR20 constexpr
#	if (__cpp_lib_constexpr_memory >= 202202L)
#		define BT_STRING_CONSTEXPR23 constexpr
#	else
#		define BT_STRING_CONSTEXPR23 inline
#	endif
#else
#	define BT_STRING_CONSTEXPR20 inline
#	define BT_STRING_CONSTEXPR23 inline
#endif

namespace bt::detail {

template<typename CharT>
struct SupportedChar
{
	static constexpr bool value = false;
};

template<> struct SupportedChar<char> { static constexpr bool value = true; };
template<> struct SupportedChar<wchar_t> { static constexpr bool value = true; };
#if defined(__cpp_lib_char8_t)
template<> struct SupportedChar<char8_t> { static constexpr bool value = true; };
#endif
template<> struct SupportedChar<char16_t> { static constexpr bool value = true; };
template<> struct SupportedChar<char32_t> { static constexpr bool value = true; };

} // namespace bt::detail

#endif //  BUFFER_THIEF_COMMON_STRING_H
