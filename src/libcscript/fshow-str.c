/*
 * Filename: fshow-str.c
 * Library: libcscript
 * Brief: Write a graphic representation of a string to an stdio stream
 *
 * Description:
 *   Given a single string, write it to the given stdio stream,
 *   using only "safe" graphic characters, so that nothing is hidden
 *   and nothing can make a terminal misbehave.
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
#include <sys/types.h>

extern size_t show_char_r(char *buf, size_t sz, int chr);

/**
 * @brief Write a graphic representation of a string to an stdio stream
 * @param f    IN  Write to this FILE.
 * @param str  IN  The string to be represented.
 * @return void
 *
 * Description:
 *   Given a single string, write it to the given stdio stream,
 *   using only "safe" graphic characters, so that nothing is hidden
 *   and nothing can make a terminal misbehave.
 */
void
fshow_str(FILE *f, char *str)
{
    char dbuf[8];
    char *s;
    int chr;

    for (s = str; (chr = *s) != '\0'; ++s) {
        show_char_r(dbuf, sizeof (dbuf), chr);
        fputs(dbuf, f);
    }
}
