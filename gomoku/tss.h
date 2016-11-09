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

namespace mcts
{
const static char * g_threat_types[] = {
  /* Five */
  "ooooo",
  /* Four */
  "oooo_",
  "ooo_o",
  "oo_oo",
  "o_ooo",
  "_oooo",
  /* Three */
  "_ooo_",
  "_o_oo_",
  "_oo_o_"
};

const static int g_threat_types_num = sizeof(g_threat_types) / sizeof(char *);

/*
 * Vertical,
 * Horizontal,
 * Diagonal-Up,
 * Diagonal-Bottom
 */
const static int dirs[4][2] = {
    { 1, 0 },
    { 0, 1 },
    { 1, 1 },
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
   * @return threat list
   * */
  std::vector<threat_t> & find_all_threats(std::vector<threat_t> & threats);

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
   * @param[Out] threats collection
   * @return gain of threat
   * */
  int find_threat_at(const State::Position & position, int row, int col, int w, int h, int agent_id, std::vector<threat_t> & threat_list);
};

mcts::Tss::Tss(const mcts::State & state):
  m_state(state)
{
}

mcts::Tss::~Tss()
{

}

std::vector<Tss::threat_t> & Tss::find_all_threats(std::vector<threat_t> & threats)
{
  int w = m_state.board_width;
  int h = m_state.board_height;

  State::Position position = m_state.position;

  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      if (m_state.position[i][j] == mcts::EMPTY) {

          position[i][j] = m_state.agent_id;
          int gain = find_threat_at(position, i, j, w, h, m_state.agent_id, threats);
          position[i][j] = mcts::EMPTY;

      }
    }
  }

  std::sort(threats.begin(), threats.end(), std::greater<threat_t>());

  return threats;
}

int Tss::find_threat_at(const State::Position & position,
        int row, int col, int w, int h, int agent_id, std::vector<threat_t> & threat_list)
{
  threat_t threat = threat_t{ point_t{row, col}, 0 };

  for (int k = 0; k < g_threat_types_num; k++) {
    for (int d = 0; d < 4; d++) {
      bool result = match_pattern(position,
                            row, col, w, h, dirs[d][0], dirs[d][1],
                            g_threat_types[k], agent_id);
      if (result) {
        threat.gain++;

        /* Crtical action, one step winning */
        if (k == CRITICAL_THREAT_TYPE_ID) {
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
