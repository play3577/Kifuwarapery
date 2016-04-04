#include "../../header/n220_position/n220_500_charToPieceUSI.hpp"
#include "../../header/n260_evaluate/n260_700_evaluate.hpp"
#include "../../header/n320_searcher/n320_150_search.hpp"
#include "../../header/n400_usi_____/n400_350_thread.hpp"

KPPBoardIndexStartToPiece g_kppBoardIndexStartToPiece;

std::array<s16, 2> EvalStorage::KPP[SquareNum][fe_end][fe_end];
std::array<s32, 2> EvalStorage::KKP[SquareNum][SquareNum][fe_end];
std::array<s32, 2> EvalStorage::KK[SquareNum][SquareNum];

#if defined USE_K_FIX_OFFSET
const s32 EvalStorage::K_Fix_Offset[SquareNum] = {
	2000*FVScale, 1700*FVScale, 1350*FVScale, 1000*FVScale,  650*FVScale,  350*FVScale,  100*FVScale,    0*FVScale,    0*FVScale,
	1800*FVScale, 1500*FVScale, 1250*FVScale, 1000*FVScale,  650*FVScale,  350*FVScale,  100*FVScale,    0*FVScale,    0*FVScale, 
	1800*FVScale, 1500*FVScale, 1250*FVScale, 1000*FVScale,  650*FVScale,  350*FVScale,  100*FVScale,    0*FVScale,    0*FVScale, 
	1700*FVScale, 1400*FVScale, 1150*FVScale,  900*FVScale,  550*FVScale,  250*FVScale,    0*FVScale,    0*FVScale,    0*FVScale, 
	1600*FVScale, 1300*FVScale, 1050*FVScale,  800*FVScale,  450*FVScale,  150*FVScale,    0*FVScale,    0*FVScale,    0*FVScale, 
	1700*FVScale, 1400*FVScale, 1150*FVScale,  900*FVScale,  550*FVScale,  250*FVScale,    0*FVScale,    0*FVScale,    0*FVScale, 
	1800*FVScale, 1500*FVScale, 1250*FVScale, 1000*FVScale,  650*FVScale,  350*FVScale,  100*FVScale,    0*FVScale,    0*FVScale, 
	1900*FVScale, 1600*FVScale, 1350*FVScale, 1000*FVScale,  650*FVScale,  350*FVScale,  100*FVScale,    0*FVScale,    0*FVScale, 
	2000*FVScale, 1700*FVScale, 1350*FVScale, 1000*FVScale,  650*FVScale,  350*FVScale,  100*FVScale,    0*FVScale,    0*FVScale
};
#endif

EvaluateHashTable g_evalTable;

const int kppArray[31] = {
	0,        f_pawn,   f_lance,  f_knight,
	f_silver, f_bishop, f_rook,   f_gold,   
	0,        f_gold,   f_gold,   f_gold,
	f_gold,   f_horse,  f_dragon,
	0,
	0,        e_pawn,   e_lance,  e_knight,
	e_silver, e_bishop, e_rook,   e_gold,   
	0,        e_gold,   e_gold,   e_gold,
	e_gold,   e_horse,  e_dragon
};

const int kppHandArray[ColorNum][HandPieceNum] = {
	{f_hand_pawn, f_hand_lance, f_hand_knight, f_hand_silver,
	 f_hand_gold, f_hand_bishop, f_hand_rook},
	{e_hand_pawn, e_hand_lance, e_hand_knight, e_hand_silver,
	 e_hand_gold, e_hand_bishop, e_hand_rook}
};



