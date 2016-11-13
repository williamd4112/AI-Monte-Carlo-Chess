#include "fast_tss.h"
#include "policy.h"
#include "state.h"
#include "constants.h"
#include "util.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <queue>

using namespace std;
using namespace mcts;

pair<int, int> count(const State & s, int w, int h)
{
    pair<int, int> res;
    res.first = 0;
    res.second = 0;

    for (int i = 0; i < h; i++){
        for (int j = 0; j < w; j++) {
            if (s.position[i][j] == BLACK)
                res.first++;
            if (s.position[i][j] == WHITE)
                res.second++;
        }
    }

    return res;
}

State run(const State & state, bool is_cpu, bool * is_over)
{
    int winner = EMPTY;
    cout << state << endl;
    if ((winner = util_check_win(state.position, state.board_width, state.board_height)) != NOT_END) {
        if (winner == EMPTY)
          cout << "Tie" << endl;
        else
          cout << "Agent " << winner << " win !" << endl;
        *is_over = true;
        return state;
    }

    if (is_cpu) {
      std::vector<State> next_states;
      int res = policy_aggresive(state, next_states);
      if (res == POLICY_FAIL) {
          *is_over = true;
          return state;
      }
      return next_states[rand() % next_states.size()];
    }
    else {
        cout << "Your round: ";
        int row = 0, col = 0;
        do {
            char row_c;
            cin >> row_c >> col;
            row = row_c - 'A';
            if (row < 0 || col < 0)
            {
                *is_over = true;
                return state;
            }
        }while(state.position[row][col] != EMPTY);

        State new_state(state);
        new_state.agent_id ^= (1 << 0);
        new_state.position[row][col] = new_state.agent_id;

        return new_state;
    }
}

int main(int argc, char * argv[])
{
  std::ifstream in(argv[1]);
  State state(15, 15, BLACK);
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 15; j++) {
      int val;
      in >> val;
      state.position[i][j] = val;
    }
  }
  //expand(state, 0, atoi(argv[2]));
  int max_depth = atoi(argv[2]);
  bool is_cpu = (atoi(argv[3]) == 0) ? true : false;
  bool is_over = false;
  for (int i = 0; !is_over && i < max_depth; i++) {
    state = run(state, is_cpu, &is_over);
    is_cpu = !is_cpu;
  }

  return 0;
}

