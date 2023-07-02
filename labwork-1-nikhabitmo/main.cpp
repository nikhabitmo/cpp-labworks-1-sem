#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>
#include <string>
#include <vector>

class Flags {
 public:
  bool counting_lines = false;
  bool counting_bytes = false;
  bool counting_words = false;
  bool counting_chars = false;
  bool counting_the_most_frequently_word = false;
};

static std::pair<std::string, size_t> findMostFrequentWord(const std::map<std::string, size_t>& t, size_t ind) {
  for (auto& it : t) if (it.second == ind) return {it};
  return {};
}

class CountArr {
 public:
  size_t count_lines = 1;
  size_t count_bytes = 0;
  size_t count_words = 0;
  size_t count_chars = 0;
  size_t most_counted_word = 0;

  CountArr& Counter(const std::string& name) {
    std::ifstream file(name, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
      std::cout << "Error opening file";
      exit(1);
    } else {
      if (file.peek() == EOF) {
        count_lines = 0;
        count_words = 0;
      }
      char cur_symbol;
      char prev_symbol;
      std::string current_word;
      bool checking_first_symbol = true;
      while (file.get(cur_symbol)) {
        if (checking_first_symbol) {
          if (std::isalnum(cur_symbol)) {
            count_words++;
            current_word += cur_symbol;
          }
          checking_first_symbol = false;
        }
        if (std::isalnum(cur_symbol) && std::isspace(prev_symbol)) {
          count_words++;
        }
        if (cur_symbol == '\n' || cur_symbol == '\0') {
          count_lines++;
        }
        if (std::isalpha(cur_symbol)) {
          count_chars++;
        }
        if (std::isspace(cur_symbol)) {
          CounterWords[current_word]++;
          most_counted_word = std::max(CounterWords[current_word], most_counted_word);
          current_word.erase();
        } else {
          current_word += cur_symbol;
        }
        prev_symbol = cur_symbol;
        count_bytes++;
      }
    }
    return *this;
  }

  void print(const Flags& Check) const {
    if (Check.counting_lines) std::cout << "LINES: " << count_lines << '\n';
    if (Check.counting_bytes) std::cout << "BYTES: " << count_bytes << '\n';
    if (Check.counting_words) std::cout << "WORDS: " << count_words << '\n';
    if (Check.counting_chars) std::cout << "CHARS: " << count_chars << '\n';
    if (Check.counting_the_most_frequently_word) {
      auto pair = findMostFrequentWord(CounterWords, most_counted_word);
      std::cout << "THE MOST FREQUENT WORD: " << pair.first << '\n';
      std::cout << "THE TIMES WORD WERE MET: " << pair.second << '\n';
    }
    if (Check.counting_chars + Check.counting_words + Check.counting_bytes + Check.counting_lines == 0) {
      std::cout << count_lines << ' ' << count_bytes << ' ' << count_words << ' ' << count_chars << ' ';
    }
  }

 private:
  std::map<std::string, size_t> CounterWords;
};

int main(int argc, char** argv) {
  std::vector<std::string> all_files;
  Flags CurFlags;
  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] != '-') {
      all_files.emplace_back(argv[i]);
    } else {
      if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--lines") == 0) {
        CurFlags.counting_lines = true;
      } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--bytes") == 0) {
        CurFlags.counting_bytes = true;
      } else if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--words") == 0) {
        CurFlags.counting_words = true;
      } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--frequent") == 0) {
        CurFlags.counting_the_most_frequently_word = true;
      } else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--chars") == 0) {
        CurFlags.counting_chars = true;
      } else {
        int j = 1;
        while (isalpha(argv[i][j])) {
          if (argv[i][j] == 'l') {
            CurFlags.counting_lines = true;
          } else if (argv[i][j] == 'c') {
            CurFlags.counting_bytes = true;
          } else if (argv[i][j] == 'w') {
            CurFlags.counting_words = true;
          } else if (argv[i][j] == 'm') {
            CurFlags.counting_chars = true;
          } else if (argv[i][j] == 'f') {
            CurFlags.counting_the_most_frequently_word = true;
          }
          j++;
        }
      }
    }
  }

  for (auto& file_name : all_files) {
    CountArr Result;
    Result.Counter(file_name);
    Result.print(CurFlags);
    std::cout << file_name << '\n';
  }

  return 0;
}