﻿#pragma once


#include "../n080_common__/n080_100_common.hpp"
#include "../n105_color___/n105_100_color.hpp"
#include "../n110_square__/n110_205_utilRank.hpp"
#include "../n160_board___/n160_100_bitboard.hpp"
#include "../n160_board___/n160_230_setMaskBB.hpp"
#include "../n160_board___/n160_600_bitboardAll.hpp"
#include "../n165_movStack/n165_500_moveStack.hpp"
#include "../n220_position/n220_650_position.hpp"
#include "../n358_dropMake/n358_100_dropMakerHand0.hpp"
#include "../n358_dropMake/n358_110_dropMakerHand1.hpp"
#include "../n358_dropMake/n358_120_dropMakerHand2.hpp"
#include "../n358_dropMake/n358_130_dropMakerHand3.hpp"
#include "../n358_dropMake/n358_140_dropMakerHand4.hpp"
#include "../n358_dropMake/n358_150_dropMakerHand5.hpp"
#include "../n358_dropMake/n358_160_dropMakerHand6.hpp"


class DropMoveGenerator {
public:

	// (^q^)追加
	DropMakerHand0 m_dropMakerHand0;
	DropMakerHand1 m_dropMakerHand1;
	DropMakerHand2 m_dropMakerHand2;
	DropMakerHand3 m_dropMakerHand3;
	DropMakerHand4 m_dropMakerHand4;
	DropMakerHand5 m_dropMakerHand5;
	DropMakerHand6 m_dropMakerHand6;
	DropMakerAbstract* m_pDropMakerArray[7];

	// (^q^)追加
	DropMoveGenerator() {
		this->m_pDropMakerArray[0] = &this->m_dropMakerHand0;
		this->m_pDropMakerArray[1] = &this->m_dropMakerHand1;
		this->m_pDropMakerArray[2] = &this->m_dropMakerHand2;
		this->m_pDropMakerArray[3] = &this->m_dropMakerHand3;
		this->m_pDropMakerArray[4] = &this->m_dropMakerHand4;
		this->m_pDropMakerArray[5] = &this->m_dropMakerHand5;
		this->m_pDropMakerArray[6] = &this->m_dropMakerHand6;
	}


