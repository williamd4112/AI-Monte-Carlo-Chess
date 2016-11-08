#include "test_base.h"

#include <stdexcept>
#include <string>
#include <vector>

#include "../constants.h"
#include "../state.h"
#include "../tss.h"

using namespace mcts;

typedef std::vector<std::string> StrPosition;
typedef std::vector<Tss::threat_t> Threats;
typedef std::vector<char> Row;

void str_2_position(const StrPosition& str, State::Position& position)
{
  for (size_t row = 0; row < str.size(); ++row) {
    const std::string& row_str = str[row];
    for (size_t col = 0; col < row_str.length(); ++col) {
      const char& ch = row_str[col];
      char agent_id;
      if (ch == '.') {
        agent_id = EMPTY;
      } else if (ch == 'o') {
        agent_id = BLACK;
      } else if (ch == 'x') {
        agent_id = WHITE;
      } else {
        throw std::runtime_error(std::string("Unknown value ") +
                                 std::string(1, ch));
      }
      position[row][col] = agent_id;
    }
  }
}

void threats_2_gains(const Threats& threats, State::Position& position)
{
  for (const auto& threat : threats) {
    const auto& gains = threat.gains;
    for (const auto& point : gains) {
      position[point.i][point.j] = 1;
    }
  }
}

void threats_2_costs(const Threats& threats, State::Position& position)
{
  for (const auto& threat : threats) {
    const auto& costs = threat.costs;
    for (const auto& point : costs) {
      position[point.i][point.j] = 1;
    }
  }
}

TEST_CASE("threat", "[mcts]")
{
  SECTION("Empty board")
  {
    const unsigned kHeight = 5;
    const unsigned kWidth = 5;
    State::Position pos_board = State::Position(kHeight, Row(kWidth, EMPTY));
    State::Position pos_gains = State::Position(kHeight, Row(kWidth, EMPTY));
    StrPosition str_board {
      ".....",
      ".....",
      ".....",
      ".....",
      "....."
    };
    StrPosition str_gains {
      ".....",
      ".....",
      ".....",
      ".....",
      "....."
    };
    str_2_position(str_board, pos_board);
    State state(kHeight, kWidth, pos_board, BLACK);
    Tss tss(state);
    tss.find_all_threats();
    Threats threats = tss.get_threats();
    threats_2_gains(threats, pos_gains);
    State state_gains(kHeight, kWidth, pos_gains, BLACK);
    std::cout << state_gains;
  }
}
