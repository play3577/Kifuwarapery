#pragma once


#include "../n110_square__/n110_300_direction.hpp"


// 2つの位置関係のテーブル
extern Direction g_squareRelation[SquareNum][SquareNum];


class SquareRelation {
public:

	static inline Direction GetSquareRelation(const Square sq1, const Square sq2) {
		return g_squareRelation[sq1][sq2];
	}


	// from, to, ksq が 縦横斜めの同一ライン上にあれば true を返す。
	template <bool FROM_KSQ_NEVER_BE_DIRECMISC>
	static inline bool IsAligned(const Square from, const Square to, const Square ksq) {
		const Direction direc = SquareRelation::GetSquareRelation(from, ksq);
		if (FROM_KSQ_NEVER_BE_DIRECMISC) {
			assert(direc != DirecMisc);
			return (direc == SquareRelation::GetSquareRelation(from, to));
		}
		else {
			return (direc != DirecMisc && direc == SquareRelation::GetSquareRelation(from, to));
		}
	}

};