EvalSum Evaluation09::doapc(const Position& pos, const int index[2]) {
	const Square sq_bk = pos.GetKingSquare(Black);
	const Square sq_wk = pos.GetKingSquare(White);
	const int* list0 = pos.GetCplist0();
	const int* list1 = pos.GetCplist1();

	EvalSum sum;
	sum.p[2][0] = EvalStorage::KKP[sq_bk][sq_wk][index[0]][0];
	sum.p[2][1] = EvalStorage::KKP[sq_bk][sq_wk][index[0]][1];
	const auto* pkppb = EvalStorage::KPP[sq_bk][index[0]];
	const auto* pkppw = EvalStorage::KPP[UtilSquare::Inverse(sq_wk)][index[1]];
#if defined USE_AVX2_EVAL || defined USE_SSE_EVAL
	sum.m[0] = _mm_set_epi32(0, 0, *reinterpret_cast<const s32*>(&pkppw[GetList1[0]][0]), *reinterpret_cast<const s32*>(&pkppb[GetList0[0]][0]));
	sum.m[0] = _mm_cvtepi16_epi32(sum.m[0]);
	for (int i = 1; i < pos.GetNlist(); ++i) {
		__m128i tmp;
		tmp = _mm_set_epi32(0, 0, *reinterpret_cast<const s32*>(&pkppw[GetList1[i]][0]), *reinterpret_cast<const s32*>(&pkppb[GetList0[i]][0]));
		tmp = _mm_cvtepi16_epi32(tmp);
		sum.m[0] = _mm_add_epi32(sum.m[0], tmp);
}
#else
	sum.p[0][0] = pkppb[list0[0]][0];
	sum.p[0][1] = pkppb[list0[0]][1];
	sum.p[1][0] = pkppw[list1[0]][0];
	sum.p[1][1] = pkppw[list1[0]][1];
	for (int i = 1; i < pos.GetNlist(); ++i) {
		sum.p[0] += pkppb[list0[i]];
		sum.p[1] += pkppw[list1[i]];
	}
#endif

	return sum;
}



std::array<s32, 2> Evaluation09::doablack(const Position& pos, const int index[2]) {
	const Square sq_bk = pos.GetKingSquare(Black);
	const int* list0 = pos.GetCplist0();

	const auto* pkppb = EvalStorage::KPP[sq_bk][index[0]];
	std::array<s32, 2> sum = { { pkppb[list0[0]][0], pkppb[list0[0]][1] } };
	for (int i = 1; i < pos.GetNlist(); ++i) {
		sum[0] += pkppb[list0[i]][0];
		sum[1] += pkppb[list0[i]][1];
	}
	return sum;
}



std::array<s32, 2> Evaluation09::doawhite(const Position& pos, const int index[2]) {
	const Square sq_wk = pos.GetKingSquare(White);
	const int* list1 = pos.GetCplist1();

	const auto* pkppw = EvalStorage::KPP[UtilSquare::Inverse(sq_wk)][index[1]];
	std::array<s32, 2> sum = { { pkppw[list1[0]][0], pkppw[list1[0]][1] } };
	for (int i = 1; i < pos.GetNlist(); ++i) {
		sum[0] += pkppw[list1[i]][0];
		sum[1] += pkppw[list1[i]][1];
	}
	return sum;
}



#if defined INANIWA_SHIFT
Score Evaluation09::inaniwaScoreBody(const Position& pos) {
	Score GetScore = ScoreZero;
	if (pos.GetCsearcher()->inaniwaFlag == InaniwaIsBlack) {
		if (pos.GetPiece(B9) == WKnight) { GetScore += 700 * FVScale; }
		if (pos.GetPiece(H9) == WKnight) { GetScore += 700 * FVScale; }
		if (pos.GetPiece(A7) == WKnight) { GetScore += 700 * FVScale; }
		if (pos.GetPiece(I7) == WKnight) { GetScore += 700 * FVScale; }
		if (pos.GetPiece(C7) == WKnight) { GetScore += 400 * FVScale; }
		if (pos.GetPiece(G7) == WKnight) { GetScore += 400 * FVScale; }
		if (pos.GetPiece(B5) == WKnight) { GetScore += 700 * FVScale; }
		if (pos.GetPiece(H5) == WKnight) { GetScore += 700 * FVScale; }
		if (pos.GetPiece(D5) == WKnight) { GetScore += 100 * FVScale; }
		if (pos.GetPiece(F5) == WKnight) { GetScore += 100 * FVScale; }
		if (pos.GetPiece(E3) == BPawn) { GetScore += 200 * FVScale; }
		if (pos.GetPiece(E4) == BPawn) { GetScore += 200 * FVScale; }
		if (pos.GetPiece(E5) == BPawn) { GetScore += 200 * FVScale; }
	}
	else {
		assert(pos.GetCsearcher()->inaniwaFlag == InaniwaIsWhite);
		if (pos.GetPiece(B1) == BKnight) { GetScore -= 700 * FVScale; }
		if (pos.GetPiece(H1) == BKnight) { GetScore -= 700 * FVScale; }
		if (pos.GetPiece(A3) == BKnight) { GetScore -= 700 * FVScale; }
		if (pos.GetPiece(I3) == BKnight) { GetScore -= 700 * FVScale; }
		if (pos.GetPiece(C3) == BKnight) { GetScore -= 400 * FVScale; }
		if (pos.GetPiece(G3) == BKnight) { GetScore -= 400 * FVScale; }
		if (pos.GetPiece(B5) == BKnight) { GetScore -= 700 * FVScale; }
		if (pos.GetPiece(H5) == BKnight) { GetScore -= 700 * FVScale; }
		if (pos.GetPiece(D5) == BKnight) { GetScore -= 100 * FVScale; }
		if (pos.GetPiece(F5) == BKnight) { GetScore -= 100 * FVScale; }
		if (pos.GetPiece(E7) == WPawn) { GetScore -= 200 * FVScale; }
		if (pos.GetPiece(E6) == WPawn) { GetScore -= 200 * FVScale; }
		if (pos.GetPiece(E5) == WPawn) { GetScore -= 200 * FVScale; }
	}
	return GetScore;
}
inline Score Evaluation09::inaniwaScore(const Position& pos) {
	if (pos.GetCsearcher()->inaniwaFlag == NotInaniwa) return ScoreZero;
	return inaniwaScoreBody(pos);
}
#endif



