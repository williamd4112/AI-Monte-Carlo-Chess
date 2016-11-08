//
//  simulation.h
//  AI-Monte-Carlo-Chess / gomoku
//
//  Copyright © 2016年 lin. All rights reserved.
//

#ifndef simulation_h
#define simulation_h

#include "constants.h"
#include "time.h"
void show_map(std::vector<std::vector<char>> &map){
    std::cout << std::endl << "   ";
    char c='A';
    for(int i=0;i<10;i++){
        std::cout << ' ' << i << ' ';
    } for(int i=10;i<map.size();i++){
        std::cout << i << ' ';
    } std::cout << std::endl;
    for(int i=0;i<map.size();i++,c++){
        std::cout << c << i%10 << ' ';
        for(int j=0;j<map.size();j++){
            char temp = map[i][j]+'A';
            if(map[i][j]!=mcts::WHITE && map[i][j]!=mcts::BLACK)  std::cout << ' ' << '+' << ' ';
            else    std::cout << ' ' << temp << ' ';
        }
        std::cout << std::endl;
    }
    
}
bool is_open_end(std::vector<std::vector<char>> &map,int i, int j,bool requirement){
    if(!requirement) return true;
    if(i<0||i>=map.size()) return false;
    if(j<0||j>=map.size()) return false;
    if(map[i][j]!=mcts::EMPTY) return false;
    return true;
}
bool is_row_line(std::vector<std::vector<char>> &map,std::pair<int, int> &this_step,char char_match,int target,bool open_end)
{
    int num_connect=0;
    // Same row
    int j_min=std::max(0,this_step.second+(-1)*target);
    int j_max=(map.size()<this_step.second+target)?(int)map.size():this_step.second+target;
    for(int j = j_min;j<j_max;j++){
        
        if(map[this_step.first][j]==char_match || j==this_step.second)
            num_connect++;
        else{ num_connect=0; }
        
        if(num_connect==target)
           if(is_open_end(map,this_step.first,j+1,open_end) &&
              is_open_end(map,this_step.first,j-target,open_end)){
               std::cout<<"map["<<this_step.first<<"]["<< j-target <<" to "<<j+1<<"] is double open for "<<target<<"\n";
               return true;
           }
    }
    return false;
}
bool is_column_line(std::vector<std::vector<char>> &map,std::pair<int, int> &this_step,char char_match,int target,bool open_end)
{
    // Same column
    int i_min=std::max(0,this_step.first+(-1)*target);
    int i_max=(map.size()<this_step.first+target)?(int)map.size():this_step.first+target;
    for(int i=i_min,num_connect=0;i<i_max;i++){
        if(map[i][this_step.second]==char_match || i==this_step.first)
            num_connect++;
        else{ num_connect=0; }
        
        if(num_connect==target)
           if(is_open_end(map,i+1,this_step.second,open_end) &&
              is_open_end(map,i-target,this_step.second,open_end)){
               std::cout<<"map["<<i<<"]["<<this_step.second<<"] is double open for "<<target<<"\n";
               return true;
           }
    }
    return false;
}
bool is_cross1_line(std::vector<std::vector<char>> &map,std::pair<int, int> &this_step,char char_match,int target,bool open_end)
{
    // Cross line
    int i_min=std::max(0,this_step.first+(-1)*target);
    int i_max=(map.size()<this_step.first+target)?(int)map.size():this_step.first+target;
    
    int j_min=std::max(0,this_step.second+(-1)*target);
    int j_max=(map.size()<this_step.second+target)?(int)map.size():this_step.second+target;
    for(int i=i_min,j=j_min,num_connect=0;j<j_max && i<i_max;i++,j++){
        if(map[i][j]==char_match || (i==this_step.first && j==this_step.second))
            num_connect++;
        else{ num_connect=0; }
        
        if(num_connect==target &&
           (is_open_end(map,i+1,j+1,open_end) &&
            is_open_end(map,i-target,j-target,open_end)))
            return true;
    }
    return false;
}
bool is_cross2_line(std::vector<std::vector<char>> &map,std::pair<int, int> &this_step,char char_match,int target,bool open_end)
{
    int i_min=std::max(0,this_step.first+(-1)*target);
    int i_max=(map.size()-1<this_step.first+target)?(int)map.size()-1:this_step.first+target;
    
    int j_min=std::max(0,this_step.second+(-1)*target);
    int j_max=(map.size()-1<this_step.second+target)?(int)map.size()-1:this_step.second+target;
    // Cross line
    for(int j=j_min,i=i_max,num_connect=0;j<j_max && i>=i_min;j++,i--){
        if(map[i][j]==char_match || (i==this_step.first && j==this_step.second))
            num_connect++;
        else{ num_connect=0;}
        
        if(num_connect==target &&
           (is_open_end(map,i+1,j+1,open_end) &&
            is_open_end(map,i-target,j-target,open_end)))
            return true;
    }
    return false;
}
bool random_output(std::vector<std::vector<char>> &map,std::vector<std::pair<int, int>> &a,char c)
{
    if(a.size()>0){
        std::pair<int,int> temp = a[rand()%a.size()];
        map[temp.first][temp.second] = c;
        if(c==mcts::BLACK)
            std::cout << "BLACK["<<temp.first<<"]["<< temp.second <<"]" << std::endl;
        else
            std::cout << "WHITE["<<temp.first<<"]["<< temp.second <<"]" << std::endl;
        /*for(std::pair<int, int> item: a){
            std::cout << "first possible point: a["<<item.first<<"]["<< item.second <<"]" << std::endl;
        }*/
        return true;
    }
    return false;
}
bool four_direction_check(std::vector<std::vector<char>> &map,std::pair<int,int> &temp_pair, int color_match, int target, bool requireopen_end){
    if(is_row_line(map,temp_pair, color_match, target,requireopen_end)){
        return true;
    } else if(is_column_line(map,temp_pair, color_match, target,requireopen_end)){
        return true;
    } else if(is_cross1_line(map,temp_pair, color_match, target,requireopen_end)){
        return true;
    } else if(is_cross2_line(map,temp_pair, color_match, target,requireopen_end)){
        return true;
    } return false;
}
std::vector<std::pair<int,int>> next_to_play_each_line(std::vector<std::vector<char>> &map,char color,bool requireopen_end,int target)
{
    bool b_next = false;
    bool w_next = false;
    std::vector<std::pair<int, int>> next_position,second_next_position;
    for(int i=0;i<map.size();i++){
        for(int j=0 ; j<map.size(); j++,b_next = false,w_next = false){
            std::pair<int,int> temp_pair = std::make_pair(i,j);
            // if occupied, then continue
            if(map[i][j]==mcts::BLACK||map[i][j]==mcts::WHITE|| map[i][j]==-1) continue;
            // check the position qualification
            if(color==mcts::BLACK){
                if(four_direction_check(map,temp_pair,mcts::BLACK,target,requireopen_end))
                    next_position.push_back(temp_pair);
                else if(four_direction_check(map,temp_pair,mcts::WHITE,target,requireopen_end))
                    second_next_position.push_back(temp_pair);
            }else{
                if(four_direction_check(map,temp_pair,mcts::WHITE,target,requireopen_end))
                    next_position.push_back(temp_pair);
                else if(four_direction_check(map,temp_pair,mcts::BLACK,target,requireopen_end))
                    second_next_position.push_back(temp_pair);
            }
        }
    }
    if(next_position.size()>0) return next_position;
    return second_next_position;
}

