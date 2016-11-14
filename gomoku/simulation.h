#ifndef simulation_h
#define simulation_h

#include <cstdlib>
#include <ctime>
#include <vector>

#include "constants.h"

bool is_open_end(std::vector<std::vector<char>> &map,int i, int j,bool requirement);
bool is_row_line(std::vector<std::vector<char>> &map,std::pair<int, int> &this_step,char char_match,int target,bool open_end);
bool is_column_line(std::vector<std::vector<char>> &map,std::pair<int, int> &this_step,char char_match,int target,bool open_end);
bool is_cross1_line(std::vector<std::vector<char>> &map,std::pair<int, int> &this_step,char char_match,int target,bool open_end);
bool is_cross2_line(std::vector<std::vector<char>> &map,std::pair<int, int> &this_step,char char_match,int target,bool open_end);
bool random_output(std::vector<std::vector<char>> &map,std::vector<std::pair<int, int>> &a,char c);
bool four_direction_check(std::vector<std::vector<char>> &map,std::pair<int,int> &temp_pair, int color_match, int target, bool requireopen_end);
std::vector<std::pair<int,int>> next_to_play_each_line(std::vector<std::vector<char>> &map,char color,bool requireopen_end,int target);
// return to interrupt
int next_to_play(std::vector<std::vector<char>> &map,char &color);
#endif /* simulation_h */
