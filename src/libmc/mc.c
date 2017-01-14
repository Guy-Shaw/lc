/*
 * Filename: src/libmc/libmc.c
 * Project: libmc
 * Brief: Library to implement the heart of Multi-column Markup Language
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

#include <stdbool.h>
    // Import type bool
    // Import constant false
    // Import constant true
#include <stdio.h>
    // Import type FILE
    // Import fprintf()
    // Import fputc()
    // Import printf()
    // Import putchar()
    // Import var stderr
#include <stdlib.h>
    // Import exit()
    // Import malloc()
    // Import realloc()
#include <string.h>
    // Import strlen()
#include <unistd.h>
    // Import type size_t

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/*
 * The heart of the program logic to calculate how many columns can
 * fit and the width of each column is lifted from the GNU coreutils
 * program, ls.c.
 *
 * But, here the elements are not necessarily filenames.
 * They are arbitrary strings.
 *
 * Also, they are assumed to already be sorted in whatever order
 * they are supposed to be in.
 *
 */

/*
 * The information we need to print a list of elements is all in a
 * structure, 'mc_t'.
 *
 * nelem:
 *     How many elements in |elemv|
 *
 * elemv:
 *     An array of character strings.
 *     We do not modify them.
 *
 * llen:
 *     Line length.  Or, looked at another way, display width.
 *     It is given to us.  It is up to the caller to figure out
 *     what it should be, based on tty properties, environment variables,
 *     options, whatever.
 *
 * indent:
 *     Indentation of each line.  Number of spaces.
 *
 * f:
 *     The stdio FILE handle to print to.
 *
 *
 * The following fields are calculated, and stored in the mc structure,
 * so that there are no static variables.
 *
 * column_info:
 *     Array with information about column filledness
 *
 * column_info_alloc:
 *     Currently allocated columns in column_info
 *
 * max_idx:
 *     Maximum number of columns ever possible for this display.
 *
 */

struct mc_s {
    size_t nelem;
    const char **elemv;
    size_t llen;
    size_t indent;
    FILE *f;

    struct column_info *column_info;
    size_t column_info_alloc;
    size_t max_idx;
};

typedef struct mc_s mc_t;


/*
 * Fake the xmalloc functions we use.
 */

void *
xnmalloc(size_t n, size_t s)
{
  return malloc(n * s);
}

void *
xnrealloc(void *p, size_t n, size_t s)
{
    return realloc(p, n * s);
}

void
xalloc_die(void)
{
    fprintf(stderr, "xalloc_die.\n");
    exit(64);
}

static void init_column_info(mc_t *mc);
static size_t calculate_columns(mc_t *mc, bool by_columns);

static void print_many_per_line(mc_t *mc);
static void print_horizontal(mc_t *mc);

/*
 * The minimum width of a column is 3:
 *     1 character for the name, and
 *     2 for the separating white space.
 */
#define MIN_COLUMN_WIDTH 3

/* Information about filling a column.  */

struct column_info {
    bool valid_len;
    size_t line_len;
    size_t *col_arr;
};


/*
 * Allocate enough column info suitable for the current number of
 * elements and display columns, and initialize the info to represent the
 * narrowest possible columns.
 */

static void
init_column_info(mc_t *mc)
{
  size_t i;
  size_t max_cols;

  max_cols = MIN(mc->max_idx, mc->nelem);
  if (mc->column_info_alloc < max_cols) {
      size_t new_column_info_alloc;
      size_t *p;

      if (max_cols < mc->max_idx / 2) {
          /*
           * The number of columns is far less than the display width allows.
           * Grow the allocation, but only so that it's double the current
           * requirements.  If the display is extremely wide, this avoids
           * allocating a lot of memory that is never needed.
           */
          mc->column_info = (struct column_info *) xnrealloc(mc->column_info, max_cols, 2 * sizeof (*mc->column_info));
          new_column_info_alloc = 2 * max_cols;
        }
      else {
          mc->column_info = (struct column_info *) xnrealloc (mc->column_info, mc->max_idx, sizeof (*mc->column_info));
          new_column_info_alloc = mc->max_idx;
        }

      /*
       * Allocate the new size_t objects by computing the triangle
       * formula n * (n + 1) / 2, except that we don't need to
       * allocate the part of the triangle that we've already
       * allocated.  Check for address arithmetic overflow.
       */

      {
        size_t column_info_growth = new_column_info_alloc - mc->column_info_alloc;
        size_t s = mc->column_info_alloc + 1 + new_column_info_alloc;
        size_t t = s * column_info_growth;
        if (s < new_column_info_alloc || t / column_info_growth != s)
          xalloc_die ();
        p = (size_t *) xnmalloc(t / 2, sizeof (*p));
      }

      /* Grow the triangle by parceling out the cells just allocated.  */
      for (i = mc->column_info_alloc; i < new_column_info_alloc; ++i) {
          mc->column_info[i].col_arr = p;
          p += i + 1;
        }

      mc->column_info_alloc = new_column_info_alloc;
    }

  for (i = 0; i < max_cols; ++i) {
      size_t j;

      mc->column_info[i].valid_len = true;
      mc->column_info[i].line_len = (i + 1) * MIN_COLUMN_WIDTH;
      for (j = 0; j <= i; ++j) {
        mc->column_info[i].col_arr[j] = MIN_COLUMN_WIDTH;
      }
    }
}


