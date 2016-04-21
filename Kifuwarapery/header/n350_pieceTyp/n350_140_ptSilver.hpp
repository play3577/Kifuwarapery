#pragma once

#include "../n105_color___/n105_100_color.hpp"
#include "../n110_square__/n110_100_square.hpp"
#include "../n110_square__/n110_500_utilSquare.hpp"
#include "../n112_pieceTyp/n112_050_pieceType.hpp"
#include "../n160_board___/n160_100_bitboard.hpp"
#include "../n160_board___/n160_110_silverAttackBb.hpp"
#include "../n165_movStack/n165_300_moveType.hpp"
#include "../n165_movStack/n165_500_moveStack.hpp"
#include "../n165_movStack/n165_600_utilMove.hpp"
#include "../n220_position/n220_650_position.hpp"
#include "../n220_position/n220_670_makePromoteMove.hpp"
#include "n350_070_ptAbstract.hpp"


class PtSilver : public PtAbstract {
public:

	inline PieceType GetNumber() const {
		return PieceType::N04_Silver;
	}

	inline Bitboard GetAttacks2From(const Bitboard& occupied, const Color c, const Square sq) const {
		return g_silverAttackBb.GetControllBb(c, sq);
	}

	// pin は省かない。
	FORCE_INLINE void Generate2RecaptureMoves(
		MoveStack* moveStackList,
		const Position& pos,
		const Square from,
		const Square to,
		const Color us
		) const {
		moveStackList->m_move = g_makePromoteMove.GetSelectedMakeMove_ExceptPromote(N00_Capture, this->GetNumber(), from, to, pos);

		if (
			UtilSquare::CanPromote(us, UtilSquare::ToRank(to))
			|
			UtilSquare::CanPromote(us, UtilSquare::ToRank(from))
		){
			MakePromoteMove::APPEND_PROMOTE_FLAG(moveStackList->m_move, N00_Capture, this->GetNumber());
		}

		moveStackList++;
	}

};
