#include "fast_tss.h"
#include "policy.h"
#include "state.h"
#include "constants.h"
#include <iostream>
#include <queue>

using namespace std;
using namespace mcts;

void expand(const State & s, int depth)
{
  if (depth >= 3) return;
  cout << "Depth " << depth << " : Agent " << (int)s.agent_id << endl;
  cout << s << endl;
  std::vector<State> next_states;
  policy_default(s, next_states);

  for (State & next : next_states) {
    expand(next, depth + 1);
  }
}

int main()
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
  expand(state, 0);

  return 0;
}