	// 駒打ちの場合
	// 歩以外の持ち駒は、loop の前に持ち駒の種類の数によって ｓｗｉｔｃｈ で展開している。
	// ループの展開はコードが膨れ上がる事によるキャッシュヒット率の低下と、演算回数のバランスを取って決める必要がある。
	// NPSに影響が出ないならシンプルにした方が良さそう。
	MoveStack* GenerateDropMoves(
		Color us,
		MoveStack* pMovestack,
		const Position& pos,
		const Bitboard& target
	) {
		const Hand hand = pos.GetHand(us);


		// まず、歩に対して指し手を生成
		if (Hand::Exists_HPawn(hand)) {

			Bitboard toBB = target;
			// 一段目には打てない

			const Rank tRank9 = (us == Black ? Rank9 : Rank1);
			toBB.AndEqualNot(g_rankMaskBb.GetRankMask(tRank9));

			// 二歩の回避
			Bitboard pawnsBB = pos.GetBbOf(N01_Pawn, us);
			Square pawnsSquare;
			foreachBB(pawnsBB, pawnsSquare, [&](const int part) {
				toBB.SetP(part, toBB.GetP(part) & ~g_fileMaskBb.GetSquareFileMask(pawnsSquare).GetP(part));
			});

			// 打ち歩詰めの回避
			const Rank tRank1 = (us == Black ? Rank1 : Rank9);
			const SquareDelta tDeltaS = (us == Black ? DeltaS : DeltaN);

			const Square ksq = pos.GetKingSquare(UtilColor::OppositeColor(us));
			// 相手玉が九段目なら、歩で王手出来ないので、打ち歩詰めを調べる必要はない。
			if (UtilSquare::ToRank(ksq) != tRank1) {
				const Square pawnDropCheckSquare = ksq + tDeltaS;
				assert(UtilSquare::ContainsOf(pawnDropCheckSquare));
				if (g_setMaskBb.IsSet(&toBB, pawnDropCheckSquare) && pos.GetPiece(pawnDropCheckSquare) == N00_Empty) {
					if (!pos.IsPawnDropCheckMate(us, pawnDropCheckSquare)) {
						// ここで clearBit だけして MakeMove しないことも出来る。
						// 指し手が生成される順番が変わり、王手が先に生成されるが、後で問題にならないか?
						(*pMovestack++).m_move = UtilMove::MakeDropMove(N01_Pawn, pawnDropCheckSquare);
					}
					g_setMaskBb.XorBit(&toBB, pawnDropCheckSquare);
				}
			}

			Square to;
			FOREACH_BB(toBB, to, {
				(*pMovestack++).m_move = UtilMove::MakeDropMove(N01_Pawn, to);
			});
		}


		// 歩 以外の駒を持っているか
		if (Hand::ExceptPawnExists(hand)) {
			PieceType haveHandArr[6]; // 歩以外の持ち駒。vector 使いたいけど、速度を求めるので使わない。
			int haveHandNum = 0; // 持ち駒の駒の種類の数

								 // 桂馬、香車、それ以外の順番で格納する。(駒を打てる位置が限定的な順)
			if (Hand::Exists_HKnight(hand)) { haveHandArr[haveHandNum++] = PieceType::N03_Knight; }
			const int noKnightIdx = haveHandNum; // 桂馬を除く駒でループするときのループの初期値
			if (Hand::Exists_HLance(hand)) { haveHandArr[haveHandNum++] = PieceType::N02_Lance; }
			const int noKnightLanceIdx = haveHandNum; // 桂馬, 香車を除く駒でループするときのループの初期値
			if (Hand::Exists_HSilver(hand)) { haveHandArr[haveHandNum++] = PieceType::N04_Silver; }
			if (Hand::Exists_HGold(hand)) { haveHandArr[haveHandNum++] = PieceType::N07_Gold; }
			if (Hand::Exists_HBishop(hand)) { haveHandArr[haveHandNum++] = PieceType::N05_Bishop; }
			if (Hand::Exists_HRook(hand)) { haveHandArr[haveHandNum++] = PieceType::N06_Rook; }

			const Rank tRank8 = (us == Black ? Rank8 : Rank2);
			const Rank tRank9 = (us == Black ? Rank9 : Rank1);
			const Bitboard TRank8BB = g_rankMaskBb.GetRankMask(tRank8);
			const Bitboard TRank9BB = g_rankMaskBb.GetRankMask(tRank9);

			DropMakerEvent dmEvent(
				us,
				pMovestack,
				pos,
				target,
				hand,
				haveHandNum,
				noKnightIdx,
				noKnightLanceIdx,
				TRank8BB,
				TRank9BB
			);
			// 桂馬、香車 以外の持ち駒があれば、
			// 一段目に対して、桂馬、香車以外の指し手を生成。
			// FIXME: 配列の範囲チェックしてないぜ☆（＾ｑ＾）
			pMovestack = this->m_pDropMakerArray[haveHandNum - noKnightLanceIdx]->MakeDropMovesToRank9ExceptNL(
				dmEvent,
				haveHandArr
				);

			// 桂馬以外の持ち駒があれば、
			// 二段目に対して、桂馬以外の指し手を生成。
			pMovestack = this->m_pDropMakerArray[haveHandNum - noKnightIdx]->MakeDropMovesToRank8ExceptN(
				dmEvent,
				haveHandArr
				);

			// 一、二段目以外に対して、全ての持ち駒の指し手を生成。
			pMovestack = this->m_pDropMakerArray[haveHandNum]->MakeDropMovesToRank1234567(
				dmEvent,
				haveHandArr
				);
		}

		return pMovestack;
	}

};


extern DropMoveGenerator g_dropMoveGenerator;

