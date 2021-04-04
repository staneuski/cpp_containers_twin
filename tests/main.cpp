#include <cassert>
#include <stdexcept>

#include <gtest/gtest.h>

#include "single_linked_list/single_linked_list.h"

// A spies that check their removal
struct DeletionSpy {
    DeletionSpy() = default;
    explicit DeletionSpy(int& instance_counter) noexcept
        : instance_counter_ptr_(&instance_counter)
    {
        OnAddInstance();
    }

    DeletionSpy(const DeletionSpy& other) noexcept
        : instance_counter_ptr_(other.instance_counter_ptr_)
    {
        OnAddInstance();
    }

    DeletionSpy& operator=(const DeletionSpy& rhs) noexcept {
        if (this != &rhs) {
            auto rhs_copy(rhs);
            std::swap(instance_counter_ptr_, rhs_copy.instance_counter_ptr_);
        }
        return *this;
    }

    ~DeletionSpy() {
        OnDeleteInstance();
    }

private:
    void OnAddInstance() noexcept {
        if (instance_counter_ptr_) {
            ++(*instance_counter_ptr_);
        }
    }

    void OnDeleteInstance() noexcept {
        if (instance_counter_ptr_) {
            assert(*instance_counter_ptr_ != 0);
            --(*instance_counter_ptr_);
        }
    }

    int* instance_counter_ptr_ = nullptr;
};

struct DeletionPtrSpy {
    ~DeletionPtrSpy() {
        if (deletion_counter_ptr) {
            ++(*deletion_counter_ptr);
        }
    }
    int* deletion_counter_ptr = nullptr;
};

// Helper class throwing an exception after N-copy is created
struct ThrowOnCopy {
    ThrowOnCopy() = default;
    explicit ThrowOnCopy(int& copy_counter) noexcept
        : countdown_ptr(&copy_counter) {
    }
    ThrowOnCopy(const ThrowOnCopy& other)
        : countdown_ptr(other.countdown_ptr)  //
    {
        if (countdown_ptr) {
            if (*countdown_ptr == 0) {
                throw std::bad_alloc();
            } else {
                --(*countdown_ptr);
            }
        }
    }

    // The assignment of elements of this type is not required
    ThrowOnCopy& operator=(const ThrowOnCopy& rhs) = delete;

    // Reverse counter.
    // If not nullptr, then decremented with each copy.
    // Once cleared, the copy constructor will throw an exception
    int* countdown_ptr = nullptr;
};

TEST(SingleLinkedList, EmptyIntList) {
    const SingleLinkedList<int> empty_int_list;

    ASSERT_EQ(empty_int_list.GetSize(), 0u);
    ASSERT_TRUE(empty_int_list.IsEmpty());
}

TEST(SingleLinkedList, EmptyStringList) {
    const SingleLinkedList<std::string> empty_string_list;

    ASSERT_EQ(empty_string_list.GetSize(), 0u);
    ASSERT_TRUE(empty_string_list.IsEmpty());
}

TEST(SingleLinkedList, IsEmpty) {
    SingleLinkedList<int> l;

    ASSERT_EQ(l.GetSize(), 0u);
    ASSERT_TRUE(l.IsEmpty());
}

TEST(SingleLinkedList, PushFront) {
    SingleLinkedList<int> l;
    l.PushFront(0);
    l.PushFront(1);

    ASSERT_EQ(l.GetSize(), 2);
    ASSERT_TRUE(!l.IsEmpty());
}

TEST(SingleLinkedList, Clear) {
    SingleLinkedList<int> l;
    l.PushFront(0);
    l.PushFront(1);

    l.Clear();

    ASSERT_EQ(l.GetSize(), 0);
    ASSERT_TRUE(l.IsEmpty());
}

