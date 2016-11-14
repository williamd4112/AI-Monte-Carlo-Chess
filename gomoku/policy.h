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
#define RANDOM_SEARCH_RANGE 3

/*
 * policy.h
 * policy is used to generate possible states.
 *
 * */
namespace mcts {

  typedef std::pair<int, int> move_t;

  std::ostream & operator << (std::ostream & os, std::vector<Tss::threat_t> & threats)
  {
    for (auto & t : threats) {
      os << "Gain(" << t.point.i << ", " << t.point.j << ")" << std::endl;
    }
    return os;
  }

  static int find_one_step_winning(const std::vector<Tss::threat_t> & threats)
  {
    for (int i = 0; i < (int)threats.size(); i++) {
      if (threats[i].winning) {
        return i;
      }
    }
    return -1;
  }

  static void find_winning_sequence(const std::vector<Tss::threat_t> & threats, std::vector<Tss::threat_t> & seq)
  {
    for (const Tss::threat_t & threat : threats) {
      DEBUG_POLICY("Threat(%c, %d): [%d, %d, %d]\n", threat.point.i + 'A', threat.point.j, threat.winning, threat.final_winning, threat.min_winning_depth);
      if (threat.final_winning) {
        seq.push_back(threat);
      }
    }
  }

  static void find_winning_sequence_sorted(const std::vector<Tss::threat_t> & threats, std::vector<Tss::threat_t> & seq)
  {
    int min_depth = INT_MAX;
    for (const Tss::threat_t & threat : threats) {
      DEBUG_POLICY("Threat(%c, %d): [%d, %d, %d]\n", threat.point.i + 'A', threat.point.j, threat.winning, threat.final_winning, threat.min_winning_depth);
      if (threat.final_winning) {
        if(threat.min_winning_depth <= min_depth)
          seq.push_back(threat);
      }
      min_depth = threat.min_winning_depth;
    }
  }

  static void expand_threat_to_states(const Tss::threat_t & threat, const State & root_state, std::vector<State> & states)
  {
      const Tss::point_t gain = threat.point;
      State new_state(root_state);
      new_state.position[gain.i][gain.j] = root_state.agent_id;
      states.push_back(new_state);
  }

  static void expand_threats_to_states(const std::vector<Tss::threat_t> & threats, const State & root_state, std::vector<State> & states)
  {
    for (const Tss::threat_t & threat : threats) {
      expand_threat_to_states(threat, root_state, states);
    }
  }

  static void expand_threats_to_moves(const std::vector<Tss::threat_t> & threats, const State & root_state, std::vector<move_t> & moves)
  {
    for (const auto & threat : threats) {
      const Tss::point_t & gain = threat.point;
      moves.push_back(move_t(gain.i, gain.j));
    }
  }

  static void expand_moves_to_states(const std::vector<move_t> & moves, const State & root_state, std::vector<State> & states)
  {
    int agent_id = root_state.agent_id;
    for (const auto & move : moves) {
      State new_state(root_state);
      new_state.position[move.first][move.second] = agent_id;
      states.push_back(new_state);
    }
  }

