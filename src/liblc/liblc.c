/*
 * Filename: src/liblc/liblc.c
 * Project: lc
 * Brief: list directory contents by category
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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <dirent.h>
    // Import readdir()
#include <errno.h>
    // Import var errno
#include <stdbool.h>
    // Import constant true
#include <stddef.h>
    // Import constant NULL
#include <stdio.h>
    // Import type FILE
    // Import printf()
    // Import var stdout
#include <sys/types.h>
    // Import closedir()
    // Import opendir()
#include <string.h>
    // Import strdup()
#include <stdlib.h>
    // Import qsort()

#include <fcntl.h>	// AT_* constants
#include <sys/stat.h>
#include <unistd.h>

#include <cscript.h>
#include <cs-strv.h>

#include <curses.h>
#include <term.h>

#include <lc.h>

typedef size_t index_t;

extern int mc(FILE *f, size_t nelem, const char **elemv, size_t llen, size_t indent, bool horizontal);

extern int mode_to_ftype(int m);

static strv_t fnames_dir;
static strv_t fnames_file;
static strv_t fnames_symlink;
static strv_t fnames_uslink;
static strv_t fnames_pipe;
static strv_t fnames_socket;
static strv_t fnames_bdev;
static strv_t fnames_cdev;
static strv_t fnames_other;
static strv_t fnames_error;

struct category {
    strv_t *fnames;
    int     ftype;
    char   *label;
};

typedef struct category category_t;

static category_t category_table[] = {
    { &fnames_dir,     'd', "Directories" },
    { &fnames_file,    'f', "Files" },
    { &fnames_symlink, 'l', "Symbolic Links" },
    { &fnames_uslink,  'u', "Unresolved Symbolic Links" },
    { &fnames_pipe,    'p', "Named pipes (FIFO)" },
    { &fnames_socket,  's', "Sockets" },
    { &fnames_bdev,    'b', "Block special files" },
    { &fnames_cdev,    'c', "Character special files" },
    { &fnames_other,   '?', "Other" },
    { &fnames_error,   'E', "ERROR" }
};

static category_t *category_other;

static size_t
get_env_columns(void)
{
    char *env = getenv("COLUMNS");
    if (env == NULL) {
        return (0);
    }
    return (strtoul(env, NULL, 10));
}

static size_t
get_tput_columns(void)
{
    int ti_cols;
    int rv;
    int term_err;

    rv = setupterm(NULL, 1, &term_err);
    if (rv != OK) {
        return (0);
    }
    ti_cols = tigetnum("cols");
    if (ti_cols > 0) {
        return ((size_t)ti_cols);
    }
    return (0);
}

size_t
determine_line_length(size_t cols)
{
    if (cols != 0) {
        return (cols);
    }

    cols = get_env_columns();
    if (cols != 0) {
        return (cols);
    }

    cols = get_tput_columns();
    if (cols != 0) {
        return (cols);
    }

    return (80);
}

static void
init_category(category_t *cat)
{
    strv_t *sv = cat->fnames;
    strv_init(sv);
    sv->sv_grow = 100;
    sv->sv_fatal = true;
}

static void
init_categories(void)
{
    size_t n;
    index_t i;

    n = sizeof (category_table) / sizeof (category_table[0]);
    for (i = 0; i < n; ++i) {
        init_category(&category_table[i]);
        if (category_table[i].ftype == '?') {
            category_other = &category_table[i];
        }
    }

    if (category_other == NULL) {
        abort();
    }
}

static void
add_fname_to_category(const char *dir, int ftype, const char *sfn)
{
    category_t *catp;
    strv_t *sv;
    size_t n;
    index_t i;

    (void)dir;

    catp = category_other;

    n = sizeof (category_table) / sizeof (category_table[0]);
    for (i = 0; i < n; ++i) {
        if (category_table[i].ftype == ftype) {
            catp = &category_table[i];
        }
    }

    sv = catp->fnames;
    strv_alloc(sv, sv->strc + 1);
    sv->strv[sv->strc] = strdup(sfn);
    ++sv->strc;
}

static int
cmpstringp(const void *p1, const void *p2)
{
  /* The actual arguments to this function are "pointers to
   * pointers to char", but strcmp(3) arguments are "pointers
   * to char", hence the following cast plus dereference
   */

    return (strcmp(* (char * const *) p1, * (char * const *) p2));
}

static bool
int_to_bool(int truthiness)
{
    switch (truthiness) {
    case 0: return false;
    case 1: return true;
    default:
        fprintf(stderr, "INTERNAL ERROR: invalid boolean option, %d.\n", truthiness);
        abort();
    }
}

static void
print_indent(FILE *dstf, size_t cols)
{
    size_t i;

    for (i = 1; i < cols; ++i) {
        fputc(' ', dstf);
    }
}

