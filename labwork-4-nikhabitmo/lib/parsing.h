#pragma once
#include <vector>
#include <string>

struct BasicArguments {
  bool creation = false;
  bool list_out = false;
  bool extraction = false;
  bool appendix = false;
  bool deletion = false;
  bool concatenate = false;
  std::string current_archive_name;
  size_t code_word_len = 8;
  std::vector<std::string> all_file_headings;
  std::pair<std::string, std::string> archives_to_concatenate;
};

BasicArguments parse(int& arg_cnt, char* arguments_arr[]);