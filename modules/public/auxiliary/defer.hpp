#pragma once

#include "config/string.h"
#include <type_traits>

namespace auxiliary
{
	template<typename Fn>
	class defer_raii {
	public:
		defer_raii(defer_raii&&) = delete;
		defer_raii(const defer_raii&) = delete;
		defer_raii& operator=(defer_raii&&) = delete;
		defer_raii& operator=(const defer_raii&) = delete;

		template<typename FF>
		explicit defer_raii(FF&& ff): f(std::forward<FF>(ff)) {}

		~defer_raii() { f(); }

	private:
		Fn f;
	};

	template<typename F>
	defer_raii(F&& f) -> defer_raii<F>;
}

#define AUXILIARY_DEFER(func) auxiliary::defer_raii AUXILIARY_UNIQUE_VARNAME(defer_object) { [&]{func;} }
