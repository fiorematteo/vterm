#include "term.h"

static void child_ready(VteTerminal *terminal, GPid pid, GError *error,
                        gpointer user_data) {
    if (!terminal)
        return;
    if (pid == -1)
        gtk_main_quit();
}

void what_the_fuck() { gtk_main_quit(); }

void print_help(char *error) {
    printf("%s\n", error);
    printf("Usage:\n"
           "  vterm [OPTION…]\n"
           "\n"
           "Help Options:\n"
           "  -h    Show help options\n"
           "\n"
           "Application Options:\n"
           "  -t    Program name\n"
           "  -g    Size of window {width}x{height}\n"
           "  -p    Position of window {X}x{Y}\n"
           "  -e    Execute the argument inside the terminal\n");
    exit(0);
}

void handle_args(int argc, char **argv, GtkWidget *window, char **cmd) {
    int c;
    opterr = 0;
    int width = 0, height = 0, x = 0, y = 0;
    regex_t regex;

    while ((c = getopt(argc, argv, "e:t:g:p:h")) != -1)
        switch (c) {
        case 't':
            gtk_window_set_title(GTK_WINDOW(window), optarg);
            break;
        case 'e':
            *cmd = malloc(strlen(optarg));
            strcpy(*cmd, optarg);
            break;
        case 'g':
            regcomp(&regex, "[0-9]\\+x[0-9]\\+", 0);
            if (regexec(&regex, optarg, 0, NULL, 0) == REG_NOMATCH)
                print_help("You fucked up dimension arguments");
            width = atoi(strtok(optarg, "x"));
            height = atoi(strtok(NULL, "x"));
            gtk_window_set_default_size(GTK_WINDOW(window), width, height);
            break;
        case 'p':
            regcomp(&regex, "[0-9]\\+x[0-9]\\+", 0);
            if (regexec(&regex, optarg, 0, NULL, 0) == REG_NOMATCH)
                print_help("You fucked up position arguments");
            x = atoi(strtok(optarg, "x"));
            y = atoi(strtok(NULL, "x"));
            gtk_window_move(GTK_WINDOW(window), x, y);
            break;
        case 'h':
            print_help("");
            break;
        case '?':
            switch (optopt) {
            case 'h':
            case 't':
            case 'g':
            case 'p':
            case 'e':
                fprintf(stderr, "Option -%c requires an argument.\n",
                        (char)optopt);
                print_help("");
                break;
            default:
                fprintf(stderr, "Unknown option character `%c'.\n",
                        (char)optopt);
                print_help("");
            }
        }
}

static void set_font_size(VteTerminal *terminal, gint delta) {
    PangoFontDescription *new_font =
        pango_font_description_copy(vte_terminal_get_font(terminal));
    if (!new_font)
        return;

    gint current = pango_font_description_get_size(new_font);
    gint new_size = current + delta * PANGO_SCALE;
    pango_font_description_set_size(new_font, new_size < 0 ? 1 : new_size);
    vte_terminal_set_font(terminal, new_font);
    pango_font_description_free(new_font);
}

int equal_struct(struct key_comb a, struct key_comb b) {
    if (a.key != b.key)
        return 0;
    if (a.mod_key != b.mod_key)
        return 0;
    return 1;
}

static gboolean on_key_press(GtkWidget *terminal, GdkEventKey *event,
                             gpointer user_data) {

    if (event->type == GDK_KEY_RELEASE)
        return FALSE;

    struct key_comb current_key = {
        .mod_key = event->state,
        .key = event->keyval,
    };

    if (equal_struct(current_key, CTRL_SHIFT_V))
        vte_terminal_paste_clipboard(VTE_TERMINAL(terminal));

    else if (equal_struct(current_key, CTRL_SHIFT_V))
        vte_terminal_copy_primary(VTE_TERMINAL(terminal));

    else if (equal_struct(current_key, CTRL_PLUS))
        set_font_size(VTE_TERMINAL(terminal), 1);

    else if (equal_struct(current_key, CTRL_MINUS))
        set_font_size(VTE_TERMINAL(terminal), -1);

    else
        return FALSE;

    return TRUE;
}

int main(int argc, char **argv) {
    GtkWidget *window;
    GtkWidget *terminal;

    // Initialise GTK, the window and the terminal
    gtk_init(&argc, &argv);

    terminal = vte_terminal_new();
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "myterm");

    // handle options
    char *cmd = NULL;
    if (argc > 1)
        handle_args(argc, argv, window, &cmd);

    // hides mouse while typing
    vte_terminal_set_mouse_autohide(VTE_TERMINAL(terminal), TRUE);

    // set colors
    const GdkRGBA fg_color = CLR_GDK(0xeceff1);
    const GdkRGBA bg_color = CLR_GDK(0x1d2021);
    vte_terminal_set_colors(VTE_TERMINAL(terminal), &fg_color, &bg_color,
                            PALETTE_16, 16);

    // set font
    PangoFontDescription *font =
        pango_font_description_from_string(DEFAULT_FONT);
    vte_terminal_set_font(VTE_TERMINAL(terminal), font);
    pango_font_description_free(font);

    // Start a new shell
    gchar **envp = g_get_environ();
    gchar **command = (gchar *[]){g_strdup(g_environ_getenv(envp, "SHELL")),
                                  cmd == NULL ? cmd : "-c", cmd, NULL};

    g_strfreev(envp);
    vte_terminal_spawn_async(VTE_TERMINAL(terminal), VTE_PTY_DEFAULT,
                             NULL,        /* working directory  */
                             command,     /* command */
                             NULL,        /* environment */
                             0,           /* spawn flags */
                             NULL, NULL,  /* child setup */
                             NULL,        /* child pid */
                             -1,          /* timeout */
                             NULL,        /* cancellable */
                             child_ready, /* callback */
                             NULL);       /* user_data */

    // Connect some signals
    g_signal_connect(window, "delete-event", what_the_fuck, NULL);
    g_signal_connect(terminal, "child-exited", what_the_fuck, NULL);
    g_signal_connect(terminal, "key-press-event", G_CALLBACK(on_key_press),
                     GTK_WINDOW(window));

    // Put widgets together and run the main loop
    gtk_container_add(GTK_CONTAINER(window), terminal);
    gtk_widget_show_all(window);
    gtk_main();

    free(cmd);
}
