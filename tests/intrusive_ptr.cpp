#include <doctest/doctest.h>
#include <auxiliary/intrusive_ptr.hpp>

struct SPTRIntrusiveTests {
	template<typename T, typename... Args>
	static auto make_intrusive(Args&&... args) {
		return auxiliary::intrusive_ptr{new T{std::forward<Args>(args)...}};
	}
};

struct TestObject : public auxiliary::intrusive_base {
	enum Status {
		Uninitialized,
		Created,
		Hosted,
		Destroyed
	};

	explicit TestObject(Status& code)
		: code(code) {
		code = Status::Created;
	}

	~TestObject() override {
		code = Status::Destroyed;
	}

	void add_refcount() noexcept override {
		code = Status::Hosted;
		rc++;
	}

	int32_t release() noexcept override {
		rc--;
		return rc;
	}

	int32_t use_count() const {
		return rc;
	}

	deleter_type custom_deleter() const noexcept override {
		return [](intrusive_base* ptr) { delete ptr; };
	}

	Status& code;
	std::atomic_int32_t rc = 0;
};

struct TestSon : public TestObject {
	TestSon(Status& code): TestObject(code) {}
};

TEST_CASE_FIXTURE(SPTRIntrusiveTests, "CopyIntrusive") {
	TestObject::Status status = TestObject::Status::Uninitialized;
	[&] {
		auto mTestObject = make_intrusive<TestObject>(status);
		CHECK_EQ(mTestObject->rc, 1);
		auxiliary::intrusive_ptr<TestObject> mCopy(mTestObject);
		CHECK_EQ(mTestObject->rc, 2);
		auto mCopy1 = mTestObject;
		CHECK_EQ(mTestObject->rc, 3);
	}();
	CHECK_EQ(status, TestObject::Status::Destroyed);
}

TEST_CASE_FIXTURE(SPTRIntrusiveTests, "CopyIntrusive2") {
	TestObject::Status status = TestObject::Status::Uninitialized;
	[&] {
		auto mTestObject = make_intrusive<TestObject>(status);
		CHECK_EQ(mTestObject->rc, 1);
		auxiliary::intrusive_ptr<TestObject> mCopy(mTestObject);
		CHECK_EQ(mTestObject->rc, 2);
		auto mCopy1 = mTestObject;
		CHECK_EQ(mTestObject->rc, 3);
	}();
	CHECK_EQ(status, TestObject::Status::Destroyed);
}

TEST_CASE_FIXTURE(SPTRIntrusiveTests, "SwapIntrusive") {
	auto one_status = TestObject::Status::Uninitialized;
	auto another_status = TestObject::Status::Uninitialized;
	auto mOneObject = make_intrusive<TestObject>(one_status);
	[&] {
		auto mAnotherObject = make_intrusive<TestObject>(another_status);
		mOneObject = mAnotherObject;
		CHECK_EQ(one_status, TestObject::Status::Destroyed);
	}();
	CHECK_EQ(another_status, TestObject::Status::Hosted);
}

TEST_CASE_FIXTURE(SPTRIntrusiveTests, "SwapIntrusive2") {
	auto one_status = TestObject::Status::Uninitialized;
	auto another_status = TestObject::Status::Uninitialized;
	auto mOneObject = make_intrusive<TestObject>(one_status);
	[&] {
		auto mAnotherObject = make_intrusive<TestObject>(another_status);
		mOneObject = mAnotherObject;
		CHECK_EQ(one_status, TestObject::Status::Destroyed);
	}();
	CHECK_EQ(another_status, TestObject::Status::Hosted);
}

TEST_CASE_FIXTURE(SPTRIntrusiveTests, "MoveIntrusive") {
	auto one_status = TestObject::Status::Uninitialized;
	auto another_status = TestObject::Status::Uninitialized;
	auto rT1 = make_intrusive<TestObject>(one_status);
	auto rT2 = make_intrusive<TestSon>(another_status);

	CHECK_EQ(one_status, TestObject::Status::Hosted);
	CHECK_EQ(another_status, TestObject::Status::Hosted);

	rT1 = std::move(rT2);

	CHECK_EQ(one_status, TestObject::Status::Destroyed);
}

TEST_CASE_FIXTURE(SPTRIntrusiveTests, "CastIntrusive") {
	TestObject::Status status = TestObject::Status::Uninitialized;
	auto pC = make_intrusive<TestSon>(status);
	auxiliary::intrusive_ptr<TestObject> pP(pC);
	// auxiliary::intrusive_ptr<TestSon> pCC(auxiliary::static_pointer_cast<TestSon>(pP));
	// CHECK_EQ(pCC->use_count(), 3);
	//
	// auxiliary::intrusive_ptr<TestObject> pCC2(pCC);
	// CHECK_EQ(pCC->use_count(), 4);
	//
	// auxiliary::intrusive_ptr<TestObject> pCC3(std::move(pCC));
	// CHECK_EQ(pCC3->use_count(), 4);
}

TEST_CASE_FIXTURE(SPTRIntrusiveTests, "CastIntrusive2") {
	TestObject::Status status = TestObject::Status::Uninitialized;
	auto pC = make_intrusive<TestSon>(status);
	auxiliary::intrusive_ptr<TestObject> pP(pC);
	// auxiliary::intrusive_ptr<TestSon> pCC(auxiliary::static_pointer_cast<TestSon>(pP));
	// CHECK_EQ(pCC->use_count(), 3);
	//
	// auxiliary::intrusive_ptr<TestObject> pCC2(pCC);
	// CHECK_EQ(pCC->use_count(), 4);
	//
	// auxiliary::intrusive_ptr<TestObject> pCC3(std::move(pCC));
	// CHECK_EQ(pCC3->use_count(), 4);
}

// TEST_CASE_FIXTURE(SPTRIntrusiveTests, "VoidPtrCastIntrusive") {
// 	TestObject::Status status = TestObject::Status::Uninitialized;
// 	[&] {
// 		auxiliary::intrusive_ptr<void> pVC;
// 		[&] {
// 			auto pC = make_intrusive<TestSon>(status);
// 			pVC = pC;
// 			CHECK_EQ(pC->use_count(), 2);
// 		}();
// 		CHECK_EQ(status, TestObject::Status::Hosted);
// 	}();
// 	CHECK_EQ(status, TestObject::Status::Destroyed);
// }
