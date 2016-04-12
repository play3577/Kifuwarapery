#pragma once

#include "n112_050_pieceType.hpp"
#include "n112_070_ptAbstract.hpp"

class PtProPawn : PtAbstract {
public:

	PieceType inline GetNumber() const {
		return PieceType::N09_ProPawn;
	}

};


extern PtProPawn g_ptProPawn;
