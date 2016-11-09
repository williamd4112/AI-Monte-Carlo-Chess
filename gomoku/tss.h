#ifndef _TSS_H_
#define _TSS_H_

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>

#include "constants.h"
#include "state.h"
#include "pattern.h"
#include "debug.h"

#define CRITICAL_THREAT_TYPE_ID 0
#define CRITICAL_THREAT_GAIN 0xff
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
  "_ooo_",  // 6
  "_o_oo_", // 7
  "_oo_o_", // 8

  /* Two */
  "oo___", // 9
  "_oo__", // 10
  "__oo_", // 11
  "___oo", // 12

  /* One */
  "o____", // 13
  "_o___", // 14
  "__o__", // 15
  "___o_", // 16
  "____o"  // 17
};

const static int g_threat_levels[][2] = {
    /* Level 1 */
    {17, 13},
    /* Level 2 */
    {12, 9},
    /* Level 3 */
    {8, 6},
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
  };

  /*
   * @brief define threat struct
   * @field point position in board
   * @field gain number of attack caused by this point
   */
  struct threat_t
  {
    point_t point;
    int gain;

    bool operator > (const threat_t & t) const
    {
        return gain > t.gain;
    }
  };

  Tss(const State & state);
  ~Tss();

  /*
   * @brief test set chess on each empty position to
   *        find all possible threats in the board, sorted by threat gain
   * @param[OUT] threats find all threats and push into threats
   * @param[IN] begin begin level
   * @param[IN] end end level
   * @return threat list
   * */
  std::vector<threat_t> & find_all_threats(std::vector<threat_t> & threats, int begin, int end);

  /*
   * @brief randomly choose one point (usually used when no threat can be created)
   * @param[OUT] points possible points on the board
   * */
  void find_possible_points(std::vector<point_t> & points);
private:
  const State & m_state;

  /*
   * @brief find threat caused by setting a chess in (row, col)
   * @param[In] board
   * @param[In] row
   * @param[In] col
   * @param[In] width
   * @param[In] height
   * @param[In] agent id (opponent/self)
   * @param[In] begin begin level
   * @param[In] end end level
   * @param[Out] threats collection
   * @return gain of threat
   * */
  int find_threat_at(const State::Position & position, int row, int col, int w, int h, int agent_id, int begin, int end, std::vector<threat_t> & threat_list);

  /*
   * @brief find possible points around specific row and col
   * @param[OUT] points points collections for output
   * @param[IN] visited visited positions
   * @param[IN] row
   * @param[IN] col
   **/
  void find_possible_points_at(std::vector<point_t> & points, std::vector<std::vector<bool>> & visited, int row, int col);
};

mcts::Tss::Tss(const mcts::State & state):
  m_state(state)
{
}

mcts::Tss::~Tss()
{

}

void Tss::find_possible_points_at(std::vector<point_t> & points, std::vector<std::vector<bool>> & visited, int row, int col)
{
    int w = m_state.board_width;
    int h = m_state.board_height;

    for (int i = row - RANDOM_SEARCH_RANGE; i < row + RANDOM_SEARCH_RANGE; i++) {
        for (int j = col - RANDOM_SEARCH_RANGE; j < col + RANDOM_SEARCH_RANGE; j++) {
            if (in_boundary(i, j, w, h)) {
                if (m_state.position[i][j] == EMPTY && !visited[i][j]) {
                    visited[i][j] = true;
                    points.push_back(point_t{i, j});
                }
            }
        }
    }
}

void Tss::find_possible_points(std::vector<Tss::point_t> & points)
{
    std::vector<std::vector<bool>> visited(m_state.board_height,
            std::vector<bool>(m_state.board_width, false));

    for (int i = 0; i < m_state.board_height; i++) {
        for (int j = 0; j < m_state.board_width; j++) {
            if (m_state.position[i][j] != EMPTY) {
                find_possible_points_at(points, visited, i, j);
            }
        }
    }

    if (points.empty()) {
        points.push_back(point_t{m_state.board_height / 2, m_state.board_width / 2});
    }

    std::random_shuffle(points.begin(), points.end());
}

std::vector<Tss::threat_t> & Tss::find_all_threats(std::vector<threat_t> & threats, int begin, int end)
{
  /* No level 1, for now */
  assert(begin != THREAT_LEVEL_1 && end != THREAT_LEVEL_1);

  int w = m_state.board_width;
  int h = m_state.board_height;

  State::Position position = m_state.position;

  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      if (m_state.position[i][j] == mcts::EMPTY) {

        position[i][j] = m_state.agent_id;
        int gain = find_threat_at(position, i, j, w, h, m_state.agent_id, begin, end, threats);
        position[i][j] = mcts::EMPTY;
      }
    }
  }
  std::sort(threats.begin(), threats.end(), std::greater<threat_t>());

  return threats;
}

int Tss::find_threat_at(const State::Position & position,
        int row, int col, int w, int h, int agent_id, int begin, int end, std::vector<threat_t> & threat_list)
{
  threat_t threat = threat_t{ point_t{row, col}, 0 };

  int begin_pattern_id = g_threat_levels[begin][BEGIN];
  int end_pattern_id = g_threat_levels[end][END];

  DEBUG_TSS("From %d to %d\n", end_pattern_id, begin_pattern_id);

  for (int k = end_pattern_id; k <= begin_pattern_id; k++) {
    DEBUG_TSS("Try pattern %d (%s)\n", k, g_threat_types[k]);

    for (int d = 0; d < 4; d++) {
      bool result = match_pattern(position,
        row, col, w, h, dirs[d][DR], dirs[d][DC],
        g_threat_types[k], agent_id);

      if (result) {
        DEBUG_TSS("Match pattern %d (%s)\n", k, g_threat_types[k]);
        threat.gain++;

        /* Crtical action, one step winning (add gain again, since one step win should have higher priority than double threat) */
        if (k == CRITICAL_THREAT_TYPE_ID) {
            threat.gain = CRITICAL_THREAT_GAIN;
            threat_list.push_back(threat);
            return threat.gain;
        }
      }
    }
  }

  if (threat.gain) {
    threat_list.push_back(threat);
  }

  return threat.gain;
}

}

#endif
