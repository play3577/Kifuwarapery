﻿#include <algorithm> // std::min
#include "../../header/n119_score___/n119_090_scoreIndex.hpp"
#include "../../header/n560_timeMng_/n560_100_limitsOfThinking.hpp"
#include "../../header/n560_timeMng_/n560_500_timeManager.hpp"
#include "../../header/n885_searcher/n885_040_rucksack.hpp"


namespace {
#if 1
	//────────────────────────────────────────────────────────────────────────────────
	// MaxRatio …… 1.0 より大きくする感じ☆？ 残り時間に何か影響する☆？
	//
	//────────────────────────────────────────────────────────────────────────────────
	// フィッシャールール 10分 加算10秒用にするには☆？（＾ｑ＾）
	const int g_MOVE_HORIZON = 47;
	const float g_MAX_RATIO = 800.0f; // 試しに１６０倍にするとどうなるんだぜ☆？（＾ｑ＾）？
								   // 1手に 17秒ぐらいかけてくれて、序盤に長考してくれていい感じ☆（＾ｑ＾）
								   //────────────────────────────────────────────────────────────────────────────────
#elif 9
	// フィッシャールール 10分 加算10秒用にするには☆？（＾ｑ＾）
	const int g_MOVE_HORIZON = 47;
	const float g_MAX_RATIO = 8000.0f; // 試しに１６００倍にするとどうなるんだぜ☆？（＾ｑ＾）？
									// 40手目まで自分の時間をあんまり使わなくなってしまった印象だぜ☆（＾ｑ＾） 800.f の方がマシかも☆
									//────────────────────────────────────────────────────────────────────────────────
#elif 8
	// フィッシャールール 10分 加算10秒用にするには☆？（＾ｑ＾）
	const int g_MOVE_HORIZON = 47;
	const float g_MAX_RATIO = 80.0f; // 試しに１６倍にするとどうなるんだぜ☆？（＾ｑ＾）？
	// 1手に 13秒ぐらいかけて考えてくれるようになったぜ☆（＾ｑ＾）
	// いい感じ☆（＾＿＾）
#elif 7
	//────────────────────────────────────────────────────────────────────────────────
	// フィッシャールール 10分 加算10秒用にするには☆？（＾ｑ＾）
	const int g_MOVE_HORIZON = 47;
	const float g_MAX_RATIO = 40.0f; // 試しに８倍にするとどうなるんだぜ☆？（＾ｑ＾）？
	// 10秒も考えずに　7秒ぐらいで　ぽんぽん指している感じだったぜ☆
#elif 6
	//────────────────────────────────────────────────────────────────────────────────
	// フィッシャールール 10分 加算10秒用にするには☆？（＾ｑ＾）
	const int g_MOVE_HORIZON = 47;
	const float g_MAX_RATIO = 20.0f; // 試しに４倍にするとどうなるんだぜ☆？（＾ｑ＾）？
	// 通常で１分読む感じ☆　最後に３分使い切らずに余してしまう。
#elif 5
	//────────────────────────────────────────────────────────────────────────────────
	// フィッシャールール 10分 加算10秒用にするには☆？（＾ｑ＾）
	const int g_MOVE_HORIZON = 47;
	const float g_MAX_RATIO = 10.0f; // 試しに２倍にするとどうなるんだぜ☆？（＾ｑ＾）？
	// 普段 20秒ぐらいで指す感じ☆　長考で 1分、相手の思考時間も合わせての大長考で 4分ぐらい☆
	// 投了時に 3分ぐらい残ってしまう感じ☆
#elif 4
	//────────────────────────────────────────────────────────────────────────────────
	// フィッシャールール 10分 加算10秒用にするには☆？（＾ｑ＾）
	const int g_MOVE_HORIZON = 47;
	const float g_MAX_RATIO = 5.0f; // 秒は使っていいかもだぜ☆（＾ｑ＾）
#elif 3
	//────────────────────────────────────────────────────────────────────────────────
	// 10分 秒読み10秒用にはちょうど良かったぜ☆（＾ｑ＾）
	const int g_MOVE_HORIZON = 47;
	const float g_MAX_RATIO = 2.5; // 2.0 は減らしすぎたかだぜ☆（＾ｑ＾）
#elif 2
	//────────────────────────────────────────────────────────────────────────────────
	// 10分 秒読み10秒用にするには☆？（＾ｑ＾）
	const int g_MOVE_HORIZON = 47;
	const float g_MAX_RATIO = 2.0; // 適当に減らすぜ☆（＾ｑ＾）→30秒ぐらいかけて 36手まで読むようになったぜ☆
								// 後半まで時間を残すのもいい感じだぜ☆（＾ｑ＾）
								// 序盤、16秒ぐらいしか考えなくなったので序盤に悪くしてしまうぜ☆（＾ｑ＾）
	//────────────────────────────────────────────────────────────────────────────────
	// 15分切れ負け用。
	// (^q^)10分だと22秒ぐらいかけて 28～31手先まで読んでる感じ☆？
	//const int MoveHorizon = 47;
	//const float MaxRatio = 3.0;
	//────────────────────────────────────────────────────────────────────────────────
	//const float MaxRatio = 5.0; // 15分 秒読み10秒用。
	//────────────────────────────────────────────────────────────────────────────────
#else
	const int g_MOVE_HORIZON = 35; // 2時間切れ負け用。(todo: もう少し時間使っても良いかも知れない。)
	const float g_MAX_RATIO = 5.0; // 2時間切れ負け用。
#endif
	// スティール率とは何なのか☆？（＾ｑ＾）？
	const float g_STEAL_RATIO = 0.33;

