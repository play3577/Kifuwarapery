#pragma once


#include "../n119_score___/n119_090_score.hpp"
#include "n886_070_rtAbstract.hpp"

#include "../n223_move____/n223_500_flashlight.hpp"// &�Q�Ƃ͎g���Ȃ��B*�Q�Ƃ͎g����B
#include "../n885_searcher/n885_500_rucksack.hpp"


class RepetitionTypeDraw : public RepetitionTypeAbstract {
public:

	void CheckStopAndMaxPly(
		bool& isReturn, Score& resultScore, const Rucksack* pSearcher, Flashlight* pFlashlightBox
	) const {
		isReturn = true;
		resultScore = ScoreDraw;
		return;
	}
};
