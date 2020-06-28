#include "big_integer.h"

static uint32_t get_32_low_bits(uint64_t a) {
    return a & UINT32_MAX;
}

static uint32_t get_32_high_bits(uint64_t a) {
    return a >> big_integer::ELEMENT_LENGTH;
}

static bool get_sign(uint64_t a) {
    return a >> (2 * big_integer::ELEMENT_LENGTH - 1);
}

static uint64_t shift_from_low(uint32_t a) {
    return static_cast<uint64_t>(a) << big_integer::ELEMENT_LENGTH;
}

void big_integer::normalize() {
    while (!number.empty() && number.back() == 0) {
        number.pop_back();
    }
    if (number.empty()) {
        sign = false;
    }
}

big_integer::big_integer() : number(), sign(false) {}

big_integer::big_integer(big_integer const& other) = default;

big_integer::big_integer(int a) : number(), sign(a < 0) {
    if (a == INT32_MIN) {
        set_nth(0, 1u<<(ELEMENT_LENGTH - 1));
    } else if (a != 0) {
        set_nth(0, abs(a));
    }
}

big_integer::big_integer(unsigned a) : number(), sign(false) {
    if (a != 0) {
        set_nth(0, a);
    }
}

big_integer::big_integer(std::string const& str) : big_integer() {
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

big_integer& big_integer::operator=(big_integer const& other) = default;

big_integer& big_integer::operator+=(big_integer const& rhs) {
    return operator=(*this + rhs);
}

big_integer& big_integer::operator-=(big_integer const& rhs) {
    return operator=(*this - rhs);
}

big_integer& big_integer::operator*=(big_integer const& rhs) {
    return operator=(*this * rhs);
}

big_integer& big_integer::operator/=(big_integer const& rhs) {
    return operator=(*this / rhs);
}

big_integer& big_integer::operator%=(big_integer const& rhs) {
    return operator=(*this % rhs);
}

big_integer& big_integer::operator&=(big_integer const& rhs) {
    return operator=(*this & rhs);
}

big_integer& big_integer::operator|=(big_integer const& rhs) {
    return operator=(*this | rhs);

}

big_integer& big_integer::operator^=(big_integer const& rhs) {
    return operator=(*this ^ rhs);

}

big_integer& big_integer::operator<<=(int rhs) {
    return operator=(*this << rhs);
}

big_integer& big_integer::operator>>=(int rhs) {
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

big_integer& big_integer::operator++() {
    return *this += 1;
}

big_integer big_integer::operator++(int) {
    big_integer r = *this;
    ++*this;
    return r;
}

big_integer& big_integer::operator--() {
    return *this -= 1;
}

big_integer big_integer::operator--(int) {
    big_integer r = *this;
    --*this;
    return r;
}

big_integer operator+(big_integer a, big_integer const& b) {
    if (a.sign && b.sign) {
        return -(-a + (-b));
    } else if (!a.sign && b.sign) {
        return a - (-b);
    } else if (a.sign && !b.sign) {
        return b - (-a);
    }
    uint32_t carry = 0;
    for (size_t i = 0; i < std::max(a.number.size(), b.number.size()); i++) {
        uint64_t x = a.get_nth(i);
        uint64_t y = b.get_nth(i);
        uint64_t sum = x + y + carry;
        uint32_t result = get_32_low_bits(sum);
        carry = get_32_high_bits(sum);
        a.set_nth(i, result);
    }
    if (carry > 0) {
        a.number.push_back(carry);
    }
    return a;
}

big_integer operator-(big_integer a, big_integer const& b) {
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
    for (size_t i = 0; i < std::max(a.number.size(), b.number.size()); i++) {
        uint64_t x = a.get_nth(i);
        uint32_t y = b.get_nth(i);
        uint64_t diff = x - y - carry;
        a.set_nth(i, get_32_low_bits(diff));
        carry = get_sign(diff);
    }
    a.normalize();
    return a;
}

big_integer operator*(big_integer a, big_integer const& b) {
    if (a.sign != b.sign) {
        return -(-a * b);
    }
    big_integer result;
    for (size_t i = 0; i < a.number.size(); i++) {
        uint32_t carry = 0;
        uint64_t x = a.number[i];
        for (size_t j = 0; j < b.number.size() || carry > 0; j++) {
            uint64_t y = j < b.number.size() ? b.number[j] : 0;
            uint64_t mul = result.get_nth(i + j) + x * y + carry;
            result.set_nth(i + j, get_32_low_bits(mul));
            carry = get_32_high_bits(mul);
        }
    }
    result.normalize();
    return result;
}

big_integer operator/(big_integer a, uint32_t b) {
    uint32_t carry = 0;
    for (ptrdiff_t i = a.number.size() - 1; i >= 0; i--) {
        uint64_t tmp = shift_from_low(carry) + a.number[i];
        a.number[i] = get_32_low_bits(tmp / b);
        carry = tmp % b;
    }
    a.normalize();
    return a;
}

big_integer operator/(big_integer a, int b) {
    return a / big_integer(b);
}


big_integer operator/(big_integer a, big_integer const& b) {
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
    uint32_t f = static_cast<uint32_t>(shift_from_low(1) / (static_cast<uint64_t> (b.number.back() + 1)));
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
        uint64_t x = norm_a.get_nth(n + i);
        uint64_t y = norm_a.get_nth(n + i - 1);
        uint64_t q64 = (shift_from_low(x) + y) / norm_b.number[n - 1];
        uint32_t q = UINT32_MAX;
        if (q64 < q) q = get_32_low_bits(q64);
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

big_integer operator%(const big_integer& a, big_integer const& b) {
    return a - (a / b) * b;
}

void big_integer::to_bits() {
    if (sign) {
        sign = false;
        operator--();
        for (uint32_t& i : number) {
            i = ~i;
        }
        sign = true;
    }
}


void big_integer::from_bits() {
    if (sign) {
        sign = false;
        for (uint32_t& i : number) {
            i = ~i;
        }
        operator++();
        sign = true;
    }
}

big_integer bit_operation(big_integer a, big_integer b, const big_integer::func& func) {
    a.to_bits();
    b.to_bits();
    for (size_t i = 0; i < a.number.size(); i++) {
        a.set_nth(i, func(a.get_nth(i), b.get_nth(i)));
    }
    a.normalize();
    a.sign = func(a.sign, b.sign);
    if (a.sign) {
        a.from_bits();
    }
    return a;
}

big_integer operator&(const big_integer& a, big_integer const& b) {
    return bit_operation(a, b, [](uint32_t a, uint32_t b) { return a & b; });
}

big_integer operator|(const big_integer& a, big_integer const& b) {
    return bit_operation(a, b, [](uint32_t a, uint32_t b) { return a | b; });
}

big_integer operator^(const big_integer& a, big_integer const& b) {
    return bit_operation(a, b, [](uint32_t a, uint32_t b) { return a ^ b; });
}

big_integer operator<<(big_integer a, unsigned int b) {
    ptrdiff_t zeros = static_cast<ptrdiff_t>(b) >> 5u;
    uint32_t shift = static_cast<size_t>(b) & 31u;
    uint32_t carry = 0;
    for (uint32_t& i : a.number) {
        uint64_t tmp = (static_cast<uint64_t>( i) << shift);
        tmp |= carry;
        i = get_32_low_bits(tmp);
        carry = get_32_high_bits(tmp);
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
        a.number[i] = (get_32_high_bits(tmp) | carry);
        carry = get_32_low_bits(tmp);
    }
    a.normalize();
    if (a.sign)
        a--;
    return a;
}

int8_t compare(const big_integer& a, const big_integer& b) {
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

bool operator==(big_integer const& a, big_integer const& b) {
    return compare(a, b) == 0;
}

bool operator!=(big_integer const& a, big_integer const& b) {
    return compare(a, b) != 0;
}

bool operator<(big_integer const& a, big_integer const& b) {
    return compare(a, b) < 0;
}

bool operator>(big_integer const& a, big_integer const& b) {
    return compare(a, b) > 0;
}

bool operator<=(big_integer const& a, big_integer const& b) {
    return compare(a, b) <= 0;
}

bool operator>=(big_integer const& a, big_integer const& b) {
    return compare(a, b) >= 0;
}

std::string to_string(big_integer const& a) {
    if (a == 0) {
        return "0";
    }
    std::string result;
    big_integer temp(a);
    while (temp != 0) {
        uint64_t digit = 0;
        for (ptrdiff_t i = temp.number.size() - 1; i >= 0; i--) {
            digit = (shift_from_low(digit) + temp.number[i]) % 10;
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

uint32_t big_integer::get_nth(size_t i) const {
    if (i < number.size()) {
        return number[i];
    }
    return sign ? UINT32_MAX : 0;
}

void big_integer::set_nth(size_t i, uint32_t value) {
    if (i < number.size()) {
        number[i] = value;
    } else {
        number.push_back(value);
    }
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
    return s << to_string(a);
}
