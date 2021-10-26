#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#define DIMENSION_STATE 9

#define CROSS 1                             //a value to indicate that a position is occupied by a X
#define CIRCLE -1                           //a value to indicate that a position is occupied by a O
#define EMPTY 0                             //a value to indicate that a position is not occupied

#define WIN 1.0
#define DRAW 0.5
#define LOSE 0.0


#define C sqrt(2.)                          //parameter for the implementation of the UCT algorithm
#define MCCycles DIMENSION_STATE*1000       //decreasing this value will make the CPU do some sub-optimal moves some time

using namespace std;

//a function to check if the state of the board is a terminal state of the game
double EndOfGame(vector<int> State){
    if((State[0]==CROSS)&&(State[1]==CROSS)&&(State[2]==CROSS)){
        return CROSS;
    } else if((State[3]==CROSS)&&(State[4]==CROSS)&&(State[5]==CROSS)){
        return CROSS;
    } else if((State[6]==CROSS)&&(State[7]==CROSS)&&(State[8]==CROSS)){
        return CROSS;
    } else if((State[0]==CROSS)&&(State[3]==CROSS)&&(State[6]==CROSS)){
        return CROSS;
    } else if((State[1]==CROSS)&&(State[4]==CROSS)&&(State[7]==CROSS)){
        return CROSS;
    } else if((State[2]==CROSS)&&(State[5]==CROSS)&&(State[8]==CROSS)){
        return CROSS;
    } else if((State[0]==CROSS)&&(State[4]==CROSS)&&(State[8]==CROSS)){
        return CROSS;
    } else if((State[2]==CROSS)&&(State[4]==CROSS)&&(State[6]==CROSS)){
        return CROSS;
    } else if((State[0]==CIRCLE)&&(State[1]==CIRCLE)&&(State[2]==CIRCLE)){
        return CIRCLE;
    } else if((State[3]==CIRCLE)&&(State[4]==CIRCLE)&&(State[5]==CIRCLE)){
        return CIRCLE;
    } else if((State[6]==CIRCLE)&&(State[7]==CIRCLE)&&(State[8]==CIRCLE)){
        return CIRCLE;
    } else if((State[0]==CIRCLE)&&(State[3]==CIRCLE)&&(State[6]==CIRCLE)){
        return CIRCLE;
    } else if((State[1]==CIRCLE)&&(State[4]==CIRCLE)&&(State[7]==CIRCLE)){
        return CIRCLE;
    } else if((State[2]==CIRCLE)&&(State[5]==CIRCLE)&&(State[8]==CIRCLE)){
        return CIRCLE;
    } else if((State[0]==CIRCLE)&&(State[4]==CIRCLE)&&(State[8]==CIRCLE)){
        return CIRCLE;
    } else if((State[2]==CIRCLE)&&(State[4]==CIRCLE)&&(State[6]==CIRCLE)){
        return CIRCLE;
    } else if((State[0]!=EMPTY)&&(State[1]!=EMPTY)&&(State[2]!=EMPTY)&&(State[3]!=EMPTY)&&(State[4]!=EMPTY)&&(State[5]!=EMPTY)&&(State[6]!=EMPTY)&&(State[7]!=EMPTY)&&(State[8]!=EMPTY)){
        return DRAW;
    } else {
        return 0;
    }
}

//a class for each node in the Monte Carlo Tree
class Node {
public:
    vector<int> State;
    int Mark;
    int NumberOfSons;
    int LeafFlag, RootFlag;
    shared_ptr <Node> Father;
    vector< shared_ptr <Node> > Sons;
    int n;
    double w;
    
    //initialization function
    Node(int mark=CROSS, int rootflag=1, int leafflag=1, int nsons=0){
        int i;
        
        Mark = mark;
        RootFlag = rootflag;
        LeafFlag = leafflag;
        NumberOfSons = nsons;
        n = 0;
        w = 0;
        Father = nullptr;
        State.resize(DIMENSION_STATE);
        for(i=0;i<DIMENSION_STATE;i++){
            State[i] = EMPTY;
        }
        
    }
    
    //a function that returns the number of sons a certain node has
    int HowManySons(){
        
        return count(State.begin(), State.end(), 0);;
    }
    
