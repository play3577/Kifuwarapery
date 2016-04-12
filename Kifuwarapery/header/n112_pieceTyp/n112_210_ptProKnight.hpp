#pragma once

#include "n112_050_pieceType.hpp"
#include "n112_070_ptAbstract.hpp"

class PtProKnight : PtAbstract {
public:

	PieceType inline GetNumber() const {
		return PieceType::N11_ProKnight;
	}

};


extern PtProKnight g_ptProKnight;
