#pragma once
#include <algorithm>
#include <optional>
#include <span>
#include <vector>

namespace cstl {

struct Shape {
    size_t rows = 0;
    size_t cols = 0;
};

template <typename Type>
class Matrix {
public:
    using Iterator = typename std::vector<Type>::iterator;
    using ConstIterator = typename std::vector<Type>::const_iterator;
    using Row = std::span<Type>;

public:
// ---------- Special Members ---------

    inline Matrix() noexcept = default;

    inline Matrix(const size_t rows, const size_t cols, const Type& value = {})
        : Matrix({rows, cols}, value) {}

    inline Matrix(const Matrix& other)
            : Matrix({other.shape_.rows, other.shape_.cols}, {}) {
        std::copy(other.elements_.begin(), other.elements_.end(),
                  elements_.begin());
        if (tr_elements_)
            std::copy(other.tr_elements_->begin(), other.tr_elements_->end(),
                      tr_elements_->begin());
    }

    explicit Matrix(const Shape shape, const Type& value = {})
        : shape_(shape)
        , elements_(shape.cols * shape.rows, value) {}

    inline Matrix(const size_t rows, const size_t cols, const Type* data)
            : Matrix(rows, cols) {
        std::copy(data, data + elements_.size(), elements_.begin());
    }

// ---------- Getters -----------------

    inline const Shape& GetShape() const noexcept {
        return shape_;
    }

    inline Type* GetData() noexcept {
        return elements_.data();
    }

    inline const Type* GetData() const noexcept {
        return elements_.data();
    }

    inline Row operator[](const size_t i) {
        return {
            &(*std::next(elements_.begin(), i * shape_.cols)),
            shape_.cols
        };
    }

// ---------- Methods -----------------

    inline void Swap(Matrix& other) noexcept {
        std::swap(shape_, other.shape_);
        elements_.swap(other.elements_);
        tr_elements_.swap(other.tr_elements_);
    }

    inline Matrix T() const {
        Matrix tr(*this);
        return tr.T();
    }

    [[maybe_unused]] inline Matrix& T() {
        return Transpose(elements_.begin(), elements_.end());
    }

    // Update transpose
    [[maybe_unused]] inline Matrix& Transpose() {
        Transpose(elements_.begin(), elements_.end(), tr_elements_->begin());
        return Transpose(elements_.begin(), elements_.end());
    }

    // Fill transpose
    template<typename InputIt>
    [[maybe_unused]] Matrix& Transpose(InputIt first, InputIt last) {
        if (tr_elements_) {
            std::swap(shape_.rows, shape_.cols);
            elements_.swap(*tr_elements_);
            return *this;
        }

        tr_elements_ = std::vector<Type>(elements_.size());
        Transpose(first, last, tr_elements_->begin());

        std::swap(shape_.rows, shape_.cols);
        elements_.swap(*tr_elements_);
        return *this;
    }

private:
    Shape shape_{};
    std::vector<Type> elements_{};
    std::optional<std::vector<Type>> tr_elements_ = std::nullopt;

    template<typename InputIt, typename OutputIt>
    void Transpose(InputIt first, InputIt last, OutputIt d_first) {
        const auto& index = [&](size_t x, size_t y) {
            return x + y * shape_.cols;
        };

        Iterator d_it = d_first;
        for (auto it = first; it != last; ++it) {
            const int pos = std::distance(first, it);
            *(d_it++) = *std::next(
                first,
                index(pos / shape_.rows, pos % shape_.rows)
            );
        }
    }
};

} // namespace cstl