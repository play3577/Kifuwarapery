﻿#pragma once


#include <algorithm>
#include "../n220_position/n220_650_position.hpp"
#include "../n220_position/n220_665_utilMoveStack.hpp"
#include "../n223_move____/n223_500_flashlight.hpp"
#include "../n640_searcher/n640_440_splitedNode.hpp" // Searcherと持ち合い
#include "../n640_searcher/n640_500_reductions.hpp"
#include "../n640_searcher/n640_510_futilityMargins.hpp"
#include "../n640_searcher/n640_520_futilityMoveCounts.hpp"

#include "../n885_searcher/n885_040_rucksack.hpp"//FIXME:
#include "../n886_repeType/n886_500_rtArray.hpp"//FIXME:
//class Rucksack;

using namespace std;


class NodetypeAbstract {
public:

	// テンプレートを使っている関数で使うには、static にするしかないぜ☆（＾ｑ＾）
	virtual inline void GoSearch(Rucksack& searcher, Position& pos, Flashlight* ss, SplitedNode& sp) const = 0;

	virtual inline const bool IsPvNode() const = 0;
	virtual inline const bool IsSplitedNode() const = 0;
	virtual inline const bool IsRootNode() const = 0;

	virtual inline void DoStep1a(
		bool& isGotoSplitPointStart,
		int& moveCount,
		int& playedMoveCount,
		bool& inCheck,
		Position& pos,
		SplitedNode** ppSplitedNode,
		Flashlight** ppFlashlight,
		Move& bestMove,
		Move& threatMove,
		ScoreIndex& bestScore,
		Move& ttMove,
		Move& excludedMove,
		ScoreIndex& ttScore
	) {

		// initialize node
		moveCount = playedMoveCount = 0;
		inCheck = pos.InCheck();

		if (this->IsSplitedNode()) {
			*ppSplitedNode = (*ppFlashlight)->m_splitedNode;
			bestMove = (*ppSplitedNode)->m_bestMove;
			threatMove = (*ppSplitedNode)->m_threatMove;
			bestScore = (*ppSplitedNode)->m_bestScore;
			//tte = nullptr;
			ttMove = excludedMove = g_MOVE_NONE;
			ttScore = ScoreNone;

			Evaluation09 evaluation;
			evaluation.evaluate(pos, *ppFlashlight);

			assert(-ScoreInfinite < (*ppSplitedNode)->m_bestScore && 0 < (*ppSplitedNode)->m_moveCount);

			isGotoSplitPointStart = true;
			return;
			//goto split_point_start;
		}
	}

	virtual inline void DoStep1b(
		ScoreIndex& bestScore,
		Flashlight** ppFlashlight,
		Move& threatMove,
		Move& bestMove
		) {

		bestScore = -ScoreInfinite;
		(*ppFlashlight)->m_currentMove = threatMove = bestMove = ((*ppFlashlight) + 1)->m_excludedMove = g_MOVE_NONE;
		(*ppFlashlight)->m_ply = ((*ppFlashlight) - 1)->m_ply + 1;
		((*ppFlashlight) + 1)->m_skipNullMove = false;
		((*ppFlashlight) + 1)->m_reduction = Depth0;
		((*ppFlashlight) + 2)->m_killers[0] = ((*ppFlashlight) + 2)->m_killers[1] = g_MOVE_NONE;
	}

	virtual inline void DoStep1c(
		Military** ppThisThread,
		const Flashlight* pFlashlight
		) {
		// PVノードのみ、最大Plyの更新の可能性があるぜ☆（＾ｑ＾）
		if ((*ppThisThread)->m_maxPly < pFlashlight->m_ply) {
			(*ppThisThread)->m_maxPly = pFlashlight->m_ply;
		}
	}

	virtual inline void DoStep2(
		bool& isReturnWithScore,
		ScoreIndex& returnScore,
		Position& pos,
		Rucksack& rucksack,
		Flashlight** ppFlashlight
		)
	{
		// stop と最大探索深さのチェック
		g_repetitionTypeArray.m_repetitionTypeArray[pos.IsDraw(16)]->CheckStopAndMaxPly(
			isReturnWithScore, returnScore, &rucksack, (*ppFlashlight));
	}

	virtual inline void DoStep3(
		bool& isReturnWithScore,
		ScoreIndex& returnScore,
		Flashlight** ppFlashlight,
		ScoreIndex& alpha,
		ScoreIndex& beta
	) {
		// ルート以外のみで行われる手続きだぜ☆（＾ｑ＾）！
		alpha = std::max(UtilScore::MatedIn((*ppFlashlight)->m_ply), alpha);
		beta = std::min(UtilScore::MateIn((*ppFlashlight)->m_ply + 1), beta);
		if (beta <= alpha) {
			isReturnWithScore = true;
			returnScore = alpha;
			return;
			//return alpha;
		}
	}

