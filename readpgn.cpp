// jose
// módulo creado para leer archivos PGN

#ifndef _CRT_SECURE_NO_DEPRECATE  // suppress MS security warnings
#define _CRT_SECURE_NO_DEPRECATE
#endif
#include <iostream>
#include "defines.h"
#include "protos.h"
#include "extglobals.h"

unsigned int obtenerPosOrigen(int row_from, int col_from, unsigned int mov_to, unsigned int pieza){
/**
* obtengo todos los movimientos posibles del tablero, los recorro y cuando encuentro uno con posFinal = a la que busco y su pieza es la que busco 
* -> retorno ese mov inicial
*
**/
	unsigned int res;
	Move movimiento;
	int i, mov_col, mov_row;
	bool fin;

	i = 0;
	res = 0;
	//copiado del comando "moves" que viene de origen.
	/*board.moveBufLen[0] = 0;
	board.moveBufLen[1] = movegen(board.moveBufLen[0]);*/

	fin = false;
	movimiento.clear();
	if ((row_from != -1) && (col_from != -1)){
		return (row_from + (8 * col_from));
	}

	i = board.moveBufLen[0];
	while ((i < board.moveBufLen[1]) && (!fin)) {
		movimiento = board.moveBuffer[i];
		mov_col = movimiento.getFrom() % 8;
		mov_row = ((movimiento.getFrom() / 8) - mov_col) * 8;
		
		if(movimiento.getPiec() == 9 || movimiento.getTosq() == 30){
			int algo = 2;
		}
		if ((movimiento.getTosq() == mov_to) && (pieza == movimiento.getPiec())) {
 /*************************  ya se que tengo a la pieza correcta, entonces solo calcular las columnas ******************************************/
			if ((row_from == -1) && (col_from == -1)){
				fin = true;
			} else if ((row_from == -1) && (mov_col == col_from)){ //tengo la columna			
				fin = true;
			} else if ((col_from == -1) && (mov_row == row_from)){ //tengo la fila
				fin = true;
			}
		} else {//no se si va algo, pero por las dudas lo pongo
		
		}
		if (!fin){
			i++;
		}
	}
	if (fin){
		return movimiento.getFrom();
	} else {
		return 0;
	}
}

Move obtenerMovimientoEnroque(BOOLTYPE corto, bool blancas, char * msg_error){
	/**
     * corto = true para enroque corto y false para enroque largo
	 * blancas = true para pieza blanca
     * mov = movimiento incluyendo pieza y color
	 * msg_error si hubo error contiene el msg de error y sino NULL
	 * retorna el mov con las posiciones iniciales y finales de movimiento correspondiente al enroque
	 * TODO validar que el rey y la torre estan en las posiciones correctas para realizar el enroque
	 **/
	bool fin;
	int i;
	Move movimiento;

	fin = false;
	board.moveBufLen[0] = 0;
	board.moveBufLen[1] = movegen(board.moveBufLen[0]);
	i = board.moveBufLen[0];
	movimiento.clear();
	while ((i < board.moveBufLen[1]) && (!fin)) {
		movimiento = board.moveBuffer[i];
		if (movimiento.isCastle()) {
			if (movimiento.isCastleOO() == corto){
				fin = true;
			}else {
				i++;
			}
		}else {
			i++;
		}
	}

	if(!fin){ // si no encontre ningun movimiento correspondiente a un enroque
		strcpy (msg_error,"No se pudo decodificar un enroque");
	}

	return movimiento;
}

