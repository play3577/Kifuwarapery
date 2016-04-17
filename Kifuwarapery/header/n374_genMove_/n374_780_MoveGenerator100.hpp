#pragma once


class MoveGenerator100 {
public:

	// �w���萶�� functor
	// �e���v���[�g��������������A�������ꉻ�����������̂ŁA�֐��ł͂Ȃ��Astruct �ɂ����B
	// ALL == true �̂Ƃ��A���A��A�p�̕s���A����2�i�ڂ̕s���A����3�i�ڂ̋�����Ȃ��s������������B
	template <MoveType MT, Color US, bool ALL = false>
	struct GenerateMoves {
		MoveStack* operator () (
			MoveStack* moveStackList, const Position& pos
			) {
			static_assert(MT == Capture || MT == NonCapture || MT == CapturePlusPro || MT == NonCaptureMinusPro, "");
			// Txxx �͐��A���̏����z�������ϐ��B�����͐��ɍ��킹�Ă���B
			const Rank TRank6 = (US == Black ? Rank6 : Rank4);
			const Rank TRank7 = (US == Black ? Rank7 : Rank3);
			const Rank TRank8 = (US == Black ? Rank8 : Rank2);
			const Bitboard TRank789BB = g_inFrontMaskBb.GetInFrontMask<US, TRank6>();
			const Bitboard TRank1_6BB = g_inFrontMaskBb.GetInFrontMask<UtilColor::OppositeColor(US), TRank7>();
			const Bitboard TRank1_7BB = g_inFrontMaskBb.GetInFrontMask<UtilColor::OppositeColor(US), TRank8>();

			const Bitboard targetPawn =
				(MT == Capture) ? pos.GetBbOf(UtilColor::OppositeColor(US)) :
				(MT == NonCapture) ? pos.GetEmptyBB() :
				(MT == CapturePlusPro) ? pos.GetBbOf(UtilColor::OppositeColor(US)) | (pos.GetOccupiedBB().NotThisAnd(TRank789BB)) :
				(MT == NonCaptureMinusPro) ? pos.GetOccupiedBB().NotThisAnd(TRank1_6BB) :
				Bitboard::CreateAllOneBB(); // error
			const Bitboard targetOther =
				(MT == Capture) ? pos.GetBbOf(UtilColor::OppositeColor(US)) :
				(MT == NonCapture) ? pos.GetEmptyBB() :
				(MT == CapturePlusPro) ? pos.GetBbOf(UtilColor::OppositeColor(US)) :
				(MT == NonCaptureMinusPro) ? pos.GetEmptyBB() :
				Bitboard::CreateAllOneBB(); // error
			const Square ksq = pos.GetKingSquare(UtilColor::OppositeColor(US));

			moveStackList = GeneratePieceMoves<MT, N01_Pawn, US, ALL>()(moveStackList, pos, targetPawn, ksq);
			moveStackList = GeneratePieceMoves<MT, N02_Lance, US, ALL>()(moveStackList, pos, targetOther, ksq);
			moveStackList = GeneratePieceMoves<MT, N03_Knight, US, ALL>()(moveStackList, pos, targetOther, ksq);
			moveStackList = GeneratePieceMoves<MT, N04_Silver, US, ALL>()(moveStackList, pos, targetOther, ksq);
			moveStackList = GeneratePieceMoves<MT, N05_Bishop, US, ALL>()(moveStackList, pos, targetOther, ksq);
			moveStackList = GeneratePieceMoves<MT, N06_Rook, US, ALL>()(moveStackList, pos, targetOther, ksq);
			moveStackList = GeneratePieceMoves<MT, N16_GoldHorseDragon, US, ALL>()(moveStackList, pos, targetOther, ksq);
			moveStackList = GeneratePieceMoves<MT, N08_King, US, ALL>()(moveStackList, pos, targetOther, ksq);

			return moveStackList;
		}
	};

	// �������ꉻ
	// ��ł�����
	template <Color US>
	struct GenerateMoves<Drop, US> {
		FORCE_INLINE MoveStack* operator () (
			MoveStack* pMovestack, const Position& pos
			) {
			const Bitboard target = pos.GetEmptyBB();
			pMovestack = DropMoveGenerator::GenerateDropMoves(US, pMovestack, pos, target);//<US>
			return pMovestack;
		}
	};


