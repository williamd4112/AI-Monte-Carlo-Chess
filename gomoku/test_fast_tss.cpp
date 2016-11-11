#include "fast_tss.h"
#include "state.h"
#include "constants.h"
#include <iostream>
#include <queue>

using namespace std;
using namespace mcts;

int main()
{
  State state(15, 15, BLACK);
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 15; j++) {
      int val;
      cin >> val;
      state.position[i][j] = val;
      cout << val;
    }
    cout << endl;
  }

  Tss tss(state);
  std::vector<Tss::threat_t> threats;
  tss.find_all_threats(state.position, threats, THREAT_LEVEL_3, THREAT_LEVEL_5, 255);

  return 0;
}
