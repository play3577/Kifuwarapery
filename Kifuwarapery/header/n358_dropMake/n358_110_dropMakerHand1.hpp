﻿#pragma once


#include "../n080_common__/n080_100_common.hpp"
#include "../n110_square__/n110_100_square.hpp"
#include "../n112_pieceTyp/n112_050_pieceType.hpp"
#include "../n160_board___/n160_100_bitboard.hpp"
#include "../n165_movStack/n165_500_moveStack.hpp"
#include "../n165_movStack/n165_600_utilMove.hpp"
#include "n358_070_dropMakerAbstract.hpp"


class DropMakerHand1 : public DropMakerAbstract {
public:

	static void MakeDropMovesToRank9ExceptNL(
		const Bitboard& target,
		const Bitboard TRank9BB,
		MoveStack* moveStackList,
		const PieceType haveHand[6],
		int noKnightLanceIdx
	) {
		Square to;
		Bitboard toBB = target & TRank9BB; FOREACH_BB(toBB, to, { Unroller<1>()([&](const int i) { (*moveStackList++).m_move = UtilMove::MakeDropMove(haveHand[noKnightLanceIdx + i], to); }); });
		//Bitboard toBB = target & TRank9BB;
		//Square to;
		//FOREACH_BB(	toBB, to, {
		//	(*pMoveStackList++).m_move = UtilMove::MakeDropMove(haveHand[noKnightLanceIdx + 0], to);
		//});
	}

	static void MakeDropMovesToRank8ExceptN(
		const Bitboard& target,
		const Bitboard TRank8BB,
		MoveStack* moveStackList,
		const PieceType haveHand[6],
		int noKnightIdx
	) {
		Square to;
		Bitboard toBB = target & TRank8BB; FOREACH_BB(toBB, to, { Unroller<1>()([&](const int i) { (*moveStackList++).m_move = UtilMove::MakeDropMove(haveHand[noKnightIdx + i], to); }); });
		//Bitboard toBB = target & TRank8BB;
		//Square to;
		//FOREACH_BB(	toBB, to, {
		//	(*pMovestack++).m_move = UtilMove::MakeDropMove(haveHand[noKnightIdx + 0], to);
		//});
	}

	static void MakeDropMovesToRank1234567(
		Bitboard& toBB,
		MoveStack* moveStackList,
		const PieceType haveHand[6]
	) {
		Square to;
		FOREACH_BB(toBB, to, { Unroller<1>()([&](const int i) { (*moveStackList++).m_move = UtilMove::MakeDropMove(haveHand[i], to); }); });
		//Square to;
		//FOREACH_BB( toBB, to, {
		//	(*pMovestack++).m_move = UtilMove::MakeDropMove(haveHand[0], to);
		//});
	}

};


extern DropMakerHand1 g_dropMakerHand1;
