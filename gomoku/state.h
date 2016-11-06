#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include <stdexcept>

#include "constants.h"

namespace mcts
{

class State
{
public:
  std::vector<std::vector<char> > position;
  char agent_id;

  State(int board_height, int board_width, char agent_id):
    agent_id(agent_id), m_board_height(board_height), m_board_width(board_width)
  {
    position = std::vector<std::vector<char>>(board_height,
               std::vector<char>(board_width, EMPTY));
  }

  State(const State& other):
    position(other.position),
    agent_id(other.agent_id)
  {
  }

  char get_agent_id() const
  {
    return agent_id;
  }

  void get_expanded_states(std::vector<State> &expanded_states) const
  {
    ///TODO Implement expansion
    
    /* Has been visited during expansion (to avoid duplicated state) */
    std::vector<std::vector<bool>> visited = std::vector<std::vector<bool>>(m_board_height,
               std::vector<bool>(m_board_width, false));

    expanded_states.clear();
    for (int i = 0; i < m_board_height; i++) {
      for (int j = 0; j < m_board_width; j++) {
         if (position[i][j] != EMPTY) {
            expand_around(expanded_states, visited, i, j);
         }
      }
    }

    /* If no chess on the board, set in middle */
    if (expanded_states.empty()) {
      State new_state(*this);
      new_state.position[m_board_height / 2][m_board_width / 2] = agent_id;
      expanded_states.push_back(new_state);
    }
  }

  void simulate(std::vector<double> &payoffs) const
  {
    double WIN = 1.0;
    double LOSE = 0.0;
    double TIE = (WIN+LOSE)/2;
    payoffs[BLACK] = 1.0;
    payoffs[WHITE] = 1.0;
    vector<vector<char>> Map = position;
    
    char color = agent_id;
    while(true){
        char result = whoWin(Map);
        if(result == TIE){ // Tie is extremely unusual
            payoffs[BLACK] = TIE;
            payoffs[WHITE] = TIE;
            cout << "No one wins." << endl;
            break;
        }
        else if(result == BLACK){
            payoffs[BLACK] = WIN;
            payoffs[WHITE] = LOSE;
            cout << "BLACK wins!" << endl;
            break;
        }
        else if(result == WHITE){
            payoffs[BLACK] = WIN;
            payoffs[WHITE] = LOSE;
            cout << "WHITE wins!" << endl;
            break;
        }
        else {
            showMap(Map);
            nextToPlay(Map,color);
            color = (color==BLACK)? WHITE: BLACK;
        }
    }
  }
private:
   int m_board_height;
   int m_board_width;

   void expand_around(std::vector<State> & expanded_states, std::vector<std::vector<bool>> & visited, int row, int col) const
   {
      for (int i = row - EXPAND_AROUND_RANGE; i < row + EXPAND_AROUND_RANGE; i++) {
         for (int j = col - EXPAND_AROUND_RANGE; j < col + EXPAND_AROUND_RANGE; j++) {
            if (i < 0 || i >= m_board_height || j < 0 || j >= m_board_width) {
               continue;
            }
            
            if (position[i][j] == EMPTY && !visited[i][j]) {
               State new_state(*this);
               new_state.position[i][j] = agent_id;
               expanded_states.push_back(new_state);
               visited[i][j] = true;
            } 
         }
      }      
   }
};
  