TEST(SingleLinkedList, ClearSave) {
    int item0_counter = 0;
    int item1_counter = 0;
    int item2_counter = 0;

    {
        SingleLinkedList<DeletionSpy> list;
        list.PushFront(DeletionSpy{item0_counter});
        list.PushFront(DeletionSpy{item1_counter});
        list.PushFront(DeletionSpy{item2_counter});

        ASSERT_EQ(item0_counter, 1);
        ASSERT_EQ(item1_counter, 1);
        ASSERT_EQ(item2_counter, 1);
        list.Clear();
        ASSERT_EQ(item0_counter, 0);
        ASSERT_EQ(item1_counter, 0);
        ASSERT_EQ(item2_counter, 0);

        list.PushFront(DeletionSpy{item0_counter});
        list.PushFront(DeletionSpy{item1_counter});
        list.PushFront(DeletionSpy{item2_counter});
        ASSERT_EQ(item0_counter, 1);
        ASSERT_EQ(item1_counter, 1);
        ASSERT_EQ(item2_counter, 1);
    }

    ASSERT_EQ(item0_counter, 0);
    ASSERT_EQ(item1_counter, 0);
    ASSERT_EQ(item2_counter, 0);
}

TEST(SingleLinkedList, ThrowOnCopy) {
    bool exception_was_thrown = false;
    // Sequentially decrement the copy counter to zero until an exception is thrown
    for (int max_copy_counter = 5; max_copy_counter >= 0; --max_copy_counter) {
        SingleLinkedList<ThrowOnCopy> list;
        list.PushFront(ThrowOnCopy{});
        try {
            int copy_counter = max_copy_counter;
            list.PushFront(ThrowOnCopy(copy_counter));
            assert(list.GetSize() == 2);
        } catch (const std::bad_alloc&) {
            exception_was_thrown = true;
            // State of the list should remain the same after throwing an exception
            assert(list.GetSize() == 1);
            break;
        }

    }
    ASSERT_TRUE(exception_was_thrown);
}

TEST(SingleLinkedList, IteratingOverEmptyList) {
    SingleLinkedList<int> list;
    const auto& const_list = list;

    ASSERT_EQ(list.begin(), list.end());
    ASSERT_EQ(const_list.begin(), const_list.end());
    ASSERT_EQ(list.cbegin(), list.cend());
    ASSERT_EQ(list.cbegin(), const_list.begin());
    ASSERT_EQ(list.cend(), const_list.end());
}

TEST(SingleLinkedList, IteratingOverNonEmptyList) {
    SingleLinkedList<int> list;
    const auto& const_list = list;

    list.PushFront(1);
    EXPECT_EQ(list.GetSize(), 1u);
    EXPECT_TRUE(!list.IsEmpty());

    EXPECT_NE(const_list.begin(), const_list.end());
    EXPECT_NE(const_list.cbegin(), const_list.cend());
    EXPECT_NE(list.begin(), list.end());

    EXPECT_EQ(const_list.begin(), const_list.cbegin());

    EXPECT_EQ(*list.cbegin(), 1);
    *list.begin() = -1;
    EXPECT_EQ(*list.cbegin(), -1);

    const auto old_begin = list.cbegin();
    list.PushFront(2);
    EXPECT_EQ(list.GetSize(), 2);

    const auto new_begin = list.cbegin();
    EXPECT_NE(new_begin, old_begin);

    // Check preincrement
    {
        auto new_begin_copy(new_begin);
        ASSERT_EQ((++(new_begin_copy)), old_begin);
    }

    // Check postincrement
    {
        auto new_begin_copy(new_begin);
        ASSERT_EQ(((new_begin_copy)++), new_begin);
        ASSERT_EQ(new_begin_copy, old_begin);
    }

    // The iterator pointing to the position after the last element is equal
    // to the end() iterator
    {
        auto old_begin_copy(old_begin);
        ASSERT_EQ((++old_begin_copy), list.end());
    }
}

TEST(SingleLinkedList, IteratorsConvertion) {
    SingleLinkedList<int> list;
    list.PushFront(1);

    // Constructing ConstItrator from Iterator
    SingleLinkedList<int>::ConstIterator const_it(list.begin());
    ASSERT_EQ(const_it, list.cbegin());
    ASSERT_EQ(*const_it, *list.cbegin());

    SingleLinkedList<int>::ConstIterator const_it1;

    // Assignment Iterator's value to ConstIterator 
    const_it1 = list.begin();
    ASSERT_EQ(const_it1, const_it);
}

