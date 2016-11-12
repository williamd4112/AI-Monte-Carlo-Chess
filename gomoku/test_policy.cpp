#include "fast_tss.h"
#include "policy.h"
#include "state.h"
#include "constants.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <queue>

using namespace std;
using namespace mcts;

int check_chain(const Position & position, int w, int h, int row, int col, int dr, int dc, int agent_id)
{
    int i = row;
    int j = col;
    int k = 0;
    for (; k < 5; k++) {
        if (i < 0 || i >= h || j < 0 || j >= w)
            break;
        if (position[i][j] != agent_id)
            break;
        i += dr;
        j += dc;
    }
    return k;
}

int check_win(const State & state)
{
    static int dirs[][2] = {
        {1, 0},
        {0, 1},
        {1, 1},
        {-1, 1}
    };

    for (int k = 0; k < 4; k++) {
        int skip = 0;
        for (int i = 0; i < state.board_height; i++) {
            for (int j = 0; j < state.board_width; j++) {
                if (skip > 0) {
                    skip--;
                }
                if (state.position[i][j] != EMPTY) {
                    int len = check_chain(state.position, state.board_height, state.board_width, i, j, dirs[k][0], dirs[k][1], state.position[i][j]);
                    if (len == 5)
                        return state.position[i][j];
                    else
                        skip = len - 1;
                }
            }
        }
    }

    return EMPTY;
}

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

void expand(const State & s, int depth, int max_depth)
{
  if (depth >= max_depth) return;
  cout << "Depth " << depth << " : Agent " << (int)s.agent_id << endl;
  cout << s << endl;
  int winner = EMPTY;
  if ((winner = check_win(s)) != EMPTY) {
      cout << "Agent " << winner << " win !" << endl;
      return;
  }
  std::vector<State> next_states;
  int res = policy_aggresive(s, next_states);
  if (res == POLICY_FAIL) {
      cout << "Tie !!" << endl;
      auto counting = count(s, s.board_width, s.board_height);
      printf("Black %d, White %d\n", counting.first, counting.second);
      return;
  }

#if 0
  for (State & next : next_states) {
    expand(next, depth + 1, max_depth);
  }
#else
  expand(next_states[rand() % next_states.size()], depth + 1, max_depth);
#endif
}

State run(const State & state, bool is_cpu, bool * is_over)
{
    int winner = EMPTY;
    cout << state << endl;
    if ((winner = check_win(state)) != EMPTY) {
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
  State state(15, 15, WHITE);
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 15; j++) {
      int val;
      in >> val;
      state.position[i][j] = val;
    }
  }
  //expand(state, 0, atoi(argv[2]));
  int max_depth = atoi(argv[2]);
  bool is_cpu = true;
  bool is_over = false;
  for (int i = 0; !is_over && i < max_depth; i++) {
    state = run(state, is_cpu, &is_over);
    is_cpu = !is_cpu;
  }

  return 0;
}

