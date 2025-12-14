/*
 * common_vector.hh
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

#ifndef BUFFER_THIEF_COMMON_VECTOR_H
#define BUFFER_THIEF_COMMON_VECTOR_H

#if !defined(__cplusplus)
	|| (defined(_MSVC_LANG) && _MSVC_LANG < 201703L)
	|| (!defined(_MSVC_LANG) || __cplusplus < 201703L)
#	error BufferThief requires at least C++17
#endif

#include <memory>
#include <vector>

#if (__cpp_lib_constexpr_vector >= 201907L)
#	define BT_VECTOR_CONSTEXPR20 constexpr
#	if (__cpp_lib_constexpr_memory >= 202202L)
#		define BT_VECTOR_CONSTEXPR23 constexpr
#	else
#		define BT_VECTOR_CONSTEXPR23 inline
#	endif
#else
#	define BT_VECTOR_CONSTEXPR20 inline
#	define BT_VECTOR_CONSTEXPR23 inline
#endif

#endif //  BUFFER_THIEF_COMMON_VECTOR_H
