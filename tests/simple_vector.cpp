#include <cassert>
#include <iostream>
#include <stdexcept>
#include <numeric>

#include <gtest/gtest.h>

#include "simple_vector/simple_vector.h"

class SpecialMembersTester {
public:
    SpecialMembersTester()
        : SpecialMembersTester(5) {
    }

    SpecialMembersTester(size_t num)
        : element_(num) {
    }

    SpecialMembersTester(const SpecialMembersTester& other) = delete;
    SpecialMembersTester& operator=(const SpecialMembersTester& other) = delete;
    SpecialMembersTester(SpecialMembersTester&& other) {
        element_ = std::exchange(other.element_, 0);
    }

    SpecialMembersTester& operator=(SpecialMembersTester&& other) {
        element_ = std::exchange(other.element_, 0);
        return *this;
    }

    size_t GetValue() const {
        return element_;
    }

private:
    size_t element_;
};

SimpleVector<int> GenerateVector(size_t size) {
    SimpleVector<int> v(size);
    std::iota(v.begin(), v.end(), 1);
    return v;
}

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
    ASSERT_EQ(v[2], 17);
    ASSERT_EQ(v[3], 0);
    ASSERT_EQ(v.GetSize(), 7);
    ASSERT_GE(v.GetCapacity(), v.GetSize());
}
TEST(SimpleVector, ResizeShrink) {
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

// PushBack
TEST(SimpleVector, PushBack) {
    SimpleVector<int> v(1);
    v.PushBack(42);
    ASSERT_GE(v.GetCapacity(), v.GetSize());
    ASSERT_EQ(v.GetSize(), 2);
    ASSERT_EQ(v[0], 0);
    ASSERT_EQ(v[1], 42);
}

// Если хватает места, PushBack не увеличивает Capacity
TEST(SimpleVector, PushBackIncreasingCapacity) {
    SimpleVector<int> v(2);
    v.Resize(1);
    const size_t old_capacity = v.GetCapacity();
    v.PushBack(123);
    ASSERT_EQ(v.GetSize(), 2);
    ASSERT_EQ(v.GetCapacity(), old_capacity);
}

// PopBack
TEST(SimpleVector, PopBack) {
    SimpleVector<int> v{0, 1, 2, 3};
    const size_t old_capacity = v.GetCapacity();
    const auto old_begin = v.begin();
    v.PopBack();
    ASSERT_EQ(v.GetCapacity(), old_capacity);
    ASSERT_EQ(v.begin(), old_begin);
    ASSERT_EQ(v, (SimpleVector<int>{0, 1, 2}));
}

// Конструктор копирования
TEST(SimpleVector, CopyConstructor) {
    SimpleVector<int> numbers{1, 2};
    auto numbers_copy(numbers);
    ASSERT_NE(&numbers_copy[0], &numbers[0]);
    ASSERT_EQ(numbers_copy.GetSize(), numbers.GetSize());
    for (size_t i = 0; i < numbers.GetSize(); ++i) {
        ASSERT_EQ(numbers_copy[i], numbers[i]);
        ASSERT_NE(&numbers_copy[i], &numbers[i]);
    }
}

// Сравнение
TEST(SimpleVector, Comparison) {
    ASSERT_EQ((SimpleVector{1, 2, 3}), (SimpleVector{1, 2, 3}));
    ASSERT_NE((SimpleVector{2, 3, 4}), (SimpleVector{2, 3, 3}));


    ASSERT_LT((SimpleVector{1, 2, 3}), (SimpleVector{1, 2, 3, 1}));
    ASSERT_GT((SimpleVector{1, 2, 3}), (SimpleVector{1, 2, 2, 1}));

    ASSERT_GE((SimpleVector{1, 2, 3}), (SimpleVector{1, 2, 3}));
    ASSERT_GE((SimpleVector{1, 2, 4}), (SimpleVector{1, 2, 3}));
    ASSERT_LE((SimpleVector{1, 2, 3}), (SimpleVector{1, 2, 3}));
    ASSERT_LE((SimpleVector{1, 2, 3}), (SimpleVector{1, 2, 4}));
}

// Обмен значений векторов
TEST(SimpleVector, Swap) {
    SimpleVector<int> v1{42, 666};
    SimpleVector<int> v2;
    v2.PushBack(0);
    v2.PushBack(1);
    v2.PushBack(2);
    const int* const begin1 = &v1[0];
    const int* const begin2 = &v2[0];

    const size_t capacity1 = v1.GetCapacity();
    const size_t capacity2 = v2.GetCapacity();

    const size_t size1 = v1.GetSize();
    const size_t size2 = v2.GetSize();

    static_assert(noexcept(v1.swap(v2)));
    v1.swap(v2);
    ASSERT_EQ(&v2[0], begin1);
    ASSERT_EQ(&v1[0], begin2);
    ASSERT_EQ(v1.GetSize(), size2);
    ASSERT_EQ(v2.GetSize(), size1);
    ASSERT_EQ(v1.GetCapacity(), capacity2);
    ASSERT_EQ(v2.GetCapacity(), capacity1);
}

// Присваивание
TEST(SimpleVector, Assignment) {
    SimpleVector<int> src_vector{1, 2, 3, 4};
    SimpleVector<int> dst_vector{1, 2, 3, 4, 5, 6};
    dst_vector = src_vector;
    ASSERT_EQ(dst_vector, src_vector);
}

// Вставка элементов
TEST(SimpleVector, Insert) {
    SimpleVector<int> v{1, 2, 3, 4};
    v.Insert(v.begin() + 2, 42);
    ASSERT_EQ(v, (SimpleVector<int>{1, 2, 42, 3, 4}));
    ASSERT_EQ(v.GetSize(), 5u);
}

// Удаление элементов
TEST(SimpleVector, Erase) {
    SimpleVector<int> v{1, 2, 3, 4};
    v.Erase(v.cbegin() + 2);
    ASSERT_EQ(v, (SimpleVector<int>{1, 2, 4}));
}

TEST(SimpleVector, ReserveConstructor) {
    SimpleVector<int> v(Reserve(5));
    ASSERT_EQ(v.GetCapacity(), 5);
    ASSERT_TRUE(v.IsEmpty());
}

TEST(SimpleVector, ReserveMethod) {
    SimpleVector<int> v;

    v.Reserve(5);
    ASSERT_EQ(v.GetCapacity(), 5);
    ASSERT_TRUE(v.IsEmpty());

    v.Reserve(1);
    ASSERT_EQ(v.GetCapacity(), 5)
        << "capacity shouldn't be changed after try to decreasing it";

    for (int i = 0; i < 10; ++i)
        v.PushBack(i);
    ASSERT_EQ(v.GetSize(), 10);

    v.Reserve(100);
    ASSERT_EQ(v.GetSize(), 10);
    ASSERT_EQ(v.GetCapacity(), 100);
    for (int i = 0; i < 10; ++i)
        ASSERT_EQ(v[i], i);
}

TEST(SimpleVector, TestTemporaryObjConstructor) {
    const size_t size = 1000000;

    SimpleVector<int> moved_vector(GenerateVector(size));
    ASSERT_EQ(moved_vector.GetSize(), size)
        << "Copy elision must work with a temporary object";
}

TEST(SimpleVector, TestTemporaryObjOperator) {
    const size_t size = 1000000;

    SimpleVector<int> moved_vector;
    ASSERT_EQ(moved_vector.GetSize(), 0);

    moved_vector = GenerateVector(size);
    ASSERT_EQ(moved_vector.GetSize(), size)
        << "operator= must work with a temporary object";
}

TEST(SimpleVector, TestNamedMoveConstructor) {
    const size_t size = 5;
    const std::string comment = "move constructor doesn't working";

    SimpleVector<int> vector_to_move(GenerateVector(size));
    ASSERT_EQ(vector_to_move.GetSize(), size);

    SimpleVector<int> moved_vector(std::move(vector_to_move));
    ASSERT_EQ(moved_vector.GetSize(), size) << comment;
    ASSERT_EQ(vector_to_move.GetSize(), 0) << comment;
}

TEST(SimpleVector, TestNamedMoveOperator) {
    const size_t size = 1000000;
    const std::string comment = "operator= must work with a named object";

    SimpleVector<int> vector_to_move(GenerateVector(size));
    ASSERT_EQ(vector_to_move.GetSize(), size);

    SimpleVector<int> moved_vector = std::move(vector_to_move);
    ASSERT_EQ(moved_vector.GetSize(), size) << comment;
    ASSERT_EQ(vector_to_move.GetSize(), 0) << comment;
}

TEST(SimpleVector, TestNoncopiableMoveConstructor) {
    const size_t size = 5;
    const std::string comment = "move constructor must work with a noncopiable object";

    SimpleVector<SpecialMembersTester> vector_to_move;
    for (size_t i = 0; i < size; ++i)
        vector_to_move.PushBack(SpecialMembersTester(i));

    SimpleVector<SpecialMembersTester> moved_vector = std::move(vector_to_move);
    ASSERT_EQ(moved_vector.GetSize(), size) << comment;
    ASSERT_EQ(vector_to_move.GetSize(), 0) << comment;

    for (size_t i = 0; i < size; ++i)
        ASSERT_EQ(moved_vector[i].GetValue(), i) << comment;
}

TEST(SimpleVector, TestNoncopiablePushBack) {
    const size_t size = 5;
    const std::string comment = "PushBack() must work with a noncopiable";

    SimpleVector<SpecialMembersTester> v;
    for (size_t i = 0; i < size; ++i)
        v.PushBack(SpecialMembersTester(i));

    ASSERT_EQ(v.GetSize(), size) << comment;
    for (size_t i = 0; i < size; ++i)
        ASSERT_EQ(v[i].GetValue(), i) << comment;
}

TEST(SimpleVector, TestNoncopiableInsert) {
    const size_t size = 5;
    const std::string comment = "Insert() must work with a noncopiable noncopiable object";
    const std::string begin_insert_comment = comment + " when inserting to begin";
    const std::string middle_insert_comment = comment + " when inserting to middle";
    const std::string end_insert_comment = comment + " when inserting to end";

    SimpleVector<SpecialMembersTester> v;
    for (size_t i = 0; i < size; ++i)
        v.PushBack(SpecialMembersTester(i));

    v.Insert(v.begin(), SpecialMembersTester(size + 1));
    ASSERT_EQ(v.GetSize(), size + 1) << begin_insert_comment;
    ASSERT_EQ(v.begin()->GetValue(), size + 1) << begin_insert_comment;

    v.Insert(v.end(), SpecialMembersTester(size + 2));
    ASSERT_EQ(v.GetSize(), size + 2) << end_insert_comment;
    ASSERT_EQ((v.end() - 1)->GetValue(), size + 2) << end_insert_comment;

    v.Insert(v.begin() + 3, SpecialMembersTester(size + 3));
    ASSERT_EQ(v.GetSize(), size + 3) << middle_insert_comment;
    ASSERT_EQ((v.begin() + 3)->GetValue(), size + 3) << middle_insert_comment;
}

TEST(SimpleVector, TestNoncopiableErase) {
    const size_t size = 3;

    SimpleVector<SpecialMembersTester> v;
    for (size_t i = 0; i < size; ++i)
        v.PushBack(SpecialMembersTester(i));

    auto it = v.Erase(v.begin());
    ASSERT_EQ(it->GetValue(), 1)
        << "Erase() must work with a noncopiable noncopiable object";
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}