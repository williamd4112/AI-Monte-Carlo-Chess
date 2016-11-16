#include "fast_tss.h"
#include "policy.h"
#include "state.h"
#include "constants.h"
#include <iostream>
#include <queue>
#include <algorithm>
#include <fstream>

using namespace std;
using namespace mcts;


int main(int argc, char * argv[])
{
  int agent_id = (argv[2][0] == 'b') ? mcts::BLACK : mcts::WHITE;
  int w = atoi(argv[3]);
  int h = atoi(argv[4]);

  ifstream in(argv[1]);
  mcts::State state(w, h, agent_id);
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
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
  cout << state << endl;

  mcts::Tss tss(state);
  std::vector<mcts::threat_t> threats;
  tss.find_all_threats(state.position, threats, THREAT_LEVEL_3, THREAT_LEVEL_5, 12);
  sort(threats.begin(), threats.end(), greater<mcts::threat_t>());

  cout << "original" << endl;
  for (auto t : threats) {
    cout << t << endl;
  }

  std::vector<mcts::threat_t> seq;
  int min_depth = 0x7fffffff;
  std::cout << "filtered" << endl;
  for (auto t : threats) {
    if (t.final_winning && t.min_winning_depth <= min_depth) {
      cout << t << endl;
      min_depth = t.min_winning_depth;
      seq.push_back(t);
    }
  }
  std::cout << "-----" << endl;
  std::vector<threat_t> new_threats;
  find_critical_winning_seq(state, seq, new_threats);

  cout << "Critical " << new_threats.size() << endl;
  for (auto & t : new_threats) {
    cout << t << endl;
  }

  return 0;

}
