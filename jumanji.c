/* See LICENSE file for license and copyright information */

#include <regex.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <webkit/webkit.h>

/* macros */
#define LENGTH(x) sizeof(x)/sizeof((x)[0])
#define CLEAN(m) (m & ~(GDK_MOD2_MASK) & ~(GDK_BUTTON1_MASK) & ~(GDK_BUTTON2_MASK) & ~(GDK_BUTTON3_MASK) & ~(GDK_BUTTON4_MASK) & ~(GDK_BUTTON5_MASK))
#define GET_CURRENT_TAB_WIDGET() GET_NTH_TAB_WIDGET(gtk_notebook_get_current_page(Jumanji.UI.view))
#define GET_NTH_TAB_WIDGET(n) GTK_SCROLLED_WINDOW(gtk_notebook_get_nth_page(Jumanji.UI.view, n))
#define GET_CURRENT_TAB() GET_NTH_TAB(gtk_notebook_get_current_page(Jumanji.UI.view))
#define GET_NTH_TAB(n) GET_WEBVIEW(gtk_notebook_get_nth_page(Jumanji.UI.view, n))
#define GET_WEBVIEW(x) WEBKIT_WEB_VIEW(gtk_bin_get_child(GTK_BIN(x)))

/* enums */
enum {
  ADD_MARKER,
  APPEND_URL,
  BOTTOM,
  DEFAULT,
  DELETE_LAST_CHAR,
  DELETE_LAST_WORD,
  DOWN,
  ERROR,
  EVAL_MARKER,
  FULL_DOWN,
  FULL_UP,
  HALF_DOWN,
  HALF_UP,
  HIDE,
  HIGHLIGHT,
  LEFT,
  NEXT,
  NEXT_CHAR,
  NEXT_GROUP,
  NORMAL,
  PREVIOUS,
  PREVIOUS_CHAR,
  PREVIOUS_GROUP,
  RIGHT,
  SPECIFIC,
  TOP,
  UP,
  WARNING,
  ZOOM_IN,
  ZOOM_ORIGINAL,
  ZOOM_OUT
};

/* define modes */
#define ALL        (1 << 0)
#define INSERT     (1 << 1)
#define VISUAL     (1 << 2)

/* typedefs */
struct CElement
{
  char            *value;
  char            *description;
  struct CElement *next;
};

typedef struct CElement CompletionElement;

struct CGroup
{
  char              *value;
  CompletionElement *elements;
  struct CGroup     *next;
};

typedef struct CGroup CompletionGroup;

typedef struct
{
  CompletionGroup* groups;
} Completion;

typedef struct
{
  char*      command;
  char*      description;
  int        command_id;
  gboolean   is_group;
  GtkWidget* row;
} CompletionRow;

typedef struct
{
  int   n;
  void *data;
} Argument;

typedef struct
{
  char* name;
  int argument;
} ArgumentName;

typedef struct
{
  int mask;
  int key;
  void (*function)(Argument*);
  int mode;
  Argument argument;
} Shortcut;

typedef struct
{
  char* name;
  int mode;
} ModeName;

typedef struct
{
  char* name;
  void (*function)(Argument*);
} ShortcutName;

typedef struct
{
  int mask;
  int key;
  void (*function)(Argument*);
  Argument argument;
} InputbarShortcut;

typedef struct
{
  char* command;
  char* abbr;
  gboolean (*function)(int, char**);
  Completion* (*completion)(char*);
  char* description;
} Command;

typedef struct
{
  char* regex;
  void (*function)(char*, Argument*);
  Argument argument;
} BufferCommand;

typedef struct
{
  char identifier;
  gboolean (*function)(char*, Argument*);
  int always;
  Argument argument;
} SpecialCommand;

struct SCList
{
  Shortcut       element;
  struct SCList *next;
};

typedef struct SCList ShortcutList;

typedef struct
{
  char* identifier;
  int   key;
} GDKKey;

typedef struct
{
  char* name;
  void* variable;
  char  type;
  gboolean reinit;
  char* description;
} Setting;

struct SEList
{
  char* name;
  char* uri;
  struct SEList *next;
};

typedef struct SEList SearchEngineList;

typedef struct
{
  GtkScrolledWindow *view;
} Page;

/* jumanji */
struct
{
  struct
  {
    GtkWidget       *window;
    GtkBox          *box;
    GtkWidget       *statusbar;
    GtkBox          *statusbar_entries;
    GtkEntry        *inputbar;
    GtkNotebook     *view;
    GdkNativeWindow  embed;
    char            *winid;
  } UI;

  struct
  {
    GdkColor default_fg;
    GdkColor default_bg;
    GdkColor inputbar_fg;
    GdkColor inputbar_bg;
    GdkColor statusbar_fg;
    GdkColor statusbar_bg;
    GdkColor completion_fg;
    GdkColor completion_bg;
    GdkColor completion_g_bg;
    GdkColor completion_g_fg;
    GdkColor completion_hl_fg;
    GdkColor completion_hl_bg;
    GdkColor notification_e_fg;
    GdkColor notification_e_bg;
    GdkColor notification_w_fg;
    GdkColor notification_w_bg;
    PangoFontDescription *font;
  } Style;

  struct
  {
    GString *buffer;
    GList   *history;
    int      mode;
    SearchEngineList *search_engines;
    char   **arguments;
  } Global;

  struct
  {
    GtkLabel *uri;
    GtkLabel *buffer;
    GtkLabel *tabs;
  } Statusbar;

  struct
  {
    ShortcutList  *sclist;
  } Bindings;

} Jumanji;

/* function declarations */
void add_marker(int);
void change_mode(int);
void create_tab(char*, int);
void eval_marker(int);
void init_directories();
void init_jumanji();
void init_keylist();
void init_look();
void init_settings();
void notify(int, char*);
void out_of_memory();
void open_uri(WebKitWebView*, char*);
void read_configuration();
void set_completion_row_color(GtkBox*, int, int);
void switch_view(GtkWidget*);
void update_status();
GtkEventBox* create_completion_row(GtkBox*, char*, char*, gboolean);

Completion* completion_init();
CompletionGroup* completion_group_create(char*);
void completion_add_group(Completion*, CompletionGroup*);
void completion_free(Completion*);
void completion_group_add_element(CompletionGroup*, char*, char*);

/* shortcut declarations */
void sc_abort(Argument*);
void sc_close_tab(Argument*);
void sc_focus_inputbar(Argument*);
void sc_nav_history(Argument*);
void sc_nav_tabs(Argument*);
void sc_reload(Argument*);
void sc_scroll(Argument*);
void sc_toggle_statusbar(Argument*);
void sc_quit(Argument*);
void sc_yank(Argument*);
void sc_zoom(Argument*);

/* inputbar shortcut declarations */
void isc_abort(Argument*);
void isc_completion(Argument*);
void isc_command_history(Argument*);
void isc_string_manipulation(Argument*);

/* command declarations */
gboolean cmd_back(int, char**);
gboolean cmd_forward(int, char**);
gboolean cmd_map(int, char**);
gboolean cmd_open(int, char**);
gboolean cmd_quit(int, char**);
gboolean cmd_quitall(int, char**);
gboolean cmd_search_engine(int, char**);
gboolean cmd_set(int, char**);
gboolean cmd_tabopen(int, char**);
gboolean cmd_winopen(int, char**);

/* completion commands */
Completion* cc_open(char*);
Completion* cc_set(char*);

/* buffer command declarations */
void bcmd_goto(char*, Argument*);
void bcmd_nav_tabs(char*, Argument*);
void bcmd_scroll(char*, Argument*);
void bcmd_zoom(char*, Argument*);

/* special command delcarations */
gboolean scmd_search(char*, Argument*);

/* callback declarations */
gboolean cb_destroy(GtkWidget*, gpointer);
gboolean cb_inputbar_kb_pressed(GtkWidget*, GdkEventKey*, gpointer);
gboolean cb_inputbar_activate(GtkEntry*, gpointer);
gboolean cb_tab_kb_pressed(GtkWidget*, GdkEventKey*, gpointer);
gboolean cb_wv_load_finished(WebKitWebView*, WebKitWebFrame*, gpointer);
gboolean cb_wv_load_progress_changed(WebKitWebView*, int, gpointer);
gboolean cb_wv_nav_policy_decision(WebKitWebView*, WebKitWebFrame*, WebKitNetworkRequest*, WebKitWebNavigationAction*, WebKitWebPolicyDecision*, gpointer);

/* configuration */
#include "config.h"

/* function implementation */
void
add_marker(int id)
{

}