static void
print_fnames(FILE *dstf, strv_t *sfnv, lc_options_t *lcopt)
{
    size_t l1_indent;
    size_t l2_indent;
    size_t l3_indent;
    int rv;

    qsort((void *)sfnv->strv, sfnv->strc, sizeof (char *), cmpstringp);
    if (lcopt->lc_cols == 0) {
        lcopt->lc_cols = determine_line_length(0);
    }

    l1_indent = lcopt->lc_showdirs ? lcopt->lc_indent_global : 0;
    l2_indent = l1_indent + lcopt->lc_indent_types;
    l3_indent = l2_indent + lcopt->lc_indent_files;
    rv = mc(dstf, sfnv->strc, (const char **)sfnv->strv, lcopt->lc_cols,
             l3_indent, int_to_bool(lcopt->lc_horizontal));
    if (rv) {
        eprintf("mc failed.\n");
    }
}

static void
print_all_categories(FILE *dstf, lc_options_t *lcopt)
{
    category_t *catp;
    strv_t *sv;
    size_t n;
    index_t i;
    size_t nprint;
    size_t l1_indent;
    size_t l2_indent;

    n = sizeof (category_table) / sizeof (category_table[0]);
    l1_indent = lcopt->lc_showdirs ? lcopt->lc_indent_global : 0;
    l2_indent = l1_indent + lcopt->lc_indent_types;
    nprint = 0;
    for (i = 0; i < n; ++i) {
        catp = &category_table[i];
        sv = catp->fnames;

        if (sv->strc != 0) {
            if (nprint != 0) {
                fputs("\n", dstf);
            }

            print_indent(dstf, l2_indent);
            fprintf(dstf, "%s:\n", catp->label);
            print_fnames(dstf, sv, lcopt);
            ++nprint;
        }
    }
}

int
parse_ftypes(const char *str)
{
    size_t n;
    index_t i;

    n = sizeof (category_table) / sizeof (category_table[0]);

    while (*str) {
        int ftype = *str;
        bool match = false;
        for (i = 0; i < n; ++i) {
            if (ftype == category_table[i].ftype) {
                match = true;
                break;
            }
        }
        if (!match) {
            return (-1);
        }
        ++str;
    }

    return (0);
}

static bool
ftype_is_member(int ftype, const char *ftypes)
{
    while (*ftypes) {
        if (ftype == *ftypes) {
            return (true);
        }
        ++ftypes;
    }

    return (false);
}

static int
scan_directory(const char *dir, lc_options_t *lcopt)
{
    const char *ftypes = lcopt->lc_ftypes;
    DIR *dp;
    int rv;

    dp = opendir(dir);
    if (dp == NULL) {
        return (errno);
    }

    while (true) {
        struct dirent *direntp;
        struct stat statb;
        const char *sfn;
        int ftype;
        int err;

        errno = 0;
        direntp = readdir(dp);
        if (direntp == NULL) {
            err = errno;
            if (err == 0) {
                break;
            }
            else {
                closedir(dp);
                return (err);
            }
        }
        sfn = direntp->d_name;
        dbg_printf("scan %s\n", sfn);

        if (*sfn == '.' && !lcopt->lc_all) {
            continue;
        }

        rv = fstatat(dirfd(dp), sfn, &statb, AT_SYMLINK_NOFOLLOW);
        if (rv) {
            err = errno;
            eprintf("fstatat(%s) failed.", sfn);
            eexplain_err(err);
        }
        else {
            ftype = mode_to_ftype(statb.st_mode);
            if (ftype == 'l') {
                rv = fstatat(dirfd(dp), sfn, &statb, 0);
                if (rv) {
                    err = errno;
                    if (err == ENOENT) {
                        ftype = 'u';
                    }
                    else {
                        ftype = 'E';
                    }
                }
            }
            dbg_printf("    ftype=[%c]\n", ftype);
            if (ftype == '-') {
                ftype = 'f';
            }
            if (ftypes == NULL || ftype_is_member(ftype, ftypes)) {
                add_fname_to_category(dir, ftype, sfn);
            }
        }
    }

    rv = closedir(dp);
    if (rv) {
        return (errno);
    }

    return (0);
}

int
lc_directory(const char *dir, lc_options_t *lcopt)
{
    FILE *dstf;
    int rv;

    dstf = stdout;
    init_categories();
    rv = scan_directory(dir, lcopt);
    if (rv) {
        return (rv);
    }

    if (lcopt->lc_showdirs) {
        print_indent(dstf, lcopt->lc_indent_global);
        fprintf(dstf, "%s:\n", dir);
    }

    print_all_categories(dstf, lcopt);
    return (0);
}
