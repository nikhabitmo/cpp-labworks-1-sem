#include <iostream>
#include "parsing.h"
BasicArguments parse(int& arg_cnt, char* arguments_arr[]) {
  BasicArguments result;
  for (int i = 1; i < arg_cnt; ++i) {
    if (arguments_arr[i][0] == '-') {
      if (arguments_arr[i][1] == 'l' || arguments_arr[i][2] == 'l') {
        if (i < arg_cnt - 1) {
          result.length = std::stoi(arguments_arr[++i]);
        }
      } else if (arguments_arr[i][1] == 'w' || arguments_arr[i][2] == 'w') {
        if (i < arg_cnt - 1) {
          result.width = std::stoi(arguments_arr[++i]);
        }
      } else if (arguments_arr[i][1] == 'i' || arguments_arr[i][2] == 'i') {
        if (i < arg_cnt - 1) {
          result.input = arguments_arr[++i];
        }
      } else if (arguments_arr[i][1] == 'o' || arguments_arr[i][2] == 'o') {
        if (i < arg_cnt - 1) {
          result.output = arguments_arr[++i];
        }
      } else if (arguments_arr[i][1] == 'm' || arguments_arr[i][2] == 'm') {
        if (i < arg_cnt - 1) {
          result.max_iterations = std::stoi(arguments_arr[++i]);
        }
      } else if (arguments_arr[i][1] == 'f' || arguments_arr[i][2] == 'f') {
        if (i < arg_cnt - 1) {
          result.frequency = std::stoi(arguments_arr[++i]);
        }
      } else {
        std::cerr << "Error 216: Invalid argument" << '\n';
        exit(216);
      }
    }
  }
  return result;
}