/*
 * Filename: strv-debug.c
 * Library: libcscript
 * Brief: functions to pretty-print data types related to string vectors
 *
 * Description:
 *   Debugger helper functions to pretty-print the data type, 'strv_t',
 *   and its components.
 *
 * Copyright (C) 2016 Guy Shaw
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

#include <cscript.h>
#include <cs-strv.h>
#include <unistd.h>
#include <errno.h>      // Import ENOMEM

char *
decode_addr_r(char *buf, size_t sz, void *addr)
{
    snprintf(buf, sz, "%p", addr);
    return (buf);
}

char *
decode_addr(void *addr)
{
    static char abuf[32];
    return (decode_addr_r(abuf, sizeof (abuf), addr));
}

const char *
decode_bool(bool predicate)
{
    return (predicate ? "true" : "false");
}

void
f_print_strv(FILE *f, strv_t *sv)
{
    fprintf(f, "strv_t {\n");
    fprintf(f, "    strv = %s\n", decode_addr(sv->strv));
    fprintf(f, "    strc = %zu\n", sv->strc);
    if (sv->strv != NULL && sv->strc != 0) {
        size_t i;
        for (i = 0; i < sv->strc; ++i) {
            char *s = sv->strv[i];
            if (s) {
                fprintf(f, "        \"%s\"\n", s);
            }
            else {
                fprintf(f, "        NULL\n");
            }
        }
    }
    fprintf(f, "    sv_capacity = %zu\n", sv->sv_capacity);
    fprintf(f, "    sv_grow     = %zu\n", sv->sv_grow);
    fprintf(f, "    sv_limit    = %zu\n", sv->sv_limit);
    fprintf(f, "    sv_err      = %d\n", sv->sv_err);
    fprintf(f, "    sv_fatal    = %s\n", decode_bool(sv->sv_fatal));
    fprintf(f, "}\n");
}

void
dbg_print_strv(strv_t *sv)
{
    if (debug) {
        f_print_strv(dprint_fh, sv);
    }
}
