/*
    This file is part of Saya.
    Copyright (C) 2020 Hung-Zhe Lin

    Saya is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Saya is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Saya.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Position.h"

void Position::init_game() {
    board.reset_board();
    m_history.clear();
}

void Position::display() const {

    board.dump_board();
}

Types::Color Position::get_to_move() const {
    return board.get_to_move();
}

std::vector<Move> Position::get_movelist() const {

    auto movelist = std::vector<Move>{};
    const auto color = get_to_move();
    board.generate_movelist(color, movelist);

    return movelist;
}

bool Position::fen2board(std::string &fen) {
    return board.fen2board(fen);
}

bool Position::is_legal(Move move) const {
    return board.is_legal(move);
}
