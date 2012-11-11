#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include <iostream>
#include <queue>
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
       	int currentMobility(Turn turn,const OthelloBoard& board);
       	int potentialMobility(Turn turn,const OthelloBoard& board);
    private:
		Move* bestMove;
		int weights[8][8];
		
};

class MovePair
{
    public:
        Move *m;
        int score;
        MovePair(Move *mo, int sc){
            m = mo;
            score = sc;
        }
        bool operator<(const MovePair &mp) const{
            return score < mp.score;
        }

};

//class MovePairCompare
//{
    //public:
        //bool operator() (const Move& lhs, const Move& rhs){
            //if(lhs.score > rhs.score) return 1;
            //if(rhs.score > lhs.score) return -1;
            //return 0;
        //}
//};



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
    priority_queue<MovePair> pq;

    for(it=moves.begin(); it!=moves.end(); it++){
        b = board;
        b.makeMove(turn,*it);
        int score = weights[it->x][it->y];
        MovePair *tempm = new MovePair(&(*it), score);
        pq.push(*tempm);
    }
    

    while(!pq.empty()){
        Move* m = pq.top().m;
        pq.pop();
        b = board;
        b.makeMove(turn,*m);
        val = alphaBeta(other(turn), b, NEGLARGE, val, PLY);
        if(val<currbest || firstRun){
            currbest = val;
            cm = m;
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
    /**
     * Find the best move, for the player, according to evaluation function,
     * searching PLY levels deep and backing up values,
     * using cutoffs whenever possible.
     */
     
     //TODO : Order the nodes according to evaluation function? (Need to test efficiency?)
	if(ply==0) {
	    //TODO : Find factors for the evaluation function.
	    // return k1*evaluationFunction(turn,board) + k2*currentMobility(turn,board) + k3*potentialMobility(turnBoard);
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
	/**
	 * Sum of the weights of player's squares minus opponent's."
	 */
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
	/**
	 * Like weighted squares, but don't take off for moving
     * near an occupied corner."
     */
	int Is[12] = {0,0,1,1,1,1,6,6,6,6,7,7};
	int Js[12] = {1,6,0,1,6,7,0,1,6,7,1,6};
	for(int i=0; i<8;i++){
	    if(!(board.get(Is[i],Js[i] == EMPTY))){
	        if(board.get(Is[i],Js[i]) == turn)
	            sum += (5-weights[Is[i]][Js[i]])*1;
	        else
	            sum += (5-weights[Is[i]][Js[i]])*-1;
	    }
	}
	return sum;
}

int MyBot::currentMobility(Turn turn,const OthelloBoard& board){
    /**
     * Current Mobility is the number of legal moves.
     */
    int current = 0;
    list<Move> moves = board.getValidMoves(turn);
	current = moves.size();
    return current;
     		
}

int MyBot::potentialMobility(Turn turn,const OthelloBoard& board) {
    /**
     * Potential mobility is the number of blank squares
     * adjacent to an opponent that are not legal moves.
     */
    int potential = 0;
    Turn opponent = other(turn);
    for(int i = 0; i < 8 ; i++){
        for(int j = 0; j < 8 ; j++){
            // Taking care of the corners first
            if(i==0 && j==0){
                if(board.get(i+1,j+1) == opponent 
                    || board.get(i+1,j) == opponent 
                        || board.get(i,j+1) == opponent){
                    potential++;        
                }
            }
            else if(i==0 && j==7){
                if(board.get(i+1,j-1) == opponent 
                    || board.get(i+1,j) == opponent 
                        || board.get(i,j-1) == opponent){
                    potential++;        
                }
            }
            else if(i==7 && j ==0){
                if(board.get(i-1,j+1) == opponent 
                    || board.get(i-1,0) == opponent 
                        || board.get(0,j+1) == opponent){
                    potential++;        
                }
            }
            else if(i==7 && j==7){
                if(board.get(i-1,j-1) == opponent 
                    || board.get(i-1,j) == opponent 
                        || board.get(i,j-1) == opponent){
                    potential++;        
                }
            }
            
            // Now handle edge rows that are not corners
            
            else if(i==0) {
                if(board.get(i,j-1) == opponent || board.get(i,j+1) == opponent || board.get(i+1,j) == opponent)
                    potential++;
            }
            else if(j==0){
                if(board.get(i-1,j) == opponent || board.get(i+1,j) == opponent || board.get(i,j+1) == opponent)
                    potential++;
            }
            else if(i==7){
                if(board.get(i,j-1) == opponent || board.get(i,j+1) == opponent || board.get(i-1,j) == opponent)
                    potential++;
            }
            else if(j==7){
                if(board.get(i-1,j) == opponent || board.get(i+1,j) == opponent || board.get(i,j-1) == opponent)
                    potential++;
            }
            
            // Now the general case
            else {
                
                if(board.get(i-1,j-1) == opponent || board.get(i-1,j) == opponent || board.get(i-1,j+1) == opponent || board.get(i,j-1) == opponent || board.get(i,j+1) == opponent || board.get(i+1,j-1) == opponent || board.get(i+1,j) == opponent || board.get(i+1,j+1) == opponent)
                    potential++;
            }
        }
    }
     return potential;
}


int MyBot::finalValue (Turn turn, const OthelloBoard& board){
	/**
	 * Is this a win, loss, or draw for player?"
     */
	int diff = countDifference(turn, board);
	if(diff > 0) return POSLARGE;
	else if(diff == 0) return 0;
	else return NEGLARGE;
	
}

int MyBot::countDifference(Turn turn, const OthelloBoard& board){
	/**
	 * Count player's pieces minus opponent's pieces."
     */
	int redCount = board.getRedCount();
	int blackCount = board.getBlackCount();
	if(turn == RED){
		return redCount - blackCount;
	}
	else {
	    return blackCount - redCount;
	}	
}


