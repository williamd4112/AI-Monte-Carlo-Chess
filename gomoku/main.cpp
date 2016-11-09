#include <cctype>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

#include "constants.h"
#include "mcts.h"
#include "simulation_usual.h"

bool check_ai_play(int mode, char& turn, mcts::State::Position& position);
void play_by_ai(char turn, mcts::State::Position& position);
bool check_game_finished(mcts::State::Position& position);
void print_status(int mode, char turn, int move_num);
void split_string(const std::string& input, std::vector<std::string>& tokens);
void check_arg_size(const std::vector<std::string>& args, size_t req_size);

const int kModeAiWhite = 1;
const int kModeAiBlack = 2;
const int kModeHumanAll = 3;
const int kModeAiAll = 4;

const int kHeight = 15;
const int kWidth = 15;
const unsigned kMaxDuration = 1000; // In milliseconds
const unsigned kMaxIterationCount = 10000;
const double kExplore = 1.41;
const bool kVerbose = true;

int main()
{
  std::string input;
  // Play mode
  int mode = kModeAiWhite;
  // Play count
  int move_num = 0;
  // Player turn
  char turn = mcts::BLACK;
  // Game status
  bool is_game_finished = false;
  // Position
  mcts::Position position(kHeight, mcts::State::Row(kWidth, mcts::EMPTY));

  std::vector<std::string> tokens;
  std::cout << "Enter commands (Enter 'help' to see list of commands)" << '\n';
  std::cout << "> ";
  while (std::getline(std::cin, input)) {
    /* Parse commands */
    split_string(input, tokens);
    if (!tokens.empty()) {
      std::string command = tokens[0];
      if (command == "n" || command == "new") {
        move_num = 0;
        turn = mcts::BLACK;
        is_game_finished = false;
        position = mcts::Position(kHeight,
                                  mcts::State::Row(kWidth, mcts::EMPTY));
        std::cout << "Started new position" << '\n';
      } else if (command == "q" || command == "quit") {
        break;
      } else if (command == "m" || command == "move") {
        if (!is_game_finished) {
          check_arg_size(tokens, 2);
          std::string str_pt = tokens[1];
          /* Place the stone and switch player */
          if (str_pt.length() >= 2) {
            int row = (toupper(str_pt[0]) - 'A');
            int col = std::atoi(str_pt.substr(1, str_pt.length() - 1).c_str());
            if (!((row >= 0 && row < kHeight) &&
                  (col >= 0 && col < kWidth))) {
              std::cerr << "Out of range" << '\n';
            } else if (position[row][col] != mcts::EMPTY) {
              std::cerr << "The point has been occupied" << '\n';
            } else {
              position[row][col] = turn;
              move_num += 1;
              turn = !turn;
              if (mode == kModeHumanAll) {
                mcts::print_position(std::cout, position);
              }
              print_status(mode, turn, move_num);
              if (check_game_finished(position)) {
                is_game_finished = true;
              }
            }
          } else {
            std::cerr << "Invalid point " << str_pt << '\n';
          }
        } else {
          std::cerr << "The game has finished" << '\n';
        }
      } else if (command == "mode") {
        check_arg_size(tokens, 2);
        std::string str_mode = tokens[1];
        if (str_mode == "b" || str_mode == "ai_black") {
          mode = kModeAiBlack;
          std::cout << "Switched mode to AI playing black" << '\n';
        } else if (str_mode == "w" || str_mode == "ai_white") {
          mode = kModeAiWhite;
          std::cout << "Switched mode to AI playing white" << '\n';
        } else if (str_mode == "h" || str_mode == "human_all") {
          mode = kModeHumanAll;
          std::cout << "Switched mode to human players" << '\n';
        } else if (str_mode == "a" || str_mode == "ai_all") {
          mode = kModeAiAll;
          std::cout << "Switched mode to AI players" << '\n';
        } else {
          std::cerr << "Unknown mode " << str_mode << '\n';
        }
      } else if (command == "s" || command == "status") {
        mcts::print_position(std::cout, position);
        print_status(mode, turn, move_num);
      } else if (command == "help") {
        std::cout << "(n|new): create new position" << '\n';
        std::cout << "(q|quit): quit the program" << '\n';
        std::cout << "(m|move) <pt>: make a move by placing a stone on PT"
                  << '\n';
        std::cout << "mode <mode>: swtich mode to MODE, available modes:"
                  << '\n'
                  << "  (b|ai_black): AI playing black" << '\n'
                  << "  (w|ai_white): AI playing white" << '\n'
                  << "  (h|human_all): Human players" << '\n'
                  << "  (a|ai_all): AI players" << '\n';
        std::cout << "(s|status): prints current status" << '\n';
      } else {
        std::cerr << "Unknown command " << command << '\n';
        std::cerr << "Enter 'help' to see list of commands" << '\n';
      }
    }
    if (!is_game_finished &&
        check_ai_play(mode, turn, position)) {
      move_num += 1;
      mcts::print_position(std::cout, position);
      std::cout << '\n';
      print_status(mode, turn, move_num);
      if (check_game_finished(position)) {
        is_game_finished = true;
      }
    }
    std::cout << "> ";
  }

  return 0;
}

