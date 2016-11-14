#ifndef _TSS_H_
#define _TSS_H_

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <climits>

#include "constants.h"
#include "state.h"
#include "pattern.h"
#include "util.h"
#include "debug.h"

#ifdef _UNIX
#include <unistd.h>
#endif

#define CRITICAL_THREAT_TYPE_ID 0
#define BEGIN 0
#define END 1
#define DR 0
#define DC 1
#define THREAT_LEVEL_1 0
#define THREAT_LEVEL_2 1
#define THREAT_LEVEL_3 2
#define THREAT_LEVEL_4 3
#define THREAT_LEVEL_5 4
#define RANDOM_SEARCH_RANGE 2

namespace mcts
{
const static char * g_threat_types[] = {
  /* Five */
  "ooooo", // 0

  /* Four */
  "oooo_", // 1
  "ooo_o", // 2
  "oo_oo", // 3
  "o_ooo", // 4
  "_oooo", // 5

  /* Three */
  "__ooo_",  // 6
  "_ooo__",  // 7
  "_o_oo_", // 8
  "_oo_o_", // 9

  /* Two */
  "oo___", // 10
  "_oo__", // 11
  "__oo_", // 12
  "___oo", // 13

  /* One */
  "o____", // 14
  "_o___", // 15
  "__o__", // 17
  "___o_", // 18
  "____o"  // 19
};

const static int g_threat_types_len[] = {
  sizeof  "ooooo" - 1, 
  sizeof  "oooo_" - 1, 
  sizeof  "ooo_o" - 1, 
  sizeof  "oo_oo" - 1,
  sizeof  "o_ooo" - 1, 
  sizeof  "_oooo" - 1, 
  sizeof  "__ooo_" - 1, 
  sizeof  "_ooo__" - 1,  
  sizeof  "_o_oo_" - 1,
  sizeof  "_oo_o_" - 1, 
  sizeof  "oo___" - 1, 
  sizeof  "_oo__" - 1, 
  sizeof  "__oo_" - 1, 
  sizeof  "___oo" - 1, 
  sizeof  "o____" - 1, 
  sizeof  "_o___" - 1,
  sizeof  "__o__" - 1,
  sizeof  "___o_" - 1, 
  sizeof  "____o" - 1
};

const static int g_threat_levels[][2] = {
    /* Level 1 */
    {19, 14},
    /* Level 2 */
    {13, 10},
    /* Level 3 */
    {9, 6},
    /* Level 4 */
    {5, 1},
    /* Level 5 */
    {0, 0}
};

const static int g_threat_types_num = sizeof(g_threat_types) / sizeof(char *);

/*
 * Vertical,
 * Horizontal,
 * Diagonal-Up,
 * Diagonal-Bottom
 */
const static int dirs[4][2] = {
    {  1, 0 },
    {  0, 1 },
    {  1, 1 },
    { -1, 1 }
};

/*
 * Construct with a state.
 * After construction, we can get all threats by calling get_threats()
 */

class Tss
{
public:
  /**
   * @field i row
   * @field j col
   */
  struct point_t
  {
    int i, j;

    friend bool operator ==(const point_t & a, const point_t & b)
    {
        return a.i == b.i && a.j == b.j;
    }
  };

  /*
   * @brief define threat struct
   * @field point position in board
   * @field gain number of attack caused by this point
   */
  struct threat_t
  {
    point_t point;
    std::vector<threat_t> children;
    bool winning;
    bool final_winning;
    int min_winning_depth;

    threat_t(point_t _point, bool _winning):
      point(_point),
      winning(_winning),
      final_winning(false),
      min_winning_depth(INT_MAX)
    {

    }

    threat_t(const threat_t & copy):
      point(copy.point),
      children(copy.children),
      winning(copy.winning),
      final_winning(copy.final_winning),
      min_winning_depth(copy.min_winning_depth)
    {

    }

    void print()
    {
      print_r(*this, 0);
    }

