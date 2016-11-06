//
//  simulation.h
//  AI-Monte-Carlo-Chess / gomoku
//
//  Copyright © 2016年 lin. All rights reserved.
//

#ifndef simulation_h
#define simulation_h

#include "constants.h"

bool isLine(std::vector<std::vector<char>> &map,std::pair<int, int> &this_step,char char_match,int target,bool open_end)
{
    int num_connect=0;
    // Same row
    int j_min=std::max(0,this_step.second+(-1)*target);
    int j_max=(map.size()<this_step.second+target)?(int)map.size():this_step.second+target;
    for(int j = j_min;j<j_max;j++){
        if(map[this_step.first][j]==char_match || j==this_step.second)
            num_connect++;
        else{ num_connect=0; }
        
        if(num_connect==target) return true;
    }
    // Same column
    int i_min=std::max(0,this_step.first+(-1)*target);
    int i_max=(map.size()<this_step.second+target)?(int)map.size():this_step.first+target;
    for(int i=i_min;i<i_max;i++){
        if(map[i][this_step.second]==char_match || i==this_step.second)
            num_connect++;
        else{ num_connect=0; }
        
        if(num_connect==target) return true;
    }
    // Cross line
    for(int i=i_min,j=j_min;j<j_max && i<i_max;i++,j++){
        if(map[i][j]==char_match || (i==this_step.first && j==this_step.second))
            num_connect++;
        else{ num_connect=0; }
        
        if(num_connect==target) return true;
    }
    for(int j=j_min,i=i_max-1;j<j_max && i>=i_min;j++,i--){
        if(map[i][j]==char_match || (i==this_step.first && j==this_step.second))
            num_connect++;
        else{ num_connect=0;}
        
        if(num_connect==target) return true;
    }
    return false;
}

bool random_output(std::vector<std::vector<char>> &map,std::vector<std::pair<int, int>> &a, std::vector<std::pair<int, int>> &b,char c)
{
    if(a.size()>0){
        std::pair<int,int> temp = a[rand()%a.size()];
        map[temp.first][temp.second] = c;
        return true;
    }
    if(b.size()>0){
        std::pair<int,int> temp = b[rand()%b.size()];
        map[temp.first][temp.second] = c;
        return true;
    }
    return false;
}
int next_to_play_each_line(std::vector<std::vector<char>> &map,char color, std::vector<bool> &requireopen_end, std::vector<int> &target)
{
    bool b_next = false;
    bool w_next = false;
    std::vector<std::pair<int, int>> next_position,second_next_position;
    for(int i=0;i<map.size();i++){
        for(int j=0 ; j<map.size(); j++,b_next = false,w_next = false){
            std::pair<int,int> temp_pair = std::make_pair(i,j);
            // if occupied, then continue
            if(map[i][j]==mcts::BLACK||map[i][j]==mcts::WHITE) continue;
            // check the position qualification
            int sum_black_target = 0;
            int sum_white_target = 0;
            for(int iTarget=0;iTarget<target.size();iTarget++){
                if(isLine(map,temp_pair, mcts::BLACK, target[iTarget],requireopen_end[iTarget])){
                    sum_black_target++;
                }
                if(isLine(map,temp_pair, mcts::WHITE, target[iTarget],requireopen_end[iTarget])){
                    sum_white_target++;
                }
            }
            if(sum_black_target>=target.size()) b_next=true;
            if(sum_white_target>=target.size()) w_next=true;
            
            //if(b_next) std::cout << "b_next map[" << i << "][" << j << "]" << std::endl;
            //if(w_next) std::cout << "w_next map[" << i << "][" << j << "]" << std::endl;
            if(color==mcts::BLACK){
                if(b_next) next_position.push_back(temp_pair);
                else if(w_next) second_next_position.push_back(temp_pair);
            }else{
                if(w_next) next_position.push_back(temp_pair);
                else if(b_next) second_next_position.push_back(temp_pair);
            }
        }
    }
    if(random_output(map,next_position,second_next_position,color)) return 1;
    next_position.clear();
    second_next_position.clear();
    return 0;
}

