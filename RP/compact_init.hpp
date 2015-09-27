/*
 * File: compact_init.hpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard.elias@matfyz.cz>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#ifndef COMPACT_INIT_HPP
#define COMPACT_INIT_HPP

#include "types.hpp"
#include "compact.hpp"

struct compact::init
{
public:
    init(
                rna_tree& _rna);

public:
    static void shift_branch(
                iterator parent,
                point vec);
    static void rotate_branch(
                iterator parent,
                double angle);

private:
    sibling_iterator get_prev(
                sibling_iterator it);
    sibling_iterator get_next(
                sibling_iterator it);
    point direction_vector_from(
                iterator it);

    point init_branch_recursive(
                sibling_iterator it,
                point from);

private:
    void run();

public:
    rna_tree &rna;
};



#endif /* !COMPACT_INIT_HPP */

