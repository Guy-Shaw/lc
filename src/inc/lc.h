/*
 * Filename: lc.h
 * Brief: Interface for liblc
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

#ifndef LC_H
#define LC_H 1

struct lc_options {
    size_t lc_cols;
    size_t lc_indent_global;
    size_t lc_indent_types;
    size_t lc_indent_files;
    char  *lc_ftypes;
    int    lc_showdirs;
    int    lc_all;
    int    lc_horizontal;
};

typedef struct lc_options lc_options_t;

extern int parse_ftypes(const char *ftypes);
extern void lc_set_line_length(size_t);
extern int lc_directory(const char *dir, lc_options_t *lcopt);

#endif /* LC_H */
