#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <iostream>

namespace mcts
{

typedef std::vector<std::vector<char> > Position;

static void print_position(std::ostream& strm,
                           const Position& position)
{
  size_t width = 0;
  if (!position.empty()) {
    width = position[0].size();
  }
  strm << "  ";
  for (size_t i = 0; i < width; ++i) {
    strm << (char)('A' + i) << " ";
  }
  strm << '\n';
  strm << "  ";
  for (size_t i = 0; i < width; ++i) {
    strm << (i % 10) << " ";
  }
  strm << '\n';
  int y = 0;
  for (const auto& row : position) {
    strm << (y % 10) << " ";
    for (const auto& val: row) {
      if (val == 0) {
        strm << 'O';
      } else if (val == 1) {
        strm << 'X';
      } else if (val == 2) {
        strm << '.';
      } else {
        throw std::runtime_error(std::string("Unknown value ") + val);
      }
      strm << " ";
    }
    strm << '\n';
    y += 1;
  }
}

}

#endif // UTIL_H_INCLUDED
