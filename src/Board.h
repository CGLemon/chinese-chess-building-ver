/*
    This file is part of ElephantArt.
    Copyright (C) 2021 Hung-Zhe Lin

    ElephantArt is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ElephantArt is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ElephantArt.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BOARD_H_INCLUDE
#define BOARD_H_INCLUDE

#include "Uint128_t.h"
#include "BitBoard.h"
#include "Zobrist.h"
#include "Utils.h"

#include <cassert>
#include <array>
#include <vector>
#include <iostream>
#include <string>

class Board {
public:
    static constexpr auto WIDTH = BITBOARD_WIDTH;
    
    static constexpr auto HEIGHT = BITBOARD_HEIGHT;

    static constexpr auto SHIFT = BITBOARD_SHIFT;

    static constexpr auto NUM_VERTICES = BITBOARD_NUM_VERTICES;

    static constexpr auto INTERSECTIONS = BITBOARD_INTERSECTIONS;

    void reset_board();

    static Types::Vertices get_vertex(const int x, const int y);
    static int get_index(const int x, const int y);
    static Types::Color swap_color(const Types::Color color);
    static int get_x(const Types::Vertices vtx);
    static int get_y(const Types::Vertices vtx);
    static std::pair<int, int> get_xy(const Types::Vertices vtx);
    static std::string get_start_position();
    Types::Piece get_piece(const int x, const int y) const;
    Types::Piece get_piece(const Types::Vertices vtx) const;

    Types::Piece_t get_piece_type(const Types::Vertices vtx) const;

    Types::Color get_to_move() const;
    int get_movenum() const;
    int get_gameply() const;
    std::uint64_t get_hash() const;
    Move get_last_move() const;
    std::array<Types::Vertices, 2> get_kings() const;
    std::array<BitBoard, 2> get_colors() const;
    int get_repetitions() const;
    int get_cycle_length() const;
    int get_rule50_ply() const;
    int get_rule50_ply_left() const;

    BitBoard generate_movelist(Types::Color color, std::vector<Move> &movelist) const;

    static bool is_on_board(const Types::Vertices vtx);

    void fen_stream(std::ostream &out) const;
    std::string get_fenstring() const;

    template<Types::Language> void board_stream(std::ostream &out, const Move lastmove) const;
    template<Types::Language> void dump_board(const Move lastmove) const;
    template<Types::Language> static void piece_stream(std::ostream &out, Types::Piece p);

    bool fen2board(std::string &fen);

    std::uint64_t calc_hash() const;

    static constexpr std::array<Types::Direction, 8> m_dirs =
        {Types::NORTH,      Types::EAST,       Types::SOUTH,      Types::WEST,
         Types::NORTH_EAST, Types::SOUTH_EAST, Types::SOUTH_WEST, Types::NORTH_WEST};

    static void pre_initialize();
    static Move text2move(std::string text);
    
    std::string get_wxfstring(Move m) const;
    static std::string get_iccsstring(Move m);

    void set_repetitions(int repetitions, int cycle_length);
    void set_last_move(Move m);
    void set_to_move(Types::Color color);
    void swap_to_move();

    void do_move_assume_legal(Move move);
    bool is_legal(Move move) const;
    bool is_capture() const;
    bool is_check(const Types::Color color) const;

private:
    #define P_  Types::R_PAWN
    #define H_  Types::R_HORSE
    #define C_  Types::R_CANNON
    #define R_  Types::R_ROOK
    #define E_  Types::R_ELEPHANT
    #define A_  Types::R_ADVISOR
    #define K_  Types::R_KING

    #define p_  Types::B_PAWN
    #define h_  Types::B_HORSE
    #define c_  Types::B_CANNON
    #define r_  Types::B_ROOK
    #define e_  Types::B_ELEPHANT
    #define a_  Types::B_ADVISOR
    #define k_  Types::B_KING

    #define ET    Types::EMPTY_PIECE
    #define invalid_  Types::INVAL_PIECE

    static constexpr std::array<Types::Piece, NUM_VERTICES> START_VERTICES = {
        R_, H_, E_, A_, K_, A_, E_, H_, R_, invalid_,
        ET, ET, ET, ET, ET, ET, ET, ET, ET, invalid_,
        ET, C_, ET, ET, ET, ET, ET, C_, ET, invalid_,
        P_, ET, P_, ET, P_, ET, P_, ET, P_, invalid_,
        ET, ET, ET, ET, ET, ET, ET, ET, ET, invalid_,
                 // 楚河  漢界
        ET, ET, ET, ET, ET, ET, ET, ET, ET, invalid_,
        p_, ET, p_, ET, p_, ET, p_, ET, p_, invalid_,
        ET, c_, ET, ET, ET, ET, ET, c_, ET, invalid_,
        ET, ET, ET, ET, ET, ET, ET, ET, ET, invalid_,
        r_, h_, e_, a_, k_, a_, e_, h_, r_, invalid_,
    };

    #undef P_
    #undef H_
    #undef C_
    #undef R_
    #undef E_
    #undef A_
    #undef K_

    #undef p_
    #undef h_
    #undef c_
    #undef r_
    #undef e_
    #undef a_
    #undef k_

    #undef ET
    #undef invalid_

    struct Magic {
        BitBoard  mask;
        std::uint64_t  upper_magic;
        std::uint64_t  lower_magic;
        std::vector<BitBoard> attacks;

        std::uint64_t limit;
        int shift;

        bool valid;

        inline std::uint64_t index(BitBoard occupied) const {
            auto mark = occupied & mask;
            return (mark.get_upper() * upper_magic +
                        mark.get_lower() * lower_magic) >> shift;
        }

        inline BitBoard attack(BitBoard occupied) const {
            const auto idx = index(occupied);
            assert(idx <= limit && valid);
            return attacks[idx];
        }
    };

    static std::array<std::array<BitBoard, NUM_VERTICES>, 2> m_pawn_attacks;
    static std::array<BitBoard, NUM_VERTICES> m_advisor_attacks;
    static std::array<BitBoard, NUM_VERTICES> m_king_attacks;

    static std::array<Magic, NUM_VERTICES> m_horse_magics;
    static std::array<Magic, NUM_VERTICES> m_elephant_magics;
    static std::array<Magic, NUM_VERTICES> m_rookrank_magics;
    static std::array<Magic, NUM_VERTICES> m_rookfile_magics;

    static std::array<Magic, NUM_VERTICES> m_cannonrank_magics;
    static std::array<Magic, NUM_VERTICES> m_cannonfile_magics;

    static void init_pawn_attacks();
    static void init_move_pattens();
    static void init_magics();
    static void dump_memory();

    void increment_gameply();
    void decrement_gameply();

    void increment_rule50_ply();
    void set_rule50_ply(const int ply);

    BitBoard &get_piece_bitboard_ref(Types::Piece_t pt);

    std::array<BitBoard, 2> m_bb_color;
    std::array<BitBoard, 2> m_bb_attacks;

    BitBoard m_bb_pawn;
    BitBoard m_bb_horse;
    BitBoard m_bb_rook;
    BitBoard m_bb_elephant;
    BitBoard m_bb_advisor;
    BitBoard m_bb_cannon;

    std::array<Types::Vertices, 2> m_king_vertex;

    Types::Color m_tomove;

    bool is_king_face_king() const;

    int m_movenum;
    int m_gameply;
    bool m_capture;
    Move m_lastmove;

    int m_cycle_length;
    int m_repetitions;
    int m_rule50_ply;

    std::uint64_t m_hash;

    void clear_status();
    template<Types::Piece_t> BitBoard generate_move(Types::Color color, std::vector<Move> &movelist) const;
    template<Types::Language> void piece_stream(std::ostream &out, const int x, const int y) const;
    template<Types::Language> void info_stream(std::ostream &out) const;

    void update_zobrist(Types::Piece p, Types::Vertices form, Types::Vertices to);
    void update_zobrist_remove(Types::Piece p, Types::Vertices vtx);
    void update_zobrist_tomove(Types::Color old_color, Types::Color new_color);

    std::array<BitBoard, 2> calc_attacks();
};

inline Types::Vertices Board::get_vertex(const int x, const int y) {
    assert(x >= 0 || x < WIDTH);
    assert(y >= 0 || y < HEIGHT);

    return Types::Vertices(x + y * SHIFT);
}

inline int Board::get_index(const int x, const int y) {
    assert(x >= 0 || x < WIDTH);
    assert(y >= 0 || y < HEIGHT);

    return x + y * WIDTH;
}

inline int Board::get_x(const Types::Vertices vertex) {
    return vertex % SHIFT;
}

inline int Board::get_y(const Types::Vertices vertex) {
    return vertex / SHIFT;
}

inline std::pair<int, int> Board::get_xy(const Types::Vertices vertex) {
    return std::make_pair(get_x(vertex), get_y(vertex));
}

inline Types::Color Board::swap_color(const Types::Color color) {
    assert(color == Types::RED || color == Types::BLACK);
    if (color == Types::RED) {
        return Types::BLACK;
    }
    return Types::RED;
}

inline void Board::update_zobrist(Types::Piece p, Types::Vertices form, Types::Vertices to) {
    m_hash ^= Zobrist::zobrist[p][form];
    m_hash ^= Zobrist::zobrist[Types::EMPTY_PIECE][form];
    m_hash ^= Zobrist::zobrist[Types::EMPTY_PIECE][to];
    m_hash ^= Zobrist::zobrist[p][to];
}

inline void Board::update_zobrist_remove(Types::Piece p, Types::Vertices vtx) {
    m_hash ^= Zobrist::zobrist[p][vtx];
    m_hash ^= Zobrist::zobrist[Types::EMPTY_PIECE][vtx];
}

inline void Board::update_zobrist_tomove(Types::Color old_color, Types::Color new_color) {
    if (old_color != new_color) {
        m_hash ^= Zobrist::zobrist_redtomove;
    }
}

template<Types::Language L>
void Board::piece_stream(std::ostream &out, const int x, const int y) const {
    const auto p = get_piece(x, y);
    piece_stream<L>(out, p);
}

template<Types::Language L>
void Board::dump_board(const Move lastmove) const {
    auto out = std::ostringstream{};
    board_stream<L>(out, lastmove);
    Utils::printf<Utils::STATIC>(out);
}
#endif