	virtual inline void DoStep4(
		Move& excludedMove,
		Flashlight** ppFlashlight,
		Key& posKey,
		Position& pos,
		const TTEntry* pTtEntry,
		Rucksack& rucksack,
		ScoreIndex& ttScore
	) {
		// trans position table lookup
		excludedMove = (*ppFlashlight)->m_excludedMove;
		posKey = (excludedMove.IsNone() ? pos.GetKey() : pos.GetExclusionKey());
		pTtEntry = rucksack.m_tt.Probe(posKey);
		ttScore = (pTtEntry != nullptr ? rucksack.ConvertScoreFromTT(pTtEntry->GetScore(), (*ppFlashlight)->m_ply) : ScoreNone);
	}

	// ルートノードか、それ以外かで　値が分かれるぜ☆（＾ｑ＾）
	virtual inline void DoStep4x(
		Move& ttMove,
		Rucksack& rucksack,
		const TTEntry* pTtEntry,
		Position& pos
		) = 0;

	virtual inline void DoStep4y(
		bool& isReturnWithScore,
		ScoreIndex& returnScore,
		Rucksack& rucksack,
		const TTEntry* pTtEntry,
		const Depth depth,
		ScoreIndex& ttScore,
		ScoreIndex& beta,
		Flashlight** ppFlashlight,
		Move& ttMove
		) {

		// ルートノード以外だけにある手続きだぜ☆（＾ｑ＾）
		if (pTtEntry != nullptr
			&& depth <= pTtEntry->GetDepth()
			&& ttScore != ScoreNone // アクセス競合が起きたときのみ、ここに引っかかる。
			&& (this->IsPvNode() ? pTtEntry->GetType() == BoundExact
				: (beta <= ttScore ? (pTtEntry->GetType() & BoundLower)
					: (pTtEntry->GetType() & BoundUpper))))
		{
			rucksack.m_tt.Refresh(pTtEntry);
			(*ppFlashlight)->m_currentMove = ttMove; // Move::moveNone() もありえる。

			if (beta <= ttScore
				&& !ttMove.IsNone()
				&& !ttMove.IsCaptureOrPawnPromotion()
				&& ttMove != (*ppFlashlight)->m_killers[0])
			{
				(*ppFlashlight)->m_killers[1] = (*ppFlashlight)->m_killers[0];
				(*ppFlashlight)->m_killers[0] = ttMove;
			}

			isReturnWithScore = true;
			returnScore = ttScore;
			return;
			//return ttScore;
		}
	}

	virtual inline void DoStep4z(
		bool& isReturnWithScore,
		ScoreIndex& returnScore,
		Rucksack& rucksack,
		bool& inCheck,
		Move& move,
		Position& pos,
		Flashlight** ppFlashlight,
		ScoreIndex& bestScore,
		Key& posKey,
		const Depth depth,
		Move& bestMove
		) {
		// ルートノード以外だけにある手続きだぜ☆（＾ｑ＾）
#if 1
		if (!inCheck)
		{
			if (!(move = pos.GetMateMoveIn1Ply()).IsNone()) {
				(*ppFlashlight)->m_staticEval = bestScore = UtilScore::MateIn((*ppFlashlight)->m_ply);
				rucksack.m_tt.Store(posKey, rucksack.ConvertScoreToTT(bestScore, (*ppFlashlight)->m_ply), BoundExact, depth,
					move, (*ppFlashlight)->m_staticEval);
				bestMove = move;

				isReturnWithScore = true;
				returnScore = bestScore;
				return;
				//return bestScore;
			}
		}
#endif
	}

	virtual inline void DoStep5(
		bool& isGotoIidStart,
		Rucksack& rucksack,
		ScoreIndex& eval,
		Flashlight** ppFlashlight,
		Position& pos,
		bool& inCheck,
		const TTEntry* pTtEntry,
		ScoreIndex& ttScore,
		Key& posKey,
		Move& move
	) {
		// evaluate the position statically
		Evaluation09 evaluation;
		eval = (*ppFlashlight)->m_staticEval = evaluation.evaluate(pos, (*ppFlashlight)); // Bonanza の差分評価の為、evaluate() を常に呼ぶ。
		if (inCheck) {
			eval = (*ppFlashlight)->m_staticEval = ScoreNone;
			isGotoIidStart = true;
			return;
			//goto iid_start;
		}
		else if (pTtEntry != nullptr) {
			if (ttScore != ScoreNone
				&& (pTtEntry->GetType() & (eval < ttScore ? Bound::BoundLower : Bound::BoundUpper)))
			{
				eval = ttScore;
			}
		}
		else {
			rucksack.m_tt.Store(posKey, ScoreNone, BoundNone, DepthNone,
				g_MOVE_NONE, (*ppFlashlight)->m_staticEval);
		}

		// 一手前の指し手について、history を更新する。
		// todo: ここの条件はもう少し考えた方が良い。
		if ((move = ((*ppFlashlight) - 1)->m_currentMove) != g_MOVE_NULL
			&& ((*ppFlashlight) - 1)->m_staticEval != ScoreNone
			&& (*ppFlashlight)->m_staticEval != ScoreNone
			&& !move.IsCaptureOrPawnPromotion() // 前回(一手前)の指し手が駒取りでなかった。
			)
		{
			const Square to = move.To();
			rucksack.m_gains.Update(move.IsDrop(), pos.GetPiece(to), to, -((*ppFlashlight) - 1)->m_staticEval - (*ppFlashlight)->m_staticEval);
		}
	}

