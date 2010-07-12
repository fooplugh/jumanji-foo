/* settings */
int   default_width      = 800;
int   default_height     = 600;
float zoom_step          = 10;
float scroll_step        = 40;
int   n_completion_items = 15;

/* completion */
static const char FORMAT_COMMAND[]     = "<b>%s</b>";
static const char FORMAT_DESCRIPTION[] = "<i>%s</i>";

/* directories and files */
static const char JUMANJI_DIR[]       = ".config/jumanji";
static const char JUMANJI_RC[]        = "jumanjirc";
static const char JUMANJI_BOOKMARKS[] = "bookmarks";
static const char JUMANJI_HISTORY[]   = "history";
static const char JUMANJI_COOKIES[]   = "cookies";

/* browser specific settings */
char* user_agent          = "jumanji/0.1";
char* home_page           = "http://www.pwmt.org";
char* proxy               = NULL;
gboolean private_browsing = FALSE;
gboolean plugin_blocker   = TRUE;
int auto_save_interval    = 0;

/* download settings */
char* download_dir     = "~/dl/";
char* download_command = "xterm -e sh -c \"wget --load-cookies ~/.config/jumanji/cookies '%s' -O '%s'\"";

/* look */
char* font                   = "monospace normal 9";
char* default_bgcolor        = "#000000";
char* default_fgcolor        = "#DDDDDD";
char* inputbar_bgcolor       = "#141414";
char* inputbar_fgcolor       = "#9FBC00";
char* statusbar_bgcolor      = "#000000";
char* statusbar_fgcolor      = "#FFFFFF";
char* statusbar_ssl_bgcolor  = "#9FBC00";
char* statusbar_ssl_fgcolor  = "#000000";
char* tabbar_bgcolor         = "#000000";
char* tabbar_fgcolor         = "#FFFFFF";
char* tabbar_focus_bgcolor   = "#000000";
char* tabbar_focus_fgcolor   = "#9FBC00";
char* tabbar_separator_color = "#575757";
char* completion_fgcolor     = "#DDDDDD";
char* completion_bgcolor     = "#232323";
char* completion_g_fgcolor   = "#DEDEDE";
char* completion_g_bgcolor   = "#000000";
char* completion_hl_fgcolor  = "#232323";
char* completion_hl_bgcolor  = "#9FBC00";
char* notification_e_bgcolor = "#FF1212";
char* notification_e_fgcolor = "#FFFFFF";
char* notification_w_bgcolor = "#FFF712";
char* notification_w_fgcolor = "#000000";

/* additional settings */
gboolean show_scrollbars = FALSE;
gboolean show_statusbar  = TRUE;
gboolean show_tabbar     = TRUE;
gboolean next_to_current = TRUE;

#define GDK_COSHIFT_MASK GDK_CONTROL_MASK | GDK_SHIFT_MASK

