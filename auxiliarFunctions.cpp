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

bool isCheckMateG(int indexBuffer){
	int i;
	Move dummy;
	if (!board.isEndOfgameMate(i, dummy, indexBuffer)) { // no es jaque mate
		return false;
	} else {
		return true;
	}
}

int generarMovimientosPosibles(int pIndexMoveBufLen){
	int posLibre;
	if(pIndexMoveBufLen == 0){
		board.moveBufLen[pIndexMoveBufLen] = 0;
	}else {
		posLibre = board.moveBufLen[pIndexMoveBufLen];
		pIndexMoveBufLen ++;
		board.moveBufLen[pIndexMoveBufLen] = posLibre;
	}
	// carga en moveBuffer todos los movimientos posibles (legales o no) desde pIndex hasta board.moveBufLen[pIndex + 1]
	board.moveBufLen[pIndexMoveBufLen + 1] = movegen(board.moveBufLen[pIndexMoveBufLen]); 
	return pIndexMoveBufLen;
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

void enpassantMoves(){
	int i, j;	
	Move dummy;
	
	j = 0;
	board.moveBufLen[0] = 0;
	board.moveBufLen[1] = movegen(board.moveBufLen[0]);
	for (i = board.moveBufLen[0]; i < board.moveBufLen[1]; i++){
		dummy = board.moveBuffer[i];
		if (dummy.isEnpassant()){
			if (j == 0){
				std::cout << "Capturas al paso: " << std::endl;
			}
			displayFullMove(dummy);
			j++;
		}
	}
	if (j == 0){
		std::cout << "No hay capturas al paso para este tablero" << std::endl;
	}
}

bool checkMateInN(int movesLeft){
	Move toCheckMoves[1024];
	Move *oponentToCheckMoves;
	Move dummy, mov;
	int i, j, number, movsToCheck;
	bool isMate;
	char sanMove[12];

	movesLeft--;
	isMate = false;
	if (movesLeft == 0){//no debo seguir buscando más abajo, recorrer los movimientos posibles y ver si alguno da mate.
		board.moveBufLen[0] = 0;
		board.moveBufLen[1] = movegen(board.moveBufLen[0]);
		
		number = 0;
//		toCheckMoves = new Move[board.moveBufLen[1]];
		j = 0;
		movsToCheck = board.moveBufLen[1];
		for (i = board.moveBufLen[0]; i < movsToCheck; i++){
			toCheckMoves[j] = board.moveBuffer[i];
			toSan(toCheckMoves[j], sanMove);
			std::cout << ++number << ". " << sanMove << " | ";
			j++;
		}
		std::cout << std::endl;
		j = 0;
		for (i = board.moveBufLen[0]; i < movsToCheck; i++){
			toSan(toCheckMoves[j], sanMove);
			std::cout << ++number << ". " << sanMove << " | ";
			makeMove(toCheckMoves[j]);
			board.display();
			if (isOtherKingAttacked()){
				unmakeMove(toCheckMoves[j]);
			} else {
				if (board.isEndOfgame(j, dummy)){
					isMate = true;
				}
				board.moveBufLen[1] = movegen(board.moveBufLen[1]);
				unmakeMove(toCheckMoves[j]);
				toSan(toCheckMoves[j], sanMove);
				std::cout << ++number << ". " << sanMove << " | ";
			}
			j++;
			board.display();
		}
		std::cout << std::endl;
/*		for (i = board.moveBufLen[0];i < board.moveBufLen[1]; i++){
			mov = board.moveBuffer[i];
//////////////////////////////////////////////////////////
			if (board.nextMove == BLACK_MOVE){
				std::cout << "mueven negras"<<std::endl;
			} else {
				std::cout << "mueven blancas"<<std::endl;
			}
//////////////////////////////////////////////////////////
			makeMove(mov);
			board.display();
			if (!isOtherKingAttacked()){
				if (board.isEndOfgame(j, dummy)){
					isMate = true;
				}
			}
//////////////////////////////////////////////////////////
			if (board.nextMove == BLACK_MOVE){
				std::cout << "mueven negras"<<std::endl;
			} else {
				std::cout << "mueven blancas"<<std::endl;
			}
//////////////////////////////////////////////////////////
			unmakeMove(mov);
//////////////////////////////////////////////////////////
			if (board.nextMove == BLACK_MOVE){
				std::cout << "mueven negras"<<std::endl;
			} else {
				std::cout << "mueven blancas"<<std::endl;
			}
//////////////////////////////////////////////////////////
			board.display();
/////////////////////////////////////////////////////////
		}*/
	
	} else {
		std::cout << "Aguanta el pichi papi, que esto no está pronto todavia" << std::endl;
	}
	return isMate;
}