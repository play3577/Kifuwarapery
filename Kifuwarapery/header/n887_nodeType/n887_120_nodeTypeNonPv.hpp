﻿#pragma once


#include "../n220_position/n220_650_position.hpp"
#include "../n223_move____/n223_040_nodeType.hpp"
#include "../n223_move____/n223_500_searchStack.hpp"
#include "../n640_searcher/n640_440_splitPoint.hpp"
#include "../n883_nodeType/n883_500_nodeTypeAbstract.hpp"

#include "../n885_searcher/n885_500_searcher.hpp"
//class Searcher;


class NodeTypeNonPv : public NodeTypeAbstract {
public:

	static void GoSearch(Searcher& searcher, Position& pos, SearchStack* ss, SplitPoint& sp) {
		searcher.Search<NodeType::SplitPointNonPV>(pos, ss + 1, sp.m_alpha, sp.m_beta, sp.m_depth, sp.m_cutNode);
	}

};


extern NodeTypeNonPv g_nodeTypeNonPv;

