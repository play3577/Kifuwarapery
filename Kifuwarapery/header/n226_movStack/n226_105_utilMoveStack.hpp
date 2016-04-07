﻿#pragma once

#include <algorithm> //std::swap std::max_element
#include "../n223_move____/n223_105_utilMove.hpp"
#include "../n226_movStack/n226_100_moveStack.hpp"

class UtilMoveStack {
public:
	// 汎用的な insertion sort. 要素数が少ない時、高速にソートできる。
	// 降順(大きいものが先頭付近に集まる)
	// *(first - 1) に 番兵(sentinel) として MAX 値が入っていると仮定して高速化してある。
	// T には ポインタかイテレータを使用出来る。
	template <typename T, bool UseSentinel = false>
	static inline void InsertionSort(T first, T last) {
		if (UseSentinel) {
			assert(std::max_element(first - 1, last) == first - 1); // 番兵が最大値となることを確認
		}
		if (first != last) {
			for (T curr = first + 1; curr != last; ++curr) {
				if (*(curr - 1) < *curr) {
					const auto tmp = std::move(*curr);
					do {
						*curr = *(curr - 1);
						--curr;
					} while ((UseSentinel || curr != first)
						&& *(curr - 1) < tmp);
					*curr = std::move(tmp);
				}
			}
		}
	}

	// 最も score の高い moveStack のポインタを返す。
	// MoveStack の数が多いとかなり時間がかかるので、
	// 駒打ちを含むときに使用してはならない。
	static inline MoveStack* PickBest(MoveStack* currMove, MoveStack* lastMove) {
		std::swap(*currMove, *std::max_element(currMove, lastMove));
		return currMove;
	}

	static inline Move Move16toMove(const Move move, const Position& pos) {
		if (move.IsNone()) {
			return Move::GetMoveNone();
		}
		if (move.IsDrop()) {
			return move;
		}
		const Square from = move.From();
		const PieceType ptFrom = UtilPiece::ToPieceType(pos.GetPiece(from));
		return move | UtilMove::GetPieceType2Move(ptFrom) | UtilMove::GetCapturedPieceType2Move(move.To(), pos);
	}


};

