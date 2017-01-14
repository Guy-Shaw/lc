/*
 * Filename: cs-strv.h
 * Brief: flexible sized vector of strings
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
#include <unistd.h>

/*
 * strv:
 *     The base address of an array of strings (like argv)
 *
 * strc:
 *     The count of how many strings there are (currently) in strv.
 *     (like argc)
 *
 * sv_capacity:
 *     How much space has been allocated for v,
 *     A count of how many elements, not space in bytes.
 *
 * sv_grow:
 *     Amount to grow capacity (number of elements),
 *     as needed, until sv_limit is reached.
 *
 * sv_limit:
 *     How big is v allowed to get?
 *     A count of how many elements, not space in bytes.
 *     Any attempt to grow v bigger than this limit is an error.
 *     0 means unlimited.
 *
 * sv_err:
 *     An errno-like status.  0 is no error.
 *
 * sv_fatal:
 *     If true, then any errors are are fatal.
 *     That is, strv*() family of functions will,
 *     cause a fatal error, instead of just returning
 *     to caller with an error status.
 *
 */

struct strv {
    char ** strv;
    size_t  strc;
    size_t  sv_capacity;
    size_t  sv_grow;
    size_t  sv_limit;
    int     sv_err;
    bool    sv_fatal;
};

typedef struct strv strv_t;

// Debug support functions
//
extern char *decode_addr_r(char *buf, size_t sz, void *addr);
extern char *decode_addr(void *addr);
extern const char *decode_bool(bool predicate);
extern void f_print_strv(FILE *f, strv_t *sv);
extern void dbg_print_strv(strv_t *sv);

extern void strv_init(strv_t *sv);
extern void strv_fatal_nomem(void);
extern int  strv_grow(strv_t *sv, size_t n);
extern void strv_alloc(strv_t *sv, size_t n);
extern void strv_free_strings(strv_t *sv);
extern void strv_free(strv_t *sv);

extern strv_t strv_null;
