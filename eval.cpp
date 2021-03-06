#include <iostream>
#include "defines.h"
#include "protos.h"
#include "extglobals.h"
#include "board.h"

 
int Board::eval()
{
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// This is winglet's evaluation function
// The score is calculated from White's perspective (in centipawns)
// and returned from the perspective of the side to move.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       int score, square;
       int whitepawns, whiteknights, whitebishops, whiterooks, whitequeens;
       int blackpawns, blackknights, blackbishops, blackrooks, blackqueens;
       int whitekingsquare, blackkingsquare;
       int whitetotalmat, blacktotalmat;
       int whitetotal, blacktotal;
       BOOLTYPE endgame;
       BitMap temp, whitepassedpawns, blackpassedpawns;
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Material
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
       score = board.Material;
       #ifdef WINGLET_VERBOSE_EVAL
              std::cout << "EVAL> MATERIAL>                       " << board.Material << std::endl;
       #endif
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Remember where the kings are
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       whitekingsquare = firstOne(board.whiteKing);
       #ifdef WINGLET_VERBOSE_EVAL
              std::cout << "EVAL> WHITE KING POSITION>            " << SQUARENAME[whitekingsquare] << std::endl;
       #endif
       blackkingsquare = firstOne(board.blackKing);
       #ifdef WINGLET_VERBOSE_EVAL
              std::cout << "EVAL> BLACK KING POSITION>            " << SQUARENAME[blackkingsquare] << std::endl;
       #endif
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Piece counts, note that we could have done this incrementally in (un)makeMove
// because it's basically the same thing as keeping board.Material up to date..
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       whitepawns = bitCnt(board.whitePawns);
       whiteknights = bitCnt(board.whiteKnights);
       whitebishops = bitCnt(board.whiteBishops);
       whiterooks = bitCnt(board.whiteRooks);
       whitequeens = bitCnt(board.whiteQueens);
       whitetotalmat = 3 * whiteknights + 3 * whitebishops + 5 * whiterooks + 10 * whitequeens;
       whitetotal = whitepawns + whiteknights + whitebishops + whiterooks + whitequeens;
       
	   blackpawns = bitCnt(board.blackPawns);
       blackknights = bitCnt(board.blackKnights);
       blackbishops = bitCnt(board.blackBishops);
       blackrooks = bitCnt(board.blackRooks);
       blackqueens = bitCnt(board.blackQueens);
       blacktotalmat = 3 * blackknights + 3 * blackbishops + 5 * blackrooks + 10 * blackqueens;
       blacktotal = blackpawns + blackknights + blackbishops + blackrooks + blackqueens;
       
		#ifdef WINGLET_VERBOSE_EVAL
              std::cout << "EVAL> WHITE TOTAL MAT>                " << whitetotalmat << std::endl;
              std::cout << "EVAL> BLACK TOTAL MAT>                " << blacktotalmat << std::endl;
              std::cout << "EVAL> WHITE TOTAL>                    " << whitetotal << std::endl;
              std::cout << "EVAL> BLACK TOTAL>                    " << blacktotal << std::endl;
       #endif
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Check if we are in the endgame
// Anything less than a queen (=10) + rook (=5) is considered endgame
// (pawns excluded in this count)
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       endgame = (whitetotalmat < 15 || blacktotalmat < 15);
       #ifdef WINGLET_VERBOSE_EVAL
              std::cout << "EVAL> ENDGAME>                        " << endgame << std::endl;
       #endif
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate for draws due to insufficient material:
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!whitepawns && !blackpawns) // si no hay peones
	{
		// king versus king:
		if ((whitetotalmat == 0) && (blacktotalmat == 0)) 
		{
			if (board.nextMove) return -DRAWSCORE;
			else return DRAWSCORE;
		}
 
		// king and knight versus king:
		if (((whitetotalmat == 3) && (whiteknights == 1) && (blacktotalmat == 0)) ||
			((blacktotalmat == 3) && (blackknights == 1) && (whitetotalmat == 0))) 
		{
			if (board.nextMove) return -DRAWSCORE;
			else return DRAWSCORE;
		}
 
		// 2 kings with one or more bishops, and all bishops on the same colour:
		if ((whitebishops + blackbishops) > 0)
		{
				if ((whiteknights == 0) && (whiterooks == 0) && (whitequeens == 0) &&
				(blackknights == 0) && (blackrooks == 0) && (blackqueens == 0))
			{
				if (!((board.whiteBishops | board.blackBishops) & WHITE_SQUARES) ||
					!((board.whiteBishops | board.blackBishops) & BLACK_SQUARES))
				{
					if (board.nextMove) return -DRAWSCORE;
					else return DRAWSCORE;
				}
			}
		}
	}
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate MATERIAL
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Have the winning side prefer to exchange pieces
// Every exchange with unequal material adds 3 centipawns to the score
// Loosing a piece (from balanced material) becomes more
// severe in the endgame
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       #ifdef WINGLET_VERBOSE_EVAL
              int iexch = 0;
              int iwhitepos = 0;
              int iwhitepawns = 0;
              int iwhiteking = 0;
              int iblackpos = 0;
              int iblackpawns = 0;
              int iblackking = 0;
       #endif
 
       if (whitetotalmat + whitepawns > blacktotalmat + blackpawns)
       {
              score += 45 + 3 * whitetotal - 6 * blacktotal;
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> EXCHANGE WHITE>                 " << (45 + 3 * whitetotal - 6 * blacktotal) << std::endl;
                     iexch = (45 + 3 * whitetotal - 6 * blacktotal);
              #endif
       }
       else
       {
              if (whitetotalmat + whitepawns < blacktotalmat + blackpawns)
              {
                     score += -45 - 3 * blacktotal + 6 * whitetotal;
                     #ifdef WINGLET_VERBOSE_EVAL
                           std::cout << "EVAL> EXCHANGE BLACK>                 " << (-45 - 3 * blacktotal + 6 * whitetotal) << std::endl;
                           iexch = (-45 - 3 * blacktotal + 6 * whitetotal);
                     #endif
              }
       }
 

	   /*********************************** EVALUACI�N POR POSICI�N DEL TALBERO **************************/


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate WHITE PIECES
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate white pawns
// - position on the board
// - distance from opponent king
// - distance from own king
// - passed, doubled, isolated or backward pawns
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       whitepassedpawns = 0;
       temp = board.whitePawns;
       while (temp)
       {
              square = firstOne(temp);
 
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> WHITE PAWN ON                   " << SQUARENAME[square] << std::endl;
              #endif
 
              score += PAWNPOS_W[square];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  POSITION SCORE IS              " << PAWNPOS_W[square] << std::endl;
                     iwhitepos += PAWNPOS_W[square];
                     iwhitepawns += PAWNPOS_W[square];
              #endif
 
              score += PAWN_OPPONENT_DISTANCE[DISTANCE[square][blackkingsquare]];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << PAWN_OPPONENT_DISTANCE[DISTANCE[square][blackkingsquare]] << std::endl;
                     iwhitepos += PAWN_OPPONENT_DISTANCE[DISTANCE[square][blackkingsquare]];
                     iwhitepawns += PAWN_OPPONENT_DISTANCE[DISTANCE[square][blackkingsquare]];
              #endif
 
              if (endgame)
              {
                     score += PAWN_OWN_DISTANCE[DISTANCE[square][whitekingsquare]];
                     #ifdef WINGLET_VERBOSE_EVAL
                           std::cout << "EVAL>  DISTANCE TO OWN KING SCORE     " << PAWN_OWN_DISTANCE[DISTANCE[square][blackkingsquare]] << std::endl;
                           iwhitepos += PAWN_OWN_DISTANCE[DISTANCE[square][whitekingsquare]];
                           iwhitepawns += PAWN_OWN_DISTANCE[DISTANCE[square][whitekingsquare]];
                     #endif
              }
 
              if (!(PASSED_WHITE[square] & board.blackPawns))
              {
                     score += BONUS_PASSED_PAWN;
                     #ifdef WINGLET_VERBOSE_EVAL
                           std::cout << "EVAL>  IS PASSED, BONUS IS            " << BONUS_PASSED_PAWN << std::endl;
                           iwhitepos += BONUS_PASSED_PAWN;
                           iwhitepawns += BONUS_PASSED_PAWN;
                     #endif
 
                     // remember its location, we need it later when evaluating the white rooks:
                     whitepassedpawns ^= BITSET[square];
              }
 
              if ((board.whitePawns ^ BITSET[square]) & FILEMASK[square])
              {
                     score -= PENALTY_DOUBLED_PAWN;
                     #ifdef WINGLET_VERBOSE_EVAL
                           std::cout << "EVAL>  IS DOUBLED, PENALTY IS         " << PENALTY_DOUBLED_PAWN << std::endl;
                           iwhitepos -= PENALTY_DOUBLED_PAWN;
                            iwhitepawns -= PENALTY_DOUBLED_PAWN;
                     #endif
              }
 
              if (!(ISOLATED_WHITE[square] & board.whitePawns))
              {
                     score -= PENALTY_ISOLATED_PAWN;
                     #ifdef WINGLET_VERBOSE_EVAL
                           std::cout << "EVAL>  IS ISOLATED, PENALTY IS        " << PENALTY_ISOLATED_PAWN << std::endl;
                           iwhitepos -= PENALTY_ISOLATED_PAWN;
                           iwhitepawns -= PENALTY_ISOLATED_PAWN;
                     #endif
              }
              else
              {
                     // If it is not isolated, then it might be backward. Two conditions must be true:
                     //  1) if the next square is controlled by an enemy pawn - we use the PAWN_ATTACKS bitmaps to check this
                     //  2) if there are no pawns left that could defend this pawn
                     if ((WHITE_PAWN_ATTACKS[square + 8] & board.blackPawns))
                     {
                           if (!(BACKWARD_WHITE[square] & board.whitePawns))
                           {
                                  score -= PENALTY_BACKWARD_PAWN;
                                  #ifdef WINGLET_VERBOSE_EVAL
                                         std::cout << "EVAL>  IS BACKWARD, PENALTY IS        " << PENALTY_BACKWARD_PAWN << std::endl;
                                         iwhitepos -= PENALTY_BACKWARD_PAWN;
                                         iwhitepawns -= PENALTY_BACKWARD_PAWN;
                                  #endif
                           }
                     }
              }
 
              temp ^= BITSET[square];
       }
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate white knights
// - position on the board
// - distance from opponent king
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       temp = board.whiteKnights;
       while (temp)
       {
              square = firstOne(temp);
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> WHITE KNIGHT ON                 " << SQUARENAME[square] << std::endl;
              #endif
 
              score += KNIGHTPOS_W[square];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  POSITION SCORE IS              " << KNIGHTPOS_W[square] << std::endl;
                     iwhitepos += KNIGHTPOS_W[square];
              #endif
 
              score += KNIGHT_DISTANCE[DISTANCE[square][blackkingsquare]];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << KNIGHT_DISTANCE[DISTANCE[square][blackkingsquare]] << std::endl;
                     iwhitepos += KNIGHT_DISTANCE[DISTANCE[square][blackkingsquare]];
              #endif
 
              temp ^= BITSET[square];
       }
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate white bishops
// - having the pair
// - position on the board
// - distance from opponent king
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       if (board.whiteBishops)
       {
		      if ((board.whiteBishops & WHITE_SQUARES) && (board.whiteBishops & BLACK_SQUARES))
              {
                     score += BONUS_BISHOP_PAIR;
                     #ifdef WINGLET_VERBOSE_EVAL
                           std::cout << "EVAL> WHITE BISHOP PAIR BONUS         " << BONUS_BISHOP_PAIR << std::endl;
                           iwhitepos += BONUS_BISHOP_PAIR;
                     #endif              
              }
       }
 
       temp = board.whiteBishops;
       while (temp)
       {
              square = firstOne(temp);
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> WHITE BISHOP ON                 " << SQUARENAME[square] << std::endl;
              #endif
 
              score += BISHOPPOS_W[square];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  POSITION SCORE IS              " << BISHOPPOS_W[square] << std::endl;
                     iwhitepos += BISHOPPOS_W[square];
              #endif
 
              score += BISHOP_DISTANCE[DISTANCE[square][blackkingsquare]];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << BISHOP_DISTANCE[DISTANCE[square][blackkingsquare]] << std::endl;
                     iwhitepos += BISHOP_DISTANCE[DISTANCE[square][blackkingsquare]];
              #endif
 
              temp ^= BITSET[square];
       }
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate white rooks
// - position on the board
// - distance from opponent king
// - on the same file as a passed pawn
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       temp = board.whiteRooks;
       while (temp)
       {
              square = firstOne(temp);
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> WHITE ROOK ON                   " << SQUARENAME[square] << std::endl;
              #endif
 
              score += ROOKPOS_W[square];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  POSITION SCORE IS              " << ROOKPOS_W[square] << std::endl;
                     iwhitepos += ROOKPOS_W[square];
              #endif
 
              score += ROOK_DISTANCE[DISTANCE[square][blackkingsquare]];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << ROOK_DISTANCE[DISTANCE[square][blackkingsquare]] << std::endl;
                     iwhitepos += ROOK_DISTANCE[DISTANCE[square][blackkingsquare]];
              #endif
 
              if (FILEMASK[square] & whitepassedpawns)
              {
                     if ((unsigned int) square < lastOne(FILEMASK[square] & whitepassedpawns))
                     {
                           score += BONUS_ROOK_BEHIND_PASSED_PAWN;
                           #ifdef WINGLET_VERBOSE_EVAL
                                  std::cout << "EVAL>  BEHIND PASSED PAWN BONUS       " << BONUS_ROOK_BEHIND_PASSED_PAWN << std::endl;
                                  iwhitepos += BONUS_ROOK_BEHIND_PASSED_PAWN;
                           #endif
                     }
              }
              temp ^= BITSET[square];
       }
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate white queens
// - position on the board
// - distance from opponent king
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       temp = board.whiteQueens;
       while (temp)
       {
              square = firstOne(temp);
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> WHITE QUEEN ON                  " << SQUARENAME[square] << std::endl;
              #endif
 
              score += QUEENPOS_W[square];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  POSITION SCORE IS              " << QUEENPOS_W[square] << std::endl;
                     iwhitepos += QUEENPOS_W[square];
              #endif
 
              score += QUEEN_DISTANCE[DISTANCE[square][blackkingsquare]];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << QUEEN_DISTANCE[DISTANCE[square][blackkingsquare]] << std::endl;
                     iwhitepos += QUEEN_DISTANCE[DISTANCE[square][blackkingsquare]];
              #endif
 
              temp ^= BITSET[square];
       }
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate the white king
// - position on the board
// - proximity to the pawns
// - pawn shield (not in the endgame)
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       if (endgame)
       {
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> WHITE KING ON (endgame)         " << SQUARENAME[whitekingsquare] << std::endl;
              #endif
 
              score += KINGPOS_ENDGAME_W[whitekingsquare];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  POSITION SCORE IS              " << KINGPOS_ENDGAME_W[whitekingsquare] << std::endl;
                     iwhitepos += KINGPOS_ENDGAME_W[whitekingsquare];
                     iwhiteking += KINGPOS_ENDGAME_W[whitekingsquare];
              #endif
 
       }
       else
       {
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> WHITE KING ON (no endgame)      " << SQUARENAME[whitekingsquare] << std::endl;
              #endif
 
              score += KINGPOS_W[whitekingsquare];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  POSITION SCORE IS              " << KINGPOS_W[whitekingsquare] << std::endl;
                     iwhitepos += KINGPOS_W[whitekingsquare];
                     iwhiteking += KINGPOS_W[whitekingsquare];
              #endif
 
              // add pawn shield bonus if we're not in the endgame:
              // strong pawn shield bonus if the pawns are near the king:
              score += BONUS_PAWN_SHIELD_STRONG * bitCnt(KINGSHIELD_STRONG_W[whitekingsquare] & board.whitePawns);
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  STRONG PAWN SHIELD SCORE IS    " << BONUS_PAWN_SHIELD_STRONG * bitCnt(KINGSHIELD_STRONG_W[whitekingsquare] & board.whitePawns) << std::endl;
                     iwhitepos += BONUS_PAWN_SHIELD_STRONG * bitCnt(KINGSHIELD_STRONG_W[whitekingsquare] & board.whitePawns);
                     iwhiteking += BONUS_PAWN_SHIELD_STRONG * bitCnt(KINGSHIELD_STRONG_W[whitekingsquare] & board.whitePawns);
              #endif
 
              // weaker pawn shield bonus if the pawns are not so near the king:
              score += BONUS_PAWN_SHIELD_WEAK * bitCnt(KINGSHIELD_WEAK_W[whitekingsquare] & board.whitePawns);
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  WEAK PAWN SHIELD SCORE IS      " << BONUS_PAWN_SHIELD_WEAK * bitCnt(KINGSHIELD_WEAK_W[whitekingsquare] & board.whitePawns) << std::endl;
                     iwhitepos += BONUS_PAWN_SHIELD_WEAK * bitCnt(KINGSHIELD_WEAK_W[whitekingsquare] & board.whitePawns);
                     iwhiteking += BONUS_PAWN_SHIELD_WEAK * bitCnt(KINGSHIELD_WEAK_W[whitekingsquare] & board.whitePawns);
              #endif
 
       }
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate BLACK PIECES
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate black pawns
// - position on the board
// - distance from opponent king
// - distance from own king
// - passed, doubled, isolated or backward pawns
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       blackpassedpawns = 0;
       temp = board.blackPawns;
       while (temp)
       {
              square = firstOne(temp);
 
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> BLACK PAWN ON                   " << SQUARENAME[square] << std::endl;
              #endif
 
              score -= PAWNPOS_B[square];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  POSITION SCORE IS              " << PAWNPOS_B[square] << std::endl;
                     iblackpos -= PAWNPOS_B[square];
                     iblackpawns -= PAWNPOS_B[square];
              #endif
 
              score -= PAWN_OPPONENT_DISTANCE[DISTANCE[square][whitekingsquare]];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << PAWN_OPPONENT_DISTANCE[DISTANCE[square][whitekingsquare]] << std::endl;
                     iblackpos -= PAWN_OPPONENT_DISTANCE[DISTANCE[square][whitekingsquare]];
                     iblackpawns -= PAWN_OPPONENT_DISTANCE[DISTANCE[square][whitekingsquare]];
              #endif
 
              if (endgame)
              {
                     score -= PAWN_OWN_DISTANCE[DISTANCE[square][blackkingsquare]];
                     #ifdef WINGLET_VERBOSE_EVAL
                           std::cout << "EVAL>  DISTANCE TO OWN KING SCORE     " << PAWN_OWN_DISTANCE[DISTANCE[square][blackkingsquare]] << std::endl;
                           iblackpos -= PAWN_OWN_DISTANCE[DISTANCE[square][blackkingsquare]];
                           iblackpawns -= PAWN_OWN_DISTANCE[DISTANCE[square][blackkingsquare]];
                     #endif
              }
 
              if (!(PASSED_BLACK[square] & board.whitePawns))
              {
                     score -= BONUS_PASSED_PAWN;
                     #ifdef WINGLET_VERBOSE_EVAL
                           std::cout << "EVAL>  IS PASSED, BONUS IS            " << BONUS_PASSED_PAWN << std::endl;
                           iblackpos -= BONUS_PASSED_PAWN;
                           iblackpawns -= BONUS_PASSED_PAWN;
                     #endif
 
                     // remember its location, we need it later when evaluating the black rooks:
                     blackpassedpawns ^= BITSET[square];
 
              }
 
              if ((board.blackPawns ^ BITSET[square]) & FILEMASK[square])
              {
                     score += PENALTY_DOUBLED_PAWN;
                     #ifdef WINGLET_VERBOSE_EVAL
                           std::cout << "EVAL>  IS DOUBLED, PENALTY IS         " << PENALTY_DOUBLED_PAWN << std::endl;
                           iblackpos += PENALTY_DOUBLED_PAWN;
                           iblackpawns += PENALTY_DOUBLED_PAWN;
                     #endif
              }
 
              if (!(ISOLATED_BLACK[square] & board.blackPawns))
              {
                     score += PENALTY_ISOLATED_PAWN;
                     #ifdef WINGLET_VERBOSE_EVAL
                           std::cout << "EVAL>  IS ISOLATED, PENALTY IS        " << PENALTY_ISOLATED_PAWN << std::endl;
                           iblackpos += PENALTY_ISOLATED_PAWN;
                            iblackpawns += PENALTY_ISOLATED_PAWN;
                     #endif
              }
              else
              {
                     // If it is not isolated, then it might be backward. Two conditions must be true:
                     //  1) if the next square is controlled by an enemy pawn - we use the PAWN_ATTACKS bitmaps to check this
                     //  2) if there are no pawns left that could defend this pawn
                     if ((BLACK_PAWN_ATTACKS[square - 8] & board.whitePawns))
                     {
                           if (!(BACKWARD_BLACK[square] & board.blackPawns))
                           {
                                  score += PENALTY_BACKWARD_PAWN;
                                  #ifdef WINGLET_VERBOSE_EVAL
                                         std::cout << "EVAL>  IS BACKWARD, PENALTY IS        " << PENALTY_BACKWARD_PAWN << std::endl;
                                         iwhitepos += PENALTY_BACKWARD_PAWN;
                                         iwhitepawns += PENALTY_BACKWARD_PAWN;
                                  #endif
                           }
                     }
              }
 
              temp ^= BITSET[square];
 
       }
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate black knights
// - position on the board
// - distance from opponent king
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       temp = board.blackKnights;
       while (temp)
       {
              square = firstOne(temp);
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> BLACK KNIGHT ON                 " << SQUARENAME[square] << std::endl;
              #endif
 
              score -= KNIGHTPOS_B[square];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  POSITION SCORE IS              " << KNIGHTPOS_B[square] << std::endl;
                     iblackpos -= KNIGHTPOS_B[square];
              #endif
 
              score -= KNIGHT_DISTANCE[DISTANCE[square][whitekingsquare]];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << KNIGHT_DISTANCE[DISTANCE[square][whitekingsquare]] << std::endl;
                     iblackpos -= KNIGHT_DISTANCE[DISTANCE[square][whitekingsquare]];
              #endif
 
              temp ^= BITSET[square];
       }
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate black bishops
// - having the pair
// - position on the board
// - distance from opponent king
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       if (board.blackBishops)
       {
		      if ((board.blackBishops & WHITE_SQUARES) && (board.blackBishops & BLACK_SQUARES))
              {
                     score -= BONUS_BISHOP_PAIR;
                     #ifdef WINGLET_VERBOSE_EVAL
                           std::cout << "EVAL> BLACK BISHOP PAIR BONUS         " << BONUS_BISHOP_PAIR << std::endl;
                           iblackpos -= BONUS_BISHOP_PAIR;
                     #endif              
              }
       }
 
       temp = board.blackBishops;
       while (temp)
       {
              square = firstOne(temp);
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> BLACK BISHOP ON                 " << SQUARENAME[square] << std::endl;
              #endif
 
              score -= BISHOPPOS_B[square];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  POSITION SCORE IS              " << BISHOPPOS_B[square] << std::endl;
                     iblackpos -= BISHOPPOS_B[square];
              #endif
 
              score -= BISHOP_DISTANCE[DISTANCE[square][whitekingsquare]];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << BISHOP_DISTANCE[DISTANCE[square][whitekingsquare]] << std::endl;
                     iblackpos -= BISHOP_DISTANCE[DISTANCE[square][whitekingsquare]];
              #endif
 
              temp ^= BITSET[square];
       }
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate black rooks
// - position on the board
// - distance from opponent king
// - on the same file as a passed pawn
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       temp = board.blackRooks;
       while (temp)
       {
              square = firstOne(temp);
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> BLACK ROOK ON                   " << SQUARENAME[square] << std::endl;
              #endif
 
              score -= ROOKPOS_B[square];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  POSITION SCORE IS              " << ROOKPOS_B[square] << std::endl;
                     iblackpos -= ROOKPOS_B[square];
              #endif
 
              score -= ROOK_DISTANCE[DISTANCE[square][whitekingsquare]];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << ROOK_DISTANCE[DISTANCE[square][whitekingsquare]] << std::endl;
                     iblackpos -= ROOK_DISTANCE[DISTANCE[square][whitekingsquare]];
              #endif
 
              if (FILEMASK[square] & blackpassedpawns)
              {
                     if ((unsigned int) square > firstOne(FILEMASK[square] & blackpassedpawns))
                     {
                           score -= BONUS_ROOK_BEHIND_PASSED_PAWN;
                           #ifdef WINGLET_VERBOSE_EVAL
                                  std::cout << "EVAL>  BEHIND PASSED PAWN BONUS       " << BONUS_ROOK_BEHIND_PASSED_PAWN << std::endl;
                                  iblackpos -= BONUS_ROOK_BEHIND_PASSED_PAWN;
                           #endif
                     }
              }
              temp ^= BITSET[square];
       }
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate black queens
// - position on the board
// - distance from opponent king
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       temp = board.blackQueens;
       while (temp)
       {
              square = firstOne(temp);
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> BLACK QUEEN ON                  " << SQUARENAME[square] << std::endl;
              #endif
 
              score -= QUEENPOS_B[square];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  POSITION SCORE IS              " << QUEENPOS_B[square] << std::endl;
                     iblackpos -= QUEENPOS_B[square];
              #endif
 
              score -= QUEEN_DISTANCE[DISTANCE[square][whitekingsquare]];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << QUEEN_DISTANCE[DISTANCE[square][whitekingsquare]] << std::endl;
                     iblackpos -= QUEEN_DISTANCE[DISTANCE[square][whitekingsquare]];
              #endif
 
              temp ^= BITSET[square];
       }
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate the black king
// - position on the board
// - proximity to the pawns
// - pawn shield (not in the endgame)
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       if (endgame)
       {
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> BLACK KING ON (endgame)         " << SQUARENAME[blackkingsquare] << std::endl;
              #endif
 
              score -= KINGPOS_ENDGAME_B[blackkingsquare];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  POSITION SCORE IS              " << KINGPOS_ENDGAME_B[blackkingsquare] << std::endl;
                     iblackpos -= KINGPOS_ENDGAME_B[blackkingsquare];
                     iblackking -= KINGPOS_ENDGAME_B[blackkingsquare];
              #endif
       }
       else
       {
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> BLACK KING ON (no endgame)      " << SQUARENAME[blackkingsquare] << std::endl;
              #endif
 
              score -= KINGPOS_B[blackkingsquare];
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  POSITION SCORE IS              " << KINGPOS_B[blackkingsquare] << std::endl;
                     iblackpos -= KINGPOS_B[blackkingsquare];
                     iblackking -= KINGPOS_B[blackkingsquare];
              #endif
 
              // add pawn shield bonus if we're not in the endgame:
              // strong pawn shield bonus if the pawns are near the king:
              score -= BONUS_PAWN_SHIELD_STRONG * bitCnt(KINGSHIELD_STRONG_B[blackkingsquare] & board.blackPawns);
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  STRONG PAWN SHIELD SCORE IS    " << BONUS_PAWN_SHIELD_STRONG * bitCnt(KINGSHIELD_STRONG_B[blackkingsquare] & board.blackPawns) << std::endl;
                     iblackpos -= BONUS_PAWN_SHIELD_STRONG * bitCnt(KINGSHIELD_STRONG_B[blackkingsquare] & board.blackPawns);
                     iblackking -= BONUS_PAWN_SHIELD_STRONG * bitCnt(KINGSHIELD_STRONG_B[blackkingsquare] & board.blackPawns);
              #endif
 
              // weaker pawn shield bonus if the pawns are not so near the king:
              score -= BONUS_PAWN_SHIELD_WEAK * bitCnt(KINGSHIELD_WEAK_B[blackkingsquare] & board.blackPawns);
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL>  WEAK PAWN SHIELD SCORE IS      " << BONUS_PAWN_SHIELD_WEAK * bitCnt(KINGSHIELD_WEAK_B[blackkingsquare] & board.blackPawns) << std::endl;
                     iblackpos -= BONUS_PAWN_SHIELD_WEAK * bitCnt(KINGSHIELD_WEAK_B[blackkingsquare] & board.blackPawns);
                     iblackking -= BONUS_PAWN_SHIELD_WEAK * bitCnt(KINGSHIELD_WEAK_B[blackkingsquare] & board.blackPawns);
              #endif
 
       }
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Return the score
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> SUMMARY:" << std::endl;
                     std::cout << "EVAL>  MATERIAL                :" << board.Material << std::endl;
                     std::cout << "EVAL>  UNEVEN MATERIAL BONUS   :" << iexch << std::endl;
                     std::cout << "EVAL>  WHITE POSITIONAL BONUS  :" << iwhitepos << std::endl;
                     std::cout << "EVAL>     WHITE PAWNS BONUS    :" << iwhitepawns << std::endl;
                     std::cout << "EVAL>     WHITE KING POS/SAF   :" << iwhiteking << std::endl;
                     std::cout << "EVAL>  BLACK POSITIONAL BONUS  :" << iblackpos << std::endl;
                     std::cout << "EVAL>     BLACK PAWNS BONUS    :" << iblackpawns << std::endl;
                     std::cout << "EVAL>     BLACK KING POS/SAF   :" << iblackking << std::endl;
       #endif
 
       if (board.nextMove) return -score;
       else return score;
 
}

