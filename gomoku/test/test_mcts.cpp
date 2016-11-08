#include "test_base.h"

#include <vector>

#include "../state.h"
#include "../tree.h"
#include "../tree_node.h"

using namespace mcts;

TEST_CASE("mcts", "[mcts]")
{
  State root_state(15, 15, BLACK);
  Tree tree(root_state);

  SECTION("Should run MCTS with empty root node") {
    double total_sim_count = 0;
    double k_explore = 1.41;
    // Should select root node
    TreeNode* root_node = tree.get_root_node();
    TreeNode* selected_node = tree.select(total_sim_count, k_explore);
    REQUIRE(selected_node == root_node);
    // Should expand and return the expanded node
    TreeNode* expanded_node = tree.expand(selected_node);
    REQUIRE(expanded_node != NULL);
    // Should simulate and return non-zero payoffs
    std::vector<double> payoffs {0.0, 0.0};
    tree.simulate(expanded_node, payoffs);
    REQUIRE(payoffs.size() == 2);
    REQUIRE(payoffs[BLACK] != 0);
    REQUIRE(payoffs[WHITE] != 0);
    // Should backpropagate
    payoffs[0] = 1.0;
    payoffs[1] = -1.0;
    tree.backpropagate(expanded_node, payoffs);
    REQUIRE(root_node->get_payoff() == 1.0);
    REQUIRE(root_node->get_simulation_count() == 1);
    REQUIRE(root_node->is_fully_expanded() == true);
    REQUIRE(root_node->is_game_finished() == false);
  }
}
