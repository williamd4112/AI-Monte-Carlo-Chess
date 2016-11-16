#ifndef _FAST_TSS_H_
#define _FAST_TSS_H_

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <cmath>

#include "board.h"
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
class State;

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
  "x_ooo__",  // 6
  "__ooo_x",  // 7
  "_ooo_",   // 8
  "_o_oo_", // 9
  "_oo_o_", // 10

  /* Two */
  "oo___", // 11
  "_oo__", // 12
  "__oo_", // 13
  "___oo", // 14

  /* One */
  "o____", // 15
  "_o___", // 16
  "__o__", // 17
  "___o_", // 18
  "____o"  // 19
};

const static int g_threat_types_len[] = {
  sizeof  "ooooo"  - 1, // 0
  sizeof  "oooo_"  - 1, // 1
  sizeof  "ooo_o"  - 1, // 2
  sizeof  "oo_oo"  - 1, // 3
  sizeof  "o_ooo"  - 1, // 4
  sizeof  "_oooo"  - 1, // 5
  sizeof  "x_ooo__" - 1,  // 6
  sizeof  "__ooo_x" - 1,  // 7
  sizeof  "_ooo_"  - 1,   // 8
  sizeof  "_o_oo_" - 1, // 9
  sizeof  "_oo_o_" - 1, // 10
  sizeof  "oo___"  - 1, // 11
  sizeof  "_oo__"  - 1, // 12
  sizeof  "__oo_"  - 1, // 13
  sizeof  "___oo"  - 1, // 14
  sizeof  "o____"  - 1, // 15
  sizeof  "_o___"  - 1, // 16
  sizeof  "__o__"  - 1, // 17
  sizeof  "___o_"  - 1, // 18
  sizeof  "____o"  - 1  // 19
};

const static int g_threat_pattern_levels[] = {
  5,
  4,
  4,
  4,
  4,
  4,
  3,
  3,
  3,
  3,
  3,
  2,
  2,
  2,
  2,
  1,
  1,
  1,
  1,
  1
};

const static int g_threat_levels[][2] = {
    /* Level 1 */
    {19, 15},
    /* Level 2 */
    {14, 11},
    /* Level 3 */
    {10, 6},
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
  Tss(const State & state);
  ~Tss();

  int find_all_threats(std::vector<threat_t> & threats, int begin_level, int end_level, int max_depth);
  int find_all_threats(const Position & position, std::vector<threat_t> & threats, int begin_level, int end_level, int max_depth);
  std::pair<bool, int> find_all_threats_at(const threat_t & dependent_threat, std::vector<threat_t> & threats, int begin_level, int end_level, int max_depth);
private:
  const State & m_state;

  std::pair<bool, int> find_all_threats_r(
    Position & state_position,
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

  std::pair<int, int> is_gain_square(const threat_t & threat, const Position & position, int begin, int end, int dir, int agent_id);

  void set_cost_squares(
    Position & position,
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
};

}

#endif
