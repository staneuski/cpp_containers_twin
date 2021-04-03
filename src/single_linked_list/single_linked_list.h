#pragma once
#include <cassert>
#include <cstddef>
#include <experimental/iterator>
#include <iostream>
#include <stdexcept>

template <typename Type>
class SingleLinkedList {
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next)
        {
        }

        Type value;
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;

        explicit BasicIterator(Node* node) 
            : node_(node)
        {
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept {
            node_ = other.node_;
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] inline bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] inline bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        [[nodiscard]] inline bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] inline bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        BasicIterator& operator++() noexcept {
            if (node_)
                node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            BasicIterator old_value(*this);
            ++(*this);
            return old_value;
        }

        [[nodiscard]] inline reference operator*() const noexcept {
            return node_->value;
        }

        [[nodiscard]] inline pointer operator->() const noexcept {
            return &(node_->value);
        }

    private:
        Node* node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    /* --------------------- Constructors & Destructor --------------------- */

    SingleLinkedList() = default;

    SingleLinkedList(std::initializer_list<Type> values) {
        assert(size_ == 0 && head_.next_node == nullptr);
        for (auto it = std::rbegin(values); it != std::rend(values); ++it)
            PushFront(*it);
    }

    ~SingleLinkedList() {
        Clear();
    }

    /* -------------- Copy constructor & assignation operator -------------- */

    SingleLinkedList(const SingleLinkedList& other) {
        assert(size_ == 0 && head_.next_node == nullptr);
        SingleLinkedList tmp;
        for (const Type& value : other)
            tmp.PushFront(value);
        swap(tmp);
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this != &rhs) {
            SingleLinkedList rhs_copy(rhs);
            swap(rhs_copy);
        }
        return *this;
    }

    /* ----------------------------- Iterators ----------------------------- */

    [[nodiscard]] inline Iterator before_begin() noexcept {
        return Iterator{&head_};
    }

    [[nodiscard]] inline ConstIterator before_begin() const noexcept {
        return cbefore_begin;
    }

    [[nodiscard]] inline ConstIterator cbefore_begin() const noexcept {
        return ConstIterator{const_cast<Node*>(&head_)};
    }

    [[nodiscard]] inline Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }

    [[nodiscard]] inline ConstIterator begin() const noexcept {
        return cbegin();
    }

    [[nodiscard]] inline ConstIterator cbegin() const noexcept {
        return ConstIterator{head_.next_node};
    }

    [[nodiscard]] inline Iterator end() noexcept {
        return Iterator{nullptr};
    }

    [[nodiscard]] inline ConstIterator end() const noexcept {
        return cend();
    }

    [[nodiscard]] inline ConstIterator cend() const noexcept {
        return ConstIterator{nullptr};
    }

    /* -------------------------- List's methods --------------------------- */

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    [[nodiscard]] inline size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] inline bool IsEmpty() const noexcept {
        return !(head_.next_node && size_);
    }

    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    inline void PushBack(const Type& value) {
        InsertAfter(++GetPositionBeforeBack(), value);
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        if (!pos.node_)
            throw std::invalid_argument("pos argument points to nullptr");

        pos.node_->next_node = new Node(value, pos.node_->next_node);
        ++size_;
        return Iterator{pos.node_->next_node};
    }

    void PopFront() noexcept {
        if (!IsEmpty()) {
            Node* next_node = head_.next_node->next_node;
            delete head_.next_node;

            head_.next_node = next_node;
            --size_;
        }
    }

    inline void PopBack() {
        EraseAfter(GetPositionBeforeBack());
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        if (!pos.node_)
            return Iterator{pos.node_};

        Node* to_erase = pos.node_->next_node;
        pos.node_->next_node = to_erase->next_node;
        delete to_erase;

        --size_;
        return Iterator{pos.node_->next_node};
    }

    void Clear() noexcept {
        while (size_ && head_.next_node)
            PopFront();
    }

private:
    Node head_ = Node();
    size_t size_ = 0;

    Iterator GetPositionBeforeBack() {
        Iterator before_back = before_begin();
        for (size_t i = 1u; i < size_; ++i)
            ++before_back;
        return before_back;
    }
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (lhs.GetSize() == rhs.GetSize()) && std::equal(lhs.begin(), lhs.begin(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (lhs.GetSize() != rhs.GetSize()) || std::equal(lhs.begin(), lhs.begin(), rhs.begin());
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(
        lhs.begin(), lhs.end(),
        rhs.begin(), rhs.end()
    );
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (lhs < rhs) || (lhs == rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs) || (lhs == rhs);
}

template <typename Type>
std::ostream& operator<<(std::ostream& out, const SingleLinkedList<Type>& list) {
    out << "[(";
    std::copy(
        list.cbegin(), list.cend(),
        std::experimental::make_ostream_joiner(out, ")->(")
    );
    return out << ")]";
}