#include "board.h"

namespace mcts
{
typedef std::vector<std::vector<board_node_t>> board_t;
typedef std::vector<std::vector<char>> board_map_t;

int update_connectivity_at(board_t & board, const board_map_t & map, int row, int col, int w, int h)
{
  assert(in_boundary(row, col, w, h));
  assert(map[row][col] == EMPTY);

  int max_connectivity = 0;
  for (int d = 0; d < NUM_DIR; d++) {
    int dr = BOARD_DIRS[d][0];
    int dc = BOARD_DIRS[d][1];
    int connectivity = 1;

    int neg_row = row - dr;
    int neg_col = col - dc;
    int pos_row = row + dr;
    int pos_col = col + dc;

    int neg_connectivity = 0;
    if (in_boundary(neg_row, neg_col, w, h)) {
      neg_connectivity = board[neg_row][neg_col].connectivity[d];
      connectivity += neg_connectivity;
    }

    int pos_connectivity = 0;
    if (in_boundary(pos_row, pos_col, w, h)) {
      pos_connectivity = board[pos_row][pos_col].connectivity[d];
      connectivity += pos_connectivity;
    }

    for (int k = 0, i = neg_row, j = neg_col; k < neg_connectivity; k++, i -= dr, j -= dc) {
      if (in_boundary(i, j, w, h)) {
        board[i][j].connectivity[d] = connectivity;
      }
    }

    for (int k = 0, i = pos_row, j = pos_col; k < neg_connectivity; k++, i += dr, j += dc) {
      if (in_boundary(i, j, w, h)) {
        board[i][j].connectivity[d] = connectivity;
      }
    }

    board[row][col].connectivity[d] = connectivity;

    max_connectivity = std::max(max_connectivity, connectivity);
  }

  return max_connectivity;
}

void find_connectivity_at(board_t & board, const board_map_t & map, int agent_id, int row, int col, int dir, int w, int h)
{
  int i = row;
  int j = col;
  int dr = BOARD_DIRS[dir][ROW];
  int dc = BOARD_DIRS[dir][COL];
  int cnt = 0;

  if (in_boundary(i, j, w, h)) {
    while (in_boundary(i, j, w, h) && map[i][j] == agent_id) {
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
}

/*
 * @brief fill connectivity in the board
 *        assume board and map has same dimension
 * */
void find_connectivities(board_t & board, const board_map_t & map, int agent_id)
{
  for (int k = 0; k < NUM_DIR; k++) {
    for (int i = 0; i < (int)map.size(); i++) {
      for (int j = 0; j < (int)map[i].size(); j++) {
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

}
