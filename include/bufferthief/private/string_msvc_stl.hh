/*
 * string_msvc_stl.hh
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

#ifndef BUFFER_THIEF_STRING_IMPLEMENTED

#include "common_string.hh"

#if defined(_MSC_VER)
#define BUFFER_THIEF_STRING_IMPLEMENTED

#include "member_accessor.hh"

namespace bt::detail {

template<typename CharT>
struct Target
{
	using Alloc = std::_Rebind_alloc_t<typename std::basic_string<CharT>::allocator_type, CharT>;
	using Data = std::_String_val<std::_Simple_types<CharT>>;
	using Member = std::_Compressed_pair<Alloc, Data>;

	friend constexpr auto get(Target, std::basic_string<CharT>&) -> Member&;
};

template struct MemberAccessor<Target<char>, &std::string::_Mypair>;
template struct MemberAccessor<Target<wchar_t>, &std::wstring::_Mypair>;
#if defined(__cpp_lib_char8_t)
template struct MemberAccessor<Target<char8_t>, &std::u8string::_Mypair>;
#endif
template struct MemberAccessor<Target<char16_t>, &std::u16string::_Mypair>;
template struct MemberAccessor<Target<char32_t>, &std::u32string::_Mypair>;

//! Does not include null terminator
template<typename CharT>
constexpr auto small_string_max_size() noexcept -> std::size_t
{
	return Target<CharT>::Data::_BUF_SIZE - 1;
}

template<typename CharT>
BT_STRING_CONSTEXPR20 auto try_steal(std::basic_string<CharT>&& input) noexcept -> CharT*
{
	using Data = Target<CharT>::Data;
	using Member = Target<CharT>::Member;

	Member& internal = get(Target<CharT>{}, input);
	Data& data = internal._Myval2;

	// See _Large_mode_engaged() or _Large_string_engaged()
	if (data._Myres < Data::_BUF_SIZE) {
		// Small string
		return nullptr;
	}

	CharT* ptr = data._Bx._Ptr;

	// See _Tidy_init() and _Tidy_deallocate()
	data._Myres = Data::_BUF_SIZE - 1; // convert to small string
	data._Activate_SSO_buffer(); // just to be safe
	data._Bx._Buf[0] = CharT();  // just to be safe

	input.clear(); // ???

	return ptr;
}

template<typename CharT>
BT_STRING_CONSTEXPR20 auto uses_large_buffer(const std::basic_string<CharT>& input) noexcept -> bool
{
	return input.capacity() >= Target<CharT>::Data::_BUF_SIZE;
}

} // namespace bt::detail

#endif // _MSC_VER
#endif // BUFFER_THIEF_STRING_IMPLEMENTED