/* shortcuts */
Shortcut shortcuts[] = {
  /* mask,             key,               function,             mode,                argument */
  {GDK_CONTROL_MASK,   GDK_c,             sc_abort,             ALL,                 {0} },
  {0,                  GDK_Escape,        sc_abort,             ALL,                 {0} },
  {0,                  GDK_BackSpace,     sc_change_buffer,     ALL,                 { DELETE_LAST_CHAR } },
  {0,                  GDK_m,             sc_change_mode,       NORMAL,              { ADD_MARKER } },
  {0,                  GDK_apostrophe,    sc_change_mode,       NORMAL,              { EVAL_MARKER } },
  {0,                  GDK_v,             sc_change_mode,       NORMAL,              { VISUAL } },
  {GDK_CONTROL_MASK,   GDK_z,             sc_change_mode,       NORMAL,              { PASS_THROUGH } },
  {GDK_CONTROL_MASK,   GDK_v,             sc_change_mode,       NORMAL,              { PASS_THROUGH_NEXT } },
  {GDK_CONTROL_MASK,   GDK_w,             sc_close_tab,         NORMAL,              {0} },
  {0,                  GDK_d,             sc_close_tab,         NORMAL,              {0} },
  {0,                  GDK_slash,         sc_focus_inputbar,    NORMAL,              { .data = "/" } },
  {GDK_SHIFT_MASK,     GDK_slash,         sc_focus_inputbar,    NORMAL,              { .data = "/" } },
  {GDK_SHIFT_MASK,     GDK_question,      sc_focus_inputbar,    NORMAL,              { .data = "?" } },
  {0,                  GDK_colon,         sc_focus_inputbar,    NORMAL,              { .data = ":" } },
  {0,                  GDK_o,             sc_focus_inputbar,    NORMAL,              { .data = ":open " } },
  {0,                  GDK_O,             sc_focus_inputbar,    NORMAL,              { .data = ":open ", .n = APPEND_URL } },
  {0,                  GDK_t,             sc_focus_inputbar,    NORMAL,              { .data = ":tabopen " } },
  {0,                  GDK_T,             sc_focus_inputbar,    NORMAL,              { .data = ":tabopen ", .n = APPEND_URL } },
  {0,                  GDK_w,             sc_focus_inputbar,    NORMAL,              { .data = ":winopen " } },
  {0,                  GDK_W,             sc_focus_inputbar,    NORMAL,              { .data = ":winopen ", .n = APPEND_URL } },
  {0,                  GDK_f,             sc_follow_link,       NORMAL,              {-1} },
  {0,                  GDK_F,             sc_follow_link,       NORMAL,              {-2} },
  {GDK_CONTROL_MASK,   GDK_i,             sc_nav_history,       NORMAL,              { NEXT } },
  {GDK_CONTROL_MASK,   GDK_o,             sc_nav_history,       NORMAL,              { PREVIOUS } },
  {0,                  GDK_L,             sc_nav_history,       NORMAL,              { NEXT } },
  {0,                  GDK_H,             sc_nav_history,       NORMAL,              { PREVIOUS } },
  {0,                  GDK_K,             sc_nav_tabs,          NORMAL,              { NEXT } },
  {0,                  GDK_J,             sc_nav_tabs,          NORMAL,              { PREVIOUS } },
  {GDK_CONTROL_MASK,   GDK_Tab,           sc_nav_tabs,          NORMAL,              { NEXT } },
  {GDK_COSHIFT_MASK,   GDK_ISO_Left_Tab,  sc_nav_tabs,          NORMAL,              { PREVIOUS } },
  {0,                  GDK_p,             sc_paste,             NORMAL,              {0} },
  {0,                  GDK_P,             sc_paste,             NORMAL,              { NEW_TAB } },
  {0,                  GDK_r,             sc_reload,            NORMAL,              {0} },
  {0,                  GDK_R,             sc_reload,            NORMAL,              { BYPASS_CACHE } },
  {0,                  GDK_h,             sc_scroll,            NORMAL,              { LEFT } },
  {0,                  GDK_j,             sc_scroll,            NORMAL,              { DOWN } },
  {0,                  GDK_k,             sc_scroll,            NORMAL,              { UP } },
  {0,                  GDK_l,             sc_scroll,            NORMAL,              { RIGHT } },
  {0,                  GDK_Left,          sc_scroll,            NORMAL,              { LEFT } },
  {0,                  GDK_Up,            sc_scroll,            NORMAL,              { UP } },
  {0,                  GDK_Down,          sc_scroll,            NORMAL,              { DOWN } },
  {0,                  GDK_Right,         sc_scroll,            NORMAL,              { RIGHT } },
  {GDK_CONTROL_MASK,   GDK_d,             sc_scroll,            NORMAL,              { HALF_DOWN } },
  {GDK_CONTROL_MASK,   GDK_u,             sc_scroll,            NORMAL,              { HALF_UP } },
  {GDK_CONTROL_MASK,   GDK_f,             sc_scroll,            NORMAL,              { FULL_DOWN } },
  {GDK_CONTROL_MASK,   GDK_b,             sc_scroll,            NORMAL,              { FULL_UP } },
  {0,                  GDK_space,         sc_scroll,            NORMAL,              { FULL_DOWN } },
  {GDK_SHIFT_MASK,     GDK_space,         sc_scroll,            NORMAL,              { FULL_UP } },
  {0,                  GDK_n,             sc_search,            NORMAL,              { FORWARD } },
  {0,                  GDK_N,             sc_search,            NORMAL,              { BACKWARD } },
  {GDK_CONTROL_MASK,   GDK_p,             sc_toggle_proxy,      NORMAL,              {0} },
  {GDK_CONTROL_MASK,   GDK_m,             sc_toggle_statusbar,  NORMAL,              {0} },
  {GDK_CONTROL_MASK,   GDK_n,             sc_toggle_tabbar,     NORMAL,              {0} },
  {GDK_CONTROL_MASK,   GDK_s,             sc_toggle_sourcecode, NORMAL,              {0} },
  {GDK_CONTROL_MASK,   GDK_q,             sc_quit,              ALL,                 {0} },
  {0,                  GDK_y,             sc_yank,              NORMAL,              {0} },
  {0,                  GDK_plus,          sc_zoom,              NORMAL,              { ZOOM_IN } },
  {0,                  GDK_minus,         sc_zoom,              NORMAL,              { ZOOM_OUT } },
  {0,                  GDK_Return,        sc_follow_link,       FOLLOW,              {10} },
  {0,                  GDK_BackSpace,     sc_follow_link,       FOLLOW,              {11} },
};

