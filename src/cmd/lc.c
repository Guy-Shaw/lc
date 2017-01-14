#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <ctype.h>
    // Import isprint()
#include <err.h>
    // Import err()
#include <errno.h>
    // Import var errno
#include <stdbool.h>
    // Import type bool
    // Import constant false
    // Import constant true
#include <stddef.h>
    // Import constant NULL
#include <stdio.h>
    // Import type FILE
    // Import fputc()
    // Import fputs()
    // Import snprintf()
    // Import var stdout
#include <stdlib.h>
    // Import exit()
#include <sys/types.h>
    // Import stat()
#include <sys/stat.h>

#include <unistd.h>
    // Import access()
    // Import getopt_long()
    // Import optarg()
    // Import opterr()
    // Import optind()
    // Import optopt()
    // Import type size_t

#include <string.h>
    // Import strcmp()

#include <cscript.h>

#include <lc.h>

extern void lsdlh(const char *fname);
extern int mode_to_ftype(int m);

const char *program_path;
const char *program_name;

size_t filec;               // Count of elements in filev
char **filev;               // Non-option elements of argv

bool verbose = false;
bool debug   = false;

static lc_options_t lcopt = {
    .lc_cols          = 0,
    .lc_indent_global = 0,
    .lc_indent_types  = 0,
    .lc_indent_files  = 2,
    .lc_ftypes        = NULL,
    .lc_showdirs      = 0,
    .lc_all           = 0,
    .lc_horizontal    = 0,
};

FILE *errprint_fh = NULL;
FILE *dbgprint_fh = NULL;

enum indent_options {
    INDENT_GLOBAL   = 0x10001,
    INDENT_TYPES,
    INDENT_FILES,
};

enum show_options {
    SHOWDIRS_NEVER  = 0x20001,
    SHOWDIRS_ALWAYS,
    SHOWDIRS_ANY,
    SHOWDIRS_AUTO,
};

static struct option long_options[] = {
    {"help",           no_argument,       0,  'h'},
    {"version",        no_argument,       0,  'V'},
    {"verbose",        no_argument,       0,  'v'},
    {"debug",          no_argument,       0,  'd'},
    {"width",          required_argument, 0,  'w'},
    {"indent-global",  required_argument, 0,  INDENT_GLOBAL},
    {"indent-types",   required_argument, 0,  INDENT_TYPES},
    {"indent-files",   required_argument, 0,  INDENT_FILES},
    {"types",          required_argument, 0,  't'},
    {"show-dirs",      required_argument, 0,  's'},
    {"all",            no_argument,       0,  'a'},
    {"horizontal",     no_argument,       0,  'x'},
    {0, 0, 0, 0}
};

// Collection option specifications, first
//
// Then, after all options have been scanned and we know how many
// explicit directory name arguments there are, set the final
// show_dirs behavior.
//
static int opt_showdirs = SHOWDIRS_AUTO;

static const char usage_text[] =
    "Options:\n"
    "  --help|-h|-?         Show this help message and exit\n"
    "  --version            Show version information and exit\n"
    "  --verbose|-v         verbose\n"
    "  --debug|-d           debug\n"
    "  --width|-w <n>       Use width (line length) of n columns\n"
    "  --indent-global=<n>  Indent top-level (everything)\n"
    "  --indent-types=<n>   Indent (further) file type labels (level 2)\n"
    "  --indent-files=<n>   Indent (further) file names       (level 3)\n"
    "                       Default is --indent-files=2\n"
    "  --show-dirs=WHEN     Show directory names (top level)\n"
    "      WHEN=always|never|any|auto\n"
    "          always: No matter what directories are on the command line\n"
    "          never:  Don't even think about it\n"
    "          any:    If there are directories named on the command line\n"
    "          auto:   If there is more than 1 directory named\n"
    "  --horizontal|-x      Display across each row, not down each column\n"
    "  --all|-a             Show all files, including hidden files\n"
    "  --types|-t <types>   Show only the given file types\n"
    "\n"
    "File types are: f, d, l, u, p, s, c, b, o, E\n"
    "  f = file\n"
    "  d = directory\n"
    "  l = symbolic link\n"
    "  u = unresolved symbolic link\n"
    "  p = pipe\n"
    "  s = socket\n"
    "  c = character device\n"
    "  b = block device\n"
    "  o = other\n"
    "  E = ERROR\n"
    "\n"
    ;

