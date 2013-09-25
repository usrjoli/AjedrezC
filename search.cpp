#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#include <conio.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "defines.h" 
#include "extglobals.h" 
#include "protos.h" 
#include "board.h" 
#include "timer.h" 

Move Board::think()
{
	
	/************** debug ***************/
	char txt[50] = "No. movimiento del juego: ";
	char txt2[50];
	char * res;

	_itoa(board.endOfGame, txt2, 10);
	res = strcat(txt, txt2);
	printDebug(txt);
	/************** fin debug ***********/
//	===========================================================================
//  This is the entry point for search, it is intended to drive iterative deepening 
//  The search stops if (whatever comes first): 
//  - there is no legal move (checkmate or stalemate)
//  - there is only one legal move (in this case we don't need to search)
//  - time is up 
//  - the search is interrupted by the user, or by winboard
//  - the search depth is reached
//	===========================================================================

	double score;
	int legalmoves, currentdepth;
	Move singlemove, first_move;

	char path[MAX_PATH_MOVES];

//	===========================================================================
//	Check if the game has ended, or if there is only one legal move,
//  because then we don't need to search:
//	===========================================================================
	if (isEndOfgame(legalmoves, singlemove)) return NOMOVE;
	if (legalmoves == 1) 
	{
		std::cout << "forced move: "; displayMove(singlemove); std::cout << std::endl; 
		if (XB_MODE && XB_POST) 
		{
			printf("0 0 0 0 "); 
			displayMove(singlemove);
			std::cout << std::endl;
		}
		return singlemove;
	}

//	===========================================================================
//	There is more than legal 1 move, so prepare to search:
//	===========================================================================

	if (XB_MODE) timeControl();

	// The Principal variation (PV) is a sequence of moves that programs consider best and therefore expect to be played
	lastPVLength = 0;
	memset(lastPV, 0 , sizeof(lastPV));
	memset(whiteHeuristics, 0, sizeof(whiteHeuristics));
	memset(blackHeuristics, 0, sizeof(blackHeuristics));
	inodes = 0;
	countdown = UPDATEINTERVAL;
	timedout = false;
	// display console header
	displaySearchStats(1, 0, 0, -1);  
	timer.init();
	msStart = timer.getms();

	//================== chequeo de mateIn N: llamar al ismateinn antes que nada, si ya tenemos una secuencia ganadora, vamos por ahí

	board.topeMovesMateInN = -1;				
	path[0]	 = '\0';
	first_move.clear();

	if(DEPTH_MATE > 0){ // si en el ini se carga 0 -> se deshabilita la búsqueda del mateInN
		if(isMateInN(DEPTH_MATE-DEPTH_MATE_DEC, 0, 1, path, false, first_move)){	
			DEPTH_MATE_DEC++;
			return first_move;
		}else {
			std::cout << "------------ NO MATE in N ------------" << std::endl;
		}
	}

// ============== FIN chequeo de mateIn N

	//  iterative deepening:
	for (currentdepth = 1; currentdepth <= searchDepth; currentdepth++)
	{
		//  clear the buffers:
		memset(moveBufLen, 0, sizeof(moveBufLen));
		memset(moveBuffer, 0, sizeof(moveBuffer));
		memset(triangularLength, 0, sizeof(triangularLength));
		memset(triangularArray, 0, sizeof(triangularArray));
		followpv = true;
		allownull = true;

		score = alphabetapvs(0, currentdepth, -LARGE_NUMBER, LARGE_NUMBER);
		
		//score = minimax(0, currentdepth, true); // carga triangularArray
		
		// now check if time is up
		// if not decide if it makes sense to start another iteration:
		if (timedout) 
		{
			std::cout << std::endl;
			return (lastPV[0]);
		}
		else
		{
			if (!XB_NO_TIME_LIMIT)
			{
				msStop = timer.getms();
				if ((msStop - msStart) > (STOPFRAC * maxTime)) 
				{
					if (!XB_MODE) std::cout << "    ok" << std::endl;
					return (lastPV[0]);
				}
			}
		}
		displaySearchStats(2, currentdepth, -1, score);
		// stop searching if the current depth leads to a forced mate:
		if ((score > (CHECKMATESCORE-currentdepth)) || (score < -(CHECKMATESCORE-currentdepth))) 
			currentdepth = searchDepth;
	}
	return (lastPV[0]);
}