	virtual inline void DoStep6(
		bool& isReturnWithScore,
		ScoreIndex& returnScore,
		Rucksack& rucksack,
		const Depth depth,
		ScoreIndex& eval,
		ScoreIndex& beta,
		Move& ttMove,
		Position& pos,
		Flashlight** ppFlashlight
	) {
		// razoring
		if (!this->IsPvNode()
			&& depth < 4 * OnePly
			&& eval + rucksack.razorMargin(depth) < beta
			&& ttMove.IsNone()
			&& abs(beta) < ScoreMateInMaxPly)
		{
			const ScoreIndex rbeta = beta - rucksack.razorMargin(depth);
			const ScoreIndex s = Hitchhiker::Qsearch(rucksack, N02_NonPV, false, pos, (*ppFlashlight), rbeta - 1, rbeta, Depth0);
			if (s < rbeta) {
				isReturnWithScore = true;
				returnScore = s;
				return;
				//return s;
			}
		}
	}

	virtual inline void DoStep7(
		bool& isReturnWithScore,
		ScoreIndex& returnScore,
		Flashlight** ppFlashlight,
		const Depth depth,
		ScoreIndex& beta,
		ScoreIndex& eval
	) {
		// static null move pruning
		if (!this->IsPvNode()
			&& !(*ppFlashlight)->m_skipNullMove
			&& depth < 4 * OnePly
			&& beta <= eval - g_futilityMargins.m_FutilityMargins[depth][0]
			&& abs(beta) < ScoreMateInMaxPly)
		{
			bool isReturnWithScore = true;
			returnScore = eval - g_futilityMargins.m_FutilityMargins[depth][0];
			//return eval - g_futilityMargins.m_FutilityMargins[depth][0];
			return;
		}
	}

	virtual inline void DoStep8(
		bool& isReturnWithScore,
		ScoreIndex& returnScore,
		Rucksack& rucksack,
		Flashlight** ppFlashlight,
		const Depth depth,
		ScoreIndex& beta,
		ScoreIndex& eval,
		Position& pos,
		StateInfo& st,
		ScoreIndex& alpha,
		const bool cutNode,
		Move& threatMove
	) {

		// null move
		if (!this->IsPvNode()
			&& !(*ppFlashlight)->m_skipNullMove
			&& 2 * OnePly <= depth
			&& beta <= eval
			&& abs(beta) < ScoreMateInMaxPly)
		{
			(*ppFlashlight)->m_currentMove = g_MOVE_NULL;
			Depth reduction = static_cast<Depth>(3) * OnePly + depth / 4;

			if (beta < eval - PieceScore::m_pawn) {
				reduction += OnePly;
			}

			pos.DoNullMove(true, st);
			((*ppFlashlight) + 1)->m_staticEvalRaw = (*ppFlashlight)->m_staticEvalRaw; // 評価値の差分評価の為。
			((*ppFlashlight) + 1)->m_skipNullMove = true;

			ScoreIndex nullScore = (depth - reduction < OnePly ?
				//────────────────────────────────────────────────────────────────────────────────
				// 深さが２手（先後１組）以上なら　クイックな探索☆？（＾ｑ＾）
				//────────────────────────────────────────────────────────────────────────────────
				-Hitchhiker::Qsearch(rucksack, N02_NonPV, false, pos, (*ppFlashlight) + 1, -beta, -alpha, Depth0)
				//────────────────────────────────────────────────────────────────────────────────
				// 深さが２手（先後１組）未満なら　ふつーの探索☆？（＾ｑ＾）
				//────────────────────────────────────────────────────────────────────────────────
				: -Hitchhiker::Travel_885_510(rucksack, NodeType::N02_NonPV, pos, (*ppFlashlight) + 1, -beta, -alpha, depth - reduction, !cutNode));

			((*ppFlashlight) + 1)->m_skipNullMove = false;
			pos.DoNullMove(false, st);

			if (beta <= nullScore) {
				if (ScoreMateInMaxPly <= nullScore) {
					nullScore = beta;
				}

				if (depth < 6 * OnePly) {
					isReturnWithScore = true;
					returnScore = nullScore;
					return;
					//return nullScore;
				}

				(*ppFlashlight)->m_skipNullMove = true;
				assert(Depth0 < depth - reduction);
				//────────────────────────────────────────────────────────────────────────────────
				// 探索☆？（＾ｑ＾）
				//────────────────────────────────────────────────────────────────────────────────
				const ScoreIndex s = Hitchhiker::Travel_885_510(rucksack, NodeType::N02_NonPV, pos, (*ppFlashlight), alpha, beta, depth - reduction, false);
				(*ppFlashlight)->m_skipNullMove = false;

				if (beta <= s) {
					isReturnWithScore = true;
					returnScore = nullScore;
					return;
					//return nullScore;
				}
			}
			else {
				// fail low
				threatMove = ((*ppFlashlight) + 1)->m_currentMove;
				if (depth < 5 * OnePly
					&& ((*ppFlashlight) - 1)->m_reduction != Depth0
					&& !threatMove.IsNone()
					&& rucksack.allows(pos, ((*ppFlashlight) - 1)->m_currentMove, threatMove))
				{
					isReturnWithScore = true;
					returnScore = beta - 1;
					return;
					//return beta - 1;
				}
			}
		}
	}

