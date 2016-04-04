#pragma once

#include "../../header/n220_position/n220_500_charToPieceUSI.hpp"
#include "../../header/n223_move____/n223_105_utilMove.hpp"
#include "../../header/n320_searcher/n320_150_search.hpp"

enum GenerateMovePhase {
	MainSearch, PH_TacticalMoves0, PH_Killers, PH_NonTacticalMoves0, PH_NonTacticalMoves1, PH_BadCaptures,
	EvasionSearch, PH_Evasions,
	QSearch, PH_QCaptures0,
	QEvasionSearch, PH_QEvasions,
	ProbCut, PH_TacticalMoves1,
	QRecapture, PH_QCaptures1,
	PH_Stop
};
OverloadEnumOperators(GenerateMovePhase); // ++phase_ の為。

class MovePicker {
public:
	MovePicker(const Position& pos, const Move ttm, const Depth depth,
			   const History& history, SearchStack* searchStack, const Score beta);
	MovePicker(const Position& pos, Move ttm, const Depth depth, const History& history, const Square sq);
	MovePicker(const Position& pos, const Move ttm, const History& history, const PieceType pt);

	template <bool SPNODE>
	Move GetNextMove();

private:

	void ScoreCaptures();

	template <bool IsDrop>
	void ScoreNonCapturesMinusPro();

	void ScoreEvasions();

	void GoNextPhase();

	MoveStack* GetFirstMove() { return &m_legalMoves_[1]; } // [0] は番兵

	MoveStack* GetCurrMove() const { return m_currMove_; }

	MoveStack* GetLastMove() const { return m_lastMove_; }

	MoveStack* GetLastNonCapture() const { return m_lastNonCapture_; }

	MoveStack* GetEndBadCaptures() const { return m_endBadCaptures_; }

	const Position& GetPos() const { return m_pos_; }

	GenerateMovePhase GetPhase() const { return m_phase_; }

	const History& GetHistory() const { return m_history_; }


	const Position&		m_pos_;
	const History&		m_history_;
	SearchStack*		m_ss_;
	Depth				m_depth_;
	Move				m_ttMove_; // transposition table move
	MoveStack			m_killerMoves_[2];
	Square				m_recaptureSquare_;
	int					m_captureThreshold_; // int で良いのか？
	GenerateMovePhase	m_phase_;
	MoveStack*			m_currMove_;
	MoveStack*			m_lastMove_;
	MoveStack*			m_lastNonCapture_;
	MoveStack*			m_endBadCaptures_;
	// std::array にした方が良さそう。
	MoveStack			m_legalMoves_[g_MaxLegalMoves];
};

