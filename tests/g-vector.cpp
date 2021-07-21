#include "vector/vector.h"

#include <gtest/gtest.h>

namespace {

struct Obj {
    Obj() {
        ++num_default_constructed;
    }

    Obj(const Obj&) {
        ++num_copied;
    }

    Obj(Obj&&) noexcept {
        ++num_moved;
    }

    Obj& operator=(const Obj& other) = default;
    Obj& operator=(Obj&& other) = default;

    ~Obj() {
        ++num_destroyed;
    }

    static int GetAliveObjectCount() {
        return num_default_constructed + num_copied + num_moved - num_destroyed;
    }

    static void ResetCounters() {
        num_default_constructed = 0;
        num_copied = 0;
        num_moved = 0;
        num_destroyed = 0;
    }

    static inline int num_default_constructed = 0;
    static inline int num_copied = 0;
    static inline int num_moved = 0;
    static inline int num_destroyed = 0;
};

}  // namespace

TEST (Vector, ConstructAndReserve) {
    using namespace cstl;

    Obj::ResetCounters();
    const size_t SIZE = 100500;
    const size_t INDEX = 10;
    const int MAGIC = 42;

    {
        Vector<int> v;
        ASSERT_EQ(v.Capacity(), 0);
        ASSERT_EQ(v.Size(), 0);

        v.Reserve(SIZE);
        ASSERT_EQ(v.Capacity(), SIZE);
        ASSERT_EQ(v.Size(), 0);
    }
    {
        Vector<int> v(SIZE);
        const auto& cv(v);
        ASSERT_EQ(v.Capacity(), SIZE);
        ASSERT_EQ(v.Size(), SIZE);
        ASSERT_EQ(v[0], 0);
        ASSERT_EQ(&v[0], &cv[0]);
        v[INDEX] = MAGIC;
        ASSERT_EQ(v[INDEX], MAGIC);
        ASSERT_EQ(&v[100] - &v[0], 100);

        v.Reserve(SIZE * 2);
        ASSERT_EQ(v.Size(), SIZE);
        ASSERT_EQ(v.Capacity(), SIZE*2);
        ASSERT_EQ(v[INDEX], MAGIC);
    }
    {
        Vector<int> v(SIZE);
        v[INDEX] = MAGIC;
        const auto v_copy(v);
        ASSERT_NE(&v[INDEX], &v_copy[INDEX]);
        ASSERT_EQ(v[INDEX], v_copy[INDEX]);
    }

    {
        Vector<Obj> v;
        v.Reserve(SIZE);
        ASSERT_EQ(Obj::GetAliveObjectCount(), 0);
    }
    {
        Vector<Obj> v(SIZE);
        ASSERT_EQ(Obj::GetAliveObjectCount(), SIZE);
        v.Reserve(SIZE*2);
        ASSERT_EQ(Obj::GetAliveObjectCount(), SIZE);
    }
    ASSERT_EQ(Obj::GetAliveObjectCount(), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}