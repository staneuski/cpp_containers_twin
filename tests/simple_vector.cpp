#include <cassert>
#include <stdexcept>

#include <gtest/gtest.h>

#include "simple_vector/simple_vector.h"


// Инициализация конструктором по умолчанию
TEST(SimpleVector, InitialisingDefault) {
    SimpleVector<int> v;
    ASSERT_EQ(v.GetSize(), 0u);
    ASSERT_EQ(v.GetCapacity(), 0u);
    ASSERT_TRUE(v.IsEmpty());
}

// Инициализация вектора указанного размера
TEST(SimpleVector, InitialisingSetSize) {
    SimpleVector<int> v(5);
    ASSERT_EQ(v.GetSize(), 5u);
    ASSERT_EQ(v.GetCapacity(), 5u);
    ASSERT_TRUE(!v.IsEmpty());
    for (size_t i = 0; i < v.GetSize(); ++i)
        ASSERT_EQ(v[i], 0);
}

// Инициализация вектора, заполненного заданным значением
TEST(SimpleVector, InitialisingFill) {
    SimpleVector<int> v(3, 42);
    ASSERT_EQ(v.GetSize(), 3);
    ASSERT_EQ(v.GetCapacity(), 3);
    for (size_t i = 0; i < v.GetSize(); ++i)
        ASSERT_EQ(v[i], 42);
}

// Инициализация вектора при помощи initializer_list
TEST(SimpleVector, initializer_list) {
    SimpleVector<int> v{1, 2, 3};
    ASSERT_EQ(v.GetSize(), 3);
    ASSERT_EQ(v.GetCapacity(), 3);
    ASSERT_EQ(v[2], 3);
}

// Доступ к элементам при помощи At
TEST(SimpleVector, At) {
    SimpleVector<int> v(3);
    ASSERT_EQ(&v.At(2), &v[2]);
    try {
        v.At(3);
        assert(false);  // Ожидается выбрасывание исключения
    } catch (const std::out_of_range&) {
    } catch (...) {
        assert(false);  // Не ожидается исключение, отличное от out_of_range
    }
}

// Очистка вектора
TEST(SimpleVector, Clear) {
    SimpleVector<int> v(10);
    const size_t old_capacity = v.GetCapacity();
    v.Clear();
    ASSERT_FALSE(v.GetSize());
    ASSERT_EQ(v.GetCapacity(), old_capacity);
}

// Изменение размера
TEST(SimpleVector, Resize0) {
    SimpleVector<int> v(3);
    v[2] = 17;
    v.Resize(7);
    ASSERT_EQ(v.GetSize(), 7);
    ASSERT_TRUE(v.GetCapacity() >= v.GetSize());
    ASSERT_EQ(v[2], 17);
    ASSERT_EQ(v[3], 0);
}
TEST(SimpleVector, Resize1) {
    SimpleVector<int> v(3);
    v[0] = 42;
    v[1] = 55;
    const size_t old_capacity = v.GetCapacity();
    v.Resize(2);
    ASSERT_EQ(v.GetSize(), 2);
    ASSERT_EQ(v.GetCapacity(), old_capacity);
    ASSERT_EQ(v[0], 42);
    ASSERT_EQ(v[1], 55);
}
TEST(SimpleVector, Resize2) {
    const size_t old_size = 3;
    SimpleVector<int> v(3);
    v.Resize(old_size + 5);
    v[3] = 42;
    v.Resize(old_size);
    v.Resize(old_size + 2);
    ASSERT_EQ(v[3], 0);
}

// Итерерирование по пустому вектору
TEST(SimpleVector, IteratingByEmpty) {
    SimpleVector<int> v;
    ASSERT_FALSE(v.begin());
    ASSERT_FALSE(v.end());
}

// Непустой вектор
TEST(SimpleVector, Iterating) {
    SimpleVector<int> v(10, 42);
    ASSERT_TRUE(v.begin());
    ASSERT_EQ(*v.begin(), 42);
    ASSERT_EQ(v.end(), v.begin() + v.GetSize());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}