#include "fast_tss.h"
#include "state.h"
#include "constants.h"
#include <iostream>
#include <queue>
#include <algorithm>

using namespace std;

int main()
{
  mcts::State state(15, 15, mcts::BLACK);
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 15; j++) {
      int val;
      cin >> val;
      state.position[i][j] = val;
      cout << val;
    }
    cout << endl;
  }

  mcts::Tss tss(state);
  std::vector<mcts::threat_t> threats;
  tss.find_all_threats(state.position, threats, THREAT_LEVEL_3, THREAT_LEVEL_5, 255);
  sort(threats.begin(), threats.end(), greater<mcts::threat_t>());
  for (auto t : threats) {
    t.print();
  }

  return 0;
}