void
change_mode(int mode)
{
  char* mode_text;

  switch(mode)
  {
    case INSERT:
      mode_text = "-- INSERT --";
      break;
    case VISUAL:
      mode_text = "-- VISUAL --";
      break;
    case ADD_MARKER:
      mode_text = "";
      break;
    case EVAL_MARKER:
      mode_text = "";
      break;
    default:
      mode_text = "";
      mode      = NORMAL;
      break;
  }

  Jumanji.Global.mode = mode;
  notify(DEFAULT, mode_text);
}

void
create_tab(char* uri, int position)
{
  GtkWidget *tab = gtk_scrolled_window_new(NULL, NULL);
  GtkWidget *wv  = webkit_web_view_new();

  if(!tab || !wv)
    return;

  if(show_scrollbars)
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(tab), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  else
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(tab), GTK_POLICY_NEVER, GTK_POLICY_NEVER);

  g_signal_connect(G_OBJECT(wv), "load-finished",                        G_CALLBACK(cb_wv_load_finished),         NULL);
  g_signal_connect(G_OBJECT(wv), "load-progress-changed",                G_CALLBACK(cb_wv_load_progress_changed), NULL);
  g_signal_connect(G_OBJECT(wv), "navigation-policy-decision-requested", G_CALLBACK(cb_wv_nav_policy_decision),   NULL);
  g_signal_connect(G_OBJECT(wv), "new-window-policy-decision-requested", G_CALLBACK(cb_wv_nav_policy_decision),   NULL);

  g_signal_connect(G_OBJECT(tab), "key-press-event", G_CALLBACK(cb_tab_kb_pressed), NULL);
  open_uri(WEBKIT_WEB_VIEW(wv), uri);

  gtk_container_add(GTK_CONTAINER(tab), wv);
  gtk_widget_show_all(tab);
  gtk_notebook_insert_page(Jumanji.UI.view, tab, NULL, position);
  gtk_notebook_set_current_page(Jumanji.UI.view, -1);
  gtk_widget_grab_focus(GTK_WIDGET(GET_CURRENT_TAB_WIDGET()));

  update_status();
}


void
eval_marker(int id)
{

}

void
init_look()
{
  /* parse  */
  gdk_color_parse(default_fgcolor,        &(Jumanji.Style.default_fg));
  gdk_color_parse(default_bgcolor,        &(Jumanji.Style.default_bg));
  gdk_color_parse(inputbar_fgcolor,       &(Jumanji.Style.inputbar_fg));
  gdk_color_parse(inputbar_bgcolor,       &(Jumanji.Style.inputbar_bg));
  gdk_color_parse(statusbar_fgcolor,      &(Jumanji.Style.statusbar_fg));
  gdk_color_parse(statusbar_bgcolor,      &(Jumanji.Style.statusbar_bg));
  gdk_color_parse(completion_fgcolor,     &(Jumanji.Style.completion_fg));
  gdk_color_parse(completion_bgcolor,     &(Jumanji.Style.completion_bg));
  gdk_color_parse(completion_g_fgcolor,   &(Jumanji.Style.completion_g_fg));
  gdk_color_parse(completion_g_fgcolor,   &(Jumanji.Style.completion_g_fg));
  gdk_color_parse(completion_hl_fgcolor,  &(Jumanji.Style.completion_hl_fg));
  gdk_color_parse(completion_hl_bgcolor,  &(Jumanji.Style.completion_hl_bg));
  gdk_color_parse(notification_e_fgcolor, &(Jumanji.Style.notification_e_fg));
  gdk_color_parse(notification_e_bgcolor, &(Jumanji.Style.notification_e_bg));
  gdk_color_parse(notification_w_fgcolor, &(Jumanji.Style.notification_w_fg));
  gdk_color_parse(notification_w_bgcolor, &(Jumanji.Style.notification_w_bg));
  Jumanji.Style.font = pango_font_description_from_string(font);

  /* statusbar */
  gtk_widget_modify_bg(GTK_WIDGET(Jumanji.UI.statusbar), GTK_STATE_NORMAL, &(Jumanji.Style.statusbar_bg));

  gtk_widget_modify_fg(GTK_WIDGET(Jumanji.Statusbar.uri),    GTK_STATE_NORMAL, &(Jumanji.Style.statusbar_fg));
  gtk_widget_modify_fg(GTK_WIDGET(Jumanji.Statusbar.buffer), GTK_STATE_NORMAL, &(Jumanji.Style.statusbar_fg));
  gtk_widget_modify_fg(GTK_WIDGET(Jumanji.Statusbar.tabs),   GTK_STATE_NORMAL, &(Jumanji.Style.statusbar_fg));

  gtk_widget_modify_font(GTK_WIDGET(Jumanji.Statusbar.uri),    Jumanji.Style.font);
  gtk_widget_modify_font(GTK_WIDGET(Jumanji.Statusbar.buffer), Jumanji.Style.font);
  gtk_widget_modify_font(GTK_WIDGET(Jumanji.Statusbar.tabs),   Jumanji.Style.font);

  if(show_statusbar)
    gtk_widget_show(GTK_WIDGET(Jumanji.UI.statusbar));
  else
    gtk_widget_hide(GTK_WIDGET(Jumanji.UI.statusbar));

  /* inputbar */
  gtk_widget_modify_base(GTK_WIDGET(Jumanji.UI.inputbar), GTK_STATE_NORMAL, &(Jumanji.Style.inputbar_bg));
  gtk_widget_modify_text(GTK_WIDGET(Jumanji.UI.inputbar), GTK_STATE_NORMAL, &(Jumanji.Style.inputbar_fg));
  gtk_widget_modify_font(GTK_WIDGET(Jumanji.UI.inputbar),                     Jumanji.Style.font);

  g_signal_connect(G_OBJECT(Jumanji.UI.inputbar), "key-press-event", G_CALLBACK(cb_inputbar_kb_pressed), NULL);
  g_signal_connect(G_OBJECT(Jumanji.UI.inputbar), "activate",        G_CALLBACK(cb_inputbar_activate),   NULL);

}

void
init_directories()
{
  /* create jumanji directory */
  gchar *base_directory = g_build_filename(g_get_home_dir(), JUMANJI_DIR, NULL);
  g_mkdir_with_parents(base_directory,  0771);
  g_free(base_directory);
}

void
init_keylist()
{
  ShortcutList* e = NULL;
  ShortcutList* p = NULL;

  int i;
  for(i = 0; i < LENGTH(shortcuts); i++)
  {
    e = malloc(sizeof(ShortcutList));
    if(!e)
      out_of_memory();

    e->element = shortcuts[i];
    e->next    = NULL;

    if(!Jumanji.Bindings.sclist)
      Jumanji.Bindings.sclist = e;
    if(p)
      p->next = e;

    p = e;
  }
}

void
init_settings()
{
  gtk_window_set_default_size(GTK_WINDOW(Jumanji.UI.window), default_width, default_height);
}

void notify(int level, char* message)
{
  if(!(GTK_WIDGET_VISIBLE(GTK_WIDGET(Jumanji.UI.inputbar))))
    gtk_widget_show(GTK_WIDGET(Jumanji.UI.inputbar));

  switch(level)
  {
    case ERROR:
      gtk_widget_modify_base(GTK_WIDGET(Jumanji.UI.inputbar), GTK_STATE_NORMAL, &(Jumanji.Style.notification_e_bg));
      gtk_widget_modify_text(GTK_WIDGET(Jumanji.UI.inputbar), GTK_STATE_NORMAL, &(Jumanji.Style.notification_e_fg));
      break;
    case WARNING:
      gtk_widget_modify_base(GTK_WIDGET(Jumanji.UI.inputbar), GTK_STATE_NORMAL, &(Jumanji.Style.notification_w_bg));
      gtk_widget_modify_text(GTK_WIDGET(Jumanji.UI.inputbar), GTK_STATE_NORMAL, &(Jumanji.Style.notification_w_fg));
      break;
    default:
      gtk_widget_modify_base(GTK_WIDGET(Jumanji.UI.inputbar), GTK_STATE_NORMAL, &(Jumanji.Style.inputbar_bg));
      gtk_widget_modify_text(GTK_WIDGET(Jumanji.UI.inputbar), GTK_STATE_NORMAL, &(Jumanji.Style.inputbar_fg));
      break;
  }

  if(message)
    gtk_entry_set_text(Jumanji.UI.inputbar, message);
}

