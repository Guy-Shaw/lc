/*
 * File:   parse-cardinal.c
 * Author: Guy Shaw
 *
 * Created on 2016-Jun-04
 *
 * (c) Copyright 2016
 *
 */

#include <ctype.h>      // isdigit(), isspace()
#include <errno.h>      // EINVAL
#include <stddef.h>     // type size_t
#include <stdio.h>      // fprintf(), stderr, NULL
#include <stdlib.h>     // strtoul()

typedef size_t index_t;

int
parse_cardinal(size_t *r, const char *str)
{
    char buf[64];
    char *bp;
    const char *s;
    char *end = NULL;
    int prev_ccl = '0';

    s = str;
    while (*s && isspace(*s)) {
        ++s;
    }

    for (bp = buf; *s; ++s) {
        int c, ccl;
        c = *s;
        ccl = isdigit(c) ? '0' : c;
        switch (ccl) {
        case ',':
        case '_':
            if (prev_ccl != '0') {
                fprintf(stderr, "Invalid numeric argument, '%s'.\n", str);
                return (EINVAL);
            }
            break;
        default:
            *bp++ = c;
        }
        prev_ccl = ccl;
    }
    *bp = '\0';
    *r = strtoul(buf, &end, 10);
    if (!(end > buf && *end == '\0')) {
        fprintf(stderr, "Invalid numeric argument, '%s'.\n", buf);
        return (EINVAL);
    }
    return (0);
}