// return to interrupt
int next_to_play(std::vector<std::vector<char>> &map,char &color)
{
    bool requireopen_end;
    int target;
    std::vector<std::pair<int,int>> three_in_line, four_in_line, temp_in_line;
    // five-in-line
    std::cout << "five-in-line" << std::endl;
    requireopen_end = false;
    target = mcts::NUMTOWIN;
    temp_in_line = next_to_play_each_line(map,color, requireopen_end, target);
    if(random_output(map,temp_in_line,color)>0) return 0;
    // four-in-line-with-open-end
    std::cout << "four-in-line-with-open-end" << std::endl;
    requireopen_end = true;
    target = mcts::NUMTOWIN-1;
    temp_in_line = next_to_play_each_line(map,color, requireopen_end, target);
    if(random_output(map,temp_in_line,color)>0) return 0;
    
    // -------------- count three and four and two first ------------------- //
    std::cout << "four-in-line-with-close-end" << std::endl;
    requireopen_end = false;
    target = mcts::NUMTOWIN-1;
    four_in_line = next_to_play_each_line(map,color, requireopen_end, target);
    
    std::cout << "three-in-line-with-open-end" << std::endl;
    requireopen_end = true;
    target = mcts::NUMTOWIN-2;
    three_in_line = next_to_play_each_line(map,color, requireopen_end, target);
    
    // four-in-line-without-open-end and three-in-line-with-open-end
    std::cout << "four-in-line-without-open-end and three-in-line-with-open-end" << std::endl;
    temp_in_line.clear();
    for(std::pair<int,int> p_four: four_in_line){
        for(std::pair<int,int> p_three: three_in_line){
            if(p_four==p_three) {
                temp_in_line.push_back(p_three);
                ;
            }
        }
    }
    if(random_output(map,temp_in_line,color)>0) return 0;
    
    // double three-in-line-with-open-end
    ;
    
    // three-in-line-with-open-end
    if(random_output(map,three_in_line,color)>0) return 0;
    // four-in-line-without-open-end
    if(random_output(map,four_in_line,color)>0) return 0;
    // two-in-line-with-open-end
    for(int i=mcts::NUMTOWIN-3;i>0;i--){
        requireopen_end = true;
        target = mcts::NUMTOWIN-3;
        temp_in_line = next_to_play_each_line(map,color, requireopen_end, target);
        if(random_output(map,temp_in_line,color)>0) return 0;
    }
    
    return -1;
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
    else{
        num_white=0;
        num_black=0;
    }
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
            if(result!=-1) return result;
            calculate_each_win_result(map[j][i], is_tie,num_column_black_connect,num_column_white_connect,result);
            if(result!=-1) return result;
        }
    }
    // Cross line
    for(int k=0;k<map.size()-mcts::NUMTOWIN+1;k++){
        num_row_black_connect=0;
        num_row_white_connect=0;
        num_column_black_connect = 0;
        num_column_white_connect = 0;
        for(int i = k,j = 0;i<map.size() && j<map.size();i++,j++){
            calculate_each_win_result(map[i][j], is_tie,num_row_black_connect,num_row_white_connect,result);
            if(result!=-1) return result;
            calculate_each_win_result(map[map.size()-i-1][j], is_tie,num_column_black_connect,num_column_white_connect,result);
            if(result!=-1) return result;
        }
        num_row_black_connect=0;
        num_row_white_connect=0;
        num_column_black_connect = 0;
        num_column_white_connect = 0;
        for(int i = 0,j = k;i<map.size() && j<map.size();i++,j++){
            calculate_each_win_result(map[i][j], is_tie,num_row_black_connect,num_row_white_connect,result);
            if(result!=-1) return result;
            calculate_each_win_result(map[i][map.size()-j-1], is_tie,num_column_black_connect,num_column_white_connect,result);
            if(result!=-1) return result;
        }
    }
    if(is_tie==true) return 0;
    
    return -1; // Return (-1) if no one wins
}

#endif /* simulation_h */