	virtual inline void DoStep9(
		bool& isReturnWithScore,
		Rucksack& rucksack,
		const Depth& depth,
		Flashlight** ppFlashlight,
		ScoreIndex& beta,
		Move& move,
		Position& pos,
		Move& ttMove,
		StateInfo& st,
		ScoreIndex& score,
		const bool cutNode
		) {

		// probcut
		if (!this->IsPvNode()
			&& 5 * OnePly <= depth
			&& !(*ppFlashlight)->m_skipNullMove
			// 確実にバグらせないようにする。
			&& abs(beta) < ScoreInfinite - 200)
		{
			const ScoreIndex rbeta = beta + 200;
			const Depth rdepth = depth - OnePly - 3 * OnePly;

			assert(OnePly <= rdepth);
			assert(!((*ppFlashlight) - 1)->m_currentMove.IsNone());
			assert(((*ppFlashlight) - 1)->m_currentMove != g_MOVE_NULL);

			assert(move == (ppFlashlight - 1)->m_currentMove);
			// move.cap() は前回(一手前)の指し手で取った駒の種類
			NextmoveEvent mp(pos, ttMove, rucksack.m_history, move.GetCap());
			const CheckInfo ci(pos);
			while (!(move = mp.GetNextMove(false)).IsNone()) {
				if (pos.IsPseudoLegalMoveIsLegal<false, false>(move, ci.m_pinned)) {
					(*ppFlashlight)->m_currentMove = move;
					pos.DoMove(move, st, ci, pos.IsMoveGivesCheck(move, ci));
					((*ppFlashlight) + 1)->m_staticEvalRaw.m_p[0][0] = ScoreNotEvaluated;

					//────────────────────────────────────────────────────────────────────────────────
					// 探索☆？（＾ｑ＾）
					//────────────────────────────────────────────────────────────────────────────────
					score = -Hitchhiker::Travel_885_510(rucksack, NodeType::N02_NonPV, pos, (*ppFlashlight) + 1, -rbeta, -rbeta + 1, rdepth, !cutNode);
					pos.UndoMove(move);
					if (rbeta <= score) {
						isReturnWithScore = true;
						return;
						//return score;
					}
				}
			}
		}
	}

	virtual inline void DoStep10(
		const Depth depth,
		Move& ttMove,
		bool& inCheck,
		ScoreIndex& beta,
		Flashlight** ppFlashlight,
		Rucksack& rucksack,
		Position& pos,
		ScoreIndex& alpha,
		const TTEntry* pTtEntry,
		Key& posKey
		)
	{
		// internal iterative deepening
		if ((this->IsPvNode() ? 5 * OnePly : 8 * OnePly) <= depth
			&& ttMove.IsNone()
			&& (this->IsPvNode() || (!inCheck && beta <= (*ppFlashlight)->m_staticEval + static_cast<ScoreIndex>(256))))
		{
			//const Depth d = depth - 2 * OnePly - (PVNode ? Depth0 : depth / 4);
			const Depth d = (this->IsPvNode() ? depth - 2 * OnePly : depth / 2);

			(*ppFlashlight)->m_skipNullMove = true;
			if (this->IsPvNode())
			{
				//────────────────────────────────────────────────────────────────────────────────
				// 探索☆？（＾ｑ＾）
				//────────────────────────────────────────────────────────────────────────────────
				Hitchhiker::Travel_885_510(rucksack, NodeType::N01_PV, pos, (*ppFlashlight), alpha, beta, d, true);
			}
			else
			{
				//────────────────────────────────────────────────────────────────────────────────
				// 探索☆？（＾ｑ＾）
				//────────────────────────────────────────────────────────────────────────────────
				Hitchhiker::Travel_885_510(rucksack, NodeType::N02_NonPV, pos, (*ppFlashlight), alpha, beta, d, true);
			}
			(*ppFlashlight)->m_skipNullMove = false;

			pTtEntry = rucksack.m_tt.Probe(posKey);
			ttMove = (pTtEntry != nullptr ?
				UtilMoveStack::Move16toMove(pTtEntry->GetMove(), pos) :
				g_MOVE_NONE);
		}
	}