bool Evaluation09::calcDifference(Position& pos, SearchStack* ss) {
#if defined INANIWA_SHIFT
	if (pos.GetCsearcher()->inaniwaFlag != NotInaniwa) return false;
#endif
	if ((ss - 1)->staticEvalRaw.p[0][0] == ScoreNotEvaluated)
		return false;

	const Move lastMove = (ss - 1)->currentMove;
	assert(lastMove != Move::GetMoveNull());

	if (lastMove.GetPieceTypeFrom() == King) {
		EvalSum diff = (ss - 1)->staticEvalRaw; // 本当は diff ではないので名前が良くない。
		const Square sq_bk = pos.GetKingSquare(Black);
		const Square sq_wk = pos.GetKingSquare(White);
		diff.p[2] = EvalStorage::KK[sq_bk][sq_wk];
		diff.p[2][0] += pos.GetMaterial() * FVScale;
		if (pos.GetTurn() == Black) {
			const auto* ppkppw = EvalStorage::KPP[UtilSquare::Inverse(sq_wk)];
			const int* list1 = pos.GetPlist1();
			diff.p[1][0] = 0;
			diff.p[1][1] = 0;
			for (int i = 0; i < pos.GetNlist(); ++i) {
				const int k1 = list1[i];
				const auto* pkppw = ppkppw[k1];
				for (int j = 0; j < i; ++j) {
					const int l1 = list1[j];
					diff.p[1] += pkppw[l1];
				}
				diff.p[2][0] -= EvalStorage::KKP[UtilSquare::Inverse(sq_wk)][UtilSquare::Inverse(sq_bk)][k1][0];
				diff.p[2][1] += EvalStorage::KKP[UtilSquare::Inverse(sq_wk)][UtilSquare::Inverse(sq_bk)][k1][1];
			}

			if (pos.GetCl().m_size == 2) {
				const int listIndex_cap = pos.GetCl().m_listindex[1];
				diff.p[0] += this->doablack(pos, pos.GetCl().m_clistpair[1].m_newlist);
				pos.GetPlist0()[listIndex_cap] = pos.GetCl().m_clistpair[1].m_oldlist[0];
				diff.p[0] -= this->doablack(pos, pos.GetCl().m_clistpair[1].m_oldlist);
				pos.GetPlist0()[listIndex_cap] = pos.GetCl().m_clistpair[1].m_newlist[0];
			}
		}
		else {
			const auto* ppkppb = EvalStorage::KPP[sq_bk];
			const int* list0 = pos.GetPlist0();
			diff.p[0][0] = 0;
			diff.p[0][1] = 0;
			for (int i = 0; i < pos.GetNlist(); ++i) {
				const int k0 = list0[i];
				const auto* pkppb = ppkppb[k0];
				for (int j = 0; j < i; ++j) {
					const int l0 = list0[j];
					diff.p[0] += pkppb[l0];
				}
				diff.p[2] += EvalStorage::KKP[sq_bk][sq_wk][k0];
			}

			if (pos.GetCl().m_size == 2) {
				const int listIndex_cap = pos.GetCl().m_listindex[1];
				diff.p[1] += this->doawhite(pos, pos.GetCl().m_clistpair[1].m_newlist);
				pos.GetPlist1()[listIndex_cap] = pos.GetCl().m_clistpair[1].m_oldlist[1];
				diff.p[1] -= this->doawhite(pos, pos.GetCl().m_clistpair[1].m_oldlist);
				pos.GetPlist1()[listIndex_cap] = pos.GetCl().m_clistpair[1].m_newlist[1];
			}
		}
		ss->staticEvalRaw = diff;
	}
	else {
		const int listIndex = pos.GetCl().m_listindex[0];
		auto diff = this->doapc(pos, pos.GetCl().m_clistpair[0].m_newlist);
		if (pos.GetCl().m_size == 1) {
			pos.GetPlist0()[listIndex] = pos.GetCl().m_clistpair[0].m_oldlist[0];
			pos.GetPlist1()[listIndex] = pos.GetCl().m_clistpair[0].m_oldlist[1];
			diff -= this->doapc(pos, pos.GetCl().m_clistpair[0].m_oldlist);
		}
		else {
			assert(pos.GetCl().m_size == 2);
			diff += this->doapc(pos, pos.GetCl().m_clistpair[1].m_newlist);
			diff.p[0] -= EvalStorage::KPP[pos.GetKingSquare(Black)][pos.GetCl().m_clistpair[0].m_newlist[0]][pos.GetCl().m_clistpair[1].m_newlist[0]];
			diff.p[1] -= EvalStorage::KPP[UtilSquare::Inverse(pos.GetKingSquare(White))][pos.GetCl().m_clistpair[0].m_newlist[1]][pos.GetCl().m_clistpair[1].m_newlist[1]];
			const int listIndex_cap = pos.GetCl().m_listindex[1];
			pos.GetPlist0()[listIndex_cap] = pos.GetCl().m_clistpair[1].m_oldlist[0];
			pos.GetPlist1()[listIndex_cap] = pos.GetCl().m_clistpair[1].m_oldlist[1];

			pos.GetPlist0()[listIndex] = pos.GetCl().m_clistpair[0].m_oldlist[0];
			pos.GetPlist1()[listIndex] = pos.GetCl().m_clistpair[0].m_oldlist[1];
			diff -= this->doapc(pos, pos.GetCl().m_clistpair[0].m_oldlist);

			diff -= this->doapc(pos, pos.GetCl().m_clistpair[1].m_oldlist);
			diff.p[0] += EvalStorage::KPP[pos.GetKingSquare(Black)][pos.GetCl().m_clistpair[0].m_oldlist[0]][pos.GetCl().m_clistpair[1].m_oldlist[0]];
			diff.p[1] += EvalStorage::KPP[UtilSquare::Inverse(pos.GetKingSquare(White))][pos.GetCl().m_clistpair[0].m_oldlist[1]][pos.GetCl().m_clistpair[1].m_oldlist[1]];
			pos.GetPlist0()[listIndex_cap] = pos.GetCl().m_clistpair[1].m_newlist[0];
			pos.GetPlist1()[listIndex_cap] = pos.GetCl().m_clistpair[1].m_newlist[1];
		}
		pos.GetPlist0()[listIndex] = pos.GetCl().m_clistpair[0].m_newlist[0];
		pos.GetPlist1()[listIndex] = pos.GetCl().m_clistpair[0].m_newlist[1];

		diff.p[2][0] += pos.GetMaterialDiff() * FVScale;

		ss->staticEvalRaw = diff + (ss - 1)->staticEvalRaw;
	}

	return true;
}