// return to interrupt
int next_to_play(std::vector<std::vector<char>> &map,char &color)
{
    std::vector<bool> requireopen_end;
    std::vector<int> target;
    // five-in-line
    //std::cout << "five-in-line" << std::endl;
    requireopen_end.push_back(false);
    target.push_back(mcts::NUMTOWIN-1);
    if(next_to_play_each_line(map,color, requireopen_end, target)>0) return 0;
    requireopen_end.clear();
    target.clear();
    // four-in-line-with-open-end
    requireopen_end.push_back(true);
    target.push_back(mcts::NUMTOWIN-2);
    if(next_to_play_each_line(map,color, requireopen_end, target)>0) return 0;
    requireopen_end.clear();
    target.clear();
    // four-in-line-without-open-end and three-in-line-with-open-end
    requireopen_end.push_back(false);
    target.push_back(mcts::NUMTOWIN-2);
    requireopen_end.push_back(true);
    target.push_back(mcts::NUMTOWIN-3);
    if(next_to_play_each_line(map,color, requireopen_end, target)>0) return 0;
    requireopen_end.clear();
    target.clear();
    // double three-in-line-with-open-end
    requireopen_end.push_back(true);
    target.push_back(mcts::NUMTOWIN-3);
    requireopen_end.push_back(true);
    target.push_back(mcts::NUMTOWIN-3);
    if(next_to_play_each_line(map,color, requireopen_end, target)>0) return 0;
    requireopen_end.clear();
    target.clear();
    // three-in-line-with-open-end
    requireopen_end.push_back(true);
    target.push_back(mcts::NUMTOWIN-3);
    if(next_to_play_each_line(map,color, requireopen_end, target)>0) return 0;
    requireopen_end.clear();
    target.clear();
    // four-in-line-without-open-end
    requireopen_end.push_back(false);
    target.push_back(mcts::NUMTOWIN-2);
    if(next_to_play_each_line(map,color, requireopen_end, target)>0) return 0;
    requireopen_end.clear();
    target.clear();
    // two-in-line-with-open-end
    requireopen_end.push_back(true);
    target.push_back(mcts::NUMTOWIN-4);
    if(next_to_play_each_line(map,color, requireopen_end, target)>0) return 0;
    requireopen_end.clear();
    target.clear();
    return 0;
}

void calculate_each_win_result(char c, bool &is_tie,int &num_black,int &num_white,char &result) {
    if(c==mcts::BLACK){
        is_tie = false;
        num_black++;
        num_white=0;
        if(num_black==mcts::NUMTOWIN) result = mcts::BLACK;
    }
    else if(c==mcts::WHITE){
        is_tie = false;
        num_black=0;
        num_white++;
        if(num_white==mcts::NUMTOWIN) result = mcts::WHITE;
    }
    else{ num_white=0; num_black=0; }
}

char who_win(std::vector<std::vector<char>> &map){  // Return mcts::BLACK if mcts::BLACK wins
    // Return mcts::WHITE if mcts::WHITE wins
    bool is_tie = true;
    // Same row or column
    int num_row_black_connect = 0;
    int num_row_white_connect = 0;
    int num_column_black_connect = 0;
    int num_column_white_connect = 0;
    char result = -1;
    for(int i = 0;i<map.size();i++){
        num_row_black_connect=0;
        num_row_white_connect=0;
        num_column_black_connect = 0;
        num_column_white_connect = 0;
        for(int j = 0;j<map.size();j++){
            calculate_each_win_result(map[i][j], is_tie,num_row_black_connect,num_row_white_connect,result);
            calculate_each_win_result(map[j][i], is_tie,num_column_black_connect,num_column_white_connect,result);
            if(result!=-1) return result;
        }
    }
    // Cross line
    // Left-Top to Right-bottom
    for(int k=0;k<map.size()-mcts::NUMTOWIN+1;k++){
        num_row_black_connect=0;
        num_row_white_connect=0;
        for(int i = k,j = 0;i<map.size() && j<map.size();i++,j++){
            calculate_each_win_result(map[i][j], is_tie,num_row_black_connect,num_row_white_connect,result);
            if(result!=-1) return result;
        }
    }
    // Right-top to Left-bottom
    for(int k=mcts::NUMTOWIN-1;k<map.size();k++){
        num_row_black_connect=0;
        num_row_white_connect=0;
        // Different Column
        for(int i = k,j = 0;i>=0 && j<map.size();i--,j++){
            calculate_each_win_result(map[i][j], is_tie,num_row_black_connect,num_row_white_connect,result);
            if(result!=-1) return result;
        }
    }
    if(is_tie==true) return 0;
    
    return -1; // Return (-1) if no one wins
}

#endif /* simulation_h */
