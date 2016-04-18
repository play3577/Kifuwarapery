﻿#pragma once


#include "../n080_common__/n080_100_common.hpp"
#include "../n105_color___/n105_100_color.hpp"
#include "../n110_square__/n110_100_square.hpp"
#include "../n110_square__/n110_205_utilRank.hpp"
#include "../n112_pieceTyp/n112_050_pieceType.hpp"
#include "../n160_board___/n160_100_bitboard.hpp"
#include "../n160_board___/n160_230_setMaskBB.hpp"
#include "../n160_board___/n160_600_bitboardAll.hpp"
#include "../n165_movStack/n165_500_moveStack.hpp"
#include "../n165_movStack/n165_600_utilMove.hpp"
#include "../n220_position/n220_650_position.hpp"


class DropMakerAbstract {
public:

	// テンプレートを使っている関数で使うには、static にするしかないぜ☆（＾ｑ＾）
	static MoveStack* MakeDropMovesToRank9ExceptNL(
		Color us,
		MoveStack* pMovestack,
		const Position& pos,
		const Bitboard& target,
		const Hand& hand,
		const int haveHandNum,
		const int noKnightIdx,
		const int noKnightLanceIdx,
		const Bitboard& TRank8BB,
		const Bitboard& TRank9BB,
		PieceType haveHandArr[6]
	) {
		// このクラスのメソッドを実行しているようではエラーだぜ☆（＾ｑ＾）
		UNREACHABLE;
		return pMovestack;
	};

	// テンプレートを使っている関数で使うには、static にするしかないぜ☆（＾ｑ＾）
	static MoveStack* MakeDropMovesToRank8ExceptN(
		Color us,
		MoveStack* pMovestack,
		const Position& pos,
		const Bitboard& target,
		const Hand& hand,
		const int haveHandNum,
		const int noKnightIdx,
		const int noKnightLanceIdx,
		const Bitboard& TRank8BB,
		const Bitboard& TRank9BB,
		PieceType haveHandArr[6]
	) {
		// このクラスのメソッドを実行しているようではエラーだぜ☆（＾ｑ＾）
		UNREACHABLE;
		return pMovestack;
	};

	// テンプレートを使っている関数で使うには、static にするしかないぜ☆（＾ｑ＾）
	static MoveStack* MakeDropMovesToRank1234567(
		Color us,
		MoveStack* pMovestack,
		const Position& pos,
		const Bitboard& target,
		const Hand& hand,
		const int haveHandNum,
		const int noKnightIdx,
		const int noKnightLanceIdx,
		const Bitboard& TRank8BB,
		const Bitboard& TRank9BB,
		PieceType haveHandArr[6]
	) {
		// このクラスのメソッドを実行しているようではエラーだぜ☆（＾ｑ＾）
		UNREACHABLE;
		return pMovestack;
	};

};


