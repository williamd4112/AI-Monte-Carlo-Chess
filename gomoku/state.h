#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include <stdexcept>

#include "constants.h"

namespace mcts
{

class State
{
public:
  std::vector<std::vector<char> > position;
  char agent_id;

  State(int board_height, int board_width, char agent_id):
    agent_id(agent_id)
  {
    position = std::vector<std::vector<char>>(board_height,
               std::vector<char>(board_width, EMPTY));
  }

  State(const State& other):
    position(other.position),
    agent_id(other.agent_id)
  {
  }

  char get_agent_id() const
  {
    return agent_id;
  }

  void get_expanded_states(std::vector<State> &expanded_states) const
  {
    ///TODO Implement expansion
    expanded_states.clear();
    State new_state(*this);
    expanded_states.push_back(new_state);
  }

  void simulate(std::vector<double> &payoffs) const
  {
    ///TODO Implement simulation
    payoffs[0] = 1.0;
    payoffs[1] = 1.0;
  }
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
  return strm;
}

}

#endif // STATE_H_INCLUDED
