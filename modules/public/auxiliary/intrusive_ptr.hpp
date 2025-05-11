#pragma once

#include "config/key_words.h"

#include <type_traits>

namespace auxiliary
{
	struct AUXILIARY_NOVTABLE intrusive_base {
		using deleter_type = void(*)(intrusive_base*);

		virtual ~intrusive_base() noexcept = default;
		virtual void add_refcount() noexcept = 0;
		virtual int32_t release() noexcept = 0;
		[[nodiscard]] virtual deleter_type custom_deleter() const noexcept = 0;
	};

	template<typename T>
	class intrusive_ptr {
	public:
		static_assert(std::is_base_of_v<intrusive_base, T>);
		static_assert(!std::is_array_v<T>);

		//== ctor & dtor ===========================================

		intrusive_ptr() noexcept = default;

		explicit intrusive_ptr(T* lp) noexcept: ptr(lp) {
			if (ptr) {
				reinterpret_cast<intrusive_base*>(ptr)->add_refcount();
			}
		}

		intrusive_ptr(const intrusive_ptr& lp) noexcept: ptr(lp.ptr) {
			reinterpret_cast<intrusive_base*>(ptr)->add_refcount();
		}

		template<typename U> requires(std::is_convertible_v<U*, T*>)
		explicit intrusive_ptr(const intrusive_ptr<U>& lp) noexcept: ptr(lp.ptr) {
			reinterpret_cast<intrusive_base*>(ptr)->add_refcount();
		}

		intrusive_ptr(intrusive_ptr&& lp) noexcept: ptr(lp.ptr) {
			lp.ptr = nullptr;
		}

		template<typename U> requires(std::is_convertible_v<U*, T*>)
		explicit intrusive_ptr(intrusive_ptr<U>&& lp) noexcept: ptr(lp.ptr) {
			lp.ptr = nullptr;
		}

		~intrusive_ptr() noexcept {
			release();
		}

		//== assign ================================================

		intrusive_ptr& operator=(T* lp) noexcept {
			if (ptr != lp) {
				intrusive_ptr(lp).swap(*this);
			}
			return *this;
		}

		intrusive_ptr& operator=(const intrusive_ptr& lp) noexcept {
			if (ptr != lp.ptr) {
				intrusive_ptr(lp).swap(*this);
			}
			return *this;
		}

		template<typename U> requires(std::is_convertible_v<U*, T*>)
		intrusive_ptr& operator=(const intrusive_ptr<U>& lp) noexcept {
			if (ptr != lp.ptr) {
				intrusive_ptr(lp).swap(*this);
			}
			return *this;
		}

		intrusive_ptr& operator=(intrusive_ptr&& lp) noexcept {
			if (ptr != lp.ptr) {
				intrusive_ptr(std::move(lp)).swap(*this);
			}
			return *this;
		}

		template<typename U> requires(std::is_convertible_v<U*, T*>)
		intrusive_ptr& operator=(intrusive_ptr<U>&& lp) noexcept {
			if (ptr != lp.ptr) {
				intrusive_ptr(std::move(lp)).swap(*this);
			}
			return *this;
		}

		//== access ================================================

		T& operator*() const noexcept {
			assert(ptr);
			return *this->ptr;
		}

		T* operator->() const noexcept {
			assert(ptr);
			return ptr;
		}

		T* get() const noexcept { return ptr; }

		//== method ================================================

		bool operator!() const noexcept {
			return !ptr;
		}

		explicit operator bool() const noexcept {
			return ptr != nullptr;
		}

		void swap(intrusive_ptr& other) noexcept {
			std::swap(ptr, other.ptr);
		}

		void release() noexcept {
			if (ptr) {
				const auto object = reinterpret_cast<intrusive_base*>(ptr);
				ptr = nullptr;
				if (object->release() <= 0) {
					object->custom_deleter()(object);
				}
			}
		}

		void reset() noexcept {
			intrusive_ptr().swap(*this);
		}

		template<typename U> requires(std::is_convertible_v<U*, T*>)
		void reset(U* pValue) noexcept {
			intrusive_ptr(pValue).swap(*this);
		}

	protected:
		template<typename>
		friend class intrusive_ptr;

		T* ptr = nullptr;
	};
}
