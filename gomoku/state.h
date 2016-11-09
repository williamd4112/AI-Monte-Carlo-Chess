#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include <stdexcept>

#include "constants.h"
#include "simulation.h"
#include "simulation_usual.h"
#include "util.h"

namespace mcts
{

class State
{
public:
  typedef std::vector<std::vector<char> > Position;

  int board_height;
  int board_width;
  Position position;
  char agent_id;

  State(int board_height, int board_width, char agent_id):
    board_height(board_height),
    board_width(board_width),
    agent_id(agent_id)
  {
    position = Position(board_height, std::vector<char>(board_width, EMPTY));
  }

  State(int board_height, int board_width, const Position& position,
        char agent_id):
    board_height(board_height),
    board_width(board_width),
    position(position),
    agent_id(agent_id)
  {
  }

  State(const State& other):
    board_height(other.board_height),
    board_width(other.board_width),
    position(other.position),
    agent_id(other.agent_id)
  {
  }

  void get_expanded_states(std::vector<State> &expanded_states) const
  {
    /* Has been visited during expansion (to avoid duplicated state) */
    std::vector<std::vector<bool>> visited = std::vector<std::vector<bool>>(board_height,
                                std::vector<bool>(board_width, false));

    expanded_states.clear();
    for (int i = 0; i < board_height; i++) {
      for (int j = 0; j < board_width; j++) {
        if (position[i][j] != EMPTY) {
          expand_around(expanded_states, visited, i, j);
        }
      }
    }

    /* If no chess on the board, set in middle */
    if (expanded_states.empty()) {
      State new_state(*this);
      new_state.position[board_height / 2][board_width / 2] = agent_id;
      expanded_states.push_back(new_state);
    }
  }

  void simulate(std::vector<double> &payoffs) const
  {
    double WIN = 1.0;
    double LOSE = 0.0;
    double TIE = (WIN+LOSE)/2;
    payoffs[BLACK] = 1.0;
    payoffs[WHITE] = 1.0;

    std::vector<std::vector<char>> map = position;
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

private:
  void expand_around(std::vector<State> & expanded_states,
                     std::vector<std::vector<bool>> & visited,
                     int row,
                     int col) const
  {
    for (int i = row - EXPAND_AROUND_RANGE; i < row + EXPAND_AROUND_RANGE; i++) {
      for (int j = col - EXPAND_AROUND_RANGE; j < col + EXPAND_AROUND_RANGE; j++) {
        if (i < 0 || i >= board_height || j < 0 || j >= board_width) {
          continue;
        }

        if (position[i][j] == EMPTY && !visited[i][j]) {
          State new_state(*this);
          new_state.position[i][j] = agent_id;
          expanded_states.push_back(new_state);
          visited[i][j] = true;
        }
      }
    }
  }
};

std::ostream& operator<<(std::ostream &strm, const State& obj)
{
  const auto& position = obj.position;
  print_position(strm, position);
  return strm;
}

}

#endif // STATE_H_INCLUDED