Move obtenerMovimiento(char str[180], bool blancas, char* msg_error){
/**
* retorna el movimiento incluyendo: tipo de pieza, color, posición origen (en caso de ambiguedad) y posición destino
* str: texto con un movimiento
* blancas: true si el movimiento corresponde al jugador blanco
* msg_error: NULL, si hay error retornará el msg correspondiente
**/

	Move mov;
	char primera;
	char str_mov[10]; // movimiento sin nombre de piezas
	unsigned int i, j, mov_to, mov_from,  col_from, row_from, posEnroqueCorto, posEnroqueLargo;
	char str_res[10];
	mov.clear();
	primera = str[0];
	
	strcpy(str_mov, "");
    strncat(str_mov, str, strlen(str));

	/*************************  si el movimiento es un enroque --> obtengo posiciones origen y destino del movimiento ****************************/

	posEnroqueLargo = find(str_mov, std::string("O-O-O"));
	if (posEnroqueLargo!=std::string::npos){ // es enroque largo
		return obtenerMovimientoEnroque(false, blancas, msg_error);
	}
	
	posEnroqueCorto = find(str_mov, std::string("O-O"));
	if (posEnroqueCorto!=std::string::npos){ // es enroque corto
		return obtenerMovimientoEnroque(true, blancas, msg_error);
	}
	
	/*********************************************  el movimiento NO es un enroque -> calculo posiciones origen y destino del movimiento *********/
		
	/*********************************************  determino el tipo de pieza que se mueve ******************************************************/
	if ((primera >= 'a') && (primera <= 'z')){ //si es un peón
		if (blancas) mov.setPiec(WHITE_PAWN);
		else mov.setPiec(BLACK_PAWN);
	}else { // es otra pieza != peón 
		// si no es peón -> elimino la primer letra correspondiente a la pieza
		strcpy(str_mov, str_mov+1);
	
		if (primera == 'R') {
			if (blancas) mov.setPiec(WHITE_ROOK);
			else mov.setPiec(BLACK_ROOK);
		}
		if (primera == 'N') {
			if (blancas) mov.setPiec(WHITE_KNIGHT);
			else mov.setPiec(BLACK_KNIGHT);
		}
		if (primera == 'B') {
			if (blancas) mov.setPiec(WHITE_BISHOP);
			else mov.setPiec(BLACK_BISHOP);
		}
		if (primera == 'Q') {
			if (blancas) mov.setPiec(WHITE_QUEEN);
			else mov.setPiec(BLACK_QUEEN);
		}
		if (primera == 'K') { 
			if (blancas) mov.setPiec(WHITE_KING);
			else mov.setPiec(BLACK_KING);
		}
	}
	
	i = 0;
	j = 0;
	// obtengo en str_res sólo el movimiento -> elimino los caracteres que no sean casilleros (por ej. x, =, O-O, O-O-O, etc)
	while (i < strlen(str_mov)){
		if (((str_mov[i] >= 'a') && (str_mov[i] <= 'h')) || ((str_mov[i] >= '1') && (str_mov[i] <= '8'))){
			str_res[j] = str_mov[i];
			j++;

		}
		i++;
	}
	str_res[j] = '\0';
	/*************************************  Decodifico el string ingresado en un movimiento  ***********************************************
	* si largo de str_mov == 2 -> sin ambiguedad
	* si largo de str_mov == 3 -> abmiguedad obtener el origen y ver cual corresponde a la columna o fila indicada en str_mov
	* si largo de str_mov == 4 -> ambiguedad y se tiene el origen en str_mov
	**/

	/* AMBIGUEDADES 
	When two (or more) identical pieces can move to the same square, the moving piece is uniquely identified by specifying the piece's letter, followed by (in descending order of preference)
	1- the file of departure (if they differ); or
	2- the rank of departure (if the files are the same but the ranks differ); or
	3- both the file and rank (if neither alone is sufficient to identify the piece—which occurs only in rare cases where one or more pawns have promoted, resulting in a player having three or more identical pieces able to reach the same square).
	Ex:
	- 2 knights on g1 and d2, either of which might move to f3, the move is specified as Ngf3 or Ndf3
	- 2 knights on g5 and g1, the moves are N5f3 or N1f3
	- an "x" can be inserted to indicate a capture, for example: N5xf3
	- 2 rooks on d3 and h5, either one of which may move to d5. If the rook on d3 moves to d5,
		it is possible to disambiguate with either Rdd5 or R3d5, but the file takes precedence over the rank, so Rdd5 is correct. (And likewise if the move is a capture, Rdxd5 is correct.)
	*/
	
	switch ( strlen(str_res) ) {
	case 2: // sin ambiguedad -> no hay pos origen
		if (str_res[0] < 'a' || str_res[0] > 'h' || str_res[1] < '1' || str_res[1] > '8') {
			strcpy (msg_error,"No se pudo decodificar la posición destino: "); strcat (msg_error,str_res);
		}

		mov_to = str_res[0] - 97; 
		mov_to += 8 * (str_res[1] - 49); 
		mov_from = obtenerPosOrigen(-1, -1, mov_to, mov.getPiec());

		break;
	case 3: // abmiguedad obtener el origen y ver cual corresponde a la columna o fila indicada en str_mov
		if (str_res[0] >= 'a' && str_res[0] <= 'h') { // tengo la columna y no la fila -> debo buscar la fila
			col_from   = str_res[0] - 97; //7 - (str_res[0] - 'a'); // TODO OJO VER LA FORMULA
			row_from   = -1;
		} else if (str_res[0] >= '1' && str_res[0] <= '8') { // tengo la fila y no la columna -> debo buscar la columna
			col_from   = -1;
			row_from   = str_res[1] - 49; //(str_res[0] - '1'); // TODO OJO VER LA FORMULA
		} else {
			strcpy (msg_error,"No se pudo decodificar la posición destino: "); strcat (msg_error,str_res);
		}
		if (str_res[1] < 'a' || str_res[1] > 'h' || str_res[2] < '1' || str_res[2] > '8') {
			strcpy (msg_error,"No se pudo decodificar la posición destino: "); strcat (msg_error,str_res);
		}
		mov_to = str_res[1] - 97; // TODO OJO VER LA FORMULA
		mov_to += 8 * (str_res[2] - 49); // TODO OJO VER LA FORMULA
		mov_from = obtenerPosOrigen(row_from, col_from, mov_to, mov.getPiec());

		break;
	case 4: // ambiguedad y se tiene el origen y el destino
		if (str_res[0] < 'a' || str_res[0] > 'h' || str_res[1] < '1' || str_res[1] > '8' || str_res[2] < 'a' || str_res[2] > 'h' || str_res[3] < '1' || str_res[3] > '8') {
			strcpy (msg_error,"No se pudo decodificar la posición destino: "); strcat (msg_error,str_res);
		}
		mov_from = str_res[0] - 97; // TODO OJO VER LA FORMULA
		mov_from += 8 * (str_res[1] - 49); // TODO OJO VER LA FORMULA
		
		mov_to = str_res[2] - 97;
		mov_to += 8 * (str_res[3] - 49);
		break;
	default:
		strcpy (msg_error,"No se pudo decodificar la posición destino: "); strcat (msg_error,str_res);
		break;
	}

	//seteo posición origne y destino al movimiento
	mov.setFrom(mov_from);
	mov.setTosq(mov_to);

	
	/**************************************************** Busco si hay promoción ****************************************************************/

	i = find(str_mov, std::string("="));
	if (i!=std::string::npos) { // si hay promoción -> obtengo en que pieza se transforma el peón
		i++; // en i queda la letra correspondiente a la promoción
		if (blancas) {
			switch (str_mov[i]){
			case 'Q':  mov.setProm(WHITE_QUEEN);
				break;
			case 'R':  mov.setProm(WHITE_ROOK);
				break;
			case 'B':  mov.setProm(WHITE_BISHOP);
				break;
			case 'N':  mov.setProm(WHITE_KNIGHT);
				break;
			default : break;
			}
		}else{
			switch (str_mov[i]){
				case 'Q':  mov.setProm(BLACK_QUEEN);
					break;
				case 'R':  mov.setProm(BLACK_ROOK);
					break;
				case 'B':  mov.setProm(BLACK_BISHOP);
					break;
				case 'N':  mov.setProm(BLACK_KNIGHT);
					break;
				default : break;
			}
		}
	}

	std::cout << "Pieza: " << mov.getPiec() << std::endl;
	return mov;
}

