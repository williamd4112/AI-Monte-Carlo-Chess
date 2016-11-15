#ifndef _BOARD_H_
#define _BOARD_H_

#include <climits>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <vector>

#include "debug.h"
#include "constants.h"
#include "util.h"

#define NUM_DIR 4
#define ROW 0
#define COL 1

namespace mcts
{

struct board_node_t;

typedef std::vector<char> Row;
typedef std::vector<Row> Position;
typedef std::pair<int, int> move_t;
typedef std::vector<std::vector<board_node_t>> board_t;
typedef std::vector<std::vector<char>> board_map_t;

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

/**
 * @field i row
 * @field j col
 */
struct point_t {
  int i, j;

  friend bool operator ==(const point_t & a, const point_t & b)
  {
    return a.i == b.i && a.j == b.j;
  }
};

/*
 * @brief define threat struct
 * @field point position in board
 * @field gain number of attack caused by this point
 */
struct threat_t {
  point_t point;
  std::vector<threat_t> children;
  bool winning;
  bool final_winning;
  int min_winning_depth;

  threat_t(point_t _point, bool _winning):
    point(_point),
    winning(_winning),
    final_winning(false),
    min_winning_depth(INT_MAX)
  {

  }

  threat_t(const threat_t & copy):
    point(copy.point),
    children(copy.children),
    winning(copy.winning),
    final_winning(copy.final_winning),
    min_winning_depth(copy.min_winning_depth)
  {

  }

  void print()
  {
    print_r(*this, 0);
  }

  friend bool operator > (const threat_t & a, const threat_t & b)
  {
    if (a.final_winning && !b.final_winning) {
      return true;
    } else if (!a.final_winning && b.final_winning) {
      return false;
    } else {
      return a.min_winning_depth < b.min_winning_depth;
    }
  }

private:
  void print_r(threat_t & t, int d)
  {
    for (int i = 0; i < d; i++) {
      putchar('-');
    }
    printf("Threat (%c, %d): [%d, %d, %d]\n", t.point.i + 'A', t.point.j, t.winning, t.final_winning, t.min_winning_depth);
    for (threat_t & child : t.children) {
      print_r(child, d + 1);
    }
  }
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
