#include "vector/vector.h"

#include <gtest/gtest.h>

namespace {

// "Магическое" число, используемое для отслеживания живости объекта
inline const uint32_t DEFAULT_COOKIE = 0xdeadbeef;

struct TestObj {
    TestObj() = default;

    TestObj(const TestObj& other) = default;

    TestObj& operator=(const TestObj& other) = default;

    TestObj(TestObj&& other) = default;

    TestObj& operator=(TestObj&& other) = default;

    ~TestObj() {
        cookie = 0;
    }

    [[nodiscard]] bool IsAlive() const noexcept {
        return cookie == DEFAULT_COOKIE;
    }

    uint32_t cookie = DEFAULT_COOKIE;
};

struct Obj {
    Obj() {
        if (default_construction_throw_countdown > 0)
            if (--default_construction_throw_countdown == 0)
                throw std::runtime_error("Oops");

        ++num_default_constructed;
    }

    explicit Obj(int id) : id(id) {
        ++num_constructed_with_id;
    }

    Obj(int id, std::string name) : id(id), name(std::move(name)) {
        ++num_constructed_with_id_and_name;
    }

    Obj(const Obj& other) : id(other.id) {
        if (other.throw_on_copy)
            throw std::runtime_error("Oops");
        ++num_copied;
    }

    Obj(Obj&& other) noexcept : id(other.id) {
        ++num_moved;
    }

    Obj& operator=(const Obj& other) {
        if (this != &other) {
            id = other.id;
            name = other.name;
            ++num_assigned;
        }
        return *this;
    }

    Obj& operator=(Obj&& other) noexcept {
        id = other.id;
        name = std::move(other.name);
        ++num_move_assigned;
        return *this;
    }

    ~Obj() {
        ++num_destroyed;
        id = 0;
    }

    static int GetAliveObjectCount() {
        return num_default_constructed
               + num_copied
               + num_moved
               + num_constructed_with_id
               + num_constructed_with_id_and_name
               - num_destroyed;
    }

    static void ResetCounters() {
        default_construction_throw_countdown = 0;
        num_default_constructed = 0;
        num_copied = 0;
        num_moved = 0;
        num_destroyed = 0;
        num_constructed_with_id = 0;
        num_constructed_with_id_and_name = 0;
        num_assigned = 0;
        num_move_assigned = 0;
    }

    bool throw_on_copy = false;
    int id = 0;
    std::string name;

    static inline int default_construction_throw_countdown = 0;
    static inline int num_default_constructed = 0;
    static inline int num_constructed_with_id = 0;
    static inline int num_constructed_with_id_and_name = 0;
    static inline int num_copied = 0;
    static inline int num_moved = 0;
    static inline int num_destroyed = 0;
    static inline int num_assigned = 0;
    static inline int num_move_assigned = 0;
};

}  // namespace

