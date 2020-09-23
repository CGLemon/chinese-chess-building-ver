/*
    This file is part of Saya.
    Copyright (C) 2020 Hung-Zhe, Lin

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

#include "BitBoard.h"
#include "Board.h"

#include "config.h"

#include <iostream>
#include <string>
#include <sstream>


const static std::string get_License() {

    auto out = std::ostringstream{};

    out << "    ";
    out << PROGRAM << " " << VERSION << " Copyright (C) 2020  Hung-Zhe, Lin"   << std::endl;

    out << "    This program comes with ABSOLUTELY NO WARRANTY."               << std::endl;
    out << "    This is free software, and you are welcome to redistribute it" << std::endl;
    out << "    under certain conditions; see the COPYING file for details."   << std::endl;

    return out.str();
}


int main(int argc, char** argv) {

    std::cout << get_License();
    init_basic_parameters();

    Board b;

    b.reset_board();
    b.dump_board();

    return 0;
}
