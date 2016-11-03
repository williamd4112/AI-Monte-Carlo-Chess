#include <iostream>
#include <vector>

#include "constants.h"
#include "state.h"
#include "tree.h"
#include "tree_node.h"

int main()
{
  mcts::State state(15, 15, mcts::BLACK);
  mcts::Tree tree(state);
  mcts::TreeNode* node = tree.select(3, 1.41);
  node = tree.expand(node);
  std::vector<double> payoffs {0.0, 0.0};
  tree.simulate(node, payoffs);
  tree.backpropagate(node, payoffs);
  std::cout << *node << '\n';
  std::cout << state << '\n';

  return 0;
}
