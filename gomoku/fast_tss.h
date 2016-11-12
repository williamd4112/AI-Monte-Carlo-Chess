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

static int get_threat_level(int i)
{
  for (int level = THREAT_LEVEL_5; level >= THREAT_LEVEL_1; level--) {
    if (i >= g_threat_levels[level][1] && i <= g_threat_levels[level][0]) {
      return level;
    }
  }

  /* Should not reach here */
  assert(false);
  return 0;
}

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
      printf("Gain(%d, %d): [%d, %d]\n", t.point.i, t.point.j, t.winning, t.final_winning);
      for (threat_t & child : t.children) {
        print_r(child, d + 1);
      }
    }
  };

  Tss(const State & state);
  ~Tss();

  int find_all_threats(const State::Position & position, std::vector<threat_t> & threats, int begin_level, int end_level, int max_depth);
private:
  const State & m_state;

  std::pair<bool, int> find_all_threats_r(State::Position & state_position, std::vector<threat_t> & threats, int begin, int end, int depth, int max_depth, threat_t & dependent_threat);

  std::pair<int, int> is_gain_square(const threat_t & threat, const State::Position & position, int begin, int end, int dir, int agent_id);

  bool is_dependent(const State::Position & position,
    int begin_row, int begin_col, int end_row, int end_col,
    int dr, int dc, const char * pattern, int pattern_match_pos, int pattern_len,
    const point_t & dependent_square);

  void set_cost_squares(State::Position & position, const char * pattern, int pattern_len, int opponent_id, int begin_row, int begin_col, int end_row, int end_col, int dr, int dc);

  void unset_cost_squares(State::Position & position, const char * pattern, int pattern_len, int opponent_id, int begin_row, int begin_col, int end_row, int end_col, int dr, int dc);
};

mcts::Tss::Tss(const mcts::State & state):
  m_state(state)
{
}

mcts::Tss::~Tss()
{

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
        g_threat_types[k], agent_id);
    if (match_pos != MISMATCH) {
      match_index = k;
    }
  }

  return std::pair<int, int>(match_index, match_pos);
}

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

void Tss::set_cost_squares(State::Position & position, const char * pattern, int pattern_len, int opponent_id, int begin_row, int begin_col, int end_row, int end_col, int dr, int dc)
{
  for (int i = 0, r = begin_row, c = begin_col; i < pattern_len; i++, r += dr, c += dc) {
    if (pattern[i] == BLANK) {
      position[r][c] = opponent_id;
    }
  }
}

void Tss::unset_cost_squares(State::Position & position, const char * pattern, int pattern_len, int opponent_id, int begin_row, int begin_col, int end_row, int end_col, int dr, int dc)
{
  for (int i = 0, r = begin_row, c = begin_col; i < pattern_len; i++, r += dr, c += dc) {
    if (pattern[i] == BLANK) {
      position[r][c] = EMPTY;
    }
  }
}

std::pair<bool, int> Tss::find_all_threats_r(
    State::Position & position,

    std::vector<threat_t> & threats, int begin, int end, int depth, int max_depth, threat_t & dependent_threat)
{
  /* No level 1, for now */
  assert(begin != THREAT_LEVEL_1 && end != THREAT_LEVEL_1);


  int w = m_state.board_width;
  int h = m_state.board_height;
  int opponent_id = m_state.agent_id ^ (1 << 0);
  std::pair<bool, int> res;

  /* Initialize no winning, invalid depth */
  res.first = false;
  res.second = INT_MAX;

  if (depth >= max_depth) {
    return res;
  }

  const Tss::point_t & dependent_square = dependent_threat.point;

  DEBUG_FAST_TSS("Start from state\n");
  DEBUG_FAST_TSS_POSITION(position);

  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      DEBUG_FAST_TSS("Move (%d, %d)[0%x]; Depth = %d\n", i, j, position[i][j], depth);
      if (position[i][j] == mcts::EMPTY) {
        threat_t threat(point_t{i, j}, false);

        position[i][j] = m_state.agent_id;
        for (int dir = 0; dir < 4; dir++) {
          int dr = dirs[dir][DR];
          int dc = dirs[dir][DC];

          std::pair<int, int> match = is_gain_square(threat, position, begin, end, dir, m_state.agent_id);
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
