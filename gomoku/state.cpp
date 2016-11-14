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

void State::get_expanded_states(std::vector<State> &expanded_states) const
{
  expanded_states.clear();
  Policy policy(board_height, board_width);
  int ret = policy.move_defensive(*this, expanded_states);
  if (ret == POLICY_FAIL) {
    return;
  }
}

void State::simulate(std::vector<double> &payoffs) const
{
  double WIN = 1.0;
  double LOSE = 0.0;
  double TIE = (WIN+LOSE)/2;
  payoffs[BLACK] = 1.0;
  payoffs[WHITE] = 1.0;

  Position map = position;
  char color = agent_id;

  while(true) {
    char result = who_win(map);
    if(result == TIE) { // Tie is extremely unusual
      payoffs[BLACK] = TIE;
      payoffs[WHITE] = TIE;
      break;
    } else if(result == BLACK) {
      payoffs[BLACK] = WIN;
      payoffs[WHITE] = LOSE;
      break;
    } else if(result == WHITE) {
      payoffs[BLACK] = LOSE;
      payoffs[WHITE] = WIN;
      break;
    } else {
      next_to_play(map,color);
      color = (color==BLACK)? WHITE: BLACK;
      //show_map(map);
    }
  }
}

std::ostream& operator<<(std::ostream &strm, const State& obj)
{
  const auto& position = obj.position;
  print_position(strm, position);
  return strm;
}

}
