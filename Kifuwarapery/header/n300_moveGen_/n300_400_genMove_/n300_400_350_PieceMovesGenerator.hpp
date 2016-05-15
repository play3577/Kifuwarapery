#pragma once


#include "../../n080_common__/n080_100_common.hpp"
#include "../../n105_color___/n105_100_color.hpp"
#include "../../n110_square__/n110_100_square.hpp"
#include "../../n110_square__/n110_200_rank.hpp"
#include "../../n110_square__/n110_250_squareDelta.hpp"
#include "../../n112_pieceTyp/n112_050_pieceType.hpp"
#include "../../n113_piece___/n113_150_piece.hpp"
#include "../../n160_board___/n160_100_bitboard.hpp"
#include "../../n160_board___/n160_600_bitboardAll.hpp"
#include "../../n165_movStack/n165_300_moveType.hpp"
#include "../../n220_position/n220_640_utilAttack.hpp"
#include "../../n220_position/n220_650_position.hpp"
#include "../../n300_moveGen_/n300_200_pieceTyp/n300_200_020_moveStack.hpp"
#include "../../n300_moveGen_/n300_200_pieceTyp/n300_200_040_ptEvent.hpp"
#include "../../n300_moveGen_/n300_200_pieceTyp/n300_200_500_ptPrograms.hpp"
#include "n300_400_040_pieceMoveEvent.hpp"
#include "n300_400_250_bishopRookMovesGenerator.hpp"


// �J�[�h������Ă����܂��B
// ������J�[�h�́@�l�������r���������@�ɐςݏグ�Ă����܂��B
class PieceMovesGenerator {
public:


	// ���̏ꍇ
	template <const Color US>
	static FORCE_INLINE MoveStack* BuildCards_N01_Pawn(
		MoveStack* moveStackList,
		const PieceMoveEvent ptEvent,
		const Bitboard& destinationBb			// �ړ����Ăق�����
	) {
		static_assert(US==Color::Black | US==Color::White,"");

		// Txxx �͐��A���̏����z�������ϐ��B�����͐��ɍ��킹�Ă���B
		const Rank TRank6 = (US == Black ? Rank6 : Rank4);
		const Rank TRank9 = (US == Black ? Rank9 : Rank1);
		const Bitboard TRank789BB = g_inFrontMaskBb.GetInFrontMask<US>(TRank6);
		const SquareDelta TDeltaS = (US == Black ? DeltaS : DeltaN);

		// �ړ����Ăق����Ƃ���ɁA�ړ��ł���ʒu�����߂܂��B
		Bitboard toBB =
			// ���̈ʒu��^����ƁA����𗘂��ɕϊ����܂��B
			Bitboard::PawnAttack<US>(ptEvent.m_pos.GetBbOf20<US>(PieceType::N01_Pawn))
			&
			// �ړ����Ăق����Ƃ���
			destinationBb;

		// ����
		if (ptEvent.m_mt != N04_NonCaptureMinusPro) {
			
			// �P�`�R�i�����̃r�b�g�{�[�h��
			Bitboard toOn789BB = toBB & TRank789BB;

			if (toOn789BB.Exists1Bit()) {//�r�b�g�������Ă����

				// �P�`�R�i���A���̃r�b�g�{�[�h����n�e�e�ɂ��܂��B
				toBB.AndEqualNot(TRank789BB);

				Square to;

				FOREACH_BB(toOn789BB, to, {
					const Square from = to + TDeltaS;

					moveStackList->m_move = g_makePromoteMove.BuildCard_mt2(ptEvent.m_mt, ptEvent.m_pos, g_PTPAWN_ONBOARD_AS_MOVE,from, to);
					MoveMaker_Promote::APPEND_PROMOTE_FLAG(moveStackList->m_move);
					moveStackList++;

					if (ptEvent.m_mt == N07_NonEvasion || ptEvent.m_all) {
						if (ConvSquare::TO_RANK10(to) != TRank9) {
							moveStackList->m_move = g_makePromoteMove.BuildCard_mt2(ptEvent.m_mt, ptEvent.m_pos, g_PTPAWN_ONBOARD_AS_MOVE,from, to);
							moveStackList++;
						}
					}
				});
			}
		}
		else {
			assert(!(destinationBb & TRank789BB).Exists1Bit());
		}

		// �c��(�s��)
		// toBB �� 8~4 �i�ڂ܂ŁB
		Square to;
		FOREACH_BB(toBB, to, {
			const Square from = to + TDeltaS;
			moveStackList->m_move = g_makePromoteMove.BuildCard_mt2(ptEvent.m_mt, ptEvent.m_pos, g_PTPAWN_ONBOARD_AS_MOVE,	from, to);
			moveStackList++;
		});

		return moveStackList;
	}//���Z�q�̃I�[�o�[���[�h�̒�`�H


