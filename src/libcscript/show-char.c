/*
 * Filename: show-char.c
 * Library: libcscript
 * Brief: Write a graphic representation of a single char to an stdio stream
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
#include <ctype.h>
#include <sys/types.h>
#include <cscript.h>

/**
 * @brief Representation of a single character using all grpahic characters.
 * @param buf  OUT  Space to hold the representation of |chr|.
 * @param sz   IN   The capacity of |buf|.
 * @param chr  IN   The character to represent.
 * @return The size of the resulting representation, in |buf| (strlen()).
 *
 * Show a single character.  That is, given a single character,
 * return a character string that is a representation of that character
 * consisting of all graphic characters, converting to escape sequences
 * where necessary.
 *
 * The caller supplies the adrress of a buffer (buffer address and size)
 * large enough to hold the largest possible escape sequence.
 *
 * A NUL byte is appended to the buffer, if there is room.
 * So, the resulting string can be used as either a string slice
 * or as a zstring.
 *
 * Returns the length of the string, not counting any NUL byte at the end.
 */

size_t
show_char_r(char *buf, size_t sz, int chr)
{
    size_t len;

    if (!(sz >= 5)) {
        eprintf("%s: buffer size is %zu.  It must be >= 5.\n",
            __FUNCTION__, sz);
        abort();
    }

    if (isprint(chr)) {
        len = 0;
        buf[len++] = chr;
        buf[len] = '\0';
    }
    else {
        len = sprintf(buf, "\\x%02x", chr);
    }
    return (len);
}
