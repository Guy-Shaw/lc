/*
 * Filename: fshow-str-array.c
 * Library: libcscript
 * Brief: Show all the elements of an array of strings, numbered, one per line
 *
 * Description:
 *   Given an array of strings, write each string to the given stdio stream,
 *   one string per line, preceded by its array index.
 *   Each string is written using only "safe" graphic characters,
 *   so that nothing is hidden and nothing can make a terminal misbehave.
 *
 *   The width allocated to show the array indices is just enough to
 *   make the numbers line up properly (right-justified).
 *
 * Copyright (C) 2015-2016 Guy Shaw
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

#include <stdio.h>
#include <string.h>     // Import strlen()
#include <cscript.h>    // Import fshow_str()

/*
 * @brief Show an argv-like array of strings on the given stdio stream.
 *
 * @param f  in stream
 * @param ac in array.count
 * @param av in array.vector<string>
 * @return void
 *
 * Each string is preceded by a number (0-origin).
 * Each string is surrounded by brackets, in order to prevent problems
 * with missing leading or trailing whitespace.
 * Each string is shown using the visible character representation.
 *
 */
void
fshow_str_array(FILE *f, size_t ac, char *const *av)
{
    char nfmt[20];      // Big enough for __INT64_MAX__ 9223372036854775807LL
    uint_t maxw;
    uint_t i;

    sprintf(nfmt, "%zu", ac);
    maxw = strlen(nfmt);
    sprintf(nfmt, "%%%uu", maxw);
    for (i = 0; i < ac; ++i) {
        fprintf(f, nfmt, i);
        fprintf(f, ") [");
        fshow_str(f, av[i]);
        fprintf(f, "]\n");
    }
}
