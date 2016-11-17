#include "util.h"

namespace mcts
{

int util_check_chain(const Position & position, int w, int h, int row, int col, int dr, int dc, int agent_id)
{
  int i = row;
  int j = col;
  int k = 0;
  for (; k < NUMTOWIN; k++) {
    if (i < 0 || i >= h || j < 0 || j >= w) {
      break;
    }
    if (position[i][j] != agent_id) {
      break;
    }
    i += dr;
    j += dc;
  }
  return k;
}

/*
 * @brief check winning status
 * @return BLACK black win
 *         WHITE white win
 *         EMPTY tie
 *         NOT_END not end
 * */
int util_check_win(const Position & position, int w, int h)
{
  const static int dirs[][2] = {
    {1, 0},
    {0, 1},
    {1, 1},
    {-1, 1}
  };

  const static int dir_num = 4;

  bool is_tie = true;

  for (int k = 0; k < dir_num; k++) {
    int dr = dirs[k][0];
    int dc = dirs[k][1];
    for (int i = 0; i < h; i++) {
      for (int j = 0; j < w; j++) {
        int chess = position[i][j];
        if (position[i][j] != EMPTY) {
          int len = util_check_chain(position, h, w, i, j, dr, dc, chess);
          if (len == NUMTOWIN) {
            return chess;
          }
        } else {
          is_tie = false;
        }
      }
    }
  }

  return (is_tie) ? EMPTY : NOT_END;
}


void print_position(std::ostream& strm, const Position& position)
{
  size_t width = 0;
  if (!position.empty()) {
    width = position[0].size();
  }
  if (width >= 10) {
    strm << "  ";
    for (size_t i = 0; i < width; ++i) {
      int ten = (i / 10);
      if (ten == 0) {
        strm << "  ";
      } else {
        strm << ten << " ";
      }
    }
    strm << '\n';
  }
  strm << "  ";
  for (size_t i = 0; i < width; ++i) {
    strm << (i % 10) << " ";
  }
  strm << '\n';
  int y = 0;
  for (const auto& row : position) {
    strm << (char)('A' + y) << " ";
    for (const auto& val: row) {
      if (val == 0) {
        strm << 'O';
      } else if (val == 1) {
        strm << 'X';
      } else if (val == 2) {
        strm << '.';
      } else {
        strm << val;
      }
      strm << " ";
    }
    strm << '\n';
    y += 1;
  }
}

void load_position_from(std::istream & in, Position & position, int w, int h)
{
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      char val;
      in >> val;
      if (val == 'O' || val == 'o')
        position[i][j] = BLACK;
      if (val == 'X' || val == 'x')
        position[i][j] = WHITE;
      if (val == '.')
        position[i][j] = EMPTY;
    }
  }
}

void find_position_diff(const Position& before, const Position& after,
                        point_t& point)
{
  point.i = -1;
  point.j = -1;
  for (unsigned y = 0; y < before.size(); ++y) {
    const auto& row = before[y];
    for (unsigned x = 0; x < row.size(); ++x) {
      if (row[x] != after[y][x]) {
        point.j = y;
        point.i = x;
        return;
      }
    }
  }
}

void get_position_size(const Position& position, point_t& pos_size)
{
  pos_size.j = position.size();
  if (!position.empty()) {
    pos_size.i = position[0].size();
  } else {
    pos_size.i = 0;
  }
}

}
