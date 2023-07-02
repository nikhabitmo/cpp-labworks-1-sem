#include "archive.h"
#include "hamarc.h"
#include <algorithm>
#include <cmath>
#include <fstream>

static std::bitset<64> vtob(const std::vector<bool>& from) {
  std::bitset<64> to;
  int j = 64 - 1;
  for (size_t i = from.size(); i < 64; ++i) {
    to[j] = false;
    j--;
  }
  for (bool i : from) {
    to[j] = i;
    j--;
  }
  return to;
}

void Archive::WriteBits(std::vector<bool>& bits, std::ofstream& fout) {
  for (size_t i = 0; i < bits.size(); i += CHAR_BIT) {
    std::bitset<CHAR_BIT> byte;
    for (size_t j = i; j - i < CHAR_BIT && j < bits.size(); ++j) {
      byte.set(CHAR_BIT - (j - i) -  1, bits[j]);
    }
    unsigned long n = byte.to_ulong();
    fout.write(reinterpret_cast<const char*>(&n), 1);
  }
}

std::vector<bool> Archive::ReadBits(std::ifstream& fin, uint32_t bits_needed) {
  std::vector<bool> result;
  size_t i = 0;
  while (i < bits_needed) {
    char ch;
    fin.read(reinterpret_cast<char*>(&ch), 1);
    std::bitset<CHAR_BIT> bits(ch);
    for (int j = CHAR_BIT - 1; j >= 0 && i < bits_needed; --j) {
      result.push_back(bits[j]);
      i++;
    }
  }
  return result;
}

uint64_t Archive::ReadInt(std::ifstream& fin) const {
  uint32_t bits_needed = (log(word_length) / log(2) + 1 + word_length + 1) * (32.0 / word_length);
  std::vector<bool> bits_before_decoding = ReadBits(fin, bits_needed);
  Hamming temp;
  temp.Decode(word_length);
  std::bitset<64> resulted_bitset = vtob(Number.bits);
  return resulted_bitset.to_ulong();
}

void Archive::WriteInt(std::ofstream& fout, int64_t to_write, bool use_default_word_length) const {
  Hamming Number(to_write);
  if (use_default_word_length) Number.Encode(kDefaultWordLength);
  else Number.Encode(word_length);
  WriteBits(Number.bits, fout);
}

std::string Archive::ReadString(std::ifstream& fin, size_t str_length) const {
  uint32_t bits_needed = (log(word_length) / log(2) + 1 + word_length + 1) * (str_length * 8 / word_length);
  std::vector<bool> bits_before_decoding = ReadBits(fin, bits_needed);
  Hamming Str(bits_before_decoding);
  Str.Decode(word_length);
  std::bitset<CHAR_BIT> temp_bitset;
  size_t i = 0;
  std::string resulted_string;
  while (i < Str.bits.size()) {
    for (size_t j = 0; j < CHAR_BIT; ++j) {
      temp_bitset[j] = Str.bits[i];
      i++;
    }
    uint64_t temp = temp_bitset.to_ulong();
    auto ch = static_cast<unsigned char>(temp);
    resulted_string += ch;
  }
  return resulted_string;
}

void Archive::WriteString(std::ofstream& fout, const std::string& to_write) const {
  Hamming Str(to_write);
  Str.Encode(word_length);
  WriteBits(Str.bits, fout);
}

void Archive::Create() {
  std::ofstream archive_file(name + ".haf", std::ios::out | std::ios::binary);
  if (!archive_file.is_open()) {
    std::cerr << "Error opening file!\n";
    exit(1);
  }
  WriteInt(archive_file, word_length, true);
  WriteInt(archive_file, number_of_files);
  for (size_t i = 0; i < number_of_files; ++i) {
    std::ifstream file_to_archive(files[i], std::ios::in | std::ios::binary);
    if (!file_to_archive.is_open()) {
      std::cerr << "Error opening file!\n";
      exit(1);
    }
    WriteInt(archive_file, files[i].length());
    WriteString(archive_file, files[i]);

    file_to_archive.seekg(0, std::ios::end);
    size_t file_to_archive_size = file_to_archive.tellg();
    file_to_archive.seekg(0);
    WriteInt(archive_file, file_to_archive_size);
    std::vector<bool> bits = ReadBits(file_to_archive, file_to_archive_size * 8);
    Hamming FileContent(bits);
    FileContent.Encode(word_length);
    WriteBits(FileContent.bits, archive_file);
  }
}