void
init_jumanji()
{
  /* other */
  Jumanji.Global.mode           = NORMAL;
  Jumanji.Global.search_engines = NULL;

  /* window */
  if(Jumanji.UI.embed)
    Jumanji.UI.window = gtk_plug_new(Jumanji.UI.embed);
  else
    Jumanji.UI.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  /* UI */
  Jumanji.UI.box               = GTK_BOX(gtk_vbox_new(FALSE, 0));
  Jumanji.UI.statusbar         = gtk_event_box_new();
  Jumanji.UI.statusbar_entries = GTK_BOX(gtk_hbox_new(FALSE, 0));
  Jumanji.UI.inputbar          = GTK_ENTRY(gtk_entry_new());
  Jumanji.UI.view              = GTK_NOTEBOOK(gtk_notebook_new());

  /* window */
  gtk_window_set_title(GTK_WINDOW(Jumanji.UI.window), "jumanji");
  GdkGeometry hints = { 1, 1 };
  gtk_window_set_geometry_hints(GTK_WINDOW(Jumanji.UI.window), NULL, &hints, GDK_HINT_MIN_SIZE);
  g_signal_connect(G_OBJECT(Jumanji.UI.window), "destroy", G_CALLBACK(cb_destroy), NULL);

  /* box */
  gtk_box_set_spacing(Jumanji.UI.box, 0);
  gtk_container_add(GTK_CONTAINER(Jumanji.UI.window), GTK_WIDGET(Jumanji.UI.box));

  /* statusbar */
  Jumanji.Statusbar.uri    = GTK_LABEL(gtk_label_new(NULL));
  Jumanji.Statusbar.buffer = GTK_LABEL(gtk_label_new(NULL));
  Jumanji.Statusbar.tabs   = GTK_LABEL(gtk_label_new(NULL));

  gtk_misc_set_alignment(GTK_MISC(Jumanji.Statusbar.uri),    0.0, 0.0);
  gtk_misc_set_alignment(GTK_MISC(Jumanji.Statusbar.buffer), 1.0, 0.0);
  gtk_misc_set_alignment(GTK_MISC(Jumanji.Statusbar.tabs),   1.0, 0.0);

  gtk_misc_set_padding(GTK_MISC(Jumanji.Statusbar.uri),    2.0, 4.0);
  gtk_misc_set_padding(GTK_MISC(Jumanji.Statusbar.buffer), 2.0, 4.0);
  gtk_misc_set_padding(GTK_MISC(Jumanji.Statusbar.tabs),   2.0, 4.0);

  gtk_label_set_use_markup(Jumanji.Statusbar.uri,    TRUE);
  gtk_label_set_use_markup(Jumanji.Statusbar.buffer, TRUE);
  gtk_label_set_use_markup(Jumanji.Statusbar.tabs,   TRUE);

  gtk_box_pack_start(Jumanji.UI.statusbar_entries, GTK_WIDGET(Jumanji.Statusbar.uri),    TRUE,   TRUE, 2);
  gtk_box_pack_start(Jumanji.UI.statusbar_entries, GTK_WIDGET(Jumanji.Statusbar.buffer), FALSE, FALSE, 2);
  gtk_box_pack_start(Jumanji.UI.statusbar_entries, GTK_WIDGET(Jumanji.Statusbar.tabs),   FALSE, FALSE, 2);

  gtk_container_add(GTK_CONTAINER(Jumanji.UI.statusbar), GTK_WIDGET(Jumanji.UI.statusbar_entries));

  /* inputbar */
  gtk_entry_set_inner_border(Jumanji.UI.inputbar, NULL);
  gtk_entry_set_has_frame(   Jumanji.UI.inputbar, FALSE);
  gtk_editable_set_editable( GTK_EDITABLE(Jumanji.UI.inputbar), TRUE);

  /* view */
  gtk_notebook_set_show_tabs(Jumanji.UI.view,   FALSE);
  gtk_notebook_set_show_border(Jumanji.UI.view, FALSE);

  /* packing */
  gtk_box_pack_start(Jumanji.UI.box, GTK_WIDGET(Jumanji.UI.view),       TRUE,  TRUE, 0);
  gtk_box_pack_start(Jumanji.UI.box, GTK_WIDGET(Jumanji.UI.statusbar), FALSE, FALSE, 0);
  gtk_box_pack_end(  Jumanji.UI.box, GTK_WIDGET(Jumanji.UI.inputbar),  FALSE, FALSE, 0);
}

void
out_of_memory()
{
  printf("error: out of memory\n");
  exit(-1);
}

void
open_uri(WebKitWebView* web_view, char* uri)
{
  if(!uri)
    return;

  while (*uri == ' ')
    uri++;

  gchar **tokens = g_strsplit(uri, " ", -1);
  int     length = g_strv_length(tokens);
  gchar* new_uri = "";

  /* check search engine */
  if(length > 1)
  {
    SearchEngineList* se = Jumanji.Global.search_engines;
    while(se)
    {
      if(!strcmp(tokens[0], se->name))
      {
        char* searchitem = uri + strlen(tokens[0]) + 1;
        new_uri   = g_strdup_printf(se->uri, searchitem);
        break;
      }

      se = se->next;
    }
  }
  else
    new_uri = g_strrstr(uri, ":") ? g_strdup(uri) : g_strconcat("http://", uri, NULL);

  webkit_web_view_load_uri(web_view, new_uri);
  g_free(new_uri);
}

void
update_status()
{
  if(!gtk_notebook_get_n_pages(Jumanji.UI.view))
    return;

  /* update uri */
  gchar* link = (gchar*) webkit_web_view_get_uri(GET_CURRENT_TAB());
  int progress = (int) g_object_get_data(G_OBJECT(GET_CURRENT_TAB()), "progress");
  gchar* ptext = (progress != 100 && progress != 0) ? g_strdup_printf(" (%d%%)", progress) : g_strdup("");
  gchar* uri   = g_strdup_printf("%s%s", link ? link : "[No name]", ptext);
  g_free(ptext);

  gtk_label_set_markup((GtkLabel*) Jumanji.Statusbar.uri, uri ? uri : "");
  g_free(uri);

  /* update title */
  const gchar* title = webkit_web_view_get_title(GET_CURRENT_TAB());
  if(title)
    gtk_window_set_title(GTK_WINDOW(Jumanji.UI.window), title);
  else
    gtk_window_set_title(GTK_WINDOW(Jumanji.UI.window), "jumanji");

  /* update tab position */
  int current_tab     = gtk_notebook_get_current_page(Jumanji.UI.view);
  int number_of_tabs  = gtk_notebook_get_n_pages(Jumanji.UI.view);

  gchar* tabs = g_strdup_printf("[%d/%d]", current_tab + 1, number_of_tabs);
  gtk_label_set_markup((GtkLabel*) Jumanji.Statusbar.tabs, tabs);
  g_free(tabs);
}

void
read_configuration()
{
  char* jumanjirc = g_strdup_printf("%s/%s/%s", g_get_home_dir(), JUMANJI_DIR, JUMANJI_RC);

  if(!jumanjirc)
    return;

  if(g_file_test(jumanjirc, G_FILE_TEST_IS_REGULAR))
  {
    char* content = NULL;

    if(g_file_get_contents(jumanjirc, &content, NULL, NULL))
    {
      gchar **lines = g_strsplit(content, "\n", -1);
      int     n     = g_strv_length(lines) - 1;

      int i;
      for(i = 0; i < n; i++)
      {
        if(!strlen(lines[i]))
          continue;

        gchar **tokens = g_strsplit(lines[i], " ", -1);
        int     length = g_strv_length(tokens);

        if(!strcmp(tokens[0], "set"))
          cmd_set(length - 1, tokens + 1);
        else if(!strcmp(tokens[0], "map"))
          cmd_map(length - 1, tokens + 1);
        else if(!strcmp(tokens[0], "searchengine"))
          cmd_search_engine(length - 1, tokens + 1);
      }
    }
  }

  g_free(jumanjirc);
}

