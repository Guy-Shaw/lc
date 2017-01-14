/*
 * Filename: ls-dlh.c
 * Library: libcscript
 * Brief: Lists information about a given filename, like ls -dlh
 *
 * Description:
 *   Given a single filename, do the rough equivalent
 *   of this shell one-liner:
 *
 *     ls -dlh --time-style='+%Y-%m-%d %H:%M' $1
 *
 *   All of the code is internal.  The 'ls' command is not used.
 *   No child process is called.  Since there is only one file name,
 *   and only one fixed format, with no other options, the code is
 *   greatly simplified, compared to the full 'ls' command.
 *
 *   This is meant to make it cheap and easy (especially easy)
 *   to list information about a file, as part of an error report.
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

#include <grp.h>            // for group, getgrgid_r
#include <pwd.h>            // for passwd, getpwuid_r
#include <stddef.h>         // for size_t
#include <stdio.h>          // for fprintf, snprintf, FILE, etc
#include <string.h>         // for strcpy
#include <sys/stat.h>       // for stat, lstat, mode_t, etc
#include <sys/sysmacros.h>  // for major, minor
#include <time.h>           // for localtime, strftime

typedef unsigned long long uint64_t;

extern char *sisfx_r(char *buf, size_t sz, uint64_t n);
extern char *sisfx_dec_r(char *buf, size_t sz, uint64_t n);
extern int mode_to_ftype(int m);
extern void ls_strmode(mode_t mode, char *str);
extern void fshow_fname(FILE *f, const char *fname);

/**
 * @brief Lists information about a given filename, like ls -dlh
 * @param f      IN Write listing to this FILE.
 * @param fname  IN The name of the file to list (just one file name).
 * @param statp  IN Pointer to a |struct stat|, already populated by stat().
 * @return void
 *
 * If |statp| is NULL, then stat() is called to populate our own
 * internal |struct stat|.
 *
 * The ls-dlh family of functions are used almost exclusively for
 * assisting in the reporting of errors.  So, here, failure is not an
 * option.  In the face of problems, w just fill in some informative
 * WTF values for some of the fields in the listing output, and keep 
 * going.
 *
 */
void
flsdlh_stat(FILE *f, const char *fname, struct stat *statp)
{
    struct stat statb;
    char mode_str[10];
    struct passwd pwdbuf;
    struct passwd *rpwd;
    char pwdstrbuf[128];
    char owner_user_name[12];
    char owner_group_name[12];
    struct group  grpbuf;
    struct group  *rgroup;
    char grpstrbuf[128];
    char sizebuf[22];
    char fmt_time[18];
    int rv;
    struct tm *tmp;
    mode_t m;

    if (fname != NULL && statp == NULL) {
        statp = &statb;
        rv = lstat(fname, &statb);
        if (rv != 0) {
            fprintf(f, "? %s\n", fname);
            return;
        }
    }

    if (statp == NULL) {
        fprintf(f, "? <bad stat>\n");
        return;
    }

    strcpy(owner_user_name, "?");
    strcpy(owner_group_name, "?");
    rv = getpwuid_r(statp->st_uid, &pwdbuf, pwdstrbuf, sizeof (pwdstrbuf), &rpwd);
    if (rv == 0 && rpwd && rpwd->pw_name) {
        strcpy(owner_user_name, rpwd->pw_name);
    }

    rv = getgrgid_r(statp->st_gid, &grpbuf, grpstrbuf, sizeof (grpstrbuf), &rgroup);
    if (rv == 0 && rgroup && rgroup->gr_name) {
        strcpy(owner_group_name, rgroup->gr_name);
    }

    tmp = localtime(&statp->st_mtime);
    if (tmp) {
        size_t rsize;

        rsize = strftime(fmt_time, sizeof (fmt_time), "%Y-%m-%d %H:%M", tmp);
        if (rsize == 0) {
            strcpy(fmt_time, "bad_strftime");
        }
    }
    else {
        // Do not complain.  Just fill in some WTF value.
        strcpy(fmt_time, "yyyy-mm-dd hh:mm");
    }

    m = statp->st_mode;
    ls_strmode(m, mode_str);

    if (S_ISCHR(m) || S_ISBLK(m)) {
        snprintf(sizebuf, sizeof (sizebuf), "%u, %u",
            major(statp->st_rdev),
            minor(statp->st_rdev));
    }
    else {
        sisfx_r(sizebuf, sizeof (sizebuf), (uint64_t)statp->st_size);
    }

    fprintf(f, "%c%s %lu %s %s %6s %s",
        mode_to_ftype(statp->st_mode),
        mode_str,
        statp->st_nlink,
        owner_user_name,
        owner_group_name,
        sizebuf,
        fmt_time);

    if (fname != NULL) {
        fprintf(f, " ");
        fshow_fname(f, fname);
    }
    fprintf(f, "\n");
}

/**
 * @brief Lists information about a given filename, like ls -dlh
 * @param f      IN Write listing to this FILE.
 * @param fname  IN The name of the file to list (just one file name).
 * @return void
 *
 * The ls-dlh family of functions are used almost exclusively for
 * assisting in the reporting of errors.  So, here, failure is not an
 * option.  In the face of problems, w just fill in some informative
 * WTF values for some of the fields in the listing output, and keep 
 * going.
 *
 */
void
flsdlh(FILE *f, const char *fname)
{
    flsdlh_stat(f, fname, NULL);
}

/**
 * @brief Lists information about a given filename to |stdout|.
 * @param fname  IN The name of the file to list (just one file name).
 * @return void
 *
 */
void
lsdlh(const char *fname)
{
    flsdlh(stdout, fname);
}

/**
 * @brief Lists information about a given filename to |stderr|.
 * @param fname  IN The name of the file to list (just one file name).
 * @return void
 *
 */
void elsdlh(const char *fname)
{
    flsdlh(stderr, fname);
}