void Archive::Extract() {
  std::ifstream archive_file(name + ".haf", std::ios::in | std::ios::binary);
  if (!archive_file.is_open()) {
    std::cerr << "Error opening file!\n";
    exit(1);
  }
  std::string temp_archive_name = "temp_ " + name;
  std::ofstream temp_archive_file(temp_archive_name + ".haf", std::ios::out | std::ios::binary);
  if (!temp_archive_file.is_open()) {
    std::cerr << "Error opening file!\n";
    exit(1);
  }
  word_length = ReadInt(archive_file);
  number_of_files = ReadInt(archive_file);
  if (!files.empty()) {
    WriteInt(temp_archive_file, word_length, true);
    number_of_files -= files.size();
    WriteInt(temp_archive_file, number_of_files);
  }
  for (size_t i = 0; i < number_of_files + files.size(); ++i) {
    size_t file_name_length = ReadInt(archive_file);
    std::string file_name = ReadString(archive_file, file_name_length);
    size_t file_size = ReadInt(archive_file);
    uint32_t bits_needed = (log(word_length) / log(2) + 1 + word_length + 1) * (file_size * 8 / word_length);
    std::vector<bool> content_bits = ReadBits(archive_file, bits_needed);
    if (std::find(files.begin(), files.end(), file_name) != files.end() || files.empty()) {
      Hamming FileContent(content_bits);
      FileContent.Decode(word_length);
      std::ofstream file_to_extract(file_name + "-extr", std::ios::out | std::ios::binary);
      if (!file_to_extract.is_open()) {
        std::cerr << "Error opening file!\n";
        exit(1);
      }
      WriteBits(FileContent.bits, file_to_extract);
    } else {
      WriteInt(temp_archive_file, file_name_length);
      WriteString(temp_archive_file, file_name);
      WriteInt(temp_archive_file, file_size);
      WriteBits(content_bits, temp_archive_file);
    }
  }
  archive_file.close();
  temp_archive_file.close();
  if (files.empty() || number_of_files == 0) {
    std::remove((name + ".haf").c_str());
    std::remove((temp_archive_name + ".haf").c_str());
  } else {
    std::remove((name + ".haf").c_str());
    std::rename((temp_archive_name + ".haf").c_str(), (name + ".haf").c_str());
  }
}

void Archive::Append() {
  std::ifstream archive_file(name + ".haf", std::ios::in | std::ios::binary);
  if (!archive_file.is_open()) {
    std::cerr << "Error opening file!\n";
    exit(1);
  }
  std::string temp_archive_name = "temp_ " + name;
  std::ofstream temp_archive_file(temp_archive_name + ".haf", std::ios::out | std::ios::binary);
  if (!temp_archive_file.is_open()) {
    std::cerr << "Error opening file!\n";
    exit(1);
  }
  word_length = ReadInt(archive_file);
  number_of_files = ReadInt(archive_file);
  WriteInt(temp_archive_file, word_length, true);
  number_of_files += files.size();
  WriteInt(temp_archive_file, number_of_files);
  for (size_t i = 0; i < number_of_files - files.size(); ++i) {
    size_t file_name_length = ReadInt(archive_file);
    std::string file_name = ReadString(archive_file, file_name_length);
    size_t file_size = ReadInt(archive_file);
    uint32_t bits_needed = (log(word_length) / log(2) + 1 + word_length + 1) * (file_size * 8 / word_length);
    std::vector<bool> content_bits = ReadBits(archive_file, bits_needed);
    WriteInt(temp_archive_file, file_name_length);
    WriteString(temp_archive_file, file_name);
    WriteInt(temp_archive_file, file_size);
    WriteBits(content_bits, temp_archive_file);
  }
  for (auto & file : files) {
    std::ifstream file_to_archive(file, std::ios::in | std::ios::binary);
    if (!file_to_archive.is_open()) {
      std::cerr << "Error opening file!\n";
      exit(1);
    }
    WriteInt(temp_archive_file, file.length());
    WriteString(temp_archive_file, file);

    file_to_archive.seekg(0, std::ios::end);
    size_t file_to_archive_size = file_to_archive.tellg();
    file_to_archive.seekg(0);
    WriteInt(temp_archive_file, file_to_archive_size);
    std::vector<bool> bits = ReadBits(file_to_archive, file_to_archive_size * 8);
    Hamming FileContent(bits);
    FileContent.Encode(word_length);
    WriteBits(FileContent.bits, temp_archive_file);
  }

  archive_file.close();
  temp_archive_file.close();
  std::remove((name + ".haf").c_str());
  std::rename((temp_archive_name + ".haf").c_str(), (name + ".haf").c_str());
}

