//#include "defines.h"
#include "protos.h"
#include "extglobals.h"
#include "board.h"
//#include "timer.h"

bool isCheckMate(){
	int i;
	Move dummy;
	if (!board.isEndOfgame(i, dummy)) {
		std::cout << "todavia quedan jugadas" << std::endl;
		return false;
	} else {
		std::cout << "es Mate" << std::endl;
		return true;
	}
}
bool isCheck(){
	if (isOwnKingAttacked()){
		std::cout << "hay un jaque" << std::endl;
		return true;
	} else {
		std::cout << "no hay jaque" << std::endl;
		return false;
	}
}
void moves(){
	int i;
	Move dummy;
	board.isEndOfgame(i, dummy);
	std::cout << "jugadas posibles para ";
	if (board.nextMove == WHITE_MOVE){
		std::cout << "blancas";
	}else {
		std::cout << "negras";
	}
	std::cout << ": " << i << std::endl;
}
void castles(){
	int i, number;
	Move dummy;
	board.moveBufLen[0] = 0;
	board.moveBufLen[1] = movegen(board.moveBufLen[0]);
	number = 0;
	for (i = board.moveBufLen[0]; i < board.moveBufLen[1]; i++){
		dummy = board.moveBuffer[i];
		if (dummy.isCastle()){
			if (number == 0){ 
				std::cout << "el jugador en ";
				if (board.nextMove == WHITE_MOVE){
					std::cout << "blancas ";
				}else {
					std::cout << "negras ";
				}
				std::cout << "puede realizar enroque ";
			}
			if (dummy.isCastleOO()){
				if (number > 0){
					std::cout << "y ";
				}
				std::cout << "corto";
				number++;
			} else {
					if (number > 0){
					std::cout << "y ";
				}
				std::cout << "largo";
				number++;
			}
		}
	}
	if (number > 0) {
		std::cout << std::endl;
	} else {
		std::cout << "no hay enroques posibles para el jugador en ";
		if (board.nextMove == WHITE_MOVE){
			std::cout << "blancas " << std::endl;;
		}else {
			std::cout << "negras " << std::endl;;
		}
	}
}
void material(){
	std::cout << "valor total de piezas para blancas: " << board.totalWhitePawns + board.totalWhitePieces << std::endl;
	std::cout << "valor total de piezas para negras : " << board.totalBlackPawns + board.totalBlackPieces << std::endl;
}
int duplicatedPositions(bool show, bool blancas){
	int i, j, dupMoves;
	bool dupMSG;
	Move dummy, current;

	//blancas = board.nextMove == WHITE_MOVE;
	dupMSG = false;
	i = 0;
	if (!blancas) {
		i++;
	}
	dupMoves = 0;
	//std::cout << "end of game   : " << board.endOfGame << std::endl;
	//std::cout << "end of search : " << board.endOfSearch << std::endl;
	//GameLineRecord gameLine[MAX_GAME_LINE];
	while (i < board.endOfGame){
		j = i + 2;
		current = board.gameLine[i].move;
		//displayFullMove(current);
		while (j < board.endOfGame){
			dummy = board.gameLine[j].move;
			//displayFullMove(dummy);
			if ((current.getFrom() == dummy.getFrom()) &&
				(current.getTosq() == dummy.getTosq()) &&
				(current.getPiec() == dummy.getPiec()) &&
				(current.isCapture() == dummy.isCapture())){
				if (show){
					if (!dupMSG) {
						std::cout << "Se ha encontrado un movimiento repetido" << std::endl;
						dupMSG = true;
					} else {
						std::cout << "---------------------------" << std::endl;
					}
					displayFullMove(dummy);
				}
				dupMoves++;
			}

			j +=2;
		}

		i += 2;
	}
	if (show){
		std::cout << "Se han encontrado " << dupMoves << " movimientos duplicados." << std::endl;
	}
	return dupMoves;
}

int movSPawn(){
	return board.fiftyMove / 2;
}

int reachableSquares(bool display){
	int i, total, distincts;	
	bool reachables[64];
	Move dummy;
	Move moves[2000];
	
	dummy.clear();
	for (i = 0; i < 64; i++){
		reachables[i] = false;
	}
	total = 0;
	distincts = 0;
	board.moveBufLen[0] = 0;
	board.moveBufLen[1] = movegen(board.moveBufLen[0]);
	for (i = board.moveBufLen[0]; i < board.moveBufLen[1]; i++){
		dummy = board.moveBuffer[i];
		makeMove(dummy);
		if (isOtherKingAttacked()){
			unmakeMove(dummy);
		}
		else {
			unmakeMove(dummy);
			if (!reachables[dummy.getTosq()]) {
				reachables[dummy.getTosq()] = true;
				distincts++;
			}
			moves[total] = dummy;
			total ++;
		}
	}
	if (display) {
		for (i = 0; i < total; i++){
			displayFullMove(moves[i]);
		}
	}
	return distincts;

}