	// ルートノードか、そうでないかで分かれるぜ☆（＾ｑ＾）
	virtual inline void DoStep11A_BeforeLoop_SplitPointStart(
		Move& ttMove,
		const Depth depth,
		ScoreIndex& score,
		ScoreIndex& bestScore,
		bool& singularExtensionNode,
		Move& excludedMove,
		const TTEntry* pTtEntry
		) = 0;

	virtual inline void DoStep11Ba_LoopHeader(
		bool& isContinue,
		const Move& move,
		const Move& excludedMove
		)
	{
		if (move == excludedMove) {
			isContinue = true;
			return;
		}
	}

	virtual inline void DoStep11Bb_LoopHeader(
		bool& isContinue,
		const Rucksack& rucksack,
		const Move& move
		) {
		// ルートノードにのみある手続きだぜ☆！（＾ｑ＾）
		if (std::find(rucksack.m_rootMoves.begin() + rucksack.m_pvIdx,
				rucksack.m_rootMoves.end(),
				move) == rucksack.m_rootMoves.end())
		{
			isContinue = true;
			return;
		}
	}


	virtual inline void DoStep11B_LoopHeader(
		bool& isContinue,
		Rucksack& rucksack,
		Move& move,
		Move& excludedMove,
		Position& pos,
		const CheckInfo& ci,
		int& moveCount,
		SplitedNode** ppSplitedNode,
		Depth& extension,
		bool& captureOrPawnPromotion,
		bool& givesCheck,
		bool& dangerous
		)
	{
		if (this->IsSplitedNode()) {
			if (!pos.IsPseudoLegalMoveIsLegal<false, false>(move, ci.m_pinned)) {
				isContinue = true;
				return;
			}
			moveCount = ++(*ppSplitedNode)->m_moveCount;
			(*ppSplitedNode)->m_mutex.unlock();
		}
		else {
			++moveCount;
		}


		if (this->IsRootNode()) {
			rucksack.m_signals.m_firstRootMove = (moveCount == 1);
#if 0
			if (GetThisThread == rucksack.m_ownerHerosPub.GetFirstCaptain() && 3000 < rucksack.m_stopwatch.GetElapsed()) {
				SYNCCOUT << "info depth " << GetDepth / OnePly
					<< " currmove " << GetMove.ToUSI()
					<< " currmovenumber " << rucksack.m_moveCount + rucksack.m_pvIdx << SYNCENDL;
			}
#endif
		}

		extension = Depth0;
		captureOrPawnPromotion = move.IsCaptureOrPawnPromotion();
		givesCheck = pos.IsMoveGivesCheck(move, ci);
		dangerous = givesCheck; // todo: not implement

	}

	virtual inline void DoStep12(
		Rucksack& rucksack,
		bool& givesCheck,
		Position& pos,
		Move& move,
		Depth& extension,
		bool& singularExtensionNode,
		Move& ttMove,
		ScoreIndex& ttScore,
		const CheckInfo& ci,
		const Depth depth,
		Flashlight** ppFlashlight,
		ScoreIndex& score,
		const bool cutNode,
		ScoreIndex& beta,
		Depth& newDepth
		) {

		if (givesCheck && ScoreIndex::ScoreZero <= pos.GetSeeSign(move))
		{
			extension = Depth::OnePly;
		}

		// singuler extension
		if (singularExtensionNode
			&& extension == Depth0
			&& move == ttMove
			&& pos.IsPseudoLegalMoveIsLegal<false, false>(move, ci.m_pinned)
			&& abs(ttScore) < PieceScore::m_ScoreKnownWin)
		{
			assert(ttScore != ScoreNone);

			const ScoreIndex rBeta = ttScore - static_cast<ScoreIndex>(depth);
			(*ppFlashlight)->m_excludedMove = move;
			(*ppFlashlight)->m_skipNullMove = true;
			//────────────────────────────────────────────────────────────────────────────────
			// 探索☆？（＾ｑ＾）
			//────────────────────────────────────────────────────────────────────────────────
			score = Hitchhiker::Travel_885_510(rucksack, N02_NonPV, pos, (*ppFlashlight), rBeta - 1, rBeta, depth / 2, cutNode);
			(*ppFlashlight)->m_skipNullMove = false;
			(*ppFlashlight)->m_excludedMove = g_MOVE_NONE;

			if (score < rBeta) {
				//extension = OnePly;
				extension = (beta <= rBeta ? OnePly + OnePly / 2 : OnePly);
			}
		}

		newDepth = depth - OnePly + extension;
	}

