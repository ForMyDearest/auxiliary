#pragma once

#include "config/key_words.h"

#include <bit>
#include <iterator>

namespace auxiliary::internal
{
	enum { architecture_big_endian = (std::endian::native == std::endian::big) };

	template<typename T>
	constexpr uint32_t read32(const T* v) {
		auto get = [](T value) { return static_cast<uint32_t>(static_cast<uint8_t>(value)); };
		if constexpr (architecture_big_endian) {
			return get(v[3]) | (get(v[2]) << 8) | (get(v[1]) << 16) | (get(v[0]) << 24);
		} else {
			return get(v[0]) | (get(v[1]) << 8) | (get(v[2]) << 16) | (get(v[3]) << 24);
		}
	}

	template<typename T>
	constexpr uint64_t read64(const T* ptr) noexcept {
		if constexpr (architecture_big_endian) {
			return (static_cast<uint64_t>(read32(ptr)) << 32) | static_cast<uint64_t>(read32(ptr + 4));
		} else {
			return static_cast<uint64_t>(read32(ptr)) | (static_cast<uint64_t>(read32(ptr + 4)) << 32);
		}
	}

	constexpr std::pair<uint64_t, uint64_t> mult64to128(const uint64_t lhs, const uint64_t rhs) noexcept {
		const uint64_t lo_lo = static_cast<uint64_t>(static_cast<uint32_t>(lhs)) * static_cast<uint32_t>(rhs);
		const uint64_t hi_lo = (lhs >> 32) * static_cast<uint32_t>(rhs);
		const uint64_t lo_hi = static_cast<uint32_t>(lhs) * (rhs >> 32);
		const uint64_t hi_hi = (lhs >> 32) * (rhs >> 32);
		const uint64_t cross = (lo_lo >> 32) + static_cast<uint32_t>(hi_lo) + lo_hi;
		uint64_t upper = (hi_lo >> 32) + (cross >> 32) + hi_hi;
		uint64_t lower = (cross << 32) | static_cast<uint32_t>(lo_lo);
		return {lower, upper};
	}

	constexpr uint64_t mul128_fold64(uint64_t lhs, uint64_t rhs) noexcept {
#if defined __GNUC__ && __WORDSIZE >= 64
		// It appears both GCC and Clang support evaluating __int128 as constexpr
		const auto product = static_cast<unsigned __int128>(lhs) * rhs;
		return static_cast<uint64_t>(product >> 64) & static_cast<uint64_t>(product);
#else
		auto [lower, upper] = mult64to128(lhs, rhs);
		return lower ^ upper;
#endif
	}

	// https://github.com/Cyan4973/xxHash/issues/496
	struct xxh32 {
		static constexpr uint32_t PRIME1 = 0x9E3779B1U;
		static constexpr uint32_t PRIME2 = 0x85EBCA77U;
		static constexpr uint32_t PRIME3 = 0xC2B2AE3DU;
		static constexpr uint32_t PRIME4 = 0x27D4EB2FU;
		static constexpr uint32_t PRIME5 = 0x165667B1U;

		template<typename T>
		static constexpr uint32_t hash(const T* input, const uint32_t len, const uint32_t seed) {
			return xxh32::finalize((len >= 16 ? xxh32::h16bytes(input, len, seed) : seed + PRIME5) + len, (input) + (len & ~0xF), len & 0xF);
		}

	private:
		// Normal stripe processing routine.
		static constexpr uint32_t round(uint32_t acc, const uint32_t input) {
			return std::rotl(acc + (input * PRIME2), 13) * PRIME1;
		}

		static constexpr uint32_t avalanche_step(const uint32_t h, const int rshift, const uint32_t prime) {
			return (h ^ (h >> rshift)) * prime;
		}

		// Mixes all bits to finalize the hash.
		static constexpr uint32_t avalanche(const uint32_t h) {
			return avalanche_step(avalanche_step(avalanche_step(h, 15, PRIME2), 13, PRIME3), 16, 1);
		}

		template<typename T>
		static constexpr uint32_t fetch32(const T* p, const uint32_t v) {
			return xxh32::round(v, internal::read32(p));
		}