int Evaluation09::make_list_unUseDiff(const Position& pos, int list0[EvalList::m_ListSize], int list1[EvalList::m_ListSize], int nlist) {
	auto func = [&](const Bitboard& posBB, const int f_pt, const int e_pt) {
		Square sq;
		Bitboard bb;
		bb = (posBB)& pos.GetBbOf(Black);
		FOREACH_BB(bb, sq, {
			list0[nlist] = (f_pt)+sq;
		list1[nlist] = (e_pt)+UtilSquare::Inverse(sq);
		nlist += 1;
		});
		bb = (posBB)& pos.GetBbOf(White);
		FOREACH_BB(bb, sq, {
			list0[nlist] = (e_pt)+sq;
		list1[nlist] = (f_pt)+UtilSquare::Inverse(sq);
		nlist += 1;
		});
	};
	func(pos.GetBbOf(Pawn), f_pawn, e_pawn);
	func(pos.GetBbOf(Lance), f_lance, e_lance);
	func(pos.GetBbOf(Knight), f_knight, e_knight);
	func(pos.GetBbOf(Silver), f_silver, e_silver);
	const Bitboard goldsBB = pos.GetGoldsBB();
	func(goldsBB, f_gold, e_gold);
	func(pos.GetBbOf(Bishop), f_bishop, e_bishop);
	func(pos.GetBbOf(Horse), f_horse, e_horse);
	func(pos.GetBbOf(Rook), f_rook, e_rook);
	func(pos.GetBbOf(Dragon), f_dragon, e_dragon);

	return nlist;
}