	// �������ꉻ
	// �����𐶐�
	// ��������Ă����ɂ�鉤��͔����邪�A
	// �ʂ̈ړ���ɓG�̗���������ꍇ�ƁApin����Ă��閡���̋�𓮂������ꍇ�A�񍇖@��𐶐�����B
	// ���̂��߁Apseudo legal �ł���B
	template <Color US, bool ALL>
	struct GenerateMoves<Evasion, US, ALL> {
		/*FORCE_INLINE*/ MoveStack* operator () (
			MoveStack* pMovestack,
			const Position& pos
			) {
			assert(pos.IsOK());
			assert(pos.InCheck());

			const Square ksq = pos.GetKingSquare(US);

			// ����̐F��
			const Color Them = UtilColor::OppositeColor(US);

			const Bitboard checkers = pos.GetCheckersBB();
			Bitboard bb = checkers;
			Bitboard bannedKingToBB = Bitboard::CreateAllZeroBB();
			int checkersNum = 0;
			Square checkSq;

			// �ʂ��������Ȃ��ʒu�� bitboard �𐶐�����B
			// ��΂ɉ��肪�|�����Ă���̂ŁAwhile �ł͂Ȃ��Ado while
			do {

				// �ŏ��Ƀr�b�g�������Ă����}�X��
				checkSq = bb.PopFirstOneFromI9();

				assert(UtilPiece::ToColor(pos.GetPiece(checkSq)) == Them);

				// �J�E���g�A�b�v��
				++checkersNum;


				// checkSq �ɂ����ŉ��肳�ꂽ�Ƃ��A�ʂ͂��̋�̗����̈ʒu�ɂ͈ړ��ł��Ȃ��̂ŁA
				// �ړ��ł��Ȃ��ʒu�� bannnedKingToBB �Ɋi�[����B
				// ������̂Ƃ��ɂ͓�x�A���ŌĂ΂�邽�߁A= �ł͂Ȃ� |= ���g�p���Ă���B
				// �ŏ��ɌĂ΂ꂽ�Ƃ��́AbannedKingToBB == allZeroBB() �ł���B
				// todo: FOECE_INLINE �� template �Ȃ���NPS��r
				//PieceArray::m_pieceAbstractArray[pos.GetPiece(checkSq)].MakeBanned2KingTo(bannedKingToBB, pos, checkSq, ksq);
				//*
				// ���������ɂ���ĕ����Ă��遙
				(Them == Black ?
					BannedKingToMaker::DoMakeBannedKingTo_black(bannedKingToBB, pos, checkSq, ksq) :
					BannedKingToMaker::DoMakeBannedKingTo_white(bannedKingToBB, pos, checkSq, ksq)
					);
				//*/

			} while (bb.Exists1Bit());


			// �ʂ��ړ��o����ړ�����i�[�B
			bb = bannedKingToBB.NotThisAnd(pos.GetBbOf(US).NotThisAnd(g_kingAttackBb.GetControllBb(ksq)));
			while (bb.Exists1Bit()) {
				const Square to = bb.PopFirstOneFromI9();
				// �ړ���ɑ����̗��������邩���ׂ��Ɏw����𐶐�����B
				// attackersTo() ���d���̂ŁAmovePicker �� search �ō��@�肩���ׂ�B
				(*pMovestack++).m_move = g_makePromoteMove.MakeNonPromoteMove<Capture>(N08_King, ksq, to, pos);
			}

			// ������Ȃ�A�ʂ��ړ����邵�������@�͖����B
			// �ʂ̈ړ��͐��������̂ŁA�����ŏI��
			if (1 < checkersNum) {
				return pMovestack;
			}

			// ���肵�Ă������ʈȊO�Ŏ���̐����B
			// pin ����Ă��邩�ǂ����� movePicker �� search �Œ��ׂ�B
			const Bitboard target1 = g_betweenBb.GetBetweenBB(checkSq, ksq);
			const Bitboard target2 = target1 | checkers;
			pMovestack = GeneratePieceMoves<Evasion, N01_Pawn, US, ALL>()(pMovestack, pos, target2, ksq);
			pMovestack = GeneratePieceMoves<Evasion, N02_Lance, US, ALL>()(pMovestack, pos, target2, ksq);
			pMovestack = GeneratePieceMoves<Evasion, N03_Knight, US, ALL>()(pMovestack, pos, target2, ksq);
			pMovestack = GeneratePieceMoves<Evasion, N04_Silver, US, ALL>()(pMovestack, pos, target2, ksq);
			pMovestack = GeneratePieceMoves<Evasion, N05_Bishop, US, ALL>()(pMovestack, pos, target2, ksq);
			pMovestack = GeneratePieceMoves<Evasion, N06_Rook, US, ALL>()(pMovestack, pos, target2, ksq);
			pMovestack = GeneratePieceMoves<Evasion, N16_GoldHorseDragon, US, ALL>()(pMovestack, pos, target2, ksq);

			if (target1.Exists1Bit()) {
				pMovestack = DropMoveGenerator::GenerateDropMoves(US, pMovestack, pos, target1);//<US>
			}

			return pMovestack;
		}
	};