int evalPieza(Move mov){
	//	===========================================================================
	// Asignar� un valor material parametrizable a cada tipo de pieza (cl�sicamente valD=9
	// para cada Dama, valT=5 para cada Torre, valA=valC=3 para cada Alfil y cada Caballo, y
	// TODO: VER EL VALOR DEL REY

	int piec = mov.getPiec();
	if(piec == WHITE_PAWN || piec == BLACK_PAWN) return PAWN_VALUE_U;
	if(piec == WHITE_KING || piec == BLACK_KING) return KING_VALUE_U;
	if(piec == WHITE_KNIGHT || piec == BLACK_KNIGHT) return KNIGHT_VALUE_U;
	if(piec == WHITE_BISHOP || piec == BLACK_BISHOP) return BISHOP_VALUE_U;
	if(piec == WHITE_ROOK || piec == BLACK_ROOK) return ROOK_VALUE_U;
	if(piec == WHITE_QUEEN || piec == BLACK_QUEEN) return QUEEN_VALUE_U;

	return 0;
	
}

int evalMaterial(bool & pIsDrawScore){
//	===========================================================================
// La evaluaci�n Material de la posici�n apunta a reflejar la diferencia de material entre los
// bandos, y deber� considerar por lo menos los siguientes elementos:
//	a) Asignar� un valor material parametrizable a cada tipo de pieza (cl�sicamente valD=9
//	   para cada Dama, valT=5 para cada Torre, valA=valC=3 para cada Alfil y cada Caballo, y
//	   valP=1 para cada Pe�n)
//	b) La evaluaci�n no puede ser m�s sencilla que la resultante de restarle al valor material
//	   total del Blanco el valor material total del Negro 
//	===========================================================================
//  Cuando se invoca a esta funci�n ya se realiz� un movimiento del jugador de turno
 
	
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// This is winglet's evaluation function
// The score is calculated from White's perspective (in centipawns)
// and returned from the perspective of the side to move.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       int score;
       int whitepawns, whiteknights, whitebishops, whiterooks, whitequeens;
       int blackpawns, blackknights, blackbishops, blackrooks, blackqueens;
       int whitetotalmat, blacktotalmat;
       int whitetotal, blacktotal;
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Material
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
       score = board.Material;
       #ifdef WINGLET_VERBOSE_EVAL
              std::cout << "EVAL> MATERIAL>                       " << board.Material << std::endl;
       #endif
  
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Piece counts, note that we could have done this incrementally in (un)makeMove
// because it's basically the same thing as keeping board.Material up to date..
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

       whitepawns = bitCnt(board.whitePawns);
       whiteknights = bitCnt(board.whiteKnights);
       whitebishops = bitCnt(board.whiteBishops);
       whiterooks = bitCnt(board.whiteRooks);
       whitequeens = bitCnt(board.whiteQueens);
	   whitetotalmat = QUEEN_VALUE_U*whitequeens + ROOK_VALUE_U*whiterooks + KNIGHT_VALUE_U*whiteknights + BISHOP_VALUE_U*whitebishops;
       whitetotal = whitepawns + whiteknights + whitebishops + whiterooks + whitequeens;
       
	   blackpawns = bitCnt(board.blackPawns);
       blackknights = bitCnt(board.blackKnights);
       blackbishops = bitCnt(board.blackBishops);
       blackrooks = bitCnt(board.blackRooks);
       blackqueens = bitCnt(board.blackQueens);
       blacktotalmat = QUEEN_VALUE_U*blackqueens + ROOK_VALUE_U*blackrooks + KNIGHT_VALUE_U*blackknights + BISHOP_VALUE_U*blackbishops;
       blacktotal = blackpawns + blackknights + blackbishops + blackrooks + blackqueens;
       
		#ifdef WINGLET_VERBOSE_EVAL
              std::cout << "EVAL> WHITE TOTAL MAT>                " << whitetotalmat << std::endl;
              std::cout << "EVAL> BLACK TOTAL MAT>                " << blacktotalmat << std::endl;
              std::cout << "EVAL> WHITE TOTAL>                    " << whitetotal << std::endl;
              std::cout << "EVAL> BLACK TOTAL>                    " << blacktotal << std::endl;
       #endif
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate for draws due to insufficient material:
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	pIsDrawScore = false; // no es empate
	if (!whitepawns && !blackpawns) {// si no hay peones
		// king versus king:
		if ((whitetotalmat == 0) && (blacktotalmat == 0)) { // si s�lo estan los reyes
			pIsDrawScore = true; // es empate
			if (board.nextMove) return -DRAWSCORE; // si le toca jugar a las negras --> retorna negativo
			else return DRAWSCORE; // le toca jugar a las blancas --> retorna positivo
		}
 
		// king and knight versus king:
		if (((whitetotalmat == 3) && (whiteknights == 1) && (blacktotalmat == 0)) ||
			((blacktotalmat == 3) && (blackknights == 1) && (whitetotalmat == 0))) 	{
			pIsDrawScore = true;
			if (board.nextMove) return -DRAWSCORE;
			else return DRAWSCORE;
		}
 
		// 2 kings with one or more bishops, and all bishops on the same colour:
		if ((whitebishops + blackbishops) > 0) {
				if ((whiteknights == 0) && (whiterooks == 0) && (whitequeens == 0) &&
				(blackknights == 0) && (blackrooks == 0) && (blackqueens == 0))	{
				if (!((board.whiteBishops | board.blackBishops) & WHITE_SQUARES) ||
					!((board.whiteBishops | board.blackBishops) & BLACK_SQUARES)) {
					pIsDrawScore = true;
					if (board.nextMove) return -DRAWSCORE;
					else return DRAWSCORE;
				}
			}
		}
	}
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate MATERIAL
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Have the winning side prefer to exchange pieces
// Every exchange with unequal material adds 3 centipawns to the score
// Loosing a piece (from balanced material) becomes more
// severe in the endgame
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       #ifdef WINGLET_VERBOSE_EVAL
              int iexch = 0;
              int iwhitepos = 0;
              int iwhitepawns = 0;
              int iwhiteking = 0;
              int iblackpos = 0;
              int iblackpawns = 0;
              int iblackking = 0;
       #endif
 
       if (whitetotalmat + whitepawns > blacktotalmat + blackpawns){ // si los blancos tienen mas valor en materiales -> 
              score += 45 + 3 * whitetotal - 6 * blacktotal;
              #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> EXCHANGE WHITE>                 " << (45 + 3 * whitetotal - 6 * blacktotal) << std::endl;
                     iexch = (45 + 3 * whitetotal - 6 * blacktotal);
              #endif
       } else {
              if (whitetotalmat + whitepawns < blacktotalmat + blackpawns) {
                     score += -45 - 3 * blacktotal + 6 * whitetotal;
                     #ifdef WINGLET_VERBOSE_EVAL
                           std::cout << "EVAL> EXCHANGE BLACK>                 " << (-45 - 3 * blacktotal + 6 * whitetotal) << std::endl;
                           iexch = (-45 - 3 * blacktotal + 6 * whitetotal);
                     #endif
              }
       }

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Return the score
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
       #ifdef WINGLET_VERBOSE_EVAL
                     std::cout << "EVAL> SUMMARY:" << std::endl;
                     std::cout << "EVAL>  MATERIAL                :" << board.Material << std::endl;
                     std::cout << "EVAL>  UNEVEN MATERIAL BONUS   :" << iexch << std::endl;
                     std::cout << "EVAL>  WHITE POSITIONAL BONUS  :" << iwhitepos << std::endl;
                     std::cout << "EVAL>     WHITE PAWNS BONUS    :" << iwhitepawns << std::endl;
                     std::cout << "EVAL>     WHITE KING POS/SAF   :" << iwhiteking << std::endl;
                     std::cout << "EVAL>  BLACK POSITIONAL BONUS  :" << iblackpos << std::endl;
                     std::cout << "EVAL>     BLACK PAWNS BONUS    :" << iblackpawns << std::endl;
                     std::cout << "EVAL>     BLACK KING POS/SAF   :" << iblackking << std::endl;
       #endif
 
      return score;

}