void Evaluation09::evaluateBody(Position& pos, SearchStack* ss) {
	if (this->calcDifference(pos, ss)) {
		assert([&] {
			const auto score = ss->staticEvalRaw.sum(pos.GetTurn());
			return (this->evaluateUnUseDiff(pos) == score);
		}());
		return;
	}

	const Square sq_bk = pos.GetKingSquare(Black);
	const Square sq_wk = pos.GetKingSquare(White);
	const int* list0 = pos.GetPlist0();
	const int* list1 = pos.GetPlist1();

	const auto* ppkppb = EvalStorage::KPP[sq_bk];
	const auto* ppkppw = EvalStorage::KPP[UtilSquare::Inverse(sq_wk)];

	EvalSum sum;
	sum.p[2] = EvalStorage::KK[sq_bk][sq_wk];
#if defined USE_AVX2_EVAL || defined USE_SSE_EVAL
	sum.m[0] = _mm_setzero_si128();
	for (int i = 0; i < pos.GetNlist(); ++i) {
		const int k0 = GetList0[i];
		const int k1 = GetList1[i];
		const auto* pkppb = ppkppb[k0];
		const auto* pkppw = ppkppw[k1];
		for (int j = 0; j < i; ++j) {
			const int l0 = GetList0[j];
			const int l1 = GetList1[j];
			__m128i tmp;
			tmp = _mm_set_epi32(0, 0, *reinterpret_cast<const s32*>(&pkppw[l1][0]), *reinterpret_cast<const s32*>(&pkppb[l0][0]));
			tmp = _mm_cvtepi16_epi32(tmp);
			sum.m[0] = _mm_add_epi32(sum.m[0], tmp);
}
		sum.GetP[2] += EvalStorage::KKP[sq_bk][sq_wk][k0];
		}
#else
	// loop 開始を i = 1 からにして、i = 0 の分のKKPを先に足す。
	sum.p[2] += EvalStorage::KKP[sq_bk][sq_wk][list0[0]];
	sum.p[0][0] = 0;
	sum.p[0][1] = 0;
	sum.p[1][0] = 0;
	sum.p[1][1] = 0;
	for (int i = 1; i < pos.GetNlist(); ++i) {
		const int k0 = list0[i];
		const int k1 = list1[i];
		const auto* pkppb = ppkppb[k0];
		const auto* pkppw = ppkppw[k1];
		for (int j = 0; j < i; ++j) {
			const int l0 = list0[j];
			const int l1 = list1[j];
			sum.p[0] += pkppb[l0];
			sum.p[1] += pkppw[l1];
		}
		sum.p[2] += EvalStorage::KKP[sq_bk][sq_wk][k0];
	}
#endif

	sum.p[2][0] += pos.GetMaterial() * FVScale;
#if defined INANIWA_SHIFT
	sum.GetP[2][0] += inaniwaScore(pos);
#endif
	ss->staticEvalRaw = sum;

	Evaluation09 evaluation;
	assert(this->evaluateUnUseDiff(pos) == sum.sum(pos.GetTurn()));
}

