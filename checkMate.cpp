#ifndef _CRT_SECURE_NO_DEPRECATE  // suppress MS security warnings
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <fstream>
#include <iostream>
#include "defines.h"
#include "protos.h"
#include "extglobals.h"

using namespace std;

bool CheckOppTurn(int pDepth, int pIndexMoveBufLen, int pTurn, char* pPath, bool write_full_path){
	// Check if all opponents moves lead to Mate-in-(N-1)
	int mates = 0;
	int lIndexMoveBufLen, i, ahogado = 0;
	bool fin = false;
	char sanMove[12];
	char lPath[MAX_PATH_MOVES];
	Move maux;
	maux.clear();

	/************ obtengo todos los movimientos posibles del turno actual **********************/
	lIndexMoveBufLen = generarMovimientosPosibles(pIndexMoveBufLen);
	i = board.moveBufLen[lIndexMoveBufLen]; // i = índice donde comienzan los movimientos generados
	/************ FIN obtengo todos los movimientos posibles del turno actual **********************/

	while (!fin && i < board.moveBufLen[lIndexMoveBufLen + 1]){
		// concateno el movimiento del oponente a la lista resultante
		strcpy(lPath, pPath);	
		if(write_full_path){
			toSanBuffFrom(board.moveBuffer[i], sanMove, lIndexMoveBufLen + 1);
			strcat(lPath, sanMove);
			strcat(lPath, " ");
			}
		makeMove(board.moveBuffer[i]); // realiza el movimiento del oponente		
		if (!isOtherKingAttacked()){// sí es movimiento legal			
			if(!isMateInN(pDepth -1, lIndexMoveBufLen + 1, pTurn + 1, lPath, write_full_path, maux)){
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
bool isMateInN(int pDepth, int pIndexMoveBufLen, int pTurn, char* pPath, bool write_full_path, Move& first_move){
	// pIndex es el primer lugar libre de board.moveBufLen para comenzar a guardar los movimientos
	// si hay mate -> write_full_path es para guardar en el buffer todos los movimientos al mate o no
	// si hay mate -> first_move indica el primer movimiento de camino al mate
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
		strcpy(lPath, pPath);
		if(write_full_path){
			// agrego el número de turno y el movimiento  a la lista resultante	
			_itoa(pTurn, lturn,10);
			strcat(lPath, lturn);
			strcat(lPath, ". ");
			toSanBuffFrom(board.moveBuffer[i], sanMove, lIndexMoveBufLen + 1);
			strcat(lPath, sanMove);
			strcat(lPath, " ");
		}else if(pTurn == 1){ // no escribe el camino al mate y es el primer movimiento -> escribo sólo el primer movimiento hacia el mate
			first_move = board.moveBuffer[i];
		}
		//displayFullMove(board.moveBuffer[i]);
		makeMove(board.moveBuffer[i]);
		if (!isOtherKingAttacked()){// sí es movimiento legal		
			if(pTurn == 1 && board.moveBuffer[i].getPiec() == WHITE_KING) {
				int algo = 0;}
			if(isCheckMateG(lIndexMoveBufLen + 1)){ // pIndexMoveBufLen + 1 = indice de moveBufLen donde se encuentra la posicion libre de moveBuffer
				// al ejecutar el movimiento hay mate -> para todos los movimientos del oponete hay mate
				fin = true;
				if(write_full_path){
					board.topeMovesMateInN++;
					strcpy(board.movesMateInN[board.topeMovesMateInN], lPath); // alamaceno el listado de movimientos que llevan al mate				
				}
			}else{
				fin = CheckOppTurn(pDepth, lIndexMoveBufLen+ 1, pTurn, lPath, write_full_path); // RETORNA TRUE SII PARA TODOS LOS MOVIMIENTOS DEL OPONENTE HAY MATE
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

//Parametros entrada:  paso, distancia, indices del buffer y lista de movimientos resultantes ver!!!
bool isMateInNMov(int pDepth, int pIndexMoveBufLen, Move pLastMove){
	// pIndex es el primer lugar libre de board.moveBufLen para comenzar a guardar los movimientos
	bool fin = false;
	char lPath[MAX_PATH_MOVES];

	if(isCheckMateG(pIndexMoveBufLen)){ // recibimos un tablero con mate (el fen ya es mate) TODO ver si en la recursión cae aca
		return true;
	}
	if (pDepth == 0){
		return false;
	}

	board.topeMovesMateInN = -1;				
	lPath[0]	 = '\0';

	makeMove(pLastMove);
	if (!isOtherKingAttacked()){// sí es movimiento legal		
		if(isCheckMateG(pIndexMoveBufLen)){ // pIndexMoveBufLen + 1 = indice de moveBufLen donde se encuentra la posicion libre de moveBuffer
			// al ejecutar el movimiento hay mate -> para todos los movimientos del oponete hay mate
			fin = true;		
		}else{
			fin = CheckOppTurn(pDepth, pIndexMoveBufLen, 0, lPath, false); // RETORNA TRUE SII PARA TODOS LOS MOVIMIENTOS DEL OPONENTE HAY MATE
		}
	}
	unmakeMove(pLastMove);

	if(fin){
		// el movimiento que dio mate se encuentra en i moveBuffer
		return true;
	}else {
		return false;
	}
}

void mateInN(char* pPathFen, int pNroFen, int pDepth){
	// carga el tablero dado el archivo fen indicado en pPathFen
			int i;	
			char fen[100];
			bool whiteToMove = true;
			char path[MAX_PATH_MOVES];
			Move first_move;
			first_move.clear();

			board.init();
			
			fen[0]	 = '\0';
			if(readFenFen(pPathFen, pNroFen, fen, whiteToMove)){
				board.display();
				board.topeMovesMateInN = -1;				
				path[0]	 = '\0';
				bool ret = isMateInN(pDepth, 0, 1, path, true, first_move);

				if (ret) {
					std::cout << "Se puede hacer un jaque mate en " << pDepth << " movimientos o menos" << std::endl;
					std::cout << "Primer movimiento:  " << std::endl;
					displayFullMove(first_move);
				} else {
					std::cout << "No se puede hacer un jaque mate en " << pDepth << " movimientos o menos" << std::endl;
				}

				//abro archivo escritura
				ofstream myfile;
				myfile.open ("resultMateInN.pgn");
				myfile << "[Event \"Problema de Mate a profundidad máxima fija\"]\n";
				myfile << "[White \"Mate en " << pDepth << "\"]\n";
				myfile << "[Black \"Juega el ";
				if(whiteToMove) myfile << "Blanco\"]\n";
				else myfile << "Negro\"]\n";
				
				myfile << "[Result \"";
				if(ret){ // si es posible un mate en N
					if(whiteToMove) myfile << "1-0\"]\n"; // si le toca jugar a las blancas
					else myfile << "0-1\"]\n";
				}else {
					myfile << "*\"]\n";
				}
					
				myfile << "[FEN " << fen << "]\n";

				if(ret) myfile << "[Mate-en-" << pDepth << " \"Verdadero\"]\n\n";
				else myfile << "[Mate-en-" << pDepth << " \"Falso\"]\n\n *\n";

				for (i = 0; i <= board.topeMovesMateInN; i++){
					std::cout << board.movesMateInN[i] << std::endl;
					myfile << board.movesMateInN[i] << "\n";
				}

				//cierro archivo de escritura
				myfile.close();
			}
}


 


