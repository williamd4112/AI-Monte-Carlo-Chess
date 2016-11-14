#include <iostream>
#include <fstream>
#include "constants.h"
#include "board.h"

using namespace std;
using namespace mcts;

int main(int argc, char * argv[])
{
  if (argc < 3) {
    cout << "./test_board <state file> <update_row> <update_col>" << endl;
    return -1;
  }

  std::vector<std::vector<char>> position(15, std::vector<char>(15, BLACK));
  ifstream in(argv[1]);
  load_position_from(in, position, 15, 15);

  cout << "Test find_connectivities" << endl;
  board_t board(15, std::vector<board_node_t>(15));
  find_connectivities(board, position, BLACK);
  show(board);

  cout << "Test update_connectivity_at" << endl;
  update_connectivity_at(board, position, atoi(argv[2]), atoi(argv[3]), 15, 15);
  show(board);

  return 0;
}
