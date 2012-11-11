#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include <iostream>
#define NEGLARGE -1000
#define POSLARGE 1000
#define PLY 4 //Need to check how this fairs for higher ply

using namespace std;
using namespace Desdemona;

int moveNo = 0;
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

        int actEval(Turn turn, const OthelloBoard& board);
    private:
		int weights[8][8];
		bool isEdgeFull(const OthelloBoard& board, int edgeCoordinate, bool isHorizontal);
		int getStableDiscsFromEdge(const OthelloBoard& board, Turn color, int edgeCoordinate, bool isHorizontal);
		int getStableDiscsFromCorner(const OthelloBoard& board, Turn color, int cornerRowIndex, int cornerColumnIndex);
		int getStableDiscsCount(Turn turn, const OthelloBoard& board);
    	
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
							{20, -5, 15, 3, 3, 15, -5, 20},
							{-20, -40, -5, -5, -5, -5, -40, -20},
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
    int val = POSLARGE;
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
	moveNo = moveNo+2;
    return *cm;

}



int MyBot::currentMobility(Turn turn,const OthelloBoard& board){
    /**
     * Current Mobility is the number of legal moves.
     */
     //TODO : Shouldn't this actually be the number of flips?
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
                        || board.get(i,j+1) == opponent){
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
	moveNo++;
	if(ply==0) {
		moveNo--;
		return actEval(turn, board);
	}
	else {
		list<Move> moves = board.getValidMoves(turn);
		int noOfMoves = moves.size();
		if(noOfMoves == 0) {
		    
		    list<Move> oppMoves = board.getValidMoves(other(turn));
			if(oppMoves.size()!= 0){
			    moveNo--;
				return (- (alphaBeta (other(turn), board, -beta, -alpha, ply-1)));
			}
			else{
				moveNo--;
				return finalValue(turn, board);
			}
		}
		else {
			list<Move>::iterator it = moves.begin();
			for(; it != moves.end(); it++){
				OthelloBoard board2 = board;
				board2.makeMove(turn, *it);
				//val is alpha in the case of max and beta in the case of min
				int val = - alphaBeta(other(turn), board2, - beta, -alpha, (ply-1));
				if(val >= beta){
				    moveNo--;
					return val;
				}
				if(val >= alpha) {
				    alpha = val;
				}
				
			}
			moveNo--;
			return alpha;
		}	
	}

}