	 // ���Ԃ̏ꍇ
	template <const Color US>
	static FORCE_INLINE MoveStack* BuildCards_N02_Lance(
		MoveStack* moveStackList,
		const PieceMoveEvent ptEvent,
		const Bitboard& destinationBb			// �ړ����Ăق�����
	) {
		Bitboard fromBB = ptEvent.m_pos.GetBbOf20<US>(N02_Lance);
		while (fromBB.Exists1Bit()) {
			const Square from = fromBB.PopFirstOneFromI9();
			const PieceTypeEvent ptEvent1(ptEvent.m_pos.GetOccupiedBB(), US, from);
			Bitboard toBB = PiecetypePrograms::m_LANCE.GetAttacks2From(ptEvent1) & destinationBb;
			do {
				if (toBB.Exists1Bit()) {
					// ����Ώۂ͕K����ȉ��Ȃ̂ŁAtoBB �̃r�b�g�� 0 �ɂ���K�v���Ȃ��B
					const Square to = (ptEvent.m_mt == N00_Capture || ptEvent.m_mt == N03_CapturePlusPro ? toBB.GetFirstOneFromI9() : toBB.PopFirstOneFromI9());
					const bool toCanPromote = ConvSquare::CAN_PROMOTE10<US>(ConvSquare::TO_RANK10(to));
					if (toCanPromote) {

						moveStackList->m_move = g_makePromoteMove.BuildCard_mt2(ptEvent.m_mt, ptEvent.m_pos, g_PTLANCE_ONBOARD_AS_MOVE,	from, to);
						MoveMaker_Promote::APPEND_PROMOTE_FLAG(moveStackList->m_move);
						moveStackList++;

						if (ptEvent.m_mt == N07_NonEvasion || ptEvent.m_all) {
							if (ConvSquare::IS_BEHIND10<US>(Rank9, Rank1, ConvSquare::TO_RANK10(to))) // 1�i�ڂ̕s���͏Ȃ�
							{
								moveStackList->m_move = g_makePromoteMove.BuildCard_mt2(ptEvent.m_mt, ptEvent.m_pos, g_PTLANCE_ONBOARD_AS_MOVE,	from, to);
								moveStackList++;
							}
						}
						else if (ptEvent.m_mt != N01_NonCapture && ptEvent.m_mt != N04_NonCaptureMinusPro) { // ������Ȃ�3�i�ڂ̕s�����Ȃ�
							if (ConvSquare::IS_BEHIND10<US>(Rank8, Rank2, ConvSquare::TO_RANK10(to))) // 2�i�ڂ̕s�����Ȃ�
							{
								moveStackList->m_move = g_makePromoteMove.BuildCard_mt2(ptEvent.m_mt, ptEvent.m_pos, g_PTLANCE_ONBOARD_AS_MOVE,	from, to);
								moveStackList++;
							}
						}
					}
					else
					{
						moveStackList->m_move = g_makePromoteMove.BuildCard_mt2(ptEvent.m_mt, ptEvent.m_pos, g_PTLANCE_ONBOARD_AS_MOVE,	from, to);
						moveStackList++;
					}
				}
				// ����Ώۂ͕K����ȉ��Ȃ̂ŁAloop �͕s�v�B�œK���� do while �������Ȃ�Ɨǂ��B
			} while (!(ptEvent.m_mt == N00_Capture || ptEvent.m_mt == N03_CapturePlusPro) && toBB.Exists1Bit());
		}
		return moveStackList;
	}


