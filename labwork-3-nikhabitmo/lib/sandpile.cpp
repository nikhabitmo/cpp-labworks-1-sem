#include "converting_bmp.h"
#include "sandpile.h"
#include "parsing.h"
#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>

void readingTSV(const std::string& file_heading,
                std::deque<std::deque<uint64_t>>& deque,
                std::deque<std::pair<uint16_t, uint16_t>>& topple) {
  std::ifstream file(file_heading);
  for (size_t i = 0; i < deque.size(); ++i) {
    for (size_t j = 0; j < deque.size(); ++j) {
      deque[i][j] = 0;
    }
  }
  if (file.is_open()) {
    uint16_t x = 0;
    uint16_t y = 0;
    uint64_t sand;
    while (!file.eof()) {
      file >> x >> y >> sand;
      deque[x][y] = sand;
      if (deque[x][y] > 3) {
        topple.emplace_back(x, y);
      }
    }

  } else {
    std::cerr << "That was an error with opening file" << "\n";
    exit(1);
  }
}

void imageCreation(const std::deque<std::deque<uint64_t>>& deque,
                   const std::string& image_number,
                   const std::string& output_path) {
  Image image(deque[0].size(), deque.size());
  for (int x = 0; x < deque.size(); ++x) {
    for (int y = 0; y < deque[y].size(); ++y) {
      if (deque[x][y] == 0) image.SetColor(Color(0.98f, .98f, .98f), x, y);
      if (deque[x][y] == 1) image.SetColor(Color(0, 110.0f, 0.3f), x, y);
      if (deque[x][y] == 2) image.SetColor(Color(0.5f, 0, 0.5f), x, y);
      if (deque[x][y] == 3) image.SetColor(Color(0.98f, 0.98f, 0), x, y);
      if (deque[x][y] > 3) image.SetColor(Color(0, 0, 0), x, y);
    }
  }
  std::string bmp_file_heading = output_path + "image_" + image_number + ".bmp";
  image.Export(bmp_file_heading);
}

void sandpileTopple(std::deque<std::deque<uint64_t>>& deque,
                    std::deque<std::pair<uint16_t, uint16_t>>& topple,
                    BasicArguments& CurrentArguments) {
  uint64_t count = 0;
  while (!topple.empty()) {
    uint16_t shift_x = 0;
    uint16_t shift_y = 0;

    std::deque<std::pair<uint16_t, uint16_t>> next_topple;

    if (CurrentArguments.frequency != 0 && count % CurrentArguments.frequency == 0) {
      std::string number = std::to_string(count / CurrentArguments.frequency);
      imageCreation(deque, number, CurrentArguments.output);
    }

    if (count != CurrentArguments.max_iterations || CurrentArguments.max_iterations == 0) {
      count++;
      for (auto & i : topple) {
        uint16_t x = i.first + shift_x;
        uint16_t y = i.second + shift_y;

        if (y + 1 >= CurrentArguments.width) {
          for (auto j = 0; j < CurrentArguments.length; ++j) {
            deque[j].push_back(0);
          }
          CurrentArguments.width++;
        }

        if (y - 1 < 0) {
          for (auto j = 0; j < CurrentArguments.width; ++j) {
            deque[j].push_front(0);
          }
          shift_y++;
          y++;
          CurrentArguments.width++;
          for(auto & j : next_topple){
            j.second++;
          }
        }

        std::deque<uint64_t> temp(CurrentArguments.width);
        for (auto& it : temp) {
          it = 0;
        }

        if (x + 1 >= CurrentArguments.length) {
          deque.push_back(temp);
          CurrentArguments.length++;
        }

        if (x - 1 < 0) {
          deque.push_front(temp);
          CurrentArguments.length++;
          ++x;
          shift_x++;
          for(auto & j : next_topple){
            j.first++;
          }
        }

        deque[x][y - 1]++;
        if (deque[x][y - 1] == 4) {
          next_topple.emplace_back(x, y - 1);
        }

        deque[x + 1][y]++;
        if (deque[x + 1][y] == 4) {
          next_topple.emplace_back(x + 1, y);
        }

        deque[x - 1][y]++;
        if (deque[x - 1][y] == 4) {
          next_topple.emplace_back(x - 1, y);
        }

        deque[x][y + 1]++;
        if (deque[x][y + 1] == 4) {
          next_topple.emplace_back(x, y + 1);
        }

        deque[x][y] -= 4;

        if (deque[x][y] > 3) {
          next_topple.emplace_back(x, y);
        }
      }

      topple = next_topple;
    } else {
      break;
    }
  }
}