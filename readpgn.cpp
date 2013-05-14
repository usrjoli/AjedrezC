// jose
// módulo creado para leer archivos PGN

#ifndef _CRT_SECURE_NO_DEPRECATE  // suppress MS security warnings
#define _CRT_SECURE_NO_DEPRECATE
#endif
#include <iostream>
#include "defines.h"
#include "protos.h"
#include "extglobals.h"


Move obtenerMovimiento(char str[180], bool blancas){
	// retorna el movimiento incluyendo: tipo de pieza, color y posición destino
	Move mov;
	char primera;
	unsigned char userTo;
	char str_mov[10]; // movimiento sin nombre de piezas

	mov.clear();
	primera = str[0];
	std::cout << "primera: " << primera << std::endl;
	
	strcpy(str_mov, "");
    strncat(str_mov, str, strlen(str));

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

	// transformo el string ingresado en un movimiento
	/*
	sacar caracteres que no sean cadillero (por ejemplo x de comer, coronación, enroque)
	si largo de str_mov == 2 -> sin ambiguedad
	si largo de str_mov == 3 -> abmiguedad obtener el origen y ver cual corresponde a la columna o fila indicada en str_mov
	si largo de str_mov == 4 -> ambiguedad y se tiene el origen en str_mov



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

	

	//userTo = str[0] - 97;
    //userTo += 8 * (str[i+1] - 49);
	//mov.setTosq(userTo); // ver cómo pasar del string e4 a la var global E4
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

	int largo;
	bool blancas;
	int posDestino;
	int contador;
	Move mov;


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
			largo = (int)strlen(s);
			posDestino = (int)s[largo-1];
			if (blancas){
				contador = 0;
				while (s[contador] != '.'){
					contador++;
				}
				strncpy(s, s+contador+1, (int)strlen(s));
				std::cout << "resultado de blancas: " << s << std::endl;
			}else {
				std::cout << "resultado de negras: " << s << std::endl;
			}
			mov = obtenerMovimiento(s, blancas);
			// TODO: obtengo todos los movimientos posibles del tablero, los recorro y cuando encuentro uno con posFinal = a la que busco y su pieza es la que busco -> retorno ese mov inicial
			// mov.obtenerOrigen();
			// makeMove(mov); realizo el movimiento en el tablero	
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


//void setupFen(char *fen, char *fencolor, char *fencastling, char *fenenpassant, int fenhalfmoveclock, int fenfullmovenumber)
//{
//       int i, file, rank, counter, piece;
//       int whiteCastle, blackCastle, next, epsq;
// 
//       piece = 0;
//       for (i = 0; i < 64; i++) {
//			board.square[i] = EMPTY;
//       }
// 
//       // loop over the FEN string characters, and populate board.square[]
//       // i is used as index for the FEN string
//       // counter is the index for board.square[], 0..63
//       // file and rank relate to the position on the chess board, 1..8
//       // There is no error/legality checking on the FEN string!!
//       file = 1;
//       rank = 8;
//       i = 0;
//       counter = 0;
//       while ((counter < 64) && (fen[i] != '\0')){
//              // '1'  through '8':
//              if (((int) fen[i] > 48) && ((int) fen[i] < 57)){
//                     file+= (int) fen[i] - 48;
//                     counter+= (int) fen[i] - 48;
//              }
//              else{ //  other characters:
//                     switch (fen[i]){
//                           case '/':
//                                  rank--;
//                                  file = 1;
//                                  break;
//                           case 'P':
//                                  board.square[BOARDINDEX[file][rank]] = WHITE_PAWN;
//                                  file += 1;
//                                  counter += 1;
//                                  break;
//                           case 'N':
//                                  board.square[BOARDINDEX[file][rank]] = WHITE_KNIGHT;
//                                  file += 1;
//                                  counter += 1;
//                                  break;
//                           case 'B':
//                                  board.square[BOARDINDEX[file][rank]] = WHITE_BISHOP;
//                                  file += 1;
//                                  counter += 1;
//                                  break;
//                           case 'R':
//                                  board.square[BOARDINDEX[file][rank]] = WHITE_ROOK;
//                                  file += 1;
//                                  counter += 1;
//                                  break;
//                           case 'Q':
//                                  board.square[BOARDINDEX[file][rank]] = WHITE_QUEEN;
//                                  file += 1;
//                                  counter += 1;
//                                  break;
//                           case 'K':
//                                  board.square[BOARDINDEX[file][rank]] = WHITE_KING;
//                                  file += 1;
//                                  counter += 1;
//                                  break;
//                           case 'p':
//                                  board.square[BOARDINDEX[file][rank]] = BLACK_PAWN;
//                                  file += 1;
//                                  counter += 1;
//                                  break;
//                           case 'n':
//                                  board.square[BOARDINDEX[file][rank]] = BLACK_KNIGHT;
//                                  file += 1;
//                                  counter += 1;
//                                  break;
//                           case 'b':
//                                  board.square[BOARDINDEX[file][rank]] = BLACK_BISHOP;
//                                  file += 1;
//                                  counter += 1;
//                                  break;
//                           case 'r':
//                                  board.square[BOARDINDEX[file][rank]] = BLACK_ROOK;
//                                  file += 1;
//                                  counter += 1;
//                                  break;
//                           case 'q':
//                                  board.square[BOARDINDEX[file][rank]] = BLACK_QUEEN;
//                                  file += 1;
//                                  counter += 1;
//                                  break;
//                           case 'k':
//                                  board.square[BOARDINDEX[file][rank]] = BLACK_KING;
//                                  file += 1;
//                                  counter += 1;
//                                  break;
//                           default:
//                                  break;
//                     }
//              }
//              i++;
//       }
//       next = WHITE_MOVE;
//       if (fencolor[0] == 'b') next = BLACK_MOVE;
// 
//       whiteCastle = 0;
//       blackCastle = 0;
//       if (strstr(fencastling, "K")) whiteCastle += CANCASTLEOO;
//       if (strstr(fencastling, "Q")) whiteCastle += CANCASTLEOOO;
//       if (strstr(fencastling, "k")) blackCastle += CANCASTLEOO;
//       if (strstr(fencastling, "q")) blackCastle += CANCASTLEOOO;
//       if (strstr(fenenpassant, "-"))
//       {
//              epsq = 0;
//       }
//       else{
//              // translate a square coordinate (as string) to int (eg 'e3' to 20):
//              epsq = ((int) fenenpassant[0] - 96) + 8 * ((int) fenenpassant[1] - 48) - 9;
//       }
//       board.initFromSquares(board.square, next, fenhalfmoveclock, whiteCastle , blackCastle , epsq);
//}