void
set_completion_row_color(GtkBox* results, int mode, int id)
{
  GtkEventBox *row   = (GtkEventBox*) g_list_nth_data(gtk_container_get_children(GTK_CONTAINER(results)), id);

  if(row)
  {
    GtkBox      *col   = (GtkBox*)      g_list_nth_data(gtk_container_get_children(GTK_CONTAINER(row)), 0);
    GtkLabel    *cmd   = (GtkLabel*)    g_list_nth_data(gtk_container_get_children(GTK_CONTAINER(col)), 0);
    GtkLabel    *cdesc = (GtkLabel*)    g_list_nth_data(gtk_container_get_children(GTK_CONTAINER(col)), 1);

    if(mode == NORMAL)
    {
      gtk_widget_modify_fg(GTK_WIDGET(cmd),   GTK_STATE_NORMAL, &(Jumanji.Style.completion_fg));
      gtk_widget_modify_fg(GTK_WIDGET(cdesc), GTK_STATE_NORMAL, &(Jumanji.Style.completion_fg));
      gtk_widget_modify_bg(GTK_WIDGET(row),   GTK_STATE_NORMAL, &(Jumanji.Style.completion_bg));
    }
    else
    {
      gtk_widget_modify_fg(GTK_WIDGET(cmd),   GTK_STATE_NORMAL, &(Jumanji.Style.completion_hl_fg));
      gtk_widget_modify_fg(GTK_WIDGET(cdesc), GTK_STATE_NORMAL, &(Jumanji.Style.completion_hl_fg));
      gtk_widget_modify_bg(GTK_WIDGET(row),   GTK_STATE_NORMAL, &(Jumanji.Style.completion_hl_bg));
    }
  }
}

void
switch_view(GtkWidget* widget)
{
  /*GtkWidget* child = gtk_bin_get_child(GTK_BIN(Jumanji.UI.viewport));*/
  /*if(child)*/
  /*{*/
    /*g_object_ref(child);*/
    /*gtk_container_remove(GTK_CONTAINER(Jumanji.UI.viewport), child);*/
  /*}*/

  /*gtk_container_add(GTK_CONTAINER(Jumanji.UI.viewport), GTK_WIDGET(widget));*/
}

GtkEventBox*
create_completion_row(GtkBox* results, char* command, char* description, gboolean group)
{
  GtkBox      *col = GTK_BOX(gtk_hbox_new(FALSE, 0));
  GtkEventBox *row = GTK_EVENT_BOX(gtk_event_box_new());

  GtkLabel *show_command     = GTK_LABEL(gtk_label_new(NULL));
  GtkLabel *show_description = GTK_LABEL(gtk_label_new(NULL));

  gtk_misc_set_alignment(GTK_MISC(show_command),     0.0, 0.0);
  gtk_misc_set_alignment(GTK_MISC(show_description), 0.0, 0.0);

  if(group)
  {
    gtk_misc_set_padding(GTK_MISC(show_command),     2.0, 4.0);
    gtk_misc_set_padding(GTK_MISC(show_description), 2.0, 4.0);
  }
  else
  {
    gtk_misc_set_padding(GTK_MISC(show_command),     1.0, 1.0);
    gtk_misc_set_padding(GTK_MISC(show_description), 1.0, 1.0);
  }

  gtk_label_set_use_markup(show_command,     TRUE);
  gtk_label_set_use_markup(show_description, TRUE);

  gtk_label_set_markup(show_command,     g_markup_printf_escaped(FORMAT_COMMAND,     command ? command : ""));
  gtk_label_set_markup(show_description, g_markup_printf_escaped(FORMAT_DESCRIPTION, description ? description : ""));

  if(group)
  {
    gtk_widget_modify_fg(GTK_WIDGET(show_command),     GTK_STATE_NORMAL, &(Jumanji.Style.completion_g_fg));
    gtk_widget_modify_fg(GTK_WIDGET(show_description), GTK_STATE_NORMAL, &(Jumanji.Style.completion_g_fg));
    gtk_widget_modify_bg(GTK_WIDGET(row),              GTK_STATE_NORMAL, &(Jumanji.Style.completion_g_bg));
  }
  else
  {
    gtk_widget_modify_fg(GTK_WIDGET(show_command),     GTK_STATE_NORMAL, &(Jumanji.Style.completion_fg));
    gtk_widget_modify_fg(GTK_WIDGET(show_description), GTK_STATE_NORMAL, &(Jumanji.Style.completion_fg));
    gtk_widget_modify_bg(GTK_WIDGET(row),              GTK_STATE_NORMAL, &(Jumanji.Style.completion_bg));
  }

  gtk_widget_modify_font(GTK_WIDGET(show_command),     Jumanji.Style.font);
  gtk_widget_modify_font(GTK_WIDGET(show_description), Jumanji.Style.font);

  gtk_box_pack_start(GTK_BOX(col), GTK_WIDGET(show_command),     TRUE,  TRUE,  2);
  gtk_box_pack_start(GTK_BOX(col), GTK_WIDGET(show_description), FALSE, FALSE, 2);

  gtk_container_add(GTK_CONTAINER(row), GTK_WIDGET(col));

  gtk_box_pack_start(results, GTK_WIDGET(row), FALSE, FALSE, 0);

  return row;
}

Completion*
completion_init()
{
  Completion *completion = malloc(sizeof(Completion));
  if(!completion)
    out_of_memory();

  completion->groups = NULL;

  return completion;
}

CompletionGroup*
completion_group_create(char* name)
{
  CompletionGroup* group = malloc(sizeof(CompletionGroup));
  if(!group)
    out_of_memory();

  group->value    = name;
  group->elements = NULL;
  group->next     = NULL;

  return group;
}

void
completion_add_group(Completion* completion, CompletionGroup* group)
{
  CompletionGroup* cg = completion->groups;

  while(cg && cg->next)
    cg = cg->next;

  if(cg)
    cg->next = group;
  else
    completion->groups = group;
}

void completion_free(Completion* completion)
{
  CompletionGroup* group = completion->groups;
  CompletionElement *element;

  while(group)
  {
    element = group->elements;
    while(element)
    {
      CompletionElement* ne = element->next;
      free(element);
      element = ne;
    }

    CompletionGroup *ng = group->next;
    free(group);
    group = ng;
  }
}

void completion_group_add_element(CompletionGroup* group, char* name, char* description)
{
  CompletionElement* el = group->elements;

  while(el && el->next)
    el = el->next;

  CompletionElement* new_element = malloc(sizeof(CompletionElement));
  if(!new_element)
    out_of_memory();

  new_element->value       = name;
  new_element->description = description;
  new_element->next        = NULL;

  if(el)
    el->next = new_element;
  else
    group->elements = new_element;
}

/* shortcut implementation */
void
sc_abort(Argument* argument)
{
  /* Clear buffer */
  if(Jumanji.Global.buffer)
  {
    g_string_free(Jumanji.Global.buffer, TRUE);
    Jumanji.Global.buffer = NULL;
    gtk_label_set_markup((GtkLabel*) Jumanji.Statusbar.buffer, "");
  }

  /* Set back to normal mode */
  change_mode(NORMAL);

  /* Hide inputbar */
  gtk_widget_hide(GTK_WIDGET(Jumanji.UI.inputbar));
}

void
sc_close_tab(Argument* argument)
{
  int current_tab = gtk_notebook_get_current_page(Jumanji.UI.view);

  if(gtk_notebook_get_n_pages(Jumanji.UI.view) > 1)
    gtk_notebook_remove_page(Jumanji.UI.view, current_tab);
  else
    open_uri(GET_CURRENT_TAB(), home_page);
}

void
sc_focus_inputbar(Argument* argument)
{
  if(!(GTK_WIDGET_VISIBLE(GTK_WIDGET(Jumanji.UI.inputbar))))
    gtk_widget_show(GTK_WIDGET(Jumanji.UI.inputbar));

  if(argument->data)
  {
    char* data = argument->data;

    if(argument->n == APPEND_URL)
      data = g_strdup_printf("%s%s", data, webkit_web_view_get_uri(GET_CURRENT_TAB()));
    else
      data = g_strdup(data);

    notify(DEFAULT, data);
    g_free(data);

    gtk_widget_grab_focus(GTK_WIDGET(Jumanji.UI.inputbar));
    gtk_editable_set_position(GTK_EDITABLE(Jumanji.UI.inputbar), -1);
  }
}

void
sc_nav_history(Argument* argument)
{
  if(argument->n == NEXT)
    webkit_web_view_go_forward(GET_CURRENT_TAB());
  else if(argument->n == PREVIOUS)
    webkit_web_view_go_back(GET_CURRENT_TAB());
}

void
sc_nav_tabs(Argument* argument)
{
  bcmd_nav_tabs(NULL, argument);
}

void
sc_reload(Argument* argument)
{
  webkit_web_view_reload(GET_CURRENT_TAB());
}

