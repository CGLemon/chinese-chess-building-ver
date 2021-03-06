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

#ifndef REPETITION_H_INCLUDE
#define REPETITION_H_INCLUDE
#include "Position.h"

class Repetition {
public:
    enum Result { NONE = 0, DRAW, LOSE, UNKNOWN };

    Repetition(Position &position);

    Result judge();

private:
    Position &m_position;
};

#endif
