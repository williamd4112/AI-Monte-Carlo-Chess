#ifndef _POLICY_H_
#define _POLICY_H_

#include "state.h"
#include "debug.h"
#include "constants.h"
#include "fast_tss.h"

#include <vector>
#include <algorithm>
#include <random>
#include <ctime>

#define ONE_STEP_WIN 1
#define DEFAULT_TSS_MAX_DEPTH 255
#define OPPONENT_TSS_MAX_DEPTH 255
#define POLICY_SUCCESS 0x1
#define POLICY_FAIL 0x0
#define RANDOM_SEARCH_RANGE 3

/*
 * policy.h
 * policy is used to generate possible states.
 *
 * */
namespace mcts {

  std::ostream & operator << (std::ostream & os, std::vector<Tss::threat_t> & threats)
  {
    for (auto & t : threats) {
      os << "Gain(" << t.point.i << ", " << t.point.j << ")" << std::endl;
    }
    return os;
  }

  int find_one_step_winning(const std::vector<Tss::threat_t> & threats)
  {
    for (int i = 0; i < (int)threats.size(); i++) {
      if (threats[i].winning) {
        return i;
      }
    }
    return -1;
  }

  void find_winning_sequence(const std::vector<Tss::threat_t> & threats, std::vector<Tss::threat_t> & seq)
  {
    for (const Tss::threat_t & threat : threats) {
      DEBUG_POLICY("Threat(%d, %d): [%d, %d, %d]\n", threat.point.i, threat.point.j, threat.winning, threat.final_winning, threat.min_winning_depth);
      if (threat.final_winning) {
        seq.push_back(threat);
      }
    }
  }

  void expand_threat_to_states(const Tss::threat_t & threat, const State & root_state, std::vector<State> & states)
  {
      const Tss::point_t gain = threat.point;
      State new_state(root_state);
      new_state.position[gain.i][gain.j] = root_state.agent_id;
      states.push_back(new_state);
  }

  void expand_threats_to_states(const std::vector<Tss::threat_t> & threats, const State & root_state, std::vector<State> & states)
  {
    for (const Tss::threat_t & threat : threats) {
      expand_threat_to_states(threat, root_state, states);
    }
  }

  int policy_attack_middle(const State & state, std::vector<State> & next_states)
  {
    DEBUG_POLICY("Policy middle (%d)\n", state.agent_id);
    int mr = state.board_height / 2;
    int mc = state.board_width / 2;
    if (state.position[mr][mc] == EMPTY){
        State new_state(state);
        new_state.position[mr][mc] = state.agent_id;
        next_states.push_back(new_state);
    }
    return next_states.empty() ? POLICY_FAIL : POLICY_SUCCESS;
  }

  int policy_attack_random(const State & state, std::vector<State> & next_states, int random_range=RANDOM_SEARCH_RANGE)
  {
    /// TODO : implement random policy
    int res = POLICY_FAIL;
    int w = state.board_width;
    int h = state.board_height;
    for (int i = 0 ; i < state.board_height; i++) {
      for (int j = 0; j < state.board_width; j++) {
        if (state.position[i][j] != EMPTY) {
          for (int r = i - random_range; r < i + random_range; r++) {
            for (int c = j - random_range; c < j + random_range; c++) {
              if (r < 0 || r >= h || c < 0 || c >= w)
                  continue;
              if (state.position[r][c] == EMPTY) {
                State new_state(state);
                new_state.position[r][c] = state.agent_id;
                next_states.push_back(new_state);
                res = POLICY_SUCCESS;
              }
            }
          }
        }
      }
    }
    if (!next_states.empty()) {
        std::shuffle(next_states.begin(), next_states.end(), std::mt19937(static_cast<int>(time(0))));
    }
    DEBUG_POLICY("Policy random (%d) = %d\n", state.agent_id, res);
    return res;
  }

