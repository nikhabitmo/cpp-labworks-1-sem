#pragma once
#include <cinttypes>
#include <iostream>

struct uint2022_t {
    static const uint32_t BASE = 1000000000;
    static const uint32_t SIZE = 68;
    uint32_t digital_len = 0;
    uint32_t digits_cnt = 0;
    uint32_t array_digits[SIZE] = {0};
};

static_assert(sizeof(uint2022_t) <= 300, "Size of uint2022_t must be no higher than 300 bytes");

uint2022_t from_uint(uint32_t i);

uint2022_t from_string(const char* buff);

uint2022_t operator+(const uint2022_t& lhs, const uint2022_t& rhs);

uint2022_t operator-(const uint2022_t& lhs, const uint2022_t& rhs);

uint2022_t operator*(const uint2022_t& lhs, const uint2022_t& rhs);

uint2022_t operator/(const uint2022_t& lhs, const uint2022_t& rhs);

bool operator==(const uint2022_t& lhs, const uint2022_t& rhs);

bool operator!=(const uint2022_t& lhs, const uint2022_t& rhs);

bool operator>(const uint2022_t& lhs, const uint2022_t& rhs);

bool operator<(const uint2022_t& lhs, const uint2022_t &rhs);

bool operator>=(const uint2022_t& lhs, const uint2022_t& rhs);

bool operator<=(const uint2022_t& lhs, const uint2022_t& rhs);

std::ostream &operator<<(std::ostream& stream, const uint2022_t& value);
