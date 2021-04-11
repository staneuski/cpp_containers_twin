#pragma once
#include <algorithm>
#include <cassert>
#include <initializer_list>

#include "array_ptr.h"

template <typename Type>
class SimpleVector {
    using Iterator = Type*;
    using ConstIterator = const Type*;

public:
    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) {
        //TODO
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) {
        //TODO
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        //TODO
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
        return begin() == end();
    }

    // Возвращает ссылку на элемент с индексом index
    inline Type& operator[](size_t index) noexcept {
        return elements_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    inline const Type& operator[](size_t index) const noexcept {
        return elements_[index];
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
    inline const Type& At(size_t index) const {
        return At(index);
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        //TODO
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        //TODO
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    inline Iterator begin() noexcept {
        return numbers.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    inline Iterator end() noexcept {
        return std::next(begin(), size_);
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    inline ConstIterator begin() const noexcept {
        return cbegin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    inline ConstIterator end() const noexcept {
        return cend();
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    inline ConstIterator cbegin() const noexcept {
        return numbers.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    inline ConstIterator cend() const noexcept {
        return std::next(begin(), size_);
    }

private:
    size_t capacity_{}, size_{};
    ArrayPtr<T> elements_{};
};