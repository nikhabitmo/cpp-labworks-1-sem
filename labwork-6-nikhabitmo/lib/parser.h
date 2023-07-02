#pragma once

#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <utility>
#include <variant>
#include <typeinfo>
#include <stack>
#include <map>
#include <sstream>

namespace omfl {

class Section {
 public:

  [[nodiscard]] bool valid() const;

  std::variant<std::monostate, int, float, std::string, bool, std::vector<Section>, std::map<std::string, Section>>
      value;

  bool validity = true;

  [[nodiscard]] bool IsInt() const;

  [[nodiscard]] int AsInt() const;

  [[nodiscard]] bool IsFloat() const;

  [[nodiscard]] float AsFloat() const;

  [[nodiscard]] int AsIntOrDefault(int val) const;

  [[nodiscard]] float AsFloatOrDefault(float val) const;

  [[nodiscard]] std::string AsStringOrDefault(const std::string& val) const;

  [[nodiscard]] bool AsBoolOrDefault(bool val) const;

  [[nodiscard]] bool IsBool() const;

  [[nodiscard]] bool AsBool() const;

  [[nodiscard]] const std::string& AsString() const;

  [[nodiscard]] bool IsString() const;

  [[nodiscard]] bool IsArray() const;

  const Section& operator[](size_t i) const {
    if (i >= std::get<std::vector<Section>>(value).size()) {
      return *this;
    }
    return std::get<std::vector<Section>>(value)[i];
  };

  [[nodiscard]] Section& Get(const std::string& key) const;

};

Section parse(const std::string& str);

Section parse(const std::filesystem::path& path);

}
