/*
 * Filename: sname.c
 * Library: libcscript
 * Brief: simple filename - like basename but not as quirky
 *
 * Description:
 *   Given a Unix-style path, return the last component.
 *
 *   The pointer returned points to somewhere inside the given string.
 *   No memory is allocated, nothing is copied, and the return value
 *   never points to any other object.
 *
 *   The given path is never modified.
 *   sname() will never segfault.
 *   It is safe to pass a static string (immutable string)
 *   to sname().
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


/**
 * @brief Simple filename - like basename but not as quirky.
 *
 * @param path  IN  A filename which could have leading directory components.
 * @return The simple file name;  that is, the last component of |path|.
 *
 * Given a Unix-style path, return the last component.
 *
 * The pointer returned points to somewhere inside the given string.
 * No memory is allocated, nothing is copied, and the return value
 * never points to any other object.  The return value is never NULL,
 * unless the given |path| is NULL.
 *
 * The given |path| is never modified.
 * sname() will never segfault.
 * It is safe to pass a static string (immutable string) to sname().
 *
 */
const char *
sname(const char *path)
{
    const char *s;
    const char *p;
    int c, prevc;

    if (!path) {
        return (path);
    }

    s = path;
    p = path;
    prevc = 0;
    while ((c = *p) != 0) {
        if (prevc == '/') {
            s = p;
        }
        prevc = c;
        ++p;
    }
    return (s);
}
