#ifndef BIGINT_SHARED_VECTOR_H
#define BIGINT_SHARED_VECTOR_H

#include <vector>
#include <cassert>

template<typename T>
struct shared_vector {
    shared_vector() noexcept {
        inner = new inner_vector(std::vector<T>());
    }

    shared_vector(shared_vector<T> const& other) : inner(other.inner) {
        inner->copies++;
    }

    shared_vector(T* begin, T* end) {
        inner = new inner_vector(std::vector<T>(begin, end));
    }

    shared_vector& operator=(shared_vector const& other) {
        if (inner != other.inner) {
            destroy();
            inner = other.inner;
            inner->copies++;
        }
        return *this;
    }

    ~shared_vector() {
        destroy();
    }

    T& operator[](size_t i) noexcept {
        copy_on_write();
        return inner->vect[i];
    }

    const T& operator[](size_t i) const noexcept {
        return inner->vect[i];
    }

    size_t size() const noexcept {
        return inner->vect.size();
    }

    T const& back() const noexcept {
        return inner->vect.back();
    }

    void push_back(T const& value) {
        copy_on_write();
        inner->vect.push_back(value);
    }

    void pop_back() {
        copy_on_write();
        inner->vect.pop_back();
    }

    bool empty() const noexcept {
        return size() == 0;
    }

    void resize(size_t n) {
        copy_on_write();
        inner->vect.resize(n);
    }

    T* begin() noexcept {
        copy_on_write();
        return inner->vect.data();
    }

    T* end() noexcept {
        copy_on_write();
        return begin() + size();
    }

    const T* begin() const noexcept {
        return inner->vect.data();
    }

    const T* end() const noexcept {
        return begin() + size();
    }

private:
    struct inner_vector {
        std::vector<T> vect;
        size_t copies;

        inner_vector() : vect(), copies(1) {}

        explicit inner_vector(std::vector<T> const& other) : vect(other), copies(1) {}

        ~inner_vector() = default;
    };

    inner_vector* inner;

    void copy_on_write() {
        if (inner->copies > 1) {
            inner->copies--;
            inner = new inner_vector(inner->vect);
        }
    }

    void destroy() {
        inner->copies--;
        if (inner->copies == 0) {
            delete inner;
        }
    }
};

#endif //BIGINT_SHARED_VECTOR_H
