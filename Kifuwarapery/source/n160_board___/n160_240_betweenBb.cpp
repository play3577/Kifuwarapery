#include "../../header/n110_square__/n110_410_squareRelation.hpp"
#include "../../header/n160_board___/n160_400_printBb.hpp"
#include "../../header/n160_board___/n160_240_betweenBb.hpp"


BetweenBb g_betweenBb;

void BetweenBb::Initialize() {
	for (Square sq1 = I9; sq1 < SquareNum; ++sq1) {
		for (Square sq2 = I9; sq2 < SquareNum; ++sq2) {
			g_betweenBb.m_betweenBB[sq1][sq2] = Bitboard::AllZeroBB();
			if (sq1 == sq2) continue;
			const Direction direc = SquareRelation::GetSquareRelation(sq1, sq2);
			if (direc & DirecCross)
				g_betweenBb.m_betweenBB[sq1][sq2] = g_rookAttackBb.GetControllBb(&g_setMaskBb.GetSetMaskBb(sq2), sq1) & g_rookAttackBb.GetControllBb(&g_setMaskBb.GetSetMaskBb(sq1), sq2);
			else if (direc & DirecDiag)
				g_betweenBb.m_betweenBB[sq1][sq2] = g_bishopAttackBb.BishopAttack(&g_setMaskBb.GetSetMaskBb(sq2), sq1) & g_bishopAttackBb.BishopAttack(&g_setMaskBb.GetSetMaskBb(sq1), sq2);
		}
	}
}