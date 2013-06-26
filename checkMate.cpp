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

bool CheckOppTurn(int pDepth, int pIndexMoveBufLen, bool pArmarLista){
	// Check if all opponents moves lead to Mate-in-(N-1)
	int mates = 0;
	int lIndexMoveBufLen, i;
	bool fin = false;

	//board.moveBufLen[buff0] = posLibre;
	//board.moveBufLen[buff1] = movegen(board.moveBufLen[buff0]); // carga en moveBuffer todos los movimientos posibles del oponente (legales o no)

	/************ obtengo todos los movimientos posibles del turno actual **********************/
	lIndexMoveBufLen = generarMovimientosPosibles(pIndexMoveBufLen);
	i = board.moveBufLen[lIndexMoveBufLen]; // i = índice donde comienzan los movimientos generados
	/************ FIN obtengo todos los movimientos posibles del turno actual **********************/

	while (!fin && i < board.moveBufLen[lIndexMoveBufLen + 1]){
		makeMove(board.moveBuffer[i]); // realiza el movimiento del oponente		
		if (!isOtherKingAttacked()){// sí es movimiento legal
			//board.display();
			// agrego el movimiento del oponente a la lista resultante
			if(pArmarLista){
				board.topeMovesMateInN++; // me muevo al lugar libre
				board.movesMateInN[board.topeMovesMateInN] = board.moveBuffer[i];
			}
			if(!isMateInN(pDepth -1, lIndexMoveBufLen + 1, pArmarLista)){
				// encontre un movimiento del oponente para el cual no hay mate -> finalizo y retorno false
				fin = true;
				// limpiar lista de movimientos resultante
				if(pArmarLista){
					board.topeMovesMateInN--;
				}
			}else {
				// ya tengo una lista de movimientos que termina en jaque -> no armo mas la lista
				pArmarLista = false;
			}
		}
		unmakeMove(board.moveBuffer[i]);
		i ++;
	}

	return !fin;
}


//Parametros entrada:  paso, distancia, indices del buffer y lista de movimientos resultantes ver!!!
bool isMateInN(int pDepth, int pIndexMoveBufLen, bool pArmarLista){
	// pIndex es el primer lugar libre de board.moveBufLen para comenzar a guardar los movimientos
	int i, lIndexMoveBufLen;
	bool fin = false;

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
		//displayFullMove(board.moveBuffer[i]);
		makeMove(board.moveBuffer[i]);
		if (!isOtherKingAttacked()){// sí es movimiento legal		
			//agrego el movimiento a la lista resultante
			if(pArmarLista){
				board.topeMovesMateInN++; // me muevo al lugar libre
				board.movesMateInN[board.topeMovesMateInN] = board.moveBuffer[i];
				//displayFullMove(board.moveBuffer[i]);
				//std::cout << "tope Movimientos: " << board.topeMovesMateInN << std::endl;
			}

			if(isCheckMateG(lIndexMoveBufLen + 1)){ // pIndexMoveBufLen + 1 = indice de moveBufLen donde se encuentra la posicion libre de moveBuffer
				// al ejecutar el movimiento hay mate -> para todos los movimientos del oponete hay mate
				fin = true;
			}else{
				fin = CheckOppTurn(pDepth, lIndexMoveBufLen+ 1, pArmarLista); // RETORNA TRUE SII PARA TODOS LOS MOVIMIENTOS DEL OPONENTE HAY MATE
			}

			if(!fin && pArmarLista){
				board.topeMovesMateInN--; // me muevo al lugar anterior (elimino el movimiento que agregué)
			}
		}
		unmakeMove(board.moveBuffer[i]);
		if(!fin){
			i++;			
		}else {
			pArmarLista = false;
		}
	}

	if(fin){
		// el movimiento que dio mate se encuentra en i moveBuffer
		return true;
	}else {
		return false;
	}
}

 
