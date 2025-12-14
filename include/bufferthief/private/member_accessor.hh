/*
 * member_accessor.hh
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

/**
 * The technique for accessing private members comes from the article
 * "Access to private members: Safer nastiness." by Johannes (litb):
 * https://bloglitb.blogspot.com/2011/12/access-to-private-members-safer.html
 */

#ifndef BUFFER_THIEF_MEMBER_ACCESSOR_H
#define BUFFER_THIEF_MEMBER_ACCESSOR_H

#include <tuple>

namespace bt::detail {

template<typename T>
struct MemberTraits;

template<typename Class, typename Member>
struct MemberTraits<Member Class::*>
{
	using ClassType = Class;
	using MemberType = Member;
};

/**
 * @brief Allows access to a private non-static member whose type is public.
 *
 * If the target member exists in a nested class, the member pointers ordered
 * from the outermost member to the innermost (target) member must be provided.
 */
template<typename Tag, auto... member_ptrs>
class MemberAccessor
{
	static_assert(sizeof...(member_ptrs) > 0);
	using MemberPtrs = std::tuple<decltype(member_ptrs)...>;
	using FirstMemberPtr = std::tuple_element_t<0, MemberPtrs>;
	using LastMemberPtr = std::tuple_element_t<sizeof...(member_ptrs) - 1, MemberPtrs>;

public:
	using ClassType = typename MemberTraits<FirstMemberPtr>::ClassType;
	using MemberType = typename MemberTraits<LastMemberPtr>::MemberType;

	friend constexpr auto get(Tag, ClassType& obj) -> MemberType&
	{
		return (obj .* ... .* member_ptrs);
	}

	friend constexpr auto get(Tag, const ClassType& obj) -> const MemberType&
	{
		return (obj .* ... .* member_ptrs);
	}
};

/**
 * @brief Allows access to a private static member whose type is public.
 */
template<typename Tag, auto member, typename MemberType = decltype((member))>
class StaticMemberAccessor
{
public:
	friend constexpr auto get(Tag) -> MemberType
	{
		return static_cast<MemberType>(member);
	}
};

} // namespace bt::detail

#endif // BUFFER_THIEF_MEMBER_ACCESSOR_H
