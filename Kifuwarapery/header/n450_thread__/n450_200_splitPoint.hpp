﻿#pragma once


#include "../n220_position/n220_600_position.hpp"
#include "../n320_searcher/n320_125_searchStack.hpp"
#include "../n450_thread__/n450_100_nodeType.hpp"


class MovePicker;	//#include "../n320_searcher/n320_540_movePicker.hpp"
struct Thread;


struct SplitPoint {
	// 局面か☆
	const Position*		m_position;

	const SearchStack*	m_searchStack;

	Thread*				m_masterThread;

	Depth				m_depth;

	Score				m_beta;

	NodeType			m_nodeType;

	Move				m_threatMove;

	bool				m_cutNode;

	MovePicker*			m_pMovePicker;

	SplitPoint*			m_pParentSplitPoint;

	Mutex				m_mutex;

	volatile u64		m_slavesMask;

	volatile s64		m_nodes;

	volatile Score		m_alpha;

	volatile Score		m_bestScore;

	volatile Move		m_bestMove;

	volatile int		m_moveCount;

	volatile bool		m_cutoff;
};