void
sc_scroll(Argument* argument)
{
  GtkAdjustment* adjustment;

  if( (argument->n == LEFT) || (argument->n == RIGHT) )
    adjustment = gtk_scrolled_window_get_hadjustment(GET_CURRENT_TAB_WIDGET());
  else
    adjustment = gtk_scrolled_window_get_vadjustment(GET_CURRENT_TAB_WIDGET());

  gdouble view_size  = gtk_adjustment_get_page_size(adjustment);
  gdouble value      = gtk_adjustment_get_value(adjustment);
  gdouble max        = gtk_adjustment_get_upper(adjustment) - view_size;

  if(argument->n == FULL_UP)
    gtk_adjustment_set_value(adjustment, (value - view_size) < 0 ? 0 : (value - view_size));
  else if(argument->n == FULL_DOWN)
    gtk_adjustment_set_value(adjustment, (value + view_size) > max ? max : (value + view_size));
  else if(argument->n == HALF_UP)
    gtk_adjustment_set_value(adjustment, (value - (view_size / 2)) < 0 ? 0 : (value - (view_size / 2)));
  else if(argument->n == HALF_DOWN)
    gtk_adjustment_set_value(adjustment, (value + (view_size / 2)) > max ? max : (value + (view_size / 2)));
  else if((argument->n == LEFT) || (argument->n == UP))
    gtk_adjustment_set_value(adjustment, (value - scroll_step) < 0 ? 0 : (value - scroll_step));
  else if(argument->n == TOP)
    gtk_adjustment_set_value(adjustment, 0);
  else if(argument->n == BOTTOM)
    gtk_adjustment_set_value(adjustment, max);
  else
    gtk_adjustment_set_value(adjustment, (value + scroll_step) > max ? max : (value + scroll_step));

  update_status();
}

void
sc_toggle_statusbar(Argument* argument)
{
  if(GTK_WIDGET_VISIBLE(GTK_WIDGET(Jumanji.UI.statusbar)))
    gtk_widget_hide(GTK_WIDGET(Jumanji.UI.statusbar));
  else
    gtk_widget_show(GTK_WIDGET(Jumanji.UI.statusbar));
}

void
sc_quit(Argument* argument)
{
  cb_destroy(NULL, NULL);
}

void
sc_yank(Argument* argument)
{
  gchar* uri = (gchar*) webkit_web_view_get_uri(GET_CURRENT_TAB());
  gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_PRIMARY), uri, -1);

  gchar* message = g_strdup_printf("Yanked %s", uri);
  notify(DEFAULT, message);
  g_free(message);
}

void
sc_zoom(Argument* argument)
{
  bcmd_zoom(NULL, argument);
}

/* inputbar shortcut declarations */
void
isc_abort(Argument* argument)
{
  Argument arg = { HIDE };
  isc_completion(&arg);

  notify(DEFAULT, "");
  gtk_widget_grab_focus(GTK_WIDGET(GET_CURRENT_TAB_WIDGET()));
  gtk_widget_hide(GTK_WIDGET(Jumanji.UI.inputbar));
}

void
isc_completion(Argument* argument)
{
  gchar *input      = gtk_editable_get_chars(GTK_EDITABLE(Jumanji.UI.inputbar), 1, -1);
  gchar  identifier = gtk_editable_get_chars(GTK_EDITABLE(Jumanji.UI.inputbar), 0,  1)[0];
  int    length     = strlen(input);

  if(!length && !identifier)
    return;

  /* get current information*/
  char* first_space = strstr(input, " ");
  char* current_command;
  char* current_parameter;
  int   current_command_length;
  int   current_parameter_length;

  if(!first_space)
  {
    current_command          = input;
    current_command_length   = length;
    current_parameter        = NULL;
    current_parameter_length = 0;
  }
  else
  {
    int offset               = abs(input - first_space);
    current_command          = g_strndup(input, offset);
    current_command_length   = strlen(current_command);
    current_parameter        = input + offset + 1;
    current_parameter_length = strlen(current_parameter);
  }

  /* if the identifier does not match the command sign and
   * the completion should not be hidden, leave this function */
  if((identifier != ':') && (argument->n != HIDE))
    return;

  /* static elements */
  static GtkBox        *results = NULL;
  static CompletionRow *rows    = NULL;

  static int current_item = 0;
  static int n_items      = 0;

  static char *previous_command   = NULL;
  static char *previous_parameter = NULL;
  static int   previous_id        = 0;
  static int   previous_length    = 0;

  static gboolean command_mode = TRUE;

  /* delete old list iff
   *   the completion should be hidden
   *   the current command differs from the previous one
   *   the current parameter differs from the previous one
   */
  if( (argument->n == HIDE) ||
      (current_parameter && previous_parameter && strcmp(current_parameter, previous_parameter)) ||
      (current_command && previous_command && strcmp(current_command, previous_command)) ||
      (previous_length != length)
    )
  {
    if(results)
      gtk_widget_destroy(GTK_WIDGET(results));

    results = NULL;

    if(rows)
      free(rows);

    rows         = NULL;
    current_item = 0;
    n_items      = 0;
    command_mode = TRUE;

    if(argument->n == HIDE)
      return;
  }

  /* create new list iff
   *  there is no current list
   *  the current command differs from the previous one
   *  the current parameter differs from the previous one
   */
  if( (!results) )
  {
    results = GTK_BOX(gtk_vbox_new(FALSE, 0));

    /* create list based on parameters iff
     *  there is a current parameter given
     *  there is an old list with commands
     *  the current command does not differ from the previous one
     *  the current command has an completion function
     */
    if( (previous_command) && (current_parameter) && !strcmp(current_command, previous_command) )
    {
      if(previous_id < 0 || !commands[previous_id].completion)
        return;

      Completion *result = commands[previous_id].completion(current_parameter);

      if(!result || !result->groups)
        return;

      command_mode               = FALSE;
      CompletionGroup* group     = NULL;
      CompletionElement* element = NULL;

      rows = malloc(sizeof(CompletionRow));
      if(!rows)
        out_of_memory();

      for(group = result->groups; group != NULL; group = group->next)
      {
        int group_elements = 0;

        for(element = group->elements; element != NULL; element = element->next)
        {
          if( (element->value) &&
              (current_parameter_length <= strlen(element->value)) && !strncmp(current_parameter, element->value, current_parameter_length)
            )
          {
            rows = realloc(rows, (n_items + 1) * sizeof(CompletionRow));
            rows[n_items].command     = element->value;
            rows[n_items].description = element->description;
            rows[n_items].command_id  = previous_id;
            rows[n_items].is_group    = FALSE;
            rows[n_items++].row       = GTK_WIDGET(create_completion_row(results, element->value, element->description, FALSE));
            group_elements++;
          }
        }

        if(group->value && group_elements > 0)
        {
          rows = realloc(rows, (n_items + 1) * sizeof(CompletionRow));
          rows[n_items].command     = group->value;
          rows[n_items].description = NULL;
          rows[n_items].command_id  = -1;
          rows[n_items].is_group    = TRUE;
          rows[n_items].row       = GTK_WIDGET(create_completion_row(results, group->value, NULL, TRUE));

          /* Swap group element with first element of the list */
          CompletionRow temp = rows[n_items - group_elements];
          gtk_box_reorder_child(results, rows[n_items].row, n_items - group_elements);
          rows[n_items - group_elements] = rows[n_items];
          rows[n_items] = temp;

          n_items++;
        }
      }

      /* clean up */
      completion_free(result);
    }
    /* create list based on commands */
    else
    {
      int i = 0;
      command_mode = TRUE;

      rows = malloc(LENGTH(commands) * sizeof(CompletionRow));
      if(!rows)
        out_of_memory();

      for(i = 0; i < LENGTH(commands); i++)
      {
        int abbr_length = commands[i].abbr ? strlen(commands[i].abbr) : 0;
        int cmd_length  = commands[i].command ? strlen(commands[i].command) : 0;

        /* add command to list iff
         *  the current command would match the command
         *  the current command would match the abbreviation
         */
        if( ((current_command_length <= cmd_length)  && !strncmp(current_command, commands[i].command, current_command_length)) ||
            ((current_command_length <= abbr_length) && !strncmp(current_command, commands[i].abbr,    current_command_length))
          )
        {
          rows[n_items].command     = commands[i].command;
          rows[n_items].description = commands[i].description;
          rows[n_items].command_id  = i;
          rows[n_items].is_group    = FALSE;
          rows[n_items++].row       = GTK_WIDGET(create_completion_row(results, commands[i].command, commands[i].description, FALSE));
        }
      }

      rows = realloc(rows, n_items * sizeof(CompletionRow));
    }

    gtk_box_pack_start(Jumanji.UI.box, GTK_WIDGET(results), FALSE, FALSE, 0);
    gtk_widget_show_all(GTK_WIDGET(Jumanji.UI.window));

    current_item = (argument->n == NEXT) ? -1 : 0;
  }

  /* update coloring iff
   *  there is a list with items
   */
  if( (results) && (n_items > 0) )
  {
    set_completion_row_color(results, NORMAL, current_item);
    char* temp;
    int i = 0, next_group = 0;

    for(i = 0; i < n_items; i++)
    {
      if(argument->n == NEXT || argument->n == NEXT_GROUP)
        current_item = (current_item + n_items + 1) % n_items;
      else if(argument->n == PREVIOUS || argument->n == PREVIOUS_GROUP)
        current_item = (current_item + n_items - 1) % n_items;

      if(rows[current_item].is_group)
      {
        if(!command_mode && (argument->n == NEXT_GROUP || argument->n == PREVIOUS_GROUP))
          next_group = 1;
        continue;
      }
      else
      {
        if(!command_mode && (next_group == 0) && (argument->n == NEXT_GROUP || argument->n == PREVIOUS_GROUP))
          continue;
        break;
      }
    }

    set_completion_row_color(results, HIGHLIGHT, current_item);

    if(command_mode)
    {
      char* cp = (current_parameter) ? g_strconcat(" ", current_parameter, NULL) : 0;
      temp = g_strconcat(":", rows[current_item].command, cp, NULL);
    }
    else
    {
      temp = g_strconcat(":", previous_command, " ", rows[current_item].command, NULL);
    }

    gtk_entry_set_text(Jumanji.UI.inputbar, temp);
    gtk_editable_set_position(GTK_EDITABLE(Jumanji.UI.inputbar), -1);
    g_free(temp);

    previous_command   = (command_mode) ? rows[current_item].command : current_command;
    previous_parameter = (command_mode) ? current_parameter : rows[current_item].command;
    previous_length    = strlen(previous_command) + ((command_mode) ? (length - current_command_length) : (strlen(previous_parameter) + 1));
    previous_id        = rows[current_item].command_id;
  }
}

