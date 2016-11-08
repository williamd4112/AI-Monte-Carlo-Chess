#ifndef _PATTERN_H_
#define _PATTERN_H_

#define SELF 'o'
#define FORBBIDEN 'x'
#define BLANK '_'

#include <vector>

bool match_pattern_position(char type, int chess, int agent_id);

bool match_pattern(
  const std::vector<std::vector<char>> & position, 
  int row, int col, int w, int h, int dr, int dc,
  const char * pattern,
  int agent_id);

#endif