    //a function to expand the tree downwards, creating all the sons of a node
    void Expansion(shared_ptr<Node> currentNode){
        int i,skipped_position,position_index;
        
        NumberOfSons = HowManySons();
        Sons.resize(NumberOfSons);
        for(i=0;i<NumberOfSons;i++){
            Sons[i] = nullptr;
        }
        
        LeafFlag = 0;
        for(i=0;i<NumberOfSons;i++){
            Sons[i] = make_shared<Node> (-Mark,0,1,NumberOfSons-1);
            Sons[i]->Father = currentNode;
            skipped_position = 0;
            for(position_index=0;position_index<DIMENSION_STATE;position_index++){
                if(State[position_index]!=EMPTY){
                    Sons[i]->State[position_index] = State[position_index];
                } else {
                    if(skipped_position==i){
                        Sons[i]->State[position_index] = Mark;
                        skipped_position++;
                    } else {
                        Sons[i]->State[position_index] = State[position_index];
                        skipped_position++;
                    }
                }
            }
        }
    };
    
    //a function to simulate the game until a terminal state, starting from the current node
    void Playout(double * outcome){
        vector<int> state_copy;
        int instant_mark, selected_move;
        int i;
        
        state_copy = State;
        instant_mark = Mark;
        while(EndOfGame(state_copy) == 0){
            while(true){
                selected_move = lrand48()%DIMENSION_STATE;
                if(state_copy[selected_move]==0){
                    state_copy[selected_move] = instant_mark;
                    instant_mark = instant_mark*(-1);
                    break;
                }
            }
        }
        *outcome = EndOfGame(state_copy);
    };
    
    //a function to compute the maximum UCT among all the sons
    int FindMaxUCT(){
        int idx, k;
        double uct, max_uct;

        k = 0;
        idx = k;
        if((Sons[k]->n)==0){
            max_uct = 100;
        } else {
            max_uct = Sons[k]->w / Sons[k]->n + C*sqrt(log(n) / Sons[k]->n);
        }
        for(k=1;k<NumberOfSons;k++){
            if((Sons[k]->n)==0){
                uct = 100;
            } else {
                uct = Sons[k]->w / Sons[k]->n + C*sqrt(log(n) / Sons[k]->n);
            }
            if(uct > max_uct){
                max_uct = uct;
                idx = k;
            }
        }

        return idx;
    };
    
    //a function to print the current state of the board on the terminal
    void PrintGame(){
        int i;
        string vertical, horizontal, cross, circle, empty;
        
        vertical = "|";
        horizontal = "-------------";
        cross = " X ";
        circle = " O ";
        empty = "   ";
        
        cout << vertical;
        if(State[0]==1){
            cout << cross;
        } else if(State[0]==-1){
            cout << circle;
        } else {
            cout << empty;
        }
        cout << vertical;
        if(State[1]==1){
            cout << cross;
        } else if(State[1]==-1){
            cout << circle;
        } else {
            cout << empty;
        }
        cout << vertical;
        if(State[2]==1){
            cout << cross;
        } else if(State[2]==-1){
            cout << circle;
        } else {
            cout << empty;
        }
        cout << vertical << "\n";
        
        cout << horizontal << "\n";
        
        cout << vertical;
        if(State[3]==1){
            cout << cross;
        } else if(State[3]==-1){
            cout << circle;
        } else {
            cout << empty;
        }
        cout << vertical;
        if(State[4]==1){
            cout << cross;
        } else if(State[4]==-1){
            cout << circle;
        } else {
            cout << empty;
        }
        cout << vertical;
        if(State[5]==1){
            cout << cross;
        } else if(State[5]==-1){
            cout << circle;
        } else {
            cout << empty;
        }
        cout << vertical << "\n";
        
        cout << horizontal << "\n";
        
        cout << vertical;
        if(State[6]==1){
            cout << cross;
        } else if(State[6]==-1){
            cout << circle;
        } else {
            cout << empty;
        }
        cout << vertical;
        if(State[7]==1){
            cout << cross;
        } else if(State[7]==-1){
            cout << circle;
        } else {
            cout << empty;
        }
        cout << vertical;
        if(State[8]==1){
            cout << cross;
        } else if(State[8]==-1){
            cout << circle;
        } else {
            cout << empty;
        }
        cout << vertical << "\n";
        
        
        cout << "\n";
    }
    
};

