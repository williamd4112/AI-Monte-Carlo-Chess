#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include "state.h"
#include "tree_node.h"

namespace mcts
{

class Tree
{
public:
  Tree(const State& root_state):
    root_state(root_state)
  {
    reset();
  }

  void reset()
  {
    root_node = new TreeNode(root_state);
  }

  TreeNode* select()
  {
    TreeNode* visiting_node = NULL;
    TreeNode* child_highest_ucb = root_node;
    do {
      visiting_node = child_highest_ucb;
      child_highest_ucb = visiting_node->get_child_highest_ucb();
    } while (child_highest_ucb != NULL);
    return visiting_node;
  }

  TreeNode* expand(TreeNode* node)
  {
    if (node->is_game_finished()) {
      return node;
    }

  }

private:
  TreeNode* root_node;

  State root_state;
};

}

#endif // TREE_H_INCLUDED
