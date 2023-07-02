#pragma once
#include <iostream>
#include <string>

struct BasicArguments {
  uint16_t length = 0;
  uint16_t width = 0;
  std::string input;
  std::string output;
  uint32_t max_iterations = 0;
  uint64_t frequency = 0;
};

BasicArguments parse(int& arg_cnt, char* arguments_arr[]);