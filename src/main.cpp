#include <cassert>
#include <cstddef>
#include <string>
#include <utility>

template <typename T>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const T& val, Node* next)
            : value(val)
            , next_node(next)
        {
        }

        T value;
        Node* next_node = nullptr;
    };

public:
    // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]] inline size_t GetSize() const noexcept {
        return size_;
    }

    // Сообщает, пустой ли список за время O(1)
    [[nodiscard]] inline bool IsEmpty() const noexcept {
        return !size_;
    }

private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_();
    size_t size_ = 0;
};

void Test0() {
    using namespace std;
    {
        const SingleLinkedList<int> empty_int_list;
        assert(empty_int_list.GetSize() == 0u);
        assert(empty_int_list.IsEmpty());
    }

    {
        const SingleLinkedList<string> empty_string_list;
        assert(empty_string_list.GetSize() == 0u);
        assert(empty_string_list.IsEmpty());
    }
}

int main() {
    Test0();
}