int evalEspacial(int pIndexMoveBufLen){
//	===========================================================================
// La evaluaci�n Espacial de la posici�n apunta a reflejar la diferencia en la cantidad de casillas
// que controlan los bandos, y deber� considerar por lo menos aqu�llas casillas controladas de
// forma indisputada por un bando y otro.
//	===========================================================================
	// Cuando se invoca a esta funci�n ya se realiz� un movimiento del jugador de turno ->
	// cargo en el array todos los casilleros alcanzables por el jugador del oponente
	// realizo un movimiento del oponente
	// para cada destino del oponente veo si ese casillero no es alcanzable por el jugador de turno. (si es alcanzable por el jugador de turno 
	// -> marco false ese casillero para jugador de turno y el oponente)

	int i, totalCurrentMoves = 0, totalOponentMoves = 0;	
	bool reachablesCurrent[64]; // vector donde cada casillero representa un casillero del tablero, si es true -> ese casillero es alcanzado por el jugador de turno
	bool reachablesOponent[64];

	Move validOponentMove;
	Move dummy;
	
	//board.display();
	//=============================== reachables for oponent player
	dummy.clear();
	// inicializo todo el tablero como no alcanzable por el oponente
	for (i = 0; i < 64; i++){
		reachablesOponent[i] = false;
	}

	/************ obtengo todos los movimientos posibles del oponente **********************/
	int lIndexMoveBufLen = generarMovimientosPosibles(pIndexMoveBufLen);
	/************ FIN obtengo todos los movimientos posibles del oponente **********************/
	for (i = board.moveBufLen[lIndexMoveBufLen]; i < board.moveBufLen[lIndexMoveBufLen + 1]; i++){  // i = �ndice donde comienzan los movimientos generados
		// para cada movimiento posible del oponente
		dummy = board.moveBuffer[i];
		makeMove(dummy);
		if (!isOtherKingAttacked()){
			// si es movimiento valido			
			if (!reachablesOponent[dummy.getTosq()]) { // si ya no lo marque
				reachablesOponent[dummy.getTosq()] = true;
				totalOponentMoves++;
			}
			validOponentMove = dummy;
		}
		unmakeMove(dummy);
	}

	//=============================== reachables for current player
	if(totalOponentMoves > 0){ 
		// si existe alg�n movimiento posible del oponente -> lo realizo pasando el turno al jugador actual y obtenemos todos sus movimientos
		makeMove(validOponentMove);

		dummy.clear();
		// inicializo todo el tablero como no alcanzable por el turno actual
		for (i = 0; i < 64; i++){
			reachablesCurrent[i] = false;
		}
		
		/************ obtengo todos los movimientos posibles del turno actual **********************/
		int lIndexMoveBufLen2 = generarMovimientosPosibles(lIndexMoveBufLen + 1);
		/************ FIN obtengo todos los movimientos posibles del turno actual **********************/  

		for (i = board.moveBufLen[lIndexMoveBufLen2]; i < board.moveBufLen[lIndexMoveBufLen2 + 1]; i++){ // i = �ndice donde comienzan los movimientos generados
			// para cada movimiento posible del turno actual
			dummy = board.moveBuffer[i];
			makeMove(dummy);
			if (!isOtherKingAttacked()){
				// si es movimiento valido
				if(reachablesOponent[dummy.getTosq()] == true){
					// si ya lo habia marcado para el oponente deja de ser casillero disponible para el actual y el oponente
					// -> marco el casillero como ocupado, as� no lo utilizo de nuevo y
					// disminuyo uno el total de casilleros que puede acceder el oponente y marco como utilizado el casillero del actual, as� no suma nuevamente.
					reachablesCurrent[dummy.getTosq()] = true;
					reachablesOponent[dummy.getTosq()] = false;
					totalOponentMoves--;
				}
				else { 
					if (!reachablesCurrent[dummy.getTosq()]) { // si ya no lo marque
						reachablesCurrent[dummy.getTosq()] = true;
						totalCurrentMoves++;
					}
				}
			}
			unmakeMove(dummy);
		}

		// vuelvo el turno al oponente, afuera de esta funci�n se realiza un unmakemove de la guada a evaluar y se vuelve el turno al jugador que le tocaba jugar.
		unmakeMove(validOponentMove);
	}

	
//	return totalCurrentMoves - totalOponentMoves; // return the score relative to the side to move
	if (board.nextMove == WHITE_MOVE){
		return totalOponentMoves - totalCurrentMoves;
	} else {
		return totalCurrentMoves - totalOponentMoves;
	}
}

