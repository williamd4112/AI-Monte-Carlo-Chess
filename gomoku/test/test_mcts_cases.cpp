#include "test_base.h"

#include "../board.h"
#include "../mcts.h"
#include "../timer.h"
#include "../util.h"

const int kMaxDuration = 1000;
const int kMaxIterationCount = 1000;
const int kExplore = 1.41;
const bool kVerbose = false;

void validate(const char& turn, const StrPosition& str_board,
              const StrPosition& expected_place);

TEST_CASE("MCTS cases", "[mcts]")
{
  SECTION("Case 1: black should place centered stone") {
    const char turn = mcts::BLACK;
    const StrPosition str_board {
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
    };
    const StrPosition expected_places {
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      ".......o.......",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
    };
    validate(turn, str_board, expected_places);
  }

  SECTION("Case 2: white should place around the centered black stone") {
    const char turn = mcts::WHITE;
    const StrPosition str_board {
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      ".......o.......",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
    };
    const StrPosition expected_places {
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "......xxx......",
      "......x.x......",
      "......xxx......",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
    };
    validate(turn, str_board, expected_places);
  }

  SECTION("Case 3: black should create winning sequence") {
    const char turn = mcts::BLACK;
    const StrPosition str_board {
      ".........",
      "...ooo...",
      "...xxx...",
      ".........",
      ".........",
      ".........",
      ".........",
      ".........",
      ".........",
    };
    const StrPosition expected_places {
      ".........",
      "..o...o..",
      ".........",
      ".........",
      ".........",
      ".........",
      ".........",
      ".........",
      ".........",
    };
    validate(turn, str_board, expected_places);
  }

  SECTION("Case 4: white should close opponent's four") {
    const char turn = mcts::WHITE;
    const StrPosition str_board {
      ".........",
      "..xoooo..",
      "...xxx...",
      ".........",
      ".........",
      ".........",
      ".........",
      ".........",
      ".........",
    };
    const StrPosition expected_places {
      ".........",
      ".......x.",
      ".........",
      ".........",
      ".........",
      ".........",
      ".........",
      ".........",
      ".........",
    };
    validate(turn, str_board, expected_places);
  }

  SECTION("Case 5: white should win") {
    const char turn = mcts::WHITE;
    const StrPosition str_board {
      ".........",
      "..xoooo..",
      "..oxxxx..",
      ".........",
      ".........",
      ".........",
      ".........",
      ".........",
      ".........",
    };
    const StrPosition expected_places {
      ".........",
      ".........",
      ".......x.",
      ".........",
      ".........",
      ".........",
      ".........",
      ".........",
      ".........",
    };
    validate(turn, str_board, expected_places);
  }

  SECTION("Case 6: black should block opponent") {
    const char turn = mcts::BLACK;
    const StrPosition str_board {
      "...............",
      "...............",
      "...............",
      "...............",
      "......x........",
      "......x........",
      "......x........",
      ".......ooox....",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
    };
    const StrPosition expected_places {
      "...............",
      "...............",
      "...............",
      "......o........",
      "...............",
      "...............",
      "...............",
      "......o........",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
    };
    validate(turn, str_board, expected_places);
  }

  SECTION("Case 7: black should win") {
    const char turn = mcts::BLACK;
    const StrPosition str_board {
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "..x...o.o......",
      "...o..xx.......",
      "...ooxoox.x....",
      "...o.ooxx......",
      "...oxooox......",
      "...xx.xxo......",
      ".......x.x.....",
      "...............",
      "...............",
      "...............",
    };
    const StrPosition expected_places {
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...o...........",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
    };
    validate(turn, str_board, expected_places);
  }

  SECTION("Case 8: black should create winning sequence") {
    const char turn = mcts::BLACK;
    const StrPosition str_board {
      "...............",
      "...............",  
      "...............",
      "...............",
      "...............",
      "........o......",
      ".......x.......",
      "...ooxoox.x....",
      ".....ooxx......",
      "...oxooox......",
      "....x.xxo......",
      ".......x.x.....",
      "...............",
      "...............",
      "...............",
    };
    const StrPosition expected_places {
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...o...........",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
    };
    validate(turn, str_board, expected_places);
  }

  SECTION("Case 9: black should create winning sequence") {
    const char turn = mcts::BLACK;
    const StrPosition str_board {
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "........o......",
      "......x........",
      "...ooxoox.x....",
      ".....ooxx......",
      "......oox......",
      "......xxo......",
      ".......x.x.....",
      "...............",
      "...............",
      "...............",
    };
    const StrPosition expected_places {
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      ".......o.......",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
    };
    validate(turn, str_board, expected_places);
  }

  SECTION("Case 10: black should struggle to defend") {
    const char turn = mcts::BLACK;
    const StrPosition str_board {
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "........x......",
      ".......oo......",
      "...xxoxxo.x....",
      ".....xxoo......",
      ".....xxxo......",
      "...x..oox......",
      ".......o.o.....",
      "...............",
      "...............",
      "...............",
    };
    const StrPosition expected_places {
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "....o..........",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
    };
    validate(turn, str_board, expected_places);
  }
  
  SECTION("Case 11: black should create winning sequence first") {
    const char turn = mcts::BLACK;
    const StrPosition str_board {
      ".x.ox..oo......",
      "ooo............",
      "..xoo..........",
      "...............",
      "..o............",
      ".x.............",
      "...............",
      "..x............",
      ".x.x...........",
      "..x............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
    };
    const StrPosition expected_places {
      "......o........",
      "...o...........",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
    };
    validate(turn, str_board, expected_places);
  }

  SECTION("Case 12: black should block white") {
    const char turn = mcts::BLACK;
    const StrPosition str_board {
      "...............",
      "...............",
      "...............",
      "...............",
      "..........X....",
      ".........O.....",
      "....X.X.O......",
      ".....XXO.......",
      "....OXO........",
      ".....XO........",
      ".....O.........",
      "...............",
      "...............",
      "...............",
      "..............."
    };
    const StrPosition expected_places {
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      ".....O.........",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
    };
    validate(turn, str_board, expected_places);
  }

  SECTION("Case 13: black should attack") {
    const char turn = mcts::BLACK;
    const StrPosition str_board {
      "...............",
      "...............",
      "...............",
      "...............",
      "..........X....",
      ".........O.....",
      "......X.O......",
      ".....XXO.......",
      "....OXO........",
      ".....XO........",
      ".....O.........",
      "...............",
      "...............",
      "...............",
      "..............."
    };
    const StrPosition expected_places {
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      ".......o.......",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
    };
    validate(turn, str_board, expected_places);
  }

  SECTION("Case 14: black should attack instead of defense") {
    const char turn = mcts::BLACK;
    const StrPosition str_board {
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "......o.o......",
      "......xo.......",
      "......xx.......",
      "...............",
      "...............",
      "...............",
      "...............",
      "...............",
      "..............."
    };
    const StrPosition expected_places {
      "...............",
      "...............",
      "...............",
      "...............",
      "....o..........",
      ".....o.........",
      ".....o.o.o.....",
      "...............",
      "........o......",
      ".........o.....",
      "...............",
      "...............",
      "...............",
      "...............",
      "..............."
    };
    validate(turn, str_board, expected_places);
  }
}

void validate(const char& turn, const StrPosition& str_board,
              const StrPosition& expected_places)
{
  using namespace mcts;
  const int height = str_board.size();
  const int width = str_board[0].length();
  Position pos_board = Position(height, Row(width, EMPTY));
  Position pos_expected = Position(height, Row(width, EMPTY));
  str_2_position(str_board, pos_board);
  str_2_position(expected_places, pos_expected);

  Timer timer(kMaxDuration, kMaxIterationCount);
  MCTS mcts(&timer, kExplore, kVerbose);
  State root_state(height, width, pos_board, !turn);
  State result_state(height, width, EMPTY);
  mcts.run(root_state, result_state);
  point_t point;
  find_position_diff(pos_board, result_state.position, point);
  if (pos_expected[point.j][point.i] != turn) {
    std::cerr << "AI placed (" << point.j << ", " << point.i << ")" << '\n';
  }
  REQUIRE(pos_expected[point.j][point.i] == turn);
}
