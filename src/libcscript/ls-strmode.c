
/*
 * Filename: ls-strmode.c
 * Library: libcscript
 * Brief: Decode permission and set*id bits of a file mode.
 *
 * Description:
 *   Given a file mode (as in stat st_mode), decode the permissions,
 *   setuid and setgid.  This function does nothing with the file type,
 *   which is handled by the libcscript function, mode_to_ftype().
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

#include <sys/stat.h>   // for S_IXGRP, S_IXOTH, S_IXUSR, etc

// ########## Taken from GNU coreutils-8.23/lib/filemode.c
//
// Responsible ONLY for the 9 characters of the mode,
// not for file type or spacing.
//
// Like filemodestring, but rely only on MODE.


/**
 * @brief Decode a given mode as a 9-character symbolic value, a la ls -l.
 * @param mode  IN   A file mode (as in |struct stat| st_mode) to be decoded.
 * @param str   OUT  The 9 character nul-terminated decoded mode.
 * @return void
 *
 */
void
ls_strmode(mode_t mode, char *str)
{
    str[0] = mode & S_IRUSR ? 'r' : '-';
    str[1] = mode & S_IWUSR ? 'w' : '-';
    str[2] = (mode & S_ISUID
              ? (mode & S_IXUSR ? 's' : 'S')
              : (mode & S_IXUSR ? 'x' : '-'));

    str[3] = mode & S_IRGRP ? 'r' : '-';
    str[4] = mode & S_IWGRP ? 'w' : '-';
    str[5] = (mode & S_ISGID
              ? (mode & S_IXGRP ? 's' : 'S')
              : (mode & S_IXGRP ? 'x' : '-'));

    str[6] = mode & S_IROTH ? 'r' : '-';
    str[7] = mode & S_IWOTH ? 'w' : '-';
    str[8] = (mode & S_ISVTX
              ? (mode & S_IXOTH ? 't' : 'T')
              : (mode & S_IXOTH ? 'x' : '-'));
    str[9] = '\0';
}
