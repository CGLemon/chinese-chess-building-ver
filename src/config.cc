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

#include "config.h"
#include "Zobrist.h"
#include "Board.h"
#include "Utils.h"

#include <string>
#include <mutex>

std::unordered_map<std::string, Utils::Option> options_map;
// std::mutex map_mutex;

#define OPTIONS_EXPASSION(T)                        \
template<>                                          \
T option<T>(std::string name) {                     \
    return options_map.find(name)->second.get<T>(); \
}

OPTIONS_EXPASSION(std::string)
OPTIONS_EXPASSION(bool)
OPTIONS_EXPASSION(int)
OPTIONS_EXPASSION(float)

#undef OPTIONS_EXPASSION

#define OPTIONS_SET_EXPASSION(T)                     \
template<>                                           \
bool set_option<T>(std::string name, T val) {        \
    auto res = options_map.find(name);               \
    if (res != std::end(options_map)) {              \
        res->second.set<T>(val);                     \
        return true;                                 \
    }                                                \
    return false;                                    \
}

OPTIONS_SET_EXPASSION(std::string)
OPTIONS_SET_EXPASSION(bool)
OPTIONS_SET_EXPASSION(int)
OPTIONS_SET_EXPASSION(float)

#undef OPTIONS_SET_EXPASSION

void init_options_map() {

    options_map["name"] << Utils::Option::setoption(PROGRAM);
    options_map["version"] << Utils::Option::setoption(VERSION);

    options_map["mode"] << Utils::Option::setoption(std::string{"ascii"});
    options_map["help"] << Utils::Option::setoption(false);

    options_map["quiet"] << Utils::Option::setoption(false);
    options_map["num_games"] << Utils::Option::setoption(1, 32, 1);
    options_map["reserve_movelist"] << Utils::Option::setoption(60);
}

void init_basic_parameters() {

    Zobrist::init_zobrist();
    Board::init_mask();
    init_options_map();
}


ArgsParser::ArgsParser(int argc, char** argv) {

    auto out = std::ostringstream{};
    for (int i = 0; i < argc; ++i) {
        out << argv[i] << " ";
    }

    auto parser = Utils::CommandParser(out.str());

    assert((int)parser.get_count() == argc);

    
    const auto is_parameter = [](const std::string &para) -> bool {
        if (para.empty()) {
            return false;
        }
        return para[0] != '-';
    };

    using List = std::vector<std::string>;

    const auto help = parser.find(List{"--help", "-h"});
    if (help) {
        set_option("help", true);
    }

    const auto mode = parser.find_next(List{"--mode", "-m"});
    if (is_parameter(mode->str)) {
        if (mode->str == "ascii" || mode->str == "ucci") {
            set_option("mode", mode->str);
        }
    }
  
}

void ArgsParser::help() const {
    Utils::auto_printf("Argumnet\n");
    Utils::auto_printf(" --help, -h\n");
    Utils::auto_printf(" --mode, -m [ascii/ucci]\n");  
}

void ArgsParser::dump() const {
    if (option<bool>("help")) {
        help();
    }
}

