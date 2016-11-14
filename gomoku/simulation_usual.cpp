#include "simulation_usual.h"

void calculate_each_win_result(char c, bool &is_tie,int &num_connect,char &result)
{
  if(c==mcts::BLACK) {
    is_tie = false;
    if(num_connect<0) {
      num_connect=1;
    } else {
      num_connect++;
    }
    if(num_connect==mcts::NUMTOWIN) {
      result = mcts::BLACK;
    }
  } else if(c==mcts::WHITE) {
    is_tie = false;
    if(num_connect>0) {
      num_connect=-1;
    } else {
      num_connect--;
    }
    if(num_connect==mcts::NUMTOWIN*(-1)) {
      result = mcts::WHITE;
    }
  } else {
    num_connect=0;
  }
}

char who_win(std::vector<std::vector<char>> &map)
{
  // Return mcts::BLACK if mcts::BLACK wins
  // Return mcts::WHITE if mcts::WHITE wins
  bool is_tie = true;
  // Same row or column
  int num_row_connect = 0;
  int num_column_connect = 0;
  char result = -1;
  for(int i = 0; i<map.size(); i++) {
    num_row_connect=0;
    num_column_connect=0;
    for(int j = 0; j<map.size(); j++) {
      calculate_each_win_result(map[i][j], is_tie,num_row_connect,result);
      if(result!=-1) {
        return result;
      }
      calculate_each_win_result(map[j][i], is_tie,num_column_connect,result);
      if(result!=-1) {
        return result;
      }
    }
  }
  // Cross line
  for(int k=0; k<map.size()-mcts::NUMTOWIN+1; k++) {
    num_row_connect=0;
    num_column_connect=0;
    for(int i = k,j = 0; i<map.size() && j<map.size(); i++,j++) {
      calculate_each_win_result(map[i][j], is_tie,num_row_connect,result);
      if(result!=-1) {
        return result;
      }
      calculate_each_win_result(map[map.size()-i-1][j], is_tie,num_column_connect,result);
      if(result!=-1) {
        return result;
      }
    }
    num_row_connect=0;
    num_column_connect=0;
    for(int i = 0,j = k; i<map.size() && j<map.size(); i++,j++) {
      calculate_each_win_result(map[i][j], is_tie,num_row_connect,result);
      if(result!=-1) {
        return result;
      }
      calculate_each_win_result(map[i][map.size()-j-1], is_tie,num_column_connect,result);
      if(result!=-1) {
        return result;
      }
    }
  }
  if(is_tie==true) {
    return mcts::EMPTY;
  }

  return -1; // Return (-1) if no one wins
}
