#pragma once
#include <iostream>
#include <deque>
#include "parsing.h"

void readingTSV(const std::string& output, std::deque<std::deque<uint64_t>>& deque, std::deque<std::pair<uint16_t, uint16_t>>& topple);
void imageCreation(const std::deque<std::deque<uint64_t>>& deque, const std::string& image_number, const std::string& output_path);
void sandpileTopple(std::deque<std::deque<uint64_t>>& deque, std::deque<std::pair<uint16_t, uint16_t>>& topple, BasicArguments& Current_Arguments);