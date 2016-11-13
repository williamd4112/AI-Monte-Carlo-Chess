#ifndef _SIM_H_
#define _SIM_H_

#include "util.h"
#include "state.h"
#include "fast_tss.h"
#include "constants.h"
#include "policy.h"
#include "debug.h"

#include <chrono>
#include <random>
#include <vector>

namespace mcts
{
  int sim_check_win(const State & state)
  {
    return util_check_win(state.position, state.board_width, state.board_height);
  }

  void sim_single_iteration_tss(const State & state, State & next_state, int max_depth, int random_num)
  {
    std::vector<State> next_states;
    policy_aggresive(state, next_states, max_depth);

    next_state = next_states[random_num % next_states.size()];
  }

  void sim_single_iteration_random(const State & state, State & next_state, int max_iter)
  {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 random_gen(seed);

    std::vector<State> next_states;
    policy_simple_random(state, next_states, max_iter);

    if (!next_states.empty()){
      next_state = next_states[random_gen() % next_states.size()];
    }
  }

  /*
   * @return BLACK: black win
   *         WHITE: white win
   *         EMPTY: tie
   * */
  int sim_until_end(const State & state, int max_depth, int max_iter)
  {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 random_gen(seed);

    State last_state(state);
    State next_state(state);

    int res = NOT_END;
    for (int iter = 0; iter < max_iter; iter++) {
      sim_single_iteration_random(last_state, next_state, max_depth);

      DEBUG_SIM("Iter = %d; Agent = %d\n", iter, next_state.agent_id);
      DEBUG_SIM_STATE(next_state);

      res = sim_check_win(next_state);
      if (res != NOT_END) {
        break;
      }
      last_state = next_state;
    }
    return res;
  }
}

#endif