/*
 * Calculate the number of columns needed to represent
 * the current set of elements in the current display width.
 */

static size_t
calculate_columns(mc_t *mc, bool by_columns)
{
    size_t enr;      /* Index into elements.  */
    size_t cols;     /* Number of elements across.  */

    /*
     * Normally the maximum number of columns is determined by the screen width.
     * But if few files are available this might limit it as well.
     */
    size_t max_cols = MIN(mc->max_idx, mc->nelem);

    init_column_info(mc);

    /* Compute the maximum number of possible columns.  */
    for (enr = 0; enr < mc->nelem; ++enr) {
        const char *elem = mc->elemv[enr];
        size_t elem_length = strlen(elem);
        size_t i;

        for (i = 0; i < max_cols; ++i) {
            if (mc->column_info[i].valid_len) {
                size_t idx = (by_columns
                ? enr / ((mc->nelem + i) / (i + 1))
                : enr % (i + 1));
                size_t real_length = elem_length + (idx == i ? 0 : 2);

                if (mc->column_info[i].col_arr[idx] < real_length) {
                    mc->column_info[i].line_len += (real_length
                    - mc->column_info[i].col_arr[idx]);
                    mc->column_info[i].col_arr[idx] = real_length;
                    mc->column_info[i].valid_len = (mc->column_info[i].line_len < mc->llen - mc->indent);
                }
            }
        }
    }

    /* Find maximum allowed columns. */
    for (cols = max_cols; 1 < cols; --cols) {
        if (mc->column_info[cols - 1].valid_len) {
            break;
        }
    }

    return (cols);
}

static void
print_many_per_line(mc_t *mc)
{
    size_t row;    /* Current row.  */
    size_t cols = calculate_columns(mc, true);
    struct column_info const *line_fmt = &mc->column_info[cols - 1];

    /* Calculate the number of rows that will be in each column,
     * except possibly for a short column on the right.
     */
    size_t rows = mc->nelem / cols + (mc->nelem % cols != 0);

#if 0
    printf("cols=%zu\n", cols);
    printf("\n");
#endif

    for (row = 0; row < rows; ++row) {
        size_t col = 0;
        size_t enr = row;

        /* Print the next row. */
        while (true) {
            const char *elem = mc->elemv[enr];
            size_t elem_length = strlen(elem);
            size_t max_elem_length = line_fmt->col_arr[col];

            if (col == 0) {
                size_t i;

                for (i = 0; i < mc->indent; ++i) {
                    fputc(' ', mc->f);
                }
            }

            fprintf(mc->f, "%s", elem);
            if (col < cols - 1) {
                size_t len;
                for (len = elem_length; len < max_elem_length; ++len) {
                    fputc(' ', mc->f);
                }
            }

            enr += rows;
            if (enr >= mc->nelem) {
                break;
            }

            ++col;
        }
        fputc('\n', mc->f);
    }
}

static void
print_horizontal(mc_t *mc)
{
    size_t enr;
    size_t cols = calculate_columns(mc, false);
    struct column_info const *line_fmt = &mc->column_info[cols - 1];

    for (enr = 0; enr < mc->nelem; ++enr) {
        const char *elem = mc->elemv[enr];
        size_t elem_length = strlen(elem);
        size_t max_elem_length;
        size_t col = enr % cols;

        max_elem_length = line_fmt->col_arr[col];
        if (col == 0) {
            size_t i;

            if (enr != 0) {
                fputc('\n', mc->f);
            }
            for (i = 0; i < mc->indent; ++i) {
                fputc(' ', mc->f);
            }
        }

        fprintf(mc->f, "%s", elem);
        if (col < cols - 1) {
            size_t len;
            for (len = elem_length; len < max_elem_length; ++len) {
                fputc(' ', mc->f);
            }
        }
    }

    fputc('\n', mc->f);
}


int
mc(FILE *f, size_t nelem, const char **elemv, size_t llen, size_t indent, bool horizontal)
{
    mc_t mcbuf;
    mc_t *mc = &mcbuf;

    mc->nelem  = nelem;
    mc->elemv  = elemv;
    mc->llen   = llen;
    mc->indent = indent;
    mc->column_info = NULL;
    mc->column_info_alloc = 0;
    mc->max_idx = mc->llen / MIN_COLUMN_WIDTH;
    if (mc->max_idx < 1) {
        mc->max_idx = 1;
    }
    mc->f = f;
    if (horizontal) {
        print_horizontal(mc);
    }
    else {
        print_many_per_line(mc);
    }
    return (0);
}
