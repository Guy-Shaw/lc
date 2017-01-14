/*
 * Filename: decode-esym.c
 * Library: libcscript
 * Brief: Like strerror_r(), but returns symbolic error as found in errno.h
 * Generate from the template: decode-esym-template.c
 * Generate on 2016-03-19 14:44:51
 *
 * Description:
 *   decode_esym_r() returns a string which is the symbol defined
 *   in errno.h for the given numeric errno value.
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
#include <sys/types.h>

static const char *errno_table[] = {
    /*    0 */ NULL,
    /*    1 */ "EPERM",
    /*    2 */ "ENOENT",
    /*    3 */ "ESRCH",
    /*    4 */ "EINTR",
    /*    5 */ "EIO",
    /*    6 */ "ENXIO",
    /*    7 */ "E2BIG",
    /*    8 */ "ENOEXEC",
    /*    9 */ "EBADF",
    /*   10 */ "ECHILD",
    /*   11 */ "EAGAIN",
    /*   12 */ "ENOMEM",
    /*   13 */ "EACCES",
    /*   14 */ "EFAULT",
    /*   15 */ "ENOTBLK",
    /*   16 */ "EBUSY",
    /*   17 */ "EEXIST",
    /*   18 */ "EXDEV",
    /*   19 */ "ENODEV",
    /*   20 */ "ENOTDIR",
    /*   21 */ "EISDIR",
    /*   22 */ "EINVAL",
    /*   23 */ "ENFILE",
    /*   24 */ "EMFILE",
    /*   25 */ "ENOTTY",
    /*   26 */ "ETXTBSY",
    /*   27 */ "EFBIG",
    /*   28 */ "ENOSPC",
    /*   29 */ "ESPIPE",
    /*   30 */ "EROFS",
    /*   31 */ "EMLINK",
    /*   32 */ "EPIPE",
    /*   33 */ "EDOM",
    /*   34 */ "ERANGE",
    /*   35 */ "EDEADLK",
    /*   36 */ "ENAMETOOLONG",
    /*   37 */ "ENOLCK",
    /*   38 */ "ENOSYS",
    /*   39 */ "ENOTEMPTY",
    /*   40 */ "ELOOP",
    /*   41 */ NULL,
    /*   42 */ "ENOMSG",
    /*   43 */ "EIDRM",
    /*   44 */ "ECHRNG",
    /*   45 */ "EL2NSYNC",
    /*   46 */ "EL3HLT",
    /*   47 */ "EL3RST",
    /*   48 */ "ELNRNG",
    /*   49 */ "EUNATCH",
    /*   50 */ "ENOCSI",
    /*   51 */ "EL2HLT",
    /*   52 */ "EBADE",
    /*   53 */ "EBADR",
    /*   54 */ "EXFULL",
    /*   55 */ "ENOANO",
    /*   56 */ "EBADRQC",
    /*   57 */ "EBADSLT",
    /*   58 */ NULL,
    /*   59 */ "EBFONT",
    /*   60 */ "ENOSTR",
    /*   61 */ "ENODATA",
    /*   62 */ "ETIME",
    /*   63 */ "ENOSR",
    /*   64 */ "ENONET",
    /*   65 */ "ENOPKG",
    /*   66 */ "EREMOTE",
    /*   67 */ "ENOLINK",
    /*   68 */ "EADV",
    /*   69 */ "ESRMNT",
    /*   70 */ "ECOMM",
    /*   71 */ "EPROTO",
    /*   72 */ "EMULTIHOP",
    /*   73 */ "EDOTDOT",
    /*   74 */ "EBADMSG",
    /*   75 */ "EOVERFLOW",
    /*   76 */ "ENOTUNIQ",
    /*   77 */ "EBADFD",
    /*   78 */ "EREMCHG",
    /*   79 */ "ELIBACC",
    /*   80 */ "ELIBBAD",
    /*   81 */ "ELIBSCN",
    /*   82 */ "ELIBMAX",
    /*   83 */ "ELIBEXEC",
    /*   84 */ "EILSEQ",
    /*   85 */ "ERESTART",
    /*   86 */ "ESTRPIPE",
    /*   87 */ "EUSERS",
    /*   88 */ "ENOTSOCK",
    /*   89 */ "EDESTADDRREQ",
    /*   90 */ "EMSGSIZE",
    /*   91 */ "EPROTOTYPE",
    /*   92 */ "ENOPROTOOPT",
    /*   93 */ "EPROTONOSUPPORT",
    /*   94 */ "ESOCKTNOSUPPORT",
    /*   95 */ "EOPNOTSUPP",
    /*   96 */ "EPFNOSUPPORT",
    /*   97 */ "EAFNOSUPPORT",
    /*   98 */ "EADDRINUSE",
    /*   99 */ "EADDRNOTAVAIL",
    /*  100 */ "ENETDOWN",
    /*  101 */ "ENETUNREACH",
    /*  102 */ "ENETRESET",
    /*  103 */ "ECONNABORTED",
    /*  104 */ "ECONNRESET",
    /*  105 */ "ENOBUFS",
    /*  106 */ "EISCONN",
    /*  107 */ "ENOTCONN",
    /*  108 */ "ESHUTDOWN",
    /*  109 */ "ETOOMANYREFS",
    /*  110 */ "ETIMEDOUT",
    /*  111 */ "ECONNREFUSED",
    /*  112 */ "EHOSTDOWN",
    /*  113 */ "EHOSTUNREACH",
    /*  114 */ "EALREADY",
    /*  115 */ "EINPROGRESS",
    /*  116 */ "ESTALE",
    /*  117 */ "EUCLEAN",
    /*  118 */ "ENOTNAM",
    /*  119 */ "ENAVAIL",
    /*  120 */ "EISNAM",
    /*  121 */ "EREMOTEIO",
    /*  122 */ "EDQUOT",
    /*  123 */ "ENOMEDIUM",
    /*  124 */ "EMEDIUMTYPE",
    /*  125 */ "ECANCELED",
    /*  126 */ "ENOKEY",
    /*  127 */ "EKEYEXPIRED",
    /*  128 */ "EKEYREVOKED",
    /*  129 */ "EKEYREJECTED",
    /*  130 */ "EOWNERDEAD",
    /*  131 */ "ENOTRECOVERABLE",
    /*  132 */ "ERFKILL",
    /*  133 */ "EHWPOISON",
};

