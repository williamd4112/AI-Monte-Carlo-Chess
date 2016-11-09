#include "tss.h"
#include "state.h"
#include "constants.h"
#include <iostream>

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
  std::vector<Tss::threat_t> & threats = tss.find_all_threats();
  for (Tss::threat_t threat : threats) {
    Tss::point_t & p = threat.point;
    int old = state.position[p.i][p.j];
    state.position[p.i][p.j] = BLACK;
    cout << "(" << p.i << ", " << p.j << ")" << " = " << threat.gain << endl;
    cout << state << endl;
    state.position[p.i][p.j] = old;
  }

  return 0;
}
