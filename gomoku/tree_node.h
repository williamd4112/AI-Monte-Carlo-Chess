#ifndef TREE_NODE_H_INCLUDED
#define TREE_NODE_H_INCLUDED

#include <cfloat>
#include <cmath>
#include <memory>
#include <vector>

#include "state.h"

namespace mcts
{

class TreeNode
{
public:
  typedef std::shared_ptr<TreeNode> Ptr;

  TreeNode(const State& state, TreeNode* parent = NULL):
    parent(parent),
    payoff(0.0),
    simulation_count(0),
    game_finished(false),
    state(state)
  {
  }

  bool is_game_finished() const
  {
    return game_finished;
  }

  double get_payoff() const
  {
    return payoff;
  }

  double get_simulation_count() const
  {
    return simulation_count;
  }

  TreeNode* get_parent() const
  {
    return parent;
  }

  TreeNode* get_child_highest_ucb(double total_sim_count,
                                  double k_explore) const
  {
    TreeNode* child_highest_ucb = NULL;
    double highest_ucb = -DBL_MAX;
    for (const auto& child_ptr: children) {
      TreeNode* child = child_ptr.get();
      double child_ucb = child->get_ucb(total_sim_count, k_explore);
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

  double get_ucb(double total_sim_count, double k_explore) const
  {
    double exploitation = payoff / (simulation_count + DBL_EPSILON);
    double exploration = sqrt(log(total_sim_count + 1) /
                              (simulation_count + DBL_EPSILON));
    double score = exploitation + k_explore * exploration;
    return score;
  }

  TreeNode* expand()
  {
    if (is_fully_expanded()) {
      return NULL;
    }
    if (actions.empty()) {
      state.get_expanded_states(actions);
      if (actions.empty()) {
        game_finished = true;
        return NULL;
      }
    }
    const State& next_action = actions[children.size()];
    return add_child(next_action);
  }

  void simulate(std::vector<double>& payoffs) const
  {
    state.simulate(payoffs);
  }

  void update(const std::vector<double> &payoffs)
  {
    int agent_id = state.get_agent_id();
    payoff += payoffs[agent_id];
    simulation_count += 1;
  }

private:
  TreeNode* parent;
  std::vector<Ptr> children;
  std::vector<State> actions;

  double payoff;
  double simulation_count;
  bool game_finished;
  State state;

  bool is_fully_expanded() const
  {
    return children.empty() == false && children.size() == actions.size();
  }

  TreeNode *add_child(const State& action)
  {
    TreeNode *new_child = new TreeNode(action, this);
    children.push_back(Ptr(new_child));
    return new_child;
  }
};

std::ostream& operator<<(std::ostream &strm, const TreeNode& obj)
{
  return strm << "TreeNode(" << obj.get_payoff() << ")";
}

}

#endif // TREE_NODE_H_INCLUDED
