#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <iostream>
#include <vector>

#include "board.h"
#include "constants.h"

#define NOT_END -1

#define in_boundary(i, j, w, h) (!((i < 0 || i >= h || j < 0 || j >= w)))

namespace mcts
{

typedef std::vector<std::vector<char> > Position;
struct point_t;

int util_check_chain(const Position & position, int w, int h, int row, int col, int dr, int dc, int agent_id);
int util_check_win(const Position & position, int w, int h);
void print_position(std::ostream& strm, const Position& position);
void load_position_from(std::istream & in, Position & position, int w, int h);
void find_position_diff(const Position& before, const Position& after,
                        point_t& point);
void get_position_size(const Position& position, point_t& pos_size);

}

#endif // UTIL_H_INCLUDED
