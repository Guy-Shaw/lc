/*
 * Filename: set-print-fh.c
 * Library: libcscript
 * Brief: Setup the stdio file handles for error messages and debug messages
 *
 * Description:
 *   libcscript uses separate stdio file handles for printing error
 *   messages and for printing debug messages.  They can be setup
 *   in some custom way.  But, the default is to check to see if
 *   stderr (fd 2) is directed to the same underlying device as
 *   stdout (fd 1), and if so, redirect both errprint_fh and dbgprint_fh
 *   to stdout; but if they are different, then redirect to stderr.
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

#include <fcntl.h>
    // Import fstat()
#include <stddef.h>
    // Import constant NULL
#include <stdio.h>
    // Import fopen()
    // Import stderr()
    // Import stdout()
    // Import type FILE
    // Import var stderr
    // Import var stdout
#include <stdlib.h>
    // Import getenv()
#include <string.h>
    // Import memcmp()
#include <sys/stat.h>
    // Import fstat()
#include <sys/types.h>
    // Import fstat()
#include <unistd.h>
    // Import fstat()

extern char *program_name;
extern FILE *errprint_fh;
extern FILE *dbgprint_fh;

struct fid {
    dev_t dev;
    ino_t ino;
};

/*
 * Compare the dev+ino of stdout and stderr
 *
 * Return:
 *     0  stderr (fd2) is the same dev+ino as stdout (fd1).
 *     1  stderr (fd2) is redirected to some place other than stdout (fd1).
 *    -1  some error occurred while trying to stat fd1 or fd2. 
 */
int
stderr_redirected(void)
{
    struct stat statbuf;
    struct fid stdout_id;
    struct fid stderr_id;
    int rv;

    rv = fstat(1, &statbuf);
    if (rv != 0) {
        return (-1);
    }
    stdout_id.dev = statbuf.st_dev;
    stdout_id.ino = statbuf.st_ino;

    rv = fstat(2, &statbuf);
    if (rv != 0) {
        return (-1);
    }
    stderr_id.dev = statbuf.st_dev;
    stderr_id.ino = statbuf.st_ino;

    rv = memcmp(&stderr_id, &stdout_id, sizeof (struct fid));
    rv = (rv != 0);
    return (rv);
}

/**
 * @brief Standard setup for the file handle, |errprint_fh|.
 *
 * The file handle, |errprint_fh|, is set  depending on whether
 * |stdout| and |stderr| are directed to the same underlying file.
 *
 */

void
set_eprint_fh(void)
{
    int rv;
    FILE *efh;

    rv = stderr_redirected();
    efh = (rv == 0) ? stdout : stderr;

    if (errprint_fh == NULL) {
        errprint_fh = efh;
    }
}

/**
 * @brief Setup the file handle, |dbgprint_fh|.
 *
 */

void
set_debug_fh(const char *dbg_fname)
{
    char dbg_envar[32];

    if (dbg_fname == NULL) {
        dbgprint_fh = NULL;
        return;
    }

    if (dbg_fname[0] != '\0') {
        dbgprint_fh = fopen(dbg_fname, "w");
        return;
    }

    snprintf(dbg_envar, sizeof (dbg_envar), "DEBUG.%s", program_name);
    dbg_fname = getenv(dbg_envar);
    if (dbg_fname) {
        dbgprint_fh = fopen(dbg_fname, "w");
        if (dbgprint_fh) {
            return;
        }
    }

    dbgprint_fh = fopen("/proc/fd/self/3", "w");
    if (dbgprint_fh) {
        return;
    }

    dbgprint_fh = errprint_fh;
    if (dbgprint_fh) {
        return;
    }
    dbgprint_fh = stderr;
}
