#include "board.hpp"

bool Board::push(Move move) {
    auto searchMove = std::find(legalMoves.begin(), legalMoves.end(), move);
    if (searchMove == legalMoves.end()) return false;
    move = *searchMove;

    if (move.defend) return false;

    Piece fromPiece;
    if (!pieceAtBitboard(move.from, &fromPiece)) return false;

    // Snapshot
    BoardSnapshot snapshot;
    snapshot.turn = turn;
    snapshot.move = move;
    snapshot.enPassantCol = enPassantCol;
    snapshot.zobrist = zobrist;
    std::copy(std::begin(castling[0]), std::end(castling[0]), std::begin(snapshot.castling[0]));
    std::copy(std::begin(castling[1]), std::end(castling[1]), std::begin(snapshot.castling[1]));
    std::copy(std::begin(piecesBitboards[0]), std::end(piecesBitboards[0]), std::begin(snapshot.piecesBitboards[0]));
    std::copy(std::begin(piecesBitboards[1]), std::end(piecesBitboards[1]), std::begin(snapshot.piecesBitboards[1]));
    snapshot.legalMoves = legalMoves;
    snapshot.opponentLegalMoves = opponentLegalMoves;
    moveStack.push_back(snapshot);

    // Update castling possibilities
    if (fromPiece.pieceType == King) {
        castling[fromPiece.color][QueenSide] = 0;
        castling[fromPiece.color][KingSide] = 0;
    } else if (fromPiece.pieceType == Rook) {
        if (move.from & ~noACol) castling[fromPiece.color][QueenSide] = false;
        else if (move.from & ~noHCol) castling[fromPiece.color][KingSide] = false;
    }

    Piece toPiece;
    bool toPieceExist = pieceAtBitboard(move.to, &toPiece);
    if (toPieceExist)
        piecesBitboards[toPiece.color][toPiece.pieceType] ^= move.to;

    // Process Castling
    if (move.castling) {
        uint64_t kingRow = move.to >> ((turn == White) ? 0 : 7*8);
        if (kingRow & 0b1111) { // Queen side
            piecesBitboards[turn][Rook] ^= move.from >> 4;
            piecesBitboards[turn][Rook] ^= move.from >> 1;
        } else { // King side
            piecesBitboards[turn][Rook] ^= move.from << 3;
            piecesBitboards[turn][Rook] ^= move.from << 1;
        }
    }

    // Process en passant
    if (move.enPassant) 
        piecesBitboards[!turn][Pawn] ^= (uint64_t) 1 << (enPassantCol + 8*((turn == White) ? 4 : 3));

    piecesBitboards[fromPiece.color][fromPiece.pieceType] ^= move.from;
    if (move.promotion) piecesBitboards[fromPiece.color][Queen] ^= move.to;
    else piecesBitboards[fromPiece.color][fromPiece.pieceType] ^= move.to;

    // En passant detection
    enPassantCol = -1;
    if (fromPiece.pieceType == Pawn) {
        if (move.from & (uint64_t) 0xFF << 8*((turn == White) ? 1 : 6)) {
            if (move.to & (uint64_t) 0xFF << 8*((turn == White) ? 3 : 4)) {
                enPassantCol = reverseBitscan(move.to) & 7;
            }
        }
    }

    turn = (Color) !turn;

    zobrist = zobristHelper.updateZobrist(snapshot.zobrist, snapshot.castling, snapshot.enPassantCol, castling, enPassantCol, fromPiece, toPiece, toPieceExist, turn, move);

    generateMoves();
    return true;
}

void Board::pop() {
    if (moveStack.empty()) return;

    BoardSnapshot snapshot = moveStack.back();
    moveStack.pop_back();

    turn = snapshot.turn;
    enPassantCol = snapshot.enPassantCol;
    std::copy(std::begin(snapshot.castling[0]), std::end(snapshot.castling[0]), std::begin(castling[0]));
    std::copy(std::begin(snapshot.castling[1]), std::end(snapshot.castling[1]), std::begin(castling[1]));
    std::copy(std::begin(snapshot.piecesBitboards[0]), std::end(snapshot.piecesBitboards[0]), std::begin(piecesBitboards[0]));
    std::copy(std::begin(snapshot.piecesBitboards[1]), std::end(snapshot.piecesBitboards[1]), std::begin(piecesBitboards[1]));
    legalMoves = snapshot.legalMoves;
    opponentLegalMoves = snapshot.opponentLegalMoves;
    zobrist = snapshot.zobrist;
}

