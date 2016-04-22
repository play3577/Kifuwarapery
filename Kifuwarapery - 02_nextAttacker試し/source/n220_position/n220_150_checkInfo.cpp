﻿#include "../../header/n105_color___/n105_100_color.hpp"
#include "../../header/n105_color___/n105_500_utilColor.hpp"
#include "../../header/n110_square__/n110_100_square.hpp"
#include "../../header/n112_pieceTyp/n112_050_pieceType.hpp"
#include "../../header/n160_board___/n160_100_bitboard.hpp"
#include "../../header/n220_position/n220_150_checkInfo.hpp"
#include "../../header/n220_position/n220_650_position.hpp"
#include "../../header/n220_position/n220_670_makePromoteMove.hpp"
#include "../../header/n350_pieceTyp/n350_040_ptEvent.hpp"
#include "../../header/n350_pieceTyp/n350_110_ptPawn.hpp"
#include "../../header/n350_pieceTyp/n350_120_ptLance.hpp"
#include "../../header/n350_pieceTyp/n350_130_ptKnight.hpp"
#include "../../header/n350_pieceTyp/n350_140_ptSilver.hpp"
#include "../../header/n350_pieceTyp/n350_150_ptBishop.hpp"
#include "../../header/n350_pieceTyp/n350_160_ptRook.hpp"
#include "../../header/n350_pieceTyp/n350_170_ptGold.hpp"
#include "../../header/n350_pieceTyp/n350_180_ptKing.hpp"
#include "../../header/n350_pieceTyp/n350_500_ptArray.hpp"

CheckInfo::CheckInfo(const Position& position) {
	const Color them = UtilColor::OppositeColor(position.GetTurn());
	const Square ksq = position.GetKingSquare(them);

	m_pinned = position.GetPinnedBB();
	m_dcBB = position.DiscoveredCheckBB();

	const PieceTypeEvent ptEvent1(position.GetOccupiedBB(), them, ksq);

	m_checkBB[N01_Pawn] = PieceTypeArray::m_pawn.GetAttacks2From(ptEvent1);
	m_checkBB[N02_Lance] = PieceTypeArray::m_lance.GetAttacks2From(ptEvent1);
	m_checkBB[N03_Knight] = PieceTypeArray::m_knight.GetAttacks2From(ptEvent1);
	m_checkBB[N04_Silver] = PieceTypeArray::m_silver.GetAttacks2From(ptEvent1);
	m_checkBB[N05_Bishop] = PieceTypeArray::m_bishop.GetAttacks2From(ptEvent1);
	m_checkBB[N06_Rook] = PieceTypeArray::m_rook.GetAttacks2From(ptEvent1);
	m_checkBB[N07_Gold] = PieceTypeArray::m_gold.GetAttacks2From(ptEvent1);
	m_checkBB[N08_King] = Bitboard::CreateAllZeroBB();
	// todo: ここで AVX2 使えそう。
	//       checkBB のreadアクセスは ｓｗｉｔｃｈ (pt) で場合分けして、余計なコピー減らした方が良いかも。
	m_checkBB[N09_ProPawn] = m_checkBB[N07_Gold];
	m_checkBB[N10_ProLance] = m_checkBB[N07_Gold];
	m_checkBB[N11_ProKnight] = m_checkBB[N07_Gold];
	m_checkBB[N12_ProSilver] = m_checkBB[N07_Gold];
	m_checkBB[N13_Horse] = m_checkBB[N05_Bishop] | PieceTypeArray::m_king.GetAttacks2From(ptEvent1);
	m_checkBB[N14_Dragon] = m_checkBB[N06_Rook] | PieceTypeArray::m_king.GetAttacks2From(ptEvent1);
}