	// �j�n�̏ꍇ
	template <const Color US>
	static FORCE_INLINE MoveStack* BuildCards_N03_Knight(
		MoveStack* moveStackList,
		const PieceMoveEvent ptEvent,
		const Bitboard& destinationBb			// �ړ����Ăق�����
	) {
		Bitboard fromBB = ptEvent.m_pos.GetBbOf20<US>(N03_Knight);
		while (fromBB.Exists1Bit()) {
			const Square from = fromBB.PopFirstOneFromI9();
			const PieceTypeEvent ptEvent1(g_nullBitboard, US, from);
			Bitboard toBB = PiecetypePrograms::m_KNIGHT.GetAttacks2From(ptEvent1) & destinationBb;
			while (toBB.Exists1Bit()) {
				const Square to = toBB.PopFirstOneFromI9();
				const bool toCanPromote = ConvSquare::CAN_PROMOTE10<US>(ConvSquare::TO_RANK10(to));
				if (toCanPromote) {

					moveStackList->m_move = g_makePromoteMove.BuildCard_mt2(ptEvent.m_mt, ptEvent.m_pos, g_PTKNIGHT_ONBOARD_AS_MOVE, from, to);
					MoveMaker_Promote::APPEND_PROMOTE_FLAG(moveStackList->m_move);
					moveStackList++;

					if (ConvSquare::IS_BEHIND10<US>(Rank8, Rank2, ConvSquare::TO_RANK10(to))) // 1, 2�i�ڂ̕s���͏Ȃ�
					{
						moveStackList->m_move = g_makePromoteMove.BuildCard_mt2(ptEvent.m_mt, ptEvent.m_pos, g_PTKNIGHT_ONBOARD_AS_MOVE, from, to);
						moveStackList++;
					}
				}
				else
				{
					moveStackList->m_move = g_makePromoteMove.BuildCard_mt2(ptEvent.m_mt, ptEvent.m_pos, g_PTKNIGHT_ONBOARD_AS_MOVE, from, to);
					moveStackList++;
				}
			}
		}
		return moveStackList;
	}


	// ��̏ꍇ
	template <const Color US>
	static FORCE_INLINE MoveStack* BuildCards_N04_Silver(
		MoveStack* moveStackList,
		const PieceMoveEvent ptEvent,
		const Bitboard& destinationBb			// �ړ����Ăق�����
	) {
		Bitboard fromBB = ptEvent.m_pos.GetBbOf20<US>(N04_Silver);
		while (fromBB.Exists1Bit()) {
			const Square from = fromBB.PopFirstOneFromI9();
			const bool fromCanPromote = ConvSquare::CAN_PROMOTE10<US>(ConvSquare::TO_RANK10(from));
			const PieceTypeEvent ptEvent1(g_nullBitboard, US, from);
			Bitboard toBB = PiecetypePrograms::m_SILVER.GetAttacks2From(ptEvent1) & destinationBb;
			while (toBB.Exists1Bit()) {
				const Square to = toBB.PopFirstOneFromI9();
				const bool toCanPromote = ConvSquare::CAN_PROMOTE10<US>(ConvSquare::TO_RANK10(to));
				if (fromCanPromote | toCanPromote) {
					moveStackList->m_move = g_makePromoteMove.BuildCard_mt2(ptEvent.m_mt, ptEvent.m_pos, g_PTSILVER_ONBOARD_AS_MOVE,	from, to);
					MoveMaker_Promote::APPEND_PROMOTE_FLAG(moveStackList->m_move);
					moveStackList++;
				}

				moveStackList->m_move = g_makePromoteMove.BuildCard_mt2(ptEvent.m_mt, ptEvent.m_pos, g_PTSILVER_ONBOARD_AS_MOVE,from, to);
				moveStackList++;
			}
		}
		return moveStackList;
	}


