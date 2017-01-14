/*
 * Filename: strv.c
 * Library: libcscript
 * Brief: Functions to support an arbirary size vector of strings
 *
 * Description:
 *   Functions to create, grow, and destroy
 *   an arbirary size vector of strings.
 *
 *   The data type, 'strv_t', keep track of the current capacity
 *   and size of the array of pointers.  When a larger size is
 *   requested, the allocation is taken from the existing capacity,
 *   if there is room.  It that case, it is a simple adjustment to
 *   the accoounting for size.  But, if there is no more cpacity,
 *   then the array of pointers is reallocated by some chunk size.
 *   Note that the conent of the strings themselves are accounted
 *   for separately.
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
#include <string.h>     // Import strdup()
#include <errno.h>      // Import ENOMEM

strv_t strv_null;

void
strv_init(strv_t *sv)
{
    *sv = strv_null;
}

void
strv_fatal_nomem(void)
{
    eprint("Fatal error: strv -- out of memory.\n");
}

int
strv_grow(strv_t *sv, size_t n)
{
    char **new_strv;
    size_t new_capacity;

    if (sv->sv_limit != 0 && sv->sv_capacity + n >= sv->sv_limit) {
        if (sv->sv_fatal) {
            strv_fatal_nomem();
        }
        return (ENOMEM);
    }

    new_capacity = sv->sv_capacity + n;
    new_strv = (char **)realloc(sv->strv, new_capacity * sizeof (char *));
    if (new_strv == NULL) {
        if (sv->sv_fatal) {
            strv_fatal_nomem();
        }
        return (ENOMEM);
    }

    sv->strv = new_strv;
    sv->sv_capacity = new_capacity;
    return (0);
}

/*
 * Make sure there is enough room for |n| elements to be appended to
 * the given strv_t.  Add capacity, if necessary.
 * Adjust argc to reflect the new size.
 */
void
strv_alloc(strv_t *sv, size_t n)
{
    dbg_printf("strv_alloc: before.\n");
    dbg_print_strv(sv);

    if (sv->strv == NULL || sv->strc + n > sv->sv_capacity) {
        size_t t;
        size_t g;

        g = sv->sv_grow;
        if (g == 0) {
            g = 1;
        }
        t = ((n + g - 1) / g) * g;
        strv_grow(sv, t);
    }
}

void
strv_free_strings(strv_t *sv)
{
    if (sv == NULL) {
        return;
    }

    if (sv->strv == NULL) {
        return;
    }

    dbg_printf("strv_free_strings: before.\n");
    dbg_print_strv(sv);

    for (size_t i = 0; i < sv->strc; ++i) {
        char *s = sv->strv[i];
        if (s) {
            free(s);
        }
    }
}

void
strv_free(strv_t *sv)
{
    if (sv->strv != NULL) {
        free(sv->strv);
    }
}
