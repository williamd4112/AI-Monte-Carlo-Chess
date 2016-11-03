#include <iostream>
#include <vector>

#include "state.h"
#include "tree.h"
#include "tree_node.h"

using namespace std;

int main()
{
  mcts::State state(15, 15);
  mcts::Tree tree(state);

  mcts::TreeNode* node = tree.select();
  std::cout << state << '\n';

  return 0;
}