		// Processes the last 0-15 bytes of p.
		template<typename T>
		static constexpr uint32_t finalize(const uint32_t h, const T* p, uint32_t len) {
			return
					(len >= 4)
						? xxh32::finalize(std::rotl(h + (internal::read32(p) * PRIME3), 17) * PRIME4, p + 4, len - 4)
						: (len > 0)
							? xxh32::finalize(std::rotl(h + (uint8_t(*p) * PRIME5), 11) * PRIME1, p + 1, len - 1)
							: avalanche(h);
		}

		template<typename T>
		static constexpr uint32_t h16bytes(const T* p, uint32_t len, const uint32_t v1, const uint32_t v2, const uint32_t v3, const uint32_t v4) {
			return
					(len >= 16)
						? xxh32::h16bytes(p + 16, len - 16, xxh32::fetch32(p, v1), xxh32::fetch32(p + 4, v2), xxh32::fetch32(p + 8, v3), xxh32::fetch32(p + 12, v4))
						: std::rotl(v1, 1) + std::rotl(v2, 7) + std::rotl(v3, 12) + std::rotl(v4, 18);
		}

		template<typename T>
		static constexpr uint32_t h16bytes(const T* p, uint32_t len, const uint32_t seed) {
			return xxh32::h16bytes(p, len, seed + PRIME1 + PRIME2, seed + PRIME2, seed, seed - PRIME1);
		}
	};

	// https://github.com/chys87/constexpr-xxh3
	namespace constexpr_xxh3
	{
		template<typename T>
		concept ByteType = (std::is_integral_v<T> && sizeof(T) == 1) || std::is_same_v<T, std::byte>;

		template<typename T>
		concept BytePtrType = requires(T ptr)
		{
			requires std::is_pointer_v<T>;
			requires ByteType<std::remove_cvref_t<decltype(*ptr)>>;
		};

		template<typename T>
		concept BytesType = requires(const T& bytes)
		{
			{ std::data(bytes) };
			requires BytePtrType<decltype(std::data(bytes))>;
			// -> std::convertible_to is not supported widely enough
			{ static_cast<size_t>(std::size(bytes)) };
		};

		constexpr uint32_t swap32(uint32_t x) noexcept {
			return ((x << 24) & 0xff000000) | ((x << 8) & 0x00ff0000) |
					((x >> 8) & 0x0000ff00) | ((x >> 24) & 0x000000ff);
		}

		constexpr uint64_t swap64(uint64_t x) noexcept {
			return ((x << 56) & 0xff00000000000000ULL) |
					((x << 40) & 0x00ff000000000000ULL) |
					((x << 24) & 0x0000ff0000000000ULL) |
					((x << 8) & 0x000000ff00000000ULL) |
					((x >> 8) & 0x00000000ff000000ULL) |
					((x >> 24) & 0x0000000000ff0000ULL) |
					((x >> 40) & 0x000000000000ff00ULL) |
					((x >> 56) & 0x00000000000000ffULL);
		}

		constexpr void writeLE64(uint8_t* dst, uint64_t v) noexcept {
			for (int i = 0; i < 8; ++i) dst[i] = uint8_t(v >> (i * 8));
		}

		inline constexpr uint64_t PRIME64_1 = 0x9E3779B185EBCA87ULL;
		inline constexpr uint64_t PRIME64_2 = 0xC2B2AE3D27D4EB4FULL;
		inline constexpr uint64_t PRIME64_3 = 0x165667B19E3779F9ULL;
		inline constexpr uint64_t PRIME64_4 = 0x85EBCA77C2B2AE63ULL;
		inline constexpr uint64_t PRIME64_5 = 0x27D4EB2F165667C5ULL;

		inline constexpr size_t SECRET_DEFAULT_SIZE = 192;
		inline constexpr size_t SECRET_SIZE_MIN = 136;

