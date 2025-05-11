#pragma once

#include "config/key_words.h"
#include "type_traits.hpp"

namespace auxiliary::internal
{
	template<typename T>
	concept is_ebco_eligible_v = std::is_empty_v<T> && !std::is_final_v<T>;

	template<typename T, size_t>
	class AUXILIARY_EBCO compressed_pair_element {
		using reference = T&;
		using const_reference = const T&;

	public:
		template<typename... Args>
		explicit constexpr compressed_pair_element(Args&&... args)
			noexcept(std::is_nothrow_constructible_v<T, Args...>)
			requires(std::is_constructible_v<T, Args...>)
			: value{std::forward<Args>(args)...} {}

		[[nodiscard]] constexpr reference get() noexcept { return value; }
		[[nodiscard]] constexpr const_reference get() const noexcept { return value; }

	private:
		AUXILIARY_NO_UNIQUE_ADDRESS T value;
	};

	template<is_ebco_eligible_v T, size_t Tag>
	class AUXILIARY_EBCO compressed_pair_element<T, Tag> : public T {
		using reference = T&;
		using const_reference = const T&;

	public:
		template<typename... Args>
		explicit constexpr compressed_pair_element(Args&&... args)
			noexcept(std::is_nothrow_constructible_v<T, Args...>)
			requires(std::is_constructible_v<T, Args...>)
			: T{std::forward<Args>(args)...} {}

		[[nodiscard]] constexpr reference get() noexcept { return *this; }
		[[nodiscard]] constexpr const_reference get() const noexcept { return *this; }
	};
}

namespace auxiliary
{
	template<typename First, typename Second>
	class AUXILIARY_EBCO compressed_pair
			: internal::compressed_pair_element<First, 0u>,
			internal::compressed_pair_element<Second, 1u> {
		using first_base = internal::compressed_pair_element<First, 0u>;
		using second_base = internal::compressed_pair_element<Second, 1u>;

	public:
		constexpr compressed_pair()
			noexcept(std::is_nothrow_default_constructible_v<first_base> && std::is_nothrow_default_constructible_v<second_base>)
			requires(std::is_default_constructible_v<first_base> && std::is_default_constructible_v<second_base>) = default;

		constexpr compressed_pair(compressed_pair&&) noexcept = default;
		constexpr compressed_pair(const compressed_pair&) noexcept = default;
		constexpr compressed_pair& operator=(compressed_pair&&) noexcept = default;
		constexpr compressed_pair& operator=(const compressed_pair&) noexcept = default;

		template<typename FArg, typename SArg>
		constexpr compressed_pair(FArg&& farg, SArg&& sarg)
			noexcept(std::is_nothrow_constructible_v<first_base, FArg> && std::is_nothrow_constructible_v<second_base, SArg>)
			requires(std::is_constructible_v<first_base, FArg> && std::is_constructible_v<second_base, SArg>)
			: first_base{std::forward<FArg>(farg)}, second_base{std::forward<SArg>(sarg)} {}

		[[nodiscard]] constexpr First& first() noexcept { return static_cast<first_base&>(*this).get(); }
		[[nodiscard]] constexpr const First& first() const noexcept { return static_cast<const first_base&>(*this).get(); }
		[[nodiscard]] constexpr Second& second() noexcept { return static_cast<second_base&>(*this).get(); }
		[[nodiscard]] constexpr const Second& second() const noexcept { return static_cast<const second_base&>(*this).get(); }

		constexpr void swap(compressed_pair& other) noexcept {
			std::swap(first(), other.first());
			std::swap(second(), other.second());
		}

		template<size_t Index>
		[[nodiscard]] constexpr decltype(auto) get() noexcept {
			if constexpr (Index == 0u) {
				return first();
			} else {
				static_assert(Index < 2, "subscript out of range");
				return second();
			}
		}

		template<size_t Index>
		[[nodiscard]] constexpr decltype(auto) get() const noexcept {
			if constexpr (Index == 0u) {
				return first();
			} else {
				static_assert(Index < 2, "subscript out of range");
				return second();
			}
		}
	};

	template<typename First, typename Second>
	compressed_pair(First&&, Second&&) -> compressed_pair<std::decay_t<First>, std::decay_t<Second>>;
}

namespace std
{
	template<typename First, typename Second>
	struct tuple_size<auxiliary::compressed_pair<First, Second>> : integral_constant<size_t, 2u> {};

	template<size_t Index, typename First, typename Second>
	struct tuple_element<Index, auxiliary::compressed_pair<First, Second>> : conditional<Index == 0u, First, Second> {
		static_assert(Index < 2u, "Index out of bounds");
	};
}
