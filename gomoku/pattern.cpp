#include "pattern.h"
#include "constants.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include "debug.h"

#define in_boundary(i, j, w, h) (!((i) < 0 || (i) >= h || (j) < 0 || (j) >= w))

bool match_pattern_position(char type, int chess, int agent_id)
{
  if (type == SELF) {
    return chess == agent_id;
  }
  else if (type == BLANK) {
    return chess == mcts::EMPTY;
  }

  /* No reach */
  assert(false);
}

bool match_pattern(
  const std::vector<std::vector<char>> & position, 
  int row, int col, int w, int h, int dr, int dc,
  const char * pattern,
  int agent_id)
{
  int pattern_len = strlen(pattern);

  for (int cursor = 0; cursor < pattern_len; cursor++) {
    int begin_row = row - cursor * dr;
    int begin_col = col - cursor * dc;
    int end_row = row + (pattern_len - cursor) * dr;
    int end_col = col + (pattern_len - cursor) * dc;
    DEBUG("Begin at (%d, %d) End at (%d, %d)\n", begin_row, begin_col,
      end_row, end_col);   
    bool result = true;
    for (int i = 0; i < pattern_len; i++) {
      char type = pattern[i];
      int cur_row = begin_row + dr * i;
      int cur_col = begin_col + dc * i;

      if (!in_boundary(cur_row, cur_col, w, h)) {
        result = false;
        break;
      }

      if (!match_pattern_position(type, position[cur_row][cur_col], agent_id)) {
        result = false;
        break;
      }
    }
    if (result) {
      DEBUG("Match %s (%d, %d) -> (%d, %d)\n", pattern, begin_row, begin_col, end_row, end_col);      
      return true;
    }
  }

  return false;
}