/* inputbar shortcuts */
InputbarShortcut inputbar_shortcuts[] = {
  /* mask,             key,               function,                  argument */
  {0,                  GDK_Escape,        isc_abort,                 {0} },
  {GDK_CONTROL_MASK,   GDK_c,             isc_abort,                 {0} },
  {0,                  GDK_Up,            isc_command_history,       { PREVIOUS } },
  {0,                  GDK_Down,          isc_command_history,       { NEXT } },
  {0,                  GDK_Tab,           isc_completion,            { NEXT } },
  {GDK_CONTROL_MASK,   GDK_Tab,           isc_completion,            { NEXT_GROUP } },
  {0,                  GDK_ISO_Left_Tab,  isc_completion,            { PREVIOUS } },
  {GDK_CONTROL_MASK,   GDK_ISO_Left_Tab,  isc_completion,            { PREVIOUS_GROUP } },
  {0,                  GDK_BackSpace,     isc_string_manipulation,   { DELETE_LAST_CHAR } },
  {GDK_CONTROL_MASK,   GDK_h,             isc_string_manipulation,   { DELETE_LAST_CHAR } },
  {GDK_CONTROL_MASK,   GDK_w,             isc_string_manipulation,   { DELETE_LAST_WORD } },
  {GDK_CONTROL_MASK,   GDK_f,             isc_string_manipulation,   { NEXT_CHAR } },
  {GDK_CONTROL_MASK,   GDK_b,             isc_string_manipulation,   { PREVIOUS_CHAR } },
};

/* mouse buttons */
Mouse mouse[] = {
  /* mask,             button,   function,   mode,      argument */
  {0,                  2,        NULL,       NORMAL,    {0} },
  {GDK_CONTROL_MASK,   2,        NULL,       NORMAL,    { NEW_TAB } },
};

/* commands */
Command commands[] = {
  /* command,   abbreviation,   function,            completion,   description  */
  {"back",      0,              cmd_back,            0,            "Go back in the browser history" },
  {"bmap",      0,              cmd_bmap,            0,            "Map a buffered command" },
  {"bmark",     "b",            cmd_bookmark,        0,            "Add a bookmark" },
  {"forward",   "f",            cmd_forward,         0,            "Go forward in the browser history" },
  {"map",       "m",            cmd_map,             0,            "Map a key sequence" },
  {"open",      "o",            cmd_open,            cc_open,      "Open URI in the current tab" },
  {"plugin",    0,              cmd_plugintype,      0,            "Allow plugin type" },
  {"quit",      "q",            cmd_quit,            0,            "Quit current tab" },
  {"quitall",   0,              cmd_quitall,         0,            "Quit jumanji" },
  {"reload",    0,              cmd_reload,          0,            "Reload the current web page" },
  {"reloadall", 0,              cmd_reload_all,      0,            "Reload all tab pages" },
  {"saveas",    0,              cmd_saveas,          0,            "Save current document to disk" },
  {"script",    0,              cmd_script,          0,            "Load a javascript file" },
  {"set",       "s",            cmd_set,             cc_set,       "Set an option" },
  {"stop",      "st",           cmd_stop,            0,            "Stop loading the current page" },
  {"tabopen",   "t",            cmd_tabopen,         cc_open,      "Open URI in a new tab" },
  {"winopen",   "w",            cmd_winopen,         cc_open,      "Open URI in a new window" },
  {"write",     "w",            cmd_write,           0,            "Write bookmark and history file" },
};

