#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstddef>
#include <iosfwd>
#include <cstdint>
#include <vector>
#include <string>

struct big_integer
{
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
    friend big_integer operator/(big_integer a, uint32_t const& b);
    friend big_integer operator/(big_integer a, int const& b);
    friend big_integer operator/(big_integer a, big_integer const& b);
    friend big_integer operator%(const big_integer& a, big_integer const& b);

    friend big_integer operator&(big_integer a, big_integer const& b);
    friend big_integer operator|(big_integer a, big_integer const& b);
    friend big_integer operator^(big_integer a, big_integer const& b);

    friend big_integer operator<<(big_integer a, unsigned int b);
    friend big_integer operator>>(big_integer a, unsigned int b);


    friend std::string to_string(big_integer const& a);
    friend int8_t compare(big_integer const& a, big_integer const& b);


private:
    big_integer to_bits() const;
    big_integer from_bits() const;
    void normalize();
    std::vector<uint32_t> number;
    bool sign{};
};

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, uint32_t const& b);
big_integer operator/(big_integer a, int const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(const big_integer& a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

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

#endif // BIG_INTEGER_H
