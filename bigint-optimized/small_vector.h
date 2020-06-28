#ifndef BIGINT_SMALL_VECTOR_H
#define BIGINT_SMALL_VECTOR_H

#include <shared_vector.h>

template<typename T>
struct small_vector {

    small_vector() noexcept {
        is_small = false;
        is_empty = true;
        big = nullptr;
        small = nullptr;
    }

    small_vector(small_vector<T> const& other) :
            is_small(other.is_small),
            is_empty(other.is_empty) {
        is_small = other.is_small;
        is_empty = other.is_empty;
        small = nullptr;
        big = nullptr;
        if (is_small) {
            small = new T(*other.small);
        } else if (!is_empty && !is_small) {
            big = new shared_vector<T>(*other.big);
        }
    }

    small_vector& operator=(small_vector const& other) {
        is_small = other.is_small;
        is_empty = other.is_empty;
        if (small == other.small && big == other.big) {
            return *this;
        }
        delete small;
        delete big;
        small = nullptr;
        big = nullptr;
        if (is_small) {
            small = new T(*other.small);
        } else if (!is_empty && !is_small) {
            big = new shared_vector<T>(*other.big);
        }
        return *this;
    }

    ~small_vector() {
        if (is_small) {
            delete small;
        }
        if (!is_small&& !is_empty) {
            delete big;
        }
    }

    T& operator[](size_t i) noexcept {
        assert(!is_empty);
        if (is_small && i == 0) {
            return *small;
        } else {
            return big->operator[](i);
        }
    }

    const T& operator[](size_t i) const noexcept {
        assert(!is_empty);
        if (is_small && i == 0) {
            return *small;
        } else {
            return big->operator[](i);
        }
    }

    size_t size() const noexcept {
        if (is_empty) {
            return 0;
        } else if (is_small) {
            return 1;
        } else {
            return big->size();
        }
    }

    T const& back() const noexcept {
        assert(size() > 0);
        return operator[](size() - 1);
    }

    void push_back(T const& value) {
        if (is_empty) {
            is_empty = 0;
            is_small = 1;
            small = new T(value);
        } else if (is_small) {
            from_small_to_big();
            big->push_back(value);
        } else {
            big->push_back(value);
        }
    }

    void pop_back() {
        assert(!is_empty);
        if (is_small) {
            delete small;
            is_small = 0;
            is_empty = 1;
        } else {
            big->pop_back();
            if (big->size() == 1) {
                from_big_to_small();
            }
        }
    }

    bool empty() const noexcept {
        return size() == 0;
    }

    void resize(size_t n) {
        if (n <= size()) return;
        if (is_empty) {
            is_empty = 0;
            small = new T();
            if (n == 1) {
                is_small = 1;
            } else {
                from_small_to_big();
                big->resize(n);
            }
        } else if (is_small) {
            from_small_to_big();
            big->resize(n);
        } else {
            big->resize(n);
        }
    }

    T* begin() noexcept {
        if (is_empty) {
            return nullptr;
        } else if (is_small) {
            return small;
        } else {
            return big->begin();
        }
    }

    T* end() noexcept {
        if (is_empty) {
            return nullptr;
        } else if (is_small) {
            return small + 1;
        } else {
            return big->end();
        }
    }

private:
    bool is_small;
    bool is_empty;
    shared_vector<T>* big;
    T* small;

    void from_small_to_big() {
        is_small = 0;
        big = new shared_vector<T>();
        big->push_back(*small);
        delete small;
    }

    void from_big_to_small() {
        small = new T(big->back());
        delete big;
        is_small = 1;
    }
};

#endif //BIGINT_SMALL_VECTOR_H
