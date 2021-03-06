﻿#pragma once


#include <cstdlib>	// abs()
#include "../n105_color___/n105_100_color.hpp"
#include "../n110_square__/n110_100_square.hpp"
#include "../n110_square__/n110_250_squareDelta.hpp"
#include "../n112_pieceTyp/n112_050_pieceType.hpp"
#include "../n161_sqDistan/n161_500_squareDistance.hpp"
#include "../n162_bonaDir_/n162_070_bonaDirAbstract.hpp"
#include "../n220_position/n220_650_position.hpp" // FIXME:


class DirecDiagNESW : public BonaDirAbstract {
public:

	void InitializeSquareDistance(SquareDistance& squareDistance, Square sq0, Square sq1) const {
		squareDistance.SetValue( sq0, sq1,
			abs(static_cast<int>(sq0 - sq1) / static_cast<int>(SquareDelta::DeltaNE))
		);
	}

	void Do2Move(Position& position, Square from, const Square ksq, const Color us) const;

};


extern DirecDiagNESW g_direcDiagNESW;