int evalDinamica(int pIndexMoveBufLen){
//	===========================================================================
// La evaluaci�n Din�mica de la posici�n apunta a reflejar la diferencia de movilidad existente
// entre los bandos. Una forma posible de realizar esta evaluaci�n consiste en computar, para
// cada bando, la cantidad de jugadas legales posibles de sus piezas, multiplic�ndolas por el valor
// de las mismas (por ejemplo: si una Torre puede realizar 7 jugadas legales, dicha Torre aportar�
// 7*valT al total de movilidad de su bando).
//	===========================================================================

	int i, totalCurrentMoves = 0, totalOponentMoves = 0;
	Move validOponentMove;
	Move dummy;
	
	//=============================== reachables for oponent player
	dummy.clear();

	/************ obtengo todos los movimientos posibles del oponente **********************/
	int lIndexMoveBufLen = generarMovimientosPosibles(pIndexMoveBufLen);
	/************ FIN obtengo todos los movimientos posibles del oponente **********************/

	for (i = board.moveBufLen[lIndexMoveBufLen]; i < board.moveBufLen[lIndexMoveBufLen + 1]; i++){  // i = �ndice donde comienzan los movimientos generados
		// para cada movimiento posible del oponente
		dummy = board.moveBuffer[i];
		makeMove(dummy);
		if (!isOtherKingAttacked()){
			// si es movimiento valido			
			totalOponentMoves += evalPieza(dummy);
			validOponentMove = dummy;
		}
		unmakeMove(dummy);
	}

	//=============================== reachables for current player

	if(totalOponentMoves > 0){ 
		// si existe alg�n movimiento posible del oponente -> lo realizo pasando el turno al jugador actual y obtenemos todos sus movimientos
		makeMove(validOponentMove);
		dummy.clear();

		/************ obtengo todos los movimientos posibles del turno actual **********************/
		int lIndexMoveBufLen2 = generarMovimientosPosibles(lIndexMoveBufLen + 1);
		/************ FIN obtengo todos los movimientos posibles del turno actual **********************/  

		for (i = board.moveBufLen[lIndexMoveBufLen2]; i < board.moveBufLen[lIndexMoveBufLen2 + 1]; i++){ // i = �ndice donde comienzan los movimientos generados
			// para cada movimiento posible del turno actual
			dummy = board.moveBuffer[i];
			makeMove(dummy);
			if (!isOtherKingAttacked()){
				// si es movimiento valido
				totalCurrentMoves += evalPieza(dummy);
			}
			unmakeMove(dummy);
		}

		// vuelvo el turno al oponente, afuera de esta funci�n se realiza un unmakemove de la guada a evaluar y se vuelve el turno al jugador que le tocaba jugar.
		unmakeMove(validOponentMove);
	}
	//score =  totalCurrentMoves - totalOponentMoves; 
	//corregido el retorno, estaba devolviendo valores siempre del lado de blancas...
	//if (board.nextMove) return score;
 //   else return -score;
	if (board.nextMove == WHITE_MOVE){
		return totalOponentMoves - totalCurrentMoves;
	} else {
		return totalCurrentMoves - totalOponentMoves;
	}
}

