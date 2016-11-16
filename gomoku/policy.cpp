#include "policy.h"

namespace mcts
{

std::ostream & operator << (std::ostream & os, std::vector<threat_t> & threats)
{
  for (auto & t : threats) {
    os << "Gain(" << t.point.i << ", " << t.point.j << ")" << std::endl;
  }
  return os;
}

int find_one_step_winning(const std::vector<threat_t> & threats)
{
  for (int i = 0; i < (int)threats.size(); i++) {
    if (threats[i].winning) {
      return i;
    }
  }
  return -1;
}

void find_winning_sequence(const std::vector<threat_t> & threats, std::vector<threat_t> & seq)
{
  for (const threat_t & threat : threats) {
    LOG_POLICY("Threat(%c, %d): [%d, %d, %d]\n", threat.point.i + 'A', threat.point.j, threat.winning, threat.final_winning, threat.min_winning_depth);
    if (threat.final_winning) {
      seq.push_back(threat);
    }
  }
}

void find_winning_sequence_sorted(const std::vector<threat_t> & threats, std::vector<threat_t> & seq)
{
  int min_depth = INT_MAX;
  for (const threat_t & threat : threats) {
    LOG_POLICY("Threat(%c, %d): [%d, %d, %d]\n", threat.point.i + 'A', threat.point.j, threat.winning, threat.final_winning, threat.min_winning_depth);
    if (threat.final_winning && threat.min_winning_depth <= min_depth) {
      seq.push_back(threat);
      min_depth = threat.min_winning_depth;
    }
  }
}

void expand_threat_to_states(const threat_t & threat, const State & root_state, std::vector<State> & states)
{
  const point_t gain = threat.point;
  State new_state(root_state);
  new_state.position[gain.i][gain.j] = root_state.agent_id;
  states.push_back(new_state);
}

void expand_threat_to_moves(const threat_t & threat, std::vector<move_t> & moves)
{
  moves.push_back(move_t(threat.point.i, threat.point.j));
}

void expand_threats_to_states(const std::vector<threat_t> & threats, const State & root_state, std::vector<State> & states)
{
  for (const threat_t & threat : threats) {
    expand_threat_to_states(threat, root_state, states);
  }
}

void expand_threats_to_moves(const std::vector<threat_t> & threats, const State & root_state, std::vector<move_t> & moves)
{
  for (const auto & threat : threats) {
    const point_t & gain = threat.point;
    moves.push_back(move_t(gain.i, gain.j));
  }
}

void expand_moves_to_states(const std::vector<move_t> & moves, const State & root_state, std::vector<State> & states)
{
  int agent_id = root_state.agent_id;
  for (const auto & move : moves) {
    State new_state(root_state);
    new_state.position[move.first][move.second] = agent_id;
    states.push_back(new_state);
  }
}

void find_critical_winning_seq(
    const State & opponent_state,
    const std::vector<threat_t> & opponent_winning_seq, std::vector<threat_t> & opponent_critical_winning_seq)
{
  if (opponent_winning_seq.empty()) {
    return;
  }

  int self_agent_id = opponent_state.agent_id ^ 1;

  std::vector<threat_t> candidate_winning_seq(opponent_winning_seq.size());
  std::pair<int, int> remain_winning_seqs[opponent_winning_seq.size()];

  for (int i = 0; i < (int) opponent_winning_seq.size(); i++) {
    const threat_t & t = opponent_winning_seq[i];

    State new_state(opponent_state);
    new_state.position[t.point.i][t.point.j] = self_agent_id;

    Tss tss(new_state);
    std::vector<threat_t> new_threats;
    for (auto & threat : opponent_winning_seq) {
      tss.find_all_threats_at(threat, new_threats, THREAT_LEVEL_3, THREAT_LEVEL_5, 4);
    }

    int remain_winning_seq = 0;
    for (auto & threat : new_threats) {
      if (threat.final_winning) {
        remain_winning_seq++;
      }
    }
    remain_winning_seqs[i] = std::pair<int, int>(remain_winning_seq, i);

    candidate_winning_seq.push_back(t);
  }

  std::sort(remain_winning_seqs, remain_winning_seqs + opponent_winning_seq.size());

  int min_remain = INT_MAX;
  for (auto & pair : remain_winning_seqs) {
    if (pair.first <= min_remain) {
      min_remain = pair.first;
      opponent_critical_winning_seq.push_back(opponent_winning_seq[pair.second]);
    }
    else {
      break;
    }
  }
}

void find_top_winning_seq(const std::vector<threat_t> & winning_seq, std::vector<threat_t> & top_winning_seq)
{
  int top_level = -INT_MAX;
  for (const threat_t & threat : winning_seq) {
    DEBUG_POLICY("Top seq(%c, %d): [%d, %d, %d: %s/%d]\n", 
      threat.point.i + 'A', threat.point.j, threat.winning, 
      threat.final_winning, threat.min_winning_depth, t.match_pattern.c_str(), t.match_pattern_level);

    if (threat.final_winning && threat.match_pattern_level >= top_level) {
      top_winning_seq.push_back(threat);
      top_level = threat.match_pattern_level;
    }
  }
}

Policy::Policy(int w, int h):
  m_random_seq(w * h)
{
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      int index = i * w + j;
      m_random_seq[index].first = i;
      m_random_seq[index].second = j;
    }
  }

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  m_random_gen = std::mt19937(seed);

  reshuffle();
}