void readTags(char pS[180], FILE *fp){
	// lee todas las etiquetas del PGN
	// supone que las etiquetas son correctas y son del tipo: [atributo "valor"]

	bool termine = false;
	int largo = (int)strlen(pS);
	char atributo[80];
	char valor[80];

	while(pS[0] == '['){ // mientras sea una etiqueta
		// obtengo el atributo
		// elimino el primer ([) de s:
        strcpy(atributo, "");
        strncat(atributo, pS, strlen(pS));
        strcpy(atributo, atributo+1);
		std::cout << "atributo: " << atributo << std::endl;
		
		// obtengo el valor
		fscanf(fp, "%s", pS);
		// TODO considerar los espacios entre "" del valor
		/*if (pS[largo-1] == ']'){
			termine = true;
			std::cout<<  "debera liquidar aca." << std::endl;
		}else{
			if (fscanf(fp, "%s",s) == EOF){
			termine = true;
			}*/

        // elimino el primer (") y los últimos 2 ("]) de s:
        strcpy(valor, "");
        strncat(valor, pS, strlen(pS)-2);
        strcpy(valor, valor+1);
		std::cout << "valor: " << valor << std::endl;
		fscanf(fp, "%s",pS);
	}
}


BOOLTYPE readPGN(char *filename)
{
	int iNumJugada;
	char s[180];
	BOOLTYPE returnValue;
	FILE * fp;
	bool fin;

	bool blancas;
	int contador;
	Move mov;
	char* msg_error = NULL;

	char nro[180];
	int n;


	returnValue = false;
	iNumJugada=0;
	fin = false;
	// open the file for read and scan through until we find the number-th position:
	fp=fopen(filename, "rt");
	blancas = true;
	if (fp != NULL){//"move e2e4, or h7h8q
		board.init(); 
		while ((!fin) && (fscanf(fp, "%s", s) != EOF)){ // lee hasta el espacio
			
			readTags(s, fp);

			//acá empieza la parte de descifrar el pgn
			//a menos que me cambien el tablero, la posición destino es del 1 al 8... o sea, el último char que se leyó

			if (blancas){
				contador = find(s, std::string("."));

				strncpy(nro, s, contador);
				nro[contador] = '\0';
				
				if(strcmp(nro, "21") == 0){
					n= 21;
				}

				strncpy(s, s+contador+1, (int)strlen(s));

				if(strcmp(s, "g4") == 0){
					n= 21;
				}
				
				fprintf(stderr, "resultado de blancas: %s", s);
			}else {
				fprintf(stderr, "resultado de negras: %s", s);
			}

			board.moveBufLen[0] = 0;
			board.moveBufLen[1] = movegen(board.moveBufLen[0]);

			mov = obtenerMovimiento(s, blancas, msg_error); 
			if(msg_error != NULL){
				fprintf(stderr, msg_error);
				fclose(fp);
				return false;
			}

			//realizo el movimiento en el tablero	
			makeMove(mov);
 
			if (isOtherKingAttacked())              // post-move check to see if we are leaving our king in check
			{
				unmakeMove(mov);
				strcpy (msg_error,"invalid move, leaving king in check: ");
				fprintf(stderr, msg_error);
				fclose(fp);
				return false;
			}
			else
			{
				board.endOfGame++;
				board.endOfSearch = board.endOfGame;
				board.display();
			}
				
			if (!blancas){
				iNumJugada++;
			}
			blancas = !blancas;
		}

		fclose(fp);
	}
	else {
	printf("winglet> error opening file: %s\n", filename);
	returnValue = false;
	}
return returnValue;
}




