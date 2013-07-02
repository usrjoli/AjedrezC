#ifndef _CRT_SECURE_NO_DEPRECATE  // suppress MS security warnings
#define _CRT_SECURE_NO_DEPRECATE
#endif
#include <iostream>
#include "defines.h"
#include "protos.h"
#include "extglobals.h"

// TODO:
//- Límites del buffer
//- lista de movimientos en string (usando toSan(board.moveBuffer[i], sanMove) o similar )
//- Verificar si usar isOtherKingAttacked o isOwnKingAttacked

bool isMateInN(int pDepth, int pIndexMoveBufLen, int pTurn, char* pPath);

bool CheckOppTurn(int pDepth, int pIndexMoveBufLen, int pTurn, char* pPath){
	// Check if all opponents moves lead to Mate-in-(N-1)
	int mates = 0;
	int lIndexMoveBufLen, i, ahogado = 0;
	bool fin = false;
	char sanMove[12];
	char lPath[MAX_PATH_MOVES];

	/************ obtengo todos los movimientos posibles del turno actual **********************/
	lIndexMoveBufLen = generarMovimientosPosibles(pIndexMoveBufLen);
	i = board.moveBufLen[lIndexMoveBufLen]; // i = índice donde comienzan los movimientos generados
	/************ FIN obtengo todos los movimientos posibles del turno actual **********************/

	while (!fin && i < board.moveBufLen[lIndexMoveBufLen + 1]){
		// concateno el movimiento del oponente a la lista resultante
		strcpy(lPath, pPath);	
		toSanBuffFrom(board.moveBuffer[i], sanMove, lIndexMoveBufLen + 1);
		strcat(lPath, sanMove);
		strcat(lPath, " ");
		makeMove(board.moveBuffer[i]); // realiza el movimiento del oponente		
		if (!isOtherKingAttacked()){// sí es movimiento legal			
			if(!isMateInN(pDepth -1, lIndexMoveBufLen + 1, pTurn + 1, lPath)){
				// encontre un movimiento del oponente para el cual no hay mate -> finalizo y retorno false
				fin = true;
				if(pTurn == 1){
					board.topeMovesMateInN = -1; // limpio el buffer de movimientos resltantes en mate
				}
			}
		}else {
			ahogado ++;
		}
		unmakeMove(board.moveBuffer[i]);
		i ++;
	}

	if(ahogado == (board.moveBufLen[lIndexMoveBufLen + 1] - board.moveBufLen[lIndexMoveBufLen])){
		return false;
	} 

	return !fin;
}


//Parametros entrada:  paso, distancia, indices del buffer y lista de movimientos resultantes ver!!!
bool isMateInN(int pDepth, int pIndexMoveBufLen, int pTurn, char* pPath){
	// pIndex es el primer lugar libre de board.moveBufLen para comenzar a guardar los movimientos
	int i, lIndexMoveBufLen;
	bool fin = false;
	char sanMove[12], lturn[3];
	char lPath[MAX_PATH_MOVES];


	if(isCheckMateG(pIndexMoveBufLen)){ // recibimos un tablero con mate (el fen ya es mate) TODO ver si en la recursión cae aca
		// cargo la lista y termino
		return true;
	}
	if (pDepth == 0){
		return false;
	}

	/************ obtengo todos los movimientos posibles del turno actual **********************/
	lIndexMoveBufLen = generarMovimientosPosibles(pIndexMoveBufLen);
	i = board.moveBufLen[lIndexMoveBufLen]; // i = índice donde comienzan los movimientos generados
	/************ FIN obtengo todos los movimientos posibles del turno actual **********************/
	
	/************ mientras hayan movimientos posibles o no se haya encontrado un mate ****/
	// TODO se podrían recorrer todos los movimientos posibles sin hacer recursión para ver si hay mate con los primeros movimientos
	while (!fin && i < board.moveBufLen[lIndexMoveBufLen + 1])
	{
		// agrego el número de turno y el movimiento  a la lista resultante
		strcpy(lPath, pPath);	

		itoa(pTurn, lturn,10);
		strcat(lPath, lturn);
		strcat(lPath, ". ");
		toSanBuffFrom(board.moveBuffer[i], sanMove, lIndexMoveBufLen + 1);
		strcat(lPath, sanMove);
		strcat(lPath, " ");
		//displayFullMove(board.moveBuffer[i]);
		makeMove(board.moveBuffer[i]);
		if (!isOtherKingAttacked()){// sí es movimiento legal		
			if(pTurn == 1 && board.moveBuffer[i].getPiec() == WHITE_KING) {
				int algo = 0;}
			if(isCheckMateG(lIndexMoveBufLen + 1)){ // pIndexMoveBufLen + 1 = indice de moveBufLen donde se encuentra la posicion libre de moveBuffer
				// al ejecutar el movimiento hay mate -> para todos los movimientos del oponete hay mate
				fin = true;
				board.topeMovesMateInN++;
				strcpy(board.movesMateInN[board.topeMovesMateInN], lPath); // alamaceno el listado de movimientos que llevan al mate				
			}else{
				fin = CheckOppTurn(pDepth, lIndexMoveBufLen+ 1, pTurn, lPath); // RETORNA TRUE SII PARA TODOS LOS MOVIMIENTOS DEL OPONENTE HAY MATE
			}
		}
		unmakeMove(board.moveBuffer[i]);
		if(!fin){
			i++;			
		}
	}

	if(fin){
		// el movimiento que dio mate se encuentra en i moveBuffer
		return true;
	}else {
		return false;
	}
}

bool mateInN(char* pPathFen, int pNroFen, int pDepth){
	// carga el tablero dado el archivo fen indicado en pPathFen
	
			board.init();
			readFen(pPathFen, pNroFen);
			board.display();

			int i;
			board.topeMovesMateInN = -1;
			char path[MAX_PATH_MOVES];
			path[0]	 = '\0';
			bool ret = isMateInN(pDepth, 0, 1, path);

			if (ret) {
				std::cout << "Se puede hacer un jaque mate en " << pDepth << " movimientos o menos" << std::endl;
			} else {
				std::cout << "No se puede hacer un jaque mate en " << pDepth << " movimientos o menos" << std::endl;
			}
			
			for (i = 0; i <= board.topeMovesMateInN; i++){
				std::cout << board.movesMateInN[i] << std::endl;
			}
}


 