  void showMap(vector<vector<char>> &Map) const
  {
    cout << "Map" << endl << "  ";
    char c='A';
    for(int i=0;i<10;i++){
        cout << ' ' << i << ' ';
    } for(int i=10;i<SIZE;i++){
        cout << i << ' ';
    } cout << endl;
    for(int i=0;i<SIZE;i++,c++){
        cout << c << ' ';
        for(int j=0;j<SIZE;j++){
            if(Map[i][j]!=WHITE && Map[i][j]!=BLACK)  cout << ' ' << '+' << ' ';
            else    cout << ' ' << Map[i][j] << ' ';
        }
        cout << endl;
    }
}

bool isLine(vector<vector<char>> &Map,pair<int, int> &thisStep,char charMatch,int &target,bool openEnd) const
{
    int numConnect=0;
    // Same row
    int j_min=max(0,thisStep.second+(-1)*target);
    int j_max=min(SIZE,thisStep.second+target);
    for(int j = j_min;j<j_max;j++){
        if(Map[thisStep.first][j]==charMatch || j==thisStep.second)
            numConnect++;
        else{ numConnect=0; }
        
        if(numConnect==target) return true;
    }
    // Same column
    int i_min=max(0,thisStep.first+(-1)*target);
    int i_max=min(SIZE,thisStep.first+target);
    for(int i=i_min;i<i_max;i++){
        if(Map[i][thisStep.second]==charMatch || i==thisStep.second)
            numConnect++;
        else{ numConnect=0; }
        
        if(numConnect==target) return true;
    }
    // Cross line
    for(int i=i_min,j=j_min;j<j_max && i<i_max;i++,j++){
        if(Map[i][j]==charMatch || (i==thisStep.first && j==thisStep.second))
            numConnect++;
        else{ numConnect=0; }
        
        if(numConnect==target) return true;
    }
    for(int j=j_min,i=i_max-1;j<j_max && i>=i_min;j++,i--){
        if(Map[i][j]==charMatch || (i==thisStep.first && j==thisStep.second))
            numConnect++;
        else{ numConnect=0;}
        
        if(numConnect==target) return true;
    }
    return false;
}

bool randomOutput(vector<vector<char>> &Map,vector<pair<int, int>> a, vector<pair<int, int>> b,char c) const
{
    if(a.size()>0){
        pair<int,int> temp = a[rand()%a.size()];
        Map[temp.first][temp.second] = c;
        showMap(Map);
        return true;
    }
    if(b.size()>0){
        pair<int,int> temp = b[rand()%b.size()];
        Map[temp.first][temp.second] = c;
        showMap(Map);
        return true;
    }
    return false;
}
int nextToPlayLine(vector<vector<char>> &Map,char &color, vector<bool> requireOpenEnd, vector<int> target) const
{
    bool bNext = false;
    bool wNext = false;
    vector<pair<int, int>> nextPosition,secondNextPosition;
    for(int i=0;i<SIZE;i++){
        for(int j=0 ; j<SIZE; j++,bNext = false,wNext = false){
            pair<int,int> temp_pair = make_pair(i,j);
            // if occupied, then continue
            if(Map[i][j]==BLACK||Map[i][j]==WHITE) continue;
            // check the position qualification
            int sumBTarget = 0;
            int sumWTarget = 0;
            for(int iTarget=0;iTarget<target.size();iTarget++){
                if(isLine(Map,temp_pair, BLACK, target[iTarget],requireOpenEnd[iTarget])){
                    sumBTarget++;
                }
                if(isLine(Map,temp_pair, WHITE, target[iTarget],requireOpenEnd[iTarget])){
                    sumWTarget++;
                }
            }
            if(sumBTarget>=target.size()) bNext=true;
            if(sumWTarget>=target.size()) wNext=true;
            
            //if(bNext) cout << "bNext Map[" << i << "][" << j << "]" << endl;
            //if(wNext) cout << "wNext Map[" << i << "][" << j << "]" << endl;
            if(color==BLACK){
                if(bNext) nextPosition.push_back(temp_pair);
                else if(wNext) secondNextPosition.push_back(temp_pair);
            }else{
                if(wNext) nextPosition.push_back(temp_pair);
                else if(bNext) secondNextPosition.push_back(temp_pair);
            }
        }
    }
    if(randomOutput(Map,nextPosition,secondNextPosition,color)) return 1;
    nextPosition.clear();
    secondNextPosition.clear();
    return 0;
}

// return to interrupt
int nextToPlay(vector<vector<char>> &Map,char &color) const
{
    vector<bool> requireOpenEnd;
    vector<int> target;
    // five-in-line
    //cout << "five-in-line" << endl;
    requireOpenEnd.push_back(false);
    target.push_back((int)NUMTOWIN-1);
    if(nextToPlayLine(Map,color, requireOpenEnd, target)>0) return 0;
    requireOpenEnd.clear();
    target.clear();
    // four-in-line-with-open-end
    requireOpenEnd.push_back(true);
    target.push_back(NUMTOWIN-2);
    if(nextToPlayLine(Map,color, requireOpenEnd, target)>0) return 0;
    requireOpenEnd.clear();
    target.clear();
    // four-in-line-without-open-end and three-in-line-with-open-end
    requireOpenEnd.push_back(false);
    target.push_back(NUMTOWIN-2);
    requireOpenEnd.push_back(true);
    target.push_back(NUMTOWIN-3);
    if(nextToPlayLine(Map,color, requireOpenEnd, target)>0) return 0;
    requireOpenEnd.clear();
    target.clear();
    // double three-in-line-with-open-end
    requireOpenEnd.push_back(true);
    target.push_back(NUMTOWIN-3);
    requireOpenEnd.push_back(true);
    target.push_back(NUMTOWIN-3);
    if(nextToPlayLine(Map,color, requireOpenEnd, target)>0) return 0;
    requireOpenEnd.clear();
    target.clear();
    // three-in-line-with-open-end
    requireOpenEnd.push_back(true);
    target.push_back(NUMTOWIN-3);
    if(nextToPlayLine(Map,color, requireOpenEnd, target)>0) return 0;
    requireOpenEnd.clear();
    target.clear();
    // four-in-line-without-open-end
    requireOpenEnd.push_back(false);
    target.push_back(NUMTOWIN-2);
    if(nextToPlayLine(Map,color, requireOpenEnd, target)>0) return 0;
    requireOpenEnd.clear();
    target.clear();
    // two-in-line-with-open-end
    requireOpenEnd.push_back(true);
    target.push_back(NUMTOWIN-4);
    if(nextToPlayLine(Map,color, requireOpenEnd, target)>0) return 0;
    requireOpenEnd.clear();
    target.clear();
    return 0;
}

void calWinResult(char c, bool &isTie,int &numB,int &numW,char &result) const
{
    if(c==BLACK){
        isTie = false;
        numB++;
        numW=0;
        if(numB==NUMTOWIN) result = BLACK;
    }
    else if(c==WHITE){
        isTie = false;
        numB=0;
        numW++;
        if(numW==NUMTOWIN) result = WHITE;
    }
    else{ numW=0; numB=0; }
}

char whoWin(vector<vector<char>> &Map) const
{
    // Return BLACK if BLACK wins
    // Return WHITE if WHITE wins
    bool isTie = true;
    // Same row or column
    int numRowBConnect = 0;
    int numRowWConnect = 0;
    int numColBConnect = 0;
    int numColWConnect = 0;
    char result = -1;
    for(int i = 0;i<SIZE;i++){
        numRowBConnect=0;
        numRowWConnect=0;
        numColBConnect = 0;
        numColWConnect = 0;
        for(int j = 0;j<SIZE;j++){
            calWinResult(Map[i][j], isTie,numRowBConnect,numRowWConnect,result);
            calWinResult(Map[j][i], isTie,numColBConnect,numColWConnect,result);
            if(result!=-1) return result;
        }
    }
    // Cross line
    // Left-Top to Right-bottom
    for(int k=0;k<SIZE-NUMTOWIN+1;k++){
        numRowBConnect=0;
        numRowWConnect=0;
        for(int i = k,j = 0;i<SIZE && j<SIZE;i++,j++){
            calWinResult(Map[i][j], isTie,numRowBConnect,numRowWConnect,result);
            if(result!=-1) return result;
        }
    }
    // Right-top to Left-bottom
    for(int k=NUMTOWIN-1;k<SIZE;k++){
        numRowBConnect=0;
        numRowWConnect=0;
        // Different Column
        for(int i = k,j = 0;i>=0 && j<SIZE;i--,j++){
            calWinResult(Map[i][j], isTie,numRowBConnect,numRowWConnect,result);
            if(result!=-1) return result;
        }
    }
    if(isTie==true) return 0;
    
    return -1; // Return (-1) if no one wins
}


std::ostream& operator<<(std::ostream &strm, const State& obj)
{
  const auto& position = obj.position;
  size_t width = 0;
  if (!position.empty()) {
    width = position[0].size();
  }
  strm << "  ";
  for (size_t i = 0; i < width; ++i) {
    strm << (char)('A' + i) << " ";
  }
  strm << '\n';
  strm << "  ";
  for (size_t i = 0; i < width; ++i) {
    strm << (i % 10) << " ";
  }
  strm << '\n';
  int y = 0;
  for (const auto& row : position) {
    strm << (y % 10) << " ";
    for (const auto& val: row) {
      if (val == 0) {
        strm << 'O';
      } else if (val == 1) {
        strm << 'X';
      } else if (val == 2) {
        strm << '.';
      } else {
        throw std::runtime_error(std::string("Unknown value ") + val);
      }
      strm << " ";
    }
    strm << '\n';
    y += 1;
  }
  return strm;
}

}

#endif // STATE_H_INCLUDED