  int policy_attack_approach(const State & state, std::vector<State> & next_states)
  {
    /// TODO : implement approach policy
    DEBUG_POLICY("Policy approach (%d)\n", state.agent_id);
    int result = POLICY_FAIL;
    std::vector<Tss::threat_t> approaches;
    Tss approach_tss(state);
    approach_tss.find_all_threats(state.position, approaches, THREAT_LEVEL_2, THREAT_LEVEL_2, 1);

    if (!approaches.empty()) {
      expand_threats_to_states(approaches, state, next_states);
      result = POLICY_SUCCESS;
    }
    std::shuffle(next_states.begin(), next_states.end(), std::mt19937(static_cast<int>(time(0))));
    return result;
  }

  int policy_attack(const State & state, const std::vector<Tss::threat_t> & threats, std::vector<State> & next_states)
  {
    int result = POLICY_FAIL;
    if (!threats.empty()) {
        expand_threats_to_states(threats, state, next_states);
        result = POLICY_SUCCESS;
    }
    return result;
  }

  int policy_aggresive(const State & opponent_state, std::vector<State> & next_states, int max_depth=DEFAULT_TSS_MAX_DEPTH)
  {
    int res = POLICY_FAIL;

    State self_state(opponent_state);
    self_state.agent_id ^= (1 << 0);

    /* Search opponent winning sequence */
    std::vector<Tss::threat_t> opponent_threats;
    std::vector<Tss::threat_t> opponent_winning_seq;
    Tss opponent_tss(opponent_state);
    opponent_tss.find_all_threats(opponent_state.position, opponent_threats, THREAT_LEVEL_3, THREAT_LEVEL_5, max_depth);
    std::sort(opponent_threats.begin(), opponent_threats.end(), std::greater<Tss::threat_t>());
    DEBUG_POLICY("Opponent winning seq\n");
    find_winning_sequence(opponent_threats, opponent_winning_seq);

    /* Search self winning sequences */
    std::vector<Tss::threat_t> self_threats;
    std::vector<Tss::threat_t> self_winning_seq;
    Tss self_tss(self_state);
    self_tss.find_all_threats(self_state.position, self_threats, THREAT_LEVEL_3, THREAT_LEVEL_5, max_depth);
    std::sort(self_threats.begin(), self_threats.end(), std::greater<Tss::threat_t>());
    DEBUG_POLICY("Self winning seq\n");
    find_winning_sequence(self_threats, self_winning_seq);

    /* Compare winning seq depth, attack/defend by winning seq */
    int opponent_min_winning_depth = (opponent_winning_seq.empty()) ? INT_MAX : opponent_winning_seq.front().min_winning_depth;
    int self_min_winning_depth = (self_winning_seq.empty()) ? INT_MAX : self_winning_seq.front().min_winning_depth;

    if (self_min_winning_depth <= opponent_min_winning_depth && !self_winning_seq.empty()) {
        DEBUG_POLICY("Attack winning (%d)\n", self_state.agent_id);
        expand_threat_to_states(self_winning_seq.front(), self_state, next_states);
        res = POLICY_SUCCESS;
    }
    else if (!opponent_winning_seq.empty()){
        DEBUG_POLICY("Defend winning (%d)\n", self_state.agent_id);
        expand_threat_to_states(opponent_winning_seq.front(), self_state, next_states);
        res = POLICY_SUCCESS;
    }

    /* No emergent status, attack with self threats */
    if (res != POLICY_SUCCESS) {
        DEBUG_POLICY("Attack (%d)\n", self_state.agent_id);
        res = policy_attack(self_state, self_threats, next_states);
    }

    /* No self threats, defend opponent threats */
    if (res != POLICY_SUCCESS) {
        DEBUG_POLICY("Defend (%d)\n", self_state.agent_id);
        res = policy_attack(self_state, opponent_threats, next_states);
    }

    /* No threats */
    if (res != POLICY_SUCCESS){
        DEBUG_POLICY("No threats (%d)\n", self_state.agent_id);
        res = policy_attack_approach(self_state, next_states);
        if (res != POLICY_SUCCESS) {
            res = policy_attack_random(self_state, next_states);
        }
        if (res != POLICY_SUCCESS) {
            res = policy_attack_middle(self_state, next_states);
        }
    }

    DEBUG_POLICY("Policy default = %d\n",res);
    return res;
  }
}

#endif

