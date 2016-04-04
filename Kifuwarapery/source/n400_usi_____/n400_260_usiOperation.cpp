#include "../../header/n220_position/n220_500_charToPieceUSI.hpp"
#include "../../header/n223_move____/n223_105_utilMove.hpp"
#include "../../header/n240_position/n240_300_tt.hpp"
#include "../../header/n276_genMove_/n276_250_makePromoteMove.hpp"
#include "../../header/n280_move____/n280_150_movePicker.hpp"
#include "../../header/n300_book____/n300_100_book.hpp"
#include "../../header/n320_searcher/n320_150_search.hpp"
#include "../../header/n360_egOption/n360_240_engineOptionsMap.hpp"
#include "../../header/n400_usi_____/n400_260_usiOperation.hpp"



class StringToPieceTypeCSA : public std::map<std::string, PieceType> {
public:
	StringToPieceTypeCSA() {
		(*this)["FU"] = Pawn;
		(*this)["KY"] = Lance;
		(*this)["KE"] = Knight;
		(*this)["GI"] = Silver;
		(*this)["KA"] = Bishop;
		(*this)["HI"] = Rook;
		(*this)["KI"] = Gold;
		(*this)["OU"] = King;
		(*this)["TO"] = ProPawn;
		(*this)["NY"] = ProLance;
		(*this)["NK"] = ProKnight;
		(*this)["NG"] = ProSilver;
		(*this)["UM"] = Horse;
		(*this)["RY"] = Dragon;
	}
	PieceType value(const std::string& str) const {
		return this->find(str)->second;
	}
	bool isLegalString(const std::string& str) const {
		return (this->find(str) != this->end());
	}
};
const StringToPieceTypeCSA g_stringToPieceTypeCSA;


// 考え始めるのはここ。
void UsiOperation::go(const Position& pos, std::istringstream& ssCmd) {
	LimitsType limits;
	std::vector<Move> moves;
	std::string token;

	// go にも種類がある☆
	while (ssCmd >> token) {
		if      (token == "ponder"     ) { limits.ponder = true; }
		else if (token == "btime"      ) { ssCmd >> limits.time[Black]; }
		else if (token == "wtime"      ) { ssCmd >> limits.time[White]; }
		else if (token == "infinite"   ) { limits.infinite = true; }
		else if (token == "byoyomi" ||
				 token == "movetime"   ) {
			// btime wtime の後に byoyomi が来る前提になっているので良くない。
			ssCmd >> limits.moveTime;
			if (limits.moveTime != 0) { limits.moveTime -= pos.GetSearcher()->options["Byoyomi_Margin"]; }
		}
		else if (token == "depth"      ) { ssCmd >> limits.depth; }
		else if (token == "nodes"      ) { ssCmd >> limits.nodes; }
		else if (token == "searchmoves") {
			UsiOperation usiOperation;
			while (ssCmd >> token)
			{
				moves.push_back(usiOperation.usiToMove(pos, token));
			}
		}
	}
	pos.GetSearcher()->searchMoves = moves;

	// 思考を開始☆
	pos.GetSearcher()->threads.startThinking(pos, limits, moves);
}

#if defined LEARN
// 学習用。通常の go 呼び出しは文字列を扱って高コストなので、大量に探索の開始、終了を行う学習では別の呼び出し方にする。
void UsiOperation::go(const Position& pos, const Ply depth, const Move move) {
	LimitsType limits;
	std::vector<Move> moves;
	limits.depth = depth;
	moves.push_back(move);
	pos.GetSearcher()->threads.startThinking(pos, limits, moves);
}
#endif



#if !defined NDEBUG
// for debug
Move UsiOperation::usiToMoveDebug(const Position& pos, const std::string& moveStr) {
	for (MoveList<LegalAll> ml(pos); !ml.end(); ++ml) {
		if (moveStr == ml.move().ToUSI()) {
			return ml.move();
		}
	}
	return Move::GetMoveNone();
}


Move UsiOperation::csaToMoveDebug(const Position& pos, const std::string& moveStr) {
	for (MoveList<LegalAll> ml(pos); !ml.end(); ++ml) {
		if (moveStr == ml.move().ToCSA()) {
			return ml.move();
		}
	}
	return Move::GetMoveNone();
}
#endif


Move UsiOperation::usiToMove(const Position& pos, const std::string& moveStr) {
	const Move move = usiToMoveBody(pos, moveStr);
	assert(move == this->usiToMoveDebug(pos, moveStr));
	return move;
}

Move UsiOperation::csaToMove(const Position& pos, const std::string& moveStr) {
	const Move move = csaToMoveBody(pos, moveStr);
	assert(move == this->csaToMoveDebug(pos, moveStr));
	return move;
}