bool check_ai_play(int mode, char& turn, mcts::State::Position& position)
{
  if ((mode == kModeAiBlack && turn == mcts::BLACK) ||
      (mode == kModeAiWhite && turn == mcts::WHITE) ||
      (mode == kModeAiAll)) {
    play_by_ai(turn, position);
    turn = !turn;
    return true;
  } else {
    return false;
  }
}

void play_by_ai(char turn, mcts::State::Position& position)
{
  mcts::Timer timer(kMaxDuration, kMaxIterationCount);
  mcts::MCTS mcts(&timer, kExplore, kVerbose);
  mcts::State root_state(kHeight, kWidth, position, turn);
  mcts::State result_state(kHeight, kWidth, mcts::EMPTY);
  mcts.run(root_state, result_state);
  // Print the stone AI placed
  bool has_found = false;
  for (unsigned row = 0; row < kHeight; ++row) {
    for (unsigned col = 0; col < kWidth; ++col) {
      if (position[row][col] != result_state.position[row][col]) {
        std::cout << "AI placed " << (char)('A' + row) << col << '\n';
        has_found = true;
        break;
      }
    }
    if (has_found) {
      break;
    }
  }
  position = result_state.position;
}

bool check_game_finished(mcts::State::Position& position)
{
  char winner = who_win(position);
  if (winner == -1) {
    return false;
  } else {
    mcts::print_position(std::cout, position);
    std::cout << '\n';
    if (winner == mcts::BLACK) {
      std::cout << "Game finished: black wins" << '\n';
    } else if (winner == mcts::WHITE) {
      std::cout << "Game finished: white wins" << '\n';
    } else if (winner == mcts::EMPTY) {
      std::cout << "Game finished: tie" << '\n';
    }
    return true;
  }
}

void print_status(int mode, char turn, int move_num)
{
  std::cout << "Mode: ";
  if (mode == kModeAiBlack) {
    std::cout << "ai_black";
  } else if (mode == kModeAiWhite) {
    std::cout << "ai_white";
  } else if (mode == kModeHumanAll) {
    std::cout << "human_all";
  } else if (mode == kModeAiAll) {
    std::cout << "ai_all";
  } else {
    std::cout << "unknown";
  }
  std::cout << ", Turn: ";
  if (turn == mcts::BLACK) {
    std::cout << "black";
  } else if (turn == mcts::WHITE) {
    std::cout << "white";
  } else {
    std::cout << "unknown";
  }
  std::cout << ", Move#: ";
  std::cout << (move_num + 1);
  std::cout << " (Next)";
  std::cout << '\n';
}

void split_string(const std::string& input, std::vector<std::string>& tokens)
{
  std::stringstream ss(input);
  std::string buf;
  tokens.clear();
  while (ss >> buf) {
    tokens.push_back(buf);
  }
}

void check_arg_size(const std::vector<std::string>& args, size_t req_size)
{
  if (args.size() < req_size) {
    std::cerr << "Require " << req_size << " arguments" << '\n';
  }
}
