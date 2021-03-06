#include "pattern.h"

bool match_pattern_position(char type, int chess, int agent_id)
{
  int opponent_id = agent_id ^ (1 << 0);

  if (type == SELF) {
    return chess == agent_id;
  } else if (type == FORBBIDEN) {
    return chess == opponent_id;
  } else if (type == BLANK) {
    return chess == mcts::EMPTY;
  }

  /* No reach */
  DEBUG_PATTERN("chess = 0x%d; type = %c\n", chess, type);
  assert(false);
}

int match_pattern(
  const std::vector<std::vector<char>> & position,
  int row, int col, int w, int h, int dr, int dc,
  const char * pattern, int pattern_len,
  int agent_id)
{
  DEBUG_PATTERN("match_pattern(%d, %d) = %s : len = %d\n", dr, dc, pattern, pattern_len);
  for (int cursor = 0; cursor < pattern_len; cursor++) {
    int begin_row = row - cursor * dr;
    int begin_col = col - cursor * dc;

    DEBUG_PATTERN("Begin at (%d, %d) End at (%d, %d)\n", begin_row, begin_col,
                  end_row, end_col);

    int result = cursor;
    for (int i = 0; i < pattern_len; i++) {
      char type = pattern[i];
      int cur_row = begin_row + dr * i;
      int cur_col = begin_col + dc * i;

      if (!in_boundary(cur_row, cur_col, w, h)) {
        result = MISMATCH;
        break;
      }

      if (!match_pattern_position(type, position[cur_row][cur_col], agent_id)) {
        result = MISMATCH;
        break;
      }

      DEBUG_PATTERN("Match 0x%x (%c) [%d, %d]\n", position[cur_row][cur_col], type, cur_row, cur_col);
    }

    if (result != MISMATCH) {
      DEBUG_PATTERN("Match %s (%d, %d) -> (%d, %d)\n", pattern, begin_row, begin_col, end_row, end_col);
      return cursor;
    }
  }
  return MISMATCH;
}
