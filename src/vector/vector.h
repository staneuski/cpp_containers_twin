#pragma once
#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>

namespace cstl {

template <typename T>
class Vector {
public:
    Vector() = default;

    explicit Vector(const size_t size)
            : data_(Allocate(size))
            , capacity_(size)
            , size_(size) {

        size_t i = 0;
        try {
            for (; i != size; ++i)
                new (data_ + i) T();
        } catch (...) {
            SafeThrow(data_, i);
        }
    }

    Vector(const Vector& other)
            : data_(Allocate(other.size_))
            , capacity_(other.size_)
            , size_(other.size_) {
        size_t i = 0;
        try {
            for (; i != other.size_; ++i)
                CopyConstruct(data_ + i, other.data_[i]);
        } catch (...) {
            SafeThrow(data_, i);
        }
    }

    ~Vector() {
        DestroyN(data_, size_);
        Deallocate(data_);
    }

    size_t Size() const noexcept {
        return size_;
    }

    size_t Capacity() const noexcept {
        return capacity_;
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<Vector&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity <= capacity_)
            return;

        T* new_data = Allocate(new_capacity);

        size_t i = 0;
        try {
            for (; i != size_; ++i)
                CopyConstruct(new_data + i, data_[i]);
        } catch (...) {
            SafeThrow(new_data, i);
        }

        DestroyN(data_, size_);
        Deallocate(data_);

        data_ = new_data;
        capacity_ = new_capacity;
    }

private:
    T* data_ = nullptr;
    size_t capacity_ = 0;
    size_t size_ = 0;

    static T* Allocate(size_t n) {
        return n != 0 ? static_cast<T*>(operator new(n*sizeof(T))) : nullptr;
    }

    static void Deallocate(T* buf) noexcept {
        operator delete(buf);
    }

    static void CopyConstruct(T* buf, const T& elem) {
        new (buf) T(elem);
    }

    static void Destroy(T* buf) noexcept {
        buf->~T();
    }

    static void DestroyN(T* buf, size_t n) noexcept {
        for (size_t i = 0; i != n; ++i)
            Destroy(buf + i);
    }

    static void SafeThrow(T* buf, const size_t i) {
        DestroyN(buf, i);
        Deallocate(buf);
        throw;
    }
};

} // namespace cstl