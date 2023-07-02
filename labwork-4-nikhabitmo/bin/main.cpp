#include "lib/parsing.h"
#include "lib/archive.h"
#include <iostream>

void showArguments(const BasicArguments& Arguments) {
  std::cout << "Creation : " << Arguments.creation << '\n';
  std::cout << "List : " << Arguments.list_out << '\n';
  std::cout << "Extract : " << Arguments.extraction << '\n';
  std::cout << "Append : " << Arguments.appendix << '\n';
  std::cout << "Delete : " << Arguments.deletion << '\n';
  std::cout << "Concatenate : " << Arguments.concatenate << '\n';
  std::cout << "Files : ";
  for (auto& all_file_heading : Arguments.all_file_headings)
    std::cout << all_file_heading << '\t';
  std::cout << '\n';
  std::cout << "Archives : " << Arguments.archives_to_concatenate.first << '\t'
            << Arguments.archives_to_concatenate.second << '\n';
  std::cout << "Code word length : " << Arguments.code_word_len << '\n';
  std::cout << "Current archive : " << Arguments.current_archive_name << '\n';
  std::cout << '\n';
}

int main(int argc, char** argv) {
  BasicArguments Arguments = parse(argc, argv);
  showArguments(Arguments);
  Archive MainArchive(Arguments);
  if (Arguments.creation) {
    MainArchive.Create();
  } else if (Arguments.extraction) {
    MainArchive.Extract();
  } else if (Arguments.appendix) {
    MainArchive.Append();
  } else if (Arguments.deletion) {
    MainArchive.Del();
  } else if (Arguments.concatenate) {
    MainArchive.Concatenate();
  } else if (Arguments.list_out) {
    MainArchive.List();
  }

  MainArchive.ShowArchiveInfo();
  return 0;
}