TEST(SingleLinkedList, IteratorArrowOperator) {
    SingleLinkedList<std::string> string_list;

    string_list.PushFront("one");
    EXPECT_EQ(string_list.cbegin()->length(), 3u);

    string_list.begin()->push_back('!');
    ASSERT_EQ(*string_list.begin(), "one!");
}

TEST(SingleLinkedList, ListsEquality) {
    SingleLinkedList<int> list_1;
    list_1.PushFront(1);
    list_1.PushFront(2);

    SingleLinkedList<int> list_2;
    list_2.PushFront(1);
    list_2.PushFront(2);
    list_2.PushFront(3);

    SingleLinkedList<int> list_1_copy;
    list_1_copy.PushFront(1);
    list_1_copy.PushFront(2);

    SingleLinkedList<int> empty_list;
    SingleLinkedList<int> another_empty_list;

    const std::string message_self_eq = "List must be equal to itself"
    ASSERT_EQ(list_1, list_1) << message_self_eq;
    ASSERT_EQ(empty_list, empty_list) << message_self_eq;

    const std::string message_ne = "Lists with different content mustn't be equal"
    ASSERT_NE(list_1, list_2) << message_ne;
    ASSERT_NE(list_2, list_1) << message_ne;

    const std::string message_eq = "Lists with the same content must be equal"
    ASSERT_EQ(list_1, list_1_copy) << message_eq;
    ASSERT_EQ(empty_list, another_empty_list) << message_eq;
}

TEST(SingleLinkedList, Swap) {
    SingleLinkedList<int> first;
    first.PushFront(1);
    first.PushFront(2);

    SingleLinkedList<int> second;
    second.PushFront(10);
    second.PushFront(11);
    second.PushFront(15);

    const auto old_first_begin = first.begin();
    const auto old_second_begin = second.begin();
    const auto old_first_size = first.GetSize();
    const auto old_second_size = second.GetSize();

    first.swap(second);

    ASSERT_EQ(first.begin(), old_second_begin);
    ASSERT_EQ(second.GetSize(), old_first_size);
    ASSERT_EQ(first.GetSize(), old_second_size);

    // Swap using the the swap function
    {
        using std::swap;
        const std::string message = "Using std::swap() instead of user swap()";

        // In the absence of a custom overload, the std::swap() function will
        // be called, which will perform the swap by creating a temporary copy
        swap(first, second);

        // If the exchange was performed with the creation of a temporary copy,
        // then the first.begin() iterator wouldn't be equal the previously
        // stored value, since the copy would stored its nodes at different
        // addresses
        ASSERT_EQ(first.begin(), old_first_begin) << message;
        ASSERT_EQ(second.begin(), old_second_begin) << message;
        ASSERT_EQ(first.GetSize(), old_first_size) << message;
        ASSERT_EQ(second.GetSize(), old_second_size) << message;
    }
}

TEST(SingleLinkedList, initializer_list) {
    SingleLinkedList<int> list{1, 2, 3, 4, 5};

    ASSERT_EQ(list.GetSize(), 5u);
    ASSERT_TRUE(!list.IsEmpty());
    ASSERT_TRUE(
        std::equal(list.begin(), list.end(), std::begin({1, 2, 3, 4, 5}))
    );
}

TEST(SingleLinkedList, LexicographicComparison) {
    using IntList = SingleLinkedList<int>;

    ASSERT_LT((IntList{1, 2, 3}), (IntList{1, 2, 3, 1}));
    ASSERT_LE((IntList{1, 2, 3}), (IntList{1, 2, 3}));
    ASSERT_GT((IntList{1, 2, 4}), (IntList{1, 2, 3}));
    ASSERT_GE((IntList{1, 2, 3}), (IntList{1, 2, 3}));
}

TEST(SingleLinkedList, CopyEmpty) {
    const SingleLinkedList<int> empty_list{};
    auto list_copy(empty_list);

    ASSERT_TRUE(list_copy.IsEmpty());
}

