#ifndef _BOARD_H_
#define _BOARD_H_

#include <vector>
#include <iostream>
#include "debug.h"
#include "constants.h"

#define NUM_DIR 4
#define ROW 0
#define COL 1

/*
 * Vertical,
 * Horizontal,
 * Diagonal-Up,
 * Diagonal-Bottom
 */
const int BOARD_DIRS[4][2] = {
    {  1, 0 },
    {  0, 1 },
    {  1, 1 },
    { -1, 1 }
};

struct board_node_t
{
  bool visited[NUM_DIR];
  int open[NUM_DIR];
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

void find_connectivity_at(board_t & board, const board_map_t & map, int agent_id, int row, int col, int dir, int w, int h)
{
  int i = row;
  int j = col;
  int dr = BOARD_DIRS[dir][ROW];
  int dc = BOARD_DIRS[dir][COL];
  int cnt = 0;

  while (map[i][j] == agent_id) {
    cnt++;
    DEBUG_BOARD("Fill (%d, %d) (%d, %d, [%d]) = %d\n", dr, dc, i, j, dir, cnt);
    board[i][j].visited[dir] = true;
    i += dr;
    j += dc;
  }

  while (row != i || col != j) {
    board[row][col].connectivity[dir] = cnt;
    DEBUG_BOARD("BP (%d, %d, [%d]) = %d\n", row, col, dir, cnt);
    row += dr;
    col += dc;
  }
}

/*
 * @brief fill connectivity in the board
 *        assume board and map has same dimension
 * */
void find_connectivities(board_t & board, const board_map_t & map, int agent_id)
{
  for (int k = 0; k < NUM_DIR; k++) {
    for (int i = 0; i < map.size(); i++) {
      for (int j = 0; j < map[i].size(); j++) {
        if (!board[i][j].visited[k]) {
          find_connectivity_at(board, map, agent_id, i, j, k, map[i].size(), map.size());
        }
      }
    }
  }
}

void show(board_t & board)
{
  for (int i = 0; i < NUM_DIR; i++) {
    std::cout << "Direction " << i << std::endl;
    for (std::vector<board_node_t> & row : board) {
      for (board_node_t & col : row) {
        std::cout << col.connectivity[i] << " ";
      }
      std::cout << std::endl;
    }
  }
}

#endif
