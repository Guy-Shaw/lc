/*
 * Filename: explain-err.c
 * Library: libcscript
 * Brief: like perror(), but shows errno all 3 ways: number, symbol, description
 *
 * Description:
 *   fexplain_err() provides a standard way to report when a system call
 *   or library function fails, with an applicable value for errno.
 *   It makes it easy and consitent to show the value of errno in all
 *   three ways:
 *     1) as a number
 *     2) as the symbolic value, according to errno.h
 *     3) as the description, according to information
 *        extracted from errno.h comments
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

#include <stdio.h>
    // Import fprintf()
    // Import type FILE
    // Import var stdout

extern char *decode_esym_r(char *buf, size_t bufsz, int err);
extern char *decode_emsg_r(char *buf, size_t bufsz, int err);

extern FILE *errprint_fh;

/**
 * @brief Explain an errno value in 3 ways: number, symbol, description.
 * @param f   IN  Write explanation to this FILE.
 * @param err IN  The errno value to be explained.
 * @return void
 *
 * Here, failure is not an option.
 * There is no provision for returning status.
 * Something has already gone wrong, and we are in the process of explaining
 * a failure.  If things are messed up so badly that we cannot succeed in
 * writing an explnation, then there is no sense whatever in trying to
 * explain our failure to explain.
 *
 */
void
fexplain_err(FILE *f, int err)
{
    char estrbuf[100];
    char *emsgp;
    char esymbuf[32];
    char *esymp;

    emsgp = decode_emsg_r(estrbuf, sizeof (estrbuf), err);
    esymp = decode_esym_r(esymbuf, sizeof (esymbuf), err);
    fprintf(f, "    %d=%s='%s'\n", err, esymp, emsgp);
}

/**
 * @brief Explain the errno value, |err|, to stdout.
 * @param err  IN  The errno value to be explained.
 * @return void
 *
 */
void
explain_err(int err)
{
    fexplain_err(stdout, err);
}

/**
 * @brief Explain the errno value, |err|, to the FILE errprint_fh.
 * @param err  IN  The errno value to be explained.
 * @return void
 *
 */
void
eexplain_err(int err)
{
    fexplain_err(errprint_fh, err);
}