Policy::~Policy()
{

}

void Policy::reshuffle()
{
  std::shuffle(m_random_seq.begin(), m_random_seq.end(), m_random_gen);
}

int Policy::move_random(const State & state, std::vector<move_t> & moves, int max_moves)
{
  DEBUG_POLICY("Agent %d: random_move\n", state.agent_id);
  int sample = 0;
  for (auto & move : m_random_seq) {
    int row = move.first;
    int col = move.second;

    if (state.position[row][col] == EMPTY) {
      DEBUG_POLICY("\tmove_random = [%d, %d: %d]\n", move.first, move.second, state.agent_id);
      moves.push_back(move);
      sample++;
      if (sample >= max_moves) {
        return POLICY_SUCCESS;
      }
    }
  }

  //reshuffle();
  return (moves.empty()) ? POLICY_FAIL : POLICY_SUCCESS;
}

int Policy::move_random(const State & state, std::vector<State> & next_states, int max_moves)
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

int Policy::move_winning_seq(
  const State & opponent_state, std::vector<threat_t> & opponent_threats,
  const State & self_state, std::vector<threat_t> & self_threats,
  std::vector<move_t> & next_moves)
{
  LOG_POLICY("Agent %d: move_winning_seq\n", self_state.agent_id);
  int res = POLICY_FAIL;

  std::vector<threat_t> opponent_winning_seq;
  std::vector<threat_t> opponent_critical_winning_seq;
  LOG_POLICY("Agent %d winning seq\n", opponent_state.agent_id);

  std::sort(opponent_threats.begin(), opponent_threats.end(), std::greater<threat_t>());
  find_winning_sequence_sorted(opponent_threats, opponent_winning_seq);
  find_critical_winning_seq(opponent_state, opponent_winning_seq, opponent_critical_winning_seq);

  std::vector<threat_t> self_winning_seq;
  LOG_POLICY("Agent %d winning seq\n", self_state.agent_id);

  std::sort(self_threats.begin(), self_threats.end(), std::greater<threat_t>());
  find_winning_sequence_sorted(self_threats, self_winning_seq);

  /* Find one step winning */
  if (res != POLICY_SUCCESS) {
    int self_one_step_winning_index = find_one_step_winning(self_winning_seq);
    if (self_one_step_winning_index >= 0) {
      expand_threat_to_moves(self_winning_seq[self_one_step_winning_index], next_moves);
      res = POLICY_SUCCESS;
    }
  }

  if (res != POLICY_SUCCESS) {
    int opponent_one_step_winning_index = find_one_step_winning(opponent_critical_winning_seq);
    if (opponent_one_step_winning_index >= 0) {
      expand_threat_to_moves(opponent_critical_winning_seq[opponent_one_step_winning_index], next_moves);
      res = POLICY_SUCCESS;
    }
  }

  /* Compare winning seq depth, attack/defend by winning seq */
  if (res != POLICY_SUCCESS) {
    int opponent_min_winning_depth = (opponent_critical_winning_seq.empty()) ? INT_MAX : opponent_critical_winning_seq.front().min_winning_depth;
    int self_min_winning_depth = (self_winning_seq.empty()) ? INT_MAX : self_winning_seq.front().min_winning_depth;

    if (self_min_winning_depth <= opponent_min_winning_depth && !self_winning_seq.empty()) {
      LOG_POLICY("Agent %d: attack winning\n", self_state.agent_id);
      expand_threats_to_moves(self_winning_seq, self_state, next_moves);
      res = POLICY_SUCCESS;
    }
    else if (!opponent_winning_seq.empty()) {
      LOG_POLICY("Agent %d: defend winning (%d)\n", self_state.agent_id);
      expand_threats_to_moves(opponent_critical_winning_seq, self_state, next_moves);
      res = POLICY_SUCCESS;
    }
  }

  return res;
}