double Board::alphabetapvs(int ply, int depth, double alpha, double beta)
{
	// PV search

	int i, j, movesfound, pvmovesfound;
	double val;

	triangularLength[ply] = ply;
	if (depth <= 0) 
	{
		followpv = false;
		return qsearch(ply, alpha, beta);
	}

	// repetition check:
	if (repetitionCount() >= 3) return DRAWSCORE;
	
	// now try a null move to get an early beta cut-off:
	if (!followpv && allownull)
	{
		if ((nextMove && (board.totalBlackPieces > NULLMOVE_LIMIT)) || (!nextMove && (board.totalWhitePieces > NULLMOVE_LIMIT)))
		{
			if (!isOwnKingAttacked())
			{
				allownull = false;
				inodes++;
				if (--countdown <=0) readClockAndInput();
				nextMove = !nextMove;
				hashkey ^= KEY.side; 
				val = -alphabetapvs(ply, depth - NULLMOVE_REDUCTION, -beta, -beta+1);
				nextMove = !nextMove;
				hashkey ^= KEY.side;
				if (timedout) return 0;
				allownull = true;
				if (val >= beta) return val;
			}
		}
	}
	allownull = true;

	movesfound = 0;
	pvmovesfound = 0;
	moveBufLen[ply+1] = movegen(moveBufLen[ply]);
	for (i = moveBufLen[ply]; i < moveBufLen[ply+1]; i++)
	{
		selectmove(ply, i, depth, followpv); 
		makeMove(moveBuffer[i]);
		{
			if (!isOtherKingAttacked()) 
			{
				inodes++;
				if (--countdown <=0) readClockAndInput();
				movesfound++;
				if (!ply) displaySearchStats(3, ply, i, -1); 
				if (pvmovesfound)
				{
					val = -alphabetapvs(ply+1, depth-1, -alpha-1, -alpha); 
		            if ((val > alpha) && (val < beta))
					{
						 // in case of failure, proceed with normal alphabeta
						val = -alphabetapvs(ply+1, depth - 1, -beta, -alpha);  		        
					}
				} 
				// normal alphabeta
	 			else val = -alphabetapvs(ply+1, depth-1, -beta, -alpha);	    
				unmakeMove(moveBuffer[i]);
				if (timedout) return 0;
				if (val >= beta)
				{
					// update the history heuristic
					if (nextMove) 
						blackHeuristics[moveBuffer[i].getFrom()][moveBuffer[i].getTosq()] += depth*depth;
					else 
						whiteHeuristics[moveBuffer[i].getFrom()][moveBuffer[i].getTosq()] += depth*depth;
					return beta;
				}
				if (val > alpha)
				{
					alpha = val;								    // both sides want to maximize from *their* perspective
					pvmovesfound++;
					triangularArray[ply][ply] = moveBuffer[i];					// save this move
					for (j = ply + 1; j < triangularLength[ply+1]; j++) 
					{
						triangularArray[ply][j] = triangularArray[ply+1][j];	// and append the latest best PV from deeper plies
					}
					triangularLength[ply] = triangularLength[ply+1];
					if (!ply) displaySearchStats(2, depth, -1, val);
				}
			}
			else unmakeMove(moveBuffer[i]);
		}
	}

	// update the history heuristic
	if (pvmovesfound)
	{
		if (nextMove) 
			blackHeuristics[triangularArray[ply][ply].getFrom()][triangularArray[ply][ply].getTosq()] += depth*depth;
		else
			whiteHeuristics[triangularArray[ply][ply].getFrom()][triangularArray[ply][ply].getTosq()] += depth*depth;
	}

	//	50-move rule:
	if (fiftyMove >= 100) return DRAWSCORE;

	//	Checkmate/stalemate detection:
	if (!movesfound)
	{
		if (isOwnKingAttacked())  return (-CHECKMATESCORE+ply-1);
		else  return (STALEMATESCORE);
	}

	return alpha;
}

