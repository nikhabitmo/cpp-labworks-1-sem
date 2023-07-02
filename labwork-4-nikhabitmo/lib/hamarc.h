#pragma once

#include <bitset>
#include <iostream>
#include <string>
#include <vector>

class Hamming {
 public:
  std::vector<bool> integerToBits(uint32_t need_coding, std::vector<bool>& bits) {
    std::bitset<32> need_coding_bits(need_coding);
    for (int i = 31; i >= 0; --i) bits.push_back(need_coding_bits[i]);
    return bits;
  }
  std::vector<bool> stringToBits (const std::string& need_coding){
    std::vector<bool> bits;
    size_t str_length = need_coding.length();
    for(size_t i = 0; i < str_length; ++i){
      std::bitset<CHAR_BIT> need_coding_char_bits(need_coding[i]);
      for (size_t j = 0; j < CHAR_BIT; ++j) bits.push_back(need_coding_char_bits[j]);
    }
    return bits;
  }
  void ShowBits(const std::vector<bool>& bits) const {
    std::cout << "Bits in Hamming: ";
    for(auto el : bits) std::cout << el;
    std::cout << '\n';
  }
  void Encode(uint8_t word_length, std::vector<bool>& bits);
  void Decode(uint8_t word_length, std::vector<bool>& bits);
 private:
  static void countParityBits(std::vector<bool>& vector);
  static std::vector<bool> EncodeWord(std::vector<bool>& word_before_encoding);
  static std::vector<bool> DecodeWord(std::vector<bool>& word_before_decoding);
};