void Archive::Del() {
  std::ifstream archive_file(name + ".haf", std::ios::in | std::ios::binary);
  if (!archive_file.is_open()) {
    std::cerr << "Error opening file!\n";
    exit(1);
  }
  std::string temp_archive_name = "temp_ " + name;
  std::ofstream temp_archive_file(temp_archive_name + ".haf", std::ios::out | std::ios::binary);
  if (!temp_archive_file.is_open()) {
    std::cerr << "Error opening file!\n";
    exit(1);
  }
  word_length = ReadInt(archive_file);
  number_of_files -= files.size();
  number_of_files = ReadInt(archive_file);
  WriteInt(temp_archive_file, word_length, true);
  number_of_files -= files.size();
  WriteInt(temp_archive_file, number_of_files);
  for (size_t i = 0; i < number_of_files + files.size(); ++i) {
    size_t file_name_length = ReadInt(archive_file);
    std::string file_name = ReadString(archive_file, file_name_length);
    size_t file_size = ReadInt(archive_file);
    uint32_t bits_needed = (log(word_length) / log(2) + 1 + word_length + 1) * (file_size * 8 / word_length);
    std::vector<bool> content_bits = ReadBits(archive_file, bits_needed);
    if (std::find(files.begin(), files.end(), file_name) == files.end()) {
      WriteInt(temp_archive_file, file_name_length);
      WriteString(temp_archive_file, file_name);
      WriteInt(temp_archive_file, file_size);
      WriteBits(content_bits, temp_archive_file);
    }
  }
  archive_file.close();
  temp_archive_file.close();
  if (files.empty() || number_of_files == 0) {
    std::remove((name + ".haf").c_str());
    std::remove((temp_archive_name + ".haf").c_str());
  } else {
    std::remove((name + ".haf").c_str());
    std::rename((temp_archive_name + ".haf").c_str(), (name + ".haf").c_str());
  }
}

