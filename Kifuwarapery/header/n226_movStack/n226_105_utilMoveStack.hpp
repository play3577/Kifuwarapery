#pragma once

#include <algorithm> //std::swap std::max_element
//#include <utility>
#include "../n223_move____/n223_105_utilMove.hpp"
#include "../n226_movStack/n226_100_moveStack.hpp"

class UtilMoveStack {
public:
	// �ėp�I�� insertion sort. �v�f�������Ȃ����A�����Ƀ\�[�g�ł���B
	// �~��(�傫�����̂��擪�t�߂ɏW�܂�)
	// *(first - 1) �� �ԕ�(sentinel) �Ƃ��� MAX �l�������Ă���Ɖ��肵�č��������Ă���B
	// T �ɂ� �|�C���^���C�e���[�^���g�p�o����B
	template <typename T, bool UseSentinel = false>
	static inline void InsertionSort(T first, T last) {
		if (UseSentinel) {
			assert(std::max_element(first - 1, last) == first - 1); // �ԕ����ő�l�ƂȂ邱�Ƃ��m�F
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

	// �ł� score �̍��� moveStack �̃|�C���^��Ԃ��B
	// MoveStack �̐��������Ƃ��Ȃ莞�Ԃ�������̂ŁA
	// ��ł����܂ނƂ��Ɏg�p���Ă͂Ȃ�Ȃ��B
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
