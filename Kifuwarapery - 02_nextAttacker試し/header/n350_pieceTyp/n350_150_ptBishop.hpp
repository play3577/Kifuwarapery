#pragma once

#include "../n105_color___/n105_100_color.hpp"
#include "../n110_square__/n110_100_square.hpp"
#include "../n110_square__/n110_500_convSquare.hpp"
#include "../n112_pieceTyp/n112_050_pieceType.hpp"
#include "../n160_board___/n160_100_bitboard.hpp"
#include "../n160_board___/n160_120_bishopAttackBb.hpp"
#include "../n165_movStack/n165_300_moveType.hpp"
#include "../n165_movStack/n165_500_moveStack.hpp"
#include "../n165_movStack/n165_600_convMove.hpp"
#include "../n220_position/n220_650_position.hpp"
#include "../n220_position/n220_670_makePromoteMove.hpp"
#include "n350_040_ptEvent.hpp"
#include "n350_070_ptAbstract.hpp"


class PtBishop : public PtAbstract {
public:

	PieceType GetNumber() const {
		return PieceType::N05_Bishop;
	}

	Bitboard GetAttacks2From(const PieceTypeEvent& ptEvent) const {
		return g_bishopAttackBb.BishopAttack(ptEvent.m_occupied, ptEvent.m_sq);
	}

	// pin は省かない。
	void Generate2RecaptureMoves(//FORCE_INLINE
		MoveStack* moveStackList,
		const Position& pos,
		const Square from,
		const Square to,
		const Color us
		) const {
		moveStackList->m_move = g_makePromoteMove.GetSelectedMakeMove_ExceptPromote_CaptureCategory(this->GetNumber(), from, to, pos);

		if (
			ConvSquare::CAN_PROMOTE10(us, ConvSquare::TO_RANK10(to))
			|
			ConvSquare::CAN_PROMOTE10(us, ConvSquare::TO_RANK10(from))
		) {
			MakePromoteMove::APPEND_PROMOTE_FLAG(moveStackList->m_move);//, N00_Capture, this->GetNumber()
		}

		moveStackList++;
	}

	Bitboard AppendToNextAttacker(
		Bitboard& attackers,
		const Position& pos,
		const Square to,
		Bitboard& occupied,
		const Color turn
	) const {
		attackers |= (g_bishopAttackBb.BishopAttack(occupied, to) & pos.GetBbOf20(N05_Bishop, N13_Horse));
		return attackers;
	}

};