// todo: 無名名前空間に入れる。
Score Evaluation09::evaluateUnUseDiff(const Position& pos) {
	int list0[EvalList::m_ListSize];
	int list1[EvalList::m_ListSize];

	const Hand handB = pos.GetHand(Black);
	const Hand handW = pos.GetHand(White);

	const Square sq_bk = pos.GetKingSquare(Black);
	const Square sq_wk = pos.GetKingSquare(White);
	int nlist = 0;

	auto func = [&](const Hand hand, const HandPiece hp, const int list0_index, const int list1_index) {
		for (u32 i = 1; i <= hand.NumOf(hp); ++i) {
			list0[nlist] = list0_index + i;
			list1[nlist] = list1_index + i;
			++nlist;
		}
	};
	func(handB, HPawn  , f_hand_pawn  , e_hand_pawn  );
	func(handW, HPawn  , e_hand_pawn  , f_hand_pawn  );
	func(handB, HLance , f_hand_lance , e_hand_lance );
	func(handW, HLance , e_hand_lance , f_hand_lance );
	func(handB, HKnight, f_hand_knight, e_hand_knight);
	func(handW, HKnight, e_hand_knight, f_hand_knight);
	func(handB, HSilver, f_hand_silver, e_hand_silver);
	func(handW, HSilver, e_hand_silver, f_hand_silver);
	func(handB, HGold  , f_hand_gold  , e_hand_gold  );
	func(handW, HGold  , e_hand_gold  , f_hand_gold  );
	func(handB, HBishop, f_hand_bishop, e_hand_bishop);
	func(handW, HBishop, e_hand_bishop, f_hand_bishop);
	func(handB, HRook  , f_hand_rook  , e_hand_rook  );
	func(handW, HRook  , e_hand_rook  , f_hand_rook  );

	nlist = this->make_list_unUseDiff(pos, list0, list1, nlist);

	const auto* ppkppb = EvalStorage::KPP[sq_bk         ];
	const auto* ppkppw = EvalStorage::KPP[UtilSquare::Inverse(sq_wk)];

	EvalSum score;
	score.p[2] = EvalStorage::KK[sq_bk][sq_wk];

	score.p[0][0] = 0;
	score.p[0][1] = 0;
	score.p[1][0] = 0;
	score.p[1][1] = 0;
	for (int i = 0; i < nlist; ++i) {
		const int k0 = list0[i];
		const int k1 = list1[i];
		const auto* pkppb = ppkppb[k0];
		const auto* pkppw = ppkppw[k1];
		for (int j = 0; j < i; ++j) {
			const int l0 = list0[j];
			const int l1 = list1[j];
			score.p[0] += pkppb[l0];
			score.p[1] += pkppw[l1];
		}
		score.p[2] += EvalStorage::KKP[sq_bk][sq_wk][k0];
	}

	score.p[2][0] += pos.GetMaterial() * FVScale;

#if defined INANIWA_SHIFT
	GetScore.GetP[2][0] += inaniwaScore(pos);
#endif

	return static_cast<Score>(score.sum(pos.GetTurn()));
}

Score Evaluation09::evaluate(Position& pos, SearchStack* ss) {
	if (ss->staticEvalRaw.p[0][0] != ScoreNotEvaluated) {
		const Score score = static_cast<Score>(ss->staticEvalRaw.sum(pos.GetTurn()));
		assert(score == evaluateUnUseDiff(pos));
		return score / FVScale;
	}

	const Key keyExcludeTurn = pos.GetKeyExcludeTurn();
	EvaluateHashEntry entry = *g_evalTable[keyExcludeTurn]; // atomic にデータを取得する必要がある。
	entry.decode();
	if (entry.key == keyExcludeTurn) {
		ss->staticEvalRaw = entry;
		assert(static_cast<Score>(ss->staticEvalRaw.sum(pos.GetTurn())) == evaluateUnUseDiff(pos));
		return static_cast<Score>(entry.sum(pos.GetTurn())) / FVScale;
	}

	this->evaluateBody(pos, ss);

	ss->staticEvalRaw.key = keyExcludeTurn;
	ss->staticEvalRaw.encode();
	*g_evalTable[keyExcludeTurn] = ss->staticEvalRaw;
	return static_cast<Score>(ss->staticEvalRaw.sum(pos.GetTurn())) / FVScale;
}
