#include <iostream>
#include <vector>

#include "mcts.h"

int main()
{
  const unsigned kMaxDuration = 1000; // In milliseconds
  const unsigned kMaxIterationCount = 1;
  const double kExplore = 1.41;
  const bool kVerbose = true;
  mcts::Timer timer(kMaxDuration, kMaxIterationCount);
  mcts::MCTS mcts(&timer, kExplore, kVerbose);
  mcts::State root_state(15, 15, 0);
  mcts::State result_state(15, 15, 0);

  mcts.run(root_state, result_state);

  std::cout << result_state << '\n';

  return 0;
}
