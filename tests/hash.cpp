#include <doctest/doctest.h>

#include <auxiliary/string.hpp>

TEST_CASE("xxhash") {
	using namespace auxiliary;

	static constexpr u8string_view sv = u8"123456";
	constexpr auto val = Hash<u8string_view>()(sv);
	u8string s = sv;
	CHECK_EQ(Hash<u8string>()(s), val);
}