//a function to recursively select a child node, starting from the current node
shared_ptr<Node> Selection(shared_ptr<Node> currentNode){
    int index;

    currentNode->n = currentNode->n + 1;
    if((EndOfGame(currentNode->State)!=0) || (currentNode->LeafFlag)){
        return currentNode;
    } else {
        index = (*currentNode).FindMaxUCT();
        return Selection(currentNode->Sons[index]);
    }
};

//a function to propagate back the information recursively to all the parent nodes
shared_ptr<Node> BackPropagation(shared_ptr<Node> currentNode, double * outcome){
    
    double * new_pointer;
    
    new_pointer = outcome;
    if(*outcome==DRAW){
        currentNode->w += DRAW;
    } else if(*outcome==currentNode->Mark){
        currentNode->w += LOSE;
    } else {
        currentNode->w += WIN;
    }
    if(currentNode->RootFlag){
        return currentNode;
    } else {
        return BackPropagation(currentNode->Father,new_pointer);
    }
};

//a function to make the user play his/her move
shared_ptr<Node> UserMove(shared_ptr<Node> currentNode){
    int move,i;
    
    i=0;
    
    if(currentNode->State[0]==0){
        i++;
        if(currentNode->Mark == CROSS){
            cout << "Move n. " << i << ": put a CROSS in the upper-left position.\n";
        } else {
            cout << "Move n. " << i << ": put a CIRCLE in the upper-left position.\n";
        }
    }
    
    if(currentNode->State[1]==0){
        i++;
        if(currentNode->Mark == CROSS){
            cout << "Move n. " << i << ": put a CROSS in the upper-central position.\n";
        } else {
            cout << "Move n. " << i << ": put a CIRCLE in the upper-central position.\n";
        }
    }
    
    if(currentNode->State[2]==0){
        i++;
        if(currentNode->Mark == CROSS){
            cout << "Move n. " << i << ": put a CROSS in the upper-right position.\n";
        } else {
            cout << "Move n. " << i << ": put a CIRCLE in the upper-right position.\n";
        }
    }
    
    if(currentNode->State[3]==0){
        i++;
        if(currentNode->Mark == CROSS){
            cout << "Move n. " << i << ": put a CROSS in the central-left position.\n";
        } else {
            cout << "Move n. " << i << ": put a CIRCLE in the central-left position.\n";
        }
    }
    
    if(currentNode->State[4]==0){
        i++;
        if(currentNode->Mark == CROSS){
            cout << "Move n. " << i << ": put a CROSS in the central position.\n";
        } else {
            cout << "Move n. " << i << ": put a CIRCLE in the central position.\n";
        }
    }
    
    if(currentNode->State[5]==0){
        i++;
        if(currentNode->Mark == CROSS){
            cout << "Move n. " << i << ": put a CROSS in the central-right position.\n";
        } else {
            cout << "Move n. " << i << ": put a CIRCLE in the central-right position.\n";
        }
    }
    
    if(currentNode->State[6]==0){
        i++;
        if(currentNode->Mark == CROSS){
            cout << "Move n. " << i << ": put a CROSS in the lower-left position.\n";
        } else {
            cout << "Move n. " << i << ": put a CIRCLE in the lower-left position.\n";
        }
    }
    
    if(currentNode->State[7]==0){
        i++;
        if(currentNode->Mark == CROSS){
            cout << "Move n. " << i << ": put a CROSS in the lower-central position.\n";
        } else {
            cout << "Move n. " << i << ": put a CIRCLE in the lower-central position.\n";
        }
    }
    
    if(currentNode->State[8]==0){
        i++;
        if(currentNode->Mark == CROSS){
            cout << "Move n. " << i << ": put a CROSS in the lower-right position.\n";
        } else {
            cout << "Move n. " << i << ": put a CIRCLE in the lower-right position.\n";
        }
    }
    
    while(true){
        cin >> move;
        if((move<1)||(move>i)){
            cout << "You selected an impossible move! Choose again.\n";
        } else {
            break;
        }
    }
    
    currentNode->Sons[move-1]->RootFlag = 1;
    currentNode->Sons[move-1]->Father = nullptr;
    return currentNode->Sons[move-1];
}

//a function to select the best child according to the UCT score
shared_ptr<Node> BestChild(shared_ptr<Node> currentNode){
    int selected_son;
    
    selected_son = currentNode->FindMaxUCT();
    currentNode->Sons[selected_son]->RootFlag = 1;
    currentNode->Sons[selected_son]->Father = nullptr;
    
    return currentNode->Sons[selected_son];
    
}