TEST(SingleLinkedList, Copy) {
    SingleLinkedList<int> non_empty_list{1, 2, 3, 4};
    const SingleLinkedList<int> empty_list{};
    auto list_copy(non_empty_list);

    ASSERT_NE(non_empty_list.begin(), list_copy.begin());
    ASSERT_EQ(list_copy, non_empty_list);
}

TEST(SingleLinkedList, Assignments) {
    const SingleLinkedList<int> source_list{1, 2, 3, 4};
    SingleLinkedList<int> receiver{5, 4, 3, 2, 1};
    receiver = source_list;

    ASSERT_NE(receiver.begin(), source_list.begin());
    ASSERT_EQ(receiver, source_list);
}

TEST(SingleLinkedList, SafeAssignments){
    SingleLinkedList<ThrowOnCopy> src_list;
    src_list.PushFront(ThrowOnCopy{});
    src_list.PushFront(ThrowOnCopy{});
    auto thrower = src_list.begin();
    src_list.PushFront(ThrowOnCopy{});

    int copy_counter = 0; // the first copy will throw an exception
    thrower->countdown_ptr = &copy_counter;

    SingleLinkedList<ThrowOnCopy> dst_list;
    dst_list.PushFront(ThrowOnCopy{});
    int dst_counter = 10;
    dst_list.begin()->countdown_ptr = &dst_counter;
    dst_list.PushFront(ThrowOnCopy{});

    try {
        dst_list = src_list;
        assert(false); // Expected exception on assignment
    } catch (const std::bad_alloc&) {
        // Check that the state of the target list has not changed
        // when throwing exceptions
        ASSERT_EQ(dst_list.GetSize(), 2);

        auto it = dst_list.begin();
        ASSERT_NE(it, dst_list.end());
        ASSERT_EQ(it->countdown_ptr, nullptr);

        ++it;

        ASSERT_NE(it, dst_list.end());
        ASSERT_EQ(it->countdown_ptr, &dst_counter);
        ASSERT_EQ(dst_counter, 10);
    } catch (...) {
        // No other types of exceptions expected
        assert(false);
    }
}

TEST(SingleLinkedList, PopFront) {
    SingleLinkedList<int> numbers{3, 14, 15, 92, 6};
    numbers.PopFront();
    ASSERT_EQ(numbers, (SingleLinkedList<int>{14, 15, 92, 6}));

    SingleLinkedList<DeletionPtrSpy> list;
    list.PushFront(DeletionPtrSpy{});
    int deletion_counter = 0;
    list.begin()->deletion_counter_ptr = &deletion_counter;
    ASSERT_EQ(deletion_counter, 0);

    list.PopFront();
    ASSERT_EQ(deletion_counter, 1);
}

TEST(SingleLinkedList, PopBack) {
    SingleLinkedList<int> numbers{3, 14, 15, 92, 6};
    numbers.PopBack();
    ASSERT_EQ(numbers, (SingleLinkedList<int>{3, 14, 15, 92}));

    SingleLinkedList<DeletionPtrSpy> list;
    list.PushFront(DeletionPtrSpy{});
    int deletion_counter = 0;
    list.begin()->deletion_counter_ptr = &deletion_counter;
    ASSERT_EQ(deletion_counter, 0);

    list.PopBack();
    ASSERT_EQ(deletion_counter, 1);
}

TEST(SingleLinkedList, IteratorBeforeBegin) {
    SingleLinkedList<int> empty_list;
    const auto& const_empty_list = empty_list;
    ASSERT_EQ(empty_list.before_begin(), empty_list.cbefore_begin());
    ASSERT_EQ(++empty_list.before_begin(), empty_list.begin());
    ASSERT_EQ(++empty_list.cbefore_begin(), const_empty_list.begin());

    SingleLinkedList<int> numbers{1, 2, 3, 4};
    const auto& const_numbers = numbers;
    ASSERT_EQ(numbers.before_begin(), numbers.cbefore_begin());
    ASSERT_EQ(++numbers.before_begin(), numbers.begin());
    ASSERT_EQ(++numbers.cbefore_begin(), const_numbers.begin());
}

