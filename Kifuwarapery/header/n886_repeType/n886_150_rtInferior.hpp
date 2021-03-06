#pragma once


#include "../n119_score___/n119_090_scoreIndex.hpp"
#include "../n223_move____/n223_500_flashlight.hpp"
#include "../n885_searcher/n885_040_rucksack.hpp"
#include "n886_070_rtAbstract.hpp" // &参照は使えない。*参照は使える。


class RepetitionTypeInferior : public RepetitionTypeAbstract {
public:

	void CheckStopAndMaxPly(
		bool& isReturn, ScoreIndex& resultScore, const Rucksack* pSearcher, Flashlight* pFlashlightBox
	) const override {
		if (pFlashlightBox->m_ply != 2) {
			isReturn = true;
			resultScore = ScoreMatedInMaxPly;
			return;
		}
	}
};