    friend bool operator > (const threat_t & a, const threat_t & b)
    {
      if (a.final_winning && !b.final_winning)
        return true;
      else if (!a.final_winning && b.final_winning)
        return false;
      else
        return a.min_winning_depth < b.min_winning_depth;
    }

  private:
    void print_r(threat_t & t, int d)
    {
      for (int i = 0; i < d; i++) putchar('-');
      printf("Threat (%d, %d): [%d, %d, %d]\n", t.point.i, t.point.j, t.winning, t.final_winning, t.min_winning_depth);
      for (threat_t & child : t.children) {
        print_r(child, d + 1);
      }
    }
  };

  Tss(const State & state);
  ~Tss();

  int find_all_threats(std::vector<threat_t> & threats, int begin_level, int end_level, int max_depth);
  int find_all_threats(const State::Position & position, std::vector<threat_t> & threats, int begin_level, int end_level, int max_depth);
private:
  const State & m_state;

  std::pair<bool, int> find_all_threats_r(
    State::Position & state_position, 
    std::vector<threat_t> & threats, 
    const int begin, const int end, 
    const int depth, const int max_depth, 
    const threat_t & dependent_threat);

  std::pair<bool, int> find_all_threats_at_gain_square_r(
    Position & position,
    std::vector<threat_t> & threats,
    const int begin, const int end,
    const int depth, const int max_depth,
    const threat_t & dependent_threat);

  std::pair<int, int> is_gain_square(const threat_t & threat, const State::Position & position, int begin, int end, int dir, int agent_id);
  
  void set_cost_squares(
    State::Position & position,
    const int row, 
    const int col,
    const char * pattern, 
    const int pattern_len, 
    const int match_pos,
    const int id, 
    const int dir);

  void apply_match_to_threat(
    std::pair<bool, int> & res,
    const std::pair<int, int> match,
    Position & position,
    threat_t & threat,
    const int begin, const int end,
    const int depth, const int max_depth);

#ifdef _OLD_FAST_TSS
  bool is_dependent(const State::Position & position,
    int begin_row, int begin_col, int end_row, int end_col,
    int dr, int dc, const char * pattern, int pattern_match_pos, int pattern_len,
    const point_t & dependent_square);

  void set_cost_squares(State::Position & position, const char * pattern, int pattern_len, int opponent_id, int begin_row, int begin_col, int end_row, int end_col, int dr, int dc);

  void unset_cost_squares(State::Position & position, const char * pattern, int pattern_len, int opponent_id, int begin_row, int begin_col, int end_row, int end_col, int dr, int dc);
#endif
};

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

int Tss::find_all_threats(const State::Position & position, std::vector<Tss::threat_t> & threats, int begin_level, int end_level, int max_depth)
{
  State::Position state_position = position;
  threat_t root_threat(point_t{0, 0}, false);

  find_all_threats_r(state_position, threats, begin_level, end_level, 0, max_depth, root_threat);

  return threats.size();
}

std::pair<int, int> Tss::is_gain_square(const Tss::threat_t & threat, const State::Position & position, int begin, int end, int dir, int agent_id)
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

#ifdef _OLD_FAST_TSS
bool Tss::is_dependent(const State::Position & position,
  int begin_row, int begin_col, int end_row, int end_col,
  int dr, int dc, const char * pattern, int pattern_match_pos, int pattern_len,
  const Tss::point_t & dependent_square)
{
  bool dependency = false;
  for (int i = 0, r = begin_row, c = begin_col; i < pattern_len && !dependency; i++, r += dr, c += dc) {
    if (position[r][c] == m_state.agent_id && i != pattern_match_pos) {
      dependency = (dependent_square.i == r && dependent_square.j == c);
    }
  }
  return dependency;
}
#endif

void Tss::set_cost_squares(
  State::Position & position,
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
      begin_col = col - dc * match_pos,
      end_row = row + dr * (pattern_len - match_pos - 1),
      end_col = col + dc * (pattern_len - match_pos - 1);

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
  Tss::threat_t & threat,
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
    threat.min_winning_depth = child_res.second;
  }
  else {
    res.first = true;
    res.second = depth;
    threat.winning = threat.final_winning = true;
    threat.min_winning_depth = depth;
    LOG_FAST_TSS("Winning sequence found [depth = %d]\n", depth);
  }
}