/* buffer commands */
BufferCommand buffer_commands[] = {
  /* regex,        function,       argument */
  {"^gh$",         bcmd_go_home,   {0} },
  {"^gH$",         bcmd_go_home,   { NEW_TAB } },
  {"^[0-9]*gu$",   bcmd_go_parent, {0} },
  {"^gU$",         bcmd_go_parent, {0} },
  {"^gt$",         bcmd_nav_tabs,  { NEXT } },
  {"^gT$",         bcmd_nav_tabs,  { PREVIOUS } },
  {"^gP$",         bcmd_paste,     { NEW_TAB } },
  {"^[0-9]+gt$",   bcmd_nav_tabs,  { SPECIFIC } },
  {"^[0-9]+gT$",   bcmd_nav_tabs,  { SPECIFIC } },
  {"^ZZ$",         bcmd_quit,      {0} },
  {"^ZQ$",         bcmd_quit,      {0} },
  {"^[0-9]+%$",    bcmd_scroll,    {0} },
  {"^[0-9]+G$",    bcmd_scroll,    {0} },
  {"^gg$",         bcmd_scroll,    { TOP } },
  {"^G$",          bcmd_scroll,    { BOTTOM } },
  {"^zI$",         bcmd_zoom,      { ZOOM_IN } },
  {"^zO$",         bcmd_zoom,      { ZOOM_OUT } },
  {"^z0$",         bcmd_zoom,      { ZOOM_ORIGINAL } },
  {"^[0-9]+Z$",    bcmd_zoom,      { SPECIFIC } },
};

/* special commands */
SpecialCommand special_commands[] = {
  /* identifier,   function,      a,   argument */
  {'/',            scmd_search,   1,   { DOWN } },
  {'?',            scmd_search,   1,   { UP } },
};

