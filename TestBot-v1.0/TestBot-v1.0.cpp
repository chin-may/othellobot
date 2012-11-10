#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include <iostream>
#define NEGLARGE -1000
#define POSLARGE 1000
#define PLY 4

using namespace std;
using namespace Desdemona;


class MyBot: public OthelloPlayer
{
    public:
        /**
         * Initialisation routines here
         * This could do anything from open up a cache of "best moves" to
         * spawning a background processing thread. 
         */
        MyBot( Turn turn );

        /**
         * Play something 
         */
        virtual Move play(const OthelloBoard& board );
	    
	    int alphaBeta(Turn turn, const OthelloBoard& board, int alpha, int beta, int ply, int maxOrMin);	
	    int finalValue (Turn turn, const OthelloBoard& board);
	    int countDifference(Turn turn, const OthelloBoard& board);
       	int evaluationFunction(Turn turn, const OthelloBoard& board);
        void brutemeasure(Turn turn, const OthelloBoard& board, int ply);
    private:
		Move* bestMove;
		int weights[8][8];

        //Only for measuring performance
        int brutecount;
        int actcount;
};

MyBot::MyBot( Turn turn )
    : OthelloPlayer( turn )
{
        int temp[8][8] = {
							{120, -20, 20, 5, 5, 20, -20, 120},
							{-20, -40, -5, -5, -5, -5, -40, -20},
							{20, -5, 15, 3, 3, 15, -5, 20},
							{5, -5, 3, 3, 3, 3, -5, 5},
							{5, -5, 3, 3, 3, 3, -5, 5},
							{-20, -40, -5, -5, -5, -5, -40, -20},
							{20, -5, 15, 3, 3, 15, -5, 20},
							{120, -20, 20, 5, 5, 20, -20, 120}
					};
	    
	    for(int i=0; i < 8; i++){
	        for(int j=0; j<8; j++){
	            weights[i][j] = temp[i][j];
	        }
	    }
}

Move MyBot::play(const OthelloBoard& board )
{   
    brutecount = 0;
    actcount = 0;


    list<Move> moves = board.getValidMoves( turn );
    if(moves.size()==0)
        return Move::pass();
    OthelloBoard b;
    list<Move>::iterator it;
    int currbest = POSLARGE;
    Move *cm = 0;
    bool firstRun = true;
    for(it=moves.begin(); it!=moves.end(); it++){
        b = board;
        b.makeMove(turn,*it);
        int val = alphaBeta(other(turn), b, NEGLARGE, POSLARGE, PLY, -1);
        if(val<currbest || firstRun){
            currbest = val;
            cm = &(*it);
            firstRun = false;
        }
    }
    brutemeasure(turn, board, PLY+1);
    if(cm==0) cout<<"cm is null!!"<<endl<<moves.size()<<endl;
    cout<<"brute force would take: "<<brutecount<<" AlphaBeta took "<<actcount<<endl;
	return *cm;

}

// The following lines are _very_ important to create a bot module for Desdemona

extern "C" {
    OthelloPlayer* createBot( Turn turn )
    {
        return new MyBot ( turn );
    }

    void destroyBot( OthelloPlayer* bot )
    {
        delete bot;
    }
}

void MyBot::brutemeasure(Turn turn, const OthelloBoard& board, int ply){
    brutecount +=1;
    if(ply==0)
        return;
    list<Move> moves = board.getValidMoves(turn);
    list<Move>::iterator it;
    for(it = moves.begin(); it != moves.end(); it++){
        OthelloBoard b = board;
        b.makeMove(turn, *it);
        brutemeasure(other(turn), b, ply-1);
    }
}

int MyBot::alphaBeta(Turn turn, const OthelloBoard& board, int alpha, int beta, int ply, int maxOrMin)
{   
    actcount += 1;
	if(ply==0) {
		return evaluationFunction(turn, board);
	}
	else {
		list<Move> moves = board.getValidMoves(turn);
		int noOfMoves = moves.size();
		if(noOfMoves == 0) {
		    
		    list<Move> oppMoves = board.getValidMoves(other(turn));
			if(oppMoves.size()!= 0){
			    return (- (alphaBeta (other(turn), board, -beta, -alpha, ply-1, -maxOrMin)));
			}
			else return finalValue(turn, board);
		}
		else {
			list<Move>::iterator it = moves.begin();
			for(; it != moves.end(); it++){
				OthelloBoard board2 = board;
				board2.makeMove(turn, *it);
				//val is alpha in the case of max and beta in the case of min
				int val = - alphaBeta(other(turn), board2, - beta, -alpha, (ply-1), -maxOrMin);
				if(val >= beta)
				     return val;
				if(val >= alpha) {
				    alpha = val;
				    if(ply == PLY)
					    bestMove = &(*it);
				}
				
				//delete board2;	
			}
			return alpha;
		}	
	}

}


int MyBot::evaluationFunction(Turn turn, const OthelloBoard& board){
	
	int sum = 0;
	Turn opp = other(turn);
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			if(turn == board.get(i,j)){
				sum = sum + weights[i][j];
			}
			else if(opp == board.get(i,j)){
				sum = sum - weights[i][j];
		    }	
		}
	}
	return sum;
}

int MyBot::finalValue (Turn turn, const OthelloBoard& board){
	int diff = countDifference(turn, board);
	//printf("DIFF : %d\n",diff);
	if(diff > 0) return POSLARGE;
	else if(diff == 0) return 0;
	else return NEGLARGE;
	
}

int MyBot::countDifference(Turn turn, const OthelloBoard& board){
	int redCount = board.getRedCount();
	int blackCount = board.getBlackCount();
	if(turn == RED){
	    //printf("Count : %d\n",redCount - blackCount);
	        return redCount - blackCount;
	}
	else {
	    //printf("Count : %d\n",redCount - blackCount);
	    
	    return blackCount - redCount;
	}	
}


