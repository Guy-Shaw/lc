/*
 * Filename: decode-emsg.c
 * Library: libcscript
 * Brief: like strerror_r(), but more portable.
 *
 * Description:
 *   decode_emsg_r() is just a trivial wrapper for strerror_r()
 *   on systems that support it.  But, not all systems do support
 *   strerror_r().  Also, programs that use decode_emsg_r() do not
 *   have to
 *       #define _GNU_SOURCE 1
 *   so they do not have to worry about other changes in behavior
 *   that might come along with _GNU_SOURCE.
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

#if defined(__GNUC__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE 1
#endif

#include <unistd.h>	// Import size_t
#include <string.h>

/**
 * @brief Decode a numeric errno into a given buffer.
 * @param buf IN  Destination string used in case result is not static.
 * @param sz  IN  The capacity of the character buffer, |buf|.
 * @param err IN  The errno value to be decoded.
 * @return The string explaining the errno value.
 *
 * Why have our own variant of strerror_r() ?
 *
 *  1) strerror_r() is not all that portable accross *nix flavors.
 *
 *  2) Ensure that we have a function that always returns char *
 *
 *  3) decode_emsg_r() and decode_esym_r() have the exact same signature.
 *
 * decode_emsg_r() will _always_ return a pointer to a string,
 * so it can be used directly in expressions where a char * is needed.
 * For example, in printf-like functions (but only once).
 *
 * There is _never_ an error.  If the given errno value is invalid,
 * a descriptive string is still returned.  It just indicates the
 * error and the numeric value.
 *
 * Normally, the decoded errno is _not_ copied into |buf|.
 * In the case of known errno values, decode_emsg_r() returns a pointer
 * to a static string.  The destination buffer is used only in the case
 * of unknown errno values.   This is when a string value must be computed.
 * So, the returned pointer is always what should be used, not |buf|.
 *
 */

char *
decode_emsg_r(char *buf, size_t sz, int err)
{
    return (strerror_r(err, buf, sz));
}