	// 旧名：ＭｏｖｅＩｍｐｏｒｔａｎｃｅ
	// Stockfish とは異なる。
	// 手目毎の時間配分の重み☆ 予定思考時間、最大延長時間の両方で使う☆（＾ｑ＾）
	// 大会は256手目までだぜ☆（＾ｑ＾）読みの配列インデックス・オーバーも考慮して、256手目の先も２倍分用意しておくぜ☆（＾ｑ＾）
	// 全部 0 にすると、おそらく残り時間＝予定思考時間＝最大延長時間　になるぜ☆（＾ｑ＾）
	const int g_SYOHI_OMOMI_BY_TEME_NUM = 512;
	const int g_SYOHI_OMOMI_BY_TEME[g_SYOHI_OMOMI_BY_TEME_NUM] = {
		//
		// 逆にしてみたぜ☆（＾ｑ＾）１手目付近が小さく、２５６手目付近が大きく☆
		// これで　１手目付近は　１７秒ぐらい使っているぜ☆　逆にすると１手目付近から３０秒ぐらい使ってしまってタイヘンだったぜ☆（＾ｑ＾）
		//
		//────────────────────────────────────────────────────────────────────────────────
		// 1～32手目の重み☆ 4/3 ぐらい上昇☆
		//────────────────────────────────────────────────────────────────────────────────
		800,  800,  800,  800,  800,  800,  800,  800,  800,  800,  800,  800,  800,  800,  800,  800, // 16:
		933,  933,  933,  933,  933,  933,  933,  933,  933,  933,  933,  933,  933,  933,  933,  933, // 32: 32 手目まで定跡で 思考 10数秒 ということはよくある☆
																									   //────────────────────────────────────────────────────────────────────────────────
																									   // 33～64手目の重み☆ 4/3 ぐらい上昇☆
																									   //────────────────────────────────────────────────────────────────────────────────
		1067, 1067, 1067, 1067, 1067, 1067, 1067, 1067, 1067, 1067, 1067, 1067, 1067, 1067, 1067, 1067, // 48:
		1425, 1425, 1425, 1425, 1425, 1425, 1425, 1425, 1425, 1425, 1425, 1425, 1425, 1425, 1425, 1425,	// 64:
																										//────────────────────────────────────────────────────────────────────────────────
																										// 80～96手目の重み☆ 4/3 ぐらい上昇☆
																										//────────────────────────────────────────────────────────────────────────────────
		1423, 1423, 1423, 1423, 1423, 1423, 1423, 1423, 1423, 1423, 1423, 1423, 1423, 1423, 1423, 1423,	// 80:
		1660, 1660, 1660, 1660, 1660, 1660, 1660, 1660, 1660, 1660, 1660, 1660, 1660, 1660, 1660, 1660,	// 96:
																										//────────────────────────────────────────────────────────────────────────────────
																										// 97～128手目の重み☆ 4/3 ぐらい上昇☆
																										//────────────────────────────────────────────────────────────────────────────────
		1898, 1898, 1898, 1898, 1898, 1898, 1898, 1898, 1898, 1898, 1898, 1898, 1898, 1898, 1898, 1898,	// 112:
		2214, 2214, 2214, 2214, 2214, 2214, 2214, 2214, 2214, 2214, 2214, 2214, 2214, 2214, 2214, 2214, // 128:
																										//────────────────────────────────────────────────────────────────────────────────
																										// 129～160手目の重み☆ 4/3 ぐらい上昇☆
																										//────────────────────────────────────────────────────────────────────────────────
		2531, 2531, 2531, 2531, 2531, 2531, 2531, 2531, 2531, 2531, 2531, 2531, 2531, 2531, 2531, 2531, // 144:
		2953, 2953, 2953, 2953, 2953, 2953, 2953, 2953, 2953, 2953, 2953, 2953, 2953, 2953, 2953, 2953,	// 160:
																										//────────────────────────────────────────────────────────────────────────────────
																										// 161～192手目の重み☆ 4/3 ぐらい上昇☆
																										//────────────────────────────────────────────────────────────────────────────────
		3375, 3375, 3375, 3375, 3375, 3375, 3375, 3375, 3375, 3375, 3375, 3375, 3375, 3375, 3375, 3375,	// 176:
		3937, 3937, 3937, 3937, 3937, 3937, 3937, 3937, 3937, 3937, 3937, 3937, 3937, 3937, 3937, 3937,	// 192:
																										//────────────────────────────────────────────────────────────────────────────────
																										// 193～224手目の重み☆ 4/3 ぐらい上昇☆
																										//────────────────────────────────────────────────────────────────────────────────
		4500, 4500, 4500, 4500, 4500, 4500, 4500, 4500, 4500, 4500, 4500, 4500, 4500, 4500, 4500, 4500,	// 208:
		5250, 5250, 5250, 5250, 5250, 5250, 5250, 5250, 5250, 5250, 5250, 5250, 5250, 5250, 5250, 5250,	// 224:
																										//────────────────────────────────────────────────────────────────────────────────
																										// 193～224手目の重み☆ 4/3 ぐらい上昇☆
																										//────────────────────────────────────────────────────────────────────────────────
		6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000,	// 240:
		7000, 7000, 7000, 7000, 7000, 7000, 7000, 7000, 7000, 7000, 7000, 7000, 7000, 7000, 7000, 7000, // 256: ここで256手目☆（＾ｑ＾）ここまでを使う☆
																										//────────────────────────────────────────────────────────────────────────────────
																										// 256手目以降は、現在＋256手先まで読む設定に対応するための、累計に影響を与えないダミーの 0 値☆
																										//────────────────────────────────────────────────────────────────────────────────
																										//────────────────────────────────────────────────────────────────────────────────
																										// 256～288手目の重み☆ 4/3 ぐらい上昇☆
																										//────────────────────────────────────────────────────────────────────────────────
		8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, // 272:
		8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000,	// 288;

																										//────────────────────────────────────────────────────────────────────────────────
																										// 以降手抜き☆
																										//────────────────────────────────────────────────────────────────────────────────
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,	// ( 3);
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,	// ( 4);
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,	// ( 5);
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,	// ( 6);
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,	// ( 7);
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,	// ( 8);
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,	// ( 9);
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,	// (10);
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,	// (11);
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,	// (12);
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,	// (13);
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,	// (14);
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,	// (15);
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 	// (16);
	};