int Policy::move_rapid(const State & opponent_state, std::vector<State> & next_states, int max_random_moves)
{
  std::vector<move_t> next_moves;
  State self_state(opponent_state);
  self_state.agent_id ^= (1 << 0);

  int res = move_rapid(opponent_state, next_moves, max_random_moves);
  expand_moves_to_states(next_moves, self_state, next_states);

  return res;
}

int Policy::move_rapid(const State & opponent_state, std::vector<move_t> & next_moves, int max_random_moves)
{
  int res = POLICY_FAIL;

  State self_state(opponent_state);
  self_state.agent_id ^= (1 << 0);

  std::vector<threat_t> opponent_threats;
  Tss opponent_tss(opponent_state);
  opponent_tss.find_all_threats(opponent_state.position, opponent_threats, THREAT_LEVEL_5, THREAT_LEVEL_5, 1);

  std::vector<threat_t> self_threats;
  Tss self_tss(self_state);
  self_tss.find_all_threats(self_state.position, self_threats, THREAT_LEVEL_5, THREAT_LEVEL_5, 1);

  res = move_winning_seq(opponent_state, opponent_threats, self_state, self_threats, next_moves);

  if (res != POLICY_SUCCESS) {
    res = move_when_no_threats(self_state, next_moves);
  }

  DEBUG_POLICY("Policy rapid result = %d; # states = %d\n",res, next_moves.size());
  return res;
}

int Policy::move_when_no_threats(const State & self_state, std::vector<State> & next_states)
{
  int res = POLICY_FAIL;
  std::vector<move_t> next_moves;
  res = move_when_no_threats(self_state, next_moves);

  expand_moves_to_states(next_moves, self_state, next_states);

  return res;
}

int Policy::move_when_no_threats(const State & self_state, std::vector<move_t> & next_moves)
{
  int res = POLICY_FAIL;
  if (res != POLICY_SUCCESS) {
    if (res != POLICY_SUCCESS) {
      LOG_POLICY("Agent %d: move middle\n", self_state.agent_id);
      res = move_middle(self_state, next_moves);
    }

    if (res != POLICY_SUCCESS) {
      LOG_POLICY("Agent %d: move approach\n", self_state.agent_id);
      res = move_approach(self_state, next_moves);
    }

    if (res != POLICY_SUCCESS) {
      LOG_POLICY("Agent %d: move random\n", self_state.agent_id);
      res = move_random(self_state, next_moves, 5);
    }
  }
  return res;
}

int Policy::move_defensive(const State & opponent_state, std::vector<State> & next_states, int max_depth)
{
  std::vector<move_t> next_moves;
  State self_state(opponent_state);
  self_state.agent_id ^= (1 << 0);

  int res = move_defensive(opponent_state, next_moves, max_depth);
  expand_moves_to_states(next_moves, self_state, next_states);

  return res;
}

int Policy::move_defensive(const State & opponent_state, std::vector<std::pair<int, int>> & next_moves, int max_depth)
{
  int res = POLICY_FAIL;

  State self_state(opponent_state);
  self_state.agent_id ^= (1 << 0);

  std::vector<threat_t> opponent_threats;
  Tss opponent_tss(opponent_state);
  opponent_tss.find_all_threats(opponent_state.position, opponent_threats, THREAT_LEVEL_3, THREAT_LEVEL_5, max_depth);

  std::vector<threat_t> self_threats;
  Tss self_tss(self_state);
  self_tss.find_all_threats(self_state.position, self_threats, THREAT_LEVEL_3, THREAT_LEVEL_5, max_depth);

  res = move_winning_seq(opponent_state, opponent_threats, self_state, self_threats, next_moves);

  if (res != POLICY_SUCCESS) {
    LOG_POLICY("Agent %d: defend threats\n", self_state.agent_id);
    res = move_threats(self_state, opponent_threats, next_moves);
  }

  if (res != POLICY_SUCCESS) {
    LOG_POLICY("Agent %d: attack threats\n", self_state.agent_id);
    res = move_threats(self_state, self_threats, next_moves);
  }

  if (res != POLICY_SUCCESS) {
    res = move_when_no_threats(self_state, next_moves);
  }

  LOG_POLICY("Policy defensive result = %d\n",res);
  return res;
}

int Policy::move_balance(const State & opponent_state, std::vector<State> & next_states, int max_depth)
{
  std::vector<move_t> next_moves;
  State self_state(opponent_state);
  self_state.agent_id ^= (1 << 0);

  int res = move_balance(opponent_state, next_moves, max_depth);
  expand_moves_to_states(next_moves, self_state, next_states);

  return res;
}