int MyBot::actEval(Turn turn, const OthelloBoard& board){
    //return evaluationFunction(turn, board);
    //cout<<"eval "<<evaluationFunction(turn, board)<<" pot m "<<potentialMobility(turn, board)<<" current m "<<currentMobility(turn, board)<<endl;
	int currmob = currentMobility(turn, board);
    int potmob = potentialMobility(turn, board);

    Turn opp = other(turn);
    int oppcurrmob = currentMobility(opp, board);
    int opppotmob = potentialMobility(opp, board);

    //cout<<" currmob "<<currmob<<" potmob "<<potmob<<" oppcurrmob "<<oppcurrmob<<" opppotmob "<<opppotmob<<endl;
    // TODO : Check factors for better results
    return 3*evaluationFunction(turn, board) + 300*getStableDiscsCount(turn,board) + 100*(currmob - oppcurrmob) + 50*(potmob - opppotmob);
    // + 2*( currmob - oppcurrmob ) +  (potmob - opppotmob);
    //return 3*evaluationFunction(turn, board) + 2*currentMobility(turn, board) + potentialMobility(turn, board);

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


    int cornerx[4] = {0,0,7,7};
    int cornery[4] = {0,7,0,7};

	int Is[12] = {0,1,1, 0,1,1, 6,7,6, 6,7,6};
	int Js[12] = {1,0,1, 6,7,6, 0,1,1, 7,6,6};

	for(int i=0; i<12;i++){
	    if(!(board.get(cornerx[i/3],cornery[i/3]) == EMPTY)){
	        if(board.get(Is[i],Js[i]) == turn)
	            sum += 5-weights[Is[i]][Js[i]];

            else if(board.get(Is[i],Js[i]) == opp)
	            sum -= 5-weights[Is[i]][Js[i]];
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

int MyBot::getStableDiscsCount(Turn turn, const OthelloBoard& board){
    int x = 
     getStableDiscsFromCorner(board, turn, 0, 0)
        + getStableDiscsFromCorner(board, turn, 0, 7)
        + getStableDiscsFromCorner(board, turn, 7, 0)
        + getStableDiscsFromCorner(board, turn, 7, 7)
        + getStableDiscsFromEdge(board, turn, 0, true)
        + getStableDiscsFromEdge(board, turn, 7, true)
        + getStableDiscsFromEdge(board, turn, 0, false)
        + getStableDiscsFromEdge(board, turn, 7, false);
    return x;
}

int MyBot::getStableDiscsFromCorner(const OthelloBoard& board, Turn color, int cornerRowIndex, int cornerColumnIndex) {
    int result = 0;

    int rowIndexChange = (cornerRowIndex == 0) ? 1 : -1;
    int columnIndexChange = (cornerColumnIndex == 0) ? 1 : -1;

    int rowIndex = cornerRowIndex;
    int rowIndexLimit = (cornerRowIndex == 0) ? 8 : -1;
    int columnIndexLimit = (cornerColumnIndex == 0) ? 8 : -1;
    for (rowIndex = cornerRowIndex; rowIndex != rowIndexLimit; rowIndex += rowIndexChange)
    {
        int columnIndex;
        for (columnIndex = cornerColumnIndex; columnIndex != columnIndexLimit; columnIndex += columnIndexChange){
            if (board.get(rowIndex, columnIndex) == color){
                result++;
            }
            else{
                break;
            }
        }

        if ((columnIndexChange > 0 && columnIndex < 8) || (columnIndexChange < 0 && columnIndex > 0)){
            columnIndexLimit = columnIndex - columnIndexChange;

            if (columnIndexChange > 0 && columnIndexLimit == 0)
            {
                columnIndexLimit++;
            }
            else if (columnIndexChange < 0 && columnIndexLimit == 7)
            {
                columnIndexLimit--;
            }

            if ((columnIndexChange > 0 && columnIndexLimit < 0)
                || (columnIndexChange < 0 && columnIndexLimit > 7))
            {
                break;
            }
        }
    }

    return result;
}

int MyBot::getStableDiscsFromEdge(const OthelloBoard& board, Turn color, int edgeCoordinate, bool isHorizontal)
{
    int result = 0;

    if (isEdgeFull(board, edgeCoordinate, isHorizontal))
    {
        bool oppositeColorDiscsPassed = false;
        for (int otherCoordinate = 0; otherCoordinate < 8; otherCoordinate++)
        {                
            Turn fieldColor = (isHorizontal) ? board.get(edgeCoordinate, otherCoordinate) : board.get(otherCoordinate, edgeCoordinate);
            if (fieldColor != color)
            {
                oppositeColorDiscsPassed = true;
            }
            else if (oppositeColorDiscsPassed)
            {
                int consecutiveDiscsCount = 0;
                while ((otherCoordinate < 8) && (fieldColor == color))
                {
                    consecutiveDiscsCount++;

                    otherCoordinate++;
                    if (otherCoordinate < 7)
                    {
                        fieldColor = (isHorizontal) ? board.get(edgeCoordinate, otherCoordinate) : board.get(otherCoordinate, edgeCoordinate);
                    }
                }
                if (otherCoordinate != 8)
                {
                    result += consecutiveDiscsCount;
                    oppositeColorDiscsPassed = true;
                }                                             
            }
        }
    }

    return result;
}

bool MyBot::isEdgeFull(const OthelloBoard& board, int edgeCoordinate, bool isHorizontal){
    for (int otherCoordinate = 0; otherCoordinate < 8; otherCoordinate++){
        if ((isHorizontal && (board.get(edgeCoordinate, otherCoordinate) == EMPTY))
            || (!isHorizontal && (board.get(otherCoordinate, edgeCoordinate) == EMPTY))){
            return false;
        }
    }
    return true;
}
