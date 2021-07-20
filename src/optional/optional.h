#include <stdexcept>
#include <utility>

namespace cstl {

class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
class Optional {
public:
    Optional() = default;

    Optional(const T& value) {
        new (data_) T(value);
        is_initialized_ = true;
    }

    Optional(T&& value) {
        new (data_) T(std::move(value));
        is_initialized_ = true;
    }

    Optional(const Optional& other) {
        if (other.is_initialized_) {
            new (data_) T(other.Value());
            is_initialized_ = true;
        }
    }

    Optional(Optional&& other) {
        if (other.is_initialized_) {
            new (data_) T(std::move(other.Value()));
            is_initialized_ = true;
        }
    }

    ~Optional() {
        Reset();
    }

    Optional& operator=(const T& rhs) {
        if (is_initialized_) {
            **this = rhs;
        } else {
            new (data_) T(rhs);
            is_initialized_ = true;
        }
        return *this;
    }

    Optional& operator=(T&& rhs) {
        if (is_initialized_) {
            **this = std::move(rhs);
        } else {
            new (data_) T(std::move(rhs));
            is_initialized_ = true;
        }
        return *this;
    }

    Optional& operator=(const Optional& rhs) {
        if (!rhs.is_initialized_) {
            Reset();
        } else if (is_initialized_ && rhs.is_initialized_) {
            **this = rhs.Value();
        } else {
            new (data_) T(rhs.Value());
            is_initialized_ = true;
        }
        return *this;
    }

    Optional& operator=(Optional&& rhs) {
        if (!rhs.is_initialized_) {
            Reset();
        } else if (is_initialized_ && rhs.is_initialized_) {
            **this = std::move(rhs.Value());
        } else {
            new (data_) T(std::move(rhs.Value()));
            is_initialized_ = true;
        }
        return *this;
    }

    inline bool HasValue() const {
        return is_initialized_;
    }

    inline T& operator*() {
        return *reinterpret_cast<T*>(data_);
    }

    inline const T& operator*() const {
        return *reinterpret_cast<const T*>(data_);
    }

    inline T* operator->() {
        return reinterpret_cast<T*>(data_);
    }

    inline const T* operator->() const {
        return reinterpret_cast<const T*>(data_);
    }

    T& Value() {
        if (is_initialized_)
            return *reinterpret_cast<T*>(data_);
        else
            throw BadOptionalAccess();
    }

    const T& Value() const {
        if (is_initialized_)
            return *reinterpret_cast<const T*>(data_);
        else
            throw BadOptionalAccess();
    }

    void Reset() {
        if (is_initialized_) {
            reinterpret_cast<T*>(data_)->~T();
            is_initialized_ = false;
        }
    }

private:
    alignas(T) char data_[sizeof(T)];
    bool is_initialized_ = false;
};

} // namespace cstl