		inline constexpr uint8_t kSecret[SECRET_DEFAULT_SIZE]{
			0xb8, 0xfe, 0x6c, 0x39, 0x23, 0xa4, 0x4b, 0xbe, 0x7c, 0x01, 0x81, 0x2c,
			0xf7, 0x21, 0xad, 0x1c, 0xde, 0xd4, 0x6d, 0xe9, 0x83, 0x90, 0x97, 0xdb,
			0x72, 0x40, 0xa4, 0xa4, 0xb7, 0xb3, 0x67, 0x1f, 0xcb, 0x79, 0xe6, 0x4e,
			0xcc, 0xc0, 0xe5, 0x78, 0x82, 0x5a, 0xd0, 0x7d, 0xcc, 0xff, 0x72, 0x21,
			0xb8, 0x08, 0x46, 0x74, 0xf7, 0x43, 0x24, 0x8e, 0xe0, 0x35, 0x90, 0xe6,
			0x81, 0x3a, 0x26, 0x4c, 0x3c, 0x28, 0x52, 0xbb, 0x91, 0xc3, 0x00, 0xcb,
			0x88, 0xd0, 0x65, 0x8b, 0x1b, 0x53, 0x2e, 0xa3, 0x71, 0x64, 0x48, 0x97,
			0xa2, 0x0d, 0xf9, 0x4e, 0x38, 0x19, 0xef, 0x46, 0xa9, 0xde, 0xac, 0xd8,
			0xa8, 0xfa, 0x76, 0x3f, 0xe3, 0x9c, 0x34, 0x3f, 0xf9, 0xdc, 0xbb, 0xc7,
			0xc7, 0x0b, 0x4f, 0x1d, 0x8a, 0x51, 0xe0, 0x4b, 0xcd, 0xb4, 0x59, 0x31,
			0xc8, 0x9f, 0x7e, 0xc9, 0xd9, 0x78, 0x73, 0x64, 0xea, 0xc5, 0xac, 0x83,
			0x34, 0xd3, 0xeb, 0xc3, 0xc5, 0x81, 0xa0, 0xff, 0xfa, 0x13, 0x63, 0xeb,
			0x17, 0x0d, 0xdd, 0x51, 0xb7, 0xf0, 0xda, 0x49, 0xd3, 0x16, 0x55, 0x26,
			0x29, 0xd4, 0x68, 0x9e, 0x2b, 0x16, 0xbe, 0x58, 0x7d, 0x47, 0xa1, 0xfc,
			0x8f, 0xf8, 0xb8, 0xd1, 0x7a, 0xd0, 0x31, 0xce, 0x45, 0xcb, 0x3a, 0x8f,
			0x95, 0x16, 0x04, 0x28, 0xaf, 0xd7, 0xfb, 0xca, 0xbb, 0x4b, 0x40, 0x7e,
		};

		constexpr uint64_t XXH64_avalanche(uint64_t h) noexcept {
			h = (h ^ (h >> 33)) * PRIME64_2;
			h = (h ^ (h >> 29)) * PRIME64_3;
			return h ^ (h >> 32);
		}

		constexpr uint64_t XXH3_avalanche(uint64_t h) noexcept {
			h = (h ^ (h >> 37)) * 0x165667919E3779F9ULL;
			return h ^ (h >> 32);
		}

		constexpr uint64_t rrmxmx(uint64_t h, uint64_t len) noexcept {
			h ^= ((h << 49) | (h >> 15)) ^ ((h << 24) | (h >> 40));
			h *= 0x9FB21C651E98DF25ULL;
			h ^= (h >> 35) + len;
			h *= 0x9FB21C651E98DF25ULL;
			return h ^ (h >> 28);
		}

		template<typename T, typename S>
		constexpr uint64_t mix16B(const T* input, const S* secret, uint64_t seed) noexcept {
			return internal::mul128_fold64(
				internal::read64(input) ^ (internal::read64(secret) + seed),
				internal::read64(input + 8) ^ (internal::read64(secret + 8) - seed)
			);
		}

		inline constexpr size_t STRIPE_LEN = 64;
		inline constexpr size_t SECRET_CONSUME_RATE = 8;
		inline constexpr size_t ACC_NB = STRIPE_LEN / sizeof(uint64_t);

		template<typename T, typename S>
		constexpr void accumulate_512(uint64_t* acc, const T* input, const S* secret) noexcept {
			for (size_t i = 0; i < ACC_NB; i++) {
				uint64_t data_val = internal::read64(input + 8 * i);
				const uint64_t data_key = data_val ^ internal::read64(secret + i * 8);
				acc[i ^ 1] += data_val;
				acc[i] += static_cast<uint32_t>(data_key) * (data_key >> 32);
			}
		}