/* settings */
Setting settings[] = {
  /* name,                   variable,                  webkit-setting,                 type, i, r, description */
  {"96dpi",                  NULL,                      "enforce-96-dpi",               'b',  0, 1, "Enforce a resolution of 96 DPI"},
  {"auto_save",              &(auto_save_interval),     NULL,                           'i',  1, 0, "Autosave bookmarks and history"},
  {"auto_shrink_images",     NULL,                      "auto-shrink-images",           'b',  0, 1, "Shrink standalone images to fit"},
  {"background",             NULL,                      "print-backgrounds",            'b',  0, 1, "Print background images"},
  {"caret_browsing",         NULL,                      "enable-caret-browsing",        'b',  0, 1, "Wheter to enable caret browsing mode"},
  {"completion_bgcolor",     &(completion_bgcolor),     NULL,                           's',  1, 0, "Completion background color"},
  {"completion_fgcolor",     &(completion_fgcolor),     NULL,                           's',  1, 0, "Completion foreground color"},
  {"completion_g_bgcolor",   &(completion_g_bgcolor),   NULL,                           's',  1, 0, "Completion (group) background color"},
  {"completion_g_fgcolor",   &(completion_g_fgcolor),   NULL,                           's',  1, 0, "Completion (group) foreground color"},
  {"completion_hl_bgcolor",  &(completion_hl_bgcolor),  NULL,                           's',  1, 0, "Completion (highlight) background color"},
  {"completion_hl_fgcolor",  &(completion_hl_fgcolor),  NULL,                           's',  1, 0, "Completion (highlight) foreground color"},
  {"n_completion_items",     &(n_completion_items),     NULL,                           'i',  0, 0, "Number of completion items"},
  {"cursive_font",           NULL,                      "cursive-font-family",          's',  0, 1, "Default cursive font family to display text"},
  {"default_bgcolor",        &(default_bgcolor),        NULL,                           's',  1, 0, "Default background color"},
  {"default_fgcolor",        &(default_fgcolor),        NULL,                           's',  1, 0, "Default foreground color"},
  {"developer_extras",       NULL,                      "enable-developer-extras",      'b',  0, 1, "Enable webkit developer extensions"},
  {"download_dir",           &(download_dir),           NULL,                           's',  0, 0, "The default download directory"},
  {"download_command",       &(download_command),       NULL,                           's',  0, 0, "Command for downloading files"},
  {"encoding",               NULL,                      "default-encoding",             's',  0, 1, "The default encoding to display text"},
  {"fantasy_font",           NULL,                      "fantasy-font-family",          's',  0, 1, "The default fantasy font family"},
  {"font",                   &(font),                   NULL,                           's',  1, 0, "The used font" },
  {"font_family",            NULL,                      "default-font-family",          's',  0, 1, "The default font family"},
  {"font_size",              NULL,                      "default-font-size",            'i',  0, 1, "The default font size to display text"},
  {"height",                 &(default_height),         NULL,                           'i',  1, 0, "Default window height"},
  {"homepage",               &(home_page),              NULL,                           's',  0, 0, "Home page"},
  {"images",                 NULL,                      "auto-load-images",             'b',  0, 1, "Load images automatically"},
  {"inputbar_bgcolor",       &(inputbar_bgcolor),       NULL,                           's',  1, 0, "Inputbar background color"},
  {"inputbar_fgcolor",       &(inputbar_fgcolor),       NULL,                           's',  1, 0, "Inputbar foreground color"},
  {"java_applet",            NULL,                      "enable-java-applet",           'b',  0, 1, "Enable Java <applet> tag"},
  {"minimum_font_size",      NULL,                      "minimum-font-size",            'i',  0, 1, "Minimum font-size"},
  {"monospace_font",         NULL,                      "monospace-font-family",        's',  0, 1, "Monospace font family"},
  {"monospace_font_size",    NULL,                      "default-monospace-font-size",  'i',  0, 1, "The default font size to display monospace text"},
  {"next_to_current",        &(next_to_current),        NULL,                           'b',  0, 0, "Open new tab next to the current one"},
  {"notification_e_bgcolor", &(notification_e_bgcolor), NULL,                           's',  1, 0, "Notification (error) background color"},
  {"notification_e_fgcolor", &(notification_e_fgcolor), NULL,                           's',  1, 0, "Notification (error) foreground color"},
  {"notification_w_bgcolor", &(notification_w_bgcolor), NULL,                           's',  1, 0, "Notification (warning) background color"},
  {"notification_w_fgcolor", &(notification_w_fgcolor), NULL,                           's',  1, 0, "Notification (warning) foreground color"},
  {"page_cache",             NULL,                      "enable-page-cache",            'b',  0, 1, "Enable page cache"},
  {"plugin_blocker",         &(plugin_blocker),         NULL,                           'b',  0, 1, "Enable plugin-blocker"},
  {"plugins",                NULL,                      "enable-plugins",               'b',  0, 1, "Enable embedded plugin objects"},
  {"private_browsing",       &(private_browsing),       "enable-private-browsing",      'b',  0, 0, "Enable private browsing"},
  {"proxy",                  &(proxy),                  NULL,                           's',  0, 0, "Set proxy"},
  {"resizable_text_areas",   NULL,                      "resizable-text-areas",         'b',  0, 1, "Resizable text areas"},
  {"sans_serif_font",        NULL,                      "sans-serif-font-family",       's',  0, 1, "Sans-serif font family"},
  {"scripts",                NULL,                      "enable-scripts",               'b',  0, 1, "Enable embedded scripting languages"},
  {"scroll_step",            &(scroll_step),            NULL,                           'f',  1, 0, "Scroll step"},
  {"scrollbars",             &(show_scrollbars),        NULL,                           'b',  0, 0, "Show scrollbars"},
  {"serif_font",             NULL,                      "serif-font-family",            's',  0, 1, "Serif font family"},
  {"statusbar",              &(show_statusbar),         NULL,                           'b',  0, 0, "Show statusbar"},
  {"spell_checking",         NULL,                      "enable-spell-checking",        'b',  0, 1, "Enable spell checking while typing"},
  {"spell_checking_lang",    NULL,                      "spell-checking-languages",     's',  0, 1, "Spell checking languages"},
  {"statusbar_bgcolor",      &(statusbar_bgcolor),      NULL,                           's',  1, 0, "Statusbar background color"},
  {"statusbar_fgcolor",      &(statusbar_fgcolor),      NULL,                           's',  1, 0, "Statusbar foreground color"},
  {"statusbar_ssl_fgcolor",  &(statusbar_ssl_bgcolor),  NULL,                           's',  1, 0, "Statusbar (SSL) background color"},
  {"statusbar_ssl_fgcolor",  &(statusbar_ssl_fgcolor),  NULL,                           's',  1, 0, "Statusbar (SSL) foreground color"},
  {"stylesheet",             NULL,                      "user-stylesheet-uri",          's',  0, 1, "Custom stylesheet"},
  {"tabbar",                 &(show_tabbar),            NULL,                           'b',  0, 0, "Show tabbar"},
  {"tabbar_bgcolor",         &(tabbar_bgcolor),         NULL,                           's',  1, 0, "Tabbar background color"},
  {"tabbar_fgcolor",         &(tabbar_fgcolor),         NULL,                           's',  1, 0, "Tabbar foreground color"},
  {"tabbar_focus_bgcolor",   &(tabbar_focus_bgcolor),   NULL,                           's',  1, 0, "Tabbar (focus) background color"},
  {"tabbar_focus_fgcolor",   &(tabbar_focus_fgcolor),   NULL,                           's',  1, 0, "Tabbar (focus) foreground color"},
  {"tabbar_separator_color", &(tabbar_separator_color), NULL,                           's',  1, 0, "Tabbar separator color"},
  {"user_agent",             &(user_agent),             "user-agent",                   's',  1, 1, "User agent"},
  {"width",                  &(default_width),          NULL,                           'i',  1, 0, "Default window width"},
  {"zoom_step",              &(zoom_step),              "zoom-step",                    'f',  0, 0, "Zoom step"},
};

