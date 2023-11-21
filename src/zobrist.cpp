#include "zobrist.hpp"

Zobrist::Zobrist() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint64_t> dis(std::numeric_limits<std::uint64_t>::min(), std::numeric_limits<std::uint64_t>::max());

    for (int color=0; color<2; color++) {
        for (int pieceType=0; pieceType<6; pieceType++)
            for (int square=0; square<64; square++) 
                piecesZobrist[color][pieceType][square] = dis(gen);

        for (int castling=0; castling<4; castling++)
            castlingZobrist[color][castling] = dis(gen);
    }

    for (int enPassant=0; enPassant<16; enPassant++)
        enPassantZobrist[enPassant] = dis(gen);

    sideZobrist = dis(gen);
}

uint64_t Zobrist::getZobrist(uint64_t piecesBitboards[2][6], bool castling[2][2], int enPassantCol, Color color) {
    uint64_t zobrist = 0;

    for (int color=0; color<2; color++)
        for (int pieceType=0; pieceType<6; pieceType++) {
            std::vector<uint64_t> bitboards = serializeBitboard(piecesBitboards[color][pieceType]);
            for (uint64_t bitboard: bitboards)
                zobrist ^= piecesZobrist[color][pieceType][reverseBitscan(bitboard)];
        }

    zobrist ^= castlingZobrist[White][int (castling[White][0]) + (int (castling[White][1]) << 1)];
    zobrist ^= castlingZobrist[Black][int (castling[Black][0]) + (int (castling[Black][1]) << 1)];

    if (enPassantCol >= 0)
        zobrist ^= enPassantZobrist[enPassantCol + 8*((color == White) ? 0:1)];

    zobrist ^= (int) color * sideZobrist;

    return zobrist;
}

uint64_t Zobrist::updateZobrist(uint64_t zobrist, bool oldCastling[2][2], int oldEnPassantCol, bool castling[2][2], int enPassantCol, Piece fromPiece, Piece toPiece, bool toPieceExist, Color newColor, Move move) {
    zobrist ^= int (!newColor) * sideZobrist;

    int toSquare = reverseBitscan(move.to);
    int fromSquare = reverseBitscan(move.from);

    if (toPieceExist)
        zobrist ^= piecesZobrist[newColor][toPiece.pieceType][toSquare];

    zobrist ^= piecesZobrist[!newColor][fromPiece.pieceType][fromSquare];
    zobrist ^= piecesZobrist[!newColor][fromPiece.pieceType][toSquare];

    if (move.castling || fromPiece.pieceType == King) {
        zobrist ^= castlingZobrist[White][int (oldCastling[White][0]) + (int (oldCastling[White][1]) << 1)];
        zobrist ^= castlingZobrist[Black][int (oldCastling[Black][0]) + (int (oldCastling[Black][1]) << 1)];

        zobrist ^= castlingZobrist[White][int (castling[White][0]) + (int (castling[White][1]) << 1)];
        zobrist ^= castlingZobrist[Black][int (castling[Black][0]) + (int (castling[Black][1]) << 1)];

        if (move.castling) {
            uint64_t kingRow = move.to >> ((newColor == Black) ? 0 : 7*8);
            if (kingRow & 0b1111) { // Queen side
                zobrist ^= piecesZobrist[!newColor][Rook][fromSquare - 4];
                zobrist ^= piecesZobrist[!newColor][Rook][fromSquare - 1];
            } else { // King side
                zobrist ^= piecesZobrist[!newColor][Rook][fromSquare + 3];
                zobrist ^= piecesZobrist[!newColor][Rook][fromSquare + 1];
            }
        }
    }

    if (oldEnPassantCol >= 0)
        zobrist ^= enPassantZobrist[oldEnPassantCol + 8*((newColor == White) ? 1:0)];
    if (enPassantCol >= 0)
        zobrist ^= enPassantZobrist[enPassantCol + 8*((newColor == White) ? 0:1)];

    zobrist ^= newColor * sideZobrist;

    return zobrist;
}