TEST(SingleLinkedList, InsertAfterInEmptyList) {
    try {
        SingleLinkedList<int> lst;
        ASSERT_EQ(lst.cbegin(), lst.cend());

        const auto inserted_item_pos = lst.InsertAfter(lst.before_begin(), 123);
        ASSERT_EQ(lst, (SingleLinkedList<int>{123}));
        ASSERT_EQ(inserted_item_pos, lst.begin());
        ASSERT_EQ(*inserted_item_pos, 123);
    } catch (std::exception const& e) {
        std::cerr << e.what() << std::endl; 
    }
}

TEST(SingleLinkedList, InsertAfter) {
    try {
        SingleLinkedList<int> lst{1, 2, 3};
        auto inserted_item_pos = lst.InsertAfter(lst.before_begin(), 123);

        ASSERT_EQ(inserted_item_pos, lst.begin());
        ASSERT_NE(inserted_item_pos, lst.end());
        ASSERT_EQ(*inserted_item_pos, 123);
        ASSERT_EQ(lst, (SingleLinkedList<int>{123, 1, 2, 3}));

        inserted_item_pos = lst.InsertAfter(lst.begin(), 555);
        ASSERT_EQ(++SingleLinkedList<int>::Iterator(lst.begin()), inserted_item_pos);
        ASSERT_EQ(*inserted_item_pos, 555);
        ASSERT_EQ(lst, (SingleLinkedList<int>{123, 555, 1, 2, 3}));
    } catch (std::exception const& e) {
        std::cerr << e.what() << std::endl; 
    }
}

TEST(SingleLinkedList, StrictSafetyExceptionsAssurance) {
    bool exception_was_thrown = false;
    for (int max_copy_counter = 10; max_copy_counter >= 0; --max_copy_counter) {
        SingleLinkedList<ThrowOnCopy> list{ThrowOnCopy{}, ThrowOnCopy{}, ThrowOnCopy{}};
        try {
            int copy_counter = max_copy_counter;
            list.InsertAfter(list.cbegin(), ThrowOnCopy(copy_counter));
            assert(list.GetSize() == 4u);
        } catch (const std::bad_alloc&) {
            exception_was_thrown = true;
            assert(list.GetSize() == 3u);
            break;
        }
    }

    ASSERT_TRUE(exception_was_thrown);
}

TEST(SingleLinkedList, EraseAfterBeforeBegin) {
    SingleLinkedList<int> lst{1, 2, 3, 4};
    const auto& const_lst = lst;
    const auto item_after_erased = lst.EraseAfter(const_lst.cbefore_begin());

    ASSERT_EQ(item_after_erased, lst.begin());
    ASSERT_EQ(lst, (SingleLinkedList<int>{2, 3, 4}));
}

TEST(SingleLinkedList, EraseAfterBegin) {
    SingleLinkedList<int> lst{1, 2, 3, 4};
    const auto item_after_erased = lst.EraseAfter(lst.cbegin());

    ASSERT_EQ(lst, (SingleLinkedList<int>{1, 3, 4}));
    ASSERT_EQ(item_after_erased, (++lst.begin()));
}

TEST(SingleLinkedList, EraseAfter) {
    {
        SingleLinkedList<int> lst{1, 2, 3, 4};
        const auto item_after_erased = lst.EraseAfter(++(++lst.cbegin()));

        ASSERT_EQ(lst, (SingleLinkedList<int>{1, 2, 3}));
        ASSERT_EQ(item_after_erased, lst.end());
    }
    {
        SingleLinkedList<DeletionPtrSpy> list{DeletionPtrSpy{}, DeletionPtrSpy{}, DeletionPtrSpy{}};
        auto after_begin = ++list.begin();
        int deletion_counter = 0;
        after_begin->deletion_counter_ptr = &deletion_counter;
        ASSERT_EQ(deletion_counter, 0u);

        list.EraseAfter(list.cbegin());
        ASSERT_EQ(deletion_counter, 1u);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}