bool Board::pieceAt(uint square, Piece* piece) {
    uint64_t bitboard = (uint64_t) 1 << square;
    return pieceAtBitboard(bitboard, piece);
}

bool Board::pieceAtBitboard(uint64_t bitboard, Piece* piece) {
    for (int color=0; color<2; color++){
        for (int pieceType=0; pieceType<6; pieceType++){
            uint64_t isPresent = piecesBitboards[color][pieceType] & bitboard;
            if (isPresent > 0) {
                piece->color = (Color) color;
                piece->pieceType = (PieceType) pieceType;
                return true;
            }
        }
    }
    return false;
}


std::vector<uint64_t> Board::pieces(PieceType pieceType, Color color) {
    uint64_t bitboard = this->piecesBitboards[color][pieceType];
    return serializeBitboard(bitboard);
}

uint64_t Board::getColorBitboard(Color color){
    uint64_t colorBitboard = 0;

    for (int pieceType=0; pieceType<6; pieceType++) {
        colorBitboard |= piecesBitboards[color][pieceType];
    }

    return colorBitboard;
}

std::vector<Move> Board::generateColorMoves(uint64_t colorBitboard, uint64_t opponentColorBitboard, uint64_t globalBitboard, Color color, std::vector<Move>* opponentLegalMovesPtr) {
    inCheck = false;
    uint kingAttackers = 0;
    uint64_t kingAttackerBitboard;
    Direction kingAttackerDirection = NoDirection;

    uint64_t attackedSquare = 0;

    if (opponentLegalMovesPtr != nullptr) 
        for (Move opponentMove: *opponentLegalMovesPtr)
            if (opponentMove.attack) {
                attackedSquare |= opponentMove.to;
                if (piecesBitboards[color][King] == opponentMove.to) {
                    kingAttackers++;
                    kingAttackerBitboard = opponentMove.from;

                    Piece kingAttacker;
                    pieceAt(kingAttackerBitboard, &kingAttacker);
                    if (kingAttacker.pieceType == Queen || kingAttacker.pieceType == Rook || kingAttacker.pieceType == Bishop)
                        kingAttackerDirection = opponentMove.direction;
                }
            }

    uint64_t kingProtectionSquare = 0xffffffffffffffff;
    if (kingAttackers > 0) {
        inCheck = true;
        if (kingAttackers == 1) {
            kingProtectionSquare = kingAttackerBitboard;
            for (Move opponentMove: *opponentLegalMovesPtr)
                if (opponentMove.attack && opponentMove.from == kingAttackerBitboard && opponentMove.direction == kingAttackerDirection) 
                    kingProtectionSquare |= opponentMove.to;
        }
    }

    std::vector<Move> moves;

    if (kingAttackers <=1) {
        for (int pieceType=0; pieceType<6; pieceType++){
            for (uint64_t pieceBitboard : pieces((PieceType) pieceType, color)){
                std::vector<Move> piece_moves;
                switch (pieceType) {
                    case Pawn: piece_moves = generatePawnMoves(pieceBitboard, globalBitboard, colorBitboard, opponentColorBitboard, color); break;
                    case Knight: piece_moves = generateKnightMoves(pieceBitboard, globalBitboard, colorBitboard, opponentColorBitboard, color); break;
                    case King: piece_moves = generateKingMoves(pieceBitboard, globalBitboard, colorBitboard, opponentColorBitboard, attackedSquare, kingAttackerDirection, color); break;
                    case Queen: piece_moves = generateSlidingPiecesMoves(pieceBitboard, queenDirections, globalBitboard, colorBitboard, opponentColorBitboard, color); break;
                    case Rook: piece_moves = generateSlidingPiecesMoves(pieceBitboard, rookDirections, globalBitboard, colorBitboard, opponentColorBitboard, color); break;
                    case Bishop: piece_moves = generateSlidingPiecesMoves(pieceBitboard, bishopDirections, globalBitboard, colorBitboard, opponentColorBitboard, color); break;
                }
                if (kingAttackers == 1 && pieceType != King) piece_moves = removeNonKingProtectionMove(piece_moves, kingProtectionSquare);
                std::vector<Move> legalMove = removeNonLegalMove(piece_moves, pieceBitboard, (PieceType) pieceType);
                moves.insert(moves.begin(), legalMove.begin(), legalMove.end());
            }
        }
    } else {
        return generateKingMoves(piecesBitboards[color][King], globalBitboard, colorBitboard, opponentColorBitboard, attackedSquare, kingAttackerDirection, color);
    }
    return moves;
}

 std::vector<Move> Board::removeNonKingProtectionMove(std::vector<Move> moves, uint64_t kingProtectionSquare) {
     std::vector<Move> legalMoves;
     for (Move move: moves) {
         if (move.to & kingProtectionSquare) legalMoves.push_back(move);
     }
     return legalMoves;
 }

 std::vector<Move> Board::removeNonLegalMove(std::vector<Move> moves, uint64_t pieceBitboard, PieceType pieceType) {
     PinnedPiece searchPinnedPiece = PinnedPiece();
     searchPinnedPiece.square = reverseBitscan(pieceBitboard);

     std::vector<PinnedPiece>::iterator pinnedPieceItr = std::find(pinnedPieces.begin(), pinnedPieces.end(), searchPinnedPiece);
     if (pinnedPieceItr == pinnedPieces.end()) return moves;

     std::vector<Move> legalMoves;
     if (pieceType == Knight) return legalMoves;

     Direction pinnedDirection = pinnedPieceItr->direction;
     Direction complementaryPinnedDirection = (pinnedDirection >= 4) ? Direction (pinnedDirection - 4) : Direction (pinnedDirection + 4);
     for (Move move: moves)
         if (move.direction == pinnedDirection || move.direction == complementaryPinnedDirection) legalMoves.push_back(move);

     return legalMoves;
}

