#ifndef VECTOR_H
#define VECTOR_H

#include <cstddef>
#include <algorithm>

template<typename T>
struct vector {
    using iterator = T*;
    using const_iterator = T const*;

    vector() noexcept: vector(0, 0, nullptr) {}

    vector(vector<T> const& other);

    vector& operator=(vector const& other);

    ~vector() {
        clear();
        operator delete(data_);
    }

    T& operator[](size_t i) noexcept {
        return data_[i];
    }

    T const& operator[](size_t i) const noexcept {
        return data_[i];
    }

    T* data() noexcept {
        return data_;
    }

    T const* data() const noexcept {
        return data_;
    }

    size_t size() const noexcept {
        return size_;
    }

    T& front() noexcept {
        return *begin();
    }

    T const& front() const noexcept {
        return *begin();
    }

    T& back() noexcept {
        return *(end() - 1);
    }

    T const& back() const noexcept {
        return *(end() - 1);
    }

    void push_back(T const& value);

    void pop_back() noexcept;

    bool empty() const noexcept {
        return size_ == 0;
    }

    size_t capacity() const noexcept {
        return capacity_;
    }

    void reserve(size_t n) {
        increase_capacity(next_power(n));
    }

    void shrink_to_fit();

    void clear() noexcept {
        while (!empty()) {
            pop_back();
        }
    }

    void swap(vector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    iterator begin() noexcept {
        return data_;
    }

    iterator end() noexcept {
        return data_ + size_;
    }

    const_iterator begin() const noexcept {
        return data_;
    }

    const_iterator end() const noexcept {
        return data_ + size_;
    }

    iterator insert(const_iterator it, T const& element);

    iterator erase(const_iterator it);

    iterator erase(const_iterator it1, const_iterator it2);

private:
    vector(size_t size, size_t capacity, T const* data);

    void increase_capacity(size_t new_capacity);

    size_t next_power(size_t n);

    T* data_;
    size_t size_;
    size_t capacity_;
};


template<typename T>
vector<T>::vector(size_t size, size_t capacity, T const* data) : size_(0), capacity_(capacity) {
    if (capacity != 0) {
        data_ = static_cast<T*>(operator new(capacity * sizeof(T)));
    } else {
        data_ = nullptr;
    }

    try {
        for (size_t i = 0; i < size; i++) {
            new(data_ + size_) T(data[i]);
            size_++;
        }
    }
    catch (...) {
        clear();
        operator delete(data_);
        throw;
    }
}

template<typename T>
size_t vector<T>::next_power(size_t n) {
    size_t result = 1;
    while (result <= n) {
        result *= 2;
    }
    return result;
}

template<typename T>
void vector<T>::increase_capacity(size_t capacity) {
    if (capacity <= capacity_) {
        return;
    }
    vector safe(size_, capacity, data_);
    swap(safe);
}

template<typename T>
vector<T>::vector(const vector& other) : vector(other.size_, other.size_, other.data_) {}

template<typename T>
vector<T>& vector<T>::operator=(vector const& other) {
    vector safe(other);
    swap(safe);
    return *this;
}

template<typename T>
void vector<T>::push_back(const T& value) {
    if (size_ == capacity_) {
        T safe = value;
        increase_capacity(next_power(capacity_));
        new(end()) T(safe);
    } else {
        new(end()) T(value);
    }
    size_++;
}

template<typename T>
void vector<T>::pop_back() noexcept {
    size_--;
    data_[size_].~T();
}

template<typename T>
void vector<T>::shrink_to_fit() {
    if (size_ == capacity_) {
        return;
    }
    vector safe(size_, size_, data_);
    swap(safe);
}

template<typename T>
typename vector<T>::iterator vector<T>::insert(const_iterator it, const T& elem) {
    size_t position = it - begin();
    push_back(elem);
    for (size_t i = size_ - 1; i > position; i--) {
        std::swap(data_[i], data_[i - 1]);
    }
    return begin() + position;
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(vector::const_iterator it) {
    return erase(it, it + 1);
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(const_iterator it1, const_iterator it2) {
    size_t first = it1 - begin();
    size_t length = it2 - it1;
    for (size_t i = first; i + length < size_; i++) {
        std::swap(data_[i], data_[i + length]);
    }
    for (size_t i = 0; i < length; i++) {
        pop_back();
    }
    return begin() + first;
}

#endif // VECTOR_H
