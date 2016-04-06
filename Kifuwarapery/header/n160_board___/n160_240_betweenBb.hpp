#pragma once

#include "n160_100_bitboard.hpp"

class BetweenBb {
public:
	Bitboard m_betweenBB[SquareNum][SquareNum];

public:

	void Initialize();

	// sq1, sq2 の間(sq1, sq2 は含まない)のビットが立った Bitboard
	inline Bitboard GetBetweenBB(const Square sq1, const Square sq2) const {
		return this->m_betweenBB[sq1][sq2];
	}

};
