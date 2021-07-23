#pragma once
#include <cassert>
#include <cstdlib>
#include <memory>
#include <new>
#include <utility>

namespace cstl {

// ---------- RawMemory ---------------

template <typename T>
class RawMemory {
public:
    RawMemory() = default;

    explicit RawMemory(size_t capacity)
        : buffer_(Allocate(capacity))
        , capacity_(capacity) {
    }

    RawMemory(const RawMemory&) = delete;

    RawMemory(RawMemory&& other) noexcept {
        Swap(other);
    }

    ~RawMemory() {
        Deallocate(buffer_);
    }

    RawMemory& operator=(const RawMemory&) = delete;

    RawMemory& operator=(RawMemory&& rhs) noexcept {
        if (this != &rhs)
            Swap(rhs);

        return *this;
    }

    T* operator+(size_t offset) noexcept {
        assert(offset <= capacity_);
        return buffer_ + offset;
    }

    const T* operator+(size_t offset) const noexcept {
        return const_cast<RawMemory&>(*this) + offset;
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<RawMemory&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < capacity_);
        return buffer_[index];
    }

    const T* GetAddress() const noexcept {
        return buffer_;
    }

    T* GetAddress() noexcept {
        return buffer_;
    }

    size_t Capacity() const {
        return capacity_;
    }

    void Swap(RawMemory& other) noexcept {
        std::swap(buffer_, other.buffer_);
        std::swap(capacity_, other.capacity_);
    }

private:
    T* buffer_ = nullptr;
    size_t capacity_ = 0;

    static T* Allocate(size_t n) {
        return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
    }

    static void Deallocate(T* buf) noexcept {
        operator delete(buf);
    }
};

// ---------- Vector ------------------

template <typename T>
class Vector {
public:
    Vector() = default;

    explicit Vector(const size_t size)
            : data_(size)
            , size_(size) {
        std::uninitialized_value_construct_n(data_.GetAddress(), size);
    }

    Vector(const Vector& other)
            : data_(other.size_)
            , size_(other.size_) {
        std::uninitialized_copy_n(
            other.data_.GetAddress(),
            other.size_,
            data_.GetAddress()
        );
    }

    Vector(Vector&& other) noexcept {
        Swap(other);
    }

    ~Vector() {
        std::destroy_n(data_.GetAddress(), size_);
    }

    Vector& operator=(const Vector& rhs) {
        if (this != &rhs && rhs.size_ > data_.Capacity()) {
            Vector tmp(rhs);
            Swap(tmp);
        } else if (this != &rhs) {
            for (size_t i = 0; i < size_ && i < rhs.size_; i++)
                data_[i] = rhs.data_[i];

            if (size_ < rhs.size_)
                std::uninitialized_copy_n(
                    rhs.data_ + size_,
                    rhs.size_ - size_,
                    data_.GetAddress() + size_
                );
            else
                std::destroy_n(
                    data_.GetAddress() + rhs.size_, 
                    size_ - rhs.size_
                );
            size_ = rhs.size_;
        }
        return *this;
    }

    Vector& operator=(Vector&& rhs) noexcept {
        Swap(rhs);
        return *this;
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<Vector&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    }

    void Swap(Vector& other) noexcept {
        data_.Swap(other.data_);
        std::swap(size_, other.size_);
    }

    size_t Size() const noexcept {
        return size_;
    }

    size_t Capacity() const noexcept {
        return data_.Capacity();
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity <= Capacity())
            return;

        RawMemory<T> new_data(new_capacity);
        UninitializedCopyOrMove(std::move(new_data));
    }

    void Resize(size_t new_size) {
        Reserve(new_size);

        if (size_ < new_size)
            std::uninitialized_value_construct_n(data_ + size_, new_size - size_);
        else
            std::destroy_n(data_ + new_size, size_ - new_size);
        size_ = new_size;
    }

    void PushBack(const T& value) {
        if (size_ == Capacity()) {
            RawMemory<T> new_data(size_ == 0 ? 1 : 2*size_);
            new (new_data + size_) T(value);
            UninitializedCopyOrMove(std::move(new_data));
        } else {
            new (data_ + size_) T(value);
        }
        ++size_;
    }

    void PushBack(T&& value) {
        if (size_ == Capacity()) {
            RawMemory<T> new_data(size_ == 0 ? 1 : 2*size_);
            new (new_data + size_) T(std::move(value));
            UninitializedCopyOrMove(std::move(new_data));
        } else {
            new (data_ + size_) T(std::move(value));
        }
        ++size_;
    }

    template <typename... Args>
    T& EmplaceBack(Args&&... args) {
        if (size_ == Capacity()) {
            RawMemory<T> new_data(size_ == 0 ? 1 : 2*size_);
            new (new_data + size_) T(std::forward<Args>(args)...);
            UninitializedCopyOrMove(std::move(new_data));
        } else {
            new (data_ + size_) T(std::forward<Args>(args)...);
        }
        return data_[size_++];
    }

    void PopBack() {
        std::destroy_at(data_ + size_ - 1);
        --size_;
    }

private:
    RawMemory<T> data_;
    size_t size_ = 0;

    void UninitializedCopyOrMove(RawMemory<T>&& new_data) {
        if constexpr (std::is_nothrow_move_constructible_v<T>
                      || !std::is_copy_constructible_v<T>)
            std::uninitialized_move_n(
                data_.GetAddress(),
                size_,
                new_data.GetAddress()
            );
        else
            std::uninitialized_copy_n(
                data_.GetAddress(),
                size_,
                new_data.GetAddress()
            );

        std::destroy_n(data_.GetAddress(), size_);
        data_.Swap(new_data);
    }
};

} // namespace cstl