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

#include "bufferthief/config.hh"

#include "private/string_libstdc++.hh"
#include "private/string_msvc_stl.hh"
#if !defined(BUFFER_THIEF_STRING_IMPLEMENTED)
#	error "No supported C++ Standard Library implementation detected"
#endif

#include <memory>

namespace bt {

// TODO:
// - Better error messages for unsupported CharT types
// - std::pmr::polymorphic_allocator support

template<typename CharT>
inline auto steal(std::basic_string<CharT>&& input) -> std::unique_ptr<CharT[]>
{
#if BT_USE_UNSTABLE_STEAL == 1
	if (auto buffer = detail::steal_impl(std::move(input))) {
		return std::unique_ptr<CharT[]>{buffer};
	}
	else
#endif // BT_USE_UNSTABLE_STEAL
	{
		// Copy the buffer
		CharT* copy = new CharT[input.size() + 1];
		std::char_traits<CharT>::copy(copy, input.c_str(), input.size());
		copy[input.size()] = CharT();

		return std::unique_ptr<CharT[]>{copy};
	}
}

} // namespace bt

#undef BUFFER_THIEF_STRING_IMPLEMENTED

#endif // BUFFER_THIEF_STRING_H
