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

#include <memory>
#include <algorithm>

#include "ForcedCheckmate.h"
#include "Board.h"

ForcedCheckmate::ForcedCheckmate(Position &position) : m_rootpos(position) {
    m_relaxed_move = 0; // unused
    m_maxdepth = 16;
    m_factor = 50.f;
    m_color = m_rootpos.get_to_move();
}

Move ForcedCheckmate::find_checkmate() {
    auto movelist = m_rootpos.get_movelist();
    return find_checkmate(movelist);
}

Move ForcedCheckmate::find_checkmate(std::vector<Move> &movelist) {
    if (m_rootpos.gameover(true)) {
        return Move{};
    }

    auto hashbuf = std::vector<std::uint64_t>(10);
    const auto kings = m_rootpos.get_kings();
    auto cnt = size_t{0};

    for (const auto &move: movelist) {
        ++cnt;
        if (move.get_to() == kings[Board::swap_color(m_color)]) {
            return move;
        }

        if (m_rootpos.is_check(m_color)) {
            // We already win the game. Just find out what move capture
            // opponent king.
            continue;
        }

        auto nextpos = std::make_shared<Position>(m_rootpos);
        nextpos->do_move_assume_legal(move);
        if (nextpos->is_check(m_color)) {
            const auto repetitions = m_rootpos.get_repetitions();
            if (repetitions >= 2) {
                // This may cause the perpetual check. We may lose the game.
                // Or the other best result is draw. We don't want these results.
                continue;
            }
            hashbuf[0] = nextpos->get_hash();
            const auto success = !uncheckmate_search(*nextpos, hashbuf, 1, movelist.size() - cnt);
            if (success) {
                return move;
            }
        }
    }

    return Move{};
}

bool ForcedCheckmate::is_opp_checkmate() {
    auto movelist = m_rootpos.get_movelist();
    return is_opp_checkmate(movelist);
}

bool ForcedCheckmate::is_opp_checkmate(std::vector<Move> &movelist) {
    if (m_rootpos.get_winner(true) == Board::swap_color(m_color)) {
        return true;
    }

    const auto kings = m_rootpos.get_kings();

    auto hashbuf = std::vector<std::uint64_t>(10);
    auto cnt = size_t{0};
    for (const auto &move: movelist) {
        ++cnt;
        if (move.get_to() == kings[Board::swap_color(m_color)]) {
            return false;
        }

        auto nextpos = std::make_shared<Position>(m_rootpos);
        nextpos->do_move_assume_legal(move);

        if (nextpos->is_check(Board::swap_color(m_color))) {
            return true;
        }

        hashbuf[0] = nextpos->get_hash();

        const auto success = checkmate_search(*nextpos, hashbuf, 1, movelist.size() - cnt);
        if (success) {
            return true;
        }
    }
    return false;
}

bool ForcedCheckmate::checkmate_search(Position &currentpos,
                                       std::vector<std::uint64_t> &buf, int depth, int nodes) const {
    int bound = depth * m_factor / float(nodes);
    if (currentpos.get_rule50_ply_left() == 0 || depth > m_maxdepth + bound) {
        return false;
    }
    const auto to_move = currentpos.get_to_move();
    const auto movelist = currentpos.get_movelist();
    const auto kings = currentpos.get_kings();

    auto cnt = size_t{0};
    for (const auto &move: movelist) {
        ++cnt;
        if (move.get_to() == kings[Board::swap_color(to_move)]) {
            return true;
        }

        if (currentpos.is_check(to_move)) {
            // We already win the game. Just find out what move capture
            // opponent king.
            continue;
        }

        auto nextpos = std::make_shared<Position>(currentpos);
        nextpos->do_move_assume_legal(move);

        const auto repetitions = m_rootpos.get_repetitions();
        if (repetitions >= 2) {
            continue;
        }

        auto hash = nextpos->get_hash();
        if ((int)buf.size() < depth+1) {
            buf.resize(depth+1);
        }

        if (std::find(std::begin(buf), 
                std::begin(buf) + depth, hash) == 
                std::begin(buf) + depth) {
            buf[depth] = hash; 
        } else {
            // This move is exist before. We don't need to
            // waste time to search it.
            continue;
        }

        if (!nextpos->is_check(to_move)) {
            continue;
        }

        const auto success = !uncheckmate_search(*nextpos, buf, depth+1, movelist.size() - cnt + nodes);
        if (success) {
            return true;
        }
    }
    // We don't find a checkmate move.
    return false;
}

bool ForcedCheckmate::uncheckmate_search(Position &currentpos,
                                         std::vector<std::uint64_t> &buf, int depth, int nodes) const {
    int bound = depth * m_factor / float(nodes);
    if (currentpos.get_rule50_ply_left() == 0 || depth > m_maxdepth + bound) {
        return true;
    }
    const auto to_move = currentpos.get_to_move();
    const auto movelist = currentpos.get_movelist();
    const auto kings = currentpos.get_kings();

    auto cnt = size_t{0};
    for (const auto &move: movelist) {
        ++cnt;
        if (move.get_to() == kings[Board::swap_color(to_move)]) {
            return true;
        }

        auto nextpos = std::make_shared<Position>(currentpos);
        nextpos->do_move_assume_legal(move);

        if (nextpos->is_check(Board::swap_color(to_move))) {
            continue;
        }

        if ((int)buf.size() < depth+1) {
            buf.resize(depth+1);
        }
        buf[depth] = nextpos->get_hash();

        const auto success = !checkmate_search(*nextpos, buf, depth+1, movelist.size() - cnt + nodes);
        if (success) {
            return true;
        }
    }

    // We don't find a uncheckmate move.
    return false;
}