void
isc_command_history(Argument* argument)
{
  static int current = 0;
  int        length  = g_list_length(Jumanji.Global.history);

  if(length > 0)
  {
    if(argument->n == NEXT)
      current = (length + current + 1) % length;
    else
      current = (length + current - 1) % length;

    gchar* command = (gchar*) g_list_nth_data(Jumanji.Global.history, current);
    notify(DEFAULT, command);
    gtk_widget_grab_focus(GTK_WIDGET(Jumanji.UI.inputbar));
    gtk_editable_set_position(GTK_EDITABLE(Jumanji.UI.inputbar), -1);
  }
}

void
isc_string_manipulation(Argument* argument)
{
  gchar *input  = gtk_editable_get_chars(GTK_EDITABLE(Jumanji.UI.inputbar), 0, -1);
  int    length = strlen(input);
  int pos       = gtk_editable_get_position(GTK_EDITABLE(Jumanji.UI.inputbar));

  if(argument->n == DELETE_LAST_WORD)
  {
    int i = pos - 1;

    if(!pos)
      return;

    /* remove trailing spaces */
    for(; i >= 0 && input[i] == ' '; i--);

    /* find the beginning of the word */
    while((i == (pos - 1)) || (((i) > 0) && (input[i] != ' ')
          && (input[i] != '/') && (input[i] != '.')
          && (input[i] != '-') && (input[i] != '=')
          && (input[i] != '&') && (input[i] != '#')
          && (input[i] != '?')
          ))
      i--;

    gtk_editable_delete_text(GTK_EDITABLE(Jumanji.UI.inputbar),  i+1, pos);
    gtk_editable_set_position(GTK_EDITABLE(Jumanji.UI.inputbar), i+1);
  }
  else if(argument->n == DELETE_LAST_CHAR)
  {
    if((length - 1) <= 0)
      isc_abort(NULL);

    gtk_editable_delete_text(GTK_EDITABLE(Jumanji.UI.inputbar), pos - 1, pos);
  }
  else if(argument->n == NEXT_CHAR)
    gtk_editable_set_position(GTK_EDITABLE(Jumanji.UI.inputbar), pos+1);
  else if(argument->n == PREVIOUS_CHAR)
    gtk_editable_set_position(GTK_EDITABLE(Jumanji.UI.inputbar), (pos == 0) ? 0 : pos - 1);
}

/* command implementation */
gboolean
cmd_back(int argc, char** argv)
{
  Argument argument;
  argument.n = PREVIOUS;
  sc_nav_history(&argument);

  return TRUE;
}

gboolean
cmd_forward(int argc, char** argv)
{
  Argument argument;
  argument.n = NEXT;
  sc_nav_history(&argument);

  return TRUE;
}

gboolean
cmd_map(int argc, char** argv)
{
  if(argc < 2)
    return TRUE;

  char* ks = argv[0];

  /* search for the right shortcut function */
  int sc_id = -1;

  int sc_c;
  for(sc_c = 0; sc_c < LENGTH(shortcut_names); sc_c++)
  {
    if(!strcmp(argv[1], shortcut_names[sc_c].name))
    {
      sc_id = sc_c;
      break;
    }
  }

  if(sc_id == -1)
  {
    notify(WARNING, "No such shortcut function exists");
    return FALSE;
  }

  /* parse modifier and key */
  int mask = 0;
  int key  = 0;
  int keyl = strlen(ks);
  int mode = NORMAL;

  // single key (e.g.: g)
  if(keyl == 1)
    key = ks[0];

  // modifier and key (e.g.: <S-g>
  // special key or modifier and key/special key (e.g.: <S-g>, <Space>)

  else if(keyl >= 3 && ks[0] == '<' && ks[keyl-1] == '>')
  {
    char* specialkey = NULL;

    /* check for modifier */
    if(keyl >= 5 && ks[2] == '-')
    {
      /* evaluate modifier */
      switch(ks[1])
      {
        case 'S':
          mask = GDK_SHIFT_MASK;
          break;
        case 'C':
          mask = GDK_CONTROL_MASK;
          break;
      }

      /* no valid modifier */
      if(!mask)
      {
        notify(WARNING, "No valid modifier given.");
        return FALSE;
      }

      /* modifier and special key */
      if(keyl > 5)
        specialkey = g_strndup(ks + 3, keyl - 4);
      else
        key = ks[3];
    }
    else
      specialkey = ks;

    /* search special key */
    int g_c;
    for(g_c = 0; specialkey && g_c < LENGTH(gdk_keys); g_c++)
    {
      if(!strcmp(specialkey, gdk_keys[g_c].identifier))
      {
        key = gdk_keys[g_c].key;
        break;
      }
    }

    if(specialkey)
      g_free(specialkey);
  }

  if(!key)
  {
    notify(WARNING, "No valid key binding given.");
    return FALSE;
  }

  /* parse argument */
  Argument arg = {0, 0};

  if(argc >= 3)
  {
    int arg_id = -1;

    /* compare argument with given argument names... */
    int arg_c;
    for(arg_c = 0; arg_c < LENGTH(argument_names); arg_c++)
    {
      if(!strcmp(argv[2], argument_names[arg_c].name))
      {
        arg_id = argument_names[arg_c].argument;
        break;
      }
    }

    /* if not, save it do .data */
    if(arg_id == -1)
      arg.data = argv[2];
    else
      arg.n = arg_id;
  }

  /* parse mode */
  if(argc >= 4)
  {
    int mode_c;
    for(mode_c = 0; mode_c < LENGTH(mode_names); mode_c++)
    {
      if(!strcmp(argv[3], mode_names[mode_c].name))
      {
        mode = mode_names[mode_c].mode;
        break;
      }
    }
  }

  /* search for existing binding to overwrite it */
  ShortcutList* sc = Jumanji.Bindings.sclist;
  while(sc && sc->next != NULL)
  {
    if(sc->element.key == key && sc->element.mask == mask
        && sc->element.mode == mode)
    {
      sc->element.function = shortcut_names[sc_id].function;
      sc->element.argument = arg;
      return TRUE;
    }

    sc = sc->next;
  }

  /* create new entry */
  ShortcutList* entry = malloc(sizeof(ShortcutList));
  if(!entry)
    out_of_memory();

  entry->element.mask     = mask;
  entry->element.key      = key;
  entry->element.function = shortcut_names[sc_id].function;
  entry->element.mode     = mode;
  entry->element.argument = arg;
  entry->next             = NULL;

  /* append to list */
  if(!Jumanji.Bindings.sclist)
    Jumanji.Bindings.sclist = entry;

  if(sc)
    sc->next = entry;

  return TRUE;
}