int evalPosicionDiff(){
	int score, square;
	int whitekingsquare, blackkingsquare;
	BOOLTYPE endgame;
	BitMap temp, whitepassedpawns, blackpassedpawns;

	int whitepawns, whiteknights, whitebishops, whiterooks, whitequeens;
    int blackpawns, blackknights, blackbishops, blackrooks, blackqueens;
    int whitetotalmat, blacktotalmat;

	//Move currentMove, controlMove;
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Remember where the kings are
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
    whitekingsquare = firstOne(board.whiteKing);
    #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL> WHITE KING POSITION>            " << SQUARENAME[whitekingsquare] << std::endl;
    #endif
    blackkingsquare = firstOne(board.blackKing);
    #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL> BLACK KING POSITION>            " << SQUARENAME[blackkingsquare] << std::endl;
    #endif
	whitepawns = bitCnt(board.whitePawns);
	whiteknights = bitCnt(board.whiteKnights);
	whitebishops = bitCnt(board.whiteBishops);
	whiterooks = bitCnt(board.whiteRooks);
	whitequeens = bitCnt(board.whiteQueens);
	whitetotalmat = KNIGHT_VALUE_U * whiteknights + BISHOP_VALUE_U * whitebishops + ROOK_VALUE_U * whiterooks + QUEEN_VALUE_U * whitequeens;
	   
	blackpawns = bitCnt(board.blackPawns);
	blackknights = bitCnt(board.blackKnights);
	blackbishops = bitCnt(board.blackBishops);
	blackrooks = bitCnt(board.blackRooks);
	blackqueens = bitCnt(board.blackQueens);
	blacktotalmat = KNIGHT_VALUE_U * blackknights + BISHOP_VALUE_U * blackbishops + ROOK_VALUE_U * blackrooks + QUEEN_VALUE_U * blackqueens;

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Chequeamos si estamos en el final del juego
	// Cualquier valor menor que la reina (=10) + torre (=5) + alfinl o caballo (=3) es considerado final de juego
	// (pawns excluded in this count)
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	endgame = (whitetotalmat < 18 || blacktotalmat < 18);
	#ifdef WINGLET_VERBOSE_EVAL
              std::cout << "EVAL> ENDGAME>                        " << endgame << std::endl;
    #endif



// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate WHITE PIECES
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate white pawns
// - position on the board
// - distance from opponent king
// - distance from own king
// - passed, doubled, isolated or backward pawns
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	score = 0;
    whitepassedpawns = 0;
    temp = board.whitePawns;
    while (temp){
        square = firstOne(temp);
 
        #ifdef WINGLET_VERBOSE_EVAL
           std::cout << "EVAL> WHITE PAWN ON                   " << SQUARENAME[square] << std::endl;
        #endif
 
        score += PAWNPOS_W[square];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  POSITION SCORE IS              " << PAWNPOS_W[square] << std::endl;
            iwhitepos += PAWNPOS_W[square];
            iwhitepawns += PAWNPOS_W[square];
        #endif
 
        score += PAWN_OPPONENT_DISTANCE[DISTANCE[square][blackkingsquare]];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << PAWN_OPPONENT_DISTANCE[DISTANCE[square][blackkingsquare]] << std::endl;
            iwhitepos += PAWN_OPPONENT_DISTANCE[DISTANCE[square][blackkingsquare]];
            iwhitepawns += PAWN_OPPONENT_DISTANCE[DISTANCE[square][blackkingsquare]];
        #endif
 
        if (endgame){
            score += PAWN_OWN_DISTANCE[DISTANCE[square][whitekingsquare]];
            #ifdef WINGLET_VERBOSE_EVAL
                std::cout << "EVAL>  DISTANCE TO OWN KING SCORE     " << PAWN_OWN_DISTANCE[DISTANCE[square][blackkingsquare]] << std::endl;
                iwhitepos += PAWN_OWN_DISTANCE[DISTANCE[square][whitekingsquare]];
                iwhitepawns += PAWN_OWN_DISTANCE[DISTANCE[square][whitekingsquare]];
            #endif
        }
 
        if (!(PASSED_WHITE[square] & board.blackPawns)) {
            score += BONUS_PASSED_PAWN;
            #ifdef WINGLET_VERBOSE_EVAL
                std::cout << "EVAL>  IS PASSED, BONUS IS            " << BONUS_PASSED_PAWN << std::endl;
                iwhitepos += BONUS_PASSED_PAWN;
                iwhitepawns += BONUS_PASSED_PAWN;
            #endif
			// remember its location, we need it later when evaluating the white rooks:
            whitepassedpawns ^= BITSET[square];
        }
 
        if ((board.whitePawns ^ BITSET[square]) & FILEMASK[square]) {
            score -= PENALTY_DOUBLED_PAWN;
            #ifdef WINGLET_VERBOSE_EVAL
                std::cout << "EVAL>  IS DOUBLED, PENALTY IS         " << PENALTY_DOUBLED_PAWN << std::endl;
                iwhitepos -= PENALTY_DOUBLED_PAWN;
                iwhitepawns -= PENALTY_DOUBLED_PAWN;
            #endif
        }
 
        if (!(ISOLATED_WHITE[square] & board.whitePawns)){
            score -= PENALTY_ISOLATED_PAWN;
            #ifdef WINGLET_VERBOSE_EVAL
                std::cout << "EVAL>  IS ISOLATED, PENALTY IS        " << PENALTY_ISOLATED_PAWN << std::endl;
                iwhitepos -= PENALTY_ISOLATED_PAWN;
                iwhitepawns -= PENALTY_ISOLATED_PAWN;
            #endif
        } else {
// If it is not isolated, then it might be backward. Two conditions must be true:
//  1) if the next square is controlled by an enemy pawn - we use the PAWN_ATTACKS bitmaps to check this
//  2) if there are no pawns left that could defend this pawn
            if ((WHITE_PAWN_ATTACKS[square + 8] & board.blackPawns)) {
                if (!(BACKWARD_WHITE[square] & board.whitePawns)) {
                    score -= PENALTY_BACKWARD_PAWN;
                    #ifdef WINGLET_VERBOSE_EVAL
                        std::cout << "EVAL>  IS BACKWARD, PENALTY IS        " << PENALTY_BACKWARD_PAWN << std::endl;
                        iwhitepos -= PENALTY_BACKWARD_PAWN;
                        iwhitepawns -= PENALTY_BACKWARD_PAWN;
                    #endif
                }
            }
        }
 
        temp ^= BITSET[square];
	}
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate white knights
// - position on the board
// - distance from opponent king
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
	temp = board.whiteKnights;
	while (temp){
        square = firstOne(temp);
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL> WHITE KNIGHT ON                 " << SQUARENAME[square] << std::endl;
        #endif
 
        score += KNIGHTPOS_W[square];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  POSITION SCORE IS              " << KNIGHTPOS_W[square] << std::endl;
            iwhitepos += KNIGHTPOS_W[square];
        #endif
 
        score += KNIGHT_DISTANCE[DISTANCE[square][blackkingsquare]];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << KNIGHT_DISTANCE[DISTANCE[square][blackkingsquare]] << std::endl;
            iwhitepos += KNIGHT_DISTANCE[DISTANCE[square][blackkingsquare]];
        #endif
 
        temp ^= BITSET[square];
	}
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate white bishops
// - having the pair
// - position on the board
// - distance from opponent king
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
	if (board.whiteBishops){
		if ((board.whiteBishops & WHITE_SQUARES) && (board.whiteBishops & BLACK_SQUARES)){
            score += BONUS_BISHOP_PAIR;
            #ifdef WINGLET_VERBOSE_EVAL
                std::cout << "EVAL> WHITE BISHOP PAIR BONUS         " << BONUS_BISHOP_PAIR << std::endl;
                iwhitepos += BONUS_BISHOP_PAIR;
            #endif              
        }
	}
 
	temp = board.whiteBishops;
	while (temp){
        square = firstOne(temp);
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL> WHITE BISHOP ON                 " << SQUARENAME[square] << std::endl;
        #endif
 
        score += BISHOPPOS_W[square];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  POSITION SCORE IS              " << BISHOPPOS_W[square] << std::endl;
            iwhitepos += BISHOPPOS_W[square];
        #endif
 
        score += BISHOP_DISTANCE[DISTANCE[square][blackkingsquare]];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << BISHOP_DISTANCE[DISTANCE[square][blackkingsquare]] << std::endl;
            iwhitepos += BISHOP_DISTANCE[DISTANCE[square][blackkingsquare]];
        #endif
 
        temp ^= BITSET[square];
	}
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate white rooks
// - position on the board
// - distance from opponent king
// - on the same file as a passed pawn
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
	temp = board.whiteRooks;
	while (temp){
        square = firstOne(temp);
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL> WHITE ROOK ON                   " << SQUARENAME[square] << std::endl;
        #endif
 
        score += ROOKPOS_W[square];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  POSITION SCORE IS              " << ROOKPOS_W[square] << std::endl;
            iwhitepos += ROOKPOS_W[square];
        #endif
 
        score += ROOK_DISTANCE[DISTANCE[square][blackkingsquare]];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << ROOK_DISTANCE[DISTANCE[square][blackkingsquare]] << std::endl;
            iwhitepos += ROOK_DISTANCE[DISTANCE[square][blackkingsquare]];
        #endif
 
        if (FILEMASK[square] & whitepassedpawns){
            if ((unsigned int) square < lastOne(FILEMASK[square] & whitepassedpawns)) {
                score += BONUS_ROOK_BEHIND_PASSED_PAWN;
                #ifdef WINGLET_VERBOSE_EVAL
                    std::cout << "EVAL>  BEHIND PASSED PAWN BONUS       " << BONUS_ROOK_BEHIND_PASSED_PAWN << std::endl;
                    iwhitepos += BONUS_ROOK_BEHIND_PASSED_PAWN;
                #endif
            }
        }
        temp ^= BITSET[square];
	}
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate white queens
// - position on the board
// - distance from opponent king
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
	temp = board.whiteQueens;
	while (temp){
        square = firstOne(temp);
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL> WHITE QUEEN ON                  " << SQUARENAME[square] << std::endl;
        #endif
 
        score += QUEENPOS_W[square];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  POSITION SCORE IS              " << QUEENPOS_W[square] << std::endl;
            iwhitepos += QUEENPOS_W[square];
        #endif
 
        score += QUEEN_DISTANCE[DISTANCE[square][blackkingsquare]];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << QUEEN_DISTANCE[DISTANCE[square][blackkingsquare]] << std::endl;
            iwhitepos += QUEEN_DISTANCE[DISTANCE[square][blackkingsquare]];
        #endif
 
        temp ^= BITSET[square];
	}
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate the white king
// - position on the board
// - proximity to the pawns
// - pawn shield (not in the endgame)
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
	if (endgame){
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL> WHITE KING ON (endgame)         " << SQUARENAME[whitekingsquare] << std::endl;
        #endif
 
        score += KINGPOS_ENDGAME_W[whitekingsquare];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  POSITION SCORE IS              " << KINGPOS_ENDGAME_W[whitekingsquare] << std::endl;
            iwhitepos += KINGPOS_ENDGAME_W[whitekingsquare];
            iwhiteking += KINGPOS_ENDGAME_W[whitekingsquare];
        #endif
 
	} else {
        #ifdef WINGLET_VERBOSE_EVAL
			std::cout << "EVAL> WHITE KING ON (no endgame)      " << SQUARENAME[whitekingsquare] << std::endl;
        #endif
 
        score += KINGPOS_W[whitekingsquare];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  POSITION SCORE IS              " << KINGPOS_W[whitekingsquare] << std::endl;
            iwhitepos += KINGPOS_W[whitekingsquare];
            iwhiteking += KINGPOS_W[whitekingsquare];
        #endif
 
        // add pawn shield bonus if we're not in the endgame:
        // strong pawn shield bonus if the pawns are near the king:
        score += BONUS_PAWN_SHIELD_STRONG * bitCnt(KINGSHIELD_STRONG_W[whitekingsquare] & board.whitePawns);
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  STRONG PAWN SHIELD SCORE IS    " << BONUS_PAWN_SHIELD_STRONG * bitCnt(KINGSHIELD_STRONG_W[whitekingsquare] & board.whitePawns) << std::endl;
            iwhitepos += BONUS_PAWN_SHIELD_STRONG * bitCnt(KINGSHIELD_STRONG_W[whitekingsquare] & board.whitePawns);
            iwhiteking += BONUS_PAWN_SHIELD_STRONG * bitCnt(KINGSHIELD_STRONG_W[whitekingsquare] & board.whitePawns);
        #endif
 
        // weaker pawn shield bonus if the pawns are not so near the king:
        score += BONUS_PAWN_SHIELD_WEAK * bitCnt(KINGSHIELD_WEAK_W[whitekingsquare] & board.whitePawns);
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  WEAK PAWN SHIELD SCORE IS      " << BONUS_PAWN_SHIELD_WEAK * bitCnt(KINGSHIELD_WEAK_W[whitekingsquare] & board.whitePawns) << std::endl;
            iwhitepos += BONUS_PAWN_SHIELD_WEAK * bitCnt(KINGSHIELD_WEAK_W[whitekingsquare] & board.whitePawns);
            iwhiteking += BONUS_PAWN_SHIELD_WEAK * bitCnt(KINGSHIELD_WEAK_W[whitekingsquare] & board.whitePawns);
        #endif
	}
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate BLACK PIECES
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate black pawns
// - position on the board
// - distance from opponent king
// - distance from own king
// - passed, doubled, isolated or backward pawns
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
	blackpassedpawns = 0;
	temp = board.blackPawns;
	while (temp) {
        square = firstOne(temp);
 
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL> BLACK PAWN ON                   " << SQUARENAME[square] << std::endl;
        #endif
 
        score -= PAWNPOS_B[square];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  POSITION SCORE IS              " << PAWNPOS_B[square] << std::endl;
            iblackpos -= PAWNPOS_B[square];
            iblackpawns -= PAWNPOS_B[square];
        #endif
 
        score -= PAWN_OPPONENT_DISTANCE[DISTANCE[square][whitekingsquare]];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << PAWN_OPPONENT_DISTANCE[DISTANCE[square][whitekingsquare]] << std::endl;
            iblackpos -= PAWN_OPPONENT_DISTANCE[DISTANCE[square][whitekingsquare]];
            iblackpawns -= PAWN_OPPONENT_DISTANCE[DISTANCE[square][whitekingsquare]];
        #endif
 
        if (endgame) {
            score -= PAWN_OWN_DISTANCE[DISTANCE[square][blackkingsquare]];
            #ifdef WINGLET_VERBOSE_EVAL
                std::cout << "EVAL>  DISTANCE TO OWN KING SCORE     " << PAWN_OWN_DISTANCE[DISTANCE[square][blackkingsquare]] << std::endl;
                iblackpos -= PAWN_OWN_DISTANCE[DISTANCE[square][blackkingsquare]];
                iblackpawns -= PAWN_OWN_DISTANCE[DISTANCE[square][blackkingsquare]];
			#endif
        }
 
        if (!(PASSED_BLACK[square] & board.whitePawns)) {
            score -= BONUS_PASSED_PAWN;
            #ifdef WINGLET_VERBOSE_EVAL
                std::cout << "EVAL>  IS PASSED, BONUS IS            " << BONUS_PASSED_PAWN << std::endl;
                iblackpos -= BONUS_PASSED_PAWN;
                iblackpawns -= BONUS_PASSED_PAWN;
            #endif
// remember its location, we need it later when evaluating the black rooks:
            blackpassedpawns ^= BITSET[square];
        }
 
        if ((board.blackPawns ^ BITSET[square]) & FILEMASK[square]) {
            score += PENALTY_DOUBLED_PAWN;
            #ifdef WINGLET_VERBOSE_EVAL
                std::cout << "EVAL>  IS DOUBLED, PENALTY IS         " << PENALTY_DOUBLED_PAWN << std::endl;
                iblackpos += PENALTY_DOUBLED_PAWN;
                iblackpawns += PENALTY_DOUBLED_PAWN;
            #endif
        }
 
        if (!(ISOLATED_BLACK[square] & board.blackPawns)) {
            score += PENALTY_ISOLATED_PAWN;
            #ifdef WINGLET_VERBOSE_EVAL
                std::cout << "EVAL>  IS ISOLATED, PENALTY IS        " << PENALTY_ISOLATED_PAWN << std::endl;
                iblackpos += PENALTY_ISOLATED_PAWN;
                iblackpawns += PENALTY_ISOLATED_PAWN;
            #endif
        } else {
// If it is not isolated, then it might be backward. Two conditions must be true:
//  1) if the next square is controlled by an enemy pawn - we use the PAWN_ATTACKS bitmaps to check this
//  2) if there are no pawns left that could defend this pawn
			if ((BLACK_PAWN_ATTACKS[square - 8] & board.whitePawns)) {
                if (!(BACKWARD_BLACK[square] & board.blackPawns)) {
                    score += PENALTY_BACKWARD_PAWN;
                    #ifdef WINGLET_VERBOSE_EVAL
                            std::cout << "EVAL>  IS BACKWARD, PENALTY IS        " << PENALTY_BACKWARD_PAWN << std::endl;
                            iwhitepos += PENALTY_BACKWARD_PAWN;
                            iwhitepawns += PENALTY_BACKWARD_PAWN;
                    #endif
                }
            }
        }
        temp ^= BITSET[square]; 
	}
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate black knights
// - position on the board
// - distance from opponent king
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
	temp = board.blackKnights;
	while (temp) {
        square = firstOne(temp);
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL> BLACK KNIGHT ON                 " << SQUARENAME[square] << std::endl;
        #endif
 
        score -= KNIGHTPOS_B[square];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  POSITION SCORE IS              " << KNIGHTPOS_B[square] << std::endl;
            iblackpos -= KNIGHTPOS_B[square];
        #endif
 
        score -= KNIGHT_DISTANCE[DISTANCE[square][whitekingsquare]];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << KNIGHT_DISTANCE[DISTANCE[square][whitekingsquare]] << std::endl;
            iblackpos -= KNIGHT_DISTANCE[DISTANCE[square][whitekingsquare]];
        #endif
 
        temp ^= BITSET[square];
	}
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate black bishops
// - having the pair
// - position on the board
// - distance from opponent king
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
	if (board.blackBishops) {
		if ((board.blackBishops & WHITE_SQUARES) && (board.blackBishops & BLACK_SQUARES)) {
            score -= BONUS_BISHOP_PAIR;
            #ifdef WINGLET_VERBOSE_EVAL
                std::cout << "EVAL> BLACK BISHOP PAIR BONUS         " << BONUS_BISHOP_PAIR << std::endl;
                iblackpos -= BONUS_BISHOP_PAIR;
            #endif              
        }
	}
 
	temp = board.blackBishops;
	while (temp) {
        square = firstOne(temp);
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL> BLACK BISHOP ON                 " << SQUARENAME[square] << std::endl;
        #endif
 
        score -= BISHOPPOS_B[square];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  POSITION SCORE IS              " << BISHOPPOS_B[square] << std::endl;
            iblackpos -= BISHOPPOS_B[square];
        #endif
 
        score -= BISHOP_DISTANCE[DISTANCE[square][whitekingsquare]];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << BISHOP_DISTANCE[DISTANCE[square][whitekingsquare]] << std::endl;
            iblackpos -= BISHOP_DISTANCE[DISTANCE[square][whitekingsquare]];
        #endif
 
        temp ^= BITSET[square];
	}
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate black rooks
// - position on the board
// - distance from opponent king
// - on the same file as a passed pawn
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
	temp = board.blackRooks;
	while (temp) {
        square = firstOne(temp);
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL> BLACK ROOK ON                   " << SQUARENAME[square] << std::endl;
        #endif
 
        score -= ROOKPOS_B[square];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  POSITION SCORE IS              " << ROOKPOS_B[square] << std::endl;
            iblackpos -= ROOKPOS_B[square];
        #endif
 
        score -= ROOK_DISTANCE[DISTANCE[square][whitekingsquare]];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << ROOK_DISTANCE[DISTANCE[square][whitekingsquare]] << std::endl;
            iblackpos -= ROOK_DISTANCE[DISTANCE[square][whitekingsquare]];
        #endif
 
        if (FILEMASK[square] & blackpassedpawns) {
            if ((unsigned int) square > firstOne(FILEMASK[square] & blackpassedpawns)) {
                score -= BONUS_ROOK_BEHIND_PASSED_PAWN;
                #ifdef WINGLET_VERBOSE_EVAL
                    std::cout << "EVAL>  BEHIND PASSED PAWN BONUS       " << BONUS_ROOK_BEHIND_PASSED_PAWN << std::endl;
                    iblackpos -= BONUS_ROOK_BEHIND_PASSED_PAWN;
                #endif
            }
        }
        temp ^= BITSET[square];
	}
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate black queens
// - position on the board
// - distance from opponent king
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
	temp = board.blackQueens;
	while (temp) {
        square = firstOne(temp);
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL> BLACK QUEEN ON                  " << SQUARENAME[square] << std::endl;
        #endif
 
        score -= QUEENPOS_B[square];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  POSITION SCORE IS              " << QUEENPOS_B[square] << std::endl;
            iblackpos -= QUEENPOS_B[square];
        #endif
 
        score -= QUEEN_DISTANCE[DISTANCE[square][whitekingsquare]];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  DISTANCE TO OPPONNT KING SCORE " << QUEEN_DISTANCE[DISTANCE[square][whitekingsquare]] << std::endl;
            iblackpos -= QUEEN_DISTANCE[DISTANCE[square][whitekingsquare]];
        #endif
 
        temp ^= BITSET[square];
	}
 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Evaluate the black king
// - position on the board
// - proximity to the pawns
// - pawn shield (not in the endgame)
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
	if (endgame) {
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL> BLACK KING ON (endgame)         " << SQUARENAME[blackkingsquare] << std::endl;
        #endif
 
        score -= KINGPOS_ENDGAME_B[blackkingsquare];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  POSITION SCORE IS              " << KINGPOS_ENDGAME_B[blackkingsquare] << std::endl;
            iblackpos -= KINGPOS_ENDGAME_B[blackkingsquare];
            iblackking -= KINGPOS_ENDGAME_B[blackkingsquare];
        #endif
	} else {
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL> BLACK KING ON (no endgame)      " << SQUARENAME[blackkingsquare] << std::endl;
        #endif
 
        score -= KINGPOS_B[blackkingsquare];
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  POSITION SCORE IS              " << KINGPOS_B[blackkingsquare] << std::endl;
            iblackpos -= KINGPOS_B[blackkingsquare];
            iblackking -= KINGPOS_B[blackkingsquare];
        #endif
 
        // add pawn shield bonus if we're not in the endgame:
        // strong pawn shield bonus if the pawns are near the king:
        score -= BONUS_PAWN_SHIELD_STRONG * bitCnt(KINGSHIELD_STRONG_B[blackkingsquare] & board.blackPawns);
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  STRONG PAWN SHIELD SCORE IS    " << BONUS_PAWN_SHIELD_STRONG * bitCnt(KINGSHIELD_STRONG_B[blackkingsquare] & board.blackPawns) << std::endl;
            iblackpos -= BONUS_PAWN_SHIELD_STRONG * bitCnt(KINGSHIELD_STRONG_B[blackkingsquare] & board.blackPawns);
            iblackking -= BONUS_PAWN_SHIELD_STRONG * bitCnt(KINGSHIELD_STRONG_B[blackkingsquare] & board.blackPawns);
        #endif
 
        // weaker pawn shield bonus if the pawns are not so near the king:
        score -= BONUS_PAWN_SHIELD_WEAK * bitCnt(KINGSHIELD_WEAK_B[blackkingsquare] & board.blackPawns);
        #ifdef WINGLET_VERBOSE_EVAL
            std::cout << "EVAL>  WEAK PAWN SHIELD SCORE IS      " << BONUS_PAWN_SHIELD_WEAK * bitCnt(KINGSHIELD_WEAK_B[blackkingsquare] & board.blackPawns) << std::endl;
            iblackpos -= BONUS_PAWN_SHIELD_WEAK * bitCnt(KINGSHIELD_WEAK_B[blackkingsquare] & board.blackPawns);
            iblackking -= BONUS_PAWN_SHIELD_WEAK * bitCnt(KINGSHIELD_WEAK_B[blackkingsquare] & board.blackPawns);
        #endif
    }
	


	// si hay promoci�n bonifico en endgame
	
	Move currentMove = board.gameLine[board.endOfGame].move;
	if (endgame) { // si 
		if (currentMove.isPromotion()){
				if (board.nextMove == WHITE_MOVE) {
					score -= 50;
				} else {
					score += 50;
				}
		}
	}
	
	return score;

}

