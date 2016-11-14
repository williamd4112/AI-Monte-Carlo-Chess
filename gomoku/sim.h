#ifndef _SIM_H_
#define _SIM_H_

#include "util.h"
#include "state.h"
#include "fast_tss.h"
#include "constants.h"
#include "policy.h"
#include "board.h"
#include "debug.h"

#include <chrono>
#include <random>
#include <vector>

namespace mcts
{
#define INVALID -1
#define is_valid_move(m) ((m).first != INVALID && (m).second != INVALID)

  const static move_t INVALID_MOVE(INVALID, INVALID);

  int sim_check_win(const State & state)
  {
    return util_check_win(state.position, state.board_width, state.board_height);
  }

  int sim_check_win(const Position & position, board_t & board, int agent_id, int row, int col, int w, int h)
  {
    int connectivity = update_connectivity_at(board, position, row, col, w, h);

    return connectivity >= NUMTOWIN ? agent_id : NOT_END;
  }

  const move_t sim_single_iteration_random(Policy & policy, const State & state, int max_random_moves, std::mt19937 & random_gen)
  {
    std::vector<move_t> next_moves;
    policy.move_rapid(state, next_moves, max_random_moves);

    if (!next_moves.empty()) {
      return next_moves[random_gen() % next_moves.size()];
    }
    return INVALID_MOVE;
  }

  const move_t sim_single_iteration_random(Policy & policy, const State & state, State & next_state, int max_random_moves, std::mt19937 & random_gen)
  {
    const move_t next_move = sim_single_iteration_random(policy, state, max_random_moves, random_gen);
    if (is_valid_move(next_move)) {
      next_state = state;
      next_state.agent_id ^= (1 << 0);
      next_state.position[next_move.first][next_move.second] = next_state.agent_id;
    }
    return next_move;
  }

  /*
   * @brief simulate a playout with only THREAT_LEVEL_5 Threat-space search and random moves
   * @return BLACK: black win
   *         WHITE: white win
   *         EMPTY: tie
   * */
  int sim_rapid_until_end(Policy & policy, const State & state, int max_iter, int max_random_moves)
  {
    assert(state.board_width * state.board_height >= max_random_moves);

    const static int BLACK_ID = (int) BLACK;
    const static int WHITE_ID = (int) WHITE;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 random_gen(seed);

    const int w = state.board_width;
    const int h = state.board_height;

    State last_state(state);
    State next_state(state);

    board_t boards[2] = {
      board_t(state.board_height, std::vector<board_node_t>(state.board_width)),
      board_t(state.board_height, std::vector<board_node_t>(state.board_width)) };
    find_connectivities(boards[BLACK_ID], state.position, BLACK);
    find_connectivities(boards[WHITE_ID], state.position, WHITE);

    int res = NOT_END;
    for (int iter = 0; iter < max_iter; iter++) {
      const move_t next_move = sim_single_iteration_random(policy, last_state, next_state, max_random_moves, random_gen);

      DEBUG_SIM("Iter = %d; Agent = %d; Move = (%d, %d)\n", iter, next_state.agent_id, next_move.first, next_move.second);
      DEBUG_SIM_STATE(next_state);

      if (!is_valid_move(next_move)) {
        res = EMPTY;
        break;
      }

      int next_state_agent_id = (int)next_state.agent_id;
      res = sim_check_win(last_state.position, boards[next_state_agent_id], next_state_agent_id, next_move.first, next_move.second, w, h);

      DEBUG_SIM("Check winning status %d\n", next_state_agent_id);
      if (res == BLACK || res == WHITE) {
        break;
      }

      last_state = next_state;
    }
    return res;
  }
}

#endif
