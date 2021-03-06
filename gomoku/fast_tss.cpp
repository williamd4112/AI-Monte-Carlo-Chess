#include "fast_tss.h"

namespace mcts
{

#define FAST_TSS_MAX_DEPENDENT_RANGE 6

mcts::Tss::Tss(const mcts::State & state):
  m_state(state)
{
}

mcts::Tss::~Tss()
{
}

int Tss::find_all_threats(std::vector<threat_t> & threats, int begin_level, int end_level, int max_depth)
{
  return find_all_threats(m_state.position, threats, begin_level, end_level, max_depth);
}

int Tss::find_all_threats(const Position & position, std::vector<threat_t> & threats, int begin_level, int end_level, int max_depth)
{
  Position state_position = position;
  threat_t root_threat(point_t{0, 0}, false);

  find_all_threats_r(state_position, threats, begin_level, end_level, 0, max_depth, root_threat);

  return threats.size();
}

std::pair<bool, int> Tss::find_all_threats_at(
    const threat_t & dependent_threat, std::vector<threat_t> & threats, int begin_level, int end_level, int max_depth)
{
  Position position = m_state.position;
  return find_all_threats_at_gain_square_r(position, threats, begin_level, end_level, 0, max_depth, dependent_threat);
}

std::pair<int, int> Tss::is_gain_square(const threat_t & threat, const Position & position, int begin, int end, int dir, int agent_id)
{
  int begin_pattern_id = g_threat_levels[begin][BEGIN];
  int end_pattern_id = g_threat_levels[end][END];
  int dr = dirs[dir][DR];
  int dc = dirs[dir][DC];
  int match_pos = MISMATCH;
  int row = threat.point.i;
  int col = threat.point.j;
  int w = m_state.board_height;
  int h = m_state.board_width;
  int match_index = 0;

  for (int k = end_pattern_id; k <= begin_pattern_id && match_pos == MISMATCH; k++) {
    match_pos = match_pattern(position, row, col, w, h, dr, dc,
                              g_threat_types[k], g_threat_types_len[k], agent_id);
    if (match_pos != MISMATCH) {
      match_index = k;
    }
  }

  return std::pair<int, int>(match_index, match_pos);
}

void Tss::set_cost_squares(
  Position & position,
  const int row,
  const int col,
  const char * pattern,
  const int pattern_len,
  const int match_pos,
  const int id,
  const int dir)
{
  int dr = dirs[dir][0];
  int dc = dirs[dir][1];
  int begin_row = row - dr * match_pos,
      begin_col = col - dc * match_pos;
  for (int i = 0, r = begin_row, c = begin_col; i < pattern_len; i++, r += dr, c += dc) {
    if (pattern[i] == BLANK) {
      position[r][c] = id;
    }
  }
}

void Tss::apply_match_to_threat(
  std::pair<bool, int> & res,
  const std::pair<int, int> match,
  Position & position,
  threat_t & threat,
  const int begin, const int end,
  const int depth, const int max_depth)
{
  const int match_index = match.first;
  const int next_depth = depth + 1;

  if (match_index != 0) {
    std::pair<bool, int> child_res = find_all_threats_at_gain_square_r(position, threat.children, begin, end, next_depth, max_depth, threat);
    res.first |= child_res.first;
    res.second = std::min(res.second, child_res.second);
    threat.final_winning |= child_res.first;
    threat.min_winning_depth = std::min(threat.min_winning_depth, child_res.second);
  } else {
    res.first = true;
    res.second = depth;
    threat.winning = threat.final_winning = true;
    threat.min_winning_depth = std::min(threat.min_winning_depth, depth);
    LOG_FAST_TSS("Winning sequence found [depth = %d]\n", depth);
  }
}

std::pair<bool, int> Tss::find_all_threats_at_gain_square_r(
  Position & position,
  std::vector<threat_t> & threats,
  const int begin, const int end,
  const int depth, const int max_depth,
  const threat_t & dependent_threat)
{
  std::pair<bool, int> res;
  res.first = false;
  res.second = INT_MAX;

  if (depth >= max_depth) {
    return res;
  }

  const int w = m_state.board_width;
  const int h = m_state.board_height;
  const int row = dependent_threat.point.i;
  const int col = dependent_threat.point.j;

  const int opponent_id = m_state.agent_id ^ (1 << 0);
  const int agent_id = m_state.agent_id;

  DEBUG_FAST_TSS("Start from state\n");
  DEBUG_FAST_TSS_POSITION(position);

  for (int dir = 0; dir < 8; dir++) {
    const int dir_mod = dir % 4;
    const int sign = (dir < 4) ? 1 : -1;
    const int dr = dirs[dir_mod][0] * sign;
    const int dc = dirs[dir_mod][1] * sign;
    int i = row;
    int j = col;
    int lose = 1;
    for (int k = 0; k < FAST_TSS_MAX_DEPENDENT_RANGE && lose >= 0; k++) {
      if (!in_boundary(i, j, w, h)) {
        lose = -1;
        break;
      }
      if (position[i][j] == opponent_id) {
        lose = -1;
        break;
      }

      if (position[i][j] == EMPTY) {
        position[i][j] = agent_id;
        threat_t child_threat(point_t{i, j}, false);
        std::pair<int, int> child_match = is_gain_square(child_threat, position, begin, end, dir_mod, agent_id);

        DEBUG_FAST_TSS("Move from gain(%d, %d) to (%d, %d)[0%x]; Depth = %d/%d; dir(%d, %d: %d)\n", row, col, i, j, position[i][j], depth, max_depth, dr, dc, sign);
        DEBUG_FAST_TSS_POSITION(position);

        if (child_match.second != MISMATCH) {
          const int match_index = child_match.first;
          const int match_pos = child_match.second;
          const char * pattern = g_threat_types[match_index];
          const int pattern_len = g_threat_types_len[match_index];
          
          if (g_threat_pattern_levels[match_index] > child_threat.match_pattern_level) {
            child_threat.match_pattern = pattern;
            child_threat.match_pattern_level = g_threat_pattern_levels[match_index];
          }

          DEBUG_FAST_TSS("Match from gain pattern[%d] %s (at %d)\n", match_index, pattern, match_pos);
          set_cost_squares(position, i, j, pattern, pattern_len, match_pos, opponent_id, dir_mod);
          LOG_FAST_TSS("Gain square from gain (%d, %d) [depth = %d]; Dependent (%d, %d)\n", i, j, depth, dependent_threat.point.i, dependent_threat.point.j);
          LOG_FAST_TSS_POSITION(position);

          apply_match_to_threat(
            res,
            child_match,
            position,
            child_threat,
            begin, end,
            depth, max_depth);

          set_cost_squares(position, i, j, pattern, pattern_len, match_pos, EMPTY, dir_mod);
          threats.push_back(child_threat);
          lose--;
          DEBUG_FAST_TSS("No remain Match from gain pattern[%d] %s (at %d)\n", match_index, pattern, match_pos);
        }
        position[i][j] = EMPTY;
      }
      i += dr;
      j += dc;
    }
  }
  LOG_FAST_TSS("\tResult (depth = %d) = [%d, %d]\n", depth, res.first, res.second);
  return res;
}

std::pair<bool, int> Tss::find_all_threats_r(
  Position & position,
  std::vector<threat_t> & threats,
  const int begin,
  const int end,
  const int depth,
  const int max_depth,
  const threat_t & dependent_threat)
{
  /* No level 1, for now */
  assert(begin != THREAT_LEVEL_1 && end != THREAT_LEVEL_1);

  const int w = m_state.board_width;
  const int h = m_state.board_height;
  const int opponent_id = m_state.agent_id ^ (1 << 0);

  std::pair<bool, int> res;
  res.first = false;
  res.second = INT_MAX;

  if (depth >= max_depth) {
    return res;
  }

  DEBUG_FAST_TSS("Start from state\n");
  DEBUG_FAST_TSS_POSITION(position);

  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      DEBUG_FAST_TSS("Move (%d, %d)[0%x]; Depth = %d\n", i, j, position[i][j], depth);
      if (position[i][j] == mcts::EMPTY) {
        threat_t child_threat(point_t{i, j}, false);

        position[i][j] = m_state.agent_id;
        for (int dir = 0; dir < 4; dir++) {
          std::pair<int, int> match = is_gain_square(child_threat, position, begin, end, dir, m_state.agent_id);
          if (match.second != MISMATCH) {
            const int match_index = match.first;
            const int match_pos = match.second;
            const char * pattern = g_threat_types[match_index];
            const int pattern_len = g_threat_types_len[match_index];
            
            if (g_threat_pattern_levels[match_index] > child_threat.match_pattern_level) {
              child_threat.match_pattern_level = g_threat_pattern_levels[match_index];
              child_threat.match_pattern = pattern;
            }

            DEBUG_FAST_TSS("Match pattern %s (at %d)\n", pattern, match_pos);
            set_cost_squares(position, i, j, pattern, pattern_len, match_pos, opponent_id, dir);
            LOG_FAST_TSS("Gain square (%d, %d) [depth = %d]; Dependent (%d, %d)\n", i, j, depth, dependent_threat.point.i, dependent_threat.point.j);
            LOG_FAST_TSS_POSITION(position);

            apply_match_to_threat(
              res,
              match,
              position,
              child_threat,
              begin, end,
              depth, max_depth);

            set_cost_squares(position, i, j, pattern, pattern_len, match_pos, EMPTY, dir);    
          }
       }
       position[i][j] = EMPTY;
       if (child_threat.match_pattern_level > 0) {
        threats.push_back(child_threat);
       }
      }
    }
  }

  LOG_FAST_TSS("\tResult (depth = %d) = [%d, %d]\n", depth, res.first, res.second);

  return res;
}

}
