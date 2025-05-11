#pragma once

#include <iterator>
#include <type_traits>

namespace auxiliary
{
	// =========================================================

	template<typename T, template <typename...> typename Template>
	inline constexpr bool is_specialization_v = false;

	template<template <typename...> typename Template, typename... Ts>
	inline constexpr bool is_specialization_v<Template<Ts...>, Template> = true;

	template<typename T, template <typename...> typename Template>
	concept Specialization = is_specialization_v<T, Template>;

	// =========================================================

	template<typename T>
	concept is_ebco_eligible_v = std::is_empty_v<T> && !std::is_final_v<T>;

	template<typename T, typename... Ts>
	inline constexpr bool is_any_of_v = std::disjunction_v<std::is_same<T, Ts>...>;

	template<typename T>
	concept is_transparent_v = requires { typename T::is_transparent; };

	template<typename T>
	concept is_complete_v = (sizeof(T) != 0);

	// =========================================================

	template<typename, typename = void>
	struct has_iterator_category : std::false_type {};

	template<typename T>
	struct has_iterator_category<T, std::void_t<typename std::iterator_traits<T>::iterator_category>> : std::true_type {};

	template<typename T, typename = void>
	struct is_iterator : std::false_type {};

	template<typename T>
	struct is_iterator<T, std::enable_if_t<!std::is_void_v<std::remove_cv_t<std::remove_pointer_t<T>>>>> : has_iterator_category<T> {};

	template<typename T>
	concept is_iterator_v = is_iterator<T>::value;
}