		template<typename T, typename S>
		constexpr uint64_t hashLong_64b_internal(const T* input, size_t len, const S* secret, size_t secretSize) noexcept {
			uint64_t acc[ACC_NB]{xxh32::PRIME3, PRIME64_1, PRIME64_2, PRIME64_3, PRIME64_4, xxh32::PRIME2, PRIME64_5, xxh32::PRIME1};
			const size_t nbStripesPerBlock = (secretSize - STRIPE_LEN) / SECRET_CONSUME_RATE;
			const size_t block_len = STRIPE_LEN * nbStripesPerBlock;
			const size_t nb_blocks = (len - 1) / block_len;

			for (size_t n = 0; n < nb_blocks; n++) {
				for (size_t i = 0; i < nbStripesPerBlock; i++)
					constexpr_xxh3::accumulate_512(acc, input + n * block_len + i * STRIPE_LEN, secret + i * SECRET_CONSUME_RATE);
				for (size_t i = 0; i < ACC_NB; i++)
					acc[i] = (acc[i] ^ (acc[i] >> 47) ^ internal::read64(secret + secretSize - STRIPE_LEN + 8 * i)) * xxh32::PRIME1;
			}

			const size_t nbStripes = ((len - 1) - (block_len * nb_blocks)) / STRIPE_LEN;
			for (size_t i = 0; i < nbStripes; i++)
				constexpr_xxh3::accumulate_512(acc, input + nb_blocks * block_len + i * STRIPE_LEN, secret + i * SECRET_CONSUME_RATE);
			constexpr_xxh3::accumulate_512(acc, input + len - STRIPE_LEN, secret + secretSize - STRIPE_LEN - 7);
			uint64_t result = len * PRIME64_1;
			for (size_t i = 0; i < 4; i++)
				result += internal::mul128_fold64(acc[2 * i] ^ internal::read64(secret + 11 + 16 * i), acc[2 * i + 1] ^ internal::read64(secret + 11 + 16 * i + 8));
			return XXH3_avalanche(result);
		}

		template<typename T, typename S, typename HashLong>
		constexpr uint64_t XXH3_64bits_internal(const T* input, size_t len, uint64_t seed, const S* secret, size_t secretLen, HashLong f_hashLong) noexcept {
			auto get = [](T value) { return static_cast<uint32_t>(static_cast<uint8_t>(value)); };

			if (len == 0) {
				return constexpr_xxh3::XXH64_avalanche(seed ^ (internal::read64(secret + 56) ^ internal::read64(secret + 64)));
			}
			if (len < 4) {
				const uint64_t keyed = ((get(input[0]) << 16) | (get(input[len >> 1]) << 24) | get(input[len - 1]) | (static_cast<uint32_t>(len) << 8)) ^
										((internal::read32(secret) ^ internal::read32(secret + 4)) + seed);
				return XXH64_avalanche(keyed);
			}
			if (len <= 8) {
				const uint64_t keyed =
						(internal::read32(input + len - 4) + (static_cast<uint64_t>(internal::read32(input)) << 32)) ^
						((internal::read64(secret + 8) ^ internal::read64(secret + 16)) -
						(seed ^ (static_cast<uint64_t>(swap32(seed)) << 32)));
				return rrmxmx(keyed, len);
			}
			if (len <= 16) {
				const uint64_t input_lo =
						internal::read64(input) ^
						((internal::read64(secret + 24) ^ internal::read64(secret + 32)) + seed);
				const uint64_t input_hi =
						internal::read64(input + len - 8) ^
						((internal::read64(secret + 40) ^ internal::read64(secret + 48)) - seed);
				const uint64_t acc = len + swap64(input_lo) + input_hi + mul128_fold64(input_lo, input_hi);
				return XXH3_avalanche(acc);
			}
			if (len <= 128) {
				uint64_t acc = len * PRIME64_1;
				size_t secret_off = 0;
				for (size_t i = 0, j = len; j > i; i += 16, j -= 16) {
					acc += mix16B(input + i, secret + secret_off, seed);
					acc += mix16B(input + j - 16, secret + secret_off + 16, seed);
					secret_off += 32;
				}
				return XXH3_avalanche(acc);
			}
			if (len <= 240) {
				uint64_t acc = len * PRIME64_1;
				for (size_t i = 0; i < 128; i += 16)
					acc += constexpr_xxh3::mix16B(input + i, secret + i, seed);
				acc = XXH3_avalanche(acc);
				for (size_t i = 128; i < len / 16 * 16; i += 16)
					acc += constexpr_xxh3::mix16B(input + i, secret + (i - 128) + 3, seed);
				acc += constexpr_xxh3::mix16B(input + len - 16, secret + SECRET_SIZE_MIN - 17, seed);
				return XXH3_avalanche(acc);
			}
			return f_hashLong(input, len, seed, secret, secretLen);
		}

