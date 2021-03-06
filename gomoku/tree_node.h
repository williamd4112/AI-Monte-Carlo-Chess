#ifndef TREE_NODE_H_INCLUDED
#define TREE_NODE_H_INCLUDED

#include <cfloat>
#include <cmath>
#include <memory>
#include <vector>

#include "sim.h"
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
    simulation_count(0.0),
    game_finished(false),
    state(state)
  {
  }

  bool is_fully_expanded() const
  {
    return (!children.empty()) &&
           (children.size() == actions.size());
  }

  bool is_game_finished() const
  {
    return game_finished;
  }

  double get_ucb(double total_sim_count, double k_explore) const
  {
    double exploitation = payoff / (simulation_count + DBL_EPSILON);
    double exploration = sqrt(log(total_sim_count + 1) /
                              (simulation_count + DBL_EPSILON));
    double score = exploitation + k_explore * exploration;
    return score;
  }

  TreeNode* get_parent() const
  {
    return parent;
  }

  double get_payoff() const
  {
    return payoff;
  }

  double get_simulation_count() const
  {
    return simulation_count;
  }

  void get_state(State& out_state) const
  {
    out_state = State(state);
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
    return child_highest_ucb;
  }

  TreeNode* get_child_highest_sim_count() const
  {
    TreeNode* child_highest_sim_count = NULL;
    double highest_sim_count = -DBL_MAX;
    for (const auto& child_ptr: children) {
      TreeNode* child = child_ptr.get();
      double child_sim_count = child->get_simulation_count();
      if (child_sim_count > highest_sim_count) {
        child_highest_sim_count = child;
        highest_sim_count = child_sim_count;
      }
    }
    return child_highest_sim_count;
  }

  TreeNode* expand()
  {
    if (is_fully_expanded()) {
      return NULL;
    }
    if (actions.empty()) {
      int strategy = 0;
      const auto& position = state.position;
      int stone_count = 0;
      for (size_t y = 0; y < position.size(); ++y) {
        const auto& row = position[y];
        for (size_t x = 0; x < row.size(); ++x) {
          if (row[x] != EMPTY) {
            stone_count += 1;
          }
        }
      }
      if (stone_count > 1) {
        strategy = STRATEGY_BALANCE;
      } else {
        strategy = STRATEGY_APPROACH;
      }
      state.get_expanded_states(actions, strategy);
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

  void update(const std::vector<double>& payoffs)
  {
    payoff += payoffs[state.agent_id];
    simulation_count += 1;
  }

private:
  /* Relative nodes */
  TreeNode* parent;
  std::vector<Ptr> children;
  std::vector<State> actions;

  /* Node itself */
  double payoff;
  double simulation_count;
  bool game_finished;
  State state;

  TreeNode* add_child(const State& action)
  {
    TreeNode* new_child = new TreeNode(action, this);
    new_child->parent = this;
    char winner = sim_check_win(action);
    if (winner != EMPTY && winner != NOT_END) {
      new_child->game_finished = true;
    }
    children.push_back(Ptr(new_child));
    return new_child;
  }
};

std::ostream& operator<<(std::ostream& strm, const TreeNode& obj)
{
  return strm << "TreeNode(" << obj.get_payoff() << ")";
}

}

#endif // TREE_NODE_H_INCLUDED
