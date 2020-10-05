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
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <stdexcept>

#include "Utils.h"
#include "config.h"

namespace Utils {

void auto_printf(const char *fmt, ...) {

    if (option<bool>("quiet")) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
}

void auto_printf(std::ostringstream &out) {

    if (option<bool>("quiet")) {
        return;
    }

    std::cout << out.str();
}

void space_stream(std::ostream &out, const size_t times) {
    for (auto t = size_t{0}; t < times; ++t) {
        out << " ";
    }
}

void strip_stream(std::ostream &out, const size_t times) {
    for (auto t = size_t{0}; t < times; ++t) {
        out << std::endl;
    }
}

CommandParser::CommandParser(std::string input) {
    parser(input);
}

bool CommandParser::valid() const {
    return m_count != 0;
}

void CommandParser::parser(std::string &input) {

    m_count = 0;
    auto stream = std::istringstream{input};
    auto in = std::string{};
    while (stream >> in) {
        m_commands.emplace_back(std::make_shared<std::string>(in));
        m_count++;
    }
}

size_t CommandParser::get_count() const {
    return m_count;
}

std::optional<CommandParser::Reuslt> CommandParser::get_command(size_t id) const {

    if (!valid() || id > m_count) {
        return std::nullopt;
    }

    return std::optional<Reuslt>(Reuslt(*m_commands[id], (int)id));
}

std::optional<CommandParser::Reuslt> CommandParser::get_commands(size_t b) const {
    return get_slice(b, m_count);
}

std::optional<CommandParser::Reuslt> CommandParser::get_slice(size_t b, size_t e) const {

     if (!valid() || b >= m_count || e > m_count || b >= e) {
         return std::nullopt;
     }

     auto out = std::ostringstream{};
     auto begin = std::next(std::begin(m_commands), b);
     auto end = std::next(std::begin(m_commands), e);
     auto stop = std::prev(end, 1);

     if (begin != end) {
         std::for_each(begin, stop, [&](auto in)
                                        {  out << *in << " "; });
     }

     out << **stop;
     return std::optional<Reuslt>(Reuslt(out.str(), -1));
}


std::optional<CommandParser::Reuslt> CommandParser::find(const std::string input, int id) const {

    if (!valid()) {
        return std::nullopt;
    }

    if (id < 0) {
        for (auto i = size_t{0}; i < get_count(); ++i) {
            const auto res = get_command((size_t)i);
            if (res->str == input) {
                return res;
            }
        }
    } else {
        if (const auto res = get_command((size_t)id)) {
            return res->str == input ? res : std::nullopt;
        }
    }

    return std::nullopt;
}

std::optional<CommandParser::Reuslt> CommandParser::find(const std::vector<std::string> inputs, int id) const {

    for (const auto &in : inputs) {
        if (const auto res = find(in, id)) {
            return res;
        }
    }

    return std::nullopt;
}

std::optional<CommandParser::Reuslt> CommandParser::find_next(const std::string input) const {

    const auto res = find(input);

    if (!res || res->idx+1 > (int)get_count()) {
        return std::nullopt;
    }

    return get_command(res->idx+1);
}

std::optional<CommandParser::Reuslt> CommandParser::find_next(const std::vector<std::string> inputs) const {

    for (const auto &in : inputs) {
        if (const auto res = find_next(in)) {
            return res;
        }
    }

    return std::nullopt;
}

bool Option::boundary_valid() const {
    option_handle();
    return !(m_max == 0 && m_min == 0);
}

template<>
Option Option::setoption<std::string>(std::string val, int /*max*/, int /*min*/) {
    return Option{type::String, val, 0, 0};
}

template<>
Option Option::setoption<bool>(bool val, int /*max*/, int /*min*/) {

    if (val) {
      return Option{type::Bool, "true", 0, 0};
    }

    return Option{type::Bool, "false", 0, 0};
}

template<>
Option Option::setoption<int>(int val, int max, int min) {
    auto op = Option{type::Integer, std::to_string(val), max, min};
    op.adjust<int>();
    return op;
}

template<>
Option Option::setoption<float>(float val, int max, int min) {
    auto op = Option{type::Float, std::to_string(val), max, min};
    op.adjust<float>();
    return op;
}


#define OPTION_EXPASSION(T)    \
template<>                     \
T Option::get<T>() const {     \
    return (T)*this;           \
}

OPTION_EXPASSION(std::string)
OPTION_EXPASSION(bool)
OPTION_EXPASSION(float)
OPTION_EXPASSION(int)

#undef OPTION_EXPASSION

template<>
void Option::set<std::string>(std::string value) {
    option_handle();
    m_value = value;
}

template<>
void Option::set<bool>(bool value) {
    option_handle();
    if (value) {
        m_value = std::string{"true"};
    } else {
        m_value = std::string{"false"};
    }
}

template<>
void Option::set<int>(int value) {
    option_handle();
    m_value = std::to_string(value);
    adjust<int>();
}

template<>
void Option::set<float>(float value) {
    option_handle();
    m_value = std::to_string(value);
    adjust<float>();
}

void Option::option_handle() const {

    if (m_max < m_min) {
        auto out = std::ostringstream{};
        out << " In option :";
        out << " Max : " << m_max << " |";
        out << " Min : " << m_min << " |";
        out << " Minimal is bigger than maximal.";
        out << " It is not accepted.";
        throw std::runtime_error(out.str());
    }

    if (m_type == type::Invalid) {
        auto out = std::ostringstream{};
        out << " In option :";
        out << " Please initialize first.";
        throw std::runtime_error(out.str());
    }
};

} // namespace Utils