int Board::alphabeta(int ply, int depth, int alpha, int beta)
{
	// Negascout

	int i, j, val;

	triangularLength[ply] = ply;
	if (depth == 0) return board.eval();

	moveBufLen[ply+1] = movegen(moveBufLen[ply]);
	for (i = moveBufLen[ply]; i < moveBufLen[ply+1]; i++)
	{
		makeMove(moveBuffer[i]);
		{
			if (!isOtherKingAttacked()) 
			{
				inodes++;
				if (!ply) displaySearchStats(3, ply, i, -1); 
				val = -alphabeta(ply+1, depth-1, -beta, -alpha);
				unmakeMove(moveBuffer[i]);
				if (val >= beta)
				{
					return beta;
				}
				if (val > alpha) 
				{
					alpha = val;												// both sides want to maximize from *their* perspective
					triangularArray[ply][ply] = moveBuffer[i];					// save this move
					for (j = ply + 1; j < triangularLength[ply + 1]; j++) 
					{
						triangularArray[ply][j] = triangularArray[ply+1][j];	// and append the latest best PV from deeper plies
					}
					triangularLength[ply] = triangularLength[ply + 1];
					if (!ply) displaySearchStats(2, depth, -1, val);
				}
			}
			else unmakeMove(moveBuffer[i]);
		}
	}
	return alpha;
}

double Board::minimax(int ply, int depth, bool inicio)
//inicio es para indicar que es la primer invocación, así hacemos el mate in
{
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// intenta minimizar la ganancia del rival, o sea busca que el rival tenga el peor resultado
	// Pasos del algoritmo Minimax:
	//	1. Generación del árbol de juego. Se generarán todos los nodos hasta llegar a un estado terminal.
	//	2. Cálculo de los valores de la función de utilidad para cada nodo terminal (funcion eval)
	//	3. Calcular el valor de los nodos superiores a partir del valor de los inferiores. 
	//		Según el nivel si es MAX o MIN se elegirán los valores mínimos y máximos representando los movimientos del jugador y del oponente, de ahí el nombre de Minimax.
	//	4. Elegir la jugada valorando los valores que han llegado al nivel superior.
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// Negamax: Usually the Negamax algorithm is used for simplicity. (max(a, b) == -min(-a, -b))
	// This means that the evaluation of a position is equivalent to the negation of the evaluation from the opponent's viewpoint.
	// This is because of the zero-sum property of chess: one side's win is the other side's loss.
	// Important!!: In order for negaMax to work, your Static Evaluation function must return a score relative to the side to being evaluated.
	//				(e.g. the simplest score evaluation could be: score = materialWeight * (numWhitePieces - numBlackPieces) * who2move 
	//				where who2move = 1 for white, and who2move = -1 for black).

	bool isMateInN = false;
	int i, j;
	double val, best;
	int a1 = 1, a2 = 0, a3 = 0; // parametros para la función de evaluación. TODO AGREGARLO AL archivo wingletx.ini
	best = -LARGE_NUMBER;
	triangularLength[ply] = ply;

	if (depth == 0) return board.evalJL(PARAM_EVAL_MATERIAL, PARAM_EVAL_ESPACIAL, PARAM_EVAL_DINAMICA, ply);
	//if (depth == 0) return board.eval();

	moveBufLen[ply+1] = movegen(moveBufLen[ply]);  

	for (i = moveBufLen[ply]; i < moveBufLen[ply+1]; i++){
		// MateIn para el movimiento a evaluar
		if (inicio){
			isMateInN = isMateInNMov(DEPTH_MATE, ply+1, moveBuffer[i]);
		} else {
			/*if (depth == DEPTH_MATE){
				isMateInN = isMateInNMov(DEPTH_MATE, ply+1, moveBuffer[i]);
			}
			else {*/
				isMateInN = false;
			//}
		}

		makeMove(moveBuffer[i]);
		{
			if (!isOtherKingAttacked()){
				inodes++;
				if (!ply) displaySearchStats(3, ply, i, -1);
				if(isMateInN){
					// si al ejecutar el movimiento moveBuffer[i] hay mateInN -> le doy el mayor valor posible al movimiento
					val = LARGE_NUMBER; // TODO cortar el for (i = moveBufLen[ply]; i < moveBufLen[ply+1]; i++){
				}else {
					val = -minimax(ply+1, depth-1, false); // note the minus sign
				}
				unmakeMove(moveBuffer[i]);
				if (val > best){                                            // both sides want to maximize from *their* perspective
					best = val;
					triangularArray[ply][ply] = moveBuffer[i];					// save this move
					for (j = ply + 1; j < triangularLength[ply + 1]; j++){
						triangularArray[ply][j] = triangularArray[ply+1][j];	// and append the latest best PV (Principal Variation) from deeper plies
																				// The Principal variation (PV) is a sequence of moves that programs consider best and therefore expect to be played
					}
					triangularLength[ply] = triangularLength[ply + 1];
					if (!ply)  displaySearchStats(2, depth, -1, val);
				}
			}
			else unmakeMove(moveBuffer[i]);
		}
	}
	return best;
}

