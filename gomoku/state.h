#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include "constants.h"
#include "policy.h"
#include "sim.h"
#include "util.h"

#define STRATEGY_BALANCE 1
#define STRATEGY_APPROACH 2

namespace mcts
{

class State
{
public:
  typedef std::vector<char> Row;
  typedef std::vector<Row> Position;

  int board_height;
  int board_width;
  Position position;
  char agent_id;

  State(int board_height, int board_width, char agent_id);
  State(int board_height, int board_width, const Position& position,
        char agent_id);
  State(const State& other);

  void get_expanded_states(std::vector<State> &expanded_states,
                           int strategy) const;
  void simulate(std::vector<double> &payoffs) const;
};

}

#endif // STATE_H_INCLUDED
