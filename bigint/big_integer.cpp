#include "big_integer.h"

#include <stdexcept>
#include <algorithm>
#include <cstddef>
#include <iosfwd>
#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>
#include <ostream>


void big_integer::normalize() {
    while (!number.empty() && number.back() == 0) {
        number.pop_back();
    }
    if (number.empty()) {
        sign = false;
    }
}

big_integer::big_integer() : number(), sign(false) {}

big_integer::big_integer(big_integer const &other) = default;

big_integer::big_integer(int a) : sign(a < 0), number() {
    if (a != 0) {
        number.push_back(abs(a));
    }
}

big_integer::big_integer(unsigned a) : sign(false), number() {
    if (a != 0) {
        number.push_back(a);
    }
}

big_integer::big_integer(std::string const &str) : big_integer() {
    if (str.empty()) {
        return;
    }
    bool csign = str[0] == '-';
    for (size_t i = csign; i < str.size(); i++) {
        *this *= 10;
        *this += str[i] - '0';
    }
    sign = (*this != 0 && csign);
}

big_integer::~big_integer() = default;

big_integer &big_integer::operator=(big_integer const &other) = default;

big_integer &big_integer::operator+=(big_integer const &rhs) {
    return operator=(*this + rhs);
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    return operator=(*this - rhs);
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    return operator=(*this * rhs);
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    return operator=(*this / rhs);
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    return operator=(*this % rhs);
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    return operator=(*this & rhs);
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    return operator=(*this | rhs);

}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    return operator=(*this ^ rhs);

}

big_integer &big_integer::operator<<=(int rhs) {
    return operator=(*this << rhs);
}

big_integer &big_integer::operator>>=(int rhs) {
    return operator=(*this >> rhs);
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    big_integer r(*this);
    if (r != 0) {
        r.sign = !r.sign;
    }
    return r;
}

big_integer big_integer::operator~() const {
    return -(*this + 1);
}

big_integer &big_integer::operator++() {
    return *this += 1;
}

big_integer big_integer::operator++(int) {
    big_integer r = *this;
    ++*this;
    return r;
}

big_integer &big_integer::operator--() {
    return *this -= 1;
}

big_integer big_integer::operator--(int) {
    big_integer r = *this;
    --*this;
    return r;
}

big_integer operator+(big_integer a, big_integer const &b) {
    if (a.sign && b.sign) {
        return -(-a + (-b));
    } else if (!a.sign && b.sign) {
        return a - (-b);
    } else if (a.sign && !b.sign) {
        return b - (-a);
    }
    uint32_t carry = 0;
    for (size_t i = 0; i < std::max(a.number.size(), b.number.size()); i++) {
        uint64_t x = a.number.size() > i ? a.number[i] : 0;
        uint64_t y = b.number.size() > i ? b.number[i] : 0;
        uint64_t sum = x + y + carry;
        uint32_t result = static_cast<uint32_t>(sum);
        carry = sum >> 32u;
        if (a.number.size() > i) {
            a.number[i] = result;
        } else {
            a.number.push_back(result);
        }
    }
    if (carry > 0) {
        a.number.push_back(carry);
    }
    return a;
}

big_integer operator-(big_integer a, big_integer const &b) {
    if (a.sign && b.sign) {
        return (-b) - (-a);
    } else if (a.sign && !b.sign) {
        return -(-a + b);
    } else if (!a.sign && b.sign) {
        return a + (-b);
    } else if (a < b) {
        return -(b - a);
    }

    uint32_t carry = 0;
    for (size_t i = 0; i < b.number.size(); i++) {
        uint64_t x = a.number[i];
        uint32_t y = b.number[i];
        uint64_t diff = x - y - carry;
        a.number[i] = static_cast<uint32_t>(diff);
        carry = diff >> 63u;
    }
    if (carry > 0) {
        a.number[b.number.size()] -= carry;
    }
    a.normalize();
    return a;
}

big_integer operator*(big_integer a, big_integer const &b) {
    if (a.sign != b.sign) {
        return -(-a * b);
    }
    big_integer result;
    result.number.resize(a.number.size() + b.number.size());
    for (size_t i = 0; i < a.number.size(); i++) {
        uint32_t carry = 0;
        uint64_t x = a.number[i];
        size_t j = 0;
        for (; j < b.number.size(); j++) {
            uint64_t y = b.number[j];
            uint64_t mul = result.number[i + j] + x * y + carry;
            result.number[i + j] = static_cast<uint32_t>(mul);
            carry = mul >> 32u;
        }
        while (carry > 0) {
            uint64_t mul = result.number[i + j] + carry;
            result.number[i + j] = static_cast<uint32_t>(mul);
            j++;
            carry = mul >> 32u;
        }
    }
    result.normalize();
    return result;
}

