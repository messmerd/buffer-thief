/*
 * vector.hh - Utility for stealing the internal buffer of std::vector
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

#ifndef BUFFER_THIEF_VECTOR_H
#define BUFFER_THIEF_VECTOR_H

#undef BUFFER_THIEF_VECTOR_IMPLEMENTED
#if !defined(BT_COPY_BUFFERS)
//#	include "private/vector_libc++.hh"
#	include "private/vector_libstdc++.hh"
//#	include "private/vector_msvc_stl.hh"
#	if !defined(BUFFER_THIEF_VECTOR_IMPLEMENTED)
#		error "No supported C++ Standard Library implementation detected"
#	endif
#endif

namespace bt {

// TODO:
// - std::pmr::polymorphic_allocator support
// - ASAN compatibility

#if defined(BT_COPY_BUFFERS)
#define BT_NOEXCEPT false
#else
#define BT_NOEXCEPT true
#endif

template<typename T>
BT_VECTOR_CONSTEXPR23 auto steal(std::vector<T>&& input) noexcept(BT_NOEXCEPT) -> std::unique_ptr<T[]>
{
#if !defined(BT_COPY_BUFFERS)
	return std::unique_ptr<T[]>{detail::steal(input)};
#else
	if (input.empty()) { return nullptr; }
	
	T* copy = std::allocator<T>{}.allocate(input.size());
	std::uninitialized_copy(input.begin(), input.end(), copy);

	return std::unique_ptr<T[]>{copy};
#endif
}

#undef BT_NOEXCEPT

} // namespace bt

#undef BUFFER_THIEF_VECTOR_IMPLEMENTED

#endif // BUFFER_THIEF_VECTOR_H
