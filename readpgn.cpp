// jose
// módulo creado para leer archivos PGN

#ifndef _CRT_SECURE_NO_DEPRECATE  // suppress MS security warnings
#define _CRT_SECURE_NO_DEPRECATE
#endif
#include <iostream>
#include "defines.h"
#include "protos.h"
#include "extglobals.h"

typedef enum Piezas {Peon, Torre, Caballo, Alfil, Reina, Rey} Piezas;

Piezas investigaPieza(char str[180]){
	Piezas pieza;
	pieza = Peon;
	char primera;

	primera = str[0];
	std::cout << "primera: " << primera << std::endl;
	if ((primera >= 'a') && (primera <= 'z')){//esto es un peón
		pieza = Peon;
	}
	if (primera == 'R') {
		pieza = Torre;
	}
	if (primera == 'N') {
		pieza = Caballo;
	}
	if (primera == 'B') {
		pieza = Alfil;
	}
	if (primera == 'Q') {
		pieza = Reina;
	}
	if (primera == 'K') {
		pieza = Rey;
	}
	std::cout << "Pieza: " << pieza << std::endl;
	return pieza;
}

BOOLTYPE readPGN(char *filename, int number)
{
	int iNumJugada;
	char s[180];
	BOOLTYPE returnValue;
	FILE * fp;
	bool fin;
	bool termine;
	int largo;
	bool blancas;
	int posDestino;
	int contador;
	Piezas pieza;

	returnValue = false;
	if (number <= 0) return returnValue;
	iNumJugada=0;
	fin = false;
	// open the file for read and scan through until we find the number-th position:
	fp=fopen(filename, "rt");
	blancas = true;
	if (fp != NULL){//"move e2e4, or h7h8q
		board.init(); 
		while ((!fin) && (fscanf(fp, "%s", s) != EOF)){
//			std::cout << " texto leido en el while: " << s << std::endl;
			if (s[0] != '['){
//			if (!strcmp(s, "[")){
//acá empieza la parte de descifrar el pgn
//hay que ver el tema de la jugada, porque si vienen varias en un mismo renglón puede dar problemas
				std::cout << "se leyo: " << s << std::endl;
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
				pieza = investigaPieza(s);
				

				if (!blancas){
					iNumJugada++;
				}
				blancas = !blancas;
				if (iNumJugada == number){
					fin = true;
				}
			}else{
				std::cout << "comentario: ";
				termine = false;
//				std::cout << s;
				while (!termine){
					largo = (int)strlen(s);
//					std::cout << "largo = " << largo << std::endl;
//					std::cout << "es esto:" << s[largo-1] << std::endl;
					std::cout << " " << s;
					if (s[largo-1] == ']'){
						termine = true;
//						std::cout<<  "debería liquidar acá." << std::endl;
					}else{
						if (fscanf(fp, "%s",s) == EOF){
							termine = true;
						}
					}
				}
				std::cout << std::endl;
			}
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