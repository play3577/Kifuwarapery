﻿#pragma once


#include "../../n110_square__/n110_100_square.hpp"
#include "../../n160_board___/n160_100_bitboard.hpp"
#include "../../n220_position/n220_650_position.hpp"
#include "n300_500_070_pieceAbstract.hpp"


class PieceWKnight : public PieceAbstract {
public:
	void MakeBanned2KingTo(Bitboard& bannedKingToBB, const Position& pos, const Square checkSq, const Square ksq
		) const override {
		// 歩、桂馬で王手したときは、どこへ逃げても、その駒で取られることはない。
		// よって、ここでは何もしない。
		assert(
			pos.GetPiece(checkSq) == N17_WPawn ||
			pos.GetPiece(checkSq) == N19_WKnight
			);
	}
};
