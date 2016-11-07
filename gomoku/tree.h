#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <cfloat>

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

  TreeNode* get_root_node()
  {
    return root_node;
  }

  void reset()
  {
    root_node = new TreeNode(root_state);
  }

  TreeNode* select(double total_sim_count, double k_explore) const
  {
    TreeNode* visiting_node = NULL;
    TreeNode* child_highest_ucb = root_node;
    while (child_highest_ucb != NULL) {
      visiting_node = child_highest_ucb;
      child_highest_ucb =
        visiting_node->get_child_highest_ucb(total_sim_count, k_explore);
    }
    return visiting_node;
  }

  TreeNode* expand(TreeNode* node) const
  {
    if (node->is_fully_expanded() || node->is_game_finished()) {
      return node;
    }
    TreeNode* expanded_node = node->expand();
    if (expanded_node != NULL) {
      return expanded_node;
    } else {
      return node;
    }
  }

  void simulate(TreeNode* node, std::vector<double>& payoffs) const
  {
    if (node->is_game_finished()) {
      return;
    }
    node->simulate(payoffs);
  }

  void backpropagate(TreeNode* node,
                     const std::vector<double>& payoffs) const
  {
    TreeNode* visiting_node = node;
    while (visiting_node != NULL) {
      visiting_node->update(payoffs);
      visiting_node = visiting_node->get_parent();
    }
  }

  TreeNode* get_best_node() const
  {
    return root_node->get_child_highest_sim_count();
  }

private:
  TreeNode* root_node;

  State root_state;
};

}

#endif // TREE_H_INCLUDED
