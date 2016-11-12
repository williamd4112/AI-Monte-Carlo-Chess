#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <iostream>
#include "constants.h"
#include "state.h"

namespace mcts
{

#define WINNING_CHAIN_NUM 5

typedef std::vector<std::vector<char> > Position;

int util_check_chain(const Position & position, int w, int h, int row, int col, int dr, int dc, int agent_id)
{
    int i = row;
    int j = col;
    int k = 0;
    for (; k < WINNING_CHAIN_NUM; k++) {
        if (i < 0 || i >= h || j < 0 || j >= w)
            break;
        if (position[i][j] != agent_id)
            break;
        i += dr;
        j += dc;
    }
    return k;
}

int util_check_win(const Position & position, int w, int h)
{
    const static int dirs[][2] = {
        {1, 0},
        {0, 1},
        {1, 1},
        {-1, 1}
    };

    const static int dir_num = 4;

    for (int k = 0; k < dir_num; k++) {
        int skip = 0;
        int dr = dirs[k][0];
        int dc = dirs[k][1];
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                if (skip > 0) {
                    skip--;
                }

                int chess = position[i][j];
                if (position[i][j] != EMPTY) {
                    int len = util_check_chain(position, h, w, i, j, dr, dc, chess);
                    if (len == WINNING_CHAIN_NUM)
                        return chess;
                    else
                        skip = len - 1;
                }
            }
        }
    }

    return EMPTY;
}


static void print_position(std::ostream& strm, const Position& position)
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

static void print_position_num(std::ostream& strm, const Position& position)
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
      strm << (char)('0' + val) << " ";
    }
    strm << '\n';
    y += 1;
  }
}

}

#endif // UTIL_H_INCLUDED