	virtual inline void DoStep13(
		bool& isContinue,
		Rucksack& rucksack,
		bool& captureOrPawnPromotion,
		bool& inCheck,
		bool& dangerous,
		ScoreIndex& bestScore,
		Move& move,
		Move& ttMove,
		const Depth depth,
		int& moveCount,
		Move& threatMove,
		Position& pos,
		SplitedNode** ppSplitedNode,
		Depth& newDepth,
		Flashlight** ppFlashlight,
		ScoreIndex& beta,
		const CheckInfo& ci,
		bool& isPVMove,
		int& playedMoveCount,
		Move movesSearched[64]
		) {

		// futility pruning
		if (!this->IsPvNode()
			&& !captureOrPawnPromotion
			&& !inCheck
			&& !dangerous
			//&& move != ttMove // 次の行がtrueならこれもtrueなので条件から省く。
			&& ScoreMatedInMaxPly < bestScore)
		{
			assert(move != ttMove);
			// move count based pruning
			if (depth < 16 * OnePly
				&& g_futilityMoveCounts.m_futilityMoveCounts[depth] <= moveCount
				&& (threatMove.IsNone() || !rucksack.refutes(pos, move, threatMove)))
			{
				if (this->IsSplitedNode()) {
					(*ppSplitedNode)->m_mutex.lock();
				}
				isContinue = true;
				return;
			}

			// score based pruning
			const Depth predictedDepth = newDepth - g_reductions.reduction(this->IsPvNode(), depth, moveCount);
			// gain を 2倍にする。
			const ScoreIndex futilityScore = (*ppFlashlight)->m_staticEval + g_futilityMargins.GetFutilityMargin(predictedDepth, moveCount)
				+ 2 * rucksack.m_gains.GetValue(move.IsDrop(), ConvPiece::FROM_COLOR_AND_PIECE_TYPE10(pos.GetTurn(), move.GetPieceTypeFromOrDropped()), move.To());

			if (futilityScore < beta) {
				bestScore = std::max(bestScore, futilityScore);
				if (this->IsSplitedNode()) {
					(*ppSplitedNode)->m_mutex.lock();
					if ((*ppSplitedNode)->m_bestScore < bestScore) {
						(*ppSplitedNode)->m_bestScore = bestScore;
					}
				}
				isContinue = true;
				return;
			}

			if (predictedDepth < 4 * OnePly
				&& pos.GetSeeSign(move) < ScoreZero)
			{
				if (this->IsSplitedNode()) {
					(*ppSplitedNode)->m_mutex.lock();
				}
				isContinue = true;
				return;
			}
		}

		// RootNode, SPNode はすでに合法手であることを確認済み。
		if (!this->IsRootNode() && !this->IsSplitedNode() && !pos.IsPseudoLegalMoveIsLegal<false, false>(move, ci.m_pinned)) {
			--moveCount;
			isContinue = true;
			return;
		}

		isPVMove = (this->IsPvNode() && moveCount == 1);
		(*ppFlashlight)->m_currentMove = move;
		if (!this->IsSplitedNode() && !captureOrPawnPromotion && playedMoveCount < 64) {
			movesSearched[playedMoveCount++] = move;
		}

	}

	virtual inline void DoStep14(
		Position& pos,
		Move& move,
		StateInfo& st,
		const CheckInfo& ci,
		bool& givesCheck,
		Flashlight** ppFlashlight
		) {
		pos.DoMove(move, st, ci, givesCheck);
		((*ppFlashlight) + 1)->m_staticEvalRaw.m_p[0][0] = ScoreIndex::ScoreNotEvaluated;
	}

