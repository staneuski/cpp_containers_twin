#pragma once

class SizeObjWrapper {
public:
    SizeObjWrapper() noexcept = default;

    explicit SizeObjWrapper(size_t value) : value_(value) {}

    inline size_t Get() const noexcept {
        return value_;
    }

private:
    size_t value_{};
};

inline SizeObjWrapper Reserve(size_t capacity_to_reserve) {
    return SizeObjWrapper(capacity_to_reserve);
}