#include "ArgParser.h"

namespace ArgumentParser {

bool ArgParser::Parse(int args_counter, char** args) {
  std::vector<std::string> vec;
  for (int i = 0; i < args_counter; ++i) {
    vec.emplace_back(args[i]);
  }
  return Parse(vec);
}

bool ArgParser::Parse(const std::vector<std::string>& arguments) {
  for (size_t i = 1; i < arguments.size(); ++i) { // help check
    if (arguments[i].length() == 2 && arguments[i][0] == '-' && arguments[i][1] == help_arguments.first
        || arguments[i] == ("--" + help_arguments.second)) {
      help_is_initialized = true;
      return true;
    }
    if (arguments[i][0] != '-') { // positional
      if (positional_index_ != std::numeric_limits<size_t>::max()) {
        AssignValue(arguments[i], positional_index_);
      } else {
        return false;
      }
    } else { // not positional
      size_t ind_eq = arguments[i].find('=');
      if (arguments[i][1] == '-') { // long arguments and flags
        if (ind_eq != std::string::npos) { // arguments
          std::string cur_arg = arguments[i].substr(2, ind_eq - 2);
          std::string cur_value = arguments[i].substr(ind_eq + 1);
          bool found = false;
          for (size_t j = 0; j < args_values_.size(); ++j) {
            if (args_values_[j].first.long_argument == cur_arg) {
              AssignValue(cur_value, j);
              found = true;
              break;
            }
          }
          if (!found) {
            return false;
          }
        } else { // flags
          std::string cur_arg = arguments[i].substr(2, ind_eq - 2);
          bool found = false;
          for (size_t j = 0; j < args_values_.size(); ++j) {
            if (args_values_[j].first.long_argument == cur_arg) {
              AssignValue("true", j);
              found = true;
              break;
            }
          }
          if (!found) {
            return false;
          }
        }
      } else { // short arguments and flags
        if (ind_eq != std::string::npos) { // arguments
          std::string cur_arg = arguments[i].substr(1, ind_eq - 1);
          std::string cur_value = arguments[i].substr(ind_eq + 1);
          bool found = false;
          for (size_t j = 0; j < args_values_.size(); ++j) {
            if (std::string(1, args_values_[j].first.short_argument) == cur_arg) {
              AssignValue(cur_value, j);
              found = true;
              break;
            }
          }
          if (!found) {
            return false;
          }
        } else { // flags
          size_t len = arguments[i].length();
          for (size_t k = 1; k < len; ++k) {
            char cur_arg = arguments[i][k];
            bool found = false;
            for (size_t j = 0; j < args_values_.size(); ++j) {
              if (args_values_[j].first.short_argument == cur_arg) {
                AssignValue("true", j);
                found = true;
                break;
              }
            }
            if (!found) {
              return false;
            }
          }
        }
      }
    }
  }
  for (auto counter_argument : counter_arguments) {
    if (counter_argument > 0) {
      return false;
    }
  }
  return true;
}

ArgParser::ArgParser(const std::string& description) {
  ArgParser::parser_name = description;
}

ArgParser& ArgParser::AddStringArgument(const std::string& param, const std::string& descr) {
  args_values_.push_back({{param, ' '}, " "});
  counter_arguments.push_back(1);
  description_arguments.push_back(descr);
  return *this;
}

ArgParser& ArgParser::AddStringArgument(char ch, const std::string& param, const std::string& descr) {
  args_values_.push_back({{param, ch}, " "});
  counter_arguments.push_back(1);
  description_arguments.push_back(descr);
  return *this;
}

ArgParser& ArgParser::AddIntArgument(const std::string& param, const std::string& descr) {
  args_values_.push_back({{param, ' '}, 0});
  counter_arguments.push_back(1);
  description_arguments.push_back(descr);
  return *this;
}

ArgParser& ArgParser::AddIntArgument(char ch, const std::string& param, const std::string& descr) {
  args_values_.push_back({{param, ch}, 0});
  counter_arguments.push_back(1);
  description_arguments.push_back(descr);
  return *this;
}

ArgParser& ArgParser::AddFlag(const std::string& param, const std::string& descr) {
  args_values_.push_back({{param, ' '}, false});
  counter_arguments.push_back(0);
  description_arguments.push_back(descr);
  return *this;
}

ArgParser& ArgParser::AddFlag(char ch, const std::string& param, const std::string& descr) {
  args_values_.push_back({{param, ch}, false});
  counter_arguments.push_back(0);
  description_arguments.push_back(descr);
  return *this;
}

std::string ArgParser::GetStringValue(const std::string& param) {
  for (auto i : args_values_) {
    if (i.first.long_argument == param || std::string(1, i.first.short_argument) == param) {
      if (std::holds_alternative<std::string>(i.second)) {
        return std::get<std::string>(i.second);
      } else if (std::holds_alternative<std::string*>(i.second)) {
        return *std::get<std::string*>(i.second);
      }
    }
  }
  return "";
}

std::string ArgParser::GetStringValue(const std::string& param, int index) {
  for (auto i : args_values_) {
    if (i.first.long_argument == param || std::string(1, i.first.short_argument) == param) {
      if (std::holds_alternative<std::vector<std::string>*>(i.second)) {
        return (*std::get<std::vector<std::string>*>(i.second))[index];
      } else if (std::holds_alternative<std::vector<std::string>>(i.second)) {
        return std::get<std::vector<std::string>>(i.second)[index];
      }
    }
  }
  return "";
}

int ArgParser::GetIntValue(const std::string& param, int index) {
  for (auto i : args_values_) {
    if (i.first.long_argument == param || std::string(1, i.first.short_argument) == param) {
      if (std::holds_alternative<std::vector<int>*>(i.second)) {
        return (*std::get<std::vector<int>*>(i.second))[index];
      } else if (std::holds_alternative<std::vector<int>>(i.second)) {
        return std::get<std::vector<int>>(i.second)[index];
      }
    }
  }
  return 0;
}

int ArgParser::GetIntValue(const std::string& param) {
  for (auto i : args_values_) {
    if (i.first.long_argument == param || std::string(1, i.first.short_argument) == param) {
      if (std::holds_alternative<int>(i.second)) {
        return std::get<int>(i.second);
      } else if (std::holds_alternative<int*>(i.second)) {
        return *std::get<int*>(i.second);
      }
    }
  }
  return 0;
}

bool ArgParser::GetFlag(const std::string& param) {
  for (auto i : args_values_) {
    if (i.first.long_argument == param || std::string(1, i.first.short_argument) == param) {
      return std::get<bool>(i.second);
    }
  }
  return false;
}

ArgParser& ArgParser::Default(const char* value) {
  if (std::holds_alternative<std::string*>(args_values_[args_values_.size() - 1].second)) {
    *(std::get<std::string*>(args_values_[args_values_.size() - 1].second)) = value;
  } else {
    args_values_[args_values_.size() - 1].second = value;
  }
  counter_arguments[args_values_.size() - 1] = 0;
  description_arguments[description_arguments.size() - 1] += " [ default = " + std::string(value) + " ]";
  return *this;
}

ArgParser& ArgParser::Default(int value) {
  if (std::holds_alternative<std::string*>(args_values_[args_values_.size() - 1].second)) {
    *(std::get<int*>(args_values_[args_values_.size() - 1].second)) = value;
  } else {
    args_values_[args_values_.size() - 1].second = value;
  }
  counter_arguments[args_values_.size() - 1] = 0;
  description_arguments[description_arguments.size() - 1] += " [ default = " + std::to_string(value) + " ]";
  return *this;
}

ArgParser& ArgParser::Default(bool value) {
  args_values_[args_values_.size() - 1].second = value;
  counter_arguments[args_values_.size() - 1] = 0;
  description_arguments[description_arguments.size() - 1] +=
      " [ default = " + std::string(value ? "true" : "false") + " ] ";
  return *this;
}

ArgParser& ArgParser::StoreValue(std::string& value) {
  args_values_[args_values_.size() - 1].second = &value;
  description_arguments[description_arguments.size() - 1] += " [ value is stored ] ";
  return *this;
}

ArgParser& ArgParser::StoreValue(bool& value) {
  args_values_[args_values_.size() - 1].second = &value;
  description_arguments[description_arguments.size() - 1] += " [ value is stored ] ";
  return *this;
}

ArgParser& ArgParser::StoreValue(int& value) {
  args_values_[args_values_.size() - 1].second = &value;
  description_arguments[description_arguments.size() - 1] += " [ value is stored ] ";
  return *this;
}

ArgParser& ArgParser::MultiValue(int min_args) {
  if (std::holds_alternative<int>(args_values_[args_values_.size() - 1].second)) {
    std::vector<int> temp = {};
    args_values_[args_values_.size() - 1].second = temp;
  } else if (std::holds_alternative<std::string>(args_values_[args_values_.size() - 1].second)) {
    std::vector<std::string> temp = {};
    args_values_[args_values_.size() - 1].second = temp;
  }
  counter_arguments[args_values_.size() - 1] = min_args;
  description_arguments[description_arguments.size() - 1] +=
      "[ repeated, min args = " + std::to_string(min_args) + " ]";
  return *this;
}

ArgParser& ArgParser::Positional() {
  positional_index_ = args_values_.size() - 1;
  return *this;
}

ArgParser& ArgParser::StoreValues(std::vector<int>& vector) {
  std::vector<int>* temp = {};
  args_values_[args_values_.size() - 1].second = temp;
  std::get<std::vector<int>*>(args_values_[args_values_.size() - 1].second) = &vector;
  description_arguments[description_arguments.size() - 1] += " [ values are stored ] ";
  return *this;
}

ArgParser& ArgParser::StoreValues(std::vector<std::string>& vector) {
  std::get<std::vector<std::string>*>(args_values_[args_values_.size() - 1].second) = &vector;
  description_arguments[description_arguments.size() - 1] += " [ values are stored ] ";
  return *this;
}

void ArgParser::AssignValue(const std::string& value, size_t ind) {
  if (std::holds_alternative<std::string>(args_values_[ind].second)) {
    args_values_[ind].second = value;
  } else if (std::holds_alternative<int>(args_values_[ind].second)) {
    args_values_[ind].second = std::stoi(value);
  } else if (std::holds_alternative<bool>(args_values_[ind].second)) {
    args_values_[ind].second = true;
  } else if (std::holds_alternative<std::string*>(args_values_[ind].second)) {
    *(std::get<std::string*>(args_values_[ind].second)) = value;
  } else if (std::holds_alternative<int*>(args_values_[ind].second)) {
    *(std::get<int*>(args_values_[ind].second)) = std::stoi(value);
  } else if (std::holds_alternative<bool*>(args_values_[ind].second)) {
    *(std::get<bool*>(args_values_[ind].second)) = true;
  } else if (std::holds_alternative<std::vector<int>>(args_values_[ind].second)) {
    std::get<std::vector<int>>(args_values_[ind].second).push_back(std::stoi(value));
  } else if (std::holds_alternative<std::vector<std::string>>(args_values_[ind].second)) {
    std::get<std::vector<std::string>>(args_values_[ind].second).push_back(value);
  } else if (std::holds_alternative<std::vector<int>*>(args_values_[ind].second)) {
    std::get<std::vector<int>*>(args_values_[ind].second)->push_back(std::stoi(value));
  } else if (std::holds_alternative<std::vector<std::string>*>(args_values_[ind].second)) {
    std::get<std::vector<std::string>*>(args_values_[ind].second)->push_back(value);
  }
  counter_arguments[ind]--;
}

void ArgParser::AddHelp(char help_short_arg, const std::string& help_long_arg, const std::string& descr) {
  description = descr;
  help_arguments.first = help_short_arg;
  help_arguments.second = help_long_arg;
}

void ArgParser::AddHelp(const std::string& help_long_arg, const std::string& descr) {
  description = descr;
  help_arguments.first = ' ';
  help_arguments.second = help_long_arg;
}

std::string ArgParser::HelpDescription() {
  std::string res;
  res += parser_name + '\n';
  res += description + "\n\n";
  for (int i = 0; i < args_values_.size(); ++i) {
    res +=
        "-" + std::string(1, args_values_[i].first.short_argument) + ", --" + args_values_[i].first.long_argument + ", "
            + description_arguments[i] + '\n';
  }
  res += "\n-" + std::string(1, help_arguments.first) + ", --" + help_arguments.second + +", "
      + "Display this help and exit\n";
  return res;
}

bool ArgParser::Help() const {
  return help_is_initialized;
}

ArgParser::Arguments::Arguments(std::string l_arg, char s_arg)
    : long_argument(std::move(l_arg)), short_argument(s_arg) {};
}