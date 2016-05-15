#pragma once

#include "../n080_common__/n080_150_overloadEnumOperators.hpp"


// 指し手生成フェーズ☆？（＾ｑ＾）320_540_ＭｏｖｅＰｉｃｋｅｒとかで使う☆
// このフェーズは順番に実行されるぜ☆（＾ｑ＾）
enum GenerateMovePhase {
	// Nフェーズ番号(start/end)_名前

	//────────────────────────────────────────────────────────────────────────────────
	// メイン探索だぜ☆（＾ｑ＾）
	//────────────────────────────────────────────────────────────────────────────────
	N00_Start_UNR_MainSearch,			// （＋＋でｔｔ）トランスポジション・テーブル・ムーブを見るだけ☆
	N01_______N03_TacticalMoves0,		// 駒を取る手
	N02_______P2__Killers,
	N03_______N04_NonTacticalMoves0,	// 駒を取らない手
	N04_______LNC_NonTacticalMoves1,
	N05_______EBC_BadCaptures,
	// フェーズ終わり☆

	//────────────────────────────────────────────────────────────────────────────────
	// 王手回避探索だぜ☆（＾ｑ＾）
	//────────────────────────────────────────────────────────────────────────────────
	N06_Start_P1__EvasionSearch,		// （＋＋でｔｔ）トランスポジション・テーブル・ムーブを見るだけ☆
	N07_______N06_Evasions,				// 王手回避の手
	// フェーズ終わり☆

	//────────────────────────────────────────────────────────────────────────────────
	// 静止探索だぜ☆（＾ｑ＾）
	//────────────────────────────────────────────────────────────────────────────────
	// たたき合いを読む予定の深さを超えたときは、ここからかだぜ☆？（＾ｑ＾）？
	// たたき合いは、読みを止めて　現局面で計算するぜ
	N08_Start_P1__QSearch,				// （＋＋でｔｔ）（Ｑ静止探索）トランスポジション・テーブル・ムーブを見るだけ☆
	N09_______N03_QCaptures0,			//              （Ｑ静止探索）駒を取る手
	// フェーズ終わり☆

	//────────────────────────────────────────────────────────────────────────────────
	// 静止探索の王手回避探索だぜ☆（＾ｑ＾）
	//────────────────────────────────────────────────────────────────────────────────
	// 王手されていることが分かったときは、ここからかだぜ☆？（＾ｑ＾）？
	N10_Start_P1__QEvasionSearch,		// （＋＋でｔｔ）（Ｑ静止探索）トランスポジション・テーブル・ムーブを見るだけ☆
	N11_______N06_QEvasions,			//             （Ｑ静止探索）王手回避の手

	//────────────────────────────────────────────────────────────────────────────────
	// どこからスタートだぜ☆？（＾ｑ＾）
	//────────────────────────────────────────────────────────────────────────────────
	N12_Start_P1__ProbCut,				// （＋＋でｔｔ）トランスポジション・テーブル・ムーブを見るだけ☆
	N13_______N03_TacticalMoves1,		// 駒を取る手
	// フェーズ終わり☆

	//────────────────────────────────────────────────────────────────────────────────
	// 静止探索のリキャプチャーだぜ☆（＾ｑ＾）
	//────────────────────────────────────────────────────────────────────────────────
	// 王手もされていないし、たたき合いを読む予定の深さも超えていない　ときは、ここからかだぜ☆？（＾ｑ＾）？
	N14_Start_P1__QRecapture,			// （Ｑ静止探索）初期化で使われることもあるぜ☆
	N15_______REC_QCaptures1,			// （Ｑ静止探索）駒を取り返す手☆
	// フェーズ終わり☆

	//────────────────────────────────────────────────────────────────────────────────
	// 終わりだぜ☆（＾ｑ＾）
	//────────────────────────────────────────────────────────────────────────────────
	N16_xxxxx_P1__Stop					//              番兵☆（＾ｑ＾）処理を止める☆
};
OverloadEnumOperators(GenerateMovePhase); // ++phase_ の為。
static const int g_MOVEGENERATOR_PHASE_NUM = 17;