static const char version_text[] =
    "0.1\n"
    ;

static const char copyright_text[] =
    "Copyright (C) 2016 Guy Shaw\n"
    "Written by Guy Shaw\n"
    ;

static const char license_text[] =
    "License GPLv3+: GNU GPL version 3 or later"
    " <http://gnu.org/licenses/gpl.html>.\n"
    "This is free software: you are free to change and redistribute it.\n"
    "There is NO WARRANTY, to the extent permitted by law.\n"
    ;

static void
fshow_program_version(FILE *f)
{
    fputs(version_text, f);
    fputc('\n', f);
    fputs(copyright_text, f);
    fputc('\n', f);
    fputs(license_text, f);
    fputc('\n', f);
}

static void
show_program_version(void)
{
    fshow_program_version(stdout);
}

static void
usage(void)
{
    eprintf("usage: %s [ <options> ]\n", program_name);
    eprint(usage_text);
}

static inline bool
is_long_option(const char *s)
{
    return (s[0] == '-' && s[1] == '-');
}

static inline char *
vischar_r(char *buf, size_t sz, int c)
{
    if (isprint(c)) {
        buf[0] = c;
        buf[1] = '\0';
    }
    else {
        snprintf(buf, sz, "\\x%02x", c);
    }
    return (buf);
}


int
file_check(const char *fname, int test)
{
    struct stat statb;
    int ftype = '?';
    int rv;
    int err;

    switch (test) {
    case 'e':
        rv = access(fname, R_OK);
        if (rv) {
            err = errno;
            eprintf("'%s' does not exist.\n", fname);
            return (err);
        }
        break;
    case 'd':
        rv = stat(fname, &statb);
        if (rv) {
            err = errno;
            eprintf("stat('%s', _) failed; %d\n", fname, err);
            return (err);
        }
        ftype = mode_to_ftype(statb.st_mode);
        if (ftype != 'd') {
            eprintf("'%s' is not a directory.\n", fname);
            lsdlh(fname);
            return (ENOTDIR);
        }
        break;
    default:
        return (EINVAL);
    }

    return (0);
}

int
file_check_tests(const char *fname, const char *tests)
{
    const char *t;

    for (t = tests; *t; ++t) {
        int rv;

        rv = file_check(fname, *t);
        if (rv) {
            return (rv);
        }
    }

    return (0);
}

int
filev_probe_dirs(void)
{
    size_t fnr;
    bool any_err;

    any_err = false;
    for (fnr = 0; fnr < filec; ++fnr) {
        int rv;

        rv = file_check_tests(filev[fnr], "ed");
        if (rv) {
            any_err = true;
        }
    }

    return (any_err ? 1 : 0);
}

int
filev_lc(void)
{
    size_t fnr;

    for (fnr = 0; fnr < filec; ++fnr) {
        lc_directory(filev[fnr], &lcopt);
    }

    return (0);
}

