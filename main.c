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

typedef enum {NORMAL, UPPER, CAMEL} case_mode_t;

int main(int argc, char **argv)
{
    setbuf(stdout, NULL);
    case_mode_t mode = NORMAL;

    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-h")) {
            printf(
                "usage: %s [OPTIONS]\n"
                "\n"
                "  -u    output layout in upper case\n"
                "  -c    output layout in camel case\n"
                "  -h    print this message\n"
                "\n",
                argv[0]);
            return 0;
        }
        if (!strcmp(argv[i], "-u")) {
            mode = UPPER;
            continue;
        }
        if (!strcmp(argv[i], "-c")) {
            mode = CAMEL;
            continue;
        }
        fprintf(stderr, "bad argument '%s', try '-h'\n", argv[i]);
        return 1;
    }

    int xkb_event_t, error, major = XkbMajorVersion, minor = XkbMinorVersion, reason;
    Display *dpy = XkbOpenDisplay(NULL, &xkb_event_t, &error, &major, &minor, &reason);
    if (!dpy) {
        fprintf(stderr, "cannot open display\n");
        return 1;
    }

    XkbSelectEventDetails(dpy, XkbUseCoreKbd, XkbStateNotify, XkbAllStateComponentsMask, XkbGroupStateMask);
    XkbSelectEvents(dpy, XkbUseCoreKbd, XkbNewKeyboardNotifyMask, XkbNewKeyboardNotifyMask);

    while (1) {
        XEvent event;
        XNextEvent(dpy, &event);
        if (event.type != xkb_event_t) {
            continue;
        }

        XkbDescPtr desc = XkbAllocKeyboard();
        if (!desc) {
            fprintf(stderr, "XkbAllocKeyboard failed\n");
            return 1;
        }
        XkbGetNames(dpy, XkbSymbolsNameMask, desc);

        Atom atom = desc->names->symbols;
        char *name = XGetAtomName(dpy, atom);
        if (!name) {
            fprintf(stderr, "XGetAtomName failed\n");
            return 1;
        }

        int group = ((XkbEvent)event).state.group & (XkbNumKbdGroups - 1);
        char *token = strtok(name, "+");
        while (token) {
            token = strtok(NULL, "+");
            if (group == 0) {
                char *colon = strstr(token, ":");
                if (colon) {
                    *colon = '\0';
                }

                char *p = token;
                switch (mode) {
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
    }

    return 0;
}