std::pair<bool, int> Tss::find_all_threats_at_gain_square_r(
  Position & position,
  std::vector<threat_t> & threats,
  const int begin, const int end,
  const int depth, const int max_depth,
  const Tss::threat_t & dependent_threat)
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
    for (int k = 0; k < 5 && lose >= 0; k++) {
      if (!in_boundary(i, j, w, h)) {
        lose = -1;
        break;
      }
      if (position[i][j] == opponent_id) {
        lose = -1;
        break;
      }

      if (position[i][j] == EMPTY){
        position[i][j] = agent_id;
        threat_t child_threat(point_t{i, j}, false);
        std::pair<int, int> child_match = is_gain_square(child_threat, position, begin, end, dir_mod, agent_id);
        
        DEBUG_FAST_TSS("Move from gain(%d, %d) to (%d, %d)[0%x]; Depth = %d; dir(%d, %d: %d)\n", row, col, i, j, position[i][j], depth, dr, dc, sign);
        DEBUG_FAST_TSS_POSITION(position);

        if (child_match.second != MISMATCH) {
          const int match_index = child_match.first;
          const int match_pos = child_match.second;
          const char * pattern = g_threat_types[match_index];
          const int pattern_len = g_threat_types_len[match_index];

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
    State::Position & position,
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
  const int agent_id = m_state.agent_id;

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
            const int next_depth = depth + 1;
            const bool winning = (match_index == 0);

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
            threats.push_back(child_threat);
            break;
          }

#ifdef _OLD_FAST_TSS
          if (match.second != MISMATCH) {

            const char * pattern = g_threat_types[match.first];
            int pattern_len = strlen(pattern);
            int match_pos = match.second;
            int begin_row = i - dr * match_pos,
                begin_col = j - dc * match_pos,
                end_row = i + dr * (pattern_len - match_pos - 1),
                end_col = j + dc * (pattern_len - match_pos - 1);

            bool dependency = (depth == 0);
            if (!dependency) {
              dependency = is_dependent(position, begin_row, begin_col, end_row, end_col, dr, dc,
                pattern, match_pos, pattern_len, dependent_square);
            }

            if (dependency) {
              DEBUG_FAST_TSS("Match pattern %s (%d)\n", pattern, match.second);

              set_cost_squares(position, pattern, pattern_len, opponent_id, begin_row, begin_col, end_row, end_col, dr, dc);

              LOG_FAST_TSS("Gain square (%d, %d) [depth = %d]; Dependent (%d, %d)\n", i, j, depth, dependent_threat.point.i, dependent_threat.point.j);
              LOG_FAST_TSS_POSITION(position);

              /* Cut when winning */
              if (match.first != 0) {
                std::pair<int, int> child_res = find_all_threats_r(position, threat.children, begin, end, depth + 1, max_depth, threat);
                res.first |= child_res.first;
                res.second = std::min(res.second, child_res.second);

                threat.final_winning |= child_res.first;
                threat.min_winning_depth = child_res.second;
              }
              else {
                res.first = true;
                res.second = depth;
                threat.winning = threat.final_winning = true;
                threat.min_winning_depth = depth;

                LOG_FAST_TSS("Winning sequence found [depth = %d]\n", depth);
              }
              unset_cost_squares(position, pattern, pattern_len, opponent_id, begin_row, begin_col, end_row, end_col, dr, dc);
              threats.push_back(threat);
              break;
            }
          }
#endif
        }
        position[i][j] = EMPTY;
      }
    }
  }

  LOG_FAST_TSS("\tResult (depth = %d) = [%d, %d]\n", depth, res.first, res.second);

  return res;
}

}

#endif
