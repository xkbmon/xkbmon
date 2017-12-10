/********************************************************************
**  xkbmon, https://github.com/xkbmon/xkbmon
**  Copyright (C) 2017 Sergey Vlasov <sergey@vlasov.me>
**
**  This program can be distributed under the terms of the GNU
**  General Public License version 3.0 as published by the Free
**  Software Foundation and appearing in the file LICENSE.GPL3
**  included in the packaging of this file.  Please review the
**  following information to ensure the GNU General Public License
**  version 3.0 requirements will be met:
**
**  http://www.gnu.org/licenses/gpl-3.0.html
**
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <X11/XKBlib.h>

#define VERSION "0.1"
typedef enum {NORMAL, UPPER, CAMEL} case_mode_t;
typedef enum {ONCE, LOOP} run_mode_t;

case_mode_t case_mode = NORMAL;
run_mode_t run_mode = LOOP;
Display *dpy = NULL;

void parse_args(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-h")) {
            printf(
                "usage: %s [OPTIONS]\n"
                "\n"
                "  -o    print layout once and exit\n"
                "  -u    output layout in upper case\n"
                "  -c    output layout in camel case\n"
                "  -v    version number\n"
                "  -h    print this message\n"
                "\n",
                argv[0]);
            exit(0);
        }
        if (!strcmp(argv[i], "-o")) {
            run_mode = ONCE;
            continue;
        }
        if (!strcmp(argv[i], "-u")) {
            case_mode = UPPER;
            continue;
        }
        if (!strcmp(argv[i], "-c")) {
            case_mode = CAMEL;
            continue;
        }
        if (!strcmp(argv[i], "-v")) {
            printf("%s\n", VERSION);
            exit(0);
        }
        fprintf(stderr, "bad argument '%s', try '-h'\n", argv[i]);
        exit(1);
    }
}

void print_layout()
{
    XkbDescPtr desc = XkbAllocKeyboard();
    if (!desc) {
        fprintf(stderr, "XkbAllocKeyboard failed\n");
        exit(1);
    }
    XkbGetNames(dpy, XkbSymbolsNameMask, desc);

    Atom atom = desc->names->symbols;
    char *name = XGetAtomName(dpy, atom);
    if (!name) {
        fprintf(stderr, "XGetAtomName failed\n");
        exit(1);
    }

    XkbStateRec	state;
    XkbGetState(dpy, XkbUseCoreKbd, &state);
    int group = state.group & (XkbNumKbdGroups - 1);

    char *token = strtok(name, "+");
    while (token) {
        token = strtok(NULL, "+");
        if (group == 0) {
            char *colon = strstr(token, ":");
            if (colon) {
                *colon = '\0';
            }

            char *p = token;
            switch (case_mode) {
                case UPPER:
                    while (*p) {
                        *p = toupper(*p);
                        ++p;
                    }
                    break;
                case CAMEL:
                    *p = toupper(*p);
                    break;
                default:
                break;
            }

            printf("%s\n", token);
            break;
        }
        --group;
    }

    if (name) {
        XFree(name);
    }
    XkbFreeKeyboard(desc, 0, 1);
}

int main(int argc, char **argv)
{
    setbuf(stdout, NULL);
    parse_args(argc, argv);

    int xkb_event_t, error, major = XkbMajorVersion, minor = XkbMinorVersion, reason;
    dpy = XkbOpenDisplay(NULL, &xkb_event_t, &error, &major, &minor, &reason);
    if (!dpy) {
        fprintf(stderr, "cannot open display\n");
        return 1;
    }

    print_layout();
    if (run_mode == ONCE) {
        return 0;
    }

    XkbSelectEventDetails(dpy, XkbUseCoreKbd, XkbStateNotify, XkbAllStateComponentsMask, XkbGroupStateMask);
    XkbSelectEvents(dpy, XkbUseCoreKbd, XkbNewKeyboardNotifyMask, XkbNewKeyboardNotifyMask);
    while (1) {
        XEvent event;
        XNextEvent(dpy, &event);
        print_layout();
    }

    return 0;
}
