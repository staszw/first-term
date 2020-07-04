#ifndef BIGINT_SMALL_VECTOR_H
#define BIGINT_SMALL_VECTOR_H

#include <shared_vector.h>

template<typename T>
struct small_vector {

    small_vector() noexcept: is_big(false), size_(0) {}

    small_vector(small_vector<T> const& other) :
            is_big(other.is_big), size_(other.size_) {
        if (is_small()) {
            copy_small(other.small, small, size_);
        } else {
            new (&big) shared_vector<T>(other.big);
        }
    }

    small_vector& operator=(small_vector const& other) {
        if (this != &other) {
            small_vector<T> safe(other);
            swap(safe);
        }
        return *this;
    }

    ~small_vector() {
        if (is_small()) {
            clear_small(small, size_);
        } else {
            big.~shared_vector();
        }
    }

    T& operator[](size_t i) noexcept {
        if (is_small()) {
            return small[i];
        } else {
            return big[i];
        }
    }

    const T& operator[](size_t i) const noexcept {
        if (is_small()) {
            return small[i];
        } else {
            return big[i];
        }
    }

    size_t size() const noexcept {
        return size_;
    }

    T const& back() const noexcept {
        if (is_small()) {
            return small[size_ - 1];
        } else {
            return big.back();
        }
    }

    void push_back(T const& value) {
        if (is_small() && size_ == MAX_SIZE) {
            from_small_to_big();
            big.push_back(value);
        } else if (is_small()) {
            small[size_] = value;
        } else {
            big.push_back(value);
        }
        size_++;
    }

    void pop_back() {
        size_--;
        if (is_small()) {
            small[size_].~T();
        } else {
            big.pop_back();
        }
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

    void resize(size_t n) {
        if (size_ < n) {
            if (is_small() && n > MAX_SIZE) {
                from_small_to_big();
            }

            if (is_small()) {
                while (size_ < n) {
                    new (small +  size_) T();
                    size_++;
                }
            } else {
                big.resize(n);
                size_ = n;
            }
        }
    }

    T* begin() noexcept {
        if (is_small()) {
            return small;
        } else {
            return big.begin();
        }
    }

    T* end() noexcept {
        return begin() + size_;
    }

    const T* begin() const noexcept {
        if (is_small()) {
            return small;
        } else {
            return big.begin();
        }
    }

    const T* end() const noexcept {
        return begin() + size_;
    }

private:
    bool is_big;
    size_t size_;
    static constexpr size_t MAX_SIZE = sizeof(shared_vector<T>) / sizeof(T);
    union {
        shared_vector<T> big;
        T small[MAX_SIZE];
    };

    bool is_small() const noexcept {
        return !is_big;
    }

    void from_small_to_big() {
        shared_vector<T> new_big(small, small + size_);
        clear_small(small, size_);
        new (&big) shared_vector<T>(new_big);
        is_big = true;
    }

    void swap_small_big_data(small_vector<T>& other) {
        shared_vector<T> safe(other.big);
        other.big.~shared_vector();
        copy_small(small, other.small, size_);
        new (&big) shared_vector<T>(safe);
    }

    void swap(small_vector<T>& other) {
        if (is_small() && other.is_small()) {
            std::swap(small, other.small);
        } else if (!is_small() && !other.is_small()) {
            std::swap(big, other.big);
        } else if (is_small()) {
            swap_small_big_data(other);
        } else {
            other.swap_small_big_data(*this);
        }
        std::swap(size_, other.size_);
        std::swap(is_big, other.is_big);
    }

    void copy_small(T const* source, T* destination, size_t const& count) {
        size_t done = 0;
        try {
            for (size_t i = 0; i < count; i++) {
                new (destination + i) T(source[i]);
                done++;
            }
        } catch (...) {
            clear_small(destination, done);
            throw;
        }
    }

    void clear_small(T* where, size_t const& count) {
        for (size_t i = 0; i < size_; i++) {
            where[i].~T();
        }
    }
};

#endif //BIGINT_SMALL_VECTOR_H
