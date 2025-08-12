/*
 * string.hh - Utility for stealing the internal buffer of std::basic_string
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

#ifndef BUFFER_THIEF_STRING_H
#define BUFFER_THIEF_STRING_H

#undef BUFFER_THIEF_STRING_IMPLEMENTED
#if !defined(BT_COPY_BUFFERS)
#	include "private/string_libstdc++.hh"
#	include "private/string_msvc_stl.hh"
#	if !defined(BUFFER_THIEF_STRING_IMPLEMENTED)
#		error "No supported C++ Standard Library implementation detected"
#	endif
#endif

namespace bt {

// TODO:
// - Better error messages for unsupported CharT types
// - std::pmr::polymorphic_allocator support

template<typename CharT>
BT_STRING_CONSTEXPR23 auto try_steal(std::basic_string<CharT>&& input) noexcept -> std::unique_ptr<CharT[]>
{
	static_assert(detail::SupportedChar<CharT>::value, "Unsupported character type");

#if !defined(BT_COPY_BUFFERS)
	return std::unique_ptr<CharT[]>{detail::try_steal(std::move(input))};
#else
	return nullptr;
#endif
}

template<typename CharT>
BT_STRING_CONSTEXPR23 auto steal(std::basic_string<CharT>&& input) -> std::unique_ptr<CharT[]>
{
	static_assert(detail::SupportedChar<CharT>::value, "Unsupported character type");

#if !defined(BT_COPY_BUFFERS)
	if (auto buffer = detail::try_steal(std::move(input))) {
		return std::unique_ptr<CharT[]>{buffer};
	}
	else
#endif
	{
		// Copy the buffer
		CharT* copy = new CharT[input.size() + 1];
		std::char_traits<CharT>::copy(copy, input.c_str(), input.size());
		copy[input.size()] = CharT();

		return std::unique_ptr<CharT[]>{copy};
	}
}

#if !defined(BT_COPY_BUFFERS)

template<typename CharT>
BT_STRING_CONSTEXPR20 auto uses_large_buffer(const std::basic_string<CharT>& input) noexcept -> bool
{
	static_assert(detail::SupportedChar<CharT>::value, "Unsupported character type");
	return detail::uses_large_buffer(input);
}

//! Does not include null terminator
template<typename CharT>
constexpr auto small_string_max_size() noexcept -> std::size_t
{
	static_assert(detail::SupportedChar<CharT>::value, "Unsupported character type");
	return detail::small_string_max_size<CharT>();
}

#endif

} // namespace bt

#undef BUFFER_THIEF_STRING_IMPLEMENTED

#endif // BUFFER_THIEF_STRING_H
