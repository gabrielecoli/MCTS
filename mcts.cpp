#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <memory>
#include "mcts.hpp"

using namespace std;

void Initialize(int *, int *);

int main() {

    shared_ptr<Node> currentNode, gameNode;
    int SelfPlay, User, seed;
    int t, i;
    double outcome;
    
    seed = time(0);
    srand48(seed);
    Initialize(&User,&SelfPlay);
    
    gameNode = make_shared<Node> ();
    currentNode = gameNode;
    
    gameNode->PrintGame();
    while(EndOfGame(gameNode->State)==0){
        currentNode = gameNode;
        if(gameNode->Mark == User){
            if(gameNode->NumberOfSons==0){
                gameNode->Expansion(gameNode);
            }
            gameNode = UserMove(gameNode);
        } else {
            for(t=0;t<MCCycles;t++){
                currentNode = Selection(currentNode);
                currentNode->Expansion(currentNode);
                currentNode->Playout(&outcome);
                currentNode = BackPropagation(currentNode,&outcome);
            }
            gameNode = BestChild(gameNode);
        }
        gameNode->PrintGame();
    }
    
    exit(0);

}


void Initialize(int * User, int * SelfPlay){
    int WhichGame;
    
    printf("\n:::TIC TAC TOE GAME:::\n\nChoose 1 if you want to play as 'X', 2 if you want to play as 'O', or anything else if you wish to watch the self-play mode. Note that if you choose 'X' you play first.\n\n");
    scanf("%d", &WhichGame);
    if(WhichGame==1){
        *User = CROSS;
        *SelfPlay = 0;
        printf("\nGame is on! It's your turn!\n");
    } else if(WhichGame==2){
        *User = CIRCLE;
        *SelfPlay = 0;
        printf("\nGame is on! It's your opponent's turn!\n");
    } else {
        *User = 0;
        *SelfPlay=1;
        printf("\nGame is on! Enjoy the match!\n");
    }
    
}
