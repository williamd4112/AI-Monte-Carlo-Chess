#ifndef TEST_BASE_H_INCLUDED
#define TEST_BASE_H_INCLUDED

#include <cctype>

#define CATCH_CONFIG_MAIN

#include "../lib/catch.hpp"
#include "../board.h"

typedef std::vector<std::string> StrPosition;

void str_2_position(const StrPosition& str, mcts::Position& position)
{
  for (size_t row = 0; row < str.size(); ++row) {
    const std::string& row_str = str[row];
    for (size_t col = 0; col < row_str.length(); ++col) {
      const char ch = tolower(row_str[col]);
      char agent_id;
      if (ch == '.') {
        agent_id = mcts::EMPTY;
      } else if (ch == 'o') {
        agent_id = mcts::BLACK;
      } else if (ch == 'x') {
        agent_id = mcts::WHITE;
      } else {
        agent_id = ch;
      }
      position[row][col] = agent_id;
    }
  }
}

#endif
