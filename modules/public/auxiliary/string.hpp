#pragma once

#include "hash.hpp"

#include <u8lib/string.hpp>

namespace auxiliary
{
	using u8string = u8lib::u8string;
	using u8string_view = u8lib::u8string_view;

	template<>
	struct Hash<u8string_view> {
		constexpr size_t operator()(u8string_view value) const noexcept {
			return XXHash::xxhash(value);
		}
	};

	template<>
	struct Hash<u8string> {
		size_t operator()(const u8string& value) const noexcept {
			return XXHash::xxhash(value);
		}
	};
}