	// 旧名：ｇｅｔＭｏｖｅＩｍｐｏｒｔａｎｃｅ
	int G_GetSyohiOmomiByTeme(const Ply ply) {
		// ply は多くても 511 まで☆（配列のリミットチェック）（＾ｑ＾）
		return g_SYOHI_OMOMI_BY_TEME[std::min(ply, g_SYOHI_OMOMI_BY_TEME_NUM-1)];//511
	}

	// 旧名：ｒｅｍａｉｎｉｎｇ
	// 次回の残り時間☆
	inline float G_TimeBairitu_YoteiSiko(
		const int movesToGo,	//ムーブ・ホライズン以下の値☆（＾ｑ＾）
		const Ply currentPly,
		const int slowMover
	) {
		const float tMaxRatio   = 1; // 予定思考時間なら 1、最大延長時間なら 定数指定☆
		const float tStealRatio = 0; // 予定思考時間なら 0、最大延長時間なら 定数指定☆

		// 旧名：ｔｈｉｓＭｏｖｅＩｍｐｏｒｔａｎｃｅ
		// 現在の手の、ムーブ・インポータンス☆（＾ｑ＾）
		const float currentOmomi = G_GetSyohiOmomiByTeme(currentPly) * slowMover / 100;
		// スロー・ムーバーの設定が 100 のときは、moveImportance そのまんまの値になるぜ☆（＾ｑ＾）

		// 旧名：ｏｔｈｅｒＭｏｖｅＩｍｐｏｒｔａｎｃｅ
		// 残りの手の、ムーブ・インポータンスの合計☆（＾ｑ＾）
		float nokoriOmomiRuikei = 0;

		for (int i = 1; i < movesToGo; ++i) {
			// 現在の手数から、白黒手番の数×ループ回数分の moveImportance を足しこみだぜ☆（＾ｑ＾）
			// 自分の手番だけを足したいので、1つ飛ばしだぜ☆（＾ｑ＾）
			nokoriOmomiRuikei += G_GetSyohiOmomiByTeme(currentPly + 2 * i);
		}

		// ２種類の比率を調べて、小さい方を取るぜ☆（＾ｑ＾）
		const float ratio1 =
			(tMaxRatio * currentOmomi)
			/
			// １／１の分母に　残り重み累計　を加算した形☆
			static_cast<float>(tMaxRatio * currentOmomi + nokoriOmomiRuikei);

		const float ratio2 =
			// 分子の重みに スティール比率 を掛けた形☆
			(tStealRatio * nokoriOmomiRuikei + currentOmomi)
			/
			static_cast<float>(nokoriOmomiRuikei + currentOmomi);

		return std::min(ratio1, ratio2);
	}

