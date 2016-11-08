#include "tss.h"

#include <cstring>
#include "debug.h"
#include "pattern.h"

const static char * g_threat_types[] = {
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

const static int dirs[4][2] = {
    { 1, 0 },
    { 0, 1 },
    { 1, 1 },
    { -1, 1 }
};

tss::tss(const State & state):
  m_state(state)
{
  find_all_threats();
}

tss::~tss()
{

}

std::vector<tss:::threat_t> tss::get_threats()
{
  return m_threats;
}

void tss::find_all_threats()
{
  int w = m_state.get_width();
  int h = m_state.get_height();

  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      if (m_state.position[i][j] == mcts::EMPTY) {
        find_threat_at(i, j);      
      }
    }
  }
}

void tss::find_threat_at(int row, int col)
{
  for (int k = 0; k < g_threat_types_num; k++) {
    for (int d = 0; d < 4; d++) {
      bool result = match_pattern(m_state.position, row, col, m_state.get_width(), m_state.get_height(), dirs[d][0], dirs[d][1],
        g_threat_rules[k], m_state.agent_id);  
      if (result) {
        m_threats.gains.push_back(point_t {row, col});          
      }
    }
  }
}



