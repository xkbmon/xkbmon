/********************************************************************
**  xkbmon, https://github.com/xkbmon/xkbmon
**  Copyright (C) 2017-2021 Sergey Vlasov <sergey@vlasov.me>
**
**  Licensed under MIT
**  https://github.com/xkbmon/xkbmon/blob/master/LICENSE.MIT
**
*********************************************************************/

#include <ctype.h>
#include <libgen.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xkb.h>

#define VERSION "0.4"

char *g_progname = NULL;

typedef enum
{
    NORMAL,
    UPPER,
    CAMEL
} case_mode_t;

typedef enum
{
    ONCE,
    LOOP
} run_mode_t;

struct options_
{
    case_mode_t case_mode;
    run_mode_t run_mode;
} g_options;

void print_usage()
{
    printf("Usage: %s [options]\n", g_progname);
    printf("\n");
    printf("  -o, --once          print layout once and exit\n");
    printf("  -u, --upper-case    output layout in upper case\n");
    printf("  -c, --camel-case    output layout in camel case\n");
    printf("  -v, --version       version number\n");
    printf("  -h, --help          print this message\n");
}

void print_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    fprintf(stderr, "%s: ", g_progname);
    vfprintf(stderr, format, args);

    va_end(args);
}

void parse_args(int argc, char **argv)
{
    g_options.case_mode = NORMAL;
    g_options.run_mode = LOOP;

    bool error = false;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-h") || strcmp(argv[i], "--help") == 0) {
            print_usage();
            exit(EXIT_SUCCESS);
        }
        if (!strcmp(argv[i], "-o") || strcmp(argv[i], "--once") == 0) {
            g_options.run_mode = ONCE;
            continue;
        }
        if (!strcmp(argv[i], "-u") || strcmp(argv[i], "--upper-case") == 0) {
            g_options.case_mode = UPPER;
            continue;
        }
        if (!strcmp(argv[i], "-c") || strcmp(argv[i], "--camel-case") == 0) {
            g_options.case_mode = CAMEL;
            continue;
        }
        if (!strcmp(argv[i], "-v") || strcmp(argv[i], "--version") == 0) {
            printf("%s\n", VERSION);
            exit(EXIT_SUCCESS);
        }
        if (argv[i][0] == '-') {  // check if it's an option
            print_error("Wrong option: '%s'\n", argv[i]);
            error = true;
            break;
        } else {
            print_error("Bad argument: '%s'\n", argv[i]);
            error = true;
            break;
        }
    }

    if (!error) {
        return;
    }

    fprintf(stderr, "Try '-h' for help\n");
    exit(EXIT_FAILURE);
}

void print_layout(xcb_connection_t *connection)
{
    int group = 0;
    {
        xcb_xkb_get_state_cookie_t cookie =
            xcb_xkb_get_state_unchecked(connection, XCB_XKB_ID_USE_CORE_KBD);
        xcb_xkb_get_state_reply_t *reply =
            xcb_xkb_get_state_reply(connection, cookie, NULL);
        if (!reply) {
            printf("error\n");
        }

        group = reply->group;
        free(reply);
    }

    xcb_xkb_get_names_value_list_t list;
    {
        xcb_xkb_get_names_cookie_t cookie = xcb_xkb_get_names(
            connection, XCB_XKB_ID_USE_CORE_KBD,
            XCB_XKB_NAME_DETAIL_GROUP_NAMES | XCB_XKB_NAME_DETAIL_SYMBOLS);
        xcb_xkb_get_names_reply_t *reply =
            xcb_xkb_get_names_reply(connection, cookie, NULL);
        if (!reply) {
            print_error("xcb_xkb_get_names failed\n");
        }

        const void *buffer = xcb_xkb_get_names_value_list(reply);
        xcb_xkb_get_names_value_list_unpack(
            buffer, reply->nTypes, reply->indicators, reply->virtualMods,
            reply->groupNames, reply->nKeys, reply->nKeyAliases,
            reply->nRadioGroups, reply->which, &list);
        free(reply);
    }

    char *name;
    {
        xcb_get_atom_name_cookie_t cookie =
            xcb_get_atom_name(connection, list.symbolsName);
        xcb_get_atom_name_reply_t *reply =
            xcb_get_atom_name_reply(connection, cookie, NULL);
        if (!reply) {
            print_error("xcb_get_atom_name failed\n");
        }

        name = xcb_get_atom_name_name(reply);
        free(reply);
    }

    char *token = strtok(name, "+");
    while (token) {
        token = strtok(NULL, "+");
        if (group == 0) {
            char *colon = strstr(token, ":");
            if (colon) {
                *colon = '\0';
            }

            char *p = token;
            switch (g_options.case_mode) {
                case UPPER:
                    while (*p) {
                        *p = (char)toupper(*p);
                        ++p;
                    }
                    break;
                case CAMEL:
                    *p = (char)toupper(*p);
                    break;
                default:
                    break;
            }

            printf("%s\n", token);
            break;
        }
        --group;
    }
}

int main(int argc, char *argv[])
{
    g_progname = basename(argv[0]);

    setbuf(stdout, NULL);
    parse_args(argc, argv);

    xcb_connection_t *connection = xcb_connect(NULL, NULL);

    {
        xcb_xkb_use_extension_cookie_t cookie = xcb_xkb_use_extension_unchecked(
            connection, XCB_XKB_MAJOR_VERSION, XCB_XKB_MINOR_VERSION);

        xcb_xkb_use_extension_reply_t *reply =
            xcb_xkb_use_extension_reply(connection, cookie, NULL);
        if (!reply || !reply->supported) {
            print_error("xcb_xkb_use_extension_unchecked failed\n");
        }

        xcb_xkb_select_events(connection, XCB_XKB_ID_USE_CORE_KBD,
                              XCB_XKB_EVENT_TYPE_STATE_NOTIFY, 0,
                              XCB_XKB_EVENT_TYPE_STATE_NOTIFY, 0, 0, NULL);
    }

    print_layout(connection);
    if (g_options.run_mode == ONCE) {
        return EXIT_SUCCESS;
    }

    xcb_generic_event_t *event;
    while ((event = xcb_wait_for_event(connection))) {
        switch (event->pad0) {
            case XCB_XKB_STATE_NOTIFY: {
                xcb_xkb_state_notify_event_t *ne = (void *)event;
                if (ne->changed & XCB_XKB_STATE_PART_GROUP_STATE) {
                    print_layout(connection);
                }

                break;
            }
            default:
                break;
        }
        free(event);
    }

    // never reached:
    return EXIT_FAILURE;
}
