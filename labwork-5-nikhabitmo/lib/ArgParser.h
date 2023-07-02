#pragma once

#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <variant>
#include <vector>



namespace ArgumentParser {

class ArgParser {
 public:
  std::string parser_name;

  explicit ArgParser(const std::string& parser_name);

  ArgParser& AddStringArgument(const std::string& param, const std::string& descr = "");
  ArgParser& AddStringArgument(char ch, const std::string& param, const std::string& descr = "");

  ArgParser& Default(const char* value);
  ArgParser& Default(int value);
  ArgParser& Default(bool value);

  ArgParser& AddIntArgument(const std::string& param, const std::string& descr = "");
  ArgParser& AddIntArgument(char ch, const std::string& param, const std::string& descr = "");

  ArgParser& AddFlag(const std::string& param, const std::string& descr = "");
  ArgParser& AddFlag(char ch, const std::string& param, const std::string& descr = "");

  ArgParser& MultiValue(int min_args = 1);

  ArgParser& StoreValues(std::vector<int>& vector);
  ArgParser& StoreValues(std::vector<std::string>& vector);

  std::string GetStringValue(const std::string& param);
  std::string GetStringValue(const std::string& param, int index);

  int GetIntValue(const std::string& param);
  int GetIntValue(const std::string& param, int index);

  bool GetFlag(const std::string& param);

  bool Parse(const std::vector<std::string>& arguments);
  bool Parse(int args_counter, char** args);

  ArgParser& StoreValue(std::string& value);
  ArgParser& StoreValue(int& value);
  ArgParser& StoreValue(bool& value);

  ArgParser& Positional();

  void AddHelp(char help_short_arg, const std::string& help_long_arg, const std::string& descr = "");
  void AddHelp(const std::string& help_long_arg, const std::string& descr = "");
  std::string HelpDescription();

  bool Help() const;

 private:

  class Arguments {
   public:
    std::string long_argument;
    char short_argument;
    Arguments(std::string  l_arg, char s_arg);
  };

  std::vector<std::pair<Arguments, std::variant<int,
                                                std::string,
                                                bool,
                                                int*,
                                                std::string*,
                                                bool*,
                                                std::vector<int>,
                                                std::vector<std::string>,
                                                std::vector<int>*,
                                                std::vector<std::string>*>>> args_values_;

  std::vector<int> counter_arguments;
  std::string description;
  std::pair<char, std::string> help_arguments;
  std::vector<std::string> description_arguments;
  size_t positional_index_ = std::numeric_limits<size_t>::max();
  bool help_is_initialized = false;

  void AssignValue(const std::string&, size_t ind);
};

}