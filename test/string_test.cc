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

#if defined(BT_COPY_BUFFERS)
#	error "BufferThief must not be configured with BT_COPY_BUFFERS for these tests"
#endif

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
	if (p) { ++deallocations_; }
}

///////////////////////////////////////////////////

TEST_F(StringTest, StealCharEmpty)
{
	auto s1 = bt::steal(generateString<char>(0));
	EXPECT_NE(s1.get(), nullptr);
	EXPECT_EQ(s1[0], '\0');
	EXPECT_EQ(allocations_, 1);
	EXPECT_EQ(deallocations_, 0);

	auto s2 = generateString<char>(0);
	EXPECT_EQ(bt::uses_large_buffer(s2), false);
	EXPECT_EQ(bt::try_steal(s2), nullptr);
	EXPECT_EQ(allocations_, 0);
	EXPECT_EQ(deallocations_, 0);
}

TEST_F(StringTest, StealCharSmall)
{
	{
		auto s1 = bt::steal(generateString<char>(3));
		EXPECT_NE(s1.get(), nullptr);
		EXPECT_EQ(std::char_traits<char>::length(s1.get()), 3);
		EXPECT_EQ(std::char_traits<char>::compare(s1.get(), "abc", 3), 0);
	}
	EXPECT_EQ(allocations_, 1);
	EXPECT_EQ(deallocations_, 1);

	{
		auto s2 = generateString<char>(3);
		EXPECT_EQ(bt::uses_large_buffer(s2), false);
		EXPECT_EQ(bt::try_steal(s2), nullptr);
	}
	EXPECT_EQ(allocations_, 0);
	EXPECT_EQ(deallocations_, 0);
}

TEST_F(StringTest, StealCharSmallMax)
{
	auto s1 = bt::steal(generateString<char>(bt::small_string_max_size<char>()));
	EXPECT_NE(s1.get(), nullptr);
	EXPECT_EQ(std::char_traits<char>::length(s1.get()), bt::small_string_max_size<char>());
	EXPECT_EQ(allocations_, 1);
	EXPECT_EQ(deallocations_, 0);

	auto s2 = generateString<char>(bt::small_string_max_size<char>());
	EXPECT_EQ(bt::uses_large_buffer(s2), false);
	EXPECT_EQ(bt::try_steal(s2), nullptr);
	EXPECT_EQ(allocations_, 0);
	EXPECT_EQ(deallocations_, 0);
}

TEST_F(StringTest, StealCharLongMin)
{
	auto s1 = bt::steal(generateString<char>(bt::small_string_max_size<char>() + 1));
	EXPECT_NE(s1.get(), nullptr);
	EXPECT_EQ(std::char_traits<char>::length(s1.get()), bt::small_string_max_size<char>() + 1);
	EXPECT_EQ(allocations_, 0);
	EXPECT_EQ(deallocations_, 0);

	auto s2 = generateString<char>(bt::small_string_max_size<char>() + 1);
	EXPECT_EQ(bt::uses_large_buffer(s2), true);
	auto s3 = bt::try_steal(s2);
	EXPECT_NE(s3, nullptr);
	EXPECT_EQ(allocations_, 0);
	EXPECT_EQ(deallocations_, 0);
}

TEST_F(StringTest, StealCharLongMin2)
{
	{
		auto s1 = bt::steal(generateString<char>(bt::small_string_max_size<char>() + 1));
		EXPECT_NE(s1.get(), nullptr);
		EXPECT_EQ(std::char_traits<char>::length(s1.get()), bt::small_string_max_size<char>() + 1);
	}
	EXPECT_EQ(allocations_, 0);
	EXPECT_EQ(deallocations_, 1);

	{
		auto s2 = generateString<char>(bt::small_string_max_size<char>() + 1);
		EXPECT_EQ(bt::uses_large_buffer(s2), true);
		auto s3 = bt::try_steal(s2);
		EXPECT_NE(s3, nullptr);
	}
	EXPECT_EQ(allocations_, 0);
	EXPECT_EQ(deallocations_, 1);
}