	// �p�̓������H
	template <const Color US>
	static FORCE_INLINE MoveStack* BuildCards_N05_Bishop(
		MoveStack* moveStackList,
		const PieceMoveEvent ptEvent,
		const Bitboard& destinationBb				// �ړ����Ăق�����
	) {
		return BishopRookMovesGenerator::BuildCards_BishopOrRook<true,US>(moveStackList, g_PTBISHOP_ONBOARD_AS_MOVE, ptEvent, destinationBb);
	}


	// ��Ԃ̓������H
	template <const Color US>
	static FORCE_INLINE MoveStack* BuildCards_N06_Rook(
		MoveStack* moveStackList,
		const PieceMoveEvent ptEvent,
		const Bitboard& destinationBb			// �ړ����Ăق�����
	) {
		return BishopRookMovesGenerator::BuildCards_BishopOrRook<false,US>(moveStackList, g_PTROOK_ONBOARD_AS_MOVE, ptEvent, destinationBb);
	}


	// ��, ������A�n�A���̎w���萶��
	template <const Color US>
	static FORCE_INLINE MoveStack* BuildCards_N16_GoldHorseDragon(
		MoveStack* moveStackList,
		const PieceMoveEvent ptEvent,
		const Bitboard& destinationBb			// �ړ����Ăق�����
	) {
		// ���A�����A�n�A����bitboard���܂Ƃ߂Ĉ����B
		Bitboard fromBB = (ptEvent.m_pos.GetGoldsBB() | ptEvent.m_pos.GetBbOf20(N13_Horse, N14_Dragon)) &
			ptEvent.m_pos.GetBbOf10<US>();
		while (fromBB.Exists1Bit()) {
			const Square from = fromBB.PopFirstOneFromI9();
			// from �ɂ����̎�ނ𔻕�
			const PieceType pt = ConvPiece::TO_PIECE_TYPE10(ptEvent.m_pos.GetPiece(from));
			Bitboard toBB = UtilAttack::GetAttacksFrom<US>(pt, from, ptEvent.m_pos.GetOccupiedBB()) & destinationBb;
			while (toBB.Exists1Bit()) {
				const Square to = toBB.PopFirstOneFromI9();
				moveStackList->m_move = g_makePromoteMove.BuildCard_goldHorseDragon(
					ptEvent.m_mt,
					ptEvent.m_pos,
					pt,// ���A������A�n�A���̂����ꂩ�������i�O���O�j
					from,
					to					
					);
				moveStackList++;
			}
		}
		return moveStackList;
	}


	// �ʂ̏ꍇ
	// �K���Տ�� 1 ���������邱�Ƃ�O��ɂ��邱�ƂŁAwhile ���[�v�� 1 �������č��������Ă���B
	template <const Color US>
	static FORCE_INLINE MoveStack* BuildCards_N08_King(
		MoveStack* moveStackList,
		const PieceMoveEvent ptEvent,
		const Bitboard& target
	) {
		const Square from = ptEvent.m_pos.GetKingSquare<US>();
		const PieceTypeEvent ptEvent1(g_nullBitboard, US, from);
		Bitboard toBB = PiecetypePrograms::m_KING.GetAttacks2From(ptEvent1) & target;
		while (toBB.Exists1Bit()) {
			const Square to = toBB.PopFirstOneFromI9();
			moveStackList->m_move = g_makePromoteMove.BuildCard_mt2(ptEvent.m_mt, ptEvent.m_pos, g_PTKING_ONBOARD_AS_MOVE,from, to);
			moveStackList++;
		}
		return moveStackList;
	}

};