void Board::displaySearchStats(int mode, int depth, int score, double scoreD)
{
	// displays various search statistics
	// only to be used when ply = 0
	// mode = 1 : display header
	// mode = 2 : display full stats, including score and latest PV
	// mode = 3 : display current root move that is being searched
	//			  depth = ply, score = loop counter in the search move list 

	char sanMove[12], timestring[8];
	U64 dt;

	dt = timer.getms() - msStart;
	switch (mode)
	{
		case 1: 
				if (!XB_MODE) std::cout << "depth  score   nodes     time  knps PV" << std::endl;
				break;

		case 2:
				if (XB_MODE && XB_POST)
				{
					printf("%5d %6d %8d %9d ", depth, scoreD, dt/10, inodes);
					rememberPV();
					displayPV();
				}
				else
				{
					// depth
					printf("%5d ", depth);

					// score
					printf("%+6.2f ", float(scoreD/100.0));

					// nodes searched
					if      (inodes > 100000000) printf("%6.0f%c ", float(inodes/1000000.0), 'M');
					else if (inodes > 10000000)	 printf("%6.2f%c ", float(inodes/1000000.0), 'M');
					else if (inodes > 1000000)	 printf("%6.0f%c ", float(inodes/1000.0),    'K');
					else if (inodes > 100000)	 printf("%6.1f%c ", float(inodes/1000.0),    'K');
					else if (inodes > 10000)	 printf("%6.2f%c ", float(inodes/1000.0),    'K');
					else						 printf("%7d ", inodes);

					// search time
					mstostring(dt, timestring);
					printf("%8s ", timestring);

					// search speed
					if (dt > 0) std::cout << std::setw(5) << (inodes/dt) << " ";
					else		std::cout << "    - ";

					// store this PV:
					rememberPV();

					// display the PV
					displayPV();
				}
				break;

		case 3: // Note that the numbers refer to pseudo-legal moves:
				if (!TO_CONSOLE) break;
				if (XB_MODE)
				{

				}
				else
				{
					mstostring(dt, timestring);
					printf("             (%2d/%2d) %8s       ", score+1, moveBufLen[depth+1]-moveBufLen[depth], timestring);
					unmakeMove(moveBuffer[score]);
					toSan(moveBuffer[score], sanMove);
					std::cout << sanMove;
					makeMove(moveBuffer[score]);
					printf("...    \r");
					std::cout.flush();
				}
				break;

		default: break;
	}

	return;
}