		template<BytesType Bytes>
		constexpr size_t bytes_size(const Bytes& bytes) noexcept {
			return std::size(bytes);
		}

		template<ByteType T, size_t N>
		constexpr size_t bytes_size(T (&)[N]) noexcept {
			return (N ? N - 1 : 0);
		}

		/// Basic interfaces

		template<ByteType T>
		consteval uint64_t XXH3_64bits_const(const T* input, size_t len) noexcept {
			return constexpr_xxh3::XXH3_64bits_internal(
				input, len, 0, kSecret, sizeof(kSecret),
				[](const T* input, size_t len, uint64_t, const void*,
					size_t) constexpr noexcept {
					return constexpr_xxh3::hashLong_64b_internal(input, len, kSecret, sizeof(kSecret));
				});
		}

		template<ByteType T, ByteType S>
		consteval uint64_t XXH3_64bits_withSecret_const(const T* input, size_t len, const S* secret, size_t secretSize) noexcept {
			return constexpr_xxh3::XXH3_64bits_internal(
				input, len, 0, secret, secretSize,
				[](const T* input, size_t len, uint64_t, const S* secret,
					size_t secretLen) constexpr noexcept {
					return constexpr_xxh3::hashLong_64b_internal(input, len, secret, secretLen);
				});
		}

		template<ByteType T>
		consteval uint64_t XXH3_64bits_withSeed_const(const T* input, size_t len, uint64_t seed) noexcept {
			if (seed == 0) return constexpr_xxh3::XXH3_64bits_const(input, len);
			return constexpr_xxh3::XXH3_64bits_internal(
				input, len, seed, kSecret, sizeof(kSecret),
				[](const T* input, size_t len, uint64_t seed, const void*,
					size_t) constexpr noexcept {
					uint8_t secret[SECRET_DEFAULT_SIZE];
					for (size_t i = 0; i < SECRET_DEFAULT_SIZE; i += 16) {
						constexpr_xxh3::writeLE64(secret + i, internal::read64(kSecret + i) + seed);
						constexpr_xxh3::writeLE64(secret + i + 8, internal::read64(kSecret + i + 8) - seed);
					}
					return constexpr_xxh3::hashLong_64b_internal(input, len, secret, sizeof(secret));
				});
		}

		/// Convenient interfaces

		template<BytesType Bytes>
		consteval uint64_t XXH3_64bits_const(const Bytes& input) noexcept {
			return constexpr_xxh3::XXH3_64bits_const(std::data(input), constexpr_xxh3::bytes_size(input));
		}

		template<BytesType Bytes, BytesType Secret>
		consteval uint64_t XXH3_64bits_withSecret_const(const Bytes& input, const Secret& secret) noexcept {
			return constexpr_xxh3::XXH3_64bits_withSecret_const(std::data(input), constexpr_xxh3::bytes_size(input), std::data(secret), constexpr_xxh3::bytes_size(secret));
		}

		template<BytesType Bytes>
		consteval uint64_t XXH3_64bits_withSeed_const(const Bytes& input, uint64_t seed) noexcept {
			return constexpr_xxh3::XXH3_64bits_withSeed_const(std::data(input), constexpr_xxh3::bytes_size(input), seed);
		}
	}
}

namespace auxiliary
{
	class XXHash {
	public:
		static constexpr uint32_t default_seed = 1610612741;

		template<typename T>
		[[nodiscard]] static constexpr uint32_t xxhash32(const T* input, size_t length, uint32_t seed = default_seed) {
			if constexpr (internal::constexpr_xxh3::ByteType<T>) {
				if (std::is_constant_evaluated()) {
					return internal::xxh32::hash(input, length, seed);
				}
				return XXHash::_xxhash32(input, length, seed);
			} else {
				return XXHash::_xxhash32(input, length * sizeof(T), seed);
			}
		}