void UsiOperation::setPosition(Position& pos, std::istringstream& ssCmd) {
	std::string token;
	std::string sfen;

	ssCmd >> token;

	if (token == "startpos") {
		sfen = g_DefaultStartPositionSFEN;
		ssCmd >> token; // "moves" が入力されるはず。
	}
	else if (token == "sfen") {
		while (ssCmd >> token && token != "moves") {
			sfen += token + " ";
		}
	}
	else {
		return;
	}

	pos.Set(sfen, pos.GetSearcher()->threads.mainThread());
	pos.GetSearcher()->setUpStates = StateStackPtr(new std::stack<StateInfo>());

	Ply currentPly = pos.GetGamePly();
	while (ssCmd >> token) {
		const Move move = this->usiToMove(pos, token);
		if (move.IsNone()) break;
		pos.GetSearcher()->setUpStates->push(StateInfo());
		pos.DoMove(move, pos.GetSearcher()->setUpStates->top());
		++currentPly;
	}
	pos.SetStartPosPly(currentPly);
}

Move UsiOperation::usiToMoveBody(const Position& pos, const std::string& moveStr) {
	Move move;
	if (g_charToPieceUSI.isLegalChar(moveStr[0])) {
		// drop
		const PieceType ptTo = UtilPiece::ToPieceType(g_charToPieceUSI.value(moveStr[0]));
		if (moveStr[1] != '*') {
			return Move::GetMoveNone();
		}
		const File toFile = UtilFile::FromCharUSI(moveStr[2]);
		const Rank toRank = UtilRank::FromCharUSI(moveStr[3]);
		if (!UtilSquare::ContainsOf(toFile, toRank)) {
			return Move::GetMoveNone();
		}
		const Square to = UtilSquare::FromFileRank(toFile, toRank);
		move = UtilMove::MakeDropMove(ptTo, to);
	}
	else {
		const File fromFile = UtilFile::FromCharUSI(moveStr[0]);
		const Rank fromRank = UtilRank::FromCharUSI(moveStr[1]);
		if (!UtilSquare::ContainsOf(fromFile, fromRank)) {
			return Move::GetMoveNone();
		}
		const Square from = UtilSquare::FromFileRank(fromFile, fromRank);
		const File toFile = UtilFile::FromCharUSI(moveStr[2]);
		const Rank toRank = UtilRank::FromCharUSI(moveStr[3]);
		if (!UtilSquare::ContainsOf(toFile, toRank)) {
			return Move::GetMoveNone();
		}
		const Square to = UtilSquare::FromFileRank(toFile, toRank);
		if (moveStr[4] == '\0') {
			move = makeNonPromoteMove<Capture>(UtilPiece::ToPieceType(pos.GetPiece(from)), from, to, pos);
		}
		else if (moveStr[4] == '+') {
			if (moveStr[5] != '\0') {
				return Move::GetMoveNone();
			}
			move = makePromoteMove<Capture>(UtilPiece::ToPieceType(pos.GetPiece(from)), from, to, pos);
		}
		else {
			return Move::GetMoveNone();
		}
	}

	if (pos.MoveIsPseudoLegal(move, true)
		&& pos.IsPseudoLegalMoveIsLegal<false, false>(move, pos.GetPinnedBB()))
	{
		return move;
	}
	return Move::GetMoveNone();
}

Move UsiOperation::csaToMoveBody(const Position& pos, const std::string& moveStr) {
	if (moveStr.size() != 6) {
		return Move::GetMoveNone();
	}
	const File toFile = UtilFile::FromCharCSA(moveStr[2]);
	const Rank toRank = UtilRank::FromCharCSA(moveStr[3]);
	if (!UtilSquare::ContainsOf(toFile, toRank)) {
		return Move::GetMoveNone();
	}
	const Square to = UtilSquare::FromFileRank(toFile, toRank);
	const std::string ptToString(moveStr.begin() + 4, moveStr.end());
	if (!g_stringToPieceTypeCSA.isLegalString(ptToString)) {
		return Move::GetMoveNone();
	}
	const PieceType ptTo = g_stringToPieceTypeCSA.value(ptToString);
	Move move;
	if (moveStr[0] == '0' && moveStr[1] == '0') {
		// drop
		move = UtilMove::MakeDropMove(ptTo, to);
	}
	else {
		const File fromFile = UtilFile::FromCharCSA(moveStr[0]);
		const Rank fromRank = UtilRank::FromCharCSA(moveStr[1]);
		if (!UtilSquare::ContainsOf(fromFile, fromRank)) {
			return Move::GetMoveNone();
		}
		const Square from = UtilSquare::FromFileRank(fromFile, fromRank);
		PieceType ptFrom = UtilPiece::ToPieceType(pos.GetPiece(from));
		if (ptFrom == ptTo) {
			// non promote
			move = makeNonPromoteMove<Capture>(ptFrom, from, to, pos);
		}
		else if (ptFrom + PTPromote == ptTo) {
			// promote
			move = makePromoteMove<Capture>(ptFrom, from, to, pos);
		}
		else {
			return Move::GetMoveNone();
		}
	}

	if (pos.MoveIsPseudoLegal(move, true)
		&& pos.IsPseudoLegalMoveIsLegal<false, false>(move, pos.GetPinnedBB()))
	{
		return move;
	}
	return Move::GetMoveNone();
}

