﻿#pragma once

#include "../n080_common__/n080_100_common.hpp"	//u32
#include "../n105_color___/n105_100_color.hpp"	//ColorNum
#include "../n119_score___/n119_090_score.hpp"	//Ply

// 元の名前：ＬｉｍｉｔｓＴｙｐｅ
// 時間や探索深さの制限を格納する為の構造体
struct LimitsOfThinking {

	LimitsOfThinking() { memset(this, 0, sizeof(LimitsOfThinking)); }

	bool IsUseTimeManagement() const {
		return !(m_depth | m_nodes | m_moveTime | static_cast<int>(m_infinite));
	}

	int m_time[ColorNum];
	int m_increment[ColorNum];

	// どこで設定しているのか☆（＾ｑ＾）
	int m_movesToGo;

	Ply m_depth;
	u32 m_nodes;
	int m_moveTime;
	bool m_infinite;
	bool m_ponder;
};