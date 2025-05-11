#include <doctest/doctest.h>
#include <auxiliary/type_traits.hpp>

#include <vector>

TEST_CASE("iterator") {
	using namespace auxiliary;

	CHECK_FALSE(is_iterator_v<void>);
	CHECK_FALSE(is_iterator_v<int>);

	CHECK_FALSE(is_iterator_v<void *>);
	CHECK(is_iterator_v<int *>);

	CHECK(is_iterator_v<std::vector<int>::iterator>);
	CHECK(is_iterator_v<std::vector<int>::const_iterator>);
	CHECK(is_iterator_v<std::vector<int>::reverse_iterator>);
}