/* shortcut names */
FunctionName function_names[] = {
  {"abort",             sc_abort,              NULL},
  {"change_buffer",     sc_change_buffer,      NULL},
  {"change_mode",       sc_change_mode,        NULL},
  {"close_tab",         sc_close_tab,          bcmd_close_tab},
  {"focus_inputbar",    sc_focus_inputbar,     NULL},
  {"follow_link",       sc_follow_link,        NULL},
  {"goto",              NULL,                  NULL},
  {"nav_history",       sc_nav_history,        bcmd_nav_history},
  {"nav_tabs",          sc_nav_tabs,           bcmd_nav_tabs},
  {"paste",             sc_paste,              NULL},
  {"proxy",             sc_toggle_proxy,       NULL},
  {"reload",            sc_reload,             NULL},
  {"script",            sc_run_script,         NULL},
  {"scroll",            sc_scroll,             bcmd_scroll},
  {"search",            sc_search,             NULL},
  {"toggle_statusbar",  sc_toggle_statusbar,   NULL},
  {"toggle_sourcecode", sc_toggle_sourcecode,  NULL},
  {"quit",              sc_quit,               bcmd_quit},
  {"yank",              sc_yank,               NULL},
  {"zoom",              sc_zoom,               bcmd_zoom},
};

/* argument names */
ArgumentName argument_names[] = {
  {"append_url",   APPEND_URL},
  {"backward",     BACKWARD},
  {"bottom",       BOTTOM},
  {"bypass_cache", BYPASS_CACHE},
  {"down",         DOWN},
  {"forward",      FORWARD},
  {"full_down",    FULL_DOWN},
  {"full_up",      FULL_UP},
  {"half_down",    HALF_DOWN},
  {"half_up",      HALF_UP},
  {"in",           ZOOM_IN},
  {"left",         LEFT},
  {"new_tab",      NEW_TAB},
  {"next",         NEXT},
  {"original",     ZOOM_ORIGINAL},
  {"out",          ZOOM_OUT},
  {"previous",     PREVIOUS},
  {"right",        RIGHT},
  {"top",          TOP},
  {"up",           UP},
};

/* mode names */
ModeName mode_names[] = {
  {"all",          ALL},
  {"insert",       INSERT},
  {"visual",       VISUAL},
  {"follow",       FOLLOW},
  {"pass_through", PASS_THROUGH},
};

/* special keys */
GDKKey gdk_keys[] = {
  {"<BackSpace>", GDK_BackSpace},
  {"<CapsLock>",  GDK_Caps_Lock},
  {"<Down>",      GDK_Down},
  {"<Esc>",       GDK_Escape},
  {"<F10>",       GDK_F10},
  {"<F11>",       GDK_F11},
  {"<F12>",       GDK_F12},
  {"<F1>",        GDK_F1},
  {"<F2>",        GDK_F2},
  {"<F3>",        GDK_F3},
  {"<F4>",        GDK_F4},
  {"<F5>",        GDK_F5},
  {"<F6>",        GDK_F6},
  {"<F7>",        GDK_F7},
  {"<F8>",        GDK_F8},
  {"<F9>",        GDK_F9},
  {"<Left>",      GDK_Left},
  {"<PageDown>",  GDK_Page_Down},
  {"<PageUp>",    GDK_Page_Up},
  {"<Return>",    GDK_Return},
  {"<Right>",     GDK_Right},
  {"<Space>",     GDK_space},
  {"<Tab>",       GDK_Tab},
  {"<Up>",        GDK_Up},
};
