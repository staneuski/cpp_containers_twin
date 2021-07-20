#pragma once
#include <cassert>
#include <cstdlib>
#include <utility>

namespace cstl {

template <typename Type>
class ArrayPtr {
public:
    ArrayPtr() = default;

    explicit ArrayPtr(size_t size)
        : raw_ptr_(size ? new Type[size] : nullptr)
    {
    }

    explicit ArrayPtr(Type* raw_ptr) noexcept : raw_ptr_(raw_ptr) {}

    ArrayPtr(const ArrayPtr&) = delete;

    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ArrayPtr(ArrayPtr&& other) {
        delete[] raw_ptr_;
        raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
    }

    ArrayPtr& operator=(ArrayPtr&& other) {
        if (this != &other) {
            delete[] raw_ptr_;
            raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
        }
        return *this;
    }

    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    [[nodiscard]] Type* Release() noexcept {
        return std::exchange(raw_ptr_, nullptr);
    }

    inline Type& operator[](size_t index) noexcept {
        return *(raw_ptr_ + index);
    }

    inline const Type& operator[](size_t index) const noexcept {
        return *(raw_ptr_ + index);
    }

    inline explicit operator bool() const {
        return raw_ptr_;
    }

    inline Type* Get() const noexcept {
        return raw_ptr_;
    }

    inline void swap(ArrayPtr& other) noexcept {
        std::swap(raw_ptr_, other.raw_ptr_);
    }

private:
    Type* raw_ptr_ = nullptr;
};

} // namespace cstl
