#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include <iostream>
#define NEGLARGE -1000
#define POSLARGE 1000
#define PLY 5

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
	    
	    int alphaBeta(Turn turn, const OthelloBoard& board, int alpha, int beta, int ply);	
	    int finalValue (Turn turn, const OthelloBoard& board);
	    int countDifference(Turn turn, const OthelloBoard& board);
       	int evaluationFunction(Turn turn, const OthelloBoard& board);
    private:
		Move* bestMove;
		int weights[8][8];
		
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
    list<Move> moves = board.getValidMoves( turn );
    if(moves.size()==0)
        return Move::pass();
    OthelloBoard b;
    list<Move>::iterator it;
    int currbest = POSLARGE;
    Move *cm = 0;
    bool firstRun = true;
    int val;
    for(it=moves.begin(); it!=moves.end(); it++){
        b = board;
        b.makeMove(turn,*it);
        val = alphaBeta(other(turn), b, NEGLARGE, val, PLY);
        if(val<currbest || firstRun){
            currbest = val;
            cm = &(*it);
            firstRun = false;
        }
    }
    if(cm==0) cout<<"cm is null!!"<<endl<<moves.size()<<endl;
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



int MyBot::alphaBeta(Turn turn, const OthelloBoard& board, int alpha, int beta, int ply)
{
	if(ply==0) {
		return evaluationFunction(turn, board);
	}
	else {
		list<Move> moves = board.getValidMoves(turn);
		int noOfMoves = moves.size();
		if(noOfMoves == 0) {
		    
		    list<Move> oppMoves = board.getValidMoves(other(turn));
			if(oppMoves.size()!= 0){
			    return (- (alphaBeta (other(turn), board, -beta, -alpha, ply-1)));
			}
			else return finalValue(turn, board);
		}
		else {
			list<Move>::iterator it = moves.begin();
			for(; it != moves.end(); it++){
				OthelloBoard board2 = board;
				board2.makeMove(turn, *it);
				//val is alpha in the case of max and beta in the case of min
				int val = - alphaBeta(other(turn), board2, - beta, -alpha, (ply-1));
				if(val >= beta)
				     return val;
				if(val >= alpha) {
				    alpha = val;
				    if(ply == PLY)
					    bestMove = &(*it);
				}
				
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
	if(diff > 0) return POSLARGE;
	else if(diff == 0) return 0;
	else return NEGLARGE;
	
}

int MyBot::countDifference(Turn turn, const OthelloBoard& board){
	int redCount = board.getRedCount();
	int blackCount = board.getBlackCount();
	if(turn == RED){
		return redCount - blackCount;
	}
	else {
	    return blackCount - redCount;
	}	
}