int Policy::move_balance(const State & opponent_state, std::vector<std::pair<int, int>> & next_moves, int max_depth)
{
  int res = POLICY_FAIL;

  State self_state(opponent_state);
  self_state.agent_id ^= (1 << 0);

  std::vector<threat_t> opponent_threats;
  Tss opponent_tss(opponent_state);
  opponent_tss.find_all_threats(opponent_state.position, opponent_threats, THREAT_LEVEL_3, THREAT_LEVEL_5, max_depth);

  std::vector<threat_t> self_threats;
  Tss self_tss(self_state);
  self_tss.find_all_threats(self_state.position, self_threats, THREAT_LEVEL_3, THREAT_LEVEL_5, max_depth);

  res = move_winning_seq(opponent_state, opponent_threats, self_state, self_threats, next_moves);

  if (res != POLICY_SUCCESS) {
    LOG_POLICY("Agent %d: balance move\n", self_state.agent_id);
    res = move_threats(self_state, opponent_threats, next_moves);
    res |= move_threats(self_state, self_threats, next_moves);
  }

  if (res != POLICY_SUCCESS) {
    res = move_when_no_threats(self_state, next_moves);
  }

  LOG_POLICY("Policy balance result = %d\n",res);
  return res;
}

int Policy::move_threats(const State & state, const std::vector<threat_t> & threats, std::vector<State> & next_states)
{
  int res = POLICY_FAIL;
  std::vector<move_t> next_moves;
  res = move_threats(state, threats, next_moves);
  expand_moves_to_states(next_moves, state, next_states);

  return res;
}

int Policy::move_threats(const State & state, const std::vector<threat_t> & threats, std::vector<move_t> & next_moves)
{
  int result = POLICY_FAIL;
  if (!threats.empty()) {
    expand_threats_to_moves(threats, state, next_moves);
    result = POLICY_SUCCESS;
  }
  return result;
}

int Policy::move_middle(const State & state, std::vector<State> & next_states)
{
  int res = POLICY_FAIL;
  std::vector<move_t> next_moves;
  res = move_middle(state, next_moves);
  expand_moves_to_states(next_moves, state, next_states);

  return res;
}

int Policy::move_middle(const State & state, std::vector<move_t> & next_moves)
{
  int mr = state.board_height / 2;
  int mc = state.board_width / 2;
  if (state.position[mr][mc] == EMPTY) {
    DEBUG_POLICY("\tmove middle [%d, %d: %d]\n", mr, mc, state.agent_id);
    next_moves.push_back(move_t(mr, mc));
  }
  return next_moves.empty() ? POLICY_FAIL : POLICY_SUCCESS;
}

int Policy::move_approach_ex(const State & state, std::vector<State> & next_states, int num_samples)
{
  static const int RANDOM_RANGE = 2;
  int w = state.board_width;
  int h = state.board_height;
  unsigned int random_ts = w / 2;
  int res = POLICY_FAIL;
  int count = 0;

  for (int i = 0; i < h && count < num_samples; i++) {
    for (int j = 0; j < w && count < num_samples; j++) {
      if (state.position[i][j] != EMPTY) {
        for (int k = 0; k < num_samples; k++) {
          int sign = (m_random_gen() % w) < random_ts ? -1 : 1;
          int r = i + sign * (m_random_gen() % RANDOM_RANGE);
          int c = j + sign * (m_random_gen() % RANDOM_RANGE);

          if (!in_boundary(r, c, w, h)) {
            continue;
          }

          if (state.position[r][c] == EMPTY) {
            State new_state(state);
            new_state.agent_id ^= (1 << state.agent_id);
            new_state.position[r][c] = new_state.agent_id;
            next_states.push_back(new_state);
            res = POLICY_SUCCESS;
            count++;
          }
        }
      }
    }
  }

  if (res == POLICY_FAIL) {
    State self_state(state);
    self_state.agent_id ^= (1 << 0);
    return move_when_no_threats(self_state, next_states);
  }
  else {
    return res;
  }
}

int Policy::move_approach(const State & state, std::vector<State> & next_states)
{
  int res = POLICY_FAIL;
  std::vector<move_t> next_moves;
  res = move_approach(state, next_moves);
  expand_moves_to_states(next_moves, state, next_states);

  return res;
}

int Policy::move_approach(const State & state, std::vector<move_t> & next_moves)
{
  Tss tss(state);
  std::vector<threat_t> threats;
  tss.find_all_threats(threats, THREAT_LEVEL_2, THREAT_LEVEL_2, 2);

  expand_threats_to_moves(threats, state, next_moves);

  DEBUG_POLICY("\tmove_approach[agent = %d] = %d\n", state.agent_id, next_moves.size());

  return next_moves.empty() ? POLICY_FAIL : POLICY_SUCCESS;
}


}
