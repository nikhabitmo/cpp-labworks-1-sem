#include "number.h"
#include <cstring>
#include <iomanip>
#include <cmath>

uint2022_t from_uint(uint32_t i) {
    uint2022_t n;
    int next = 0;
    if (i == 0) {
        n.digital_len = 1;
    }
    while (i) {
        n.array_digits[next++] = i % uint2022_t::BASE;
        i /= uint2022_t::BASE;
        n.digital_len++;
    }
    return n;
}

uint2022_t from_string(const char* buff) {
    uint2022_t result;
    int32_t i = strlen(buff) - 1;
    result.digital_len = 1;
    while (i >= 0) {
        if (result.digits_cnt % 9 == 0 && result.digits_cnt > 1) {
            result.digital_len++;
        }
        result.array_digits[result.digital_len - 1] += ((buff[i] - '0') * pow(10, (result.digits_cnt % 9)));
        i--;
        result.digits_cnt++;
    }
    if (result.digits_cnt == 0) {
        result.digits_cnt = 1;
    }
    return result;
}

uint2022_t operator+(const uint2022_t& lhs, const uint2022_t& rhs) {
    uint2022_t result;
    for (int i = 0; i < uint2022_t::SIZE; ++i) {
        result.array_digits[i] += lhs.array_digits[i] + rhs.array_digits[i];
        if (result.array_digits[i] >= uint2022_t::BASE) {
            result.array_digits[i] -= uint2022_t::BASE;
            result.array_digits[i + 1]++;
        }
        if (result.array_digits[i]!=0){
            result.digital_len++;
        }
    }
    return result;
}

uint2022_t operator-(const uint2022_t& lhs, const uint2022_t& rhs) {
    uint2022_t result;
    result.digital_len = std::max(lhs.digital_len, rhs.digital_len);
    for (int i = 0; i < result.digital_len; ++i) {
        result.array_digits[i] = lhs.array_digits[i] - rhs.array_digits[i];
    }
    for (int i = 0; i < result.digital_len; ++i) {
        if (result.array_digits[i] < 0) {
            result.array_digits[i] += uint2022_t::BASE;
            result.array_digits[i + 1]--;
        }
    }
    return result;
}

uint2022_t operator*(const uint2022_t& lhs, const uint2022_t& rhs) {
    uint2022_t result;
    uint64_t result_long[uint2022_t::SIZE] = {0};
    for (int i = 0; i < uint2022_t::SIZE; ++i) {
        for (int j = 0; j < uint2022_t::SIZE - i; ++j) {
            result_long[i + j] += uint64_t(lhs.array_digits[i]) * uint64_t(rhs.array_digits[j]);
        }
    }
    for (int i = 0; i < uint2022_t::SIZE - 1; ++i) {
        result_long[i + 1] += result_long[i] / uint2022_t::BASE;
        result_long[i] = result_long[i] % uint2022_t::BASE;
    }
    for (int i = 0; i < uint2022_t::SIZE; ++i) {
        result.array_digits[i] = result_long[i];
    }
    return result;
}

uint2022_t operator/(const uint2022_t& lhs, const uint2022_t& rhs) {
    uint2022_t result;
    uint2022_t temp;
    int j = lhs.digital_len - 1;
    while (j >= 0) {
        while (temp < rhs) {
            temp = temp * from_uint(uint2022_t::BASE);
            temp.array_digits[0] = lhs.array_digits[j];
            temp.digital_len++;
            j--;
        }
        uint32_t mult = 1;
        uint32_t l = 0;
        uint32_t r = 1000000000;
        while (l <= r) {
            uint32_t mid = (l + r) / 2;
            if (from_uint(mid) * rhs <= temp) {
                l = mid + 1;
                mult = mid;
            } else {
                r = mid - 1;
            }
        }
        result = result * from_uint(uint2022_t::BASE);
        result.digital_len++;
        result.array_digits[0] = mult;
        temp = temp - (from_uint(mult) * rhs);
    }
    return result;
}

bool operator==(const uint2022_t& lhs, const uint2022_t& rhs) {
    for (int i = 0; i < uint2022_t::SIZE; ++i) {
        if (lhs.array_digits[i] != rhs.array_digits[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const uint2022_t& lhs, const uint2022_t& rhs) {
    return !(lhs == rhs);
}

bool operator>(const uint2022_t& lhs, const uint2022_t& rhs) {
    for (int i = uint2022_t::SIZE - 1; i >= 0; --i) {
        if (lhs.array_digits[i] != rhs.array_digits[i]) {
            return lhs.array_digits[i] > rhs.array_digits[i];
        }
    }
    return false;
}

bool operator<(const uint2022_t& lhs, const uint2022_t& rhs) {
    return !(lhs > rhs || lhs == rhs);
}

bool operator>=(const uint2022_t& lhs, const uint2022_t& rhs) {
    return (lhs > rhs || lhs == rhs);
}

bool operator<=(const uint2022_t& lhs, const uint2022_t& rhs) {
    return (lhs < rhs || lhs == rhs);
}

std::ostream &operator<<(std::ostream& stream, const uint2022_t& value) {
    bool check_null = false;
    for (int i = uint2022_t::SIZE - 1; i >= 0; --i) {
        if (value.array_digits[i] != 0) {
            check_null = true;
        }
        if (check_null) {
            stream << std::setw(9) << std::setfill('0') << value.array_digits[i];
        }
    }
    if (!check_null) {
        stream << 0;
    }
    return stream;
}