BOOLTYPE Board::isEndOfgame(int &legalmoves, Move &singlemove)
{
	// Checks if the current position is end-of-game due to:
	// checkmate, stalemate, 50-move rule, or insufficient material

	int whiteknights, whitebishops, whiterooks, whitequeens, whitetotalmat;
	int blackknights, blackbishops, blackrooks, blackqueens, blacktotalmat;
	int i;

	// are we checkmating the other side?
	if (isOtherKingAttacked()) 
	{
		if (nextMove) std::cout << "0-1 {Jaque Mate Negro}" << std::endl;
		else std::cout << "1-0 {Jaque Mate Blanco}" << std::endl;
		return true;
	}

	// how many legal moves do we have?
	legalmoves = 0;
	moveBufLen[0] = 0;
	moveBufLen[1] = movegen(moveBufLen[0]);
	for (i = moveBufLen[0]; i < moveBufLen[1]; i++)
	{
		makeMove(moveBuffer[i]);
		if (!isOtherKingAttacked())
		{ 
			legalmoves++;
			singlemove = moveBuffer[i];
		}
		unmakeMove(moveBuffer[i]);
	}

	// checkmate or stalemate?
	if (!legalmoves) 
	{
		if (isOwnKingAttacked()) 
		{
			if (nextMove) std::cout << "1-0 {Jaque Mate Blanco}" << std::endl;
			else std::cout << "0-1 {Jaque Mate Negro}" << std::endl;
		}
		else std::cout << "1/2-1/2 {stalemate}" << std::endl;
		return true;
	}

	// draw due to insufficient material:
    if (!whitePawns && !blackPawns)
	{
		whiteknights = bitCnt(whiteKnights);
		whitebishops = bitCnt(whiteBishops);
		whiterooks = bitCnt(whiteRooks);
		whitequeens = bitCnt(whiteQueens);
		whitetotalmat = 3 * whiteknights + 3 * whitebishops + 5 * whiterooks + 10 * whitequeens;
		blackknights = bitCnt(blackKnights);
		blackbishops = bitCnt(blackBishops);
		blackrooks = bitCnt(blackRooks);
		blackqueens = bitCnt(blackQueens);
		blacktotalmat = 3 * blackknights + 3 * blackbishops + 5 * blackrooks + 10 * blackqueens;

		// king versus king:
		if ((whitetotalmat == 0) && (blacktotalmat == 0)) 
		{
			std::cout << "1/2-1/2 {material}" << std::endl;
			return true;
		}
 
		// king and knight versus king:
		if (((whitetotalmat == 3) && (whiteknights == 1) && (blacktotalmat == 0)) ||
                    ((blacktotalmat == 3) && (blackknights == 1) && (whitetotalmat == 0))) 
		{
			std::cout << "1/2-1/2 {material}" << std::endl;
			return true;
		}
 
        // 2 kings with one or more bishops, all bishops on the same colour:
		if ((whitebishops + blackbishops) > 0)
		{
			if ((whiteknights == 0) && (whiterooks == 0) && (whitequeens == 0) &&
				(blackknights == 0) && (blackrooks == 0) && (blackqueens == 0))
			{
				if (!((whiteBishops | blackBishops) & WHITE_SQUARES) ||
                !((whiteBishops | blackBishops) & BLACK_SQUARES))
				{
					std::cout << "1/2-1/2 {material}" << std::endl;
					return true;
				}
			}
		}
	}

	// draw due to repetition:
	if (repetitionCount() >= 3) 
	{
		std::cout << "1/2-1/2 {repetition}" << std::endl;
		return true;

	}

	// draw due to 50 move rule:
	if (fiftyMove >= 100) 
	{
		std::cout << "1/2-1/2 {50-move rule}" << std::endl;
		return true;
	}

	return false;
}

BOOLTYPE Board::isEndOfgameMate(int &legalmoves, Move &singlemove, int pIndex)
{
	// Checks if the current position is end-of-game to:
	// checkmate

	//int whiteknights, whitebishops, whiterooks, whitequeens, whitetotalmat;
	//int blackknights, blackbishops, blackrooks, blackqueens, blacktotalmat;
	int i;

	// are we checkmating the other side?
	if (isOtherKingAttacked()){
		return true;
	}

	// how many legal moves do we have?
	legalmoves = 0;
	//moveBufLen[0] = 0;
	//moveBufLen[1] = movegen(moveBufLen[0]);
	//for (i = moveBufLen[0]; i < moveBufLen[1]; i++)

	/*moveBufLen[pIndex] = pIndex;
	moveBufLen[pIndex + 1] = movegen(moveBufLen[pIndex]);*/
	
	/************ obtengo todos los movimientos posibles del turno actual **********************/
	int lIndexMoveBufLen = generarMovimientosPosibles(pIndex);
	/************ FIN obtengo todos los movimientos posibles del turno actual **********************/

	// i = índice donde comienzan los movimientos generados
	for (i = board.moveBufLen[lIndexMoveBufLen]; i < board.moveBufLen[lIndexMoveBufLen + 1]; i++){
		makeMove(moveBuffer[i]);
		if (!isOtherKingAttacked()){ 
			legalmoves++;
			singlemove = moveBuffer[i];
		}
		unmakeMove(moveBuffer[i]);
	}

	// checkmate or stalemate?
	if (!legalmoves){
		if (isOwnKingAttacked()){
			return true;
		}
	}

	return false;
}

