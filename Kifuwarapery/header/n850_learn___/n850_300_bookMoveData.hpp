#pragma once


#include <string>
#include <vector>
#include "../n165_movStack/n165_400_move.hpp"


//────────────────────────────────────────────────────────────────────────────────────────────────────
// 以下のようなフォーマットが入力される。
// <棋譜番号> <日付> <先手名> <後手名> <0:引き分け, 1:先手勝ち, 2:後手勝ち> <総手数> <棋戦名前> <戦形>
// <CSA1行形式の指し手>
//
// (例)
// 1 2003/09/08 羽生善治 谷川浩司 2 126 王位戦 その他の戦型
// 7776FU3334FU2726FU4132KI
//────────────────────────────────────────────────────────────────────────────────────────────────────
struct BookMoveData {
	std::string player; // その手を指した人
	std::string date; // 対局日
	std::vector<Move> pvBuffer; // 正解のPV, その他0のPV, その他1のPV という順に並べる。
								// 間には MoveNone で区切りを入れる。

	Move move; // 指し手符号
	bool winner; // 勝ったかどうか
	bool useLearning; // 学習に使うかどうか
	bool otherPVExist; // 棋譜の手と近い点数の手があったか。useLearning == true のときだけ有効な値が入る
};
