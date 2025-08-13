/*
 * string_libc++.hh
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

#if defined(_LIBCPP_VERSION)
#define BUFFER_THIEF_STRING_IMPLEMENTED

// TODO: Check that the _LIBCPP_ABI_ALTERNATE_STRING_LAYOUT new string ABI is supported

#include "member_accessor.hh"

namespace bt::detail {

template<typename CharT>
struct MinCapTarget
{
	friend constexpr auto get(MinCapTarget) -> std::size_t;
};

template struct StaticMemberAccessor<MinCapTarget<char>, std::string::__min_cap, std::size_t>;
template struct StaticMemberAccessor<MinCapTarget<wchar_t>, std::wstring::__min_cap, std::size_t>;
#if defined(__cpp_lib_char8_t)
template struct StaticMemberAccessor<MinCapTarget<char8_t>, std::u8string::__min_cap, std::size_t>;
#endif
template struct StaticMemberAccessor<MinCapTarget<char16_t>, std::u16string::__min_cap, std::size_t>;
template struct StaticMemberAccessor<MinCapTarget<char32_t>, std::u32string::__min_cap, std::size_t>;

//! Does not include null terminator
template<typename CharT>
constexpr auto small_string_max_size() noexcept -> std::size_t
{
	return get(MinCapTarget<CharT>{}) - 1;
}

template<typename CharT>
struct IsLongTarget
{
	friend constexpr auto get(IsLongTarget) -> bool(std::basic_string<CharT>::*)() const noexcept;
};

template struct StaticMemberAccessor<IsLongTarget<char>, &std::string::__is_long>;
template struct StaticMemberAccessor<IsLongTarget<wchar_t>, &std::wstring::__is_long>;
#if defined(__cpp_lib_char8_t)
template struct StaticMemberAccessor<IsLongTarget<char8_t>, &std::u8string::__is_long>;
#endif
template struct StaticMemberAccessor<IsLongTarget<char16_t>, &std::u16string::__is_long>;
template struct StaticMemberAccessor<IsLongTarget<char32_t>, &std::u32string::__is_long>;

template<typename CharT>
BT_STRING_CONSTEXPR20 auto uses_large_buffer(const std::basic_string<CharT>& input) noexcept -> bool
{
	return (input.*get(IsLongTarget<CharT>{}))();
}

template<typename CharT>
struct SetShortSizeTarget
{
	using size_type = std::basic_string<CharT>::size_type;
	friend constexpr auto get(SetShortSizeTarget) -> void(std::basic_string<CharT>::*)(size_type) noexcept;
};

template struct StaticMemberAccessor<SetShortSizeTarget<char>, &std::string::__set_short_size>;
template struct StaticMemberAccessor<SetShortSizeTarget<wchar_t>, &std::wstring::__set_short_size>;
#if defined(__cpp_lib_char8_t)
template struct StaticMemberAccessor<SetShortSizeTarget<char8_t>, &std::u8string::__set_short_size>;
#endif
template struct StaticMemberAccessor<SetShortSizeTarget<char16_t>, &std::u16string::__set_short_size>;
template struct StaticMemberAccessor<SetShortSizeTarget<char32_t>, &std::u32string::__set_short_size>;

template<typename CharT>
BT_STRING_CONSTEXPR20 auto try_steal(std::basic_string<CharT>& input) noexcept -> CharT*
{
	if (!uses_large_buffer<CharT>(input))
	{
		// Small string
		return nullptr;
	}

	CharT* ptr = input.data();

	// Convert to small string
	(input.*get(SetShortSizeTarget<CharT>{}))(0);

	// Just in case
	input.data()[0] = CharT();

	input.clear();

	return ptr;
}

} // namespace bt::detail

#endif // _LIBCPP_VERSION
#endif // BUFFER_THIEF_STRING_IMPLEMENTED