TEST(Vector, Reserve) {
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

        v.Reserve(SIZE*2);
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

TEST(Vector, SaveConstuct) {
    using namespace cstl;

    const size_t SIZE = 100;

    Obj::ResetCounters();
    {
        Obj::default_construction_throw_countdown = SIZE/2;

        ASSERT_THROW(Vector<Obj> v(SIZE), std::runtime_error);
        ASSERT_EQ(Obj::num_default_constructed, SIZE/2 - 1);
        ASSERT_EQ(Obj::GetAliveObjectCount(), 0);
    }

    Obj::ResetCounters();
    {
        Vector<Obj> v(SIZE);

        ASSERT_THROW(
            {
                v[SIZE/2].throw_on_copy = true;
                Vector<Obj> v_copy(v);
            },
            std::runtime_error
        );
        ASSERT_EQ(Obj::num_copied, SIZE/2);
        ASSERT_EQ(Obj::GetAliveObjectCount(), SIZE);
    }

    Obj::ResetCounters();
    {
        Vector<Obj> v(SIZE);
        v[SIZE - 1].throw_on_copy = true;
        v.Reserve(SIZE*2);

        ASSERT_EQ(v.Capacity(), SIZE*2);
        ASSERT_EQ(v.Size(), SIZE);
        ASSERT_EQ(Obj::GetAliveObjectCount(), SIZE);
    }
}

TEST(Vector, CopyAndMove) {
    using namespace cstl;

    const size_t MEDIUM_SIZE = 100;
    const size_t LARGE_SIZE = 250;
    const int ID = 42;

    {
        Obj::ResetCounters();
        Vector<int> v(MEDIUM_SIZE);
        {
            auto v_copy(std::move(v));

            ASSERT_EQ(v_copy.Size(), MEDIUM_SIZE);
            ASSERT_EQ(v_copy.Capacity(), MEDIUM_SIZE);
        }
        ASSERT_EQ(Obj::GetAliveObjectCount(), 0);
    }

    {
        Obj::ResetCounters();
        {
            Vector<Obj> v(MEDIUM_SIZE);
            v[MEDIUM_SIZE/2].id = ID;
            ASSERT_EQ(Obj::num_default_constructed, MEDIUM_SIZE);

            Vector<Obj> moved_from_v(std::move(v));
            ASSERT_EQ(moved_from_v.Size(), MEDIUM_SIZE);
            ASSERT_EQ(moved_from_v[MEDIUM_SIZE/2].id, ID);
        }
        ASSERT_EQ(Obj::GetAliveObjectCount(), 0);

        ASSERT_EQ(Obj::num_moved, 0);
        ASSERT_EQ(Obj::num_copied, 0);
        ASSERT_EQ(Obj::num_default_constructed, MEDIUM_SIZE);
    }

    {
        Obj::ResetCounters();
        Vector<Obj> v_medium(MEDIUM_SIZE);
        v_medium[MEDIUM_SIZE/2].id = ID;
        Vector<Obj> v_large(LARGE_SIZE);
        v_large = v_medium;
        ASSERT_EQ(v_large.Size(), MEDIUM_SIZE);
        ASSERT_EQ(v_large.Capacity(), LARGE_SIZE);
        ASSERT_EQ(v_large[MEDIUM_SIZE/2].id, ID);
        ASSERT_EQ(Obj::GetAliveObjectCount(), MEDIUM_SIZE + MEDIUM_SIZE);
    }

    {
        Obj::ResetCounters();
        Vector<Obj> v(MEDIUM_SIZE);
        {
            Vector<Obj> v_large(LARGE_SIZE);
            v_large[LARGE_SIZE - 1].id = ID;
            v = v_large;
            ASSERT_EQ(v.Size(), LARGE_SIZE);
            ASSERT_EQ(v_large.Capacity(), LARGE_SIZE);
            ASSERT_EQ(v_large[LARGE_SIZE - 1].id, ID);
            ASSERT_EQ(Obj::GetAliveObjectCount(), LARGE_SIZE + LARGE_SIZE);
        }
        ASSERT_EQ(Obj::GetAliveObjectCount(), LARGE_SIZE);
    }

    ASSERT_EQ(Obj::GetAliveObjectCount(), 0);
    {
        Obj::ResetCounters();
        Vector<Obj> v(MEDIUM_SIZE);
        v[MEDIUM_SIZE - 1].id = ID;
        Vector<Obj> v_small(MEDIUM_SIZE/2);
        v_small.Reserve(MEDIUM_SIZE + 1);
        const size_t num_copies = Obj::num_copied;
        v_small = v;
        ASSERT_EQ(v_small.Size(), v.Size());
        ASSERT_EQ(v_small.Capacity(), MEDIUM_SIZE + 1);
        v_small[MEDIUM_SIZE - 1].id = ID;
        ASSERT_EQ(Obj::num_copied - num_copies, MEDIUM_SIZE - (MEDIUM_SIZE/2));
    }
}

TEST(Vector, Resize) {
    using namespace cstl;

    const size_t SIZE = 100'500;

    {
        Obj::ResetCounters();
        Vector<Obj> v;
        v.Resize(SIZE);
        ASSERT_EQ(v.Size(), SIZE);
        ASSERT_EQ(v.Capacity(), SIZE);
        ASSERT_EQ(Obj::num_default_constructed, SIZE);
    }
    ASSERT_EQ(Obj::GetAliveObjectCount(), 0);

    {
        const size_t NEW_SIZE = 10'000;
        Obj::ResetCounters();
        Vector<Obj> v(SIZE);
        v.Resize(NEW_SIZE);
        ASSERT_EQ(v.Size(), NEW_SIZE);
        ASSERT_EQ(v.Capacity(), SIZE);
        ASSERT_EQ(Obj::num_destroyed, SIZE - NEW_SIZE);
    }
    ASSERT_EQ(Obj::GetAliveObjectCount(), 0);
}

TEST(Vector, PushBack) {
    using namespace cstl;

    const size_t ID = 42;
    const size_t SIZE = 100'500;

    {
        Obj::ResetCounters();
        Vector<Obj> v(SIZE);
        Obj o{ID};
        v.PushBack(o);
        ASSERT_EQ(v.Size(), SIZE + 1);
        ASSERT_EQ(v.Capacity(), SIZE*2);
        ASSERT_EQ(v[SIZE].id, ID);
        ASSERT_EQ(Obj::num_default_constructed, SIZE);
        ASSERT_EQ(Obj::num_copied, 1);
        ASSERT_EQ(Obj::num_constructed_with_id, 1);
        ASSERT_EQ(Obj::num_moved, SIZE);
    }
    ASSERT_EQ(Obj::GetAliveObjectCount(), 0);

    {
        Obj::ResetCounters();
        Vector<Obj> v(SIZE);
        v.PushBack(Obj{ID});
        ASSERT_EQ(v.Size(), SIZE + 1);
        ASSERT_EQ(v.Capacity(),SIZE*2);
        ASSERT_EQ(v[SIZE].id, ID);
        ASSERT_EQ(Obj::num_default_constructed, SIZE);
        ASSERT_EQ(Obj::num_copied, 0);
        ASSERT_EQ(Obj::num_constructed_with_id, 1);
        ASSERT_EQ(Obj::num_moved, SIZE + 1);
    }
    {
        Vector<TestObj> v(1);
        ASSERT_EQ(v.Size(), v.Capacity());
        // Операция PushBack существующего элемента вектора 
        // должна быть безопасна даже при реаллокации памяти
        v.PushBack(v[0]);
        ASSERT_TRUE(v[0].IsAlive());
        ASSERT_TRUE(v[1].IsAlive());
    }
    {
        Vector<TestObj> v(1);
        ASSERT_EQ(v.Size(), v.Capacity());
        // Операция PushBack для перемещения существующего элемента вектора 
        // должна быть безопасна даже при реаллокации памяти
        v.PushBack(std::move(v[0]));
        ASSERT_TRUE(v[0].IsAlive());
        ASSERT_TRUE(v[1].IsAlive());
    }
}

TEST(Vector, PopBack) {
    using namespace cstl;

    const size_t ID = 42;

    Obj::ResetCounters();
    Vector<Obj> v;
    v.PushBack(Obj{ID});
    v.PopBack();
    ASSERT_EQ(v.Size(), 0);
    ASSERT_EQ(v.Capacity(), 1);
    ASSERT_EQ(Obj::GetAliveObjectCount(), 0);
}

TEST(Vector, EmplaceBack) {
    using namespace std::literals;
    using namespace cstl;

    const int ID = 42;

    {
        Obj::ResetCounters();
        Vector<Obj> v;
        auto& elem = v.EmplaceBack(ID, "Ivan"s);
        ASSERT_EQ(v.Capacity(), 1);
        ASSERT_EQ(v.Size(), 1);
        ASSERT_EQ(&elem, &v[0]);
        ASSERT_EQ(v[0].id, ID);
        ASSERT_EQ(v[0].name, "Ivan"s);
        ASSERT_EQ(Obj::num_constructed_with_id_and_name, 1);
        ASSERT_EQ(Obj::GetAliveObjectCount(), 1);
    }

    ASSERT_EQ(Obj::GetAliveObjectCount(), 0);
    {
        Vector<TestObj> v(1);
        ASSERT_EQ(v.Size(), v.Capacity());
        // Операция EmplaceBack существующего элемента вектора должна быть безопасна
        // даже при реаллокации памяти
        v.EmplaceBack(v[0]);
        ASSERT_TRUE(v[0].IsAlive());
        ASSERT_TRUE(v[1].IsAlive());
    }
}

TEST(Vector, Iterators) {
    using namespace cstl;
    using namespace std::literals;

    const size_t SIZE = 10;

    Obj::ResetCounters();
    Vector<int> v(SIZE);
    const auto& cv(v);
    v.PushBack(1);
    ASSERT_EQ(&*v.begin(), &v[0]);
    *v.begin() = 2;
    ASSERT_EQ(v[0], 2);
    ASSERT_EQ(v.end() - v.begin(), static_cast<std::ptrdiff_t>(v.Size()));
    ASSERT_EQ(v.begin(), cv.begin());
    ASSERT_EQ(v.end(), cv.end());
    ASSERT_EQ(v.cbegin(), cv.begin());
    ASSERT_EQ(v.cend(), cv.end());
}

TEST(Vector, Insert) {
    using namespace cstl;
    using namespace std::literals;

    const size_t SIZE = 10;

    {
        Obj::ResetCounters();
        Vector<Obj> v{SIZE};
        Obj obj{1};
        Vector<Obj>::iterator pos = v.Insert(v.cbegin() + 1, obj);
        ASSERT_EQ(v.Size(), SIZE + 1);
        ASSERT_EQ(v.Capacity(), SIZE*2);
        ASSERT_EQ(&*pos, &v[1]);
        ASSERT_EQ(v[1].id, obj.id);
        ASSERT_EQ(Obj::num_copied, 1);
        ASSERT_EQ(Obj::num_default_constructed, SIZE);
        ASSERT_EQ(Obj::GetAliveObjectCount(), SIZE + 2);
    }
    {
        Obj::ResetCounters();
        Vector<Obj> v{SIZE};
        Vector<Obj>::iterator pos = v.Insert(v.cbegin() + 1, Obj{1});
        ASSERT_EQ(v.Size(), SIZE + 1);
        ASSERT_EQ(v.Capacity(), SIZE*2);
        ASSERT_EQ(&*pos, &v[1]);
        ASSERT_EQ(v[1].id, 1);
        ASSERT_EQ(Obj::num_copied, 0);
        ASSERT_EQ(Obj::num_default_constructed, SIZE);
        ASSERT_EQ(Obj::GetAliveObjectCount(), SIZE + 1);
    }
    {
        Vector<TestObj> v{SIZE};
        v.Insert(v.cbegin() + 2, v[0]);
        ASSERT_TRUE(std::all_of(v.begin(), v.end(), [](const TestObj& obj) {
            return obj.IsAlive();
        }));
    }
    {
        Vector<TestObj> v{SIZE};
        v.Insert(v.cbegin() + 2, std::move(v[0]));
        ASSERT_TRUE(std::all_of(v.begin(), v.end(), [](const TestObj& obj) {
            return obj.IsAlive();
        }));
    }
}

TEST(Vector, Erase) {
    using namespace cstl;

    const size_t SIZE = 10;
    const int ID = 42;

    Obj::ResetCounters();
    Vector<Obj> v{SIZE};
    v[2].id = ID;
    auto* pos = v.Erase(v.cbegin() + 1);

    ASSERT_EQ((pos - v.begin()), 1);
    ASSERT_EQ(v.Size(), SIZE - 1);
    ASSERT_EQ(v.Capacity(), SIZE);
    ASSERT_EQ(pos->id, ID);
    ASSERT_EQ(Obj::num_copied, 0);
    ASSERT_EQ(Obj::num_assigned, 0);
    ASSERT_EQ(Obj::num_move_assigned, SIZE - 2);
    ASSERT_EQ(Obj::num_moved, 0);
    ASSERT_EQ(Obj::GetAliveObjectCount(), SIZE - 1);
}

TEST(Vector, Emplace) {
    using namespace cstl;
    using namespace std::literals;

    const size_t SIZE = 10;
    const int ID = 42;

    {
        Obj::ResetCounters();
        Vector<Obj> v;
        auto* pos = v.Emplace(v.end(), Obj{1});
        ASSERT_EQ(v.Size(), 1);
        ASSERT_GE(v.Capacity(), v.Size());
        ASSERT_EQ(&*pos, &v[0]);
        ASSERT_EQ(Obj::num_moved, 1);
        ASSERT_EQ(Obj::num_constructed_with_id, 1);
        ASSERT_EQ(Obj::num_copied, 0);
        ASSERT_EQ(Obj::num_assigned, 0);
        ASSERT_EQ(Obj::num_move_assigned, 0);
        ASSERT_EQ(Obj::GetAliveObjectCount(), 1);
    }
    {
        Obj::ResetCounters();
        Vector<Obj> v;
        v.Reserve(SIZE);
        auto* pos = v.Emplace(v.end(), Obj{1});
        ASSERT_EQ(v.Size(), 1);
        ASSERT_GE(v.Capacity(), v.Size());
        ASSERT_EQ(&*pos, &v[0]);
        ASSERT_EQ(Obj::num_moved, 1);
        ASSERT_EQ(Obj::num_constructed_with_id, 1);
        ASSERT_EQ(Obj::num_copied, 0);
        ASSERT_EQ(Obj::num_assigned, 0);
        ASSERT_EQ(Obj::num_move_assigned, 0);
        ASSERT_EQ(Obj::GetAliveObjectCount(), 1);
    }
    {
        Vector<TestObj> v{SIZE};
        v.Emplace(v.cbegin() + 2, std::move(v[0]));
        ASSERT_TRUE(std::all_of(v.begin(), v.end(), [](const TestObj& obj) {
            return obj.IsAlive();
        }));
    }
    {
        Obj::ResetCounters();
        Vector<Obj> v{SIZE};
        auto* pos = v.Emplace(v.cbegin() + 1, ID, "Ivan"s);
        ASSERT_EQ(v.Size(), SIZE + 1);
        ASSERT_EQ(v.Capacity(), SIZE*2);
        ASSERT_EQ(&*pos, &v[1]);
        ASSERT_EQ(v[1].id, ID);
        ASSERT_EQ(v[1].name, "Ivan"s);
        ASSERT_EQ(Obj::num_copied, 0);
        ASSERT_EQ(Obj::num_default_constructed, SIZE);
        ASSERT_EQ(Obj::num_moved, SIZE);
        ASSERT_EQ(Obj::num_move_assigned, 0);
        ASSERT_EQ(Obj::num_assigned, 0);
        ASSERT_EQ(Obj::GetAliveObjectCount(), SIZE + 1);
    }
    {
        Obj::ResetCounters();
        Vector<Obj> v{SIZE};
        auto* pos = v.Emplace(v.cbegin() + v.Size(), ID, "Ivan"s);
        ASSERT_EQ(v.Size(), SIZE + 1);
        ASSERT_EQ(v.Capacity(), SIZE*2);
        ASSERT_EQ(&*pos, &v[SIZE]);
        ASSERT_EQ(v[SIZE].id, ID);
        ASSERT_EQ(v[SIZE].name, "Ivan"s);
        ASSERT_EQ(Obj::num_copied, 0);
        ASSERT_EQ(Obj::num_default_constructed, SIZE);
        ASSERT_EQ(Obj::num_moved, SIZE);
        ASSERT_EQ(Obj::num_move_assigned, 0);
        ASSERT_EQ(Obj::num_assigned, 0);
        ASSERT_EQ(Obj::GetAliveObjectCount(), SIZE + 1);
    }
    {
        Obj::ResetCounters();
        Vector<Obj> v{SIZE};
        v.Reserve(SIZE*2);
        const int old_num_moved = Obj::num_moved;
        ASSERT_EQ(v.Capacity(), SIZE*2);
        auto* pos = v.Emplace(v.cbegin() + 3, ID, "Ivan"s);
        ASSERT_EQ(v.Size(), SIZE + 1);
        ASSERT_EQ(&*pos, &v[3]);
        ASSERT_EQ(v[3].id, ID);
        ASSERT_EQ(v[3].name, "Ivan");
        ASSERT_EQ(Obj::num_copied, 0);
        ASSERT_EQ(Obj::num_default_constructed, SIZE);
        ASSERT_EQ(Obj::num_constructed_with_id_and_name, 1);
        ASSERT_EQ(Obj::num_moved, old_num_moved + 1);
        ASSERT_EQ(Obj::num_move_assigned, SIZE - 3);
        ASSERT_EQ(Obj::num_assigned, 0);
    }
    {
        Obj::ResetCounters();
        Vector<Obj> v{SIZE};
        v[2].id = ID;
        auto* pos = v.Erase(v.cbegin() + 1);
        ASSERT_EQ((pos - v.begin()), 1);
        ASSERT_EQ(v.Size(), SIZE - 1);
        ASSERT_EQ(v.Capacity(), SIZE);
        ASSERT_EQ(pos->id, ID);
        ASSERT_EQ(Obj::num_copied, 0);
        ASSERT_EQ(Obj::num_assigned, 0);
        ASSERT_EQ(Obj::num_move_assigned, SIZE - 2);
        ASSERT_EQ(Obj::num_moved, 0);
        ASSERT_EQ(Obj::GetAliveObjectCount(), SIZE - 1);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}