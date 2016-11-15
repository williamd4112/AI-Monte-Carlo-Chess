#ifndef _POLICY_H_
#define _POLICY_H_

#include "state.h"
#include "debug.h"
#include "constants.h"
#include "fast_tss.h"

#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <ctime>

#define ONE_STEP_WIN 1
#define DEFAULT_TSS_MAX_DEPTH 255
#define OPPONENT_TSS_MAX_DEPTH 255
#define POLICY_SUCCESS 0x1
#define POLICY_FAIL 0x0

/*
 * policy.h
 * policy is used to generate possible states.
 *
 * */
namespace mcts
{
class State;
struct threat_t;
typedef std::pair<int, int> move_t;

class Policy
{
public:
  Policy(int w, int h);
  ~Policy();

  void reshuffle();
  int move_random(const State & state, std::vector<move_t> & moves, int max_moves=5);
  int move_random(const State & state, std::vector<State> & next_states, int max_moves=5);
  int move_rapid(const State & opponent_state, std::vector<State> & next_states, int max_random_moves=5);
  int move_rapid(const State & opponent_state, std::vector<move_t> & next_moves, int max_random_moves=5);
  int move_defensive(const State & opponent_state, std::vector<State> & next_states, int max_depth=DEFAULT_TSS_MAX_DEPTH);
  int move_defensive(const State & opponent_state, std::vector<std::pair<int, int>> & next_moves, int max_depth=DEFAULT_TSS_MAX_DEPTH);
private:
  std::vector<std::pair<int, int>> m_random_seq;

  int move_threats(const State & state, const std::vector<threat_t> & threats, std::vector<State> & next_states);
  int move_threats(const State & state, const std::vector<threat_t> & threats, std::vector<move_t> & next_moves);
  int move_middle(const State & state, std::vector<State> & next_states);
  int move_middle(const State & state, std::vector<move_t> & next_moves);
  int move_approach(const State & state, std::vector<State> & next_states);
  int move_approach(const State & state, std::vector<move_t> & next_moves);
};

}

#endif
