/*
 * File name: mode-to-ftype.c
 * Library: libcscript
 * Brief: Translate a mode_t (as in st_mode) to a single-letter file type.
 *
 * Generated from the template file: mode-to-ftype-template.c
 * Generated on 2016-03-19 16:20:22
 *
 * Description:
 *   Template for the C library function, 'mode_to_ftype()',
 *   which translates an st_mode into a human-readable, single-letter
 *   file type like that produced by the 'ls' command.
 *   The function is constructed using the information generated
 *   by the program, 'gen-ftype'.
 *
 * Copyright (C) 2014-2016 Guy Shaw
 * Written by Guy Shaw <gshaw@acm.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

const char *ftype_table = "?pc?d?b?-?l?s???";

/**
 * @brief Translate a mode_t (as in st_mode) to a single-letter file type.
 * @param m  IN  A file mode to be decoded.
 * @return A single-letter file type.
 *
 * Note that |mode| is of type |int|, not |mode_t|.
 * The idea is that mode_to_ftype() can be called without bringing in
 * an include file just to define |mode_t|.
 *
 */
int
mode_to_ftype(int m)
{
    unsigned int pos = (m & 0xf000) >> 12;
    return (ftype_table[pos]);
}