	inline float G_TimeBairitu_SaidaiEncho(
		const int movesToGo,	//ムーブ・ホライズン以下の値☆（＾ｑ＾）
		const Ply currentPly,
		const int slowMover
		) {
		const float tMaxRatio = g_MAX_RATIO; // 予定思考時間なら 1、最大延長時間なら 定数指定☆
		const float tStealRatio = g_STEAL_RATIO; // 予定思考時間なら 0、最大延長時間なら 定数指定☆

																		  // 旧名：ｔｈｉｓＭｏｖｅＩｍｐｏｒｔａｎｃｅ
																		  // 現在の手の、ムーブ・インポータンス☆（＾ｑ＾）
		const float currentOmomi = G_GetSyohiOmomiByTeme(currentPly) * slowMover / 100;
		// スロー・ムーバーの設定が 100 のときは、moveImportance そのまんまの値になるぜ☆（＾ｑ＾）

		// 旧名：ｏｔｈｅｒＭｏｖｅＩｍｐｏｒｔａｎｃｅ
		// 残りの手の、ムーブ・インポータンスの合計☆（＾ｑ＾）
		float nokoriOmomiRuikei = 0;

		for (int i = 1; i < movesToGo; ++i) {
			// 現在の手数から、白黒手番の数×ループ回数分の moveImportance を足しこみだぜ☆（＾ｑ＾）
			// 自分の手番だけを足したいので、1つ飛ばしだぜ☆（＾ｑ＾）
			nokoriOmomiRuikei += G_GetSyohiOmomiByTeme(currentPly + 2 * i);
		}

		// ２種類の比率を調べて、小さい方を取るぜ☆（＾ｑ＾）
		const float ratio1 =
			(tMaxRatio * currentOmomi)
			/
			// １／１の分母に　残り重み累計　を加算した形☆
			static_cast<float>(tMaxRatio * currentOmomi + nokoriOmomiRuikei);

		const float ratio2 =
			// 分子の重みに スティール比率 を掛けた形☆
			(tStealRatio * nokoriOmomiRuikei + currentOmomi)
			/
			static_cast<float>(nokoriOmomiRuikei + currentOmomi);

		return std::min(ratio1, ratio2);
	}
}



// 旧名：ＳｅｔＰｖＩｎｓｔａｂｉｌｉｔｙ
void TimeManager::SetPvInstability(
	const int currChanges,
	const int prevChanges
) {
	this->SetSikoAsobiTime(
		currChanges * (this->GetYoteiSikoTime() / 2)
		+
		prevChanges * (this->GetYoteiSikoTime() / 3)
		);
}




