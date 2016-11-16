#ifndef _POLICY_H_
#define _POLICY_H_

#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <ctime>

#include "board.h"
#include "constants.h"
#include "debug.h"
#include "state.h"

#define ONE_STEP_WIN 1
#define DEFAULT_TSS_MAX_DEPTH 12
#define OPPONENT_TSS_MAX_DEPTH 12
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

int find_one_step_winning(const std::vector<threat_t> & threats);
void find_winning_sequence(const std::vector<threat_t> & threats, std::vector<threat_t> & seq);
void find_winning_sequence_sorted(const State & opponent_state, const std::vector<threat_t> & threats, std::vector<threat_t> & seq);
void find_critical_winning_seq(
    const State & opponent_state,
    const std::vector<threat_t> & opponent_winning_seq, std::vector<threat_t> & opponent_critical_winning_seq);
void find_top_winning_seq(const std::vector<threat_t> & winning_seq, std::vector<threat_t> & top_winning_seq);

void expand_threat_to_states(const threat_t & threat, const State & root_state, std::vector<State> & states);
void expand_threat_to_moves(const threat_t & threat, std::vector<move_t> & moves);
void expand_threats_to_states(const std::vector<threat_t> & threats, const State & root_state, std::vector<State> & states);
void expand_threats_to_moves(const std::vector<threat_t> & threats, const State & root_state, std::vector<move_t> & moves);
void expand_moves_to_states(const std::vector<move_t> & moves, const State & root_state, std::vector<State> & states);

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
  int move_balance(const State & opponent_state, std::vector<State> & next_states, int max_depth=DEFAULT_TSS_MAX_DEPTH);
  int move_balance(const State & opponent_state, std::vector<std::pair<int, int>> & next_moves, int max_depth=DEFAULT_TSS_MAX_DEPTH);
  int move_approach_ex(const State & state, std::vector<State> & next_states, int num_samples=10);
private:
  std::vector<std::pair<int, int>> m_random_seq;
  std::mt19937 m_random_gen;

  int move_winning_seq(
    const State & opponent_state, std::vector<threat_t> & opponent_threats,
    const State & self_state, std::vector<threat_t> & self_threats,
    std::vector<move_t> & next_moves);
  int move_when_no_threats(const State & self_state, std::vector<move_t> & next_moves);
  int move_when_no_threats(const State & self_state, std::vector<State> & next_states);
  int move_threats(const State & state, const std::vector<threat_t> & threats, std::vector<State> & next_states);
  int move_threats(const State & state, const std::vector<threat_t> & threats, std::vector<move_t> & next_moves);
  int move_middle(const State & state, std::vector<State> & next_states);
  int move_middle(const State & state, std::vector<move_t> & next_moves);
  int move_approach(const State & state, std::vector<State> & next_states);
  int move_approach(const State & state, std::vector<move_t> & next_moves);
};

}

#endif
