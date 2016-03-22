#include "../../header/n240_position/n240_100_position.hpp"
#include "../../header/n240_position/n240_150_move.hpp"
#include "../../header/n240_position/n240_300_tt.hpp"
#include "../../header/n280_move____/n280_100_generateMoves.hpp"
#include "../../header/n280_move____/n280_150_movePicker.hpp"
#include "../../header/n320_operate_/n320_100_book.hpp"
#include "../../header/n320_operate_/n320_150_search.hpp"
#include "../../header/n320_operate_/n320_240_usiOptionsMap.hpp"
#include "../../header/n320_operate_/n320_250_usi.hpp"
#include "../../header/n320_operate_/n320_300_benchmark.hpp"
#include "../../header/n320_operate_/n320_400_learner.hpp"

#include "../../header/n320_operate_/n320_260_usiOperation.hpp"



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



void UsiOperation::go(const Position& pos, std::istringstream& ssCmd) {
	LimitsType limits;
	std::vector<Move> moves;
	std::string token;

	while (ssCmd >> token) {
		if (token == "ponder") { limits.ponder = true; }
		else if (token == "btime") { ssCmd >> limits.time[Black]; }
		else if (token == "wtime") { ssCmd >> limits.time[White]; }
		else if (token == "infinite") { limits.infinite = true; }
		else if (token == "byoyomi" || token == "movetime") {
			// btime wtime の後に byoyomi が来る前提になっているので良くない。
			ssCmd >> limits.moveTime;
			if (limits.moveTime != 0) { limits.moveTime -= pos.searcher()->options["Byoyomi_Margin"]; }
		}
		else if (token == "depth") { ssCmd >> limits.depth; }
		else if (token == "nodes") { ssCmd >> limits.nodes; }
		else if (token == "searchmoves") {
			UsiOperation usiOperation;
			while (ssCmd >> token)
			{
				moves.push_back(usiOperation.usiToMove(pos, token));
			}
		}
	}
	pos.searcher()->searchMoves = moves;
	pos.searcher()->threads.startThinking(pos, limits, moves);
}

#if defined LEARN
// 学習用。通常の go 呼び出しは文字列を扱って高コストなので、大量に探索の開始、終了を行う学習では別の呼び出し方にする。
void UsiOperation::go(const Position& pos, const Ply depth, const Move move) {
	LimitsType limits;
	std::vector<Move> moves;
	limits.depth = depth;
	moves.push_back(move);
	pos.searcher()->threads.startThinking(pos, limits, moves);
}
#endif



#if !defined NDEBUG
// for debug
Move UsiOperation::usiToMoveDebug(const Position& pos, const std::string& moveStr) {
	for (MoveList<LegalAll> ml(pos); !ml.end(); ++ml) {
		if (moveStr == ml.move().toUSI()) {
			return ml.move();
		}
	}
	return Move::moveNone();
}


Move UsiOperation::csaToMoveDebug(const Position& pos, const std::string& moveStr) {
	for (MoveList<LegalAll> ml(pos); !ml.end(); ++ml) {
		if (moveStr == ml.move().toCSA()) {
			return ml.move();
		}
	}
	return Move::moveNone();
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
		sfen = DefaultStartPositionSFEN;
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

	pos.set(sfen, pos.searcher()->threads.mainThread());
	pos.searcher()->setUpStates = StateStackPtr(new std::stack<StateInfo>());

	Ply currentPly = pos.gamePly();
	while (ssCmd >> token) {
		const Move move = this->usiToMove(pos, token);
		if (move.isNone()) break;
		pos.searcher()->setUpStates->push(StateInfo());
		pos.doMove(move, pos.searcher()->setUpStates->top());
		++currentPly;
	}
	pos.setStartPosPly(currentPly);
}

Move UsiOperation::usiToMoveBody(const Position& pos, const std::string& moveStr) {
	Move move;
	if (g_charToPieceUSI.isLegalChar(moveStr[0])) {
		// drop
		const PieceType ptTo = pieceToPieceType(g_charToPieceUSI.value(moveStr[0]));
		if (moveStr[1] != '*') {
			return Move::moveNone();
		}
		const File toFile = charUSIToFile(moveStr[2]);
		const Rank toRank = charUSIToRank(moveStr[3]);
		if (!isInSquare(toFile, toRank)) {
			return Move::moveNone();
		}
		const Square to = makeSquare(toFile, toRank);
		move = makeDropMove(ptTo, to);
	}
	else {
		const File fromFile = charUSIToFile(moveStr[0]);
		const Rank fromRank = charUSIToRank(moveStr[1]);
		if (!isInSquare(fromFile, fromRank)) {
			return Move::moveNone();
		}
		const Square from = makeSquare(fromFile, fromRank);
		const File toFile = charUSIToFile(moveStr[2]);
		const Rank toRank = charUSIToRank(moveStr[3]);
		if (!isInSquare(toFile, toRank)) {
			return Move::moveNone();
		}
		const Square to = makeSquare(toFile, toRank);
		if (moveStr[4] == '\0') {
			move = makeNonPromoteMove<Capture>(pieceToPieceType(pos.piece(from)), from, to, pos);
		}
		else if (moveStr[4] == '+') {
			if (moveStr[5] != '\0') {
				return Move::moveNone();
			}
			move = makePromoteMove<Capture>(pieceToPieceType(pos.piece(from)), from, to, pos);
		}
		else {
			return Move::moveNone();
		}
	}

	if (pos.moveIsPseudoLegal(move, true)
		&& pos.pseudoLegalMoveIsLegal<false, false>(move, pos.pinnedBB()))
	{
		return move;
	}
	return Move::moveNone();
}

Move UsiOperation::csaToMoveBody(const Position& pos, const std::string& moveStr) {
	if (moveStr.size() != 6) {
		return Move::moveNone();
	}
	const File toFile = charCSAToFile(moveStr[2]);
	const Rank toRank = charCSAToRank(moveStr[3]);
	if (!isInSquare(toFile, toRank)) {
		return Move::moveNone();
	}
	const Square to = makeSquare(toFile, toRank);
	const std::string ptToString(moveStr.begin() + 4, moveStr.end());
	if (!g_stringToPieceTypeCSA.isLegalString(ptToString)) {
		return Move::moveNone();
	}
	const PieceType ptTo = g_stringToPieceTypeCSA.value(ptToString);
	Move move;
	if (moveStr[0] == '0' && moveStr[1] == '0') {
		// drop
		move = makeDropMove(ptTo, to);
	}
	else {
		const File fromFile = charCSAToFile(moveStr[0]);
		const Rank fromRank = charCSAToRank(moveStr[1]);
		if (!isInSquare(fromFile, fromRank)) {
			return Move::moveNone();
		}
		const Square from = makeSquare(fromFile, fromRank);
		PieceType ptFrom = pieceToPieceType(pos.piece(from));
		if (ptFrom == ptTo) {
			// non promote
			move = makeNonPromoteMove<Capture>(ptFrom, from, to, pos);
		}
		else if (ptFrom + PTPromote == ptTo) {
			// promote
			move = makePromoteMove<Capture>(ptFrom, from, to, pos);
		}
		else {
			return Move::moveNone();
		}
	}

	if (pos.moveIsPseudoLegal(move, true)
		&& pos.pseudoLegalMoveIsLegal<false, false>(move, pos.pinnedBB()))
	{
		return move;
	}
	return Move::moveNone();
}

