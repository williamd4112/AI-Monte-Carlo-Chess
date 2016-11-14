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
class Policy;
#define INVALID -1
#define is_valid_move(m) ((m).first != INVALID && (m).second != INVALID)
typedef std::pair<int, int> move_t;

const static move_t INVALID_MOVE(INVALID, INVALID);

int sim_check_win(const State & state);

int sim_check_win(const Position & position, board_t & board, int agent_id, int row, int col, int w, int h);

const move_t sim_single_iteration_random(Policy & policy, const State & state, int max_random_moves, std::mt19937 & random_gen);

const move_t sim_single_iteration_random(Policy & policy, const State & state, State & next_state, int max_random_moves, std::mt19937 & random_gen);

/*
 * @brief simulate a playout with only THREAT_LEVEL_5 Threat-space search and random moves
 * @return BLACK: black win
 *         WHITE: white win
 *         EMPTY: tie
 * */
int sim_rapid_until_end(Policy & policy, const State & state, int max_iter, int max_random_moves);
}

#endif