int Board::evalJL(int pa1, int pa2, int pa3, int pa4, int pIndexMoveBufLen){
	//	===========================================================================
	// An evaluation function is used to heuristically determine the relative value of a position, i.e. the chances of winning.
	// If we could see to the end of the game in every line, the evaluation would only have values of -1 (loss), 0 (draw), and 1 (win).
	// In practice, however, we do not know the exact value of a position, so we must make an approximation.
	// Beginning chess players learn to do this starting with the value of the pieces themselves. 
	// Computer evaluation functions also use the value of the material as the most significant aspect and then add other considerations.
	//	===========================================================================

	int score;
	score = 0;
	bool isDrawScore;
	
	score = evalMaterial(isDrawScore);

	if(!isDrawScore){
		score = pa1*score;
		score += pa2*evalEspacial(pIndexMoveBufLen);
		score += pa3*evalDinamica(pIndexMoveBufLen);//revisada 3/11
		score += pa4*evalPosicionDiff();
	}
	 // return the score relative to the side to move
	if (board.nextMove == BLACK_MOVE){
		return -score;
	} else {
		return score;
	}
}

double Board::evalJLD(double pa1, double pa2, double pa3, double pa4, int pIndexMoveBufLen){
	//	===========================================================================
	// An evaluation function is used to heuristically determine the relative value of a position, i.e. the chances of winning.
	// If we could see to the end of the game in every line, the evaluation would only have values of -1 (loss), 0 (draw), and 1 (win).
	// In practice, however, we do not know the exact value of a position, so we must make an approximation.
	// Beginning chess players learn to do this starting with the value of the pieces themselves. 
	// Computer evaluation functions also use the value of the material as the most significant aspect and then add other considerations.
	//	===========================================================================

	double score;
	score = 0;
	bool isDrawScore;
	
	score = evalMaterial(isDrawScore);

	if(!isDrawScore){
		score = pa1*score;
		score += pa2*evalEspacial(pIndexMoveBufLen);
		score += pa3*evalDinamica(pIndexMoveBufLen);
		score += pa4*evalPosicionDiff();
	}
	
	// return the score relative to the side to move
	if (board.nextMove == BLACK_MOVE){
		return -score;
	} else {
		return score;
	}
	
}