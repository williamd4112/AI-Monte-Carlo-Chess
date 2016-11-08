#ifndef MCTS_H_INCLUDED
#define MCTS_H_INCLUDED

#include <vector>

#include "state.h"
#include "timer.h"
#include "tree.h"
#include "tree_node.h"

namespace mcts
{
class MCTS
{
public:
  MCTS(Timer* timer, double k_explore, bool verbose=false):
    timer(timer),
    k_explore(k_explore),
    verbose(verbose)
  {
  }

  void run(const State& root_state, State& result_state) const
  {
    Tree tree(root_state);
    TreeNode* root_node = tree.get_root_node();
    std::vector<double> payoffs {0.0, 0.0};
    timer->reset();
    timer->init();
    while (!timer->check_resource_limit()) {
      timer->start_loop();
      double root_node_sim_count = root_node->get_simulation_count();
      TreeNode* selected_node = tree.select(root_node_sim_count, k_explore);
      TreeNode* expanded_node = tree.expand(selected_node);
      tree.simulate(expanded_node, payoffs);
      tree.backpropagate(expanded_node, payoffs);
    }
    TreeNode* best_node = tree.get_best_node();
    best_node->get_state(result_state);
    if (verbose) {
      std::cout << "duration: " << timer->get_duration() << '\n';
      std::cout << "iteration count: " << timer->iteration_count << '\n';
    }
  }

private:
  Timer* timer;
  double k_explore;

  /* Debugging */
  bool verbose;
};
}

#endif // MCTS_H_INCLUDED
