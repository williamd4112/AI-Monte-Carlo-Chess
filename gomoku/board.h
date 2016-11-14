#ifndef _BOARD_H_
#define _BOARD_H_

#include <vector>
#include <iostream>
#include <cmath>
#include "debug.h"
#include "constants.h"
#include "util.h"

#define NUM_DIR 4
#define ROW 0
#define COL 1

namespace mcts
{
/*
 * Vertical,
 * Horizontal,
 * Diagonal-Up,
 * Diagonal-Bottom
 */
const static int BOARD_DIRS[4][2] = {
  {  1, 0 },
  {  0, 1 },
  {  1, 1 },
  { -1, 1 }
};

struct board_node_t {
  bool visited[NUM_DIR];
  int connectivity[NUM_DIR];

  board_node_t():
    visited { 0 },
    connectivity { 0 }
  {
  }

  ~board_node_t()
  {
  }
};

typedef std::vector<std::vector<board_node_t>> board_t;
typedef std::vector<std::vector<char>> board_map_t;

int update_connectivity_at(board_t & board, const board_map_t & map, int row, int col, int w, int h);

void find_connectivity_at(board_t & board, const board_map_t & map, int agent_id, int row, int col, int dir, int w, int h);

/*
 * @brief fill connectivity in the board
 *        assume board and map has same dimension
 * */
void find_connectivities(board_t & board, const board_map_t & map, int agent_id);

void show(board_t & board);

}

#endif
