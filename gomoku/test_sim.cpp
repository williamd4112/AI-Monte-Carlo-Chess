#include "fast_tss.h"
#include "policy.h"
#include "state.h"
#include "constants.h"
#include "util.h"
#include "sim.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <queue>

#ifdef _UNIX
#include <unistd.h>
#endif

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

int main(int argc, char * argv[])
{
  if (argc < 5) {
    cout << "./test_sim <state file> <simulation iter> <max iter in one playout> <max random sampling times>" << endl;
    return -1;
  }
#ifdef _UNIX
#ifdef _PERF
  printf("pid = %d\n", getpid());
  sleep(5);
#endif
#endif
  std::ifstream in(argv[1]);
  State state(15, 15, WHITE);
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 15; j++) {
      int val;
      in >> val;
      state.position[i][j] = val;
    }
  }

  Policy policy(15, 15);
  int cnt[2] = { 0 };
  for (int iter = 0; iter < atoi(argv[2]); iter++) {
    int res = sim_rapid_until_end(policy, state, atoi(argv[3]), atoi(argv[4]));
    if (res == BLACK) cnt[0]++;
    if (res == WHITE) cnt[1]++;
  }
  printf("%d / %d\n", cnt[0], cnt[1]);
  return 0;
}

