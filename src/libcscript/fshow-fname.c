/*
 * Filename: fshow-fname.c
 * Library: libcscript
 * Brief: write a representation of a filename with all graphic characters
 *
 * Description:
 *   Write a "safe" representation of a given filename to an stdio stream,
 *   taking care to render all graphic characters, so that nothing is
 *   hidden and nothing can make a terminal misbehave.
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
 * @brief Write a representation of a filename with all graphic characters
 * @param f      IN Write to this FILE
 * @param fname  IN  The filename to be represented.
 * @return void
 *
 * For now, this is the same as fshow_str(), the generic function
 * to show the contents of any string with all visible characters.
 * But, there could be some difference in the future, because
 * of some behavior specialized for filenames.
 */

void
fshow_fname(FILE *f, const char *fname)
{
    char dbuf[8];
    const char *s;
    int chr;

    for (s = fname; (chr = *s) != '\0'; ++s) {
        show_char_r(dbuf, sizeof (dbuf), chr);
        fputs(dbuf, f);
    }
}
