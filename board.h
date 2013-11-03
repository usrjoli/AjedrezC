#ifndef WINGLET_BOARD_H_
#define WINGLET_BOARD_H_
 
#include "defines.h"
#include "move.h"
#include "gameline.h"
#include "timer.h"

struct Board
{
	BitMap whiteKing, whiteQueens, whiteRooks, whiteBishops, whiteKnights, whitePawns;
	BitMap blackKing, blackQueens, blackRooks, blackBishops, blackKnights, blackPawns;
	BitMap whitePieces, blackPieces, occupiedSquares;
 
	unsigned char nextMove;        // WHITE_MOVE or BLACK_MOVE
	unsigned char castleWhite;     // White's castle status, CANCASTLEOO = 1, CANCASTLEOOO = 2
	unsigned char castleBlack;     // Black's castle status, CANCASTLEOO = 1, CANCASTLEOOO = 2
	int epSquare;                  // En-passant target square after double pawn move
	int fiftyMove;                 // Moves since the last pawn move or capture
	U64 hashkey;                   // Random 'almost' unique signature for current board position.
 
	// additional variables:
	int square[64];                // incrementally updated, this array is usefull if we want to
								   // probe what kind of piece is on a particular square.
	int Material;                  // incrementally updated, total material balance on board,
								   // in centipawns, from white�s side of view
	int totalWhitePawns;           // sum of P material value for white (in centipawns)
	int totalBlackPawns;           // sum of P material value for black  (in centipawns)
	int totalWhitePieces;          // sum of Q+R+B+N material value for white (in centipawns)
	int totalBlackPieces;          // sum of Q+R+B+N material value for black  (in centipawns)

	BOOLTYPE viewRotated;          // only used for displaying the board. TRUE or FALSE.

	// storing moves:
	Move moveBuffer[MAX_MOV_BUFF]; // all generated moves of the current search tree are stored in this array.
	int moveBufLen[MAX_PLY];       // this arrays keeps track of which moves belong to which ply
	char movesMateInN[MAX_MOV_BUFF][MAX_PATH_MOVES];
	int topeMovesMateInN;
	int endOfGame;                 // index for board.gameLine
	int endOfSearch;               // index for board.gameLine
	GameLineRecord gameLine[MAX_GAME_LINE];

	// search variables:
	int triangularLength[MAX_PLY];
	Move triangularArray[MAX_PLY][MAX_PLY];
	Timer timer;
	U64 msStart, msStop;
	int searchDepth;
	int lastPVLength;
	Move lastPV[MAX_PLY];
	unsigned int whiteHeuristics[64][64];
	unsigned int blackHeuristics[64][64];
	BOOLTYPE followpv;
	BOOLTYPE allownull;
	U64 inodes;
	U64 countdown;
	U64 maxTime;
	BOOLTYPE timedout;
	BOOLTYPE ponder;

	void init();
	int eval();
	int evalJL(int pA1, int pA2, int pA3, int pA4, int pIndexMoveBufLen);
	double evalJLD(double pA1, double pA2, double pA3, double pA4, int pIndexMoveBufLen);
	Move think();
	int minimax(int ply, int depth, bool inicio);//inicio es para indicar que es la primer invocaci�n, as� hacemos el mate in
	int alphabeta(int ply, int depth, int alpha, int beta);
	int alphabetapvs(int ply, int depth, int alpha, int beta);
	double alphabetapvsD(int ply, int depth, double alpha, double beta);
	int qsearch(int ply, int alpha, int beta);
	double qsearchD(int ply, double alpha, double beta);
	void displaySearchStats(int mode, int depth, int score);
	void displaySearchStatsD(int mode, int depth, int score, double scoreD); // para mode = 2 se utiliza scoreD
	BOOLTYPE isEndOfgame(int &legalmoves, Move &singlemove);
	BOOLTYPE isEndOfgameMate(int &legalmoves, Move &singlemove, int pIndex);
	int repetitionCount();
	void mirror();
	void initFromSquares(int input[64], unsigned char next, int fiftyM, int castleW, int castleB, int epSq);
	void display();
	void rememberPV();
	void selectmove(int &ply, int &i, int &depth, BOOLTYPE &followpv); 
	void addCaptScore(int &ifirst, int &index);
	int SEE(Move &move);
	BitMap attacksTo(int &target);
	BitMap revealNextAttacker(BitMap &attackers, BitMap &nonremoved, int &target, int &heading);
	void readClockAndInput();
	bool isTimeOutMate();

};
 
#endif