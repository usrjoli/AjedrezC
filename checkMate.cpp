


// Paso Base
//board.moveBufLen[0] = 0;
//board.moveBufLen[1] = movegen(board.moveBufLen[0]); // carga en moveBuffer todos los movimientos posibles (legales o no)
//int mates, i;
//mates = 0;
//bool fin = false;
//while (!fin && i < board.moveBufLen[1])
//{
//	makeMove(board.moveBuffer[i]);
//	if (!isOtherKingAttacked())
//	{// sí es movimiento legal		
//		if(isCheckMateG(board.moveBufLen[1]){// si al ejecutar el movimiento hay mate -> incremento la cantidad de movimientos en mate
//			fin = true				
//		}
//	}
//	unmakeMove(board.moveBuffer[i]);
//	if(!fin) i++;
//}
//
//if(fin){
//	// el movimiento que dio mate se encuentra en i moveBuffer
//}

//--------------------------------------------------------------------
// TODO:
//- Límites del buffer
//- lista de movimientos en string (usando toSan(board.moveBuffer[i], sanMove) o similar )
//- Verificar si usar isOtherKingAttacked o isOwnKingAttacked

bool CheckOppTurn(int pDepth, int pIndexMoveBufLen){
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
			if(!isMateInN(pDepth -1, lIndexMoveBufLen + 1)){
				// encontre un movimiento del oponente para el cual no hay mate -> finalizo y retorno false
				fin = true;
			}
		}
		unmakeMove(board.moveBuffer[i]);
		i ++;
	}

	return !fin;
}


//Parametros entrada:  paso, distancia, indices del buffer y lista de movimientos resultantes ver!!!
bool isMateInN(int pDepth, int pIndexMoveBufLen){
	// pIndex es el primer lugar libre de board.moveBufLen para comenzar a guardar los movimientos
	int i, lIndexMoveBufLen;
	mates = 0;
	bool fin = false;

	if(isCheckMateG(pIndexMoveBufLen){ // recibimos un tablero con mate (el fen ya es mate) TODO ver si en la recursión cae aca
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
		makeMove(board.moveBuffer[i]);
		if (!isOtherKingAttacked()){// sí es movimiento legal		
			if(isCheckMateG(lIndexMoveBufLen + 1){ // pIndexMoveBufLen + 1 = indice de moveBufLen donde se encuentra la posicion libre de moveBuffer
				// al ejecutar el movimiento hay mate -> para todos los movimientos del oponete hay mate
				fin = true
			}else{
				fin = CheckOppTurn(pDepth)) // RETORNA TRUE SII PARA TODOS LOS MOVIMIENTOS DEL OPONENTE HAY MATE
			}
		}
		unmakeMove(board.moveBuffer[i]);
		if(!fin) i++;
	}

	if(fin){
		// el movimiento que dio mate se encuentra en i moveBuffer
		return true;
	}else {
		return false;
	}
}

 
