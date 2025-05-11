#include <doctest/doctest.h>
#include <auxiliary/compressed_pair.hpp>

namespace test
{
	struct empty {};

	struct other_empty {};

	struct non_default_constructible {
		non_default_constructible() = delete;

		non_default_constructible(int v): value{v} {}

		int value;
	};
}

struct CompressedPair {};


TEST_CASE_FIXTURE(CompressedPair, "Size") {
	struct local {
		int value;
		test::empty empty;
	};

	CHECK_EQ(sizeof(auxiliary::compressed_pair<int, int>), sizeof(int) * 2u);
	CHECK_EQ(sizeof(auxiliary::compressed_pair<test::empty, int>), sizeof(int));
	CHECK_EQ(sizeof(auxiliary::compressed_pair<int, test::empty>), sizeof(int));
	CHECK_LT(sizeof(auxiliary::compressed_pair<int, test::empty>), sizeof(local));
	CHECK_LT(sizeof(auxiliary::compressed_pair<int, test::empty>), sizeof(std::pair<int, test::empty>));
}

TEST_CASE_FIXTURE(CompressedPair, "ConstructCopyMove") {
	static_assert(!std::is_default_constructible_v<auxiliary::compressed_pair<test::non_default_constructible, test::empty>>, "Default constructible type not allowed");
	static_assert(std::is_default_constructible_v<auxiliary::compressed_pair<std::unique_ptr<int>, test::empty>>, "Default constructible type required");

	static_assert(std::is_copy_constructible_v<auxiliary::compressed_pair<test::non_default_constructible, test::empty>>, "Copy constructible type required");
	static_assert(!std::is_copy_constructible_v<auxiliary::compressed_pair<std::unique_ptr<int>, test::empty>>, "Copy constructible type not allowed");
	static_assert(std::is_copy_assignable_v<auxiliary::compressed_pair<test::non_default_constructible, test::empty>>, "Copy assignable type required");
	static_assert(!std::is_copy_assignable_v<auxiliary::compressed_pair<std::unique_ptr<int>, test::empty>>, "Copy assignable type not allowed");

	static_assert(std::is_move_constructible_v<auxiliary::compressed_pair<std::unique_ptr<int>, test::empty>>, "Move constructible type required");
	static_assert(std::is_move_assignable_v<auxiliary::compressed_pair<std::unique_ptr<int>, test::empty>>, "Move assignable type required");

	auxiliary::compressed_pair copyable{test::non_default_constructible{2}, test::empty{}};
	auto by_copy{copyable};

	CHECK_EQ(by_copy.first().value, 2);

	by_copy.first().value = 3;
	copyable = by_copy;

	CHECK_EQ(copyable.first().value, 3);

	auxiliary::compressed_pair<test::empty, std::unique_ptr<int>> movable{test::empty{}, std::make_unique<int>(1)};
	auto by_move{std::move(movable)};

	CHECK(by_move.second());
	CHECK_EQ(*by_move.second(), 1);

	*by_move.second() = 3;
	movable = std::move(by_move);

	CHECK(movable.second());
	CHECK_EQ(*movable.second(), 3);
}

TEST_CASE_FIXTURE(CompressedPair, "DeductionGuide") {
	const int value = 2;
	const test::empty empty{};
	auxiliary::compressed_pair pair{value, 3};


	CHECK(std::is_same_v<decltype(auxiliary::compressed_pair{test::empty{}, empty}), auxiliary::compressed_pair<test::empty, test::empty>>);
	CHECK(std::is_same_v<decltype(pair), auxiliary::compressed_pair<int, int>>);

	CHECK_EQ(pair.first(), 2);
	CHECK_EQ(pair.second(), 3);
}

TEST_CASE_FIXTURE(CompressedPair, "Getters") {
	auxiliary::compressed_pair pair{3, test::empty{}};
	const auto& cpair = pair;

	CHECK(std::is_same_v<decltype(pair.first()), int &>);
	CHECK(std::is_same_v<decltype(pair.second()), test::empty &>);

	CHECK(std::is_same_v<decltype(cpair.first()), const int &>);
	CHECK(std::is_same_v<decltype(cpair.second()), const test::empty &>);

	CHECK_EQ(pair.first(), cpair.first());
	CHECK_EQ(&pair.second(), &cpair.second());
}

TEST_CASE_FIXTURE(CompressedPair, "Swap") {
	auxiliary::compressed_pair pair{1, 2};
	auxiliary::compressed_pair other{3, 4};

	std::swap(pair, other);

	CHECK_EQ(pair.first(), 3);
	CHECK_EQ(pair.second(), 4);
	CHECK_EQ(other.first(), 1);
	CHECK_EQ(other.second(), 2);

	pair.swap(other);

	CHECK_EQ(pair.first(), 1);
	CHECK_EQ(pair.second(), 2);
	CHECK_EQ(other.first(), 3);
	CHECK_EQ(other.second(), 4);
}

TEST_CASE_FIXTURE(CompressedPair, "Get") {
	auxiliary::compressed_pair pair{1, 2};

	CHECK_EQ(pair.get<0>(), 1);
	CHECK_EQ(pair.get<1>(), 2);

	CHECK_EQ(&pair.get<0>(), &pair.first());
	CHECK_EQ(&pair.get<1>(), &pair.second());

	auto&& [first, second] = pair;

	CHECK_EQ(first, 1);
	CHECK_EQ(second, 2);

	first = 3;
	second = 4;

	CHECK_EQ(pair.first(), 3);
	CHECK_EQ(pair.second(), 4);

	// NOLINTNEXTLINE(readability-qualified-auto)
	auto& [cfirst, csecond] = std::as_const(pair);

	CHECK_EQ(cfirst, 3);
	CHECK_EQ(csecond, 4);

	CHECK(std::is_same_v<decltype(cfirst), const int>);
	CHECK(std::is_same_v<decltype(csecond), const int>);

	auto [tfirst, tsecond] = auxiliary::compressed_pair{32, 64};

	CHECK_EQ(tfirst, 32);
	CHECK_EQ(tsecond, 64);

	CHECK(std::is_same_v<decltype(cfirst), const int>);
	CHECK(std::is_same_v<decltype(csecond), const int>);
}
