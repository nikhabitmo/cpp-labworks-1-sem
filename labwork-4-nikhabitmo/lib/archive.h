#pragma once

#include "parsing.h"
#include <string>
#include <vector>

class Archive {
 public:
  std::string name;
  std::vector<std::string> files;
  uint64_t word_length;
  uint64_t number_of_files;
  std::pair<std::string, std::string> origin_archives;

  const uint8_t kBytesPerWordLength = 4;
  const uint64_t kDefaultWordLength = 8;

  explicit Archive(const BasicArguments& Arguments) :
      name(Arguments.current_archive_name),
      files(Arguments.all_file_headings),
      word_length(Arguments.code_word_len),
      number_of_files(Arguments.all_file_headings.size()),
      origin_archives(Arguments.archives_to_concatenate){}

  void ShowArchiveInfo();
  void Create();
  void Extract();
  void Append();
  void Del();
  void Concatenate();
  void List();
 private:
  static void WriteBits(std::vector<bool>& bits, std::ofstream& fout);
  static std::vector<bool> ReadBits(std::ifstream& fin, uint32_t bits_needed);
  uint64_t ReadInt(std::ifstream& fin) const;
  void WriteInt(std::ofstream& fout, int64_t to_write, bool use_default_word_length = false) const;
  std::string ReadString(std::ifstream& fin, size_t str_length) const;
  void WriteString(std::ofstream& fout, const std::string& to_write) const;
};