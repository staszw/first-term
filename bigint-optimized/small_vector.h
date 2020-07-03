#ifndef BIGINT_SMALL_VECTOR_H
#define BIGINT_SMALL_VECTOR_H

#include <shared_vector.h>

template<typename T>
struct small_vector {

    small_vector() noexcept: size_(0) {}

    small_vector(small_vector<T> const& other) :
            size_(other.size_) {
        if (is_small()) {
            std::copy(other.small, other.small + size_, small);
        } else {
            big = new shared_vector<T>(*other.big);
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
        if (!is_small()) {
            delete big;
        }
    }

    T& operator[](size_t i) noexcept {
        if (is_small()) {
            return small[i];
        } else {
            return (*big)[i];
        }
    }

    const T& operator[](size_t i) const noexcept {
        if (is_small()) {
            return small[i];
        } else {
            return (*big)[i];
        }
    }

    size_t size() const noexcept {
        return size_;
    }

    T const& back() const noexcept {
        if (is_small()) {
            return small[size_ - 1];
        } else {
            return big->back();
        }
    }

    void push_back(T const& value) {
        if (size_ == MAX_SIZE) {
            from_small_to_big();
            big->push_back(value);
        } else if (size_ < MAX_SIZE) {
            small[size_] = value;
        } else {
            big->push_back(value);
        }
        size_++;
    }

    void pop_back() {
        size_--;
        if (size_ == MAX_SIZE) {
            big->pop_back();
            from_big_to_small();
        } else if (size_ < MAX_SIZE) {
            small[size_].~T();
        } else {
            big->pop_back();
        }
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

    void resize(size_t n) {
        while (size_ < n && is_small()) {
            push_back(T());
        }
        if (size_ < n) {
            big->resize(n);
            size_ = n;
        }
    }

    T* begin() noexcept {
        if (is_small()) {
            return small;
        } else {
            return big->begin();
        }
    }

    T* end() noexcept {
        return begin() + size_;
    }

    const T* begin() const noexcept {
        if (is_small()) {
            return small;
        } else {
            return big->begin();
        }
    }

    const T* end() const noexcept {
        return begin() + size_;
    }

private:
    size_t size_;
    static const size_t MAX_SIZE = sizeof(shared_vector<T>*) / sizeof(T);
    union {
        shared_vector<T>* big;
        T small[MAX_SIZE];
    };

    bool is_small() const noexcept {
        return size_ <= MAX_SIZE;
    }

    void from_small_to_big() {
        T safe[MAX_SIZE];
        std::copy(small, small + MAX_SIZE, safe);
        big = new shared_vector<T>();
        big->resize(MAX_SIZE);
        for (size_t i = 0; i < MAX_SIZE; i++) {
            std::swap((*big)[i], safe[i]);
        }
    }

    void from_big_to_small() {
        shared_vector<T> safe(*big);
        delete big;
        for (size_t i = 0; i < MAX_SIZE; i++) {
            small[i] = safe[i];
        }
    }

    void swap_small_big(small_vector<T>& other) {
        shared_vector<T>* safe = other.big;
        std::copy(small, small + size_, other.small);
        big = safe;
    }

    void swap(small_vector<T>& other) {
        if (is_small() && other.is_small()) {
            std::swap(small, other.small);
        } else if (!is_small() && !other.is_small()) {
            std::swap(big, other.big);
        } else if (is_small()) {
            swap_small_big(other);
        } else {
            other.swap_small_big(*this);
        }
        std::swap(size_, other.size_);
    }
};

#endif //BIGINT_SMALL_VECTOR_H
