#include "parser.h"

static omfl::Section parseString(const std::string& str) {
  omfl::Section Result;
  if (std::count(str.begin(), str.end(), '"') != 2) Result.value = std::monostate();
  else Result.value = str.substr(1, str.length() - 2);
  return Result;
}

static omfl::Section parseFloat(const std::string& str) {
  omfl::Section Result;
  size_t len = str.length();
  for (int i = 0; i < len; ++i) {
    if ((!std::isdigit(str[i]) && str[i] != '.' && str[i] != '+' && str[i] != '-')
        || (str[i] == '.' && (!i || !std::isdigit(str[i - 1]) || len < 2 || i + 1 >= len || !std::isdigit(str[i + 1])))
        || ((str[i] == '+' || str[i] == '-') && (i || len < 2 || !std::isdigit(str[i + 1])))) {
      Result.value = std::monostate();
      return Result;
    }
  }
  Result.value = std::stof(str);
  return Result;
}

static omfl::Section parseInteger(const std::string& str) {
  omfl::Section Result;
  size_t len = str.length();
  for (int i = 0; i < len; ++i) {
    if (!(isdigit(str[i]) || ((str[0] == '+' || str[0] == '-') && len > 1))) {
      Result.value = std::monostate();
      return Result;
    }
  }
  Result.value = std::stoi(str);
  return Result;
}

static omfl::Section parseBool(const std::string& str) {
  omfl::Section Result;
  if (str == "true") Result.value = true;
  else if (str == "false") Result.value = false;
  else Result.value = std::monostate();
  return Result;
}

static bool keyIsCorrect(const std::string& str) {
  size_t len = str.length();
  for (int i = 0; i < len; ++i) {
    if (!(isalnum(str[i]) || str[i] == '_' || str[i] == '-')) {
      return false;
    }
  }
  return !str.empty();
}

static omfl::Section getValue(const std::string& str) {
  omfl::Section Result;
  if (str.empty()) {
    Result.value = std::monostate();
    return Result;
  }
  if (str[0] != '[') {
    if (str[0] == '"' && str[str.size() - 1] == '"') {
      return parseString(str);
    } else if (std::count(str.begin(), str.end(), '.') == 1) {
      return parseFloat(str);
    } else if (str == "true" || str == "false") {
      return parseBool(str);
    } else {
      return parseInteger(str);
    }
  } else {
    std::vector<omfl::Section> vector;
    std::string temp_value;
    bool check_in_string = false;
    std::stack<char> checker_brackets;
    size_t len = str.length();
    for (size_t i = 1; i < len; ++i) {
      while (std::isspace(str[i]) && i < len - 1) ++i;
      temp_value.erase();
      while (i < len && (str[i] != ',' && str[i] != ']' || !checker_brackets.empty()
          || (check_in_string && !std::isspace(str[i]) && str[i] != '[' && str[i] != ']'
              && str[i] != ','))) {
        if (str[i] == '"') check_in_string = !check_in_string;
        if (str[i] == '[') checker_brackets.push('[');
        if (str[i] == ']') checker_brackets.pop();
        temp_value += str[i];
        i++;
      }
      omfl::Section Check = getValue(temp_value);
      if (std::holds_alternative<std::monostate>(Check.value)) {
        return Check;
      }
      vector.push_back(Check);
    }
    Result.value = vector;
  }
  return Result;
}

