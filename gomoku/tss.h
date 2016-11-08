#ifndef _TSS_H_
#define _TSS_H_

#include <iostream>
#include <vector>

#include "constants"
#include "state.h"

class Tss
{
public:
  struct point_t
  {
    int i, j;
  }

  struct threat_t
  {
    std::vector<point_t> gains;
    std::vector<point_t> costs;
  }

  Tss(const State & state);
  ~Tss();
  
  std::vector<threat_t> & get_threats();
private:
  State & m_state;
  std::vector<threat_t> m_threats; 

  void find_all_threats();
  void find_threat_at(int , int);
};

#endif