big_integer operator/(big_integer a, uint32_t const &b) {
    uint32_t carry = 0;
    for (ptrdiff_t i = a.number.size() - 1; i >= 0; i--) {
        uint64_t tmp = (static_cast<uint64_t>(carry) << 32u) + a.number[i];
        a.number[i] = static_cast<uint32_t>(tmp / b);
        carry = tmp % b;
    }
    a.normalize();
    return a;
}

big_integer operator/(big_integer a, int const &b) {
    big_integer result = a / static_cast<uint32_t>(abs(b));
    result.sign = (a.sign != (b < 0));
    result.normalize();
    return result;
}


big_integer operator/(big_integer a, big_integer const &b) {
    if (a.sign != b.sign) {
        return -((-a) / b);
    } else if (a.number.size() < b.number.size()) {
        return 0;
    } else if (b.number.size() == 1) {
        if (a.sign) {
            return (-a) / b.number[0];
        } else {
            return a / b.number[0];
        }
    }
    uint32_t f = static_cast<uint32_t>((1ull << 32u) / (static_cast<uint64_t> (b.number.back() + 1)));
    big_integer norm_a = a * f;
    big_integer norm_b = b * f;
    if (a.sign) {
        norm_a = -norm_a;
        norm_b = -norm_b;
    }
    size_t n = norm_b.number.size();
    size_t m = norm_a.number.size() - norm_b.number.size();
    big_integer result;
    result.number.resize(m);
    if (norm_a >= (norm_b << 32 * m)) {
        result.number.push_back(1);
        norm_a -= (norm_b << 32 * m);
    }
    for (ptrdiff_t i = m - 1; i >= 0; i--) {
        uint64_t x = n + i < norm_a.number.size() ? norm_a.number[n + i] : 0;
        uint64_t y = n + i - 1 < norm_a.number.size() ? norm_a.number[n + i - 1] : 0;
        uint64_t q64 = ((x << 32u) + y) / norm_b.number[n - 1];
        uint32_t q = UINT32_MAX;
        if (q64 < q) q = static_cast<uint32_t>(q64);
        big_integer big_q(q);
        norm_a -= ((big_q * norm_b) << (32 * i));
        while (norm_a < 0) {
            q--;
            norm_a += (norm_b << (32 * i));
        }
        result.number[i] = q;
        if (norm_a == 0) {
            break;
        }
    }
    result.normalize();
    return result;
}

big_integer operator%(const big_integer &a, big_integer const &b) {
    return a - (a / b) * b;
}

big_integer big_integer::to_bits() const {
    big_integer result(*this);
    if (sign) {
        result++;
        for (uint32_t &i : result.number) {
            i = ~i;
        }
    }
    return result;
}


big_integer big_integer::from_bits() const {
    big_integer result(*this);
    if (sign) {
        for (uint32_t &i : result.number) {
            i = ~i;
        }
        result--;
    }
    return result;
}


big_integer operator&(big_integer a, big_integer const &b) {
    a = a.to_bits();
    big_integer new_b = b.to_bits();
    uint32_t a_element = (a.sign || a.number.empty()) ? UINT32_MAX : 0;
    uint32_t b_element = (new_b.sign || new_b.number.empty()) ? UINT32_MAX : 0;
    while (a.number.size() < new_b.number.size()) {
        a.number.push_back(a_element);
    }
    while (a.number.size() > new_b.number.size()) {
        new_b.number.push_back(b_element);
    }
    for (size_t i = 0; i < a.number.size(); i++) {
        a.number[i] &= new_b.number[i];
    }
    a.normalize();
    a.sign &= b.sign;
    if (a.sign)
        return a.from_bits();
    return a;
}

big_integer operator|(big_integer a, big_integer const &b) {
    a = a.to_bits();
    big_integer new_b = b.to_bits();
    uint32_t a_element = (a.sign || a.number.empty()) ? UINT32_MAX : 0;
    uint32_t b_element = (new_b.sign || new_b.number.empty()) ? UINT32_MAX : 0;
    while (a.number.size() < new_b.number.size()) {
        a.number.push_back(a_element);
    }
    while (a.number.size() > new_b.number.size()) {
        new_b.number.push_back(b_element);
    }
    for (size_t i = 0; i < a.number.size(); i++) {
        a.number[i] |= new_b.number[i];
    }
    a.normalize();
    a.sign |= b.sign;
    if (a.sign)
        return a.from_bits();
    return a;
}