int Board::repetitionCount()
{
	//	repetitionCount is used to detect threefold repetitions of the current position

	int i, ilast, rep;
	rep = 1;                                              // current position has occurred once, namely now!
	ilast = board.endOfSearch - board.fiftyMove;          // we don't need to go back all the way
	for (i = board.endOfSearch - 2; i >= ilast; i -= 2)   // we can skip every other position
	{
		if (gameLine[i].key == board.hashkey) rep++;
	}
	return rep;
}

void Board::rememberPV()
{
	// remember the last PV, and also the 5 previous ones because 
	// they usually contain good moves to try
	int i;
	lastPVLength = board.triangularLength[0];
	for (i = 0; i < board.triangularLength[0]; i++)
	{
		lastPV[i] = board.triangularArray[0][i];
	}
}

void mstostring(U64 dt, char *timestring)
{

	// convert milliseconds to a time string (hh:mm:ss, mm:ss, s, ms)

	U64 hh, mm, ss;

	if (dt > 3600000) 
	{      
		hh = dt/3600000;
		mm = (dt - 3600000*hh)/60000;
		ss = (dt - 3600000*hh - 60000*mm)/1000;
		sprintf(timestring, "%02d:%02d:%02d", hh, mm, ss);
	}
	else if (dt > 60000) 
	{      
		mm = dt/60000;
		ss = (dt - 60000*mm)/1000;
		sprintf(timestring, "%02d:%02d", mm, ss);
	}
	else if (dt > 10000)		sprintf(timestring, "%6.1f%s", float(dt/1000.0), "s");
	else if (dt > 1000)			sprintf(timestring, "%6.2f%s", float(dt/1000.0), "s");
	else if (dt > 0)			sprintf(timestring, "%5dms", dt);
	else						sprintf(timestring, "0ms");

	return;

}

void timeControl()
{

//	===========================================================================
//	This routine is used to calculate maxTime, the maximum time for this move 
//	in millisceonds. Based on:
//	XB_CTIM = computer's time, milliseconds
//	XB_OTIM = opponents' time, millseconds
//	XB_INC = time increment, milliseconds
//	===========================================================================
	int xb_ctim, movesLeft;

//	===========================================================================
//	First build in a safety buffer of 2000 milliseconds:
//	===========================================================================
	xb_ctim = XB_CTIM - 2000;
	if (xb_ctim < 1) xb_ctim = 1;

//	===========================================================================
//	Estimate the number of moves per side that are left. Assume 80 half moves 
//	per game with a minimum of 10 half moves left to play, no matter how many moves are played:
//	===========================================================================
	movesLeft = 80 - board.endOfSearch;
	if (movesLeft < 20) movesLeft = 20;

	#ifdef WINGLET_DEBUG_WINBOARD
		std::cout << "#-winglet : in timecontrol, movesleft=" << movesLeft << std::endl;
	#endif
//	===========================================================================
//	Use up part of the thinking time advantage that we may have:
//	===========================================================================
	if ((XB_OTIM + XB_INC) < xb_ctim)
		board.maxTime = (xb_ctim / movesLeft) + XB_INC + (int)(0.80*(xb_ctim - XB_OTIM - XB_INC)); 
	else
		board.maxTime = (xb_ctim / movesLeft);


//	===========================================================================
//	If an XB_INC is defined, then there is no reason to run out of time:
//	===========================================================================
	if ((XB_INC) && (xb_ctim < XB_INC)) board.maxTime = xb_ctim;

//	===========================================================================
//	Final checks, all moves should be > something:
//	===========================================================================
	if (board.maxTime > xb_ctim) board.maxTime = (int)(0.8 * xb_ctim);
	if (board.maxTime < 1) board.maxTime = 1;

	#ifdef WINGLET_DEBUG_WINBOARD
		std::cout << "#-winglet : in timecontrol, XB_CTIM=" << XB_CTIM << std::endl;
		std::cout << "#-winglet : in timecontrol, XB_OTIM=" << XB_OTIM << std::endl;
		std::cout << "#-winglet : in timecontrol, XB_INC =" << XB_INC << std::endl;
		std::cout << "#-winglet : in timecontrol, maxtime=" << board.maxTime << std::endl;
	#endif
	
	return;
}