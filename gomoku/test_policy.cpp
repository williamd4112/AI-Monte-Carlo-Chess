#include "fast_tss.h"
#include "policy.h"
#include "state.h"
#include "constants.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <queue>

using namespace std;
using namespace mcts;

int check_win(const State & state)
{
    return EMPTY;
}

void expand(const State & s, int depth, int max_depth)
{
  if (depth >= max_depth) return;
  cout << "Depth " << depth << " : Agent " << (int)s.agent_id << endl;
  cout << s << endl;
  std::vector<State> next_states;
  policy_aggresive(s, next_states);

#if 0
  for (State & next : next_states) {
    expand(next, depth + 1, max_depth);
  }
#else
  expand(next_states[rand() % next_states.size()], depth + 1, max_depth);
#endif
}

int main(int argc, char * argv[])
{
  State state(15, 15, WHITE);
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 15; j++) {
      int val;
      cin >> val;
      state.position[i][j] = val;
      cout << val;
    }
    cout << endl;
  }
  expand(state, 0, atoi(argv[1]));

  return 0;
}