void TimeManager::InitializeTimeManager_OnHitchhikerThinkStarted(
	bool& isMoveTime0Clear,// false を入れてくれ☆（＾ｑ＾）
	const LimitsOfThinking& limits, // m_moveTimeを 0にする場合があるぜ☆（＾ｑ＾）
	const Ply currentPly,
	const Color us,
	Rucksack* pRucksack
	) {

	const int emergencyMoveHorizon = pRucksack->m_engineOptions["Emergency_Move_Horizon"];
	//const int emergencyBaseTime    = pRucksack->m_engineOptions["Emergency_Base_Time"];	// 緊急時用に残しておこうというタイム（ミリ秒）か☆？
	//const int emergencyMoveTime    = pRucksack->m_engineOptions["Emergency_Move_Time"];	// 緊急時用に残しておこうというタイム（ミリ秒）か☆？
	const int minThinkingTime      = pRucksack->m_engineOptions["Minimum_Thinking_Time"];	// １手につき、最低限の思考時間☆
    const int slowMover            = pRucksack->m_engineOptions["Slow_Mover"];

	this->ZeroClearSikoAsobiTime();
	this->SetYoteiSikoTime( limits.GetNokoriTime(us));// 予定思考時間は、残り時間をそのまんま入れて初期化☆？（＾ｑ＾）？
	this->SetSaidaiEnchoTime( limits.GetNokoriTime(us));// 最大延長時間も☆？（＾ｑ＾）？

	for (
		// ムーブス・ツー・ゴー
		int iHypMtg = 1; // ループ・カウンター☆
		iHypMtg <= g_MOVE_HORIZON;
		++iHypMtg
	) {
		// 元の名前：ｈｙｐＭｙＴｉｍｅ
		// マイ・タイム☆　叩き台となる時間だぜ☆（＾ｑ＾）
		int tatakidaiTime =
			limits.GetNokoriTime(us)
			+ limits.GetIncrement(us) * (iHypMtg - 1)	// 今後追加されるインクリメントの累計☆
			//- emergencyBaseTime	// 緊急時用に残しておこうというタイム（ミリ秒）か☆？
			//- emergencyMoveTime	// 緊急時用に残しておこうというタイム（ミリ秒）か☆？
			//+ std::min(iHypMtg, emergencyMoveHorizon) // 1～255 ミリ秒を加算してどうするのか☆？（＾ｑ＾）
			;

		tatakidaiTime = std::max(tatakidaiTime, 0); // 0以上を確保するぜ☆（＾ｑ＾）

		// 思考予定タイムが、少なくなっていれば更新します。
		this->SmallUpdate_YoteiSikoTime(
			static_cast<int>(tatakidaiTime * G_TimeBairitu_YoteiSiko(iHypMtg, currentPly, slowMover)) + minThinkingTime// 残り時間 × 倍率 + 最低思考時間
			); 

		// 最大延長タイムが、少なくなっていれば更新します。
		this->SmallUpdate_SaidaiEnchoTime(
			static_cast<int>(tatakidaiTime * G_TimeBairitu_SaidaiEncho(iHypMtg, currentPly, slowMover)) + minThinkingTime// 残り時間 × 倍率 + 最低思考時間
		);
	}

	if (pRucksack->m_engineOptions["USI_Ponder"]) {
		// ポンダーを使う設定☆？（＾ｑ＾）？
		// 思考予定タイムには、 4分の1　のボーナスを追加します。
		this->IncreaseYoteiSikoTime( this->GetYoteiSikoTime() / 4);
	}

	// 大きくなっていれば更新します。
	this->LargeUpdate_YoteiSikoTime( minThinkingTime);
	// 小さくなっていれば更新します。
	this->SmallUpdate_YoteiSikoTime( this->GetSaidaiEnchoTime() );

	if (limits.GetMoveTime() != 0) {//（＾ｑ＾）いつも　０　な気がするぜ☆
		// こんなとこ、実行されないんじゃないかだぜ☆？（＾ｑ＾）？
		if (this->GetYoteiSikoTime() < limits.GetMoveTime()) {
			this->SetYoteiSikoTime( std::min(limits.GetNokoriTime(us), limits.GetMoveTime()));
		}
		if (this->GetSaidaiEnchoTime() < limits.GetMoveTime()) {
			this->SetSaidaiEnchoTime( std::min(limits.GetNokoriTime(us), limits.GetMoveTime()) );
		}
		this->IncreaseYoteiSikoTime( limits.GetMoveTime());
		this->IncreaseSaidaiEnchoTime( limits.GetMoveTime());
		if (limits.GetNokoriTime(us) != 0) {
			isMoveTime0Clear = true;
		}
	}
	//旧表示：optimum_search_time
	//旧表示：maximum_search_time	
	SYNCCOUT << "info string limits inc time " << limits.GetIncrement(us) << SYNCENDL;
	SYNCCOUT << "info string tukatteii time " << this->GetTukatteiiTime() << " ( yotei " << this->GetYoteiSikoTime() << " + asobi " << this->GetSikoAsobiTime() << ") / saidai encho " << this->GetSaidaiEnchoTime() << SYNCENDL;
}
