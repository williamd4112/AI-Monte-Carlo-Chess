#ifndef simulation_usual_h
#define simulation_usual_h

#include <vector>

#include "constants.h"

void calculate_each_win_result(char c, bool &is_tie,int &num_connect,char &result);

char who_win(std::vector<std::vector<char>> &map);

#endif /* simulation_usual_h */
