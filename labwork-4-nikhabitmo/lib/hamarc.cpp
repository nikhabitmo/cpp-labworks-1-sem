#include "hamarc.h"
#include <cmath>

static bool isPowerOfTwo(size_t number) {
  while (number % 2 == 0 && number) {
    number /= 2;
  }
  return number == 1;
}

void Hamming::countParityBits(std::vector<bool>& vector) {
  size_t cur_parity_bit = 1;
  while (cur_parity_bit < vector.size()) {
    size_t i = cur_parity_bit;
    bool skipping = false;
    while (i < vector.size()) {
      if (skipping) {
        i += cur_parity_bit;
        skipping = false;
      } else {
        for (size_t j = 0; j < cur_parity_bit && i < vector.size(); ++j, ++i) {
          vector[cur_parity_bit] = vector[cur_parity_bit] ^ vector[i];
        }
        skipping = true;
      }
    }
    cur_parity_bit *= 2;
  }
  for (int j = 0; j < vector.size(); ++j) {
    vector[0] = vector[0] ^ vector[j];
  }
}

std::vector<bool> Hamming::EncodeWord(std::vector<bool>& word_before_encoding) {
  std::vector<bool> result;
  result.push_back(false);
  size_t i = 0;
  while (i < word_before_encoding.size()) {
    if (isPowerOfTwo(result.size())) {
      result.push_back(false);
    } else {
      result.push_back(word_before_encoding[i++]);
    }
  }
  countParityBits(result);
  return result;
}

void Hamming::Encode(uint8_t word_length, std::vector<bool>& bits) {
  std::vector<bool> result;
  size_t i = 0;
  while (i < bits.size()) {
    size_t j = 0;
    std::vector<bool> word = {};
    while (j < word_length && (i + j) < bits.size()) {
      word.push_back(bits[i + j]);
      j++;
    }
    word = EncodeWord(word);
    for (auto && k : word) result.push_back(k);
    i += j;
  }
  bits = result;
}

std::vector<bool> Hamming::DecodeWord(std::vector<bool>& word_before_decoding) {
  std::vector<bool> result;
  std::vector<bool> after_decoding;
  for (size_t i = 0; i < word_before_decoding.size(); ++i) {
    if (isPowerOfTwo(i)) {
      after_decoding.push_back(false);
    } else {
      after_decoding.push_back(word_before_decoding[i]);
    }
  }
  countParityBits(after_decoding);

  size_t sm = 0;
  for (size_t i = 0; i < after_decoding.size(); ++i) {
    if (after_decoding[i] != word_before_decoding[i]) {
      sm += i;
    }
  }
  if (sm != 0) {
    if (after_decoding[0] == word_before_decoding[0]) {
      std::cerr << "\n2 errors detected\n";
    } else if(after_decoding[0] != word_before_decoding[0]) {
      std::cerr << "\n1 error fixed\n";
      after_decoding[sm] = !after_decoding[sm];
    }
  }
  for (size_t i = 1; i < after_decoding.size(); ++i) {
    if (!isPowerOfTwo(i)) result.push_back(after_decoding[i]);
  }
  return result;
}

void Hamming::Decode(uint8_t word_length, std::vector<bool>& bits) {
  std::vector<bool> result;
  uint32_t after_encoding_word_length = log(word_length) / log(2) + 1 + word_length + 1;
  size_t i = 0;
  std::vector<bool> word = {};
  while (i < bits.size()) {
    size_t j = 0;
    word = {};
    while (j < after_encoding_word_length && (i + j) < bits.size()) {
      word.push_back(bits[i + j]);
      j++;
    }
    word = DecodeWord(word);
    for (auto && k : word) result.push_back(k);
    i += j;
  }
  bits = result;
}