	virtual inline void DoStep15(
		Rucksack& rucksack,
		const Depth depth,
		bool& isPVMove,
		bool& captureOrPawnPromotion,
		Move& move,
		Move& ttMove,
		Flashlight** ppFlashlight,
		const bool PVNode,
		int& moveCount,
		const bool cutNode,
		Depth& newDepth,
		ScoreIndex& alpha,
		SplitedNode** ppSplitedNode,
		ScoreIndex& score,
		Position& pos,
		bool& doFullDepthSearch
		) {
		// LMR
		if (3 * OnePly <= depth
			&& !isPVMove
			&& !captureOrPawnPromotion
			&& move != ttMove
			&& (*ppFlashlight)->m_killers[0] != move
			&& (*ppFlashlight)->m_killers[1] != move)
		{
			(*ppFlashlight)->m_reduction = g_reductions.reduction(PVNode, depth, moveCount);
			if (!PVNode && cutNode) {
				(*ppFlashlight)->m_reduction += OnePly;
			}
			const Depth d = std::max(newDepth - (*ppFlashlight)->m_reduction, OnePly);
			if (this->IsSplitedNode()) {
				alpha = (*ppSplitedNode)->m_alpha;
			}
			//────────────────────────────────────────────────────────────────────────────────
			// 探索☆？（＾ｑ＾）
			//────────────────────────────────────────────────────────────────────────────────
			// PVS
			score = -Hitchhiker::Travel_885_510(
				rucksack, N02_NonPV, pos, (*ppFlashlight) + 1, -(alpha + 1), -alpha, d, true);

			doFullDepthSearch = (alpha < score && (*ppFlashlight)->m_reduction != Depth0);
			(*ppFlashlight)->m_reduction = Depth0;
		}
		else {
			doFullDepthSearch = !isPVMove;
		}
	}

	virtual inline void DoStep16(
		Rucksack& rucksack,
		bool& doFullDepthSearch,
		SplitedNode** ppSplitedNode,
		ScoreIndex& alpha,
		ScoreIndex& score,
		Depth& newDepth,
		bool& givesCheck,
		Position& pos,
		Flashlight** ppFlashlight,
		const bool cutNode,
		bool& isPVMove,
		ScoreIndex& beta
		) {

		// full depth search
		// PVS
		if (doFullDepthSearch) {
			if (this->IsSplitedNode()) {
				alpha = (*ppSplitedNode)->m_alpha;
			}
			score = (newDepth < OnePly ?
				(givesCheck ? -Hitchhiker::Qsearch(rucksack, N02_NonPV, true, pos, (*ppFlashlight) + 1, -(alpha + 1), -alpha, Depth0)
					: -Hitchhiker::Qsearch(rucksack, N02_NonPV, false, pos, (*ppFlashlight) + 1, -(alpha + 1), -alpha, Depth0))
				//────────────────────────────────────────────────────────────────────────────────
				// 探索☆？（＾ｑ＾）
				//────────────────────────────────────────────────────────────────────────────────
				: -Hitchhiker::Travel_885_510(
					rucksack, N02_NonPV, pos, (*ppFlashlight) + 1, -(alpha + 1), -alpha, newDepth, !cutNode));
		}

		// 通常の探索
		if (this->IsPvNode() && (isPVMove || (alpha < score && (this->IsRootNode() || score < beta)))) {
			score = (newDepth < OnePly ?
				(givesCheck ? -Hitchhiker::Qsearch(rucksack, N01_PV, true, pos, (*ppFlashlight) + 1, -beta, -alpha, Depth0)
					: -Hitchhiker::Qsearch(rucksack, N01_PV, false, pos, (*ppFlashlight) + 1, -beta, -alpha, Depth0))
				//────────────────────────────────────────────────────────────────────────────────
				// 探索☆？（＾ｑ＾）
				//────────────────────────────────────────────────────────────────────────────────
				: -Hitchhiker::Travel_885_510(
					rucksack, N01_PV, pos, (*ppFlashlight) + 1, -beta, -alpha, newDepth, false));
		}

	}

	virtual inline void DoStep17(
		Position& pos,
		Move& move
		) {
		pos.UndoMove(move);
	}

	virtual inline void DoStep18(
		bool& isBreak,
		Rucksack& rucksack,
		Move& move,
		bool& isPVMove,
		ScoreIndex& alpha,
		ScoreIndex& score,
		Position& pos,
		ScoreIndex& bestScore,
		SplitedNode** ppSplitedNode,
		Move& bestMove,
		ScoreIndex& beta
		) {

		if (this->IsRootNode()) {
			RootMove& rm = *std::find(rucksack.m_rootMoves.begin(), rucksack.m_rootMoves.end(), move);
			if (isPVMove || alpha < score) {
				// PV move or new best move
				rm.m_score_ = score;
#if defined BISHOP_IN_DANGER
				if ((bishopInDangerFlag == BlackBishopInDangerIn28 && GetMove.ToCSA() == "0082KA")
					|| (bishopInDangerFlag == WhiteBishopInDangerIn28 && GetMove.ToCSA() == "0028KA")
					|| (bishopInDangerFlag == BlackBishopInDangerIn78 && GetMove.ToCSA() == "0032KA")
					|| (bishopInDangerFlag == WhiteBishopInDangerIn78 && GetMove.ToCSA() == "0078KA"))
				{
					rm.m_score_ -= m_engineOptions["Danger_Demerit_Score"];
				}
#endif
				rm.ExtractPvFromTT(pos);

				if (!isPVMove) {
					++rucksack.m_bestMoveChanges;
				}
			}
			else {
				rm.m_score_ = -ScoreInfinite;
			}
		}

		if (bestScore < score) {
			bestScore = (this->IsSplitedNode() ? (*ppSplitedNode)->m_bestScore = score : score);

			if (alpha < score) {
				bestMove = (this->IsSplitedNode() ? (*ppSplitedNode)->m_bestMove = move : move);

				if (this->IsPvNode() && score < beta) {
					alpha = (this->IsSplitedNode() ? (*ppSplitedNode)->m_alpha = score : score);
				}
				else {
					// fail high
					if (this->IsSplitedNode()) {
						(*ppSplitedNode)->m_cutoff = true;
					}
					isBreak = true;
					return;
				}
			}
		}

	}

