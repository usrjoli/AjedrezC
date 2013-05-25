#ifndef WINGLET_PROTOS_H
#define WINGLET_PROTOS_H

#include "board.h"
#include "move.h"
#include <string>

unsigned int  bitCnt(BitMap);
int           captgen(int);
void          dataInit();
void          displayBitmap(BitMap);
void          displayMove(Move &);
void		  displayPV();
void		  commands();
BOOLTYPE      doIniCommand(const char *);
unsigned int  firstOne(BitMap);
void          info();
BOOLTYPE      isAttacked(BitMap &, const unsigned char &);
BOOLTYPE      isOtherKingAttacked();
BOOLTYPE      isOwnKingAttacked();
BOOLTYPE      isValidTextMove(char *, Move &);
unsigned int  lastOne(BitMap);
void          makeBlackPromotion(unsigned int, unsigned int &);
void          makeCapture(unsigned int &, unsigned int &);
void          makeMove(Move &);
void          makeWhitePromotion(unsigned int, unsigned int &);
int           movegen(int);
void		  mstostring(U64 dt, char *);
U64           perft(int, int);
BOOLTYPE      readFen(char *, int);
// jose - inicio - lectura de archivo PGN
BOOLTYPE      readPGN(char *);
// jose - fin - lectura de archivo PGN
void		  readIniFile();
void          setup();
void          setupFen(char *, char *, char *, char *, int , int );
void		  test(char *);
void		  timeControl();
BOOLTYPE	  toSan(Move &, char *);
void          unmakeBlackPromotion(unsigned int, unsigned int &);
void          unmakeCapture(unsigned int &, unsigned int &);
void          unmakeMove(Move &);
void          unmakeWhitePromotion(unsigned int, unsigned int &);
//jose inicio debug movimientos
bool isCheckMate();
bool isCheck();
void displayFullMove(Move &move);
void moves();
void castles();
void material();
int duplicatedPositions(bool show, bool blancas);
int movSPawn();
int reachableSquares(bool display);
int find(char* source, std::string element);
//jose fin debug movimientos
#endif 