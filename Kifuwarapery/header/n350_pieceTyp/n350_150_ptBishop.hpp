#pragma once

#include "../n105_color___/n105_100_color.hpp"
#include "../n110_square__/n110_100_square.hpp"
#include "../n110_square__/n110_500_utilSquare.hpp"
#include "../n112_pieceTyp/n112_050_pieceType.hpp"
#include "../n160_board___/n160_100_bitboard.hpp"
#include "../n160_board___/n160_120_bishopAttackBb.hpp"
#include "../n165_movStack/n165_300_moveType.hpp"
#include "../n165_movStack/n165_500_moveStack.hpp"
#include "../n165_movStack/n165_600_utilMove.hpp"
#include "../n220_position/n220_650_position.hpp"
#include "../n220_position/n220_670_makePromoteMove.hpp"
#include "n350_070_ptAbstract.hpp"


class PtBishop : public PtAbstract {
public:

	inline PieceType GetNumber() const {
		return PieceType::N05_Bishop;
	}

	inline Bitboard GetAttacks2From(const Bitboard& occupied, const Color c, const Square sq) const {
		return g_bishopAttackBb.BishopAttack(&occupied, sq);;
	}

	// pin は省かない。
	FORCE_INLINE void Generate2RecaptureMoves(
		MoveStack* moveStackList,
		const Position& pos,
		const Square from,
		const Square to,
		const Color us
		) const {
		(*moveStackList++).m_move = (
			(UtilSquare::CanPromote(us, UtilSquare::ToRank(to)) | UtilSquare::CanPromote(us, UtilSquare::ToRank(from))
				) ?
			g_makePromoteMove.MakePromoteMove2<Capture>(this->GetNumber(), from, to, pos) :
			g_makePromoteMove.MakeNonPromoteMove<Capture>(this->GetNumber(), from, to, pos)
			);
	}

};
