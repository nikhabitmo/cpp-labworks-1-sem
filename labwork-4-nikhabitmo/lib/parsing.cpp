#include "parsing.h"
#include <cstring>

BasicArguments parse(int& arg_cnt, char* arguments_arr[]) {
  BasicArguments result;
  for (int i = 1; i < arg_cnt; ++i) {

    if (strcmp(arguments_arr[i], "--create") == 0 || strcmp(arguments_arr[i], "-c") == 0) {
      result.creation = true;
    } else if (strlen(arguments_arr[i]) > 6 && std::string(arguments_arr[i]).substr(0, 6) == "--file"
        && arguments_arr[i][6] == '=') {
      result.current_archive_name = std::string(arguments_arr[i]).substr(7);

    } else if (strcmp(arguments_arr[i], "-f") == 0) {
      if (i < arg_cnt - 1) {
        result.current_archive_name = arguments_arr[++i];
      }

    } else if (strcmp(arguments_arr[i], "--list") == 0 || strcmp(arguments_arr[i], "-l") == 0) {
      result.list_out = true;

    } else if (strcmp(arguments_arr[i], "--extract") == 0 || strcmp(arguments_arr[i], "-x") == 0) {
      result.extraction = true;

    } else if (strcmp(arguments_arr[i], "--append") == 0 || strcmp(arguments_arr[i], "-a") == 0) {
      result.appendix = true;

    } else if (strcmp(arguments_arr[i], "--delete") == 0 || strcmp(arguments_arr[i], "-d") == 0) {
      result.deletion = true;

    } else if (strcmp(arguments_arr[i], "--word-length") == 0 || strcmp(arguments_arr[i], "-w") == 0) {
      if (i < arg_cnt - 1) {
        result.code_word_len = std::stoi(arguments_arr[i + 1]);
      }

    } else if (strcmp(arguments_arr[i], "--concatenate") == 0 || strcmp(arguments_arr[i], "-A") == 0) {
      if (i < arg_cnt - 2) {
        result.archives_to_concatenate.first = arguments_arr[++i];
        result.archives_to_concatenate.second = arguments_arr[++i];
        result.concatenate = true;
      }

    } else
      result.all_file_headings.emplace_back(arguments_arr[i]);
  }
  return result;
}