		template<internal::constexpr_xxh3::BytesType Bytes>
		[[nodiscard]] static constexpr uint32_t xxhash32(const Bytes& input, uint32_t seed = default_seed) {
			return XXHash::xxhash32(std::data(input), internal::constexpr_xxh3::bytes_size(input), seed);
		}

		template<typename T>
		[[nodiscard]] static constexpr uint64_t xxhash64(const T* input, size_t length) {
			if constexpr (internal::constexpr_xxh3::ByteType<T>) {
				if (std::is_constant_evaluated()) {
					return XXHash::_XXH3_64bits_const(input, length);
				}
				return XXHash::_xxhash64(input, length);
			} else {
				return XXHash::_xxhash64(input, length * sizeof(T));
			}
		}

		template<typename T>
		[[nodiscard]] static constexpr uint64_t xxhash64(const T* input, size_t length, uint64_t seed) {
			if constexpr (internal::constexpr_xxh3::ByteType<T>) {
				if (std::is_constant_evaluated()) {
					return XXHash::_XXH3_64bits_withSeed_const(input, length, seed);
				}
				return XXHash::_xxhash64(input, length, seed);
			} else {
				return XXHash::_xxhash64(input, length * sizeof(T), seed);
			}
		}

		template<internal::constexpr_xxh3::BytesType Bytes>
		[[nodiscard]] static constexpr uint64_t xxhash64(const Bytes& input) {
			return XXHash::xxhash64(std::data(input), internal::constexpr_xxh3::bytes_size(input));
		}

		template<internal::constexpr_xxh3::BytesType Bytes>
		[[nodiscard]] static constexpr uint64_t xxhash64(const Bytes& input, uint64_t seed) {
			return XXHash::xxhash64(std::data(input), internal::constexpr_xxh3::bytes_size(input), seed);
		}

		template<typename T>
		[[nodiscard]] static constexpr size_t xxhash(const T* input, size_t length) {
			if constexpr (sizeof(size_t) == 4) {
				return XXHash::xxhash32(input, length);
			} else {
				return XXHash::xxhash64(input, length);
			}
		}

		template<typename T>
		[[nodiscard]] static constexpr size_t xxhash(const T* input, size_t length, size_t seed) {
			if constexpr (sizeof(size_t) == 4) {
				return XXHash::xxhash32(input, length, seed);
			} else {
				return XXHash::xxhash64(input, length, seed);
			}
		}

		template<internal::constexpr_xxh3::BytesType Bytes>
		[[nodiscard]] static constexpr size_t xxhash(const Bytes& input) {
			return XXHash::xxhash(std::data(input), internal::constexpr_xxh3::bytes_size(input));
		}

		template<internal::constexpr_xxh3::BytesType Bytes>
		[[nodiscard]] static constexpr size_t xxhash(const Bytes& input, size_t seed) {
			return XXHash::xxhash(std::data(input), internal::constexpr_xxh3::bytes_size(input), seed);
		}

	private:
		AUXILIARY_API static uint32_t _xxhash32(const void* input, size_t length, uint32_t seed);
		AUXILIARY_API static uint64_t _xxhash64(const void* input, size_t length);
		AUXILIARY_API static uint64_t _xxhash64(const void* input, size_t length, uint64_t seed);

		template<internal::constexpr_xxh3::ByteType T>
		static constexpr uint64_t _XXH3_64bits_const(const T* input, size_t len) noexcept {
			auto hashlong = [](const T* input, size_t len, uint64_t, const void*, size_t) constexpr noexcept {
				return internal::constexpr_xxh3::hashLong_64b_internal(input, len, internal::constexpr_xxh3::kSecret, sizeof(internal::constexpr_xxh3::kSecret));
			};

			return internal::constexpr_xxh3::XXH3_64bits_internal(input, len, 0, internal::constexpr_xxh3::kSecret, sizeof(internal::constexpr_xxh3::kSecret), hashlong);
		}

