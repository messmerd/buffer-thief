/*
 * vector_libstdc++.hh
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

#ifndef BUFFER_THIEF_VECTOR_IMPLEMENTED

#include "common_vector.hh"

#if defined(__GLIBCXX__)
#define BUFFER_THIEF_VECTOR_IMPLEMENTED

#include <type_traits>

namespace bt::detail {

template<typename T>
BT_VECTOR_CONSTEXPR20 auto steal(std::vector<T>& input) noexcept -> T*
{
	using Base = std::_Vector_base<T, std::allocator<T>>;
	using Impl = Base::_Vector_impl;

#if defined(__cpp_lib_is_pointer_interconvertible) && __cpp_lib_is_pointer_interconvertible >= 201907L
	static_assert(std::is_pointer_interconvertible_base_of_v<Base, std::vector<T>>);
#else
	static_assert(std::is_base_of_v<Base, std::vector<T>>);
#endif

	// C-style cast allows converting derived class to inaccessible base class
	Impl& impl = ((Base&)input)._M_impl;

	T* ptr = impl._M_start;

	impl._M_start = nullptr;
	impl._M_finish = nullptr;
	impl._M_end_of_storage = nullptr;

	return ptr;
}

} // namespace bt::detail

#endif // __GLIBCXX__
#endif // BUFFER_THIEF_VECTOR_IMPLEMENTED
