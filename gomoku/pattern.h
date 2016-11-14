#ifndef _PATTERN_H_
#define _PATTERN_H_

#define SELF 'o'
#define FORBBIDEN 'x'
#define BLANK '_'
#define MISMATCH -1

#include <vector>

#include "constants.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <iostream>
#include "debug.h"
#include "util.h"

#define in_boundary(i, j, w, h) (!((i) < 0 || (i) >= h || (j) < 0 || (j) >= w))

bool match_pattern_position(char type, int chess, int agent_id);

int match_pattern(
  const std::vector<std::vector<char>> & position,
  int row, int col, int w, int h, int dr, int dc,
  const char * pattern, int pattern_len,
  int agent_id);

#endif