		template<internal::constexpr_xxh3::ByteType T>
		static constexpr uint64_t _XXH3_64bits_withSeed_const(const T* input, size_t len, uint64_t seed) noexcept {
			if (seed == 0) return XXHash::_XXH3_64bits_const(input, len);

			auto hashlong = [](const T* input, size_t len, uint64_t seed, const void*, size_t) constexpr noexcept {
				uint8_t secret[internal::constexpr_xxh3::SECRET_DEFAULT_SIZE];
				for (size_t i = 0; i < internal::constexpr_xxh3::SECRET_DEFAULT_SIZE; i += 16) {
					internal::constexpr_xxh3::writeLE64(secret + i, internal::read64(internal::constexpr_xxh3::kSecret + i) + seed);
					internal::constexpr_xxh3::writeLE64(secret + i + 8, internal::read64(internal::constexpr_xxh3::kSecret + i + 8) - seed);
				}
				return internal::constexpr_xxh3::hashLong_64b_internal(input, len, secret, sizeof(secret));
			};

			return internal::constexpr_xxh3::XXH3_64bits_internal(input, len, seed, internal::constexpr_xxh3::kSecret, sizeof(internal::constexpr_xxh3::kSecret), hashlong);
		}
	};

	class Fnv1aHash {
	public:
		/*! @brief If size of the pointer type is 8 bits, this function is constexpr. */
		template<typename T>
		[[nodiscard]] constexpr uint32_t fnv1a_hash32(const T* input, const size_t length) noexcept {
			if constexpr (internal::constexpr_xxh3::ByteType<T>) {
				auto hash = fnv1a_32_params::offset;
				for (auto i = 0; i < length; i++) {
					hash = (hash ^ static_cast<size_t>(input[i])) * fnv1a_32_params::prime;
				}
				return hash;
			} else {
				return fnv1a_hash32(reinterpret_cast<const unsigned char*>(input), length * sizeof(T));
			}
		}

		/*! @brief If size of the pointer type is 8 bits, this function is constexpr. */
		template<typename T>
		[[nodiscard]] constexpr uint64_t fnv1a_hash64(const T* input, const size_t length) noexcept {
			if constexpr (internal::constexpr_xxh3::ByteType<T>) {
				auto hash = fnv1a_64_params::offset;
				for (auto i = 0; i < length; i++) {
					hash = (hash ^ static_cast<size_t>(input[i])) * fnv1a_64_params::prime;
				}
				return hash;
			} else {
				return fnv1a_hash64(reinterpret_cast<const unsigned char*>(input), length * sizeof(T));
			}
		}

		template<internal::constexpr_xxh3::BytesType Bytes>
		[[nodiscard]] constexpr uint32_t fnv1a_hash32(const Bytes& input) noexcept {
			return Fnv1aHash::fnv1a_hash32(std::data(input), internal::constexpr_xxh3::bytes_size(input));
		}

		template<internal::constexpr_xxh3::BytesType Bytes>
		[[nodiscard]] constexpr uint64_t fnv1a_hash64(const Bytes& input) noexcept {
			return Fnv1aHash::fnv1a_hash64(std::data(input), internal::constexpr_xxh3::bytes_size(input));
		}

		template<typename T>
		[[nodiscard]] constexpr size_t fnv1a_hash(const T* input, const size_t length) noexcept {
			if constexpr (sizeof(size_t) == 4) {
				return Fnv1aHash::fnv1a_hash32(input, length);
			} else {
				return Fnv1aHash::fnv1a_hash64(input, length);
			}
		}

		template<internal::constexpr_xxh3::BytesType Bytes>
		[[nodiscard]] constexpr size_t fnv1a_hash(const Bytes& input) noexcept {
			return Fnv1aHash::fnv1a_hash(std::data(input), internal::constexpr_xxh3::bytes_size(input));
		}

	private:
		struct fnv1a_32_params {
			using value_type = uint32_t;
			static constexpr value_type offset = 2166136261u;
			static constexpr value_type prime = 16777619u;
		};

		struct fnv1a_64_params {
			using value_type = uint64_t;
			static constexpr value_type offset = 14695981039346656037ull;
			static constexpr value_type prime = 1099511628211ull;
		};
	};

	template<typename... Ts>
	[[nodiscard]] constexpr size_t hash_combine(const size_t seed, const size_t combine, Ts&&... rest) noexcept {
		const size_t res = seed ^ (combine + 0x9e3779b9 + (seed << 6) + (seed >> 2));
		if constexpr (sizeof...(Ts) == 0) {
			return res;
		} else {
			return auxiliary::hash_combine(res, std::forward<Ts>(rest)...);
		}
	}

	template<typename T>
	struct Hash : std::hash<T> {};
}