static unsigned int errno_table_size = sizeof (errno_table) / sizeof (char *);

/**
 *
 * @brief Like strerror_r(), but returns symbolic error as found in errno.h
 * @param buf    IN,OUT  String buffer that _may_ be used to hold the result.
 * @param bufsz  IN      The capacity of |buf|.
 * @param err    IN      The errno value to be symbolically decoded.
 * @return  A string symbolically explaining |err|.
 *
 * decode_esym_r() will _always_ return a pointer to a string,
 * so it can be used directly in expressions where a char * is needed.
 * For example, in printf-like functions (but only once).
 *
 * There is _never_ an error.  If the given errno value is invalid,
 * a descriptive string is still returned.  It just indicates the
 * error and the numeric value.
 *
 * Normally, the decoded errno is _not_ copied into |buf|.
 * In the case of known errno values, decode_esym_r() returns a pointer
 * to a static string, which is a simple identifier, of the form E*,
 * that is defined to be the numeric value, |err|, according to the
 * definitions in the file errno.h.
 *
 * The destination buffer is used only in the case
 * of unknown errno values.   This is when a string value must be computed.
 * So, the returned pointer is always what should be used, not |buf|.
 *
 */
const char *
decode_esym_r(char *buf, size_t bufsz, int err)
{
    const char *sym;

    sym = NULL;
    if (err >= 0 && (unsigned int)err < errno_table_size) {
        sym = errno_table[err];
    }

    if (sym == NULL) {
        snprintf(buf, bufsz, "#%d", err);
        sym = (const char *)buf;
    }

    return (sym);
}
