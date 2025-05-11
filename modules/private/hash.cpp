#include "pch.hpp"

#include <auxiliary/hash.hpp>

#include <xxh3.h>

namespace auxiliary
{
	uint32_t XXHash::_xxhash32(const void* input, size_t length, uint32_t seed) {
		return XXH32(input, length, seed);
	}

	uint64_t XXHash::_xxhash64(const void* input, size_t length) {
		return XXH3_64bits(input, length);
	}

	uint64_t XXHash::_xxhash64(const void* input, size_t length, uint64_t seed) {
		return XXH3_64bits_withSeed(input, length, seed);
	}
}