  class Policy
  {
  public:
    Policy(int w, int h):
      m_random_seq(w * h)
    {
      for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
          int index = i * w + j;
          m_random_seq[index].first = i;
          m_random_seq[index].second = j;
        }
      }
      reshuffle();
    }

    ~Policy(){}

    void reshuffle()
    {
      unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
      std::mt19937 random_gen(seed);
      std::shuffle(m_random_seq.begin(), m_random_seq.end(), random_gen);
    }

    int move_random(const State & state, std::vector<move_t> & moves, int max_moves=5)
    {
      DEBUG_POLICY("Agent %d: random_move\n", state.agent_id);
      int sample = 0;
      for (auto & move : m_random_seq) {
        int row = move.first;
        int col = move.second;

        if (state.position[row][col] == EMPTY) {
          DEBUG_POLICY("\tmove_random = [%d, %d: %d]\n", move.first, move.second, state.agent_id);
          moves.push_back(move);
          reshuffle();
          sample++;
          if (sample >= max_moves) {
            return POLICY_SUCCESS;
          }
        }
      }
      return (moves.empty()) ? POLICY_FAIL : POLICY_SUCCESS;
    }

    int move_random(const State & state, std::vector<State> & next_states, int max_moves=5)
    {
      std::vector<std::pair<int, int>> next_moves;
      int res = move_random(state, next_moves, max_moves);
      for (auto & move : next_moves) {
        DEBUG_POLICY("\tmove_random = [%d, %d: %d]\n", move.first, move.second, state.agent_id);
        State new_state(state);
        new_state.position[move.first][move.second] = state.agent_id;
        next_states.push_back(new_state);
      }
      return res;
    }

    int move_rapid(const State & opponent_state, std::vector<State> & next_states, int max_random_moves=5)
    {
      std::vector<move_t> next_moves;
      State self_state(opponent_state);
      self_state.agent_id ^= (1 << 0);

      int res = move_rapid(opponent_state, next_moves, max_random_moves);
      expand_moves_to_states(next_moves, self_state, next_states);

      return res;
    }

    int move_rapid(const State & opponent_state, std::vector<move_t> & next_moves, int max_random_moves=5)
    {
      int res = POLICY_FAIL;

      State self_state(opponent_state);
      self_state.agent_id ^= (1 << 0);

      std::vector<Tss::threat_t> opponent_threats;
      Tss opponent_tss(opponent_state);
      opponent_tss.find_all_threats(opponent_state.position, opponent_threats, THREAT_LEVEL_5, THREAT_LEVEL_5, 1);
      DEBUG_POLICY("Agent %d winning seq\n", opponent_state.agent_id);

      std::vector<Tss::threat_t> self_threats;
      Tss self_tss(self_state);
      self_tss.find_all_threats(self_state.position, self_threats, THREAT_LEVEL_5, THREAT_LEVEL_5, 1);
      DEBUG_POLICY("Agent %d winning seq\n", self_state.agent_id);

      if (!self_threats.empty()) {
          DEBUG_POLICY("Agent %d: attack winning\n", self_state.agent_id);
          expand_threats_to_moves(self_threats, self_state, next_moves);
          res = POLICY_SUCCESS;
      }
      else if (!opponent_threats.empty()){
          DEBUG_POLICY("Agent %d: defend winning\n", self_state.agent_id);
          expand_threats_to_moves(opponent_threats, self_state, next_moves);
          res = POLICY_SUCCESS;
      }

      if (res != POLICY_SUCCESS){
          if (res != POLICY_SUCCESS) {
              DEBUG_POLICY("Agent %d: move middle\n", self_state.agent_id);
              res = move_middle(self_state, next_moves);
          }
          if (res != POLICY_SUCCESS) {
              DEBUG_POLICY("Agent %d: move random\n", self_state.agent_id);
              res = move_random(self_state, next_moves, max_random_moves);
          }
      }

      DEBUG_POLICY("Policy rapid result = %d; # states = %d\n",res, next_moves.size());
      return res;
    }

    int move_defensive(const State & opponent_state, std::vector<State> & next_states, int max_depth=DEFAULT_TSS_MAX_DEPTH)
    {
      std::vector<move_t> next_moves;
      State self_state(opponent_state);
      self_state.agent_id ^= (1 << 0);

      int res = move_defensive(opponent_state, next_moves, max_depth);
      expand_moves_to_states(next_moves, self_state, next_states);

      return res;
    }

    int move_defensive(const State & opponent_state, std::vector<std::pair<int, int>> & next_moves, int max_depth=DEFAULT_TSS_MAX_DEPTH)
    {
      int res = POLICY_FAIL;

      State self_state(opponent_state);
      self_state.agent_id ^= (1 << 0);

      std::vector<Tss::threat_t> opponent_threats;
      std::vector<Tss::threat_t> opponent_winning_seq;
      Tss opponent_tss(opponent_state);
      opponent_tss.find_all_threats(opponent_state.position, opponent_threats, THREAT_LEVEL_3, THREAT_LEVEL_5, max_depth);
      std::sort(opponent_threats.begin(), opponent_threats.end(), std::greater<Tss::threat_t>());

      DEBUG_POLICY("Agent %d winning seq\n", opponent_state.agent_id);
      find_winning_sequence_sorted(opponent_threats, opponent_winning_seq);

      std::vector<Tss::threat_t> self_threats;
      std::vector<Tss::threat_t> self_winning_seq;
      Tss self_tss(self_state);
      self_tss.find_all_threats(self_state.position, self_threats, THREAT_LEVEL_3, THREAT_LEVEL_5, max_depth);
      std::sort(self_threats.begin(), self_threats.end(), std::greater<Tss::threat_t>());

      DEBUG_POLICY("Agent %d winning seq\n", self_state.agent_id);
      find_winning_sequence_sorted(self_threats, self_winning_seq);

      /* Compare winning seq depth, attack/defend by winning seq */
      int opponent_min_winning_depth = (opponent_winning_seq.empty()) ? INT_MAX : opponent_winning_seq.front().min_winning_depth;
      int self_min_winning_depth = (self_winning_seq.empty()) ? INT_MAX : self_winning_seq.front().min_winning_depth;

      if (self_min_winning_depth < opponent_min_winning_depth && !self_winning_seq.empty()) {
          DEBUG_POLICY("Agent %d: attack winning\n", self_state.agent_id);
          expand_threats_to_moves(self_winning_seq, self_state, next_moves);
          res = POLICY_SUCCESS;
      }
      else if (!opponent_winning_seq.empty()){
          DEBUG_POLICY("Agent %d: defend winning (%d)\n", self_state.agent_id);
          expand_threats_to_moves(opponent_winning_seq, self_state, next_moves);
          res = POLICY_SUCCESS;
      }

      if (res != POLICY_SUCCESS) {
          DEBUG_POLICY("Agent %d: defend threats\n", self_state.agent_id);
          res = move_threats(self_state, opponent_threats, next_moves);
      }

      if (res != POLICY_SUCCESS) {
          DEBUG_POLICY("Agent %d: attack threats\n", self_state.agent_id);
          res = move_threats(self_state, self_threats, next_moves);
      }

      if (res != POLICY_SUCCESS){
          DEBUG_POLICY("Agent %d: move random\n", self_state.agent_id);
          if (res != POLICY_SUCCESS) {
              res = move_middle(self_state, next_moves);
          }
          if (res != POLICY_SUCCESS) {
              res = move_random(self_state, next_moves, 5);
          }
      }

      DEBUG_POLICY("Policy default result = %d\n",res);
      return res;
    }
  private:
    std::vector<std::pair<int, int>> m_random_seq;

    int move_threats(const State & state, const std::vector<Tss::threat_t> & threats, std::vector<State> & next_states)
    {
      int res = POLICY_FAIL;
      std::vector<move_t> next_moves;
      res = move_threats(state, threats, next_moves);
      expand_moves_to_states(next_moves, state, next_states);

      return res;
    }

    int move_threats(const State & state, const std::vector<Tss::threat_t> & threats, std::vector<move_t> & next_moves)
    {
      int result = POLICY_FAIL;
      if (!threats.empty()) {
          expand_threats_to_moves(threats, state, next_moves);
          result = POLICY_SUCCESS;
      }
      return result;
    }

    int move_middle(const State & state, std::vector<State> & next_states)
    {
      int res = POLICY_FAIL;
      std::vector<move_t> next_moves;
      res = move_middle(state, next_moves);
      expand_moves_to_states(next_moves, state, next_states);

      return res;
    }

    int move_middle(const State & state, std::vector<move_t> & next_moves)
    {
      int mr = state.board_height / 2;
      int mc = state.board_width / 2;
      if (state.position[mr][mc] == EMPTY){
          DEBUG_POLICY("\tmove middle [%d, %d: %d]\n", mr, mc, state.agent_id);
          next_moves.push_back(move_t(mr, mc));
      }
      return next_moves.empty() ? POLICY_FAIL : POLICY_SUCCESS;
    }
  };
}

#endif