	// �������ꉻ
	// ���肪�|�����Ă��Ȃ��Ƃ��̎w���萶��
	// ����ɂ́A�ʂ������̗����̂���n�_�Ɉړ����鎩�E��ƁApin ����Ă����𓮂������E����܂ށB
	// �����Ő���������� pseudo legal
	template <Color US>
	struct GenerateMoves<NonEvasion, US> {
		/*FORCE_INLINE*/ MoveStack* operator () (
			MoveStack* pMovestack, const Position& pos
			) {
			Bitboard target = pos.GetEmptyBB();

			pMovestack = DropMoveGenerator::GenerateDropMoves(US, pMovestack, pos, target);//<US>
			target |= pos.GetBbOf(UtilColor::OppositeColor(US));
			const Square ksq = pos.GetKingSquare(UtilColor::OppositeColor(US));

			pMovestack = GeneratePieceMoves<NonEvasion, N01_Pawn, US, false>()(pMovestack, pos, target, ksq);
			pMovestack = GeneratePieceMoves<NonEvasion, N02_Lance, US, false>()(pMovestack, pos, target, ksq);
			pMovestack = GeneratePieceMoves<NonEvasion, N03_Knight, US, false>()(pMovestack, pos, target, ksq);
			pMovestack = GeneratePieceMoves<NonEvasion, N04_Silver, US, false>()(pMovestack, pos, target, ksq);
			pMovestack = GeneratePieceMoves<NonEvasion, N05_Bishop, US, false>()(pMovestack, pos, target, ksq);
			pMovestack = GeneratePieceMoves<NonEvasion, N06_Rook, US, false>()(pMovestack, pos, target, ksq);
			pMovestack = GeneratePieceMoves<NonEvasion, N16_GoldHorseDragon, US, false>()(pMovestack, pos, target, ksq);
			pMovestack = GeneratePieceMoves<NonEvasion, N08_King, US, false>()(pMovestack, pos, target, ksq);

			return pMovestack;
		}
	};

	// �������ꉻ
	// �A������̐����ȊO�̔������r���������@�萶��
	// ����Ȃɑ��x���v�������Ƃ���ł͌Ă΂Ȃ��B
	template <Color US>
	struct GenerateMoves<Legal, US> {
		FORCE_INLINE MoveStack* operator () (
			MoveStack* moveStackList, const Position& pos
			) {
			MoveStack* curr = moveStackList;
			const Bitboard pinned = pos.GetPinnedBB();

			moveStackList = pos.InCheck() ?
				GenerateMoves<Evasion, US>()(moveStackList, pos) : GenerateMoves<NonEvasion, US>()(moveStackList, pos);

			// �ʂ̈ړ��ɂ�鎩�E��ƁApin����Ă����̈ړ��ɂ�鎩�E����폜
			while (curr != moveStackList) {
				if (!pos.IsPseudoLegalMoveIsLegal<false, false>(curr->m_move, pinned)) {
					curr->m_move = (--moveStackList)->m_move;
				}
				else {
					++curr;
				}
			}

			return moveStackList;
		}
	};

	// �������ꉻ
	// Evasion �̂Ƃ��ɕ��A��A�p�ƁA����2�i�ڂ̕s������������B
	template <Color US>
	struct GenerateMoves<LegalAll, US> {
		FORCE_INLINE MoveStack* operator () (
			MoveStack* moveStackList, const Position& pos
			) {
			MoveStack* curr = moveStackList;
			const Bitboard pinned = pos.GetPinnedBB();

			moveStackList = pos.InCheck() ?
				GenerateMoves<Evasion, US, true>()(moveStackList, pos) : GenerateMoves<NonEvasion, US>()(moveStackList, pos);

			// �ʂ̈ړ��ɂ�鎩�E��ƁApin����Ă����̈ړ��ɂ�鎩�E����폜
			while (curr != moveStackList) {
				if (!pos.IsPseudoLegalMoveIsLegal<false, false>(curr->m_move, pinned)) {
					curr->m_move = (--moveStackList)->m_move;
				}
				else {
					++curr;
				}
			}

			return moveStackList;
		}
	};

};
