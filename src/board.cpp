#include "board.hpp"

uint64_t bishopLegalAttack[64][512];
uint64_t rookLegalAttack[64][4096];
uint64_t knightAttack[64];
uint64_t kingAttack[64];
uint64_t pawnAttack[2][64];
uint64_t pawnMove[2][64];
Magic bishopMagic[64];
Magic rookMagic[64];

uint64_t powOf2[64];

bool Board::push(Move move) {
    auto searchMove = std::find(legalMoves.begin(), legalMoves.end(), move);
    if (searchMove == legalMoves.end()) return false;
    move = *searchMove;

    Piece fromPiece;
    if (!pieceAtBitboard(move.from, &fromPiece)) return false;

    // Snapshot
    BoardSnapshot snapshot;
    snapshot.turn = turn;
    snapshot.move = move;
    snapshot.enPassantCol = enPassantCol;
    snapshot.zobrist = zobrist;
    std::copy(std::begin(castling[0]), std::end(castling[1]), std::begin(snapshot.castling[0]));
    std::copy(std::begin(piecesBitboards[0]), std::end(piecesBitboards[1]), std::begin(snapshot.piecesBitboards[0]));
    std::copy(std::begin(enemyAttacks), std::end(enemyAttacks), std::begin(snapshot.enemyAttacks));
    snapshot.legalMoves = legalMoves;
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
    snapshot.lastMoveIsCapture = toPieceExist;
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
    std::copy(std::begin(snapshot.castling[0]), std::end(snapshot.castling[1]), std::begin(castling[0]));
    std::copy(std::begin(snapshot.piecesBitboards[0]), std::end(snapshot.piecesBitboards[1]), std::begin(piecesBitboards[0]));
    std::copy(std::begin(snapshot.enemyAttacks), std::end(snapshot.enemyAttacks), std::begin(enemyAttacks));
    legalMoves = snapshot.legalMoves;
    zobrist = snapshot.zobrist;
}

bool Board::pieceAt(uint square, Piece* piece) {
    uint64_t bitboard = powOf2[square];
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

std::vector<int> Board::pieces(PieceType pieceType, Color color) {
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

bool Board::isRepetition() {
    bool isRepetition = false;
    int size = moveStack.size();
    if (size >= 12) {
        if (moveStack[size-1].move == moveStack[size-5].move && moveStack[size-1].move == moveStack[size-9].move)
            if (moveStack[size-3].move == moveStack[size-7].move && moveStack[size-3].move == moveStack[size-11].move)
                if (moveStack[size-2].move == moveStack[size-6].move && moveStack[size-2].move == moveStack[size-10].move)
                    if (moveStack[size-4].move == moveStack[size-8].move && moveStack[size-4].move == moveStack[size-12].move)
                        return true;
    }
    return isRepetition;
}

Board::Board(std::string fen) {
    initAttack();

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

    generateMoves();
}

void Board::initAttack() {
    for (int i=0; i<64; i++) powOf2[i] = (uint64_t) 1 << i;

    initKnightAttack();
    initKingAttack();
    initPawnAttack();

    initSlidingPieces();
}
