#ifndef TREE_NODE_H_INCLUDED
#define TREE_NODE_H_INCLUDED

#include <cfloat>
#include <memory>
#include <vector>

#include "state.h"

namespace mcts
{

class TreeNode
{
public:
  TreeNode(const State& state, TreeNode* parent = NULL):
    parent(parent),
    payoff(0),
    ucb(0),
    game_finished(false),
    simulation_count(0),
    state(state)
  {
  }

  double get_payoff() const
  {
    return payoff;
  }

  bool is_game_finished()
  {
    return game_finished;
  }

  TreeNode* get_child_highest_ucb()
  {
    TreeNode* child_highest_ucb = NULL;
    double highest_ucb = -DBL_MAX;
    for (TreeNode* child: children) {
      double child_ucb = child->ucb;
      if (child_ucb > highest_ucb) {
        child_highest_ucb = child;
        highest_ucb = child_ucb;
      }
    }
    if (child_highest_ucb != NULL) {
      if (!child_highest_ucb->is_game_finished()) {
        return child_highest_ucb;
      } else {
        return NULL;
      }
    } else {
      return NULL;
    }
  }

private:
  TreeNode* parent;
  std::vector<TreeNode*> children;

  double payoff;
  double ucb;
  bool game_finished;
  double simulation_count;

  State state;
};

std::ostream& operator<<(std::ostream &strm, const TreeNode& obj)
{
  return strm << "TreeNode(" << obj.get_payoff() << ")";
}

}

#endif // TREE_NODE_H_INCLUDED
