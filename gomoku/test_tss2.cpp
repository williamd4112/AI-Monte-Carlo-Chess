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
  std::vector<Tss::point_t> points;
  tss.find_possible_points(points);

  for (Tss::point_t p : points) {
    int old = state.position[p.i][p.j];
    state.position[p.i][p.j] = BLACK;
    cout << state << endl;
    state.position[p.i][p.j] = old;
  }

  return 0;
}
