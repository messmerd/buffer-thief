/*
 * string_libstdc++.hh
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

#include <string>

#if defined(__GLIBCXX__)
#define BUFFER_THIEF_STRING_IMPLEMENTED

#if !defined(_GLIBCXX_USE_CXX11_ABI) || _GLIBCXX_USE_CXX11_ABI != 1
#	error "Only the C++11 ABI is supported"
#endif

#include "member_accessor.hh"

namespace bt::detail {

//! Does not include null terminator
template<typename CharT>
inline constexpr auto kSmallStringMaxSize = std::size_t{15 / sizeof(CharT)};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-template-friend"

template<typename CharT>
struct PointerTarget
{
	friend constexpr auto get(PointerTarget, std::basic_string<CharT>&) -> CharT*&;
};

template<typename CharT>
struct LocalPointerTarget
{
	using type = CharT(&)[kSmallStringMaxSize<CharT> + 1];
	friend constexpr auto get(LocalPointerTarget, std::basic_string<CharT>&) -> type;
};

#pragma GCC diagnostic pop

template struct MemberAccessor<PointerTarget<char>, &std::string::_M_dataplus, &std::string::_Alloc_hider::_M_p>;
template struct MemberAccessor<PointerTarget<wchar_t>, &std::wstring::_M_dataplus, &std::wstring::_Alloc_hider::_M_p>;
#if defined(__cpp_lib_char8_t)
template struct MemberAccessor<PointerTarget<char8_t>, &std::u8string::_M_dataplus, &std::u8string::_Alloc_hider::_M_p>;
#endif
template struct MemberAccessor<PointerTarget<char16_t>, &std::u16string::_M_dataplus, &std::u16string::_Alloc_hider::_M_p>;
template struct MemberAccessor<PointerTarget<char32_t>, &std::u32string::_M_dataplus, &std::u32string::_Alloc_hider::_M_p>;

template struct MemberAccessor<LocalPointerTarget<char>, &std::string::_M_local_buf>;
template struct MemberAccessor<LocalPointerTarget<wchar_t>, &std::wstring::_M_local_buf>;
#if defined(__cpp_lib_char8_t)
template struct MemberAccessor<LocalPointerTarget<char8_t>, &std::u8string::_M_local_buf>;
#endif
template struct MemberAccessor<LocalPointerTarget<char16_t>, &std::u16string::_M_local_buf>;
template struct MemberAccessor<LocalPointerTarget<char32_t>, &std::u32string::_M_local_buf>;

template<typename CharT>
constexpr auto steal_impl(std::basic_string<CharT>&& input) noexcept -> CharT*
{
	CharT*& internal_ptr = get(PointerTarget<CharT>{}, input);
	CharT* local_buffer_ptr = get(LocalPointerTarget<CharT>{}, input);

	if (internal_ptr == local_buffer_ptr)
	{
		// Small string
		return nullptr;
	}

	CharT* ptr = internal_ptr;
	internal_ptr = local_buffer_ptr; // convert to small string
	internal_ptr[0] = CharT(); // just in case

	input.clear(); // ???

	return ptr;
}

} // namespace bt::detail

#endif // __GLIBCXX__
#endif // BUFFER_THIEF_STRING_IMPLEMENTED
