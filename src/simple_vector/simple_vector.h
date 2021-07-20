#pragma once
#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <stdexcept>

#include "array_ptr.h"
#include "size_obj_wrapper.h"

namespace cstl {

template <typename Type>
class SimpleVector {
    using Iterator = Type*;
    using ConstIterator = const Type*;

public:
    /* --------------------- Special Member Functions ---------------------- */

    SimpleVector() noexcept = default;

    SimpleVector(size_t size) : SimpleVector(size, {}) {}

    explicit SimpleVector(size_t size, const Type& value)
        : capacity_(size)
        , size_(size)
        , elements_(size)
    {
        std::fill(begin(), end(), value);
    }

    SimpleVector(size_t size, Type&& value)
        : capacity_(size)
        , size_(size)
        , elements_(size)
    {
        for (size_t i = 0u; i < size_; ++i)
            At(i) = std::move(value);
    }

    SimpleVector(std::initializer_list<Type> init_values)
        : capacity_(init_values.size())
        , size_(init_values.size())
        , elements_(init_values.size())
    {
        std::move(init_values.begin(), init_values.end(), begin());
    }

    SimpleVector(const SizeObjWrapper capacity_obj)
        : capacity_(capacity_obj.Get())
    {
    }

    SimpleVector(const SimpleVector& other) {
        SimpleVector<Type> tmp(other.size_);
        std::copy(other.begin(), other.end(), tmp.begin());
        swap(tmp);
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (begin() != rhs.begin()) {
            SimpleVector rhs_copy(rhs);
            swap(rhs_copy);
        }
        return *this;
    }

    SimpleVector(SimpleVector&& other) 
        : capacity_(other.capacity_)
        , size_(other.size_)
    {
        SimpleVector tmp(other.size_);
        std::move(other.begin(), other.end(), tmp.begin());

        elements_.swap(tmp.elements_);
        other.Clear();
    }

    SimpleVector& operator=(SimpleVector&& rhs) {
        if (rhs.IsEmpty()) {
            Clear();
        } else if (this != &rhs) {
            SimpleVector tmp(std::move(rhs));
            swap(tmp);
        }
        return *this;
    }

    /* ----------------------------- Methods ------------------------------- */

    inline Type& operator[](size_t index) noexcept {
        return elements_[index];
    }

    inline const Type& operator[](size_t index) const noexcept {
        return elements_[index];
    }

    inline size_t GetSize() const noexcept {
        return size_;
    }

    inline size_t GetCapacity() const noexcept {
        return capacity_;
    }

    inline bool IsEmpty() const noexcept {
        return !size_ && begin() == end();
    }

    Type& At(size_t index) {
        if (index >= size_)
            throw std::out_of_range(
                "Try to get element at index " + std::to_string(index) +
                " for vector size " + std::to_string(size_)
            );

        return elements_[index];
    }

    const Type& At(size_t index) const {
        if (index >= size_)
            throw std::out_of_range(
                "Try to get element at index " + std::to_string(index) +
                " for vector size " + std::to_string(size_)
            );

        return elements_[index];
    }

    void Resize(size_t new_size) {
        if (new_size <= size_) {
            size_ = new_size;
        } else if (new_size < capacity_) {
            std::fill(end(), std::next(elements_.Get(), new_size), 0);
            size_ = new_size;
        } else {
            Extend(std::max(2*capacity_, new_size), new_size);
        }
    }

    inline void Reserve(size_t new_capacity) {
        if (capacity_ <= new_capacity)
            Extend(new_capacity, size_);
    }

    inline void PushBack(const Type& value) {
        Insert(end(), value);
    }

    inline void PushBack(Type&& value) {
        Insert(end(), std::move(value));
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        size_t pos_index = InsertDefault(pos);
        At(pos_index) = std::move(value);
        return &At(pos_index);
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        size_t pos_index = InsertDefault(pos);
        At(pos_index) = std::move(value);
        return &At(pos_index);
    }

    inline void Clear() noexcept {
        size_ = 0u;
    }

    inline void PopBack() noexcept {
        --size_;
    }

    Iterator Erase(ConstIterator pos) {
        if (pos == std::prev(cend())) {
            --size_;
            return end();
        }

        size_t pos_index = std::distance(cbegin(), pos);
        std::move(
            std::next(begin(), pos_index + 1),
            end(),
            std::next(begin(), pos_index)
        );
        --size_;

        return &At(pos_index);
    }

    void swap(SimpleVector& other) noexcept {
        elements_.swap(other.elements_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    /* ----------------------------- Iterators ----------------------------- */

    [[nodiscard]] inline Iterator begin() noexcept {
        return elements_.Get();
    }

    [[nodiscard]] inline Iterator end() noexcept {
        return std::next(begin(), size_);
    }

    [[nodiscard]] inline ConstIterator begin() const noexcept {
        return cbegin();
    }

    [[nodiscard]] inline ConstIterator end() const noexcept {
        return cend();
    }

    [[nodiscard]] inline ConstIterator cbegin() const noexcept {
        return elements_.Get();
    }

    [[nodiscard]] inline ConstIterator cend() const noexcept {
        return std::next(begin(), size_);
    }

private:
    size_t capacity_{}, size_{};
    ArrayPtr<Type> elements_{};

    void Extend(const size_t new_capacity, const size_t new_size) {
        ArrayPtr<Type> new_elements(new_capacity);
        std::fill(new_elements.Get(), std::next(new_elements.Get(), new_size), 0);

        std::move(begin(), end(), new_elements.Get());
        elements_.swap(new_elements);

        capacity_ = new_capacity;
        size_ = new_size;
    }

    size_t InsertDefault(ConstIterator pos) {
        size_t pos_index = std::distance(cbegin(), pos);
        if (size_ == capacity_)
            Resize(size_ + 1);
        else
            ++size_;

        ArrayPtr<Type> tmp(size_);
        std::move(begin(), &At(pos_index), tmp.Get());
        std::move_backward(
            &At(pos_index),
            std::prev(end()),
            std::next(tmp.Get(), size_)
        );
        elements_.swap(tmp);

        return pos_index;
    }
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs.GetSize() == rhs.GetSize())
        && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(
        lhs.begin(), lhs.end(),
        rhs.begin(), rhs.end()
    );
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs < rhs) || (lhs == rhs);
}

template <typename Type>
bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs) && (lhs != rhs);
}

template <typename Type>
bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs) || (lhs == rhs);
}

} // namespace cstl