void Archive::Concatenate() {
  std::ofstream archive_file(name + ".haf", std::ios::out | std::ios::binary);
  if (!archive_file.is_open()) {
    std::cerr << "Error opening file!\n";
    exit(1);
  }
  std::ifstream origin_archive_file_1(origin_archives.first + ".haf", std::ios::in | std::ios::binary);
  if (!origin_archive_file_1.is_open()) {
    std::cerr << "Error opening file!\n";
    exit(1);
  }
  std::ifstream origin_archive_file_2(origin_archives.second + ".haf", std::ios::in | std::ios::binary);
  if (!origin_archive_file_2.is_open()) {
    std::cerr << "Error opening file!\n";
    exit(1);
  }
  word_length = ReadInt(origin_archive_file_1);
  word_length = ReadInt(origin_archive_file_2);
  size_t number_of_files_origin_1 = ReadInt(origin_archive_file_1);
  size_t number_of_files_origin_2 = ReadInt(origin_archive_file_2);
  number_of_files = number_of_files_origin_1 + number_of_files_origin_2;
  WriteInt(archive_file, word_length, true);
  WriteInt(archive_file, number_of_files);
  for (size_t i = 0; i < number_of_files_origin_1; ++i) {
    size_t file_name_length = ReadInt(origin_archive_file_1);
    std::string file_name = ReadString(origin_archive_file_1, file_name_length);
    size_t file_size = ReadInt(origin_archive_file_1);
    uint32_t bits_needed = (log(word_length) / log(2) + 1 + word_length + 1) * (file_size * 8 / word_length);
    std::vector<bool> content_bits = ReadBits(origin_archive_file_1, bits_needed);
    WriteInt(archive_file, file_name_length);
    WriteString(archive_file, file_name);
    WriteInt(archive_file, file_size);
    WriteBits(content_bits, archive_file);
  }
  for (size_t i = 0; i < number_of_files_origin_2; ++i) {
    size_t file_name_length = ReadInt(origin_archive_file_2);
    std::string file_name = ReadString(origin_archive_file_2, file_name_length);
    size_t file_size = ReadInt(origin_archive_file_2);
    uint32_t bits_needed = (log(word_length) / log(2) + 1 + word_length + 1) * (file_size * 8 / word_length);
    std::vector<bool> content_bits = ReadBits(origin_archive_file_2, bits_needed);
    WriteInt(archive_file, file_name_length);
    WriteString(archive_file, file_name);
    WriteInt(archive_file, file_size);
    WriteBits(content_bits, archive_file);
  }
  origin_archive_file_1.close();
  origin_archive_file_2.close();
  std::remove((origin_archives.first + ".haf").c_str());
  std::remove((origin_archives.second + ".haf").c_str());
}

void Archive::List() {
  std::ifstream archive_file(name + ".haf", std::ios::in | std::ios::binary);
  if (!archive_file.is_open()) {
    std::cerr << "Error opening file!\n";
    exit(1);
  }
  word_length = ReadInt(archive_file);
  number_of_files = ReadInt(archive_file);
  std::cout << "number of files: " << number_of_files << '\n';
  for (size_t i = 0; i < number_of_files; ++i) {
    size_t file_name_length = ReadInt(archive_file);
    files.push_back(ReadString(archive_file, file_name_length));
    std::cout << "\tfile #" << i + 1 << ": " << files[files.size() - 1] << '\n';
    size_t file_size = ReadInt(archive_file);
    uint32_t bits_needed = (log(word_length) / log(2) + 1 + word_length + 1) * (file_size * 8 / word_length);
    std::vector<bool> content_bits = ReadBits(archive_file, bits_needed);
  }
}

void Archive::ShowArchiveInfo() {
  std::cout << "---------------Archive Info----------------\n";
  std::ifstream archive_file(name + ".haf", std::ios::in | std::ios::binary);
  if (!archive_file.is_open()) {
    std::cerr << "Error opening file!\n";
    exit(1);
  }

  word_length = ReadInt(archive_file);
  std::cout << "word length: " << word_length << '\n';

  number_of_files = ReadInt(archive_file);
  std::cout << "number of files: " << number_of_files << '\n';

  for (size_t i = 0; i < number_of_files; ++i) {
    size_t file_name_length = ReadInt(archive_file);
    files.push_back(ReadString(archive_file, file_name_length));
    std::cout << "filename: " << file_name_length << " " << files[files.size() - 1] << '\n';
    size_t file_size = ReadInt(archive_file);
    uint32_t bits_needed = (log(word_length) / log(2) + 1 + word_length + 1) * (file_size * 8 / word_length);
    std::vector<bool> content_bits = ReadBits(archive_file, bits_needed);
    Hamming FileContent(content_bits);
    FileContent.Decode(word_length);
    size_t j = 0;
    std::string resulted_string;
    while (j < FileContent.bits.size()) {
      std::bitset<CHAR_BIT> temp_bitset;
      for (int k = CHAR_BIT - 1; k >= 0; --k) {
        temp_bitset[k] = FileContent.bits[j];
        j++;
      }
      uint64_t temp = temp_bitset.to_ulong();
      auto ch = static_cast<unsigned char>(temp);
      resulted_string += ch;
    }
    std::cout << "content: " << file_size << '\n' << resulted_string << '\n';
  }

  std::cout << "-------------------------------------------\n";
}