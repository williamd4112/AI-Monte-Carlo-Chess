#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include "constants.h"

namespace mcts
{

class State
{
public:
  State(int board_height, int board_width)
  {
    position = std::vector<std::vector<char>>(board_height,
               std::vector<char>(board_width, EMPTY));
  }

  State(const State& other):
    position(other.position)
  {
  }

  bool is_terminal() const
  {
    return false;
  }

  std::vector<std::vector<char> > position;
};

std::ostream& operator<<(std::ostream &strm, const State& obj)
{
  const auto& position = obj.position;
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
      strm << val << " ";
    }
    strm << '\n';
    y += 1;
  }
  return strm;
}

}

#endif // STATE_H_INCLUDED
