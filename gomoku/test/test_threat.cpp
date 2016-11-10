#include "test_base.h"

#include <string>
#include <vector>

#include "../constants.h"
#include "../state.h"
#include "../tss.h"

using namespace mcts;

typedef std::vector<std::string> StrPosition;
typedef std::vector<Tss::threat_t> Threats;
typedef std::vector<char> Row;

void str_2_position(const StrPosition& str, State::Position& position);
void gains_2_position(const StrPosition& str, State::Position& position);
void threats_2_gains(const Threats& threats, State::Position& position);
void validate(const StrPosition& str_board, const StrPosition& exp_gains,
              int start_threat, int end_threat);

const unsigned kHeight = 5;
const unsigned kWidth = 5;

TEST_CASE("threat 5x5", "[mcts]")
{
  SECTION("Empty board") {
    StrPosition str_board {
      ".....",
      ".....",
      ".....",
      ".....",
      "....."
    };
    StrPosition expected_gains {
      "00000",
      "00000",
      "00000",
      "00000",
      "00000"
    };
    validate(str_board, expected_gains, THREAT_LEVEL_3, THREAT_LEVEL_5);
  }

  SECTION("Single stone") {
    StrPosition str_board {
      ".....",
      ".....",
      "..o..",
      ".....",
      "....."
    };
    StrPosition expected_gains {
      "00000",
      "01110",
      "01010",
      "01110",
      "00000"
    };
    validate(str_board, expected_gains, THREAT_LEVEL_2, THREAT_LEVEL_2);
  }

  SECTION("Diagnoal 3") {
    StrPosition str_board {
      ".....",
      ".oxo.",
      ".xox.",
      ".oxo.",
      "....."
    };
    StrPosition expected_gains {
      "10001",
      "00000",
      "00000",
      "00000",
      "10001"
    };
    validate(str_board, expected_gains, THREAT_LEVEL_3, THREAT_LEVEL_5);
  }
}

void validate(const StrPosition& str_board, const StrPosition& exp_gains,
              int start_threat, int end_threat)
{
  State::Position pos_board = State::Position(kHeight, Row(kWidth, EMPTY));
  State::Position pos_exp_gains = State::Position(kHeight, Row(kWidth, EMPTY));
  State::Position pos_gains = State::Position(kHeight, Row(kWidth, 0));
  str_2_position(str_board, pos_board);
  gains_2_position(exp_gains, pos_exp_gains);
  State state(kHeight, kWidth, pos_board, BLACK);
  Tss tss(state);
  Threats threats;
  tss.find_all_threats(threats, start_threat, end_threat);
  threats_2_gains(threats, pos_gains);
  if (pos_gains != pos_exp_gains) {
    print_position_num(std::cout, pos_gains);
  }
  REQUIRE(pos_gains == pos_exp_gains);
}

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
        agent_id = ch;
      }
      position[row][col] = agent_id;
    }
  }
}

void gains_2_position(const StrPosition& str, State::Position& position)
{
  for (size_t row = 0; row < str.size(); ++row) {
    const std::string& row_str = str[row];
    for (size_t col = 0; col < row_str.length(); ++col) {
      const char& ch = row_str[col];
      char agent_id = (ch - '0');
      position[row][col] = agent_id;
    }
  }
}

void threats_2_gains(const Threats& threats, State::Position& position)
{
  for (const auto& threat : threats) {
    const auto& point = threat.point;
    char gain = threat.gain;
    position[point.i][point.j] = gain;
  }
}