	virtual inline void DoStep19(
		bool& isBreak,
		Rucksack& rucksack,
		const Depth depth,
		Military** ppThisThread,
		ScoreIndex& bestScore,
		ScoreIndex& beta,
		Position& pos,
		Flashlight** ppFlashlight,
		ScoreIndex& alpha,
		Move& bestMove,
		Move& threatMove,
		int& moveCount,
		NextmoveEvent& mp,
		NodeType& NT,
		const bool cutNode
		){
		if (!this->IsSplitedNode()
			&& rucksack.m_ownerHerosPub.GetMinSplitDepth() <= depth
			&& rucksack.m_ownerHerosPub.GetAvailableSlave(*ppThisThread)
			&& (*ppThisThread)->m_splitedNodesSize < g_MaxSplitedNodesPerThread)
		{
			assert(bestScore < beta);
			(*ppThisThread)->ForkNewFighter<Rucksack::FakeSplit>(
				pos, *ppFlashlight, alpha, beta, bestScore, bestMove,
				depth, threatMove, moveCount, mp, NT, cutNode
				);
			if (beta <= bestScore) {
				isBreak = true;
				return;
			}
		}

	}

	virtual inline void DoStep20(
		int& moveCount,
		Move& excludedMove,
		Rucksack& rucksack,
		ScoreIndex& alpha,
		Flashlight** ppFlashlight,//サーチスタック
		ScoreIndex& bestScore,
		int& playedMoveCount,
		ScoreIndex& beta,
		Key& posKey,
		const Depth depth,
		Move& bestMove,
		bool& inCheck,
		Position& pos,
		Move movesSearched[64]
	) {
		if (moveCount == 0) {
			bestScore = !excludedMove.IsNone() ? alpha : UtilScore::MatedIn((*ppFlashlight)->m_ply);
			return;
		}

		if (bestScore == -ScoreInfinite) {
			assert(playedMoveCount == 0);
			bestScore = alpha;
		}

		if (beta <= bestScore) {
			// failed high
			rucksack.m_tt.Store(posKey, rucksack.ConvertScoreToTT(bestScore, (*ppFlashlight)->m_ply), BoundLower, depth,
				bestMove, (*ppFlashlight)->m_staticEval);

			if (!bestMove.IsCaptureOrPawnPromotion() && !inCheck) {
				if (bestMove != (*ppFlashlight)->m_killers[0]) {
					(*ppFlashlight)->m_killers[1] = (*ppFlashlight)->m_killers[0];
					(*ppFlashlight)->m_killers[0] = bestMove;
				}

				const ScoreIndex bonus = static_cast<ScoreIndex>(depth * depth);
				const Piece pc1 = ConvPiece::FROM_COLOR_AND_PIECE_TYPE10(pos.GetTurn(), bestMove.GetPieceTypeFromOrDropped());
				rucksack.m_history.Update(bestMove.IsDrop(), pc1, bestMove.To(), bonus);

				for (int i = 0; i < playedMoveCount - 1; ++i) {
					const Move m = movesSearched[i];
					const Piece pc2 = ConvPiece::FROM_COLOR_AND_PIECE_TYPE10(pos.GetTurn(), m.GetPieceTypeFromOrDropped());
					rucksack.m_history.Update(m.IsDrop(), pc2, m.To(), -bonus);
				}
			}
		}
		else {
			// failed low or PV search
			rucksack.m_tt.Store(
				posKey,
				rucksack.ConvertScoreToTT(bestScore, (*ppFlashlight)->m_ply),
				this->GetBoundAtStep20(!bestMove.IsNone()), //((this->IsPvNode() && !bestMove.IsNone()) ? Bound::BoundExact : Bound::BoundUpper),
				depth,
				bestMove,
				(*ppFlashlight)->m_staticEval
			);
		}

		assert(-ScoreIndex::ScoreInfinite < bestScore && bestScore < ScoreIndex::ScoreInfinite);
	}

	// スタティック・メソッドは継承できないので、スタティックにはしないぜ☆（＾ｑ＾）
	virtual inline Bound GetBoundAtStep20(bool bestMoveExists) const = 0;


};


