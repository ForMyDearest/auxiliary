#pragma once

#include <optional>

namespace auxiliary
{
	template<typename T>
	class lazy_t {
	public:
		operator T() = delete;
		constexpr lazy_t() = default;

		constexpr lazy_t(lazy_t&&) = delete;
		constexpr lazy_t(const lazy_t&) = delete;
		constexpr lazy_t& operator=(lazy_t&&) = delete;
		constexpr lazy_t& operator=(const lazy_t&) = delete;

		explicit operator bool() const { return mData.has_value(); }
		[[nodiscard]] bool is_initialized() const { return mData.has_value(); }

		template<typename... Args> requires(std::is_constructible_v<T, Args...>)
		void reset(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
			mData.emplace(std::forward<Args>(args)...);
		}

		template<typename... Args> requires(std::is_constructible_v<T, Args...>)
		T& get(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
			if (!mData.has_value()) {
				mData.emplace(std::forward<Args>(args)...);
			}
			return mData.value();
		}

		template<typename... Args> requires(std::is_constructible_v<T, Args...>)
		const T& get(Args&&... args) const noexcept(std::is_nothrow_constructible_v<T, Args...>) {
			if (!mData.has_value()) {
				mData.emplace(std::forward<Args>(args)...);
			}
			return mData.value();
		}

	private:
		mutable std::optional<T> mData;
	};
}