///// TODO ENROQUE 


 /// <summary>
        /// Find a castle move
        /// </summary>
        /// <param name="ePlayerColor">     Color moving</param>
        /// <param name="bShortCastling">   true for short, false for long</param>
        /// <param name="iTruncated">       Truncated count</param>
        /// <param name="strMove">          Move</param>
        /// <param name="movePos">          Returned moved if found</param>
        /// <returns>
        /// Moving position or -1 if error
        /// </returns>
        //private int FindCastling(ChessBoard.PlayerColorE ePlayerColor, bool bShortCastling, ref int iTruncated, string strMove, ref ChessBoard.MovePosS movePos) {
        //    int                         iRetVal = -1;
        //    int                         iWantedDelta;
        //    int                         iDelta;
        //    List<ChessBoard.MovePosS>   arrMovePos;

        //    arrMovePos      = m_chessBoard.EnumMoveList(ePlayerColor);
        //    iWantedDelta    = bShortCastling ? 2 : -2;
        //    foreach (ChessBoard.MovePosS move in arrMovePos) {
        //        if ((move.Type & ChessBoard.MoveTypeE.MoveTypeMask) == ChessBoard.MoveTypeE.Castle) {
        //            iDelta = ((int)move.StartPos & 7) - ((int)move.EndPos & 7);
        //            if (iDelta == iWantedDelta) {
        //                iRetVal = (int)move.StartPos + ((int)move.EndPos << 8);
        //                movePos = move;
        //                m_chessBoard.DoMove(move);
        //            }
        //        }
        //    }
        //    if (iRetVal == -1) {
        //        if (m_bDiagnose) {
        //            throw new PgnParserException("Unable to find compatible move - " + strMove, GetCodeInError());
        //        }
        //        iTruncated++;
        //    }
        //    return(iRetVal);
        //}