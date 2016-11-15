#include "fast_tss.h"
#include "state.h"
#include "constants.h"
#include <iostream>
#include <queue>
#include <algorithm>
#include <fstream>

using namespace std;

int main(int argc, char * argv[])
{
  int agent_id = (argv[2][0] == 'b') ? mcts::BLACK : mcts::WHITE;
  ifstream in(argv[1]);
  mcts::State state(15, 15, agent_id);
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 15; j++) {
      char c;
      in >> c;
      if (c == 'O')
        state.position[i][j] = mcts::BLACK;
      if (c == 'X')
        state.position[i][j] = mcts::WHITE;
      if (c == '.')
        state.position[i][j] = mcts::EMPTY;
    }
  }

  mcts::Tss tss(state);
  std::vector<mcts::threat_t> threats;
  tss.find_all_threats(state.position, threats, THREAT_LEVEL_3, THREAT_LEVEL_5, 255);
  sort(threats.begin(), threats.end(), greater<mcts::threat_t>());

  std::vector<mcts::threat_t> seq;
  int min_depth = 0x7fffffff;
  cout << state << endl;
  for (auto t : threats) {
    cout << t << endl;
  }
  std::cout << "filtered" << endl;
  for (auto t : threats) {
    if (t.final_winning && t.min_winning_depth <= min_depth) {
      cout << t << endl;
      min_depth = t.min_winning_depth;
    }
  }

  return 0;
}