gboolean
cmd_open(int argc, char** argv)
{
  if(argc <= 0)
    return TRUE;

  int i;
  GString *uri = g_string_new("");

  for(i = 0; i < argc; i++)
  {
    if(i != 0)
      uri = g_string_append_c(uri, ' ');

    uri = g_string_append(uri, argv[i]);
  }

  open_uri(GET_CURRENT_TAB(), uri->str);
  g_string_free(uri, FALSE);

  return TRUE;
}

gboolean
cmd_quit(int argc, char** argv)
{
  sc_close_tab(NULL);
  return TRUE;
}

gboolean
cmd_quitall(int argc, char** argv)
{
  cb_destroy(NULL, NULL);
  return TRUE;
}

gboolean
cmd_search_engine(int argc, char** argv)
{
  if(argc < 2)
    return TRUE;

  char* name = argv[0];
  char* uri  = argv[1];

  /* search for existing search engine to overwrite it */
  SearchEngineList* se = Jumanji.Global.search_engines;
  while(se && se->next != NULL)
  {
    if(!strcmp(se->name, name))
    {
      se->uri = uri;
      return TRUE;
    }

    se = se->next;
  }

  /* create new engine */
  SearchEngineList* entry = malloc(sizeof(SearchEngineList));
  if(!entry)
    out_of_memory();

  entry->name = name;
  entry->uri  = uri;
  entry->next = NULL;

  /* append to list */
  if(!Jumanji.Global.search_engines)
    Jumanji.Global.search_engines = entry;

  if(se)
    se->next = entry;

  return TRUE;
}

gboolean
cmd_set(int argc, char** argv)
{
  if(argc <= 0)
    return TRUE;

  int i;
  for(i = 0; i < LENGTH(settings); i++)
  {
    if(!strcmp(argv[0], settings[i].name))
    {
      /* check var type */
      if(settings[i].type == 'b')
      {
        gboolean *x = (gboolean*) (settings[i].variable);
        *x = !(*x);

        if(argv[1])
        {
          if(!strcmp(argv[1], "false") || !strcmp(argv[1], "0"))
            *x = FALSE;
          else
            *x = TRUE;
        }
      }
      else if(settings[i].type == 'i')
      {
        if(argc != 2)
          return TRUE;

        int *x = (int*) (settings[i].variable);

        int id = -1;
        int arg_c;
        for(arg_c = 0; arg_c < LENGTH(argument_names); arg_c++)
        {
          if(!strcmp(argv[1], argument_names[arg_c].name))
          {
            id = argument_names[arg_c].argument;
            break;
          }
        }

        if(id == -1)
          id = atoi(argv[1]);

        *x = id;
      }
      else if(settings[i].type == 'f')
      {
        if(argc != 2)
          return TRUE;

        float *x = (float*) (settings[i].variable);
        if(argv[1])
          *x = atof(argv[1]);
      }
      else if(settings[i].type == 's')
      {
        if(argc < 2)
          return TRUE;

        /* assembly the arguments back to one string */
        int j;
        GString *s = g_string_new("");
        for(j = 1; j < argc; j++)
        {
          if(j != 1)
            s = g_string_append_c(s, ' ');

          s = g_string_append(s, argv[j]);
        }

        char **x = (char**) settings[i].variable;
        *x = s->str;
      }
      else if(settings[i].type == 'c')
      {
        if(argc != 2)
          return TRUE;

        char *x = (char*) (settings[i].variable);
        if(argv[1])
          *x = argv[1][0];
      }

      /* re-init */
      if(settings[i].reinit)
        init_look();
    }
  }

  update_status();
  return TRUE;
}

gboolean
cmd_tabopen(int argc, char** argv)
{
  if(argc <= 0)
    return TRUE;

  int i;
  GString *uri = g_string_new("");

  for(i = 0; i < argc; i++)
  {
    if(i != 0)
      uri = g_string_append_c(uri, ' ');

    uri = g_string_append(uri, argv[i]);
  }

  create_tab(uri->str, -1);
  g_string_free(uri, FALSE);

  return TRUE;
}

gboolean
cmd_winopen(int argc, char** argv)
{
  if(argc <= 0)
    return TRUE;

  int i;
  GString *uri = g_string_new("");

  for(i = 0; i < argc; i++)
  {
    if(i != 0)
      uri = g_string_append_c(uri, ' ');

    uri = g_string_append(uri, argv[i]);
  }

  char* nargv[6];
  if(Jumanji.UI.embed)
  {
    nargv[0] = *(Jumanji.Global.arguments);
    nargv[1] = "-e";
    nargv[2] = Jumanji.UI.winid;
    nargv[3] = uri->str;
    nargv[4] = NULL;
  }
  else
  {
    nargv[0] = *(Jumanji.Global.arguments);
    nargv[1] = uri->str;
    nargv[2] = NULL;
  }

  g_spawn_async(NULL, nargv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL);

  g_string_free(uri, FALSE);

  return TRUE;
}

/* completion command implementation */
Completion*
cc_open(char* input)
{
  Completion* completion = completion_init();

  /* search engines */
  CompletionGroup* search_engines = completion_group_create("Search engines");
  SearchEngineList* se = Jumanji.Global.search_engines;

  if(se)
    completion_add_group(completion, search_engines);

  while(se)
  {
    completion_group_add_element(search_engines, se->name, NULL);
    se = se->next;
  }

  return completion;
}

Completion*
cc_set(char* input)
{
  Completion* completion = completion_init();
  CompletionGroup* group = completion_group_create(NULL);

  completion_add_group(completion, group);

  int i = 0;
  int input_length = input ? strlen(input) : 0;

  for(i = 0; i < LENGTH(settings); i++)
  {
    if( (input_length <= strlen(settings[i].name)) &&
          !strncmp(input, settings[i].name, input_length) )
    {
      completion_group_add_element(group, settings[i].name, settings[i].description);
    }
  }

  return completion;
}

/* buffer command implementation */
void
bcmd_goto(char* buffer, Argument* argument)
{
  sc_scroll(argument);
}

void
bcmd_nav_tabs(char* buffer, Argument* argument)
{
  int current_tab     = gtk_notebook_get_current_page(Jumanji.UI.view);
  int number_of_tabs  = gtk_notebook_get_n_pages(Jumanji.UI.view);
  int step            = 1;

  if(argument->n == PREVIOUS)
    step = -1;
  else if(argument->n == SPECIFIC)
  {
    char* number = g_strndup(buffer, strlen(buffer) - 2);
    step         = atoi(number) * ((buffer[strlen(buffer)-1] == 't') ? 1 : -1);
  }

  gtk_notebook_set_current_page(Jumanji.UI.view, (current_tab + step) % number_of_tabs);
  update_status();
}

void
bcmd_scroll(char* buffer, Argument* argument)
{
  GtkAdjustment* adjustment = gtk_scrolled_window_get_vadjustment(GET_CURRENT_TAB_WIDGET());

  gdouble view_size = gtk_adjustment_get_page_size(adjustment);
  gdouble max       = gtk_adjustment_get_upper(adjustment) - view_size;

  int number     = atoi(g_strndup(buffer, strlen(buffer) - 1));
  int percentage = (number < 0) ? 0 : (number > 100) ? 100 : number;
  gdouble value  = (max / 100.0f) * (float) percentage;

  gtk_adjustment_set_value(adjustment, value);
}

void
bcmd_zoom(char* buffer, Argument* argument)
{
  float zoom_level = webkit_web_view_get_zoom_level(GET_CURRENT_TAB());

  if(argument->n == ZOOM_IN)
    webkit_web_view_set_zoom_level(GET_CURRENT_TAB(), zoom_level + (float) (zoom_step / 100));
  else if(argument->n == ZOOM_OUT)
    webkit_web_view_set_zoom_level(GET_CURRENT_TAB(), zoom_level - (float) (zoom_step / 100));
  else if(argument->n == ZOOM_ORIGINAL)
    webkit_web_view_set_zoom_level(GET_CURRENT_TAB(), 100.0f);
  else if(argument->n == SPECIFIC)
  {
    char* number = g_strndup(buffer, strlen(buffer) - 1);
    webkit_web_view_set_zoom_level(GET_CURRENT_TAB(), (float) (atoi(number) / 100));
    g_free(number);
  }
}

/* special command implementation */
gboolean
scmd_search(char* input, Argument* argument)
{
  return TRUE;
}

