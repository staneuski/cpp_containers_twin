#pragma once
#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <stdexcept>

#include "array_ptr.h"
#include "size_obj_wrapper.h"

template <typename Type>
class SimpleVector {
    using Iterator = Type*;
    using ConstIterator = const Type*;

public:
    /* --------------------------- Constructors ---------------------------- */

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size)
        : capacity_(size), size_(size)
        , elements_(size)
    {
        std::fill(begin(), end(), 0);
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value)
        : capacity_(size), size_(size)
        , elements_(size)
    {
        std::fill(begin(), end(), value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init_values)
        : capacity_(init_values.size()), size_(init_values.size())
        , elements_(init_values.size())
    {
        size_t i = 0;
        for (const Type& init_value : init_values) {
            elements_[i] = init_value;
            ++i;
        }
    }

    SimpleVector(const SimpleVector& other) {
        SimpleVector<Type> tmp(other.size_);
        std::copy(other.begin(), other.end(), tmp.begin());
        swap(tmp);
    }

    SimpleVector(const SizeObjWrapper capacity_obj)
        : capacity_(capacity_obj.Get())
        , elements_(capacity_obj.Get())
    {
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (begin() != rhs.begin()) {
            SimpleVector rhs_copy(rhs);
            swap(rhs_copy);
        }
        return *this;
    }

    /* ----------------------------- Methods ------------------------------- */

    // Возвращает ссылку на элемент с индексом index
    inline Type& operator[](size_t index) noexcept {
        return elements_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    inline const Type& operator[](size_t index) const noexcept {
        return elements_[index];
    }

    // Возвращает количество элементов в массиве
    inline size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    inline size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    inline bool IsEmpty() const noexcept {
        return !size_ && begin() == end();
    }

    // Возвращает ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_)
            throw std::out_of_range(
                "Try to get element at index " + std::to_string(index) +
                " for vector size " + std::to_string(size_)
            );

        return elements_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_)
            throw std::out_of_range(
                "Try to get element at index " + std::to_string(index) +
                " for vector size " + std::to_string(size_)
            );

        return elements_[index];
    }


    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
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
    };

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        size_t pos_t = std::distance(cbegin(), pos);
        if (size_ == capacity_)
            Resize(size_ + 1);
        else
            ++size_;

        ArrayPtr<Type> tmp(size_);
        std::copy(begin(), &At(pos_t), tmp.Get());
        std::copy_backward(
            &At(pos_t),
            std::prev(end()),
            std::next(tmp.Get(), size_)
        );
        elements_.swap(tmp);

        At(pos_t) = value;
        return &At(pos_t);
    }

    // Обнуляет размер массива, не изменяя его вместимость
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

        size_t pos_t = std::distance(cbegin(), pos);
        for (Iterator it = &At(pos_t); it != end(); ++it)
            *it = *std::next(it);
        --size_;

        return &At(pos_t);
    }

    void swap(SimpleVector& other) noexcept {
        elements_.swap(other.elements_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    /* ----------------------------- Iterators ----------------------------- */

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    [[nodiscard]] inline Iterator begin() noexcept {
        return elements_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    [[nodiscard]] inline Iterator end() noexcept {
        return std::next(begin(), size_);
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    [[nodiscard]] inline ConstIterator begin() const noexcept {
        return cbegin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    [[nodiscard]] inline ConstIterator end() const noexcept {
        return cend();
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    [[nodiscard]] inline ConstIterator cbegin() const noexcept {
        return elements_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    [[nodiscard]] inline ConstIterator cend() const noexcept {
        return std::next(begin(), size_);
    }

private:
    size_t capacity_{}, size_{};
    ArrayPtr<Type> elements_{};

    void Extend(const size_t new_capacity, const size_t new_size) {
        ArrayPtr<Type> new_elements(new_capacity);
        std::fill(new_elements.Get(), std::next(new_elements.Get(), new_size), 0);

        std::copy(begin(), end(), new_elements.Get());
        elements_.swap(new_elements);

        capacity_ = new_capacity;
        size_ = new_size;
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