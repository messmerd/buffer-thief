/*
 * string_test.cc
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

#include <bufferthief/string.hh>
#include <gtest/gtest.h>

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <type_traits>
#include <utility>

static_assert(bt::using_unstable_steal(), "bufferthief must be configured with -DBT_USE_UNSTABLE_STEAL=1");

namespace {

auto allocations_ = 0;
auto deallocations_ = 0;

void resetAllocDealloc()
{
	allocations_ = 0;
	deallocations_ = 0;
}

} // namespace

//! Test fixture for strings
class StringTest : public ::testing::Test
{
public:
	StringTest() = default;
	~StringTest() override = default;

	void SetUp() override
	{
		resetAllocDealloc();
	}

	void TearDown() override {}

	template<typename CharT>
	auto generateString(std::size_t length, std::size_t reserve = 0) const -> std::basic_string<CharT>
	{
		std::basic_string<CharT> ret;
		if (reserve > 0) {
			ret.reserve(reserve);
		}

		auto getChar = [](std::size_t offset) -> CharT {
			offset %= 26;
			if constexpr (std::is_same_v<CharT, char>) { return 'a' + offset; }
			else if constexpr (std::is_same_v<CharT, wchar_t>) { return L'a' + offset; }
#if defined(__cpp_lib_char8_t)
			else if constexpr (std::is_same_v<CharT, char8_t>) { return u8'a' + offset; }
#endif
			else if constexpr (std::is_same_v<CharT, char16_t>) { return u'a' + offset; }
			else if constexpr (std::is_same_v<CharT, char32_t>) { return U'a' + offset; }
			else { throw std::runtime_error{"Unsupported character type"}; }
		};

		for (std::size_t i = 0; i < length; ++i) {
			ret += getChar(i);
		}

		resetAllocDealloc();

		return ret;
	}
};

void* operator new(std::size_t n) noexcept(false)
{
	void* p = std::malloc(n);
	if (!p) { throw std::bad_alloc{}; }
	++allocations_;
	return p;
}

void operator delete(void* p) noexcept
{
	std::free(p);
	++deallocations_;
}

///////////////////////////////////////////////////

TEST_F(StringTest, CharEmpty)
{
	auto s1 = bt::steal(generateString<char>(0));
	EXPECT_NE(s1.get(), nullptr);
	EXPECT_EQ(s1[0], '\0');
	EXPECT_EQ(allocations_, 1);
	EXPECT_EQ(deallocations_, 0);
}

TEST_F(StringTest, CharSmall)
{
	{
		auto s2 = bt::steal(generateString<char>(3));
		EXPECT_NE(s2.get(), nullptr);
		EXPECT_EQ(std::char_traits<char>::length(s2.get()), 3);
		EXPECT_EQ(std::char_traits<char>::compare(s2.get(), "abc", 3), 0);
	}
	EXPECT_EQ(allocations_, 1);
	EXPECT_EQ(deallocations_, 1);
}

TEST_F(StringTest, CharSmallMax)
{
	auto s3 = bt::steal(generateString<char>(bt::detail::kSmallStringMaxSize<char>));
	EXPECT_NE(s3.get(), nullptr);
	EXPECT_EQ(std::char_traits<char>::length(s3.get()), bt::detail::kSmallStringMaxSize<char>);
	EXPECT_EQ(allocations_, 1);
	EXPECT_EQ(deallocations_, 0);
}

TEST_F(StringTest, CharLongMin)
{
	auto s4 = bt::steal(generateString<char>(bt::detail::kSmallStringMaxSize<char> + 1));
	EXPECT_NE(s4.get(), nullptr);
	EXPECT_EQ(std::char_traits<char>::length(s4.get()), bt::detail::kSmallStringMaxSize<char> + 1);
	EXPECT_EQ(allocations_, 0);
	EXPECT_EQ(deallocations_, 0);
}

TEST_F(StringTest, CharLongMin2)
{
	{
		auto s5 = bt::steal(generateString<char>(bt::detail::kSmallStringMaxSize<char> + 1));
		EXPECT_NE(s5.get(), nullptr);
		EXPECT_EQ(std::char_traits<char>::length(s5.get()), bt::detail::kSmallStringMaxSize<char> + 1);
	}
	EXPECT_EQ(allocations_, 0);
	EXPECT_EQ(deallocations_, 1);
}
