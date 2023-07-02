#include "lib/parsing.h"
#include "lib/sandpile.h"
#include "lib/converting_bmp.h"
#include <string>
#include <deque>
int main(int argc, char **argv){
  BasicArguments Arguments = parse(argc, argv);
  std::deque<std::deque<uint64_t>> deque (Arguments.length, std::deque<uint64_t>(Arguments.width));
  std::deque<std::pair<uint16_t, uint16_t>> topple;
  readingTSV(Arguments.input,deque,topple);
  sandpileTopple(deque, topple, Arguments);
  imageCreation(deque, "final", Arguments.output);
  return 0;
}