/* callback implementation */
gboolean
cb_destroy(GtkWidget* widget, gpointer data)
{
  pango_font_description_free(Jumanji.Style.font);

  /* clean shortcut list */
  ShortcutList* sc = Jumanji.Bindings.sclist;

  while(sc)
  {
    ShortcutList* ne = sc->next;
    free(sc);
    sc = ne;
  }

  gtk_main_quit();

  return TRUE;
}

gboolean
cb_inputbar_kb_pressed(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  int i;

  /* inputbar shortcuts */
  for(i = 0; i < LENGTH(inputbar_shortcuts); i++)
  {
    if(event->keyval == inputbar_shortcuts[i].key &&
      (((event->state & inputbar_shortcuts[i].mask) == inputbar_shortcuts[i].mask)
       || inputbar_shortcuts[i].mask == 0))
    {
      inputbar_shortcuts[i].function(&(inputbar_shortcuts[i].argument));
      return TRUE;
    }
  }

  /* special commands */
  char identifier = gtk_editable_get_chars(GTK_EDITABLE(Jumanji.UI.inputbar), 0, 1)[0];
  for(i = 0; i < LENGTH(special_commands); i++)
  {
    if((identifier == special_commands[i].identifier) &&
       (special_commands[i].always == 1))
    {
      gchar *input  = gtk_editable_get_chars(GTK_EDITABLE(Jumanji.UI.inputbar), 1, -1);
      special_commands[i].function(input, &(special_commands[i].argument));
      return FALSE;
    }
  }

  return FALSE;
}

gboolean
cb_inputbar_activate(GtkEntry* entry, gpointer data)
{
  gchar  *input  = gtk_editable_get_chars(GTK_EDITABLE(entry), 1, -1);
  gchar **tokens = g_strsplit(input, " ", -1);
  gchar *command = tokens[0];
  int     length = g_strv_length(tokens);
  int          i = 0;
  gboolean  retv = FALSE;
  gboolean  succ = FALSE;

  /* no input */
  if(length < 1)
  {
    isc_abort(NULL);
    return FALSE;
  }

  /* append input to the command history */
  Jumanji.Global.history = g_list_append(Jumanji.Global.history, g_strdup(gtk_entry_get_text(entry)));

  /* special commands */
  char identifier = gtk_editable_get_chars(GTK_EDITABLE(entry), 0, 1)[0];
  for(i = 0; i < LENGTH(special_commands); i++)
  {
    if(identifier == special_commands[i].identifier)
    {
      /* special commands that are evaluated every key change are not
       * called here */
      if(special_commands[i].always == 1)
      {
        isc_abort(NULL);
        return TRUE;
      }

      retv = special_commands[i].function(input, &(special_commands[i].argument));
      if(retv) isc_abort(NULL);
      gtk_widget_grab_focus(GTK_WIDGET(GET_CURRENT_TAB_WIDGET()));
      return TRUE;
    }
  }

  /* search commands */
  for(i = 0; i < LENGTH(commands); i++)
  {
    if((g_strcmp0(command, commands[i].command) == 0) ||
       (g_strcmp0(command, commands[i].abbr)    == 0))
    {
      retv = commands[i].function(length - 1, tokens + 1);
      succ = TRUE;
      break;
    }
  }

  if(retv)
    isc_abort(NULL);

  if(!succ)
    notify(ERROR, "Unknown command.");

  Argument arg = { HIDE };
  isc_completion(&arg);

  gtk_widget_grab_focus(GTK_WIDGET(GET_CURRENT_TAB_WIDGET()));

  return TRUE;
}

gboolean
cb_tab_kb_pressed(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  ShortcutList* sc = Jumanji.Bindings.sclist;
  while(sc)
  {
    if(
       event->keyval == sc->element.key
       && (CLEAN(event->state) == sc->element.mask || (sc->element.key >= 0x21
       && sc->element.key <= 0x7E && CLEAN(event->state) == GDK_SHIFT_MASK))
       && (Jumanji.Global.mode & sc->element.mode || sc->element.mode == ALL)
       && sc->element.function
      )
    {
      if(!(Jumanji.Global.buffer && strlen(Jumanji.Global.buffer->str)) || (sc->element.mask == GDK_CONTROL_MASK) ||
         (sc->element.key <= 0x21 || sc->element.key >= 0x7E)
        )
      {
        sc->element.function(&(sc->element.argument));
        return TRUE;
      }
    }

    sc = sc->next;
  }

  if(Jumanji.Global.mode == ADD_MARKER)
  {
    add_marker(event->keyval);
    change_mode(NORMAL);
    return TRUE;
  }
  else if(Jumanji.Global.mode == EVAL_MARKER)
  {
    eval_marker(event->keyval);
    change_mode(NORMAL);
    return TRUE;
  }

  /* append only numbers and characters to buffer */
  if( (event->keyval >= 0x21) && (event->keyval <= 0x7E))
  {
    if(!Jumanji.Global.buffer)
      Jumanji.Global.buffer = g_string_new("");

    Jumanji.Global.buffer = g_string_append_c(Jumanji.Global.buffer, event->keyval);
    gtk_label_set_markup((GtkLabel*) Jumanji.Statusbar.buffer, Jumanji.Global.buffer->str);
  }

  /* search buffer commands */
  if(Jumanji.Global.buffer)
  {
    int i;
    for(i = 0; i < LENGTH(buffer_commands); i++)
    {
      regex_t regex;
      int     status;

      regcomp(&regex, buffer_commands[i].regex, REG_EXTENDED);
      status = regexec(&regex, Jumanji.Global.buffer->str, (size_t) 0, NULL, 0);
      regfree(&regex);

      if(status == 0)
      {
        buffer_commands[i].function(Jumanji.Global.buffer->str, &(buffer_commands[i].argument));
        g_string_free(Jumanji.Global.buffer, TRUE);
        Jumanji.Global.buffer = NULL;
        gtk_label_set_markup((GtkLabel*) Jumanji.Statusbar.buffer, "");

        return TRUE;
      }
    }
  }

  return FALSE;
}

gboolean
cb_wv_load_finished(WebKitWebView* wv, WebKitWebFrame* frame, gpointer data)
{
  if(wv == GET_CURRENT_TAB())
    update_status();

  return TRUE;
}

gboolean
cb_wv_load_progress_changed(WebKitWebView* wv, int progress, gpointer data)
{
  if(wv == GET_CURRENT_TAB())
  {
    g_object_set_data(G_OBJECT(wv), "progress", (gpointer) progress);
    update_status();
  }

  return TRUE;
}

gboolean
cb_wv_nav_policy_decision(WebKitWebView* wv, WebKitWebFrame* frame, WebKitNetworkRequest* request,
    WebKitWebNavigationAction* action, WebKitWebPolicyDecision* decision, gpointer data)
{
  switch(webkit_web_navigation_action_get_button(action))
  {
    case 1: /* left mouse button */
      open_uri(GET_CURRENT_TAB(), (char*) webkit_network_request_get_uri(request));
      webkit_web_policy_decision_ignore(decision);
      return TRUE;
    case 2: /* middle mouse button */
      create_tab((char*) webkit_network_request_get_uri(request), -1);
      webkit_web_policy_decision_ignore(decision);
      return TRUE;
    case 3: /* right mouse button */
      return FALSE;
    default:
      return FALSE;;
  }
}

/* main function */
int main(int argc, char* argv[])
{
  g_thread_init(NULL);

  gtk_init(&argc, &argv);

  /* parse arguments & embed */
  Jumanji.UI.embed = 0;
  Jumanji.UI.winid = 0;
  Jumanji.Global.arguments = argv;

  int i;
  for(i = 1; i < argc && argv[i][0] == '-' && argv[i][1] != '\0'; i++)
  {
    switch(argv[i][1])
    {
      case 'e':
        if(++i < argc)
        {
          Jumanji.UI.embed = atoi(argv[i]);
          Jumanji.UI.winid = argv[i];
        }
        break;
    }
  }

  /* init jumanji and read configuration */
  init_jumanji();
  init_directories();
  init_keylist();
  read_configuration();
  init_settings();
  init_look();

  /* create tab */
  if(argc < 2)
    create_tab(home_page, -1);
  else
    create_tab(argv[i], -1);

  gtk_widget_show_all(GTK_WIDGET(Jumanji.UI.window));
  gtk_widget_grab_focus(GTK_WIDGET(GET_CURRENT_TAB_WIDGET()));
  gtk_widget_hide(GTK_WIDGET(Jumanji.UI.inputbar));

  if(!show_statusbar)
    gtk_widget_hide(GTK_WIDGET(Jumanji.UI.statusbar));

  gtk_main();

  return 0;
}