int
main(int argc, char **argv)
{
    extern char *optarg;
    extern int optind, opterr, optopt;
    int option_index;
    int err_count;
    int optc;
    int rv;

    set_eprint_fh();
    program_path = *argv;
    program_name = sname(program_path);
    option_index = 0;
    err_count = 0;
    opterr = 0;

    while (true) {
        int this_option_optind;

        if (err_count > 10) {
            eprintf("%s: Too many option errors.\n", program_name);
            break;
        }

        this_option_optind = optind ? optind : 1;
        optc = getopt_long(argc, argv, "+hVdvw:i:axt:", long_options, &option_index);
        if (optc == -1) {
            break;
        }

        rv = 0;
        if (optc == '?' && optopt == '?') {
            optc = 'h';
        }

        switch (optc) {
        case 'V':
            show_program_version();
            exit(0);
            break;
        case 'h':
            fputs(usage_text, stdout);
            exit(0);
            break;
        case 'd':
            debug = true;
            set_debug_fh(NULL);
            break;
        case 'v':
            verbose = true;
            break;
        case 'w':
            rv = parse_cardinal(&lcopt.lc_cols, optarg);
            break;
        case INDENT_GLOBAL:
            rv = parse_cardinal(&lcopt.lc_indent_global, optarg);
            break;
        case INDENT_TYPES:
            rv = parse_cardinal(&lcopt.lc_indent_types, optarg);
            break;
        case INDENT_FILES:
            rv = parse_cardinal(&lcopt.lc_indent_files, optarg);
            break;
        case 'x':
            lcopt.lc_horizontal = true;
            break;
        case 't':
            rv = parse_ftypes(optarg);
            if (rv == 0) {
                lcopt.lc_ftypes = optarg;
            }
            break;
        case 'a':
            lcopt.lc_all = 1;
            break;
        case 's':
            if (strcmp(optarg, "never") == 0) {
                opt_showdirs = SHOWDIRS_NEVER;
            }
            else if (strcmp(optarg, "always") == 0) {
                opt_showdirs = SHOWDIRS_ALWAYS;
            }
            else if (strcmp(optarg, "any") == 0) {
                opt_showdirs = SHOWDIRS_ANY;
            }
            else if (strcmp(optarg, "auto") == 0) {
                opt_showdirs = SHOWDIRS_AUTO;
            }
            else {
                eprintf("Invalid --show-dirs option, '%s'.\n", optarg);
                rv = -1;
                exit(2);
            }
            break;
        case '?':
            eprint(program_name);
            eprint(": ");
            if (is_long_option(argv[this_option_optind])) {
                eprintf("unknown long option, '%s'\n",
                    argv[this_option_optind]);
            }
            else {
                char chrbuf[10];
                eprintf("unknown short option, '%s'\n",
                    vischar_r(chrbuf, sizeof (chrbuf), optopt));
            }
            ++err_count;
            break;
        default:
            eprintf("%s: INTERNAL ERROR: unknown option, '%c'\n",
                program_name, optopt);
            exit(64);
            break;
        }
    }

    verbose = verbose || debug;

    if (argc != 0) {
        filec = (size_t) (argc - optind);
        filev = argv + optind;
    }
    else {
        filec = 0;
        filev = NULL;
    }

    switch (opt_showdirs) {
    default:
        eprintf("%s: INTERNAL ERROR: unknown show-dirs suboption, '%x'\n",
                program_name, opt_showdirs);
        exit(64);
        break;
    case SHOWDIRS_NEVER:
        lcopt.lc_showdirs = 0;
        break;
    case SHOWDIRS_ALWAYS:
        lcopt.lc_showdirs = 1;
        break;
    case SHOWDIRS_ANY:
        lcopt.lc_showdirs = (filec >= 1);
        break;
    case SHOWDIRS_AUTO:
        lcopt.lc_showdirs = (filec >= 2);
        break;
    }

    if (verbose) {
        fshow_str_array(errprint_fh, filec, filev);
    }

    if (verbose && optind < argc) {
        eprintf("non-option ARGV-elements:\n");
        while (optind < argc) {
            eprintf("    %s\n", argv[optind]);
            ++optind;
        }
    }

    if (err_count != 0) {
        usage();
        exit(1);
    }

    if (filec == 0) {
        lc_directory(".", &lcopt);
    }
    else {
        rv = filev_probe_dirs();
        if (rv != 0) {
            exit(rv);
        }

        rv = filev_lc();
    }

    if (rv != 0) {
        exit(rv);
    }

    exit(0);
}
