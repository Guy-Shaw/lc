/*
 * Filename: sisfx.c
 * Library: libcscript
 * Brief: decode a number as simple number with an SI suffix
 *
 * Description:
 *   Decode a number as a simple number less than 1024 (or 1000),
 *   followed by an SI suffix ( K M G T P E Z ).  If the number
 *   is less than 1024 (or 1000) then it is unchanged.  For numbers
 *   greater than 1024 (or 1000) the result is range reduced,
 *   by integer division, and the appropriate suffix is appended.
 *   If there is any remainder worth mentioning, then the numeric
 *   part of the result is shown to the nearest 1/10th, otherwise
 *   it is a simple unsigned integer.
 *
 *   For example, 1774 is 1.8K but 4096 is just 4K, not 4.0K
 *
 *   There are two variants:
 *     decimal, where range reduction is done in units of 10^3 (1000)  and
 *     binary,  where range reduction is done in units of 2^10 (1024)
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
    // Import snprintf()

typedef unsigned long long uint64_t;
typedef unsigned int uint_t;

static char sfx[] = { 0, 'K', 'M', 'G', 'T', 'P', 'E', 'Z' };

/**
 * @brief Decode a number as simple number with an SI suffix.
 * @param res  OUT  A string buffer to hold the result.
 * @param sz   IN   The capacity of |buf|.
 * @param nn   IN   A 64-bit number to be decoded.
 * @result The result -- always points to |buf|.
 *
 * Decode a number as a simple number less than 1024 (or 1000),
 * followed by an SI suffix ( K M G T P E Z ).  If the number
 * is less than 1024 (or 1000) then it is unchanged.  For numbers
 * greater than 1024 (or 1000) the result is range reduced,
 * by integer division, and the appropriate suffix is appended.
 * If there is any remainder worth mentioning, then the numeric
 * part of the result is shown to the nearest 1/10th, otherwise
 * it is a simple unsigned integer.
 *
 * For example, 1774 is 1.8K but 4096 is just 4K, not 4.0K
 *
 * There are two variants:
 *   decimal, where range reduction is done in units of 10^3 (1000)  and
 *   binary,  where range reduction is done in units of 2^10 (1024)
 *
 */

char *
sisfx_r(char *res, size_t sz, uint64_t nn)
{
    uint_t n;
    uint_t rem;
    uint_t mag;

    mag = 0;
    while (nn >= __INT_MAX__) {
        ++mag;
        nn /= 1024;
    }

    n = (uint_t)nn;
    rem = 0;
    while (n >= 1023) {
        ++mag;
        rem = n;
        n /= 1024;
    }

    if (mag > 0) {
        rem = rem % 1024;
    }

    if (mag > 0) {
        if (rem > 102) {
            snprintf(res, sz, "%u.%u%c", n, rem / 102, sfx[mag]);
        }
        else {
            snprintf(res, sz, "%u%c", n, sfx[mag]);
        }
    }
    else {
        snprintf(res, sz, "%u", n);
    }

    return (res);
}

char *
sisfx_dec_r(char *res, size_t sz, uint64_t nn)
{
    uint_t n;
    uint_t rem;
    uint_t mag;

    mag = 0;
    while (nn >= __INT_MAX__) {
        ++mag;
        nn /= 1000;
    }

    n = (uint_t)nn;
    rem = 0;
    while (n >= 999) {
        ++mag;
        rem = n;
        n /= 1000;
    }

    if (mag > 0) {
        rem = rem % 1000;
    }

    if (mag > 0) {
        if (rem > 100) {
            snprintf(res, sz, "%u.%u%c", n, rem / 100, sfx[mag]);
        }
        else {
            snprintf(res, sz, "%u%c", n, sfx[mag]);
        }
    }
    else {
        snprintf(res, sz, "%u", n);
    }

    return (res);
}
