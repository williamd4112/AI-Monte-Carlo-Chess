#ifndef _POLICY_H_
#define _POLICY_H_

#include "state.h"
#include "debug.h"
#include "constants.h"
#include "fast_tss.h"

#include <vector>
#include <algorithm>

#define ONE_STEP_WIN 1
#define DEFAULT_TSS_MAX_DEPTH 255
#define OPPONENT_TSS_MAX_DEPTH 255
#define POLICY_SUCCESS 0x1
#define POLICY_FAIL 0x0
#define RANDOM_SEARCH_RANGE 2

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
    State new_state(state);
    new_state.position[state.board_height / 2][state.board_width / 2] = state.agent_id;
    next_states.push_back(new_state);
    return POLICY_SUCCESS;
  }

  int policy_attack_random(const State & state, std::vector<State> & next_states)
  {
    /// TODO : implement random policy
    DEBUG_POLICY("Policy random (%d)\n", state.agent_id);

    for (int i = 0 ; i < state.board_height; i++) {
      for (int j = 0; j < state.board_width; j++) {
        if (state.position[i][j] != EMPTY) {
          for (int r = i - RANDOM_SEARCH_RANGE; r < i + RANDOM_SEARCH_RANGE; r++) {
            for (int c = j - RANDOM_SEARCH_RANGE; c < j + RANDOM_SEARCH_RANGE; c++) {
              State new_state(state);
              new_state.position[r][c] = state.agent_id;
              next_states.push_back(new_state);
            }
          }
        }
      }
    }

    return next_states.empty() ? POLICY_FAIL : POLICY_SUCCESS;
  }

  int policy_attack_approach(const State & state, std::vector<State> & next_states)
  {
    /// TODO : implement approach policy
    DEBUG_POLICY("Policy approach (%d)\n", state.agent_id);
    int result = 0;
    std::vector<Tss::threat_t> approaches;
    Tss approach_tss(state);
    approach_tss.find_all_threats(state.position, approaches, THREAT_LEVEL_2, THREAT_LEVEL_2, 1);

    if (!approaches.empty()) {
      expand_threats_to_states(approaches, state, next_states);
      result = 1;
    }

    return result;
  }

  int policy_defend_winning_seq(const State & state, const std::vector<Tss::threat_t> & threats, std::vector<State> & next_states)
  {
    std::vector<Tss::threat_t> winning_seq;
    find_winning_sequence(threats, winning_seq);

    if (winning_seq.empty()) {
      DEBUG_POLICY("No winning sequence\n");
      expand_threats_to_states(threats, state, next_states);
    }
    else {
      DEBUG_POLICY("Winning sequence detect\n");
      expand_threats_to_states(winning_seq, state, next_states);
    }

    return POLICY_SUCCESS;
  }

  int policy_defend_urgent_move(const State & state, const std::vector<State> & next_states)
  {
    int result = POLICY_FAIL;

    return result;
  }

  int policy_defend(const State & state, const std::vector<Tss::threat_t> & threats, std::vector<State> & next_states)
  {
    int result = 0;
    int one_step_win_threat_index = find_one_step_winning(threats);
    if (one_step_win_threat_index < 0) {
      result = policy_defend_winning_seq(state, threats, next_states);
    }
    else {
      expand_threat_to_states(threats[one_step_win_threat_index], state, next_states);
      result = ONE_STEP_WIN;
    }
    return result;
  }

  int policy_attack(const State & state, const std::vector<Tss::threat_t> & threats, std::vector<State> & next_states)
  {
    int result = 0;
    if (!threats.empty()) {
      result = policy_defend_winning_seq(state, threats, next_states);
    }
    else {
      /// TODO : Implement policy when no threats
      DEBUG_POLICY("No threats, approach\n");
      if (policy_attack_approach(state, next_states) != POLICY_SUCCESS) {
        if (policy_attack_random(state, next_states) != POLICY_SUCCESS) {
          policy_attack_middle(state, next_states);
        }
      }
    }
    return result;
  }

  int policy_default(const State & opponent_state, std::vector<State> & next_states)
  {
    int result = 0;

    /* Search opponent winning sequence */
    std::vector<Tss::threat_t> opponent_threats;
    Tss opponent_tss(opponent_state);
    opponent_tss.find_all_threats(opponent_state.position, opponent_threats, THREAT_LEVEL_3, THREAT_LEVEL_5, OPPONENT_TSS_MAX_DEPTH);
    std::sort(opponent_threats.begin(), opponent_threats.end(), std::greater<Tss::threat_t>());
    DEBUG_POLICY("Opponent threats : %d\n", opponent_threats.size());
    std::cout << opponent_threats << std::endl;
    for (auto t : opponent_threats)
        t.print();

    /* Search self winning sequence */
    State self_state(opponent_state);
    self_state.agent_id = opponent_state.agent_id ^ (1 << 0);

    std::vector<Tss::threat_t> self_threats;
    Tss self_tss(self_state);
    self_tss.find_all_threats(self_state.position, self_threats, THREAT_LEVEL_3, THREAT_LEVEL_5, DEFAULT_TSS_MAX_DEPTH);
    std::sort(self_threats.begin(), self_threats.end(), std::greater<Tss::threat_t>());
    DEBUG_POLICY("Self threats : %d\n",self_threats.size());
    std::cout << self_threats << std::endl;

    DEBUG_POLICY("Opponent id = %d\n", opponent_state.agent_id);
    DEBUG_POLICY("Self id = %d\n", self_state.agent_id);

    /* Check self one step winning */
    int one_step_win_threat_index = find_one_step_winning(self_threats);
    if (one_step_win_threat_index < 0) {
      /// TODO Merge opponent threats and self threats to find optimal move
      DEBUG_POLICY("No-one step winning:\n");

      /* Defend when opponent_threats is not empty */
      if (!opponent_threats.empty()) {
        DEBUG_POLICY("Policy defend\n");
        /* Find threats using opponent state, expand using self_state */
        result = policy_defend(self_state, opponent_threats, next_states);
      }
      /* Attack when opponent threats is empty, and no one step winning */
      else {
        DEBUG_POLICY("Policy attack\n");
        result = policy_attack(self_state, self_threats, next_states);
      }
    }
    else {
      DEBUG_POLICY("One step winning:\n");
      expand_threat_to_states(self_threats[one_step_win_threat_index], self_state, next_states);
      result = ONE_STEP_WIN;
    }
    return result;
  }
}

#endif