void Board::generateMoves(){
    uint64_t colorBitboard = getColorBitboard(turn);
    uint64_t opponentColorBitboard = getColorBitboard((Color) !turn);
    uint64_t globalBitboard = opponentColorBitboard | colorBitboard;

    pinnedPieces.clear();

    opponentLegalMoves = generateColorMoves(opponentColorBitboard, colorBitboard, globalBitboard, (Color) !turn, nullptr);
    legalMoves = generateColorMoves(colorBitboard, opponentColorBitboard, globalBitboard, turn, &opponentLegalMoves);
}

Board::Board(std::string fen) {
    uint row = 7;
    uint col = 0;

    uint fen_part = 0;

    for (char& c : fen) {
        if (c == ' ') {
            fen_part++;
            continue;
        }

        bool isCapital = c >= 'A' && c <= 'Z';
        bool isDigit = c >= '0' && c <= '9';

        if (fen_part == 0) {
            if (c == '/') {
                row--;
                col = 0;
                continue;
            }
            if (isDigit) {
                col += (int) c - (int) '0';
                continue;
            }

            Color color = static_cast<Color>(!isCapital);
            if (isCapital) c += 'a' - 'A';

            PieceType pieceType;
            switch (c) {
                case 'r': pieceType = Rook; break;
                case 'b': pieceType = Bishop; break;
                case 'n': pieceType = Knight; break;
                case 'q': pieceType = Queen; break;
                case 'k': pieceType = King; break;
                case 'p': pieceType = Pawn; break;
            }

            uint64_t bitboard = (uint64_t) 1 << (col + 8*row);
            this->piecesBitboards[color][pieceType] ^= bitboard;

            col++;
        } else if (fen_part == 1) {
            switch (c) {
                case 'w': this->turn = White; break;
                case 'b': this->turn = Black; break;
            }
        } else if (fen_part == 2) {
            Color color = isCapital ? White : Black;
            if (isCapital) c += 'a' - 'A';

            switch (c) {
                case 'q': this->castling[color][QueenSide] = true; break;
                case 'k': this->castling[color][KingSide] = true; break;
            }
        } else if (fen_part == 3) {
            if (isDigit) this->enPassantCol = (int) c - (int) '0';
        }
    }

    zobrist = zobristHelper.getZobrist(piecesBitboards, castling, enPassantCol, turn);

    initRayAttack();
    generateMoves();
}
