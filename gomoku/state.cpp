#include <stdexcept>

#include "state.h"

namespace mcts
{

State::State(int board_height, int board_width, char agent_id):
  board_height(board_height),
  board_width(board_width),
  agent_id(agent_id)
{
  position = Position(board_height, Row(board_width, EMPTY));
}

State::State(int board_height, int board_width, const Position& position,
             char agent_id):
  board_height(board_height),
  board_width(board_width),
  position(position),
  agent_id(agent_id)
{
}

State::State(const State& other):
  board_height(other.board_height),
  board_width(other.board_width),
  position(other.position),
  agent_id(other.agent_id)
{
}

void State::get_expanded_states(std::vector<State> &expanded_states,
                                int strategy) const
{
  expanded_states.clear();
  Policy policy(board_height, board_width);
  State new_state(*this);
  if (strategy == STRATEGY_BALANCE) {
    policy.move_balance(new_state, expanded_states);
  } else if (strategy == STRATEGY_APPROACH) {
    policy.move_approach_ex(new_state, expanded_states);
  } else {
    throw std::invalid_argument("Unknown strategy value");
  }
}

void State::simulate(std::vector<double> &payoffs) const
{
  Policy policy(board_height, board_width);
  int ret = sim_rapid_until_end(policy, *this, 100, 4);
  payoffs[BLACK] = 0.0;
  payoffs[WHITE] = 0.0;
  if (ret == BLACK) {
    payoffs[BLACK] = 1.0;
  } else if (ret == WHITE) {
    payoffs[WHITE] = 1.0;
  } else {
    payoffs[BLACK] = 0.5;
    payoffs[WHITE] = 0.5;
  }
}

std::ostream& operator<<(std::ostream &strm, const State& obj)
{
  const auto& position = obj.position;
  print_position(strm, position);
  return strm;
}

}
