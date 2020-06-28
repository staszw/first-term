#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstddef>
#include <iosfwd>
#include <cstdint>
#include <vector>
#include <string>
#include <functional>

struct big_integer {
    big_integer();

    big_integer(big_integer const& other);

    big_integer(int a);

    big_integer(unsigned a);

    explicit big_integer(std::string const& str);

    ~big_integer();

    big_integer& operator=(big_integer const& other);

    big_integer& operator+=(big_integer const& rhs);

    big_integer& operator-=(big_integer const& rhs);

    big_integer& operator*=(big_integer const& rhs);

    big_integer& operator/=(big_integer const& rhs);

    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);

    big_integer& operator|=(big_integer const& rhs);

    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(int rhs);

    big_integer& operator>>=(int rhs);

    big_integer operator+() const;

    big_integer operator-() const;

    big_integer operator~() const;

    big_integer& operator++();

    big_integer operator++(int);

    big_integer& operator--();

    big_integer operator--(int);

    friend big_integer operator+(big_integer a, big_integer const& b);

    friend big_integer operator-(big_integer a, big_integer const& b);

    friend big_integer operator*(big_integer a, big_integer const& b);

    friend big_integer operator/(big_integer a, uint32_t b);

    friend big_integer operator/(big_integer a, int b);

    friend big_integer operator/(big_integer a, big_integer const& b);

    friend big_integer operator%(big_integer const& a, big_integer const& b);

    friend big_integer operator&(big_integer const& a, big_integer const& b);

    friend big_integer operator|(big_integer const& a, big_integer const& b);

    friend big_integer operator^(big_integer const& a, big_integer const& b);

    friend big_integer operator<<(big_integer a, unsigned int b);

    friend big_integer operator>>(big_integer a, unsigned int b);


    friend std::string to_string(big_integer const& a);

    friend int8_t compare(big_integer const& a, big_integer const& b);

    friend big_integer
    bit_operation(big_integer a, big_integer b, const std::function<uint32_t(uint32_t, uint32_t)>& func);

private:
    void to_bits();

    void from_bits();

    void normalize();

    std::vector<uint32_t> number;
    bool sign{};
    static const uint32_t element_length = 32;
};


big_integer operator+(big_integer a, big_integer const& b);

big_integer operator-(big_integer a, big_integer const& b);

big_integer operator*(big_integer a, big_integer const& b);

big_integer operator/(big_integer a, uint32_t b);

big_integer operator/(big_integer a, int b);

big_integer operator/(big_integer a, big_integer const& b);

big_integer operator%(const big_integer& a, big_integer const& b);

big_integer operator&(const big_integer& a, big_integer const& b);

big_integer operator|(const big_integer& a, big_integer const& b);

big_integer operator^(const big_integer& a, big_integer const& b);

big_integer operator<<(big_integer a, unsigned int b);

big_integer operator>>(big_integer a, unsigned int b);

int8_t compare(big_integer const& a, big_integer const& b);

bool operator==(big_integer const& a, big_integer const& b);

bool operator!=(big_integer const& a, big_integer const& b);

bool operator<(big_integer const& a, big_integer const& b);

bool operator>(big_integer const& a, big_integer const& b);

bool operator<=(big_integer const& a, big_integer const& b);

bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);

std::ostream& operator<<(std::ostream& s, big_integer const& a);

big_integer bit_operation(big_integer a, big_integer b, const std::function<uint32_t(uint32_t, uint32_t)>& func);

#endif // BIG_INTEGER_H