big_integer operator^(big_integer a, big_integer const &b) {
    a = a.to_bits();
    big_integer new_b = b.to_bits();
    uint32_t a_element = (a.sign || a.number.empty()) ? UINT32_MAX : 0;
    uint32_t b_element = (new_b.sign || new_b.number.empty()) ? UINT32_MAX : 0;
    while (a.number.size() < new_b.number.size()) {
        a.number.push_back(a_element);
    }
    while (a.number.size() > new_b.number.size()) {
        new_b.number.push_back(b_element);
    }
    for (size_t i = 0; i < a.number.size(); i++) {
        a.number[i] ^= new_b.number[i];
    }
    a.normalize();
    a.sign ^= b.sign;
    if (a.sign)
        return a.from_bits();
    return a;
}

big_integer operator<<(big_integer a, unsigned int b) {
    ptrdiff_t zeros = static_cast<ptrdiff_t>(b) >> 5u;
    uint32_t shift = static_cast<size_t>(b) & 31u;
    uint32_t carry = 0;
    for (uint32_t &i : a.number) {
        uint64_t tmp = (static_cast<uint64_t>( i) << shift);
        tmp |= carry;
        i = static_cast<uint32_t>(tmp);
        carry = (tmp >> 32u);
    }
    if (carry > 0) {
        a.number.push_back(carry);
    }
    a.number.resize(a.number.size() + zeros);
    for (ptrdiff_t i = a.number.size() - 1; i >= zeros; i--) {
        a.number[i] = a.number[i - zeros];
    }
    for (ptrdiff_t i = 0; i < zeros; i++) {
        a.number[i] = 0;
    }
    return a;
}

big_integer operator>>(big_integer a, unsigned int b) {
    size_t zeros = static_cast<size_t> (b) >> 5u;
    size_t shift = static_cast<size_t> (b) & 31u;
    uint32_t carry = 0;
    for (size_t i = 0; i < a.number.size() - zeros; i++) {
        a.number[i] = a.number[i + zeros];
    }
    for (size_t i = 0; i < zeros; i++)
        a.number.pop_back();
    for (ptrdiff_t i = a.number.size() - 1; i >= 0; i--) {
        uint64_t tmp = (static_cast<uint64_t>(a.number[i]) << (32 - shift));
        a.number[i] = ((tmp >> 32u) | carry);
        carry = static_cast<uint32_t>(tmp);
    }
    a.normalize();
    if (a.sign)
        a--;
    return a;
}

int8_t compare(const big_integer &a, const big_integer &b) {
    if (a.sign && !b.sign) {
        return -1;
    } else if (!a.sign && b.sign) {
        return 1;
    }
    int8_t result = 0;
    if (a.number.size() < b.number.size()) {
        result = -1;
    } else if (a.number.size() > b.number.size()) {
        result = 1;
    } else {
        for (ptrdiff_t i = a.number.size() - 1; i >= 0; i--) {
            if (a.number[i] < b.number[i]) {
                result = -1;
                break;
            } else if (a.number[i] > b.number[i]) {
                result = 1;
                break;
            }
        }
    }
    if (a.sign) {
        result = -result;
    }
    return result;
}

bool operator==(big_integer const &a, big_integer const &b) {
    return compare(a, b) == 0;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return compare(a, b) != 0;
}

bool operator<(big_integer const &a, big_integer const &b) {
    return compare(a, b) < 0;
}

bool operator>(big_integer const &a, big_integer const &b) {
    return compare(a, b) > 0;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return compare(a, b) <= 0;
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return compare(a, b) >= 0;
}

std::string to_string(big_integer const &a) {
    if (a == 0) {
        return "0";
    }
    std::string result;
    big_integer temp(a);
    while (temp != 0) {
        uint64_t digit = 0;
        for (ptrdiff_t i = temp.number.size() - 1; i >= 0; i--) {
            digit = ((digit << 32u) + temp.number[i]) % 10;
        }
        result += std::to_string(digit);
        temp = temp / 10;
    }
    if (a.sign) {
        result += '-';
    }
    std::reverse(result.begin(), result.end());
    return result;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}
