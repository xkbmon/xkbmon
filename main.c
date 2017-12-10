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
#include <X11/XKBlib.h>

int main(int argc, char **argv)
{
    setbuf(stdout, NULL);

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