namespace omfl {

Section parse(const std::string& str) {
  Section Result;
  Section* CurSection = &Result;
  std::stringstream stream;
  stream << str;
  std::string line;
  while (std::getline(stream, line)) {
    size_t ind = 0;
    size_t len = line.length();
    std::string key;
    while (ind < len && isspace(line[ind])) ++ind;
    if (ind == line.length() || line[ind] == '#') continue;
    if (line[ind] == '[') {
      CurSection = &Result;
      std::string temp_string;
      bool section_ended = false;
      for (size_t i = ind + 1; i < len; ++i) {
        if (line[i] != '.') {
          if (line[i] == ']') {
            if (i != len - 1) {
              Result.validity = false;
              return Result;
            }
            section_ended = true;
            if (!keyIsCorrect(temp_string)) {
              Result.validity = false;
              return Result;
            }
            if (!std::holds_alternative<std::monostate>(CurSection->value)
                && !std::holds_alternative<std::map<std::string, Section>>(CurSection->value)) {
              Result.validity = false;
              return Result;
            } else {
              CurSection->value = std::map<std::string, Section>();
            }
            std::get<std::map<std::string, Section>>(CurSection->value)[temp_string] = Section();
            CurSection = &CurSection->Get(temp_string);
            break;
          } else {
            temp_string += line[i];
            continue;
          }
        }
        if (!keyIsCorrect(temp_string)) {
          Result.validity = false;
          return Result;
        }
        if (!std::holds_alternative<std::map<std::string, Section>>(CurSection->value)) {
          if (!std::holds_alternative<std::monostate>(CurSection->value)) {
            Result.validity = false;
            return Result;
          } else {
            CurSection->value = std::map<std::string, Section>();
          }
        }
        std::get<std::map<std::string, Section>>(CurSection->value).insert({temp_string, Section()});
        CurSection = &CurSection->Get(temp_string);
        temp_string.erase();
      }
      if (!section_ended) {
        Result.validity = false;
        return Result;
      }
      continue;
    }
    while (ind < len && !(isspace(line[ind]) || line[ind] == '=')) {
      key += line[ind];
      ind++;
    }
    if (!keyIsCorrect(key)) {
      Result.validity = false;
      return Result;
    }
    if (std::holds_alternative<std::map<std::string, Section>>(CurSection->value)) {
      if (std::get<std::map<std::string, Section>>(CurSection->value).count(key)) {
        Result.validity = false;
        return Result;
      }
    }
    while (ind < len && isspace(line[ind])) ind++;
    if (line[ind] != '=' || ind + 1 == len) {
      Result.validity = false;
      return Result;
    } else {
      ind++;
    }
    while (ind < len && isspace(line[ind])) ind++;
    size_t hashtag_ind = line.find_first_of('#');
    if (hashtag_ind != std::string::npos) {
      line = line.substr(ind, hashtag_ind - ind);
    }
    bool in_string = false;
    size_t end_ind;
    if (line[ind] == '[') {
      end_ind = line.find_last_of(']');
      if (end_ind == std::string::npos) {
        Result.validity = false;
        return Result;
      }
    } else {
      end_ind = ind;
      while (end_ind < len) {
        if ((isspace(line[end_ind]) || line[end_ind] == '#') && !in_string) break;
        if (line[end_ind] == '"') in_string = !in_string;
        end_ind++;
      }
    }
    Section Check = getValue(line.substr(ind, end_ind - ind));
    for (end_ind++; end_ind < len; ++end_ind) {
      if (line[end_ind] == '#') break;
      if (std::isspace(line[end_ind])) continue;
      Result.validity = false;
      return Result;
    }
    if (std::holds_alternative<std::monostate>(Check.value)) {
      Result.validity = false;
      return Result;
    } else {
      if (!std::holds_alternative<std::map<std::string, Section>>(CurSection->value)) {
        CurSection->value = std::map<std::string, Section>();
      }
      std::get<std::map<std::string, Section>>(CurSection->value)[key] = Check;
    }
  }
  return Result;
}

Section parse(const std::filesystem::path& path) {
  std::ifstream file(path);
  file.seekg(0, std::ios::end);
  size_t size = file.tellg();
  std::string s(size, ' ');
  file.seekg(0);
  file.read(&s[0], size);
  return parse(s);
}

Section& Section::Get(const std::string& key) const {
  size_t ind_dot = key.find_first_of('.');
  if (ind_dot != std::string::npos) {
    return std::get<std::map<std::string, Section>>(value).at(key.substr(0, ind_dot)).Get(key.substr(ind_dot + 1));
  }
  return const_cast<Section&>(std::get<std::map<std::string, Section>>(value).at(key));
}

bool Section::IsInt() const {
  return std::holds_alternative<int>(value);
}

bool Section::IsFloat() const {
  return std::holds_alternative<float>(value);
}

bool Section::IsString() const {
  return std::holds_alternative<std::string>(value);
}

bool Section::IsBool() const {
  return std::holds_alternative<bool>(value);
}

bool Section::IsArray() const {
  return std::holds_alternative<std::vector<Section>>(value);
}

bool Section::valid() const {
  return validity;
}

int Section::AsInt() const {
  return std::get<int>(value);
}

int Section::AsIntOrDefault(int val) const {
  if (IsInt()) return std::get<int>(value);
  return val;
}

float Section::AsFloat() const {
  return std::get<float>(value);
}

float Section::AsFloatOrDefault(float val) const {
  if (IsFloat()) return std::get<float>(value);
  return val;
}

const std::string& Section::AsString() const {
  return std::get<std::string>(value);
}

std::string Section::AsStringOrDefault(const std::string& val) const {
  if (IsString()) return std::get<std::string>(value);
  return val;
}

bool Section::AsBool() const {
  return std::get<bool>(value);
}

bool Section::AsBoolOrDefault(bool val) const {
  if (IsBool()) return std::get<bool>(value);
  return val;
}

}