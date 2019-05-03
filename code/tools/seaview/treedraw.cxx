//njplot coords = pdf coords: bottom to top
//QuickDraw coords: top to bottom
#include <math.h>
#include <time.h>
#include <ctype.h>

#include <FL/Enumerations.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/Fl_Simple_Counter.H>
#include <FL/Fl_Printer.H>
#include "unrooted.h"
#include "treedraw.h"
#include "pdf_or_ps.h"
#include "svg.h"
#include <FL/Fl_PostScript.H>

#define MAX_FRAC 0.95
#ifndef FALSE
#define FALSE 0
#define TRUE (!FALSE)
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* typedefs */

typedef enum { helvetica, courier, times} font_name;

enum {save_to_trees_menu, remove_from_trees_menu, save_rooted, save_unrooted, save_all_trees,
  save_patristic, print_tree, save_as_ps_pdf, A4_format, letter_format, landscape, page_count};
enum {save_as_svg = page_count + 14, reorder_align_following_tree, select_seqs_in_alignment, open_tree_or_align,
  new_window, close_window};


typedef struct branche { /* une branche definie par ses deux extremites */
	struct noeud *bouta;
	struct noeud *boutb;
	char *br_label;
	} branche;

#define s_noeud sizeof(struct noeud)


/* prototypes of included functions */
#ifdef MICRO
static void copy(Fl_Widget *wgt, void *data);
#endif
static void paste(Fl_Widget *wgt, void *data);
void toggle_br_lengths(Fl_Widget *wgt, void *data);
void toggle_bootstrap(Fl_Widget *wgt, void *data);
void change_ffam(Fl_Widget *wgt, void *data);
char *prepare_ps_or_pdf_font(int font_num);
void subtreeup_callback(Fl_Widget *wgt, void *data);
void operation_callback(Fl_Widget *wgt, void *data);
void do_plot(FD_nj_plot *fd_nj_plot, int doing_print);
int calc_text_size(char *text, int *pheight, int *pascent);
void plotstring(const char *nom);
void dir_moveto(double x,double y);
void dir_lineto(double x,double y);
void dir_plotsquare(double x, double y, int edge, int use_color);
void scale_window(double lxmin, double lxmax, double lymin, double lymax,
 	double pxmin, double pxmax, double pymin, double pymax);
void ch_echelle(double lx, double ly, double *px, double *py);
void init_tree(char *fname, char *displayname);
const char *preptree(FD_nj_plot *fd_nj_plot);
char *check_alloc(int nbrelt, int sizelt);
const char *loadphylip(FD_nj_plot *fd_nj_plot, char *arbre, char *last_bootstrap);
struct noeud *unrootedset(FD_nj_plot *fd_nj_plot, char *deb, char *fin, branche **p_int_br);
char *nextpar(char *pospar);
const char *make_binary_or_unrooted_extra(char *arbre, bool *p_has_3_subtrees);
const char *make_binary_or_unrooted(char *arbre) {return make_binary_or_unrooted_extra(arbre, NULL); }
int make_binary(char *arbre, char *debut, char *fin, int go_down);
void mydrawstring(double x, double y, char *nom, char option, int height);
void moveto(double x,double y);
void lineto(double x,double y);
int calc_brl_for_lengthless(struct noeud *centre, struct noeud *pere);
void add_value_downstream(struct noeud *centre, int value);
double place_midpoint_root(struct noeud *from, struct noeud *racine, int notu, int fixed_outgroup);
double get_length_down(struct noeud *, struct noeud *);
void runtree(FD_nj_plot *fd_nj_plot);
void mem_plot(FD_nj_plot *fd_nj_plot, struct noeud *pere, struct noeud *centre, double currx,
	double *curry);
void mem_point(FD_nj_plot *fd_nj_plot, double x, double y, int number);
void mem_nom(FD_nj_plot *fd_nj_plot, double x, double y, char *nom, char option);
void mem_trait(FD_nj_plot *fd_nj_plot, double xd, double yd, double xf, double yf, int width);
char *get_br_label(FD_nj_plot *fd_nj_plot, struct noeud *a, struct noeud *b);
char *get_br_label_with_threshold(FD_nj_plot *fd_nj_plot, struct noeud *a, struct noeud *b);
int get_br_from_bouts(FD_nj_plot *fd_nj_plot, struct noeud *a, struct noeud *b);
void free_tree(FD_nj_plot *fd_nj_plot);
char *ecrit_arbre_parenth_unrooted(FD_nj_plot *fd_nj_plot, struct noeud *root);
char *ecrit_arbre_parenth(FD_nj_plot *fd_nj_plot, struct noeud *root);
char *recur_ecrit_arbre(FD_nj_plot *fd_nj_plot, struct noeud *centre, char *arbre, char *finarbre);
void removeroot(FD_nj_plot *fd_nj_plot);
double length_log_phys(double p);
double length_phys_log(double p);
double arrondi_echelle(double x);
double calc_echelle(double larg);
void draw_scale(FD_nj_plot *fd_nj_plot);
int calc_n_desc(struct noeud *pere);
void majuscules(char *p);
void treeplotw_callback(Fl_Widget *wgt, void *data);
void scrollnotu(FD_nj_plot *fd_nj_plot);
void zoom_callback(Fl_Widget *wgt, void *data);
void scroller_callback(Fl_Widget *wgt, void *data);
static void patristic_callback(Fl_Widget *wgt, void *unused);
static void file_callback(Fl_Widget *wgt, void *data);
void remove_from_tree_menu(FD_nj_plot *fd_nj_plot, const char *name);
void add_to_tree_menu(FD_nj_plot *fd_nj_plot);
void physic_to_logic(double px, double py, double *lx, double *ly);
void tree_click_proc(tree_panel *panel, int mx, int my);
void disconnect_tree_windows(SEA_VIEW *view);
void reorder_following_tree(struct noeud *root, int notu, SEA_VIEW *view);
static void search_callback(Fl_Widget *wgt, void *data);
void bt_threshold_callback(Fl_Widget *wgt, void *data);
void midpoint_callback(Fl_Widget *wgt, void *data);
void edit_tree_header(Fl_Widget *wgt, void *data);
void set_win_size_callback(Fl_Widget *wgt, void *data);
int compare_newick_with_names(const char *tree, char **names, int notu, char **pname);
void edit_shape_callback(Fl_Widget *obj, void *data);
void edit_trim_callback(Fl_Widget *obj, void *data);
void select_clade_callback(Fl_Widget *obj, void *data);
void delete_clade_callback(Fl_Widget *obj, void *data);
void complete_edit_callback(Fl_Widget *obj, void *data);
void select_in_alignment(FD_nj_plot *fd_nj_plot);
bool is_parent(struct noeud *centre, struct noeud *from, const char *label);
struct noeud * span_nodes(struct noeud *centre, struct noeud *from);
double recur_patristic_distances(FD_nj_plot *fd_nj_plot, struct noeud *centre, float **d, int **listdesc);
void calc_patristic_distances(FD_nj_plot *fd_nj_plot, FILE *out);
void print_plot(FD_nj_plot *fd_nj_plot, bool to_ps_file, const char *directname = NULL);
static double midpoint_rooting(struct noeud *from, int notu, double *p_root_br_length);
char *extract_one_tree(char *from, char **pnext);
void replace_with_new_tree(FD_nj_plot *fd_nj_plot, char *newtree);
void tree_counter_callback(Fl_Widget *, void *data);
int calc_tree_count(char *tree);
void remove_parenth_no_comma(char *tree);

//extern variables & functions
extern Fl_Window *use_initial_file(SEA_VIEW *view, char *masename, int doing_dnd);
extern void direct_help_callback(Fl_Widget *wgt, void *data);
extern void rooted_unrooted_callback(Fl_Widget *o, void *data);
extern void free_unrooted(FD_unrooted *data);
extern void unrooted_search(FD_unrooted *fd_unrooted, const char *select);
extern void svg_tree_save(FD_nj_plot *fd_nj_plot, Fl_SVG_File_Surface *svg);
extern int debut_arbre(const char *tree, FD_unrooted *fd_unrooted);
extern int testbit(int *plist, int num);
extern void bit1(int *plist, int num);
extern char *get_res_value(const char *name, const char *def_value);
extern int set_res_value(const char *name, const char *value);
#if defined(__APPLE__) && 100*FL_MAJOR_VERSION + FL_MINOR_VERSION < 104
  extern int add_windowmenuitem(const char *name, Fl_Window *w);
  extern void rename_windowmenuitem(const char *name, int rank);
  extern void delete_windowmenuitem(int rank);
  extern int find_windowmenuitem(Fl_Window *w);
#endif


/* globals */
static int page_x_offset = 0;// +/- quantity to add to all horizontal drawing coordinates
static int page_y_offset = 0;// +/- quantity to add to all vertical drawing coordinates
char *end_br_length;
double	physx,physy,physx_min,physy_min,physx_corr,physy_corr;
/* window scaling variables */
double tek_xmin,tek_ymin,tek_dx,tek_dy;
double maxx, maxy, nexty;
int num_noeud, nextotu;
char list_ps_font_name[3][15] = { "Helvetica", "Courier", "Times"};
extern Fl_Paged_Device::Page_Format printout_pageformat;
extern Fl_Paged_Device::Page_Layout printout_layout;
// will contain the two end nodes of the root-containing branch
static struct noeud *current_cote1=NULL, *current_cote2=NULL;
static const char bad_tree[] = "Incorrect tree data.";
static char *outotu=0, *inotu=0;
static char *target1=0, *target2=0, *target3=0; // for -outgroup option


LeafNameTrimmer::LeafNameTrimmer(int x, int y, int w, int h) : Fl_Widget(x, y, w, h, "") {
  copy_label(get_res_value("name_trimming_rule", ""));
}

void LeafNameTrimmer::draw() {
  fl_color(FL_WHITE);
  fl_rectf(x(), y(), w(), h());
  fl_color(FL_BLACK);
  fl_font(labelfont(), labelsize());
  fl_draw(label(), x(), y(), w(), h(), FL_ALIGN_CENTER, NULL, 0);
  fl_font(labelfont(), 11);
  fl_draw("Trim rule:", x()-50, y(), 50, h(), FL_ALIGN_LEFT, NULL, 0);
 }


int LeafNameTrimmer::handle(int event) {
  if (event == FL_PUSH) {
    const char *limit = fl_input("Enter sequence name trimming rule.  Examples:\n"
                                 " ~~  will cut names at their second ~\n"
                                 " 25 will cut names longer than 25 characters\n"
                                 "<empty> for full names", label());
    if (limit) {
      copy_label(limit);
      redraw();
      FD_nj_plot *fd_nj_plot = (FD_nj_plot*)user_data();
      if (!fd_nj_plot->fd_unrooted) {
        fd_nj_plot->need_runtree = TRUE;
        fd_nj_plot->panel->parent()->redraw();
      } else {
        process_unrooted();
        FD_unrooted *fd_unrooted = (FD_unrooted*)fd_nj_plot->fd_unrooted;
        fd_unrooted->comp_phys_bounds = TRUE;
        fd_unrooted->previous_w = 0;
        fd_unrooted->previous_h = 0;
        fd_unrooted->unrooted_plot->parent()->redraw();
      }
    }
    return 1;
  }
  return 0;
}

#define MAX_TRIM_LENGTH 500

static char* trim_after_chars(const char *in, const char *target) {
  static char output[MAX_TRIM_LENGTH+1];
  const char *t = target;
  const char *p = in, *q = NULL;
  while (*t) {
    q = strchr(p, *t);
    t++;
    if (q) p = q + 1;
  }
  strncpy(output, in, MAX_TRIM_LENGTH);
  if (q && q-in <= MAX_TRIM_LENGTH) output[q-in] = 0;
  return output;
}

static char* trim_at_pos(const char *in, const int pos) {
  static char output[MAX_TRIM_LENGTH+1];
  memcpy(output, in, pos);
  output[pos] = 0;
  return output;
}

int LeafNameTrimmer::by_position() {
  int pos = -1;
  sscanf(label(), "%d", &pos);
  if (pos > MAX_TRIM_LENGTH) pos = MAX_TRIM_LENGTH;
  return pos > 0 ? pos : 0;
}

void LeafNameTrimmer::process() {
  FD_nj_plot *fd_nj_plot = (FD_nj_plot *)user_data();
  int pos = by_position();
  for (int i = 0; i < fd_nj_plot->notu; i++) {
    char *newname = pos ?
        trim_at_pos(fd_nj_plot->labels[i], pos) : trim_after_chars(fd_nj_plot->labels[i], label());
    strcat(fd_nj_plot->tabtax[i]->nom, newname);
  }
}

void LeafNameTrimmer::process_to_labels() {
  FD_nj_plot *fd_nj_plot = (FD_nj_plot *)user_data();
  int pos = by_position();
  for (int i = 0; i < fd_nj_plot->notu; i++) {
    char *newname = pos ?
      trim_at_pos(fd_nj_plot->labels[i], pos) : trim_after_chars(fd_nj_plot->labels[i], label());
    strcpy(fd_nj_plot->labels[i], newname);
  }
}

void LeafNameTrimmer::process_unrooted() {
  FD_nj_plot *fd_nj_plot = (FD_nj_plot *)user_data();
  FD_unrooted *fd_unrooted = (FD_unrooted*)fd_nj_plot->fd_unrooted;
  int pos = by_position();
  for (int i = 0; i < 2 * fd_nj_plot->notu - 3; i++) {
    char *name = (fd_unrooted->branches+i)->nom;
    if (!name) continue;
    int rank = (fd_unrooted->branches+i)->rank;
    const char *oldname = fd_unrooted->tabtax[rank]->nom;
    char *p = pos ? trim_at_pos(oldname, pos) : trim_after_chars(oldname, label());
    free(name);
    (fd_unrooted->branches+i)->nom = strdup(p);
  }
}

Fl_Window *treedraw(char *trees, SEA_VIEW *view, const char *name, int from_tree_menu, int tree_count)
/* returns the created window or NULL if none created
 tree_count: optional # of input trees in string trees
 */
{
  if (trees && tree_count == 1) tree_count = calc_tree_count(trees);
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)calloc(1, sizeof(FD_nj_plot));
  fd_nj_plot->trees = trees;
  fd_nj_plot->current_tree = trees;
  fd_nj_plot->rank = 0;
	fd_nj_plot->view = view;
#ifdef unix
	putenv((char *)"LC_NUMERIC=C");
#endif
	fd_nj_plot->swap = 0;
	fd_nj_plot->tree_name = NULL;
	fd_nj_plot->show_bootstrap = 0;
	fd_nj_plot->plot_br_l = 0;
	fd_nj_plot->choix = show_tree;
	fd_nj_plot->need_runtree = FALSE;
	
	Fl_Window *w = new Fl_Window(590, 530);
	w->xclass(TREE_WINDOW);
	Fl_Group *g = new Fl_Group(0, 0, w->w()-2, 50);
  
  Fl_Menu_Bar* menubar = new Fl_Menu_Bar(3, 3, 130, 20);
  menubar->user_data(fd_nj_plot);
  int left = menubar->x() + menubar->w() + 1;
  
  static const Fl_Menu_Item fileitems[] = {
    {"Save to Trees menu", 0, file_callback, NULL, 0},
    {"Remove from Trees menu", 0, file_callback, NULL, 0},
    {"Save rooted tree", 0, file_callback, NULL, 0},
    {"Save unrooted tree", 0, file_callback, NULL, 0},
    {"Save all trees", 0, file_callback, NULL, 0},
    {"Save patristic distances", 0, patristic_callback, NULL, FL_MENU_DIVIDER},
    {"Print", FL_COMMAND | 'p', file_callback, NULL, 0},
    {"Save as "PDF_OR_PS"", 0, file_callback, NULL, 0},
    {"A4", 0, file_callback, NULL, FL_MENU_RADIO | 0},
    {"Letter", 0, file_callback, NULL, FL_MENU_RADIO | 0},
    {"Landscape", 0, file_callback, NULL, FL_MENU_TOGGLE},
    {"Page count", 0, NULL, NULL, FL_SUBMENU | FL_MENU_DIVIDER},
    {"1", 0, file_callback, NULL, FL_MENU_RADIO | FL_MENU_VALUE},
    {"2", 0, file_callback, NULL, FL_MENU_RADIO},
    {"3", 0, file_callback, NULL, FL_MENU_RADIO},
    {"4", 0, file_callback, NULL, FL_MENU_RADIO},
    {"5", 0, file_callback, NULL, FL_MENU_RADIO},
    {"6", 0, file_callback, NULL, FL_MENU_RADIO},
    {"7", 0, file_callback, NULL, FL_MENU_RADIO},
    {"8", 0, file_callback, NULL, FL_MENU_RADIO},
    {"9", 0, file_callback, NULL, FL_MENU_RADIO},
    {"10", 0, file_callback, NULL, FL_MENU_RADIO},
    {"11", 0, file_callback, NULL, FL_MENU_RADIO},
    {"other value", 0, file_callback, NULL, 0},
    {0},
    {"Save as SVG", 0, file_callback, NULL, FL_MENU_DIVIDER},
    {"Reorder following tree", 0, file_callback, NULL, 0},
    {"Select in alignment", 0, file_callback, NULL, FL_MENU_INACTIVE | FL_MENU_DIVIDER},
    {"Open tree or alignment", FL_COMMAND | 'o', file_callback, NULL, 0},
    {"New window", FL_COMMAND | 'n', file_callback, NULL, 0},
    {"Close window", FL_COMMAND | 'w', file_callback, NULL, 0}
  };
  fd_nj_plot->menu_file = new vlength_menu(menubar, "File", fileitems, sizeof(fileitems)/sizeof(Fl_Menu_Item));
  (fd_nj_plot->menu_file->get_menu() + (printout_pageformat == Fl_Paged_Device::A4 ? A4_format : letter_format))->setonly();
  if (printout_layout == Fl_Paged_Device::LANDSCAPE) (fd_nj_plot->menu_file->get_menu() + landscape)->set();
    if(view == NULL || view->tot_seqs == 0) {
      (fd_nj_plot->menu_file->get_menu() + reorder_align_following_tree)->deactivate();
      fd_nj_plot->menu_file->get_menu()->deactivate();
    }
    if(!from_tree_menu) (fd_nj_plot->menu_file->get_menu() + remove_from_trees_menu)->deactivate();
    fd_nj_plot->page_count = 1;
  
  static const Fl_Menu_Item edititems[] = {
#ifdef MICRO
    {"Copy", FL_COMMAND | 'c', copy, NULL, 0},
#else
    {"Copy", 0, NULL, NULL, FL_MENU_INVISIBLE},
#endif
    {"Paste tree", FL_COMMAND | 'v', paste, NULL, 0},
    {"Find", FL_COMMAND | 'f', search_callback, NULL, 0},
    {"Again", FL_COMMAND | 'a', search_callback, NULL, 0},
    {"Edit tree header", 0, edit_tree_header, NULL, 0},
    {"Branch support threshold", 0, 0, NULL, FL_SUBMENU},
    {"0", 0, NULL, NULL, FL_MENU_DIVIDER},
    {"Edit threshold", 0, bt_threshold_callback, NULL, 0},
    {0},
    {"Root at tree center", 0, midpoint_callback, NULL, 0},
    {"Get or Set Window size", 0, set_win_size_callback, NULL, FL_MENU_DIVIDER},
    {"Edit tree shape", 0, edit_shape_callback, NULL, FL_MENU_DIVIDER},
    {"Default name trimming rule", 0, edit_trim_callback, NULL, 0},
  };
  fd_nj_plot->center_rank = 9;
  fd_nj_plot->edit_shape_rank = 11;
  fd_nj_plot->menu_edit = new vlength_menu(menubar, "Edit", edititems, sizeof(edititems)/sizeof(Fl_Menu_Item));
  fd_nj_plot->bootstrap_threshold = 0;
  
  static const Fl_Menu_Item fontitems[] = {
    // 1st item useful against FLTK bug when array of Fl_Menu_Item's begins with radio items
    {"invisible", 0, NULL, NULL, FL_MENU_INVISIBLE},
    {"Helvetica", 0, change_ffam, NULL, FL_MENU_RADIO},
    {"Courier", 0, change_ffam, NULL, FL_MENU_RADIO},
    {"Times", 0, change_ffam, NULL, FL_MENU_RADIO | FL_MENU_DIVIDER | FL_MENU_VALUE},
    {"Bold", 0, change_ffam, NULL, FL_MENU_TOGGLE},
    {"Italic", 0, change_ffam, NULL, FL_MENU_TOGGLE | FL_MENU_DIVIDER},
    {"8", 0, change_ffam, NULL, FL_MENU_RADIO},
    {"10", 0, change_ffam, NULL, FL_MENU_RADIO},
    {"12", 0, change_ffam, NULL, FL_MENU_RADIO | FL_MENU_VALUE},
    {"14", 0, change_ffam, NULL, FL_MENU_RADIO},
    {"16", 0, change_ffam, NULL, FL_MENU_RADIO},
    {"18", 0, change_ffam, NULL, FL_MENU_RADIO},
    {"24", 0, change_ffam, NULL, FL_MENU_RADIO},
  };
  fd_nj_plot->menu_font = new vlength_menu(menubar, "Font", fontitems, sizeof(fontitems)/sizeof(Fl_Menu_Item));
  
	fd_nj_plot->l_button = new Fl_Check_Button(left,3,92, 20, "Br lengths");
	fd_nj_plot->l_button->value(fd_nj_plot->plot_br_l);
	fd_nj_plot->bt_button = new Fl_Check_Button(fd_nj_plot->l_button->x() + fd_nj_plot->l_button->w() + 1,3,90, 20,
												"Br support");
	fd_nj_plot->bt_button->value(fd_nj_plot->show_bootstrap);
	fd_nj_plot->root_unroot = new Fl_Choice(fd_nj_plot->bt_button->x() + fd_nj_plot->bt_button->w() + 2, 
										   fd_nj_plot->bt_button->y(), 98, 20, "");
	fd_nj_plot->root_unroot->add("squared");
	fd_nj_plot->root_unroot->add("circular");
	fd_nj_plot->root_unroot->add("cladogram");
	fd_nj_plot->root_unroot->value(0);
	fd_nj_plot->root_unroot->clear_visible_focus();
	Fl_Button *help_button = new Fl_Button(g->w() - 52,26,50, 20, "Help");
	help_button->callback(direct_help_callback, (void *)"Tree windows");

	fd_nj_plot->full = new Fl_Round_Button(3,26,47, 20, "Full");
	fd_nj_plot->full->type(FL_RADIO_BUTTON);
	fd_nj_plot->swap_button = new Fl_Round_Button(52,26,60, 20, "Swap");
	fd_nj_plot->swap_button->type(FL_RADIO_BUTTON);
	fd_nj_plot->new_outgroup = new Fl_Round_Button(110,26,73, 20, "Re-root");
	fd_nj_plot->new_outgroup->type(FL_RADIO_BUTTON);
  fd_nj_plot->select = new Fl_Round_Button(183,26,65, 20, "Select");
  fd_nj_plot->select->type(FL_RADIO_BUTTON);
	fd_nj_plot->subtree = new Fl_Round_Button(248,26,75, 20, "Subtree");
	fd_nj_plot->subtree->type(FL_RADIO_BUTTON);
	fd_nj_plot->full->setonly();
	fd_nj_plot->up = new Fl_Button(323, 26, 90, 20, "Subtree Up");
	fd_nj_plot->up->deactivate();
	fd_nj_plot->zoom = new Fl_Simple_Counter(fd_nj_plot->up->x() + fd_nj_plot->up->w() + 45, fd_nj_plot->up->y(), 70, 20, "Zoom");
	fd_nj_plot->zoom->callback(zoom_callback, fd_nj_plot);
	fd_nj_plot->zoom->align(FL_ALIGN_LEFT);
	fd_nj_plot->zoom->value(1.);
	fd_nj_plot->zoom->bounds(1., 100.);
	fd_nj_plot->zoom->step(0.4);
	fd_nj_plot->zoom->clear_visible_focus();
	fd_nj_plot->zoomvalue = 1;
  
  fd_nj_plot->change_tree = new Fl_Simple_Counter(fd_nj_plot->root_unroot->x() + fd_nj_plot->root_unroot->w() + 5, fd_nj_plot->root_unroot->y(), 50, 20, "");
  fd_nj_plot->change_tree->value(1);
  fd_nj_plot->change_tree->step(1);
  fd_nj_plot->change_tree->bounds(1, tree_count);
  fd_nj_plot->change_tree->callback(tree_counter_callback, fd_nj_plot);
  fd_nj_plot->leaf_trimmer = new LeafNameTrimmer(g->w() - 52,
                                                 fd_nj_plot->change_tree->y(), 50, 20);
  fd_nj_plot->leaf_trimmer->user_data(fd_nj_plot);
  if (tree_count == 1) {
    fd_nj_plot->change_tree->hide();
    (fd_nj_plot->menu_file->get_menu() + save_all_trees)->deactivate();
  }

	g->end();
	g->resizable(NULL);
	
	Fl_Box *down = new Fl_Box(FL_DOWN_FRAME, 4, 50, w->w()-8, w->h()-54, NULL); 
	Fl_Window *w2 = (Fl_Window*)new Fl_Window(down->x()+3, down->y()+3, down->w()-6, down->h()-6);
	w->resizable(w2);
	w2->box(FL_FLAT_BOX);
	w2->color(FL_WHITE);
	fd_nj_plot->panel = new tree_panel(0, 0, w2->w(), w2->h());
	fd_nj_plot->panel->user_data(fd_nj_plot);
	fd_nj_plot->root_unroot->callback(rooted_unrooted_callback, fd_nj_plot);
	fd_nj_plot->scroller = new Fl_Scrollbar(w2->w() - 15, fd_nj_plot->panel->y(), 15, fd_nj_plot->panel->h()
#ifdef __APPLE__
						- 5
#endif
	);
	Fl_Box *b = new Fl_Box(0, 0, w2->w() - fd_nj_plot->scroller->w(), w2->h());
	b->hide();
	w2->resizable(b);
	fd_nj_plot->scroller->hide();
	fd_nj_plot->scroller->callback(scroller_callback, fd_nj_plot);
	fd_nj_plot->scroller->bounds(0, 1);
	fd_nj_plot->scroller->linesize(1);
	fd_nj_plot->scroller->slider_size(1);
	((Fl_Slider *)fd_nj_plot->scroller)->value(0);
	w2->end();
	fd_nj_plot->l_button->callback(toggle_br_lengths, fd_nj_plot->panel);
	fd_nj_plot->bt_button->callback(toggle_bootstrap, fd_nj_plot->panel);
	fd_nj_plot->font_family = FL_TIMES;
	fd_nj_plot->font_size = 12;
	fd_nj_plot->full->callback(operation_callback, fd_nj_plot->panel);
	fd_nj_plot->swap_button->callback(operation_callback, fd_nj_plot->panel);
	fd_nj_plot->new_outgroup->callback(operation_callback, fd_nj_plot->panel);
  fd_nj_plot->select->callback(operation_callback, fd_nj_plot->panel);
	fd_nj_plot->subtree->callback(operation_callback, fd_nj_plot->panel);
	fd_nj_plot->up->callback(subtreeup_callback, fd_nj_plot->panel);
	w->end();
	w->callback(treeplotw_callback, fd_nj_plot);
	if(name != NULL) {
		const char *mess;
		if( (mess = preptree(fd_nj_plot)) != NULL ) {
			fl_alert("%s", mess);
			fd_nj_plot->notu = 0;
			w->do_callback();
			return NULL;
			}
		else {
			fd_nj_plot->need_runtree = TRUE;
			if(!fd_nj_plot->has_br_length) {
				fd_nj_plot->l_button->deactivate();
				(fd_nj_plot->menu_edit->get_menu() + fd_nj_plot->center_rank)->hide();
				((Fl_Menu_Item *)fd_nj_plot->root_unroot->menu() + 2)->hide();
				}
			if(!fd_nj_plot->has_internal)fd_nj_plot->bt_button->deactivate();
		  (fd_nj_plot->menu_edit->get_menu() + 1)->deactivate();
			fd_nj_plot->tree_name = strdup(name);
			}
		}
	else fd_nj_plot->tree_name = strdup("no tree");
	w->copy_label(fd_nj_plot->tree_name);
#if !(defined(__APPLE__) || defined(WIN32)) && SEAVIEW_FLTK_VERSION < 133
	extern Fl_Offscreen seaview_X11_icon;
	w->icon( (void*)seaview_X11_icon );
#endif
	w->show();
#ifndef MICRO
	w->hotspot(w);
#endif
#if defined(__APPLE__) && 100*FL_MAJOR_VERSION + FL_MINOR_VERSION < 104
	add_windowmenuitem(w->label(), w);
#endif
	return w;
}


void tree_panel::draw(void)
{
	static int old_w = 0;
	static int old_h = 0;
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)this->user_data();
	if(fd_nj_plot->notu == 0) return;
	fl_font(fd_nj_plot->font_family, fd_nj_plot->font_size);
	fl_color(FL_BLACK);
	if(fd_nj_plot->need_runtree || this->w() != old_w || this->h() != old_h) {
		old_w = this->w();
		old_h = this->h();
		physx = this->w();
		physy = this->h();
		maxy = 1000.;
		runtree(fd_nj_plot);
		fd_nj_plot->need_runtree = FALSE;
		fd_nj_plot->swap = 0;
	}
	do_plot(fd_nj_plot, FALSE);
}

#ifdef MICRO
extern void frame_and_draw_unrooted(FD_unrooted *fd_unrooted);
static void copy(Fl_Widget *wgt, void *data)
{
  FD_nj_plot *fd_nj_plot = (FD_nj_plot *)wgt->user_data();
  Fl_Widget *g;
  if(fd_nj_plot->fd_unrooted != NULL) 
    g = ((FD_unrooted*)(fd_nj_plot->fd_unrooted))->unrooted_plot;
  else
    g = fd_nj_plot->panel;
  Fl_Surface_Device *current = Fl_Surface_Device::surface();
  Copy_Surface *copy_surf = new Copy_Surface(
#ifdef __APPLE__
						   fd_nj_plot->fd_unrooted == NULL, 
#endif
						   g->w(), g->h());
  copy_surf->set_current();
  if(fd_nj_plot->fd_unrooted != NULL) {
    frame_and_draw_unrooted((FD_unrooted *)fd_nj_plot->fd_unrooted);
    }
  else {
    g->draw();
    }
  delete copy_surf;
  current->set_current();
}
#endif // MICRO

static void paste(Fl_Widget *wgt, void *data)
{
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)wgt->user_data();
	Fl::paste(*fd_nj_plot->panel, 1);
}


char *prepare_ps_or_pdf_font(int font_num)
{
	/* prepare PDF name of font */
	static char current_ps_font[80];
	int modulo = font_num % 4;
	int use_bold = modulo == 1 || modulo == 3;
	int use_italic = modulo >= 2;
	int fam_num = font_num/4;
	strcpy(current_ps_font, list_ps_font_name[fam_num]);
	if( use_bold && use_italic ) {
		if(fam_num == times) strcat(current_ps_font,"-BoldItalic");
		else strcat(current_ps_font,"-BoldOblique");
		}
	else if( use_bold ) strcat(current_ps_font,"-Bold");
	else if( use_italic )  {
			if(fam_num == times) strcat(current_ps_font,"-Italic");
			else strcat(current_ps_font,"-Oblique");
		}
	else if(fam_num == times) strcat(current_ps_font,"-Roman");
	return current_ps_font;
}


void toggle_br_lengths(Fl_Widget *wgt, void *data)
{
	tree_panel *g = (tree_panel *)data;
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)g->user_data();
	fd_nj_plot->plot_br_l = ((Fl_Check_Button *)wgt)->value();
	fd_nj_plot->need_runtree = TRUE;
	g->window()->redraw();	
}

void toggle_bootstrap(Fl_Widget *wgt, void *data)
{
	tree_panel *g = (tree_panel *)data;
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)g->user_data();
	fd_nj_plot->show_bootstrap = ((Fl_Check_Button *)wgt)->value();
	fd_nj_plot->need_runtree = TRUE;
	g->window()->redraw();	
}

void change_ffam(Fl_Widget *wgt, void *data)
{
  FD_nj_plot *fd_nj_plot = (FD_nj_plot *)wgt->user_data();
  int val = ((Fl_Menu_*)wgt)->mvalue() - 1 - fd_nj_plot->menu_font->get_menu();
	if(val < 3) fd_nj_plot->font_family = FL_HELVETICA + 4 * val;
	else if (val >= 5) sscanf( ((Fl_Menu_ *)wgt)->text(), "%d", &fd_nj_plot->font_size);
	fd_nj_plot->font_family = 4*(fd_nj_plot->font_family/4);
	const Fl_Menu_Item* menu = fd_nj_plot->menu_font->get_menu();
	int bold = (menu[4].flags & FL_MENU_VALUE);
	int italic = (menu[5].flags & FL_MENU_VALUE);
	if(bold && italic) fd_nj_plot->font_family += 3;
	else if (bold)  fd_nj_plot->font_family += 1;
	else if (italic)  fd_nj_plot->font_family += 2;
	if( fd_nj_plot->fd_unrooted != NULL) {
		FD_unrooted *fd_unrooted = (FD_unrooted *)fd_nj_plot->fd_unrooted;
		fd_unrooted->current_font = fd_nj_plot->font_family;
		fd_unrooted->font_size = fd_nj_plot->font_size;
		fd_unrooted->comp_phys_bounds = TRUE;
		fd_unrooted->unrooted_plot->window()->redraw();
		}
	else {
		fd_nj_plot->need_runtree = TRUE;
		fd_nj_plot->panel->window()->redraw();
		}
}

int tree_panel::handle(int event)
{
FD_nj_plot *fd_nj_plot = (FD_nj_plot *)this->user_data();
const char *p;
char *q;
Fl_Window *w;
int my = Fl::event_y(), count = 1;
static int start_y;
static int start_sy;
static int changed_cursor = FALSE;
			
switch (event)
    {
	case FL_PUSH:
		tree_click_proc(this, Fl::event_x(), Fl::event_y());
		start_y = my;
		start_sy = fd_nj_plot->scroller->value();
		break;
	case FL_DRAG:
			if(fd_nj_plot->zoomvalue > 1) {
				if(!changed_cursor) this->window()->cursor(FL_CURSOR_NS); 
				changed_cursor = TRUE;
				int val = (int)fd_nj_plot->scroller->clamp(start_sy - 
						(my - start_y) * fd_nj_plot->scroller->maximum() / (fd_nj_plot->zoomvalue - 1) / 
						(this->h()-2.6*fd_nj_plot->char_height) );
				((Fl_Slider *)fd_nj_plot->scroller)->value(val);
				fd_nj_plot->scroller->do_callback();
			}
			break;
	case FL_RELEASE:
			if(fd_nj_plot->zoomvalue > 1) {
				fl_reset_cursor(this->window());
				changed_cursor = FALSE;
			}
			break;
	case FL_PASTE:
		p = Fl::event_text();
		fd_nj_plot->trees = (char *)malloc(strlen(p) + 1);
    fd_nj_plot->current_tree = fd_nj_plot->trees;
		q = fd_nj_plot->trees;
		while(*p != 0) {
			if(*p != '\n' && *p != '\r') *q++ = *p;
			p++;
			}
		*q = 0;
		if(preptree(fd_nj_plot) != NULL) {
			fl_alert("Clipboard does not contain Newick-formatted tree");
			break;
			}
		fd_nj_plot->need_runtree = TRUE;
    count = calc_tree_count(fd_nj_plot->trees);
		if(!fd_nj_plot->has_br_length) {
			fd_nj_plot->l_button->deactivate();
			(fd_nj_plot->menu_edit->get_menu() + fd_nj_plot->center_rank)->hide();
			((Fl_Menu_Item *)fd_nj_plot->root_unroot->menu() + 2)->hide();
			} 
		if(!fd_nj_plot->has_internal)fd_nj_plot->bt_button->deactivate();
	(fd_nj_plot->menu_edit->get_menu() + 1)->deactivate();
		fd_nj_plot->tree_name = strdup("pasted tree");
		w = fd_nj_plot->full->window();
		w->copy_label(fd_nj_plot->tree_name);
#if defined(__APPLE__) && 100*FL_MAJOR_VERSION + FL_MINOR_VERSION < 104
		rename_windowmenuitem(w->label(), find_windowmenuitem(w));
#endif
    if (count > 1) {
      fd_nj_plot->change_tree->show();
      fd_nj_plot->change_tree->bounds(1, count);
    }
		fd_nj_plot->panel->window()->redraw();
		break;
	default :
		return 0; // other events are not processed here
	}
	return 1; // from processed events
}


void scrollnotu(FD_nj_plot *fd_nj_plot)
{
	int lines;
	float old;
	int count = fd_nj_plot->subtree_notu;
	
	old = fd_nj_plot->scroller->value() / (float)fd_nj_plot->scroller->maximum();
	lines = (int)((count + 1) * (1 - 1/fd_nj_plot->zoomvalue) + 0.5); 
	if(lines < 3) lines = 3;
	fd_nj_plot->scroller->maximum(lines);
	((Fl_Slider *)fd_nj_plot->scroller)->value(old * lines);
	fd_nj_plot->scroller->slider_size(1/fd_nj_plot->zoomvalue);
}


void zoom_callback(Fl_Widget *wgt, void *data)
{
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)data;
	if(((Fl_Counter *)wgt)->value() > fd_nj_plot->zoomvalue) fd_nj_plot->zoomvalue *= 1.4;
	else fd_nj_plot->zoomvalue /= 1.4;
	fd_nj_plot->zoomvalue = ((Fl_Counter *)wgt)->clamp(fd_nj_plot->zoomvalue);
	((Fl_Counter *)wgt)->value(fd_nj_plot->zoomvalue);
	if(fd_nj_plot->zoomvalue <= 1.1) {
		fd_nj_plot->panel->size(fd_nj_plot->panel->parent()->w(), 
										 fd_nj_plot->panel->h());
		fd_nj_plot->scroller->hide();
	}
	else {
		fd_nj_plot->panel->size(fd_nj_plot->panel->parent()->w() - 15, 
										 fd_nj_plot->panel->h());
		fd_nj_plot->scroller->show();
	}
	scrollnotu(fd_nj_plot);
	fd_nj_plot->panel->parent()->redraw();
}

void scroller_callback(Fl_Widget *wgt, void *data)
{
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)data;
	fd_nj_plot->panel->parent()->redraw();
}

static void patristic_callback(Fl_Widget *wgt, void *unused)
{
  char *outfname, *p;
  FD_nj_plot *fd_nj_plot = (FD_nj_plot *)wgt->user_data();
  Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
  chooser->type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  outfname = new char[strlen(fd_nj_plot->tree_name)+10];
  strcpy(outfname, fd_nj_plot->tree_name);
  p = strrchr(outfname, '.'); if (!p) p = outfname + strlen(outfname);
  strcpy(p, ".dist");
  chooser->preset_file(outfname);
  chooser->options(Fl_Native_File_Chooser::SAVEAS_CONFIRM | chooser->options());
  chooser->title("Choose filename to store patristic distances");   
  char *filename = run_and_close_native_file_chooser(chooser);
  delete[] outfname;
  if (filename == NULL) return;
  FILE *out = fopen(filename, "w");
  if (!out) return;
  calc_patristic_distances(fd_nj_plot, out);
  fclose(out);
}

static void file_callback(Fl_Widget *wgt, void *data)
{
  FD_nj_plot *fd_nj_plot = (FD_nj_plot *)wgt->user_data();
	Fl_Menu_Item *item = fd_nj_plot->menu_file->get_menu();
	int reponse =  ((Fl_Menu_*)wgt)->mvalue() - item;
	if(reponse == save_to_trees_menu) {//save to tree menu
		add_to_tree_menu(fd_nj_plot);
	}
	else if(reponse == remove_from_trees_menu) {//remove from tree menu
		remove_from_tree_menu(fd_nj_plot, wgt->window()->label());
	}
	else if(reponse == save_rooted || reponse == save_unrooted) {//save to file
        int saved_notu = fd_nj_plot->subtree_center != NULL ? fd_nj_plot->subtree_notu : fd_nj_plot->notu;
		if( (reponse == save_rooted && saved_notu < 2) || (reponse == save_unrooted && saved_notu < 3) ) return;
		Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
		chooser->type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	  chooser->options(Fl_Native_File_Chooser::SAVEAS_CONFIRM | chooser->options());
		chooser->title("Set tree filename");   
	  chooser->preset_file(fd_nj_plot->tree_name);
		char *filename = run_and_close_native_file_chooser(chooser);
		if(filename == NULL) return;
        struct noeud *center = fd_nj_plot->subtree_center != NULL ? fd_nj_plot->subtree_center : fd_nj_plot->racine;
	  char *tree;
	  if(reponse == save_rooted) tree = ecrit_arbre_parenth(fd_nj_plot, center);
	  else tree = ecrit_arbre_parenth_unrooted(fd_nj_plot, center);
	  FILE *out = fopen(filename, "w");
          if(out) {
	    fputs(tree, out);
	    fputs("\n", out);
	    fclose(out);
	    free(tree);
	    }
	  else fl_alert("Incorrect output filename: %s", filename);
	  }
  else if (reponse == save_all_trees) {
    Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
    chooser->type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    chooser->options(Fl_Native_File_Chooser::SAVEAS_CONFIRM | chooser->options());
    chooser->title("Set multiple-tree filename");
    chooser->preset_file(fd_nj_plot->tree_name);
    char *filename = run_and_close_native_file_chooser(chooser);
    if (filename == NULL) return;
    FILE *out = fopen(filename, "w");
    if (out) {
      char *p = fd_nj_plot->trees;
      while (*p) {
        fputc(*p, out);
        if (*p == ';') fputc('\n', out);
        p++;
      }
      fclose(out);
    }
  }
	else if(reponse == print_tree) {//print
		print_plot(fd_nj_plot, false);
	}
	else if(reponse == save_as_ps_pdf) {//save to PDF/PS
	  print_plot(fd_nj_plot, true);
	  }
	else if(reponse == save_as_svg) {// SVG
	  extern void svg_tree_dialog_and_save(FD_nj_plot *fd_nj_plot);
	  svg_tree_dialog_and_save(fd_nj_plot);
	}
	else if(reponse == A4_format) {// A4
	  printout_pageformat = Fl_Paged_Device::A4;
	}
	else if(reponse == letter_format) {// Letter
	  printout_pageformat = Fl_Paged_Device::LETTER;
	}
        else if(reponse == landscape) {// Landscape
          printout_layout = (item + landscape)->value() ? Fl_Paged_Device::LANDSCAPE : Fl_Paged_Device::PORTRAIT;
        }
	else if(reponse >= page_count+1 && reponse <= page_count+12) {// page counts
		if(reponse >= page_count+1 && reponse <= page_count+10) {// fixed page counts
			fd_nj_plot->page_count = reponse - page_count;
		}
		else if(reponse == page_count+11) {// variable page count
			sscanf((item + page_count+11)->label(), "%d", &fd_nj_plot->page_count);
		}
		else if(reponse == page_count+12) {
			int newval;
			const char *reply = fl_input("Enter page count (>= 11)", "11");
			if(reply == NULL) return;
			sscanf(reply, "%d", &newval);
			if(newval >= 1) fd_nj_plot->page_count = newval;
			if(newval >= 11) {
			  if (strcmp((item + page_count + 11)->label(), "11") != 0) free((void*)(item + page_count + 11)->label());
			  (item + page_count + 11)->label(strdup(reply));
				(item + page_count + 11)->setonly();
				}
			else if(newval >= 1) {
				(item + page_count + fd_nj_plot->page_count)->setonly();
				}
			}
		char tmp[20];
		sprintf(tmp, "Page count (%d)", fd_nj_plot->page_count);
	  if (strcmp((item + page_count)->label(), "Page count") != 0) free((void*)(item + page_count)->label());
	  (item + page_count)->label(strdup(tmp));
	}
	else if(reponse == reorder_align_following_tree) {
		reorder_following_tree(fd_nj_plot->racine, fd_nj_plot->notu, fd_nj_plot->view);
	}
	else if(reponse == select_seqs_in_alignment) {
		select_in_alignment(fd_nj_plot);
	}
	else if(reponse == open_tree_or_align) {//open tree/alignment
		Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
		chooser->type(Fl_Native_File_Chooser::BROWSE_FILE);
		chooser->title("Choose a Newick tree or alignment file");                        
		char *filename = run_and_close_native_file_chooser(chooser);
		if(filename == NULL) return;
		Fl_Window *w = use_initial_file(fd_nj_plot->view, filename, FALSE);
		//close the opening window if it was empty and a new tree window was successfully created
		const char *c;
		if(w != NULL && fd_nj_plot->notu == 0 && (c = w->xclass()) != NULL && strcmp(c, TREE_WINDOW) == 0) 
			fd_nj_plot->full->window()->do_callback();
	}
	else if(reponse == new_window) {//new tree window
		treedraw(NULL, fd_nj_plot->view, NULL, FALSE);
	}
	else if(reponse == close_window) {//close
	  wgt->window()->do_callback();
	}
}

void remove_from_tree_menu(FD_nj_plot *fd_nj_plot, const char *name)
{
  SEA_VIEW *view = fd_nj_plot->view;
	if(view == NULL) return;
	vlength_menu *menu_trees = view->menu_trees;
	Fl_Menu_Item *item;
	Fl_Menu_Item *items = menu_trees->vitem(0);
	if( (item = menu_trees->find_item(name)) == NULL) return;
	int rank = item - items;
	menu_trees->remove(rank);
	items = fd_nj_plot->menu_file->get_menu();
	items->activate();
	(items + remove_from_trees_menu)->deactivate();
	free(view->trees[rank]);
	(view->tot_trees)--;
	for(int i = rank; i < view->tot_trees; i++)  view->trees[i] = view->trees[i + 1];
	if(view->tot_trees == 0) free(view->trees);
}

void add_to_tree_menu(FD_nj_plot *fd_nj_plot)
{
	SEA_VIEW *view = fd_nj_plot->view;
	char *p, *fulltree;
	if(view == NULL) return;
	char *name = fd_nj_plot->tree_name;
	vlength_menu *menu_trees = view->menu_trees;
	name = (char *)fl_input("Enter tree name:", name);
	if(name == NULL) return;
	free(fd_nj_plot->tree_name);
	char *newname = NULL;
	int count=2;
	if( menu_trees->find_item(name) != NULL) {//check if same name already exists
		newname = new char[strlen(name) + 5];
		do sprintf(newname, "%s_%d", name, count++);
		while( menu_trees->find_item(newname) != NULL);
		name = newname;
	}
  fd_nj_plot->tree_name = strdup(name);
  fd_nj_plot->full->window()->copy_label(fd_nj_plot->tree_name);
  menu_trees->add(name, trees_callback, NULL, 0);
  int rank = menu_trees->vlength();
  menu_trees->vitem(rank - 1)->labelfont(FL_HELVETICA_ITALIC);
	if(newname != NULL) delete[] newname;
	if(view->tot_trees == 0) view->trees = (char **)malloc(sizeof(char *));
	else view->trees = (char **)realloc(view->trees, (view->tot_trees + 1) * sizeof(char *));
  if (fd_nj_plot->change_tree->visible()) {
    fulltree = strdup(fd_nj_plot->trees);
  } else {
    p = ecrit_arbre_parenth(fd_nj_plot, fd_nj_plot->racine);
    if(fd_nj_plot->tree_label != NULL) {
      fulltree = (char *)malloc(strlen(p) + strlen(fd_nj_plot->tree_label) + 3);
      sprintf(fulltree, "[%s]%s", fd_nj_plot->tree_label, p);
      free(p);
    }
    else fulltree = p;
  }
	view->trees[view->tot_trees] = fulltree;
	view->tot_trees++;
  (fd_nj_plot->menu_file->get_menu() + remove_from_trees_menu)->activate();
}

void treeplotw_callback(Fl_Widget *wgt, void *data)
{
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)data;
	if(fd_nj_plot->fd_unrooted != NULL) {
		FD_unrooted *fd_unrooted = (FD_unrooted *)fd_nj_plot->fd_unrooted;
		free_unrooted(fd_unrooted);
		}
	if(fd_nj_plot != NULL) {
		free_tree(fd_nj_plot);
	  delete fd_nj_plot->menu_file;
	  delete fd_nj_plot->menu_edit;
	  delete fd_nj_plot->menu_font;
		free(fd_nj_plot);
		}
#if defined(__APPLE__) && 100*FL_MAJOR_VERSION + FL_MINOR_VERSION < 104
	int rank = find_windowmenuitem((Fl_Window *)wgt);
	if(rank > 0) delete_windowmenuitem(rank);
#endif
//when a callback destroys its own widget delete is bad; hide()+Fl::delete_widget is to be called instead
	wgt->hide();
	Fl::delete_widget(wgt);
}

void subtreeup_callback(Fl_Widget *wgt, void *data)
{
	tree_panel *g = (tree_panel *)data;
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)g->user_data();
	fd_nj_plot->subtree_center = fd_nj_plot->subtree_ascend;
	fd_nj_plot->subtree_ascend = fd_nj_plot->subtree_center->v3;
	if(fd_nj_plot->subtree_ascend == NULL) {
		fd_nj_plot->subtree_center = NULL;
		wgt->deactivate();
		fd_nj_plot->new_outgroup->activate();
		((Fl_Menu_Item *)fd_nj_plot->menu_edit->get_menu() + fd_nj_plot->center_rank)->activate();
		fd_nj_plot->full->setonly();
	  (fd_nj_plot->menu_file->get_menu()+reorder_align_following_tree)->activate();
	  (fd_nj_plot->menu_file->get_menu()+select_seqs_in_alignment)->deactivate();
		fd_nj_plot->subtree_notu = fd_nj_plot->notu;
		if(fd_nj_plot->root_unroot != NULL) fd_nj_plot->root_unroot->activate();
    fd_nj_plot->change_tree->activate();
	  (fd_nj_plot->menu_file->get_menu()+save_rooted)->label("Save rooted tree");
	  (fd_nj_plot->menu_file->get_menu()+save_unrooted)->label("Save unrooted tree");
	}
	else fd_nj_plot->subtree_notu = calc_n_desc(fd_nj_plot->subtree_center);
	fd_nj_plot->need_runtree = TRUE;
	fd_nj_plot->swap_button->value(0);
	fd_nj_plot->subtree->value(0);
  fd_nj_plot->select->value(0);
	fd_nj_plot->choix = show_tree;
	if(fd_nj_plot->has_internal) fd_nj_plot->bt_button->activate();
	if(fd_nj_plot->has_br_length) fd_nj_plot->l_button->activate();
	if(fd_nj_plot->select_clade_button != NULL) fd_nj_plot->select_clade_button->activate();
	g->window()->redraw();
}

void operation_callback(Fl_Widget *wgt, void *data)
{
	tree_panel *g = (tree_panel *)data;
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)g->user_data();
	((Fl_Button *)wgt)->setonly();
	if(wgt == fd_nj_plot->swap_button) {
		fd_nj_plot->choix = permutation;
		fd_nj_plot->show_bootstrap = FALSE;
		fd_nj_plot->plot_br_l = FALSE;
		fd_nj_plot->l_button->deactivate();
		fd_nj_plot->l_button->value(0);
		fd_nj_plot->bt_button->deactivate();
		fd_nj_plot->bt_button->value(0);
		if(fd_nj_plot->select_clade_button != NULL) fd_nj_plot->select_clade_button->activate();
		}
	else if(wgt == fd_nj_plot->new_outgroup) {
		fd_nj_plot->choix = depl_racine;
		fd_nj_plot->show_bootstrap = FALSE;
		fd_nj_plot->plot_br_l = FALSE;
		fd_nj_plot->l_button->deactivate();
		fd_nj_plot->l_button->value(0);
		fd_nj_plot->bt_button->deactivate();
		fd_nj_plot->bt_button->value(0);
	  (fd_nj_plot->menu_file->get_menu()+reorder_align_following_tree)->deactivate(); // Re-order following tree
	}
	else if(wgt == fd_nj_plot->subtree) {
		fd_nj_plot->choix = subtree;
		fd_nj_plot->show_bootstrap = FALSE;
		fd_nj_plot->plot_br_l = FALSE;
		((Fl_Menu_Item *)fd_nj_plot->menu_edit->get_menu() + fd_nj_plot->center_rank)->deactivate();
		fd_nj_plot->new_outgroup->deactivate();
		fd_nj_plot->l_button->deactivate();
		fd_nj_plot->l_button->value(0);
		fd_nj_plot->bt_button->deactivate();
		fd_nj_plot->bt_button->value(0);
    fd_nj_plot->change_tree->deactivate();
		if(fd_nj_plot->root_unroot != NULL) fd_nj_plot->root_unroot->deactivate();
	}
	else if(wgt == fd_nj_plot->select) {
	  fd_nj_plot->choix = selection;
	  fd_nj_plot->l_button->deactivate();
	  fd_nj_plot->l_button->value(0);
	  fd_nj_plot->bt_button->deactivate();
	  fd_nj_plot->bt_button->value(0);
	  fd_nj_plot->show_bootstrap = FALSE;
	  fd_nj_plot->plot_br_l = FALSE;
	  (fd_nj_plot->menu_file->get_menu()+reorder_align_following_tree)->deactivate(); // Re-order following tree
	  }
	else {//full
		fd_nj_plot->choix = show_tree;
		fd_nj_plot->subtree_notu = fd_nj_plot->notu;
		if(fd_nj_plot->subtree_center != NULL) scrollnotu(fd_nj_plot);
		fd_nj_plot->subtree_center = NULL;
		fd_nj_plot->new_outgroup->activate();
		((Fl_Menu_Item *)fd_nj_plot->menu_edit->get_menu() + fd_nj_plot->center_rank)->activate();
		fd_nj_plot->up->deactivate();
	  if(fd_nj_plot->view != NULL) (fd_nj_plot->menu_file->get_menu()+reorder_align_following_tree)->activate();
	  (fd_nj_plot->menu_file->get_menu()+select_seqs_in_alignment)->deactivate();
		if(fd_nj_plot->has_internal) fd_nj_plot->bt_button->activate();
		if(fd_nj_plot->has_br_length) fd_nj_plot->l_button->activate();
		if(fd_nj_plot->root_unroot != NULL) fd_nj_plot->root_unroot->activate();
    fd_nj_plot->change_tree->activate();
	  (fd_nj_plot->menu_file->get_menu()+save_rooted)->label("Save rooted tree");
	  (fd_nj_plot->menu_file->get_menu()+save_unrooted)->label("Save unrooted tree");
	}
	fd_nj_plot->need_runtree = TRUE;
	g->window()->redraw();
}

void select_clade(struct noeud *n, SEA_VIEW *view, int on, char **labels)
/* select (on is true) or deselect (on is false) all seqs in alignment placed below node n in tree
 */
{
  if (n == NULL) return;
  select_clade(n->v1, view, on, labels);
  select_clade(n->v2, view, on, labels);
  char *p = labels[n->rank];
  if (n->v1 == NULL) { // a leaf
    int num;
    for ( num = 0; num < view->tot_seqs; num++) { // find what sequence it is in alignment
      if (strcmp(p, view->seqname[num]) == 0) {
	if ((view->sel_seqs[num] && !on) || (!view->sel_seqs[num] && on)) select_deselect_seq(view, num+1, TRUE);
	break;
	}
      }
    }
  p = n->nom;
  if (*p) *p = (on ? '(' : ')');
}

void tree_click_proc(tree_panel *panel, int mx, int my)
{
	double eps, x, y, epsx, epsy, px, py;
	int i, found=0, node_num;
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *) panel->user_data();
	if(fd_nj_plot->notu == 0) return;
	if(fd_nj_plot->choix != permutation && fd_nj_plot->choix != depl_racine && fd_nj_plot->choix != subtree && 
		fd_nj_plot->choix != selection &&fd_nj_plot->choix != prune_clade && fd_nj_plot->choix != prune_clade_found) 
		return;
	eps = fd_nj_plot->char_height/2.;
	x = mx - panel->x();
	my -= panel->y();
	y = panel->h() - my;
	//next 5 statements so that physic_to_logic apply to the clicked window
	tek_dx = fd_nj_plot->tek_dx;
	tek_dy = fd_nj_plot->tek_dy;
	tek_ymin = fd_nj_plot->tek_ymin;
	physx_min = fd_nj_plot->ascent;
	physy_min = 0.6 * fd_nj_plot->char_height;
	physic_to_logic(x, y, &x, &y); 
	epsx = eps / tek_dx;
	epsy = eps / tek_dy;
	for(i=0; i<=fd_nj_plot->totpoints; i++ ) {
		px = (fd_nj_plot->points+i)->x; py = (fd_nj_plot->points+i)->y;
		if(x < px || x > px + epsx) continue;
		if(y < py - epsy/2 || y > py + epsy/2) continue;
		found=1;
		break;
	}
	if(!found)return;
	node_num=(fd_nj_plot->points+i)->number;
	if(fd_nj_plot->choix==depl_racine) {
		if(node_num >= 0 && node_num <= 2*fd_nj_plot->notu - 3 && 
		   node_num != fd_nj_plot->root_num) {
			fd_nj_plot->root_num = node_num;
			removeroot(fd_nj_plot);
		} 
	}
	else if(fd_nj_plot->choix==permutation) {
		if(node_num >= fd_nj_plot->notu && node_num <= 2*fd_nj_plot->notu - 2) 
			fd_nj_plot->swap = node_num;
		else
			fd_nj_plot->swap = 0;
	}
	else if(fd_nj_plot->choix==subtree) {
		if(node_num < fd_nj_plot->notu || node_num > 2*fd_nj_plot->notu - 2) return;
		fd_nj_plot->subtree_center = fd_nj_plot->tabtax[node_num];
		fd_nj_plot->subtree_ascend = fd_nj_plot->subtree_center->v3;
		fd_nj_plot->subtree_notu = calc_n_desc(fd_nj_plot->subtree_center);
		scrollnotu(fd_nj_plot);
		fd_nj_plot->choix = show_tree;
		fd_nj_plot->subtree->value(0);
		fd_nj_plot->up->activate();
		if(fd_nj_plot->select_clade_button != NULL) fd_nj_plot->select_clade_button->activate();
		if(fd_nj_plot->has_br_length) {
			fd_nj_plot->l_button->activate();
		}
		if(fd_nj_plot->has_internal) {
			fd_nj_plot->bt_button->activate();
		} 
	  (fd_nj_plot->menu_file->get_menu()+reorder_align_following_tree)->deactivate();
	  (fd_nj_plot->menu_file->get_menu()+select_seqs_in_alignment)->activate();
	  (fd_nj_plot->menu_file->get_menu()+save_rooted)->label("Save rooted subtree");
	  (fd_nj_plot->menu_file->get_menu()+save_unrooted)->label("Save unrooted subtree");
	}
	else if(fd_nj_plot->choix == selection) {
	  SEA_VIEW *view = fd_nj_plot->view;
	  if (!view) return;
	  if(node_num >= fd_nj_plot->notu && node_num <= 2*fd_nj_plot->notu - 2) {
	    int on = fd_nj_plot->tabtax[node_num]->nom[0] == ')';
	    select_clade(fd_nj_plot->tabtax[node_num], view, on, fd_nj_plot->labels);
	    view->DNA_obj->redraw();
	    panel->window()->redraw();
	    return;
	    }
	  for (int num = 0; num < view->tot_seqs; num++) {
	    if (strcmp(fd_nj_plot->labels[node_num], view->seqname[num]) == 0) {
	      select_deselect_seq(view, num+1, TRUE);
	      fd_nj_plot->tabtax[node_num]->nom[0] = (view->sel_seqs[num] ? '(' : ')');
	      extern void set_and_show_new_cursor_seq(SEA_VIEW *view, int new_pos);
	      if (num+1 < view->first_seq || num+1 >= view->first_seq + view->tot_lines)
		set_and_show_new_cursor_seq(view, num+1);
	      view->DNA_obj->redraw();
	      panel->window()->redraw();
	      break;
	      }
	    }
	  return;
	  }
	else if(fd_nj_plot->choix==prune_clade) {
		fd_nj_plot->prune_clade_node = fd_nj_plot->tabtax[node_num];
		fd_nj_plot->choix = prune_clade_found;
		fd_nj_plot->select_clade_button->activate();
		fd_nj_plot->delete_clade_button->activate();
		}
	else if(fd_nj_plot->choix==prune_clade_found) {
		struct noeud *sister, *parent, *gparent;//move selected clade to its target branch
		parent = fd_nj_plot->prune_clade_node->v3;
		sister = parent->v1 == fd_nj_plot->prune_clade_node ? parent->v2 : parent->v1;
		gparent = parent->v3;
		if(gparent != NULL) {
			if(gparent->v1 == parent) { gparent->v1 = sister; gparent->l1 = 1; }
			else { gparent->v2 = sister; gparent->l2 = 1; }
			sister->v3 = gparent;
			sister->l3 = 1;
			gparent = fd_nj_plot->tabtax[node_num]->v3;
			parent->v1 = fd_nj_plot->prune_clade_node;
			parent->l1 = 1;
			fd_nj_plot->prune_clade_node->v3 = parent;
			fd_nj_plot->prune_clade_node->l3 = 1;
			parent->v2 = fd_nj_plot->tabtax[node_num];
			parent->l2 = 1;
			fd_nj_plot->tabtax[node_num]->v3 = parent;
			fd_nj_plot->tabtax[node_num]->l3 = 1;
			parent->v3 = gparent;
			parent->l3 = 1;
			if(gparent->v1 == fd_nj_plot->tabtax[node_num]) { gparent->v1 = parent; gparent->l1 = 1; }
			else { gparent->v2 = parent; gparent->l2 = 1; }
			}
		else {
			parent->v1 = sister->v1; parent->l1 = 1;
			parent->v2 = sister->v2; parent->l2 = 1;
			parent->v1->v3 = parent; parent->v1->l3 = 1;
			parent->v2->v3 = parent; parent->v2->l3 = 1;
			gparent = fd_nj_plot->tabtax[node_num]->v3;
			if(gparent->v1 == fd_nj_plot->tabtax[node_num]) { gparent->v1 = sister; gparent->l1 = 1;}
			else { gparent->v2 = sister; gparent->l2 = 1;}
			sister->v3 = gparent; sister->l3 = 1;
			sister->v1 = fd_nj_plot->prune_clade_node; sister->l1 = 1;
			sister->v2 = fd_nj_plot->tabtax[node_num]; sister->l2 = 1;
			fd_nj_plot->prune_clade_node->v3 = sister; fd_nj_plot->prune_clade_node->l3 = 1;
			fd_nj_plot->tabtax[node_num]->v3 = sister; fd_nj_plot->tabtax[node_num]->l3 = 1;
			}
		calc_brl_for_lengthless(fd_nj_plot->racine, NULL);
		}
	fd_nj_plot->need_runtree = TRUE;
	panel->window()->redraw();
}


double length_log_phys(double p)
{
return p * tek_dx;
}

double length_phys_log(double p)
{
return  p / tek_dx;
}


double arrondi_echelle(double x)
{ /* arrondi x a une valeur 1, 2, 5 pour echelle */
double l, n;
int r;
static int corresp[] = {1,1,2,2,5,5,5,10,10,10,10,10};
                     /* 0,1,2,3,4,5,6, 7, 8, 9,10,11 */
l = log10(x);
n = floor(l);
l = x * pow(10., -n);  /* 10. plutot que 10 necessaire pour alpha! */
r = myrint(l); r = corresp[r];
return r * pow(10., n);  /* 10. plutot que 10 necessaire pour alpha! */
}


void scale_window(double lxmin, double lxmax, double lymin, double lymax,
 double pxmin, double pxmax, double pymin, double pymax)
/* to scale the plot window for logical coords l... in physical coords p... */
{
tek_xmin=lxmin; tek_ymin=lymin; 
tek_dx = (pxmax-pxmin)/(lxmax-tek_xmin < 1e-4 ? 1e-4 : lxmax-tek_xmin);
tek_dy = (pymax-pymin)/(lymax-tek_ymin);
}


void ch_echelle(double lx, double ly, double *px, double *py)
/* conversion coord logique lx,ly en coord physique px,py */
{
*px = (lx-tek_xmin)*tek_dx + physx_min;
*py = (ly-tek_ymin)*tek_dy + physy_min;
}

void physic_to_logic(double px, double py, double *lx, double *ly)
/* conversion coord physique px,py en coord logique lx,ly */
{
	*lx = (px - physx_min)/tek_dx + tek_xmin;
	*ly = (py - physy_min)/tek_dy + tek_ymin;
}


double calc_echelle(double larg)
{ /* rend taille logique pour echelle optimale */
double log_val, phys_val;
phys_val = larg/10;
log_val = length_phys_log(phys_val);
log_val = arrondi_echelle(log_val);
return log_val;
}


void draw_scale(FD_nj_plot *fd_nj_plot)
{
	char ech_name[20];
	double phys_w, y, xd, xf, lc;
	double log_val;

	y = physy - 1.1 * fd_nj_plot->char_height;
	if(fd_nj_plot->choix == prune_clade) {
		fl_color(FL_RED);
		dir_moveto( physx * 0.01, y ); //sd be placed here to avoid pdf error
		plotstring("Click on a square to select sequence group or \"End edit\".");
		fl_color(FL_BLACK);
	}
	else if(fd_nj_plot->choix == prune_clade_found) {
		fl_color(FL_BLUE);
		dir_moveto( physx * 0.01, y ); 
		plotstring("Click on a square to move selection or \"Select group|Delete group|End edit\".");
		fl_color(FL_BLACK);
	}
	else if(fd_nj_plot->tree_label != NULL) {
		dir_moveto( physx * 0.01, y ); 
		plotstring(fd_nj_plot->tree_label);
	}
	
	log_val = calc_echelle(physx);	
	phys_w = myrint(length_log_phys(log_val));
	fl_font(FL_TIMES, 12);
	y = physy - 1.1 * 12;
	xf = physx * 0.95;
	xd = xf - phys_w;
	sprintf(ech_name, "%.1g", log_val);
	lc = calc_text_size(ech_name, NULL, NULL);
	if(fd_nj_plot->has_br_length) {
		dir_moveto(xd, y);
		dir_lineto(xf, y);
		dir_moveto(xd, y - 4);
		dir_lineto(xd, y + 4);
		dir_moveto(xf, y - 4);
		dir_lineto(xf, y + 4);
		dir_moveto( (xd + xf)/2 - lc/2, y + 2 ); 
		plotstring(ech_name);
		}
}

void do_plot(FD_nj_plot *fd_nj_plot, int doing_print)
{
struct trait *p;
int num;
double factor, value, m = 1, v = 0, zoom = 1;

if(! doing_print) {
	/* size of the tree plot window  */
	v = fd_nj_plot->scroller->value();
	m = fd_nj_plot->scroller->maximum();
	zoom = fd_nj_plot->zoomvalue;
	}

factor = 1e99;
for(num=0; num<fd_nj_plot->notu; num++) {
	if(fd_nj_plot->profs[num] <= 0) continue; 
	value = (physx - fd_nj_plot->widnames[num] - 2 * fd_nj_plot->ascent) / fd_nj_plot->profs[num];
	if(value < factor) factor = value;
	}

physx_min  = fd_nj_plot->ascent;
physx_corr = maxx * factor + physx_min;
if(physx_corr <= physx_min + 1) {
	physx_corr = physx_min + 1;
	}
physy_corr = physy - 2 * fd_nj_plot->char_height;
physy_min  = 0.6 * fd_nj_plot->char_height;
scale_window(0, maxx, 
	((m-v)/m) * (maxy - maxy/zoom), 
	((m-v)/m) * (maxy - maxy/zoom) + maxy/zoom, 
	physx_min, physx_corr, physy_min, physy_corr);
fd_nj_plot->tek_dx = tek_dx;
fd_nj_plot->tek_dy = tek_dy;
fd_nj_plot->tek_ymin = tek_ymin;

for(num=0; num<=fd_nj_plot->totnoms; num++) {
	mydrawstring( (fd_nj_plot->noms+num)->x, (fd_nj_plot->noms+num)->y , (fd_nj_plot->noms+num)->nom,
			(fd_nj_plot->noms+num)->disp_option, fd_nj_plot->char_height );
	}
for(num=0; num<=fd_nj_plot->tottraits; num++) {
	p= fd_nj_plot->traits+num;
	moveto(p->xd,p->yd);
	fl_line_style(0, p->width);
	lineto(p->xf,p->yf);
	}

if(fd_nj_plot->choix == prune_clade_found) {//redraw in color the prune clade subtree
	fl_color(FL_RED);
	for(num=fd_nj_plot->colored_names_1; num<=fd_nj_plot->colored_names_2; num++) {
		mydrawstring( (fd_nj_plot->noms+num)->x, (fd_nj_plot->noms+num)->y , (fd_nj_plot->noms+num)->nom,
					 (fd_nj_plot->noms+num)->disp_option, fd_nj_plot->char_height );
	}
	for(num=fd_nj_plot->colored_traits_1; num<=fd_nj_plot->colored_traits_2; num++) {
		p= fd_nj_plot->traits+num;
		moveto(p->xd,p->yd);
		fl_line_style(0, 0);
		lineto(p->xf,p->yf);
	}
	fl_color(FL_BLACK);
}
/* echelle */
fl_line_style(0, 0);
draw_scale(fd_nj_plot);
} /* end of do_plot */



int calc_text_size(char *text, int *pheight, int *pascent)
{
	int width, height, ascent;

	width = (int)fl_width(text);
	height = fl_height();
	ascent = fl_height() - fl_descent();
	if(pheight != NULL) *pheight = height;
	if(pascent != NULL) *pascent = ascent;
	return width;
}


static int moveto_x, moveto_y;
void plotstring(const char *nom)
{
  fl_draw(nom, moveto_x, moveto_y);
}

void dir_moveto(double x,double y)
/* move to physical coord x,y */
{
	moveto_x = myrint(x);
//	moveto_y = myrint(y);
	moveto_x += page_x_offset;
	moveto_y = myrint(page_y_offset + physy - y);
}


void dir_lineto(double x,double y)
/* draw line from current pos to physical coord x,y */
{
  int yi = myrint(page_y_offset + physy - y);
  fl_line(moveto_x, moveto_y, myrint(page_x_offset + x), yi);
}

void dir_plotsquare(double x, double y, int edge, int use_color)
{
  y += edge/2;
  int xi = (int)(page_x_offset + x + 0.5);
  int yi = (int)(page_y_offset + physy - (y + 0.5));
  if (use_color) fl_color(FL_RED);
  fl_rectf(xi, yi, edge, edge);
  if (use_color) fl_color(FL_BLACK);
}

//struct noeud *gracine;

const char *preptree(FD_nj_plot *fd_nj_plot)
{
int i, c, maxlname, v;
char *arbre, *finarbre;
char *last_bootstrap, *p, *q;
bool has_3_subtrees;

arbre = strdup(fd_nj_plot->current_tree);
/* lecture de l'arbre et de son label entre [] */
p = arbre; while(isspace(*p)) p++;
if(*p == '[') { 
	q = p;
	do q++; while(*q != ']');
	if(q > p + 1) {
		*q = 0;
		fd_nj_plot->tree_label = (char *)malloc(q - p);
		strcpy(fd_nj_plot->tree_label, p + 1);
		}
	p = q + 1;
	while(isspace(*p)) p++;
	}
fd_nj_plot->notu = 2; i = 3; v = 0;
if(*p == '(') {
	if(p > arbre) memmove(arbre, p, strlen(p) + 1);
	p = arbre + 1;
	while( (c=*(p++)) != 0 && c != ';') {
		if(c == ')') fd_nj_plot->notu++;
		if(c == '(') i++;
		if(c == ',') v++;
		}
	}
if(i != fd_nj_plot->notu) {
	free(arbre);
	fd_nj_plot->notu = 0;
	return bad_tree;
}
finarbre = nextpar(arbre);
if(finarbre == NULL) {
	fd_nj_plot->notu = 0;
	return ("Unbalanced parentheses in tree.");
	}
finarbre++;
/* memorize bootstrap value after last ) */
while(isspace(*finarbre)) finarbre++;
if(*finarbre != ';' && *finarbre != 0) {
	last_bootstrap = strdup(finarbre);
	p = strchr(last_bootstrap, ';');
	if(p != NULL) *p = 0;
	}
else last_bootstrap = NULL;

arbre = (char *)realloc(arbre, strlen(arbre) + 4 * v + 5 ); /* worst case add 4 chars for each , */
p = (char *)make_binary_or_unrooted_extra(arbre, &has_3_subtrees);
  if (has_3_subtrees && v + 2 == fd_nj_plot->notu) remove_parenth_no_comma(arbre);

if(p != NULL) {
	fd_nj_plot->notu = 0;
	free(arbre);
	return p;
	}
fd_nj_plot->long_arbre_parenth = strlen(arbre);
fd_nj_plot->notu = v + 1 ; /* after this fd_nj_plot->notu = number of OTUs  */
fd_nj_plot->totbranches= -1;
/* allocate all memory */
fd_nj_plot->tabtax = (struct noeud **)check_alloc(2*fd_nj_plot->notu - 1,sizeof(struct noeud *));
fd_nj_plot->branches = (branche *)check_alloc(fd_nj_plot->notu - 1, sizeof(branche));
for(i=0; i<2*fd_nj_plot->notu - 1; i++) *(fd_nj_plot->tabtax+i)=
			(struct noeud *)check_alloc(1,s_noeud);
fd_nj_plot->noms = (struct nom *)check_alloc(5*(fd_nj_plot->notu-1)+1,sizeof(struct nom));
fd_nj_plot->points = (struct mon_point *)check_alloc(2*(fd_nj_plot->notu-1)+1,sizeof(struct mon_point));
fd_nj_plot->traits = (struct trait *)check_alloc(3*(fd_nj_plot->notu-1),sizeof(struct trait));
fd_nj_plot->labels = (char **)check_alloc(fd_nj_plot->notu, sizeof(char *));
fd_nj_plot->widnames = (int *)check_alloc(fd_nj_plot->notu, sizeof(int));
fd_nj_plot->profs = (double *)check_alloc(fd_nj_plot->notu, sizeof(double));
fd_nj_plot->br_length_txt = (char *)check_alloc(2*(fd_nj_plot->notu - 1),10);
p = (char *)loadphylip(fd_nj_plot, arbre, last_bootstrap);
free(arbre);
if(p != NULL) return p;
maxlname = 0; /* largeur max des noms des feuilles */

if(nextotu != fd_nj_plot->notu - 1) return bad_tree;

for(i = 0; i <= nextotu; i++) {
	c = strlen(fd_nj_plot->labels[i]);
	if(c > maxlname) maxlname = c;
	}
for(i = 0; i < 2*fd_nj_plot->notu - 1; i++) {
	fd_nj_plot->tabtax[i]->nom = (char *)check_alloc(maxlname + 2 + 1, 1); /*2=place pour ) */
	fd_nj_plot->tabtax[i]->rank = i;
	}
  
  if ( (outotu || inotu) && fd_nj_plot->rooted && fd_nj_plot->has_br_length ) { // with -outgroup or -ingroup option but rooted
    struct noeud *root = *(fd_nj_plot->tabtax+num_noeud); // unroot the rooted tree
    root->v1->v3 = root->v2;
    root->v1->l3 = root->l1 + root->l2;
    root->v2->v3 = root->v1;
    root->v2->l3 = root->l1 + root->l2;
    num_noeud--;
    fd_nj_plot->rooted = FALSE;
  }
	
if (!fd_nj_plot->rooted) {
	fd_nj_plot->racine = *(fd_nj_plot->tabtax+(++num_noeud));
  //gracine = fd_nj_plot->racine;
	if (num_noeud >= 2*fd_nj_plot->notu - 1) return bad_tree;
	if (fd_nj_plot->has_br_length) {
	  struct noeud *node = NULL;
	  if (outotu || inotu) { // with -outgroup or -ingroup option
	    for (i = 0; i < fd_nj_plot->notu; i++) {
	      strcpy(fd_nj_plot->tabtax[i]->nom, fd_nj_plot->labels[i]);
	    }
	    if (target3) { // with 3 outgroup labels
	      node = span_nodes(fd_nj_plot->tabtax[0], NULL); // search central node
	    }
	    else { // with 1 outgroup leaf
	      for (i = 0; i < fd_nj_plot->notu; i++) { // search node with outgroup leaf
		if (strcmp(fd_nj_plot->tabtax[i]->nom, target1) == 0) break;
	      }
	      if (i < fd_nj_plot->notu) node = fd_nj_plot->tabtax[i];
	    }
	    if (node) { // node = tree center or outgroup leaf
	      current_cote1 = node;
	      if (outotu) { // -outgroup option
		if (!target3 || !is_parent(node, node->v3, target3)) {
		  current_cote2 = node->v3;
		  }
		else if (!is_parent(node, node->v2, target3)) {
		  current_cote2 = node->v2;
		  }
		else {
		  current_cote2 = node->v1;
		  }
		if (target3) {
		  struct noeud *aux = current_cote1; // have target3 at bottom of plot
		  current_cote1 = current_cote2;
		  current_cote2 = aux;
		}
	      }
	      else { // -ingroup option
		if (!is_parent(node, node->v3, target1)) {
		  current_cote2 = node->v3;
		  }
		else if (!is_parent(node, node->v2, target1)) {
		  current_cote2 = node->v2;
		  }
		else {
		  current_cote2 = node->v1;
		  }
	      }
	    }
	  }
	  fd_nj_plot->root_br_l = place_midpoint_root(fd_nj_plot->tabtax[0], fd_nj_plot->racine, fd_nj_plot->notu, node != NULL);
	  fd_nj_plot->rooted = TRUE;
	  fd_nj_plot->root_num = -1;//means the tree is rooted at fd_nj_plot->racine
	}
	else	{
/* derniere espece est groupe externe */
		fd_nj_plot->racine->v3 = NULL;
		fd_nj_plot->root_num = fd_nj_plot->notu - 1; 
		}
	}
else	{
	fd_nj_plot->racine = *(fd_nj_plot->tabtax+num_noeud);
	fd_nj_plot->root_br_l= fd_nj_plot->racine->l1 + fd_nj_plot->racine->l2;
	fd_nj_plot->root_num = num_noeud;
	if(!fd_nj_plot->has_br_length) calc_brl_for_lengthless(fd_nj_plot->racine, NULL);

/* y a-t-il un bootstrap sur l'une des branches racine ? */
	i = get_br_from_bouts(fd_nj_plot, fd_nj_plot->racine, fd_nj_plot->racine->v1); 
	if(i == -1) i = get_br_from_bouts(fd_nj_plot, fd_nj_plot->racine, fd_nj_plot->racine->v2);
  // build the exclusive OR of 2 conditions
	int cond1 = (i != -1 ? 1 : 0);
	int cond2 = 0;
	int j = get_br_from_bouts(fd_nj_plot, fd_nj_plot->racine, NULL);
	if (j != -1) { cond2 = 1; if (!cond1) i = j; }
	if (cond1 ^ cond2) {
		fd_nj_plot->branches[i].bouta = fd_nj_plot->racine->v1;
		fd_nj_plot->branches[i].boutb = fd_nj_plot->racine->v2;
		}
	} 
if(fd_nj_plot->notu < 2) return ("Tree should contain at least 2 elements.");
fd_nj_plot->subtree_notu = fd_nj_plot->notu;
fd_nj_plot->subtree_center = NULL;
if (fd_nj_plot->up) fd_nj_plot->up->deactivate();
if (fd_nj_plot->full) fd_nj_plot->full->value(1);
  if(fd_nj_plot->view == NULL && fd_nj_plot->menu_file) (fd_nj_plot->menu_file->get_menu() + reorder_align_following_tree)->deactivate();
if (fd_nj_plot->new_outgroup) fd_nj_plot->new_outgroup->activate();
return NULL;
} /* end of preptree */


char *check_alloc(int nbrelt, int sizelt)
{
char *retval;
if( (retval=(char *)calloc(nbrelt,sizelt)) != NULL ) return retval;
fl_alert("ERROR: Not enough memory.");
exit(1);
}


const char *loadphylip(FD_nj_plot *fd_nj_plot, char *arbre, char *last_bootstrap)
//returns NULL iff OK
{
char *deba,*debb,*debc, *finarbre;
struct noeud *p1, *p2, *p3, *p;
branche *int_br_g, *int_br_d;

fd_nj_plot->has_br_length = 2;
fd_nj_plot->has_internal = FALSE;
/* ignore all stuff after last closing parenthesis 
(needed for fastDNAml output)
*/
finarbre= nextpar(arbre);
fd_nj_plot->rooted = 0;
deba=arbre+1;
debb=deba;
while(*debb != ',') {
	if(*debb == 0) return bad_tree;
	if(*debb == '(')debb=nextpar(debb);
	debb++;
	}
debb++;
debc=debb;
while(*debc != ',' && debc<finarbre) {
	if(*debc == '(')debc=nextpar(debc);
	debc++;
	}
if(*debc==',') {
/* the tree is unrooted <==> it has 3 subtrees at its bottommost level */
	debc++;
	}
else	{
/* the tree is rooted */
	debc=finarbre+1;
	fd_nj_plot->rooted = 1;
	}

  fd_nj_plot->input_was_rooted = fd_nj_plot->rooted;
num_noeud = fd_nj_plot->notu - 1;
nextotu = -1;
p1=unrootedset(fd_nj_plot, deba,debb-2,&int_br_g);
if(p1 != NULL) p2=unrootedset(fd_nj_plot, debb,debc-2,&int_br_d);
p = *(fd_nj_plot->tabtax+(++num_noeud));
if(p1==NULL || p2==NULL || num_noeud >= 2*fd_nj_plot->notu - 1) return bad_tree;
p->v1=p1; p1->v3=p; p->l1=p1->l3;
if(int_br_g!=NULL) { int_br_g->bouta=p; int_br_g->boutb=p1; }
p->v2=p2; p2->v3=p; p->l2=p2->l3;
if(int_br_d!=NULL) { int_br_d->bouta=p; int_br_d->boutb=p2; }
if(!fd_nj_plot->rooted) {
	p3=unrootedset(fd_nj_plot, debc,finarbre-1,&int_br_g);
	if(p3==NULL) return bad_tree;
	if(int_br_g!=NULL) { int_br_g->bouta=p; int_br_g->boutb=p3; }
	p->v3=p3; p3->v3=p; p->l3=p3->l3;
	}
else	{
	p->v3=NULL;
/* recherche d'un dernier label interne */
	debc=finarbre+1;
	while(*debc!=0 && *debc!=':' && *debc!='[') debc++;
	if(debc-finarbre>1) {
		int l=debc-finarbre-1;
		fd_nj_plot->has_internal = TRUE;
		fd_nj_plot->totbranches++;
		fd_nj_plot->branches[fd_nj_plot->totbranches].br_label=check_alloc(l+1,1);
		memcpy(fd_nj_plot->branches[fd_nj_plot->totbranches].br_label,finarbre+1,l);
		fd_nj_plot->branches[fd_nj_plot->totbranches].br_label[l]=0;
		fd_nj_plot->branches[fd_nj_plot->totbranches].bouta=p1;
		fd_nj_plot->branches[fd_nj_plot->totbranches].boutb=p2;
		}
	}
if(fd_nj_plot->rooted && last_bootstrap != NULL) {
/* attach last_bootstrap to branch racine <--> NULL */
		fd_nj_plot->totbranches++;
		fd_nj_plot->branches[fd_nj_plot->totbranches].br_label = strdup(last_bootstrap);
		fd_nj_plot->branches[fd_nj_plot->totbranches].bouta=p;
		fd_nj_plot->branches[fd_nj_plot->totbranches].boutb=NULL;
	}
if(last_bootstrap != NULL) free(last_bootstrap);
return NULL;
}


struct noeud *unrootedset(FD_nj_plot *fd_nj_plot, char *deb, char *fin, branche **p_int_br)
//returns NULL iff error
{
struct noeud *p, *pp;
char *virg, *ferme;
branche *int_br;
static int l;
static double brlength;

*p_int_br=NULL;
while(*deb==' ')deb++;
while(*fin==' ')fin--;
if(*deb != '(') { /* une feuille */
	virg = strchr(deb, ':');
	if(virg != NULL && virg < fin) {
		sscanf(virg+1, "%le", &brlength);
		fd_nj_plot->has_br_length=1;
		}
	else	{
		brlength = 1;
		fd_nj_plot->has_br_length=0;
		virg = fin + 1;
		}
	virg--;
	while(*deb==' ')deb++;
	l = virg-deb+1;
	fd_nj_plot->labels[ ++nextotu] = (char *)check_alloc(l + 1, 1);
	memcpy(fd_nj_plot->labels[nextotu], deb, l);
	fd_nj_plot->labels[nextotu][l] = 0;
	p = *(fd_nj_plot->tabtax + nextotu);
	p->l3 = brlength;
	p->v1 = p->v2 = p->v3 = NULL;
	return p;
	}
/* un noeud */
num_noeud++;
if(num_noeud >= 2*fd_nj_plot->notu - 1) return NULL;
p = *(fd_nj_plot->tabtax + num_noeud);
ferme =  nextpar(deb);
virg=deb + 1;
while(*virg != ',' && virg < fin) {
	if(*virg == '(') virg=nextpar(virg);
	virg++;
	}
if(virg>=ferme) return NULL;
pp = unrootedset(fd_nj_plot, deb + 1, virg - 1, &int_br);
if(pp == NULL) return NULL;
p->v1 = pp; pp->v3 = p; p->l1 = pp->l3;
if(int_br != NULL) { int_br->bouta = p; int_br->boutb = pp; }
pp = unrootedset(fd_nj_plot, virg + 1, ferme - 1, &int_br);
if(pp == NULL) return NULL;
p->v2 = pp; pp->v3 = p; p->l2 = pp->l3;
if(int_br != NULL) { int_br->bouta = p; int_br->boutb = pp; }
virg = strchr(ferme, ':');
if(virg != NULL && virg < fin) { /* traitement longueur */
//	if(fd_nj_plot->has_br_length == 0) return NULL;
	sscanf(virg+1, "%le", &brlength);
	fd_nj_plot->has_br_length=1;
	if(*fin == ']') { /* bootstrap entre [] apres longueurs */
		static char *q;
		q = fin - 1;
		while(q > virg && *q != '[') q--;
		if(*q == '[' && fin - q >= 2) {
			fd_nj_plot->has_internal = TRUE;
			fd_nj_plot->totbranches++;
			l = fin - q - 1;
			fd_nj_plot->branches[fd_nj_plot->totbranches].br_label =
				check_alloc(l+1,1);
			memcpy(fd_nj_plot->branches[fd_nj_plot->totbranches].br_label,q+1,l);
			fd_nj_plot->branches[fd_nj_plot->totbranches].br_label[l]=0;
			*p_int_br= &fd_nj_plot->branches[fd_nj_plot->totbranches];
			}
		}
	}
else	{
//	if(fd_nj_plot->has_br_length == 1) return NULL;
	brlength = 1;
	fd_nj_plot->has_br_length=0;
	virg = fin + 1;
	}
/* recherche bootstrap (internal label) */
while (ferme < virg-1 && *(ferme+1) == ' ') ferme++;
l=virg-ferme-1;
if(l>0) {
	fd_nj_plot->has_internal = TRUE;
	fd_nj_plot->totbranches++;
	fd_nj_plot->branches[fd_nj_plot->totbranches].br_label=
		check_alloc(l+1,1);
	memcpy(fd_nj_plot->branches[fd_nj_plot->totbranches].br_label,ferme+1,l);
	fd_nj_plot->branches[fd_nj_plot->totbranches].br_label[l]=0;
	*p_int_br= &fd_nj_plot->branches[fd_nj_plot->totbranches];
	}
p->l3 = brlength;
return p;
}


char *nextpar(char *pospar)
//returns NULL iff error
{
char *pos;
pos=pospar+1;
while(*pos != ')') {
	if(*pos == 0) return NULL;
	if(*pos == '(') pos=nextpar(pos);
	pos++;
	}
return pos;
}


const char *make_binary_or_unrooted_extra(char *arbre, bool *p_has_3_subtrees)
//returns NULL iff OK
{
  char *finarbre, *deba, *debb, *debc;
  int retval;
  
  finarbre= nextpar(arbre);
  *(finarbre + 1) = 0;
  deba=arbre+1;
  while (*deba == '(' && nextpar(deba) == finarbre - 1) { // remove extra enclosing parentheses
    *finarbre = 0;
    memmove(arbre, deba, finarbre - arbre);
    finarbre -= 2;
  }
  debb=deba;
  while(*debb != ',') {
    if(*debb == 0) return("Incorrect tree file");
    if(*debb == '(')debb=nextpar(debb);
    debb++;
  }
  debb++;
  debc=debb;
  while(*debc != ',' && debc<finarbre) {
    if(*debc == '(')debc=nextpar(debc);
    debc++;
  }
  if(*debc == ',') {
    /* the tree is unrooted <==> it has 3 subtrees or more at its bottommost level */
    retval = make_binary(arbre , debb, finarbre - 1, TRUE);
    if(retval != -1) retval = make_binary(arbre , deba, debb - 2, TRUE);
    if (p_has_3_subtrees) *p_has_3_subtrees = TRUE;
  }
  else {
    retval = make_binary(arbre , deba, finarbre - 1, TRUE);
    if (p_has_3_subtrees) *p_has_3_subtrees = FALSE;
  }
  return (retval >= 0 ? NULL : "Cannot process multibranched tree without branch lengths");
}


int make_binary(char *arbre, char *debut, char *fin, int go_down)
//returns -1 iff error
{
int virg, l, retval;
char *p, *q;

p = debut; virg = 0; retval = 0;
while(p < fin) {
	if(*p == ',') virg++;
	else if(*p == '(')	{
		q = nextpar(p);
		if(go_down) {
			l = make_binary(arbre, p+1, q-1, TRUE);
			fin += l;
			retval += l;
			p = q + l;
			}
		else p = q;
		}
	p++;
	}
if(virg > 1) { /* multifurcation */
	/* recherche de la 2eme virgule */
	p = debut; l = 0;
	while(TRUE) {
		if(*p == ',') {
			l++;
			if(l == 2) break;
			}
		else if(*p == '(')	{
			p = nextpar(p);
			}
		p++;
		}
	l = strlen(p);
	memmove(p + 4, p, l + 1);
	memmove(debut + 1, debut, p - debut);
	*debut = '(';
	memcpy(p + 1, "):0", 3);
	fin += 4;
	retval += 4;
	if(virg > 2) retval += make_binary(arbre, debut, fin, FALSE);  
	}
return retval;
}


void mydrawstring(double x, double y, char *nom, char option, int height)
{
static double px,py;

ch_echelle(x,y,&px,&py);
if(option == '1') {
/* ecrire une chaine en la montant d'un chouia */
	py += height/6.;
	}
else if(option == 'c' || option == 'r')	{
/* ecrire une chaine en la centrant vert. sur cette position*/
	if (*nom == ')' || *nom == '(') {//indicates we want to draw a square before the name
		int edge = height/2;
		dir_plotsquare(px, py, edge, *nom == '(');
		nom++;
		while(*nom==' ')nom++;
		if(*nom == 0) return;
		px += edge;
		}
	py -= height/3.;
	px += height/6.;
	}
else if(option == 't')	{
	py -=  (5./6.) * height;
	}
else if(option == 'b')	{
	py += height/6.;
	}
dir_moveto(px,py);
if(option == 'r') fl_color(FL_RED);
plotstring(nom);
if(option == 'r') fl_color(FL_BLACK);
}


void moveto(double x,double y)
/* move from current pos to logical coord x,y */
{
static double px,py;
ch_echelle(x,y,&px,&py);
dir_moveto(px,py);
}


void lineto(double x,double y)
/* draw line from current pos to logical coord x,y */
{
static double px,py;
ch_echelle(x,y,&px,&py);
dir_lineto(px,py);
}


int calc_brl_for_lengthless(struct noeud *centre, struct noeud *pere)
/* Recursively computes branch lengths of a lengthless tree having some branches fixed to 0 to allow
 multifurcations so that all tips align to the right of the plot.
 */
{
	int n1 = 0, n2, depth;
	volatile double l;
	if(centre->v1 == NULL && centre->v2 == NULL) {
	  if(centre->l3 == 0) {//no terminal branch of 0 length
		centre->l3 = 1E-6;
		if(centre->v3->v1 == centre) centre->v3->l1 = centre->l3;
		else centre->v3->l2 = centre->l3;
		}
	  return 0;//a leaf
	  }
	//rearrange with centre->v3 towards root
	if(centre->v1 == pere) {
		centre->v1 = centre->v3;
		centre->v3 = pere;
		l = centre->l3;
		centre->l3 = centre->l1;
		centre->l1 = l;
	}
	else if(centre->v2 == pere) {
		centre->v2 = centre->v3;
		centre->v3 = pere;
		l = centre->l3;
		centre->l3 = centre->l2;
		centre->l2 = l;
	}
	n1 = calc_brl_for_lengthless(centre->v1, centre);
	n2 = calc_brl_for_lengthless(centre->v2, centre);
	depth = n1;
	if(centre->l1 != 0) depth++;
	if(depth < n2) depth = n2;
	if(centre->l2 != 0 && n2 + 1 > depth) depth++;
	if(centre->l1 != 0) {
		centre->l1 = depth - n1; 
		centre->v1->l3 = depth - n1;
		}
	else if(depth - n1 > 0) add_value_downstream(centre->v1, depth - n1);
	if(centre->l2 != 0) {
		centre->l2 = depth - n2; 
		centre->v2->l3 = depth - n2;
		}
	else if(depth - n2 > 0) add_value_downstream(centre->v2, depth - n2);
	return depth;
}

void add_value_downstream(struct noeud *centre, int value)
{
  if(centre->l1 != 0) {
	  centre->l1 += value;
	  centre->v1->l3 = centre->l1;
  }
  else add_value_downstream(centre->v1, value);
  if(centre->l2 != 0) {
	  centre->l2 += value;
	  centre->v2->l3 = centre->l1;
  }
  else add_value_downstream(centre->v2, value);
}


double place_midpoint_root(struct noeud *from, struct noeud *racine, int notu, int fixed_outgroup)
/* to root an unrooted tree in the middle of its largest tip-to-tip path 
 from: any leave or node of tree
 racine: unused node that will become the tree root
 notu: number of tree leaves
 fixed_outgroup: if true, current_cote1, current_cote2 have been previously set to two adjacent nodes where the root is wanted
 Returns: length of root-containing branch
 Requires that node->rank values have been correctly set before
*/
{
struct noeud *aux;
double laux, balance, root_br_length;
  
  if (!fixed_outgroup) {
    balance = midpoint_rooting(from, notu, &root_br_length);
  }
  else {
    double l1 = get_length_down(current_cote2, current_cote1);
    double l2 = get_length_down(current_cote1, current_cote2);
    if (current_cote2->v1 == current_cote1 )
      root_br_length = current_cote2->l1;
    else if (current_cote2->v2 == current_cote1)
      root_br_length = current_cote2->l2;
    else
      root_br_length = current_cote2->l3;
    balance = (root_br_length - l1 + l2)/(2*root_br_length);
    }
/* il faut toujours que la racine soit telle que racine->v1->v3=racine */
if (current_cote1->v1 == current_cote2 ) {
/* echanger les voisins v1 et v3 de cote1 */
	aux=current_cote1->v1;
	current_cote1->v1=current_cote1->v3;
	current_cote1->v3=aux;
	laux=current_cote1->l1;
	current_cote1->l1=current_cote1->l3;
	current_cote1->l3=laux;
	}
else if (current_cote1->v2 == current_cote2) {
/* echanger les voisins v2 et v3 de cote1 */
	aux=current_cote1->v2;
	current_cote1->v2=current_cote1->v3;
	current_cote1->v3=aux;
	laux=current_cote1->l2;
	current_cote1->l2=current_cote1->l3;
	current_cote1->l3=laux;
	}
current_cote1->v3 = racine;

if (current_cote2->v1 == current_cote1 )
	current_cote2->v1 = racine;
else if (current_cote2->v2 == current_cote1)
	current_cote2->v2 = racine;
else
	current_cote2->v3 = racine;
racine->v1=current_cote1;
racine->v2=current_cote2;
racine->v3=NULL;
racine->l3=0;
/* avoid very unbalanced division of root branch */
if(balance > MAX_FRAC) balance = MAX_FRAC;
else if(balance < 1 - MAX_FRAC) balance = 1 - MAX_FRAC;
racine->l1 = root_br_length * balance;
  racine->v1->l3 = racine->l1;
racine->l2 = root_br_length - racine->l1;
  if (current_cote2->v1 == racine )
    current_cote2->l1 = racine->l2;
  else if (current_cote2->v2 == racine)
    current_cote2->l2 = racine->l2;
  else
    current_cote2->l3 = racine->l2;
  return root_br_length;
}


double get_length_down(struct noeud *pere, struct noeud *racine)
/* compute the max length of the tree down a node */
{
if (racine == NULL) return 0.0;
else	{
	struct noeud *gauche, *droite;
	double bg,bd,lg,ld;
	if ( racine->v1 == pere ) {
		gauche = racine->v2; droite = racine->v3;
		bg = (racine->l2); bd = (racine->l3);
		}
	else if ( racine->v2 == pere ) {
		gauche = racine->v1; droite = racine->v3;
		bg = (racine->l1); bd = (racine->l3);
		}
	else	{
		gauche = racine->v1; droite = racine->v2;
		bg = (racine->l1); bd = (racine->l2);
		}
 	/* conserver cette ecriture sinon plante sur PC */
 	lg = ld = 0;
 	if (gauche != NULL) lg = get_length_down(racine, gauche);
 	lg += bg;
 	if (droite != NULL) ld = get_length_down(racine, droite);
 	ld += bd;
  return lg > ld ? lg : ld;
	}
}


void clear_squares_below(struct noeud *n)
{
	if(n == NULL) return;
	clear_squares_below(n->v1);
	clear_squares_below(n->v2);
	char *p = n->nom;
	if(p != NULL && *p == ')') memmove(p, p+1, strlen(p));
}


void runtree(FD_nj_plot *fd_nj_plot)
{
double currx, curry;
int i;
struct noeud *p1, *p2;
if (!fd_nj_plot->rooted) {
	/* place root at user-chosen place: node # fd_nj_plot->root_num */
	fd_nj_plot->rooted = TRUE;
	p1 = fd_nj_plot->tabtax[fd_nj_plot->root_num];
	p2 = p1->v3;
	fd_nj_plot->root_br_l = p1->l3;
	if(fd_nj_plot->has_br_length) {
	  current_cote1 = p1;
	  current_cote2 = p2;
	  place_midpoint_root(p1, fd_nj_plot->racine, fd_nj_plot->notu, true);
		}
	else {
	      p1->v3 = fd_nj_plot->racine;
	      if (p2->v1 == p1 )
		      p2->v1 = fd_nj_plot->racine;
	      else if (p2->v2 == p1)
		      p2->v2 = fd_nj_plot->racine;
	      else
		      p2->v3 = fd_nj_plot->racine;
	      fd_nj_plot->racine->v1=p1;
	      fd_nj_plot->racine->v2=p2;
	      fd_nj_plot->racine->v3=NULL;
		fd_nj_plot->racine->l1 = p1->l3;
		if (p2->v1 == fd_nj_plot->racine )
			fd_nj_plot->racine->l2 = p2->l1;
		else if (p2->v2 == fd_nj_plot->racine)
			fd_nj_plot->racine->l2 = p2->l2;
		else
			fd_nj_plot->racine->l2 = p2->l3;
		calc_brl_for_lengthless(fd_nj_plot->racine, NULL);
		}
	}
/* initialize leave and node names */
for(i = 0; i <= 2*(fd_nj_plot->notu - 1); i++) fd_nj_plot->tabtax[i]->nom[0] = 0;
if (fd_nj_plot->choix == depl_racine) {
	for(i=0; i <= 2*(fd_nj_plot->notu - 1) - 1; i++) {
		if(i == fd_nj_plot->root_num) continue;//skip current root
		if( !fd_nj_plot->has_br_length) {
		   if(fd_nj_plot->tabtax[i]->l3 == 0) continue;//skip internal multifurcation nodes that can't be broken by root
			}
		sprintf(fd_nj_plot->tabtax[i]->nom,")");
		}
	}
else if(fd_nj_plot->choix == permutation)
	for(i=fd_nj_plot->notu; i <= 2*(fd_nj_plot->notu - 1); i++) sprintf(fd_nj_plot->tabtax[i]->nom,")");
else if(fd_nj_plot->choix == subtree)
	for(i=fd_nj_plot->notu; i <= 2*(fd_nj_plot->notu - 1) - 1; i++) sprintf(fd_nj_plot->tabtax[i]->nom,")");
else if(fd_nj_plot->choix == selection) {
  int j;
  SEA_VIEW *view = fd_nj_plot->view;
  if (view) {
    for(i=fd_nj_plot->notu; i <= 2*(fd_nj_plot->notu - 1); i++) sprintf(fd_nj_plot->tabtax[i]->nom,")");
    for (i=0; i < fd_nj_plot->notu; i++) {
      for (j=0; j < view->tot_seqs; j++) {
	if (view->sel_seqs[j] && strcmp(fd_nj_plot->labels[i], view->seqname[j]) == 0) break;
	}
      // ) means draw black square, ( means draw red square
      sprintf(fd_nj_plot->tabtax[i]->nom, j >= view->tot_seqs ? ")" : "(");
      }
    }
  }
else if(fd_nj_plot->choix == prune_clade)
	for(i=0; i <= 2*(fd_nj_plot->notu - 1) - 1; i++) sprintf(fd_nj_plot->tabtax[i]->nom,")");
else if(fd_nj_plot->choix == prune_clade_found) {
	char *p;
	struct noeud *parent;
	for(i=0; i <= 2*(fd_nj_plot->notu - 1) - 1; i++) sprintf(fd_nj_plot->tabtax[i]->nom,")");
	parent = fd_nj_plot->prune_clade_node->v3;
	parent->nom[0] = 0;//no square on clade's parent
	if(parent->v1 == fd_nj_plot->prune_clade_node) p = parent->v2->nom;//nor on clade's sister
	else p = parent->v1->nom;
	if(*p != 0) memmove(p, p + 1, strlen(p));
	clear_squares_below(fd_nj_plot->prune_clade_node);//nor in clade
	}

if (fd_nj_plot->leaf_trimmer) fd_nj_plot->leaf_trimmer->process();
fd_nj_plot->totnoms = fd_nj_plot->tottraits = fd_nj_plot->totpoints = -1;
end_br_length = fd_nj_plot->br_length_txt;
currx = 0.;
maxx = 0.;
memset(fd_nj_plot->profs, 0, (fd_nj_plot->notu)*sizeof(double));
if(fd_nj_plot->subtree_center != NULL)
	fd_nj_plot->deltay = maxy / (fd_nj_plot->subtree_notu - 1);
else {
	fd_nj_plot->deltay = maxy / (fd_nj_plot->notu - 1);
	fd_nj_plot->subtree_notu = fd_nj_plot->notu;
	}
nexty = -fd_nj_plot->deltay;
calc_text_size((char *)"Mq", &fd_nj_plot->char_height, &fd_nj_plot->ascent);
if(fd_nj_plot->subtree_center == NULL)
	mem_plot(fd_nj_plot, NULL, fd_nj_plot->racine, currx, &curry);
else
	mem_plot(fd_nj_plot, fd_nj_plot->subtree_ascend, fd_nj_plot->subtree_center, currx, &curry);
} /* end of runtree */


void mem_plot(FD_nj_plot *fd_nj_plot, struct noeud *pere, struct noeud *centre, double currx,
double *curry)
/* prepare tree by memorizing all graphic requests */
{
  int num=0, widthd = 0, widthg = 0;
char *p;
struct noeud *gauche, *droite;
double bg, bd, bpere;

if(fd_nj_plot->choix == prune_clade_found && centre == fd_nj_plot->prune_clade_node) {
	fd_nj_plot->colored_names_1 = fd_nj_plot->totnoms + 1;
	fd_nj_plot->colored_traits_1 = fd_nj_plot->tottraits + 1;
	}

while( *(fd_nj_plot->tabtax+num) != centre) num++;
if(pere != NULL && 
	(centre->v1 == NULL || centre->v2 == NULL || centre->v3 == NULL) ) {
	/* orienter le noeud centre de maniere standard: centre->v3=pere */
	if( centre->v1 == pere ) {
		gauche =centre->v2; droite = centre->v3;
		bg = centre->l2; bd = centre->l3; bpere = centre->l1;
		}
	else if( centre->v2 == pere ) {
		gauche =centre->v1; droite = centre->v3;
		bg = centre->l1; bd = centre->l3; bpere = centre->l2;
		}
	else	{
		gauche =centre->v1; droite = centre->v2;
		bg = centre->l1; bd = centre->l2; bpere = centre->l3;
		}
	centre->v3=pere; centre->v1=gauche; centre->v2=droite;
	centre->l3=bpere; centre->l1=bg; centre->l2=bd;
	/* write taxon name */
	nexty += fd_nj_plot->deltay;
	mem_nom(fd_nj_plot, currx,nexty,fd_nj_plot->tabtax[num]->nom,'c');
	fd_nj_plot->profs[num] = currx;
	fd_nj_plot->widnames[num] = calc_text_size(fd_nj_plot->tabtax[num]->nom, NULL, NULL);
	if(fd_nj_plot->choix==depl_racine || fd_nj_plot->choix== selection || fd_nj_plot->choix == prune_clade || fd_nj_plot->choix == prune_clade_found) 
		mem_point(fd_nj_plot, currx, nexty, num);
	*curry = nexty;
	}
else 	{
	double yg, yd, xg, xd;

	static int doswap;
	static struct noeud *tmp;
	/* doswap vrai ssi permutation de 2 descendants necessaire ici */
	doswap = (fd_nj_plot->swap != 0 && *(fd_nj_plot->tabtax+fd_nj_plot->swap) == centre);
	if( centre->v1 == pere ) {
		if(doswap) {
			tmp= centre->v2;centre->v2= centre->v3;centre->v3= tmp;
			bg= centre->l2; centre->l2= centre->l3; centre->l3= bg;
			}
		gauche =centre->v2; droite = centre->v3;
		bg = centre->l2; bd = centre->l3; bpere = centre->l1;
		}
	else if( centre->v2 == pere ) {
		if(doswap) {
			tmp= centre->v1;centre->v1= centre->v3;centre->v3= tmp;
			bg= centre->l1; centre->l1= centre->l3; centre->l3= bg;
			}
		gauche =centre->v1; droite = centre->v3;
		bg = centre->l1; bd = centre->l3; bpere = centre->l2;
		}
	else	{
		if(doswap) {
			tmp= centre->v1;centre->v1= centre->v2;centre->v2= tmp;
			bg= centre->l1; centre->l1= centre->l2; centre->l2= bg;
			}
		gauche =centre->v1; droite = centre->v2;
		bg = centre->l1; bd = centre->l2; bpere = centre->l3;
		}
	/* orienter le noeud centre de maniere standard: centre->v3=pere */
	centre->v3=pere; centre->v1=gauche; centre->v2=droite;
	centre->l3=bpere; centre->l1=bg; centre->l2=bd;
	xg=currx+bg; xd=currx+bd;

	mem_plot(fd_nj_plot, centre, gauche, xg, &yg);
	mem_plot(fd_nj_plot, centre, droite, xd, &yd);

  if (fd_nj_plot->branch_width_as_support) {
    float f = 0;
    char *p = get_br_label(fd_nj_plot, pere?centre:droite, gauche);
    if (p) sscanf(p, "%f", &f);
    if (f > fd_nj_plot->support_threshold_high) widthg = 5;
    else if (f > fd_nj_plot->support_threshold_low) widthg = 3;
    else widthg = 0;
    p = get_br_label(fd_nj_plot, pere?centre:gauche, droite);
    f = 0;
    if (p) sscanf(p, "%f", &f);
    if (f > fd_nj_plot->support_threshold_high) widthd = 5;
    else if (f > fd_nj_plot->support_threshold_low) widthd = 3;
    else widthd = 0;
  }
  else {
    widthd = widthg = 0;
    }
  mem_trait(fd_nj_plot, currx, yg, xg, yg, widthg);
  mem_trait(fd_nj_plot, currx, yd, xd, yd, widthd);
	mem_trait(fd_nj_plot, currx, yg, currx, yd, 0); // vertical
	*curry = (yg+yd)/2;

/* write internal fd_nj_plot->labels */
	if(fd_nj_plot->choix==show_tree && fd_nj_plot->show_bootstrap) {
		if(pere != NULL) { /* for all but root node */
			if( (p=get_br_label_with_threshold(fd_nj_plot, centre, gauche)) != NULL ) mem_nom(fd_nj_plot, currx,yg,p,'t');
			if( (p=get_br_label_with_threshold(fd_nj_plot, centre, droite)) != NULL ) mem_nom(fd_nj_plot, currx,yd,p,'b');
			}
		else {/* for root node */
			if( (p=get_br_label_with_threshold(fd_nj_plot, centre, NULL)) != NULL ) { /* if root bootstrap exists */
				mem_nom(fd_nj_plot, currx,*curry,p,'c');
				if( (p=get_br_label_with_threshold(fd_nj_plot, centre, gauche)) != NULL ) mem_nom(fd_nj_plot, currx,yg,p,'t');
				if( (p=get_br_label_with_threshold(fd_nj_plot, centre, droite)) != NULL ) mem_nom(fd_nj_plot, currx,yd,p,'b');
				}
			else {/* if no root bootstrap any bootstrap is to be centered */
				if( (p=get_br_label_with_threshold(fd_nj_plot, centre, gauche)) != NULL ) mem_nom(fd_nj_plot, currx,*curry,p,'c');
				if( (p=get_br_label_with_threshold(fd_nj_plot, centre, droite)) != NULL ) mem_nom(fd_nj_plot, currx,*curry,p,'c');
				if( (p=get_br_label_with_threshold(fd_nj_plot, gauche, droite)) != NULL ) mem_nom(fd_nj_plot, currx,*curry,p,'c');
				}
			}
		}

/* write node number */
	mem_nom(fd_nj_plot, currx,*curry,fd_nj_plot->tabtax[num]->nom,'c');
	mem_point(fd_nj_plot, currx, *curry, num);
	if(fd_nj_plot->plot_br_l && fd_nj_plot->has_br_length) {
/* write branch length */
		double min_br_l = 0.008; /* minimum branch length displayed */
		if(bg>min_br_l) {
			sprintf(end_br_length,"%.3f",bg);
			mem_nom(fd_nj_plot, currx+bg/10,yg,end_br_length,'1');
			end_br_length += (strlen(end_br_length)+1);
			}
		if(bd>min_br_l) {
			sprintf(end_br_length,"%.3f",bd);
			mem_nom(fd_nj_plot, currx+bd/10,yd,end_br_length,'1');
			end_br_length += (strlen(end_br_length)+1);
			}
		}
	}
if(fd_nj_plot->choix == prune_clade_found && centre == fd_nj_plot->prune_clade_node) {
	fd_nj_plot->colored_names_2 = fd_nj_plot->totnoms;
	fd_nj_plot->colored_traits_2 = fd_nj_plot->tottraits;
	}
}  /* end of mem_plot */

void mem_point(FD_nj_plot *fd_nj_plot, double x, double y, int number)
{
if( *(fd_nj_plot->tabtax[number]->nom) != ')' &&  *(fd_nj_plot->tabtax[number]->nom) != '(') return;
++fd_nj_plot->totpoints;
(fd_nj_plot->points+fd_nj_plot->totpoints)->x = x;
(fd_nj_plot->points+fd_nj_plot->totpoints)->y = y;
(fd_nj_plot->points+fd_nj_plot->totpoints)->number = number;
}

void mem_nom(FD_nj_plot *fd_nj_plot, double x, double y, char *nom, char option)
/* x,y: logical coordinates of beginning of string 
   nom: address of string to be displayed later
   option: 'c' use for position of center of character height
           '1' put bottom of characters at y+1 pixel
	   't' use for position of top of character
	   'b' use for position of bottom of character
*/
{
if(strlen(nom) != 0) {
	fd_nj_plot->totnoms++;
	if(x > maxx) maxx = x;
	(fd_nj_plot->noms+fd_nj_plot->totnoms)->x = x;
	(fd_nj_plot->noms+fd_nj_plot->totnoms)->y = y;
	(fd_nj_plot->noms+fd_nj_plot->totnoms)->nom = nom;
	(fd_nj_plot->noms+fd_nj_plot->totnoms)->disp_option = option;
	}
}


void mem_trait(FD_nj_plot *fd_nj_plot, double xd, double yd, double xf, double yf, int width)
{
fd_nj_plot->tottraits++;
if(xd>maxx) maxx=xd;
if(xf>maxx) maxx=xf;
(fd_nj_plot->traits+fd_nj_plot->tottraits)->xd = xd;
(fd_nj_plot->traits+fd_nj_plot->tottraits)->yd = yd;
(fd_nj_plot->traits+fd_nj_plot->tottraits)->xf = xf;
(fd_nj_plot->traits+fd_nj_plot->tottraits)->yf = yf;
(fd_nj_plot->traits+fd_nj_plot->tottraits)->width = width;
}


char *get_br_label(FD_nj_plot *fd_nj_plot, struct noeud *a, struct noeud *b)
{
int i;
for(i=0; i<=fd_nj_plot->totbranches; i++) {
	if(fd_nj_plot->branches[i].bouta==a && fd_nj_plot->branches[i].boutb==b) {
		return fd_nj_plot->branches[i].br_label;
		}
	else if(fd_nj_plot->branches[i].boutb==a && fd_nj_plot->branches[i].bouta==b) {
		return fd_nj_plot->branches[i].br_label;
		}
	}
return NULL;
}

char *get_br_label_with_threshold(FD_nj_plot *fd_nj_plot, struct noeud *a, struct noeud *b)
{
	float value;
	char *label = get_br_label(fd_nj_plot, a, b);
	if(label != NULL && fd_nj_plot->bootstrap_threshold > 0.01) {
		sscanf(label, "%f", &value);
		if(value < fd_nj_plot->bootstrap_threshold) label = NULL;
	}
	return label;
}


int get_br_from_bouts(FD_nj_plot *fd_nj_plot, struct noeud *a, struct noeud *b)
{
int i;
for(i=0; i<=fd_nj_plot->totbranches; i++) {
	if(fd_nj_plot->branches[i].bouta==a && fd_nj_plot->branches[i].boutb==b) 
		return i;
	if(fd_nj_plot->branches[i].boutb==a && fd_nj_plot->branches[i].bouta==b) 
		return i;
	}
return -1;
}


void free_tree(FD_nj_plot *fd_nj_plot)
{
	int i;
	if(fd_nj_plot->notu == 0) return;
	/* de-allocate all memory */
	for(i=0; i<2*fd_nj_plot->notu - 1; i++) {
		free(fd_nj_plot->tabtax[i]->nom);
		free(fd_nj_plot->tabtax[i]);
		}
	free(fd_nj_plot->tabtax);
	for(i=0; i<fd_nj_plot->notu - 1; i++)
		if(fd_nj_plot->branches[i].br_label != NULL) free(fd_nj_plot->branches[i].br_label);
	free(fd_nj_plot->branches);
	free(fd_nj_plot->widnames);
	free(fd_nj_plot->noms);
	free(fd_nj_plot->points);
	free(fd_nj_plot->traits);
	free(fd_nj_plot->profs);
	for(i = 0; i < fd_nj_plot->notu; i++) free(fd_nj_plot->labels[i]);
	free(fd_nj_plot->labels);
	free(fd_nj_plot->br_length_txt);
	if(fd_nj_plot->tree_name != NULL) {
	  free(fd_nj_plot->tree_name);
	  fd_nj_plot->tree_name = NULL;
	  }
	if(fd_nj_plot->trees != NULL) {
	  free(fd_nj_plot->trees);
	  fd_nj_plot->trees = NULL;
	  }
	if(fd_nj_plot->tree_label != NULL) {
	  free(fd_nj_plot->tree_label);
	  fd_nj_plot->tree_label = NULL;
	  }
}


/* ecriture d'un arbre non racine au format phylip, multifurcations allowed */
char *ecrit_arbre_parenth_unrooted(FD_nj_plot *fd_nj_plot, struct noeud *root)
{
struct noeud *t1, *t2, *t3;
char *p1, *p2, *p3, *p, *bootstrap;
int l;
float l1, l2, l3;	

if(root == NULL) return NULL;
t1 = root->v1->v1; t2 = root->v1->v2; t3 = root->v2;
l1 = root->v1->l1; l2 = root->v1->l2; 
if(t1 == NULL) {
	t1 = root->v2->v1; t2 = root->v2->v2; t3 = root->v1;
	l1 = root->v2->l1; l2 = root->v2->l2; 
	}
l3 = root->l1 + root->l2;
bootstrap = get_br_label(fd_nj_plot, root->v1, root->v2);
if(bootstrap == NULL) bootstrap = (char *)"";

p1 = ecrit_arbre_parenth(fd_nj_plot, t1);  *(p1 + strlen(p1) - 1) = 0;
p2 = ecrit_arbre_parenth(fd_nj_plot, t2);  *(p2 + strlen(p2) - 1) = 0;
p3 = ecrit_arbre_parenth(fd_nj_plot, t3);  *(p3 + strlen(p3) - 1) = 0;
l = strlen(p1) + strlen(p2)+ strlen(p3) + 150;
p = (char *)check_alloc(l, 1);
if(fd_nj_plot->has_br_length) sprintf(p, "(%s:%g,%s:%g,%s%s:%g);", p1, l1, p2, l2, p3, bootstrap, l3);
else sprintf(p, "(%s,%s,%s%s);", p1, p2, p3, bootstrap);
free(p1); free(p2); free(p3);
return p;
}


/* ecriture d'un arbre racine au format phylip, multifurcations allowed */
char *ecrit_arbre_parenth(FD_nj_plot *fd_nj_plot, struct noeud *root)
{
char *arbre, *fin, *p;
int l, maxarbre = 2 * fd_nj_plot->long_arbre_parenth + 1000;

arbre=check_alloc( maxarbre+20,1);
fin=recur_ecrit_arbre(fd_nj_plot, root,arbre,arbre+maxarbre-1);
/* ecriture du dernier label interne */
if( fin != NULL && (p=get_br_label(fd_nj_plot, root->v1,root->v2)) != NULL ) {
	l=strlen(p);
	if(fin+l>=arbre+maxarbre) fin= NULL;
	else	{
		memcpy(fin+1,p,l);
		fin+=l;
		}
	}
if(fin == NULL) {
	free(arbre);
	return NULL;
	}
strcpy(fin+1,";");
arbre = (char *)realloc(arbre, strlen(arbre) + 1);
return arbre;
}


char *recur_ecrit_arbre(FD_nj_plot *fd_nj_plot, struct noeud *centre, char *arbre, char *finarbre)
{
int l;
char *p, *q;

if(centre->v1==NULL && centre->v2==NULL) {
        p = fd_nj_plot->labels[centre->rank];
	if(*p == ')' || *p == '(') p++;//skip square mark
	l = strlen(p);
	if(arbre+l>=finarbre) return NULL;	
	memcpy(arbre,p,l);
	arbre += l-1;
	}
else	{
	*arbre='(';
	p = arbre;
	arbre=recur_ecrit_arbre(fd_nj_plot, centre->v1,arbre+1,finarbre);
	if(arbre==NULL) return NULL;
	if(fd_nj_plot->has_br_length) {
		if(arbre+10>=finarbre) return NULL;
		sprintf(++arbre,":%g",centre->l1);
		while(*arbre!=0) arbre++;
		}
	else arbre++;
	*arbre=',';
	arbre=recur_ecrit_arbre(fd_nj_plot, centre->v2,arbre+1,finarbre);
	if(arbre==NULL) return NULL;
	if(fd_nj_plot->has_br_length) {
		if(arbre+10>=finarbre) return NULL;
		sprintf(++arbre,":%g",centre->l2);
		while(*arbre!=0) arbre++;
		}
	else arbre++;
	*arbre=')';
	/* ecriture des fd_nj_plot->labels internes */
	if( (q=get_br_label(fd_nj_plot, centre,centre->v3) ) != NULL  && (fd_nj_plot->has_br_length || (centre->l3 != 0) ) ) {
		l=strlen(q);
		if(arbre+l>=finarbre) return NULL;
		memcpy(arbre+1,q,l);
		arbre+=l;
		}
	else if(centre->v3 != NULL && (!fd_nj_plot->has_br_length) && (centre->l3 == 0) ) {//multibranches processed here
		memmove(p, p + 1, arbre - p);
		arbre -= 2;
	}
}
return arbre;
}


void removeroot(FD_nj_plot *fd_nj_plot)
{
struct noeud *p1, *p2;
p1=fd_nj_plot->racine->v1;
p2=fd_nj_plot->racine->v2;
if(p1->v1 == fd_nj_plot->racine )
	{p1->v1 = p2; p1->l1 = fd_nj_plot->root_br_l;}
else if (p1->v2 == fd_nj_plot->racine)
	{p1->v2 = p2; p1->l2 = fd_nj_plot->root_br_l;}
else
	{p1->v3 = p2; p1->l3 = fd_nj_plot->root_br_l;}
if(p2->v1 == fd_nj_plot->racine )
	{p2->v1 = p1; p2->l1 = fd_nj_plot->root_br_l;}
else if (p2->v2 == fd_nj_plot->racine)
	{p2->v2 = p1; p2->l2 = fd_nj_plot->root_br_l;}
else
	{p2->v3 = p1; p2->l3 = fd_nj_plot->root_br_l;}
fd_nj_plot->rooted = 0;
}


int calc_n_desc(struct noeud *pere)
{
if(pere->v1 == NULL) return 1;
else return calc_n_desc(pere->v1) + calc_n_desc(pere->v2);
}


void disconnect_tree_windows(SEA_VIEW *view)
//disconnect all tree windows pointing to view
{
	Fl_Window *w = Fl::first_window();
	while(w != NULL) {
		const char *c = w->xclass();
		if(c != NULL && strcmp(c, TREE_WINDOW) == 0) {
			FD_nj_plot *fd_nj_plot = (FD_nj_plot *)(w->user_data());
			if(fd_nj_plot->view == view) fd_nj_plot->view = NULL;
			}
		w = Fl::next_window(w);
	}
}


int calc_new_order(int *intree, struct noeud *from, char **names, int tot_seqs, int found, char **pmissing)
//computes tree-based order of names
//upon return, intree[old] = new
//returns # of names found in array names, or 0 if any was not found
{
	if(from->v1 == NULL && from->v2 == NULL) {
		int i;
		for(i = 0; i < tot_seqs; i++) if(intree[i] == -1 && strcmp(names[i], from->nom) == 0) break;
		if(i < tot_seqs) {
			intree[i] = found++;
			return found;
			}
		else {
			*pmissing = from->nom;
			return 0;
			}
		}
	found = calc_new_order(intree, from->v2, names, tot_seqs, found, pmissing);
	if(found != 0) 	found = calc_new_order(intree, from->v1, names, tot_seqs, found, pmissing);
	return found;
}


void reorder_following_tree(struct noeud *root, int notu, SEA_VIEW *view)
{
	int i, j;
	char *missing_name;
	if(view == NULL || view->alt_col_rank != NULL) return;
	
	int *intree = (int *)malloc(view->tot_seqs * sizeof(int));
	for( i = 0; i < view->tot_seqs; i++) intree[i] = -1;
	int found = calc_new_order(intree, root, view->seqname, view->tot_seqs, 0, &missing_name);
	if(found != notu) { 
		fl_alert("Missing tree sequence from alignment: [%s]", missing_name);
		free(intree); 
		return; 
		}
	int *order = (int *)malloc(view->tot_seqs * sizeof(int));
	int *fromtree = (int *)malloc(notu * sizeof(int));
	for( j = 0, i = 0; i < view->tot_seqs; i++) {
		if(intree[i] != -1) fromtree[j++] = i;
		}
	for( i = 0; i < view->tot_seqs; i++) {
		if(intree[i] == -1) order[i] = i;
		else order[i] = fromtree[intree[i]];
		}
	free(fromtree); free(intree);
	char **tmp = (char **)malloc(view->tot_seqs * sizeof(char *));
	memcpy(tmp, view->sequence, view->tot_seqs * sizeof(char *));
	for( i = 0; i < view->tot_seqs; i++) view->sequence[order[i]] = tmp[i];
	if(view->comments != NULL) {
		memcpy(tmp, view->comments, view->tot_seqs * sizeof(char *));
		for( i = 0; i < view->tot_seqs; i++) view->comments[order[i]] = tmp[i];
		}
	memcpy(tmp, view->seqname, view->tot_seqs * sizeof(char *));
	for( i = 0; i < view->tot_seqs; i++) view->seqname[order[i]] = tmp[i];
	memcpy(tmp, view->col_rank, view->tot_seqs * sizeof(char *));
	for( i = 0; i < view->tot_seqs; i++) view->col_rank[order[i]] = tmp[i];
	if(view->viewasprots != NULL) {
		memcpy(tmp, view->viewasprots, view->tot_seqs * sizeof(char *));
		for( i = 0; i < view->tot_seqs; i++) ((char **)(view->viewasprots))[order[i]] = tmp[i];
		}
	free(tmp);
	int *tmpint = (int *)malloc(view->tot_seqs * sizeof(int));
	memcpy(tmpint, view->each_length, view->tot_seqs * sizeof(int));
	for( i = 0; i < view->tot_seqs; i++) view->each_length[order[i]] = tmpint[i];
	if(view->tot_sel_seqs != 0) {
		memcpy(tmpint, view->sel_seqs, view->tot_seqs * sizeof(int));
		for( i = 0; i < view->tot_seqs; i++) view->sel_seqs[order[i]] = tmpint[i];
		}
	for(int numset = 0; numset < view->numb_species_sets; numset++) {
		for(i = 0; i < view->tot_seqs; i++)
			tmpint[order[i]] = view->list_species_sets[numset][i];
		memcpy(view->list_species_sets[numset], tmpint, view->tot_seqs * sizeof(int) );
	}
	free(tmpint);
	if(!view->cursor_in_comment) {
		view->cursor_seq = order[view->cursor_seq - 1] + 1;
		view->old_cursor_seq = view->cursor_seq;
		}
	free(order);
	set_seaview_modified(view, TRUE);
	view->DNA_obj->redraw();
	view->DNA_obj->take_focus();
}

static void search_callback(Fl_Widget *wgt, void *data)
{
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)wgt->user_data();
	static char select[500] = "";
	char aux[500];
	int num, trouve;
	
	if(fd_nj_plot->notu == 0) return;
	if(strcmp(((Fl_Menu_ *)wgt)->text(), "Find") == 0) {
		const char *p = fl_input("Enter searched name:", select);
		if(p == NULL) return;
		strcpy(select, p);
		majuscules(select);
	}
	if(strlen(select) == 0) return;
	if(fd_nj_plot->fd_unrooted != NULL) {
		unrooted_search((FD_unrooted *)fd_nj_plot->fd_unrooted, select);
		return;
		}
	trouve = FALSE;
	for(num = 0; num <= fd_nj_plot->totnoms; num++) {
		strcpy(aux, (fd_nj_plot->noms+num)->nom);
		majuscules(aux);
		if(strstr( aux, select) != NULL) {
			(fd_nj_plot->noms+num)->disp_option = 'r';
			trouve = TRUE;
		}
	}
	if(trouve) {
		fd_nj_plot->panel->window()->redraw();
	}
}

void bt_threshold_callback(Fl_Widget *wgt, void *data)
{
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)wgt->user_data();
  Fl_Menu_Item *item = fd_nj_plot->menu_edit->get_menu();
	const char *reply = fl_input("Enter branch support threshold value", NULL);
	if(reply == NULL) return;
  char *p = (char *)(item + 6)->label();
	if (strcmp(p, "0") != 0) free(p);
	(item + 6)->label(strdup(reply));
	sscanf(reply, "%f", &fd_nj_plot->bootstrap_threshold);
	fd_nj_plot->need_runtree = TRUE;
	fd_nj_plot->panel->window()->redraw();
}

void midpoint_callback(Fl_Widget *wgt, void *data)
{
	struct noeud *v1, *v2;
	double l, l1, l2;
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)(wgt ? wgt->user_data() : data);
	if(fd_nj_plot->notu == 0) return;
	v1 = fd_nj_plot->racine->v1; v2 = fd_nj_plot->racine->v2;
	l1 = fd_nj_plot->racine->l1; l2 = fd_nj_plot->racine->l2;
	if (fd_nj_plot->rooted) removeroot(fd_nj_plot);
	l = place_midpoint_root(fd_nj_plot->tabtax[0], fd_nj_plot->racine, fd_nj_plot->notu, FALSE);
	if(fd_nj_plot->racine->v1 != v2 || fd_nj_plot->racine->v2 != v1) {//accept new root
		fd_nj_plot->root_br_l = l;
		fd_nj_plot->root_num = -1;
		}
	else {//don't accept to only exchange v1 & v2 of previous root
		fd_nj_plot->racine->v1 = v1; fd_nj_plot->racine->v2 = v2;
		fd_nj_plot->racine->l1 = l1; fd_nj_plot->racine->l2 = l2;
		}
	fd_nj_plot->rooted = TRUE;
	fd_nj_plot->need_runtree = TRUE;
	if (fd_nj_plot->panel) fd_nj_plot->panel->window()->redraw();	
}

void edit_tree_header(Fl_Widget *wgt, void *unused)
{
	char *p, *q, *newtree;
	int i;
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)wgt->user_data();
	if(fd_nj_plot->notu == 0) return;
	SEA_VIEW *view = fd_nj_plot->view;
	char *reply = (char*)fl_input("Set tree header:", fd_nj_plot->tree_label);
	if(reply == NULL) return;
  while ((p = strchr(reply, '[')) != NULL) *p = ' '; // remove [ and ] from tree label
  while ((p = strchr(reply, ']')) != NULL) *p = ' ';
  if(fd_nj_plot->tree_label != NULL) free(fd_nj_plot->tree_label);
	fd_nj_plot->tree_label = strdup(reply);
	fd_nj_plot->panel->window()->redraw();
	if(view == NULL) return;
	p = fd_nj_plot->trees;
	for(i = 0; i < view->tot_trees; i++) {//search for current tree among known trees
		if(strcmp(view->trees[i], p) == 0) break;
		}
	if(i >= view->tot_trees) return;
	while(*p == ' ') p++;
  newtree = fd_nj_plot->current_tree;
  if (*newtree == '[') newtree = strchr(newtree, ']') + 1;
  p = nextpar(newtree);
  while (*p != ';') p++;
  q = (char*)malloc(p - newtree + 2);
  memcpy(q, newtree, p-newtree+1);
  q[p-newtree+1] = 0;
  replace_with_new_tree(fd_nj_plot, q);
  free(view->trees[i]);
  view->trees[i] = strdup(fd_nj_plot->trees);
}


void set_win_size_callback(Fl_Widget *wgt, void *data)
{
	int w = 300, h = 100;
	char current[30];
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)wgt->user_data();
	sprintf(current, "%dx%d", fd_nj_plot->full->window()->w(), fd_nj_plot->full->window()->h() );
	const char *reply = fl_input("Enter desired window size", current);
	if(reply == NULL) return;
	sscanf(reply, "%dx%d", &w, &h);
	fd_nj_plot->full->window()->size(w, h);
	fd_nj_plot->full->window()->size_range(300, 100);
}


int compare_newick_with_names(const char *tree, char **names, int notu, char **pname)
/* returns 0 iff names in tree are the same as in names[0::notu[
           1 when name from tree absent from names and puts differing name in *pname
		  -1 when elements in names absent from tree and puts one absent name in *pname
 */
{
	char *p = (char *)tree - 1;
	int found = 0, i, l, *seen, retval;
	char *start = NULL;
	static char name[200];
	seen = (int *)calloc(notu, sizeof(int));
	while(*(++p) != 0) {
		if(*p == '(' ) start = p + 1;
		else if(start == NULL && *p == ',') start = p + 1;
		else if(start != NULL && (*p == ')' || *p == ':' || *p == ',') ) {
			found++;
			while(*start == ' ') start++;
			memcpy(name, start, p - start); name[p - start] = 0;
			l = strlen(name) - 1; while( l >= 0 && name[l] == ' ')  name[l--] = 0;
			for(i = 0; i < notu; i++) if(strcmp(name, names[i]) == 0) break;
			if(i == notu) {
				if(pname != NULL) *pname = name;
				free(seen);
				return 1;
				}
			else seen[i] = 1;
			if(*p != ',') start = NULL; else start = p + 1;
			}
		}
	for(i = 0; i < notu; i++) if(!seen[i]) break;
	if(i < notu) {
		strcpy(name, names[i]);
		if(pname != NULL) *pname = name;
		retval = -1;
	}
	else retval = 0;
	free(seen);
	return retval;
}


void edit_shape_callback(Fl_Widget *obj, void *data)
{
FD_nj_plot *fd_nj_plot = (FD_nj_plot *)obj->user_data();
Fl_Menu_Item *item = (Fl_Menu_Item*)((Fl_Menu_ *)obj)->mvalue();
if( fd_nj_plot->notu == 0) return;
if( fd_nj_plot->select_clade_button == NULL || item->value() ) {
	char *p, *q, *oldtree, *newtree, *label;
	int in_name = FALSE;
	oldtree = ecrit_arbre_parenth( fd_nj_plot, fd_nj_plot->racine);
	label = fd_nj_plot->tree_label;
	newtree = (char *)malloc(strlen(oldtree) + 1);//rewrite tree without lengths nor bootstrap
	q = newtree;
	p = oldtree - 1;
	while(*(++p) != 0) {
		if( *p != '(' && (*(p-1) == ',' || *(p-1) == '(')) {
			in_name = TRUE;
			*q++ = *p;
			continue;
			}
		else if(in_name) {
			if(strchr(":,()", *p) != NULL) in_name = FALSE;
			}
		if(in_name || *p == ',' || *p == ')' || *p == '(') *q++ = *p;
		}
	*(q++) = ';'; *q = 0;
	free(oldtree);
	if(fd_nj_plot->select_clade_button == NULL) {
		Fl_Window *w = treedraw(newtree, fd_nj_plot->view, "edited_tree", FALSE);
		fd_nj_plot = (FD_nj_plot *)w->user_data();
		int x = fd_nj_plot->l_button->x();
		int y = fd_nj_plot->l_button->y();
		Fl_Group::current(fd_nj_plot->l_button->parent());
		fd_nj_plot->select_clade_button = new Fl_Button(x, y, 90, 20, "Select group");
		fd_nj_plot->delete_clade_button = new Fl_Button(fd_nj_plot->select_clade_button->x() + fd_nj_plot->select_clade_button->w() 
														+ 5, y, 90, 20, "Delete group");
		fd_nj_plot->complete_edit_button = new Fl_Button(fd_nj_plot->delete_clade_button->x() + fd_nj_plot->delete_clade_button->w() 
														 + 5, y, 65, 20, "End edit");
		fd_nj_plot->select_clade_button->callback(select_clade_callback, fd_nj_plot);
		fd_nj_plot->delete_clade_button->callback(delete_clade_callback, fd_nj_plot);
		fd_nj_plot->complete_edit_button->callback(complete_edit_callback, fd_nj_plot);
	  (fd_nj_plot->menu_edit->get_menu() + fd_nj_plot->edit_shape_rank)->flags = FL_MENU_TOGGLE;
		if(label != NULL) fd_nj_plot->tree_label = strdup(label);
		}
	else {
		fd_nj_plot->select_clade_button->show();
		fd_nj_plot->delete_clade_button->show();
		fd_nj_plot->complete_edit_button->show();
		}
	fd_nj_plot->l_button->hide();
	fd_nj_plot->bt_button->hide();
	if(fd_nj_plot->root_unroot != NULL) fd_nj_plot->root_unroot->hide();
	(fd_nj_plot->menu_edit->get_menu() + fd_nj_plot->edit_shape_rank)->set();
	fd_nj_plot->menu_file->bar_item()->deactivate();
	select_clade_callback(NULL, fd_nj_plot);
	}
else {
	complete_edit_callback(NULL, fd_nj_plot);
	}
}


void edit_trim_callback(Fl_Widget *obj, void *data)
{
  const char *rep = fl_input("Enter desired sequence name trimming rule", get_res_value("name_trimming_rule", ""));
  if (rep) {
    set_res_value("name_trimming_rule", rep);
  }
}


void select_clade_callback(Fl_Widget *obj, void *data)
{
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)data;
	fd_nj_plot->choix = prune_clade;
	fd_nj_plot->select_clade_button->deactivate();
	fd_nj_plot->delete_clade_button->deactivate();
	fd_nj_plot->swap_button->value(0);
	fd_nj_plot->need_runtree = TRUE;
	fd_nj_plot->panel->window()->redraw();
}


void delete_clade_callback(Fl_Widget *obj, void *data)
{
	struct noeud *parent, *gparent, *sister;
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)data;
	parent = fd_nj_plot->prune_clade_node->v3;
	if(parent->v1 == fd_nj_plot->prune_clade_node) sister = parent->v2;
	else sister = parent->v1;
	gparent = parent->v3;
	if(gparent != NULL) {
		if(gparent->v1 == parent) { gparent->v1 = sister; gparent->l1 += sister->l3; sister->l3 = gparent->l1; }
		else { gparent->v2 = sister; gparent->l2 += sister->l3; sister->l3 = gparent->l2; }
		sister->v3 = gparent; 
		}
	else {
		struct noeud *X, *Y;
		X = sister->v1; Y = sister->v2;
		if(X == NULL || Y == NULL) return;
		parent->v1 = X; parent->l1 = sister->l3 + X->l3;
		X->v3 = parent; X->l3 = parent->l1;
		parent->v2 = Y; parent->l2 = sister->l3 + Y->l3;
		Y->v3 = parent; Y->l3 = parent->l2;
		}
	clear_squares_below(fd_nj_plot->racine);
	char *newtree = ecrit_arbre_parenth(fd_nj_plot, fd_nj_plot->racine);
	char *p = strdup(fd_nj_plot->tree_name);
	char *q;
	if(fd_nj_plot->tree_label != NULL) q = strdup(fd_nj_plot->tree_label);
	else q = NULL;
	free_tree(fd_nj_plot);
	fd_nj_plot->tree_name = p;
  fd_nj_plot->full->window()->copy_label(p);
	fd_nj_plot->tree_label = q;
	fd_nj_plot->trees = newtree;
  fd_nj_plot->current_tree = newtree;
  fd_nj_plot->rank = 0;
	if(preptree(fd_nj_plot) == NULL) {
		select_clade_callback(NULL, data);
		}
	else {
		fd_nj_plot->notu = 0;
		fd_nj_plot->panel->window()->redraw();
		}
}

	
void complete_edit_callback(Fl_Widget *obj, void *data)
{
	FD_nj_plot *fd_nj_plot = (FD_nj_plot *)data;
	fd_nj_plot->l_button->show();
	fd_nj_plot->bt_button->show();
	if(fd_nj_plot->root_unroot != NULL) fd_nj_plot->root_unroot->show();
	((Fl_Menu_Item *)fd_nj_plot->menu_edit->get_menu() + fd_nj_plot->edit_shape_rank)->clear();
	fd_nj_plot->select_clade_button->hide();
	fd_nj_plot->delete_clade_button->hide();
	fd_nj_plot->complete_edit_button->hide();
	fd_nj_plot->choix = show_tree;
	fd_nj_plot->need_runtree = TRUE;
	fd_nj_plot->panel->window()->redraw();
	fd_nj_plot->menu_file->bar_item()->activate();
}


void sel_next_in_subtree(struct noeud *center, SEA_VIEW *view, char **missing)
{
	if(center == NULL) return;
	if(center->v1 == NULL || center->v2 == NULL || center->v3 == NULL) {//a leaf
		int i;
		for(i = 0; i < view->tot_seqs; i++)
			if(view->sel_seqs[i] == 0 && strcmp(center->nom, view->seqname[i]) == 0) break;
		if(i < view->tot_seqs) {
			view->sel_seqs[i] = 1;
			}
		else if(*missing == NULL) *missing = center->nom;
		}
	sel_next_in_subtree(center->v1, view, missing);
	sel_next_in_subtree(center->v2, view, missing);
}


void select_in_alignment(FD_nj_plot *fd_nj_plot)
//selects in alignment all members of subtree
{
	char *missing = NULL;
	SEA_VIEW *view = fd_nj_plot->view;
	if(view == NULL || fd_nj_plot->subtree_center == NULL) return;
	view->tot_sel_seqs = 0;
	memset(view->sel_seqs, 0, view->tot_seqs * sizeof(int));
	sel_next_in_subtree(fd_nj_plot->subtree_center, view, &missing);
	for(int i = 0; i < view->tot_seqs; i++) if(view->sel_seqs[i]) view->tot_sel_seqs++;
	select_deselect_seq(view, -2);
	view->DNA_obj->redraw();
	if(view->tot_sel_seqs != fd_nj_plot->subtree_notu) 
		fl_alert("At least one subtree leave not found in alignment: %s", missing);
}


static FD_nj_plot *find_matching_tree_window(SEA_VIEW *view)
{ // returns the FD_nj_plot of the first tree window found that refers to view
  // and that is in selection mode
  Fl_Window *win = Fl::first_window();
  while (win) {
    const char *c;
    if (!win->parent() && (c = win->xclass()) != NULL && strcmp(c, TREE_WINDOW) == 0) {
      FD_nj_plot *fd_nj_plot = (FD_nj_plot *)(win->user_data());
      if (fd_nj_plot->select->value() && fd_nj_plot->view == view) {
	return fd_nj_plot;
      }
    }
    win = Fl::next_window(win);
  }
  return NULL;
}


void select_deselect_in_tree(SEA_VIEW *view)
{ // select/deselect seqs in tree following selection in view
  FD_nj_plot* tree_plot = find_matching_tree_window(view);
  if (tree_plot) {
    tree_plot->need_runtree = TRUE;
    tree_plot->panel->window()->redraw();
  }
}


void print_title(int x, int y, char *text, int family, int size, int p, int totp)
{
	static char ligne[200];
	time_t heure;
	int h;
	
	time (&heure);
	sprintf(ligne, "Seaview    %s    %s", text, ctime(&heure) );
	h = strlen(ligne) - 1; ligne[h] = 0;
	if(totp > 1) sprintf(ligne + h, " Page %d of %d", p, totp);
	fl_color(FL_BLACK);
	fl_font(family, size);
	fl_draw(ligne, x, y);
}

extern void print_unrooted(FD_unrooted *fd_unrooted, const char *name, bool to_ps_file, const char *directname = NULL);
void print_plot(FD_nj_plot *fd_nj_plot, bool to_ps_file, const char *directname)
{
  if(fd_nj_plot->fd_unrooted != NULL) {
    print_unrooted((FD_unrooted *)fd_nj_plot->fd_unrooted, fd_nj_plot->tree_name, to_ps_file, directname);
    return;
    }
  int h, page, superpos, frompage, topage, error, title_height, title_ascent;
  int true_print_rect_x, true_print_rect_y, true_print_rect_w, true_print_rect_h;
  int title_rect_x, title_rect_y, title_rect_w, title_rect_h;
  
  if(fd_nj_plot->notu == 0) return;
  if (fd_nj_plot->full) my_watch_cursor(fd_nj_plot->full->window());
  Fl_Printer* myprinter;
  if (to_ps_file) {
    myprinter = (Fl_Printer*)new Fl_PDF_or_PS_File_Device();
    if (directname) {
      error = ((Fl_PDF_or_PS_File_Device*)myprinter)->begin_document(directname, printout_pageformat, printout_layout);
      }
    else error = ((Fl_PDF_or_PS_File_Device*)myprinter)->start_job(fd_nj_plot->tree_name, printout_pageformat, printout_layout);
    frompage = 1;
    topage = fd_nj_plot->page_count;
    }
  else {
    myprinter = new Fl_Printer();
    error = myprinter->start_job(fd_nj_plot->page_count, &frompage, &topage);
    }
  if (error) {
    fl_reset_cursor(fd_nj_plot->full->window());
    return;
  }
#ifndef NO_PDF
  jmp_buf *jbuf;
  if (to_ps_file) ((Fl_PDF_File_Device*)myprinter)->surface_try(&jbuf);
  if ( (!to_ps_file) || setjmp(*jbuf) == 0) { // replaces PDF_TRY
#endif
  myprinter->printable_rect(&true_print_rect_w, &true_print_rect_h);
  true_print_rect_x = true_print_rect_y = 0;
  fl_font(FL_TIMES, 11);
  calc_text_size((char*)"Mq", &title_height, &title_ascent);
  title_rect_x = true_print_rect_x; 
  title_rect_y = true_print_rect_y;
  title_rect_w = true_print_rect_w; 
  title_rect_h = title_ascent;
  true_print_rect_y += title_height;
  true_print_rect_h -= title_height;
    fl_font(fd_nj_plot->font_family, fd_nj_plot->font_size);
    calc_text_size((char*)"Mq", &fd_nj_plot->char_height, &fd_nj_plot->ascent);

  h = true_print_rect_h;
  superpos = h / 30;
  physx = true_print_rect_w;
  physy = (fd_nj_plot->page_count - 1) * (h - superpos) + h - 1;
  page_x_offset = true_print_rect_x;
  page_y_offset = true_print_rect_y;
  page_y_offset -= (frompage - 1) * (h - superpos);
  for(page = frompage; page <= topage; page++) {
    myprinter->start_page();
    fl_push_clip(title_rect_x, title_rect_y, title_rect_w, title_height);
    char *p = fd_nj_plot->tree_name;
    if (!p) p = (char*)fd_nj_plot->panel->window()->window()->label();
    print_title(title_rect_x, title_rect_y + title_rect_h - 1, p,
				  FL_TIMES, 10, page, fd_nj_plot->page_count);
    fl_pop_clip();
    fl_push_clip(true_print_rect_x, true_print_rect_y, true_print_rect_w, true_print_rect_h );
    fl_font(fd_nj_plot->font_family, fd_nj_plot->font_size);
    fl_color(FL_GRAY);
    fl_rect(true_print_rect_x, true_print_rect_y, true_print_rect_w, true_print_rect_h);
    fl_color(FL_BLACK);
    do_plot(fd_nj_plot, TRUE);
    fl_pop_clip();
    myprinter->end_page();
    page_y_offset -= h - superpos;
  }
  myprinter->end_job();
#ifndef NO_PDF
  } // end of PDF_TRY
  if (to_ps_file && ((Fl_PDF_File_Device*)myprinter)->surface_catch()) { // replaces PDF_CATCH
    ((Fl_PDF_File_Device*)myprinter)->error_catch();
  }
#endif
  delete myprinter;
  fd_nj_plot->need_runtree = TRUE;
  if (fd_nj_plot->panel) fd_nj_plot->panel->redraw();	
  page_x_offset = 0;
  page_y_offset = 0;
  if (fd_nj_plot->full) fl_reset_cursor(fd_nj_plot->full->window());
}

bool is_parent(struct noeud *centre, struct noeud *from, const char *label)
{ // returns true if leave label is below centre in tree (away from from)
  int count = 0;
  if (centre->v1) count++;
  if (centre->v2) count++;
  if (centre->v3) count++;
  if (count == 1) { // a leaf
    return strcmp(centre->nom, label) == 0;
  }
  if (centre->v1 == from) {
    return is_parent(centre->v2, centre, label) || is_parent(centre->v3,centre, label);
  }
  else if (centre->v2==from) {
    return is_parent(centre->v1, centre, label) || is_parent(centre->v3, centre, label);
  }
  return is_parent(centre->v1, centre, label) || is_parent(centre->v2, centre, label);
}

int count_node(struct noeud *centre, struct noeud *from)
{ // count # of matching leaves below centre
  int count = 0;
  if (centre->v1) count++;
  if (centre->v2) count++;
  if (centre->v3) count++;
  if (count == 1) { // a leaf
    if (strcmp(centre->nom, target1) == 0 || strcmp(centre->nom, target2) == 0 || strcmp(centre->nom, target3) == 0) {
      return 1;
    }
    return 0;
  }
  if (centre->v1 == from) {
    return count_node(centre->v2, centre) + count_node(centre->v3, centre);
  }
  else if (centre->v2==from) {
    return count_node(centre->v1, centre) + count_node(centre->v3, centre);
  }
  return count_node(centre->v1, centre) + count_node(centre->v2, centre);
}

struct noeud *span_nodes(struct noeud *centre, struct noeud *from)
/* parcourir recursivement tous les noeuds de l'arbre sans racine
 a partir de centre et dans la direction opposee a son voisin from
 */
{
  if(centre == NULL) return NULL;
  int c1=0, c2=0, c3 =0;
  if (centre->v1) c1 = count_node(centre->v1, centre);
  if (centre->v2) c2 = count_node(centre->v2, centre);
  if (centre->v3) c3 = count_node(centre->v3, centre);
  if (c1 == 1 && c2 == 1 && c3 == 1) return centre;
  struct noeud *v = NULL;
  if (centre->v1==from) {
    v = span_nodes(centre->v2,centre);
    if (!v) v = span_nodes(centre->v3,centre);
  }
  else if (centre->v2==from) {
    v = span_nodes(centre->v1,centre);
    if (!v) v = span_nodes(centre->v3,centre);
  }
  else {
    v = span_nodes(centre->v1,centre);
    if (!v) v = span_nodes(centre->v2,centre);
  }
  return v;
}

float argval(int argc, char *argv[], const char *arg, float defval)
{
  for (int i = 1; i < argc - 1; i++) {
    if (strcmp(argv[i], arg) == 0) {
      sscanf(argv[i+1], "%f", &defval);
      break;
      }
    }
  return defval;
}

char *argname(int argc, char *argv[], const char *arg)
{
  for (int i = 1; i < argc - 1; i++) {
    if (strcmp(argv[i], arg) == 0) {
      return argv[i+1];
    }
  }
  return NULL;
}


bool isarg(int argc, char *argv[], const char *arg)
{
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], arg) == 0) return true;
  }
  return false;
}

#ifdef WIN32
#ifdef small
#undef small  // necessary under WIN32
#endif
#endif

double recur_patristic_distances(FD_nj_plot *fd_nj_plot, struct noeud *centre, float **d, int **listdesc)
/* Recursive part of patristic distance computation.
 int **listdesc of size 2*notu - 1 (number of internal and terminal nodes)
 can hold in listdesc[rank] a bit list of notu bits where rank is the number of a tree node.
 listdesc of a leaf = leaf rank at one.
 listdesc of an internal node = union (listdesc of left son, listdesc of right son).
 For a node with left and right sons, and l1 length to left, and l2 length to right:
 d(i, j) is incremented by l1 for all i in listdesc(left) and all j not in listdesc(left),
 d(i, j) is incremented by l2 for all i in listdesc(right) and all j not in listdesc(right).
 At most depth+1 bit lists are simultaneously allocated where depth is the max # of branches from root to tip.
 Returns the length of the subtree rooted at centre.
 */
{
  int i, j, small, big;
  const int lmot = 8*sizeof(int); // bits in an int
  const int width = (fd_nj_plot->notu + lmot - 1)/lmot; // # of int's to hold notu bits
  float l;
  if (centre->v1 == NULL && centre->v2 == NULL) { // centre is a leaf
    listdesc[centre->rank] = (int*)calloc(width, sizeof(int));
    bit1(listdesc[centre->rank], centre->rank + 1);
  }
  else	{
    double left, right;
    left = recur_patristic_distances(fd_nj_plot, centre->v1, d, listdesc);
    if (fd_nj_plot->has_br_length || centre->l1 == 0) l = (float)centre->l1;
    else if (centre == fd_nj_plot->racine) l = 0.5;
    else l = 1;
    for (i = 0; i < fd_nj_plot->notu; i++) {
      if (testbit(listdesc[centre->v1->rank], i+1) == 0) continue;
      for (j = 0; j < fd_nj_plot->notu; j++) {
	if (testbit(listdesc[centre->v1->rank], j+1) == 0) {
	  if (i < j) { small = i; big = j; }
	  else { small = j; big = i; }
	  d[small][big-small-1] += l; 
	}
      }
    }
    right = recur_patristic_distances(fd_nj_plot, centre->v2, d, listdesc);    
    if (fd_nj_plot->has_br_length || centre->l2 == 0) l = (float)centre->l2;
    else if (centre == fd_nj_plot->racine) l = 0.5;
    else l = 1;
    for (i = 0; i < fd_nj_plot->notu; i++) {
      if (testbit(listdesc[centre->v2->rank], i+1) == 0) continue;
      for (j = 0; j < fd_nj_plot->notu; j++) {
	if (testbit(listdesc[centre->v2->rank], j+1) == 0) {
	  if (i < j) { small = i; big = j; }
	  else { small = j; big = i; }
	  d[small][big-small-1] += l; 
	}
      }
    }
    for (i = 0; i < width; i++)  listdesc[centre->v1->rank][i] |= listdesc[centre->v2->rank][i];
    listdesc[centre->rank] = listdesc[centre->v1->rank];
    free(listdesc[centre->v2->rank]);
    return left + centre->l1 + right + centre->l2;
  }
  return 0;
}

void calc_patristic_distances(FD_nj_plot *fd_nj_plot, FILE *out)
/* Compute patristic distances from rooted tree in fd_nj_plot,
 and write them to out one line for each pair.
 */
{
  int i, j;
  char *n1, *n2, *p;
  int **listdesc = (int**)calloc(2 *fd_nj_plot->notu - 1, sizeof(int*));
  float **d = (float**)calloc(fd_nj_plot->notu, sizeof(float*));
  for (i = 0; i < fd_nj_plot->notu - 1; i++) d[i] = (float*)calloc(fd_nj_plot->notu - i - 1, sizeof(float));
  struct noeud *center;
  int *testlist;
  if (fd_nj_plot->subtree_center) {
    center = fd_nj_plot->subtree_center;
    }
  else {
    center = fd_nj_plot->racine;
    testlist = NULL;
    }
  double tree_length = recur_patristic_distances(fd_nj_plot, center, d, listdesc);
  if (fd_nj_plot->subtree_center) {
    testlist = listdesc[fd_nj_plot->subtree_center->rank];
  }
  fprintf(out, "#patristic distances (tree length=%f)\n", tree_length);
  for (i = 0; i < fd_nj_plot->notu; i++) {
    if (testlist && !testbit(testlist, i+1)) continue;
    for (j = i+1; j < fd_nj_plot->notu; j++) {
      if (testlist && !testbit(testlist, j+1)) continue;
      n1 = fd_nj_plot->tabtax[i]->nom;
      n2 = fd_nj_plot->tabtax[j]->nom;
      if (strcmp(n1, n2) > 0) { p = n1; n1 = n2; n2 = p; }
      fprintf(out, "%s,%s: %f\n", n1, n2, d[i][j-i-1]);
    }
    free(d[i]);
  }
  free(d);
  free(listdesc[center->rank]);
  free(listdesc);
}


#ifndef NO_PDF
void plotonly(int argc, char *argv[])
{
  const char *fname = argv[argc-1]; // read tree in file named in last argument or in stdin
  FILE *in = (strcmp(fname, "-") != 0 ? fopen(fname, "r") : stdin);
  if (!in) return;
  int l = 1000;
  char *tree = (char*)malloc(l+1);
  char line[200], *p = tree, *q;
  while (true) {
    if (fgets(line, sizeof(line), in) == NULL) break;
    if (p - tree + (int)strlen(line) + 1 > l) {
      l += strlen(line) + 1000;
      q = (char*)realloc(tree, l + 1);
      p = q + (p - tree);
      tree = q;
    }
    q = line;
    while (*q) {
      if (*q != '\n' && *q != '\r') *p++ = *q;
      q++;
    }
  }
  *p = 0;
  fclose(in);
  FD_nj_plot *fd_nj_plot = (FD_nj_plot *)calloc(1, sizeof(FD_nj_plot));
  fd_nj_plot->trees = tree;
  fd_nj_plot->current_tree = tree;
  fd_nj_plot->rank = 0;
  fd_nj_plot->font_size = argval(argc, argv, "-fontsize", 12);
  fd_nj_plot->font_family = FL_TIMES;
  fd_nj_plot->page_count = argval(argc, argv, "-pagecount", 1);
  fd_nj_plot->plot_br_l = isarg(argc, argv, "-lengths");
  fd_nj_plot->show_bootstrap = 
      isarg(argc, argv, "-bootstrap") || isarg(argc, argv, "-bootstrap_threshold");
  fd_nj_plot->bootstrap_threshold = argval(argc, argv, "-bootstrap_threshold", 0);
  int use_svg = isarg(argc, argv, "-svg");
  fd_nj_plot->choix = show_tree;
  printout_pageformat = isarg(argc, argv, "-letter") ? Fl_Paged_Device::LETTER : Fl_Paged_Device::A4;
  outotu = argname(argc, argv, "-outgroup");
  if (!outotu) inotu = argname(argc, argv, "-ingroup");
  if (outotu || inotu) {
    target1 = strtok( (outotu?outotu:inotu), ",");
    p = target1 + strlen(target1) - 1; while (p > target1 && *p == ' ') *p-- = 0;
    target2 = strtok(NULL, ",");
    if (target2) {
      while (*target2 == ' ')  target2++;
      p = target2 + strlen(target2) - 1; while (p > target2 && *p == ' ') *p-- = 0;
      target3 = strtok(NULL, ",");
      if (target3) while (*target3 == ' ')  target3++;
      }
    }
  
  char *trim = argname(argc, argv, "-trim");
    fd_nj_plot->leaf_trimmer = new LeafNameTrimmer(0, 0, 10, 20);
    fd_nj_plot->leaf_trimmer->user_data(fd_nj_plot);
    fd_nj_plot->leaf_trimmer->copy_label(trim ? trim : "");
  
  if (preptree(fd_nj_plot)) {
    free(fd_nj_plot);
    return;
    }
  char *pdfname;
  FILE *out;
  if ( isarg(argc, argv, "-reroot") ) {
    if ( isarg(argc, argv, "-plotonly") ) {
      fprintf(stderr, "Can't use both -plotonly and -reroot\n");
      exit(1);
      }
    if ( isarg(argc, argv, "-root_at_center") ) {
      midpoint_callback(NULL, fd_nj_plot);
      }
    if ( (pdfname = argname(argc, argv, "-outnewick")) == NULL) {
      pdfname = (char*)malloc(strlen(fname) + 10);
      strcpy(pdfname, fname);
      if ((p = strrchr(pdfname, '.')) == NULL) p = pdfname + strlen(pdfname);
      strcpy(p, ".tree");
      if (strcmp(fname, pdfname) == 0) strcat(pdfname, "_2");
    }
    if (trim) fd_nj_plot->leaf_trimmer->process_to_labels();
    // we must call runtree() so the tree is correctly oriented from its new root
    // so we must have a graphics driver to compute string widths
    Fl_PDF_Graphics_Driver* pdf_driver = new Fl_PDF_Graphics_Driver;
    fl_graphics_driver = pdf_driver;
    pdf_driver->pdf = PDF_new();
    PDF_begin_document(pdf_driver->pdf, "", 0, "");
    fl_font(FL_TIMES, 12);
    runtree(fd_nj_plot);
    if ( (p = argname(argc, argv, "-patristic_distances")) ) {
      out = fopen(p, "w");
      if (!out) Fl::fatal("Can't open %s for writing.", p);
      calc_patristic_distances(fd_nj_plot, out);
      fclose(out);
      exit(0);
    }
    out = fopen(pdfname, "w");
    if (out) {
      bool write_unrooted = false;
      if (isarg(argc, argv, "-unroot")) write_unrooted = true;
      if (isarg(argc, argv, "-remove_bootstrap")) {
	fd_nj_plot->totbranches = -1; // forces branch support not to appear in Nexus tree
	if (!fd_nj_plot->input_was_rooted) write_unrooted = true;
	}
      if (write_unrooted) tree = ecrit_arbre_parenth_unrooted(fd_nj_plot, fd_nj_plot->racine);
      else tree = ecrit_arbre_parenth(fd_nj_plot, fd_nj_plot->racine);
      fwrite(tree, 1, strlen(tree), out);
      free(tree);
      fputs("\n", out);
      fclose(out);
    }
    exit(0);
  }
  fd_nj_plot->tree_name = strdup(fname);
  maxy = 1000.;
  if ((pdfname = argname(argc, argv, "-o")) == NULL)  {
    pdfname = (char*)malloc(strlen(fname) + 5);
    strcpy(pdfname, fname);
    if ((p = strrchr(pdfname, '.')) == NULL) p = pdfname + strlen(pdfname);
    strcpy(p, use_svg ? ".svg" : ".pdf");
  }
  Fl_SVG_File_Surface *svg;
  if (use_svg) {
    out = (strcmp(pdfname, "-") != 0 ? fl_fopen(pdfname, "w") : stdout);
    if (!out) return;
    physx = Fl_Paged_Device::page_formats[printout_pageformat].width;
    physy = Fl_Paged_Device::page_formats[printout_pageformat].height;
    if (isarg(argc, argv, "-landscape")) {
      int tmp = physx;
      physx = physy;
      physy = tmp;
    }
    if (isarg(argc, argv, "-size")) {
      sscanf(argname(argc, argv, "-size"), "%lfx%lf", &physx, &physy);
      }
    svg = new Fl_SVG_File_Surface(physx, physy, out, true);
    svg->set_current();
    }
  else {
    // we must call runtree() so we must have a graphics driver to compute string widths
    Fl_PDF_Graphics_Driver* pdf_driver = new Fl_PDF_Graphics_Driver;
    fl_graphics_driver = pdf_driver;
    pdf_driver->pdf = PDF_new();
    PDF_begin_document(pdf_driver->pdf, "", 0, "");
    if (isarg(argc, argv, "-landscape")) printout_layout = Fl_Paged_Device::LANDSCAPE;
  }
  fl_font(fd_nj_plot->font_family, fd_nj_plot->font_size);
  if (isarg(argc, argv, "-branch_width_as_support")) {
    fd_nj_plot->branch_width_as_support = true;
    fd_nj_plot->support_threshold_high = argval(argc, argv, "-support_threshold_high", .95);
    fd_nj_plot->support_threshold_low  = argval(argc, argv, "-support_threshold_low",   .8);
    }
  runtree(fd_nj_plot);
  if (!use_svg) {
    Fl_PDF_Graphics_Driver* pdf_driver = (Fl_PDF_Graphics_Driver*)fl_graphics_driver;
    PDF_delete(pdf_driver->pdf);
    delete pdf_driver;
    }
  if (isarg(argc, argv, "-unrooted")) {
    tree = ecrit_arbre_parenth_unrooted(fd_nj_plot, fd_nj_plot->racine);
    if (tree == NULL) { return; }
    FD_unrooted *fd_unrooted = (FD_unrooted *) calloc(1, sizeof(FD_unrooted));
    fd_nj_plot->fd_unrooted = fd_unrooted;
    if ( debut_arbre(tree, fd_unrooted) ) return;
    free(tree);
    fd_unrooted->font_size = fd_nj_plot->font_size;
    fd_nj_plot->leaf_trimmer->process_unrooted();
    }

  char *select, aux[500];
  if ((select = argname(argc, argv, "-match"))) {
    majuscules(select);
    bool unrooted = (fd_nj_plot->fd_unrooted != NULL);
    int last = unrooted ? 2  * ((FD_unrooted *)fd_nj_plot->fd_unrooted)->notu  - 4 : fd_nj_plot->totnoms;
    for(int num = 0; num <= last; num++) {
      if (unrooted && ((FD_unrooted *)fd_nj_plot->fd_unrooted)->branches[num].nom == NULL) continue;
      strcpy(aux, unrooted ? ((FD_unrooted *)fd_nj_plot->fd_unrooted)->branches[num].nom : (fd_nj_plot->noms+num)->nom );
      majuscules(aux);
      if(strstr( aux, select) != NULL) {
	if (unrooted) ((FD_unrooted *)fd_nj_plot->fd_unrooted)->branches[num].color = TRUE;
	else (fd_nj_plot->noms+num)->disp_option = 'r';
      }
    }
    if ((select = argname(argc, argv, "-color"))) {
      int rgb;
      sscanf(select, "%x", &rgb);
      Fl::set_color(FL_RED, rgb >> 16, (rgb >> 8) & 0xFF, rgb & 0xFF);
    }
  }
  if (use_svg) {
    svg_tree_save(fd_nj_plot, svg);
    }
  else {
    print_plot(fd_nj_plot, true, pdfname);
    }
  if (argname(argc, argv, "-o") == NULL) free(pdfname);
  if ( (pdfname = argname(argc, argv, "-outnewick"))) {
    out = (strcmp(pdfname, "-") != 0 ? fopen(pdfname, "w") : stdout);
    if (out) {
      char *newick = ecrit_arbre_parenth(fd_nj_plot, fd_nj_plot->racine);
      fwrite(newick, 1, strlen(newick), out);
      free(newick);
      fputs("\n", out);
      fclose(out);
    }
  }
}
#endif // NO_PDF

#if !(defined(__APPLE__) || defined(WIN32)) && 100*FL_MAJOR_VERSION + 10*FL_MINOR_VERSION  + FL_PATCH_VERSION == 130
// for X11 under 1.3.0 only
#undef Fl_Printer
#undef Fl_PostScript_File_Device
corr_Fl_Printer::corr_Fl_Printer() : Fl_Printer() {
  count = 0;
}

corr_Fl_PostScript_File_Device::corr_Fl_PostScript_File_Device() : Fl_PostScript_File_Device() {
  count = 0;
}


// bypass FLTK 1.3.0 bug where printable_rect() not usable before start_page() on X11
int corr_Fl_Printer::start_job(int pagecount, int *frompage, int *topage) {
  int retval = Fl_Printer::start_job(pagecount, frompage, topage);
  if (retval) return retval;
  start_page();
  return 0;
}

int corr_Fl_PostScript_File_Device::start_job(int pagecount, enum Fl_Paged_Device::Page_Format format, 
						     enum Fl_Paged_Device::Page_Layout layout)
 {
  int retval = Fl_PostScript_File_Device::start_job(pagecount, format, layout);
  if (retval) return retval;
  start_page();
  return 0;
}

int corr_Fl_PostScript_File_Device::start_job(FILE *file, int pagecount, enum Fl_Paged_Device::Page_Format format, 
					      enum Fl_Paged_Device::Page_Layout layout)
{
  int retval = Fl_PostScript_File_Device::start_job(file, pagecount, format, layout);
  if (retval) return retval;
  start_page();
  return 0;
}

// correct FLTK 1.3.0 Fl_PostScript_Printer bug where %%PageBoundingBox is missing.
int corr_Fl_Printer::start_page() {
  if (count++ == 1) return 0;
  int retval = Fl_Printer::start_page();
  Fl_PostScript_Graphics_Driver *d = (Fl_PostScript_Graphics_Driver*)driver();
  if (count == 1) {
    m = (int)d->pw_;
    M = (int)d->ph_;
    if(m > M) {
      int x;
      x = m; m = M; M = x;
    }
  }
  fprintf(d->output, "%%%%PageBoundingBox: 0 0 %d %d\n", m, M);
  return retval;
}

int corr_Fl_PostScript_File_Device::start_page()
{
  if (count++ == 1) return 0;
  int retval = Fl_PostScript_File_Device::start_page();
  Fl_PostScript_Graphics_Driver *d = (Fl_PostScript_Graphics_Driver*)driver();
  if (count == 1) {
    m = (int)d->pw_;
    M = (int)d->ph_;
    if(m > M) {
      int x;
      x = m; m = M; M = x;
    }
  }
  fprintf(d->output, "%%%%PageBoundingBox: 0 0 %d %d\n", m, M);
  return retval;
}
#endif // for X11 under 1.3.0 only


typedef struct extra_node {
  double m; // largest root-to-tip length in subtree
  struct noeud *f; // child directing to largest length m
} extra_node_struct;

static extra_node_struct *extras;
static double best_tip_to_tip; // current best tip-to-tip length
static struct noeud *current_r; //  node corresponding to best_tip_to_tip
static struct noeud *ancestor; // ancestor of current_r node

static void traverse_midpoint(struct noeud *parent, struct noeud *centre )
{
  extra_node_struct *extra = &extras[centre->rank];
  struct noeud *left, *right;
  double ll, lr, ml, mr;
  if (centre->v1 == parent) {
    left = centre->v2;
    right = centre->v3;
    ll = centre->l2;
    lr = centre->l3;
  }
  else if (centre->v2 == parent) {
    left = centre->v1;
    right = centre->v3;
    ll = centre->l1;
    lr = centre->l3;
  }
  else {
    left = centre->v1;
    right = centre->v2;
    ll = centre->l1;
    lr = centre->l2;
  }
  if (left) traverse_midpoint(centre, left);
  if (right) traverse_midpoint(centre, right);
  ml = left ? extras[left->rank].m + ll : 0;
  mr = right ? extras[right->rank].m + lr : 0;
  if (ml > mr) {
    extra->m = ml;
    extra->f = left;
  }
  else {
    extra->m = mr;
    extra->f = right;
  }
  if (ml + mr > best_tip_to_tip) {
    best_tip_to_tip = ml + mr;
    ancestor = parent;
    current_r = centre;
  }
}


static double midpoint_rooting(struct noeud *from, int notu, double *p_root_br_length)
{
  extras = (extra_node_struct*)malloc((2*notu-1)*sizeof(extra_node_struct));
  
  best_tip_to_tip = -10000;
  traverse_midpoint(NULL, from);

  struct noeud *node, *tmp, *centre, *left, *right;
  if (ancestor == NULL) {
    centre = current_r->v3;
    right = extras[centre->rank].f;
    left = current_r;
    extras[left->rank].f = NULL;
  }
  else {
    centre = current_r;
    if (centre->v1 == ancestor) {   left = centre->v2;    right = centre->v3;  }
    else if (centre->v2 == ancestor) {left = centre->v1; right = centre->v3; }
    else { left = centre->v1; right = centre->v2; }
  }
  double ll = 0, lr = 0, lb, laux;
  node = left;
  while (node) {
    tmp = extras[node->rank].f;
    if (!tmp) {
      break;
    }
    if (node->v1 == tmp) ll += node->l1;
    else if (node->v2 == tmp) ll += node->l2;
    else ll += node->l3;
    node = tmp;
    }
  node = right;
  while (node) {
    tmp = extras[node->rank].f;
    if (!tmp) {
      break;
    }
    if (node->v1 == tmp) lr += node->l1;
    else if (node->v2 == tmp) lr += node->l2;
    else lr += node->l3;
    node = tmp;
  }
  while ( !(ll <= best_tip_to_tip/2 && lr <= best_tip_to_tip/2) ) {
    if (ll > lr) {
      if (right->v1 == centre) lb = right->l1;
      else if (right->v2 == centre) lb = right->l2;
      else lb = right->l3;
      lr += lb;
      right = centre;
      centre = left;
      left = extras[left->rank].f;
      if (left->v1 == centre) lb = left->l1;
      else if (left->v2 == centre) lb = left->l2;
      else lb = left->l3;
      ll -= lb;
    }
    else {
      if (left->v1 == centre) lb = left->l1;
      else if (left->v2 == centre) lb = left->l2;
      else lb = left->l3;
      ll += lb;
      left = centre;
      centre = right;
      right = extras[right->rank].f;
      if (right->v1 == centre) lb = right->l1;
      else if (right->v2 == centre) lb = right->l2;
      else lb = right->l3;
      lr -= lb;
    }
  }
  free(extras);
  
  if( !left || !right) {
    current_cote1 = centre;
    current_cote2 = centre->v3;
    *p_root_br_length = 0;
    return 0;
  }
  if (left->v1 == centre) laux = left->l1;
  else if (left->v2 == centre) laux = left->l2;
  else laux = left->l3;
  if (right->v1 == centre) lb = right->l1;
  else if (right->v2 == centre) lb = right->l2;
  else lb = right->l3;
  if (lr + lb >= best_tip_to_tip/2) {
    node = right;
    ll += laux;
    current_cote1 = centre;
    current_cote2 = node;
    }
  else {
    node = left;
    lr += lb;
    lb = laux;
    current_cote1 = node;
    current_cote2 = centre;
  }
  *p_root_br_length = lb;
  return (best_tip_to_tip/2 - ll) / lb;
}

char *get_next_tree(FD_nj_plot *fd_nj_plot)
{
  char *next = fd_nj_plot->current_tree;
  while (isspace(*next)) next++;
  if (*next == '[') {
    do next++; while (*next && *next != ']');
  }
  while (*next && *next != '(') next++;
  if (*next == 0) return NULL;
  next = nextpar(next);
  if (next == 0) return NULL;
  next++;
  while (*next && *next != ';') next++;
  if (*next == 0) return NULL;
  char *p = ++next;
  while (*p && *p != '(') p++;
  if (*p == 0) return NULL;
  fd_nj_plot->current_tree = next;
  fd_nj_plot->rank++;
  return next;
}

void replace_with_new_tree(FD_nj_plot *fd_nj_plot, char *newtree)
{
  char *p, *end = fd_nj_plot->current_tree;
  int found = -1;
  while (isspace(*end)) end++;
  if (*end == '[') {
    do end++; while (*end != ']');
  }
  while (*end != '(') end++;
  end = nextpar(end);
  while (*end != ';') end++;
  end++;
  int lnt = strlen(newtree);
  int le = strlen(end);
  int l = (fd_nj_plot->current_tree - fd_nj_plot->trees) + lnt + le;
  if (fd_nj_plot->tree_label) l += strlen(fd_nj_plot->tree_label) + 2;
  char *s = (char*)malloc(l+1);
  p = s;
  if (fd_nj_plot->current_tree > fd_nj_plot->trees) {
    memcpy(s, fd_nj_plot->trees, fd_nj_plot->current_tree - fd_nj_plot->trees);
    p += fd_nj_plot->current_tree - fd_nj_plot->trees;
  }
  if (fd_nj_plot->tree_label) {
    sprintf(p, "[%s]", fd_nj_plot->tree_label);
    p += strlen(p);
  }
  memcpy(p, newtree, lnt);
  p += lnt;
  memcpy(p, end, le + 1);
  if (fd_nj_plot->view) {
    for (int i = 0; i < fd_nj_plot->view->tot_trees; i++) {
      if (strcmp(fd_nj_plot->trees, fd_nj_plot->view->trees[i]) == 0) {
        found = i;
        break;
      }
    }
  }
  free(fd_nj_plot->trees);
  free(newtree);
  fd_nj_plot->current_tree = s + (fd_nj_plot->current_tree - fd_nj_plot->trees);
  fd_nj_plot->trees = s;
  if (found >= 0) {
    free(fd_nj_plot->view->trees[found]);
    fd_nj_plot->view->trees[found] = strdup(s);
  }
}

void tree_counter_callback(Fl_Widget *wid, void *data)
{
  char *p;
  Fl_Counter *counter = (Fl_Counter*)wid;
  FD_nj_plot *fd_nj_plot = (FD_nj_plot*)data;
  if (fd_nj_plot->root_unroot->value() != 0) {
    fd_nj_plot->root_unroot->value(0);
    fd_nj_plot->root_unroot->do_callback();
    for (int i = 0; i < fd_nj_plot->notu; i++) {
      strcpy(fd_nj_plot->tabtax[i]->nom, fd_nj_plot->labels[i]);
    }
  }
  p = ecrit_arbre_parenth(fd_nj_plot, fd_nj_plot->racine);
  replace_with_new_tree(fd_nj_plot, p); // to preserve reroots/swaps
  char *oldtree = fd_nj_plot->current_tree;
  int oldrank = fd_nj_plot->rank;
  if (counter->value() == fd_nj_plot->rank + 2) p = get_next_tree(fd_nj_plot);
  else {
    fd_nj_plot->current_tree = fd_nj_plot->trees;
    fd_nj_plot->rank = 0;
    p = fd_nj_plot->trees;
    while (p && fd_nj_plot->rank < counter->value()-1) {
      p = get_next_tree(fd_nj_plot);
    }
  }
  if (!p) {
    counter->bounds(1, fd_nj_plot->rank + 1);
    if (counter->value() > counter->maximum()) counter->value(counter->maximum());
  } else {
    char *oldtrees = fd_nj_plot->trees;
    fd_nj_plot->trees = NULL;
    char *oldname = fd_nj_plot->tree_name;
    fd_nj_plot->tree_name = NULL;
    free_tree(fd_nj_plot);
    fd_nj_plot->trees = oldtrees;
    fd_nj_plot->tree_name = oldname;
    const char *mess = preptree(fd_nj_plot);
    if (mess) {
      fl_alert("%s", mess);
      counter->bounds(1, oldrank+1);
      fd_nj_plot->current_tree = oldtree;
      preptree(fd_nj_plot);
      fd_nj_plot->rank = oldrank;
      counter->value(oldrank+1);
      fd_nj_plot->need_runtree = TRUE;
      return;
    }
    fd_nj_plot->need_runtree = TRUE;
    fd_nj_plot->full->setonly();
    fd_nj_plot->choix = show_tree;
    if (fd_nj_plot->has_internal) fd_nj_plot->bt_button->activate();
    if (fd_nj_plot->has_br_length) fd_nj_plot->l_button->activate();
    fd_nj_plot->panel->window()->redraw();
  }
}

int calc_tree_count(char *tree)
{
  int count = 0;
  while (TRUE) {
    while (*tree && isspace(*tree)) tree++;
    if (*tree == '[') do tree++; while (*tree && *tree != ']');
    while (*tree && *tree != '(') tree++;
    if (*tree == '(') tree = nextpar(tree);
    while (*tree && *tree != ';') tree++;
    if (*tree == 0) break;
    count++;
  }
  return count;
}

/* Remove parentheses around comma-free blocks
     ( name-or-block : length1 ) bootstrap : length2
 that occur with IQtree. This substring is replaced by :
       name-or-block : length1+length2
 */
void remove_parenth_no_comma(char *tree)
{
  char *close, *p;
  int found = FALSE;
  if (*tree != '(') return;
  close = nextpar(tree);
  p = tree;
  while (++p < close) {
    if (*p == '(') {
      char *q = nextpar(p);
      remove_parenth_no_comma(p);
      p = q;
    }
    else if(*p == ',') {
      found = TRUE;
    }
  }
  if (!found) {
    char *q, *r, length_value[20];
    int len;
    double l2, l3;
    p = close-1;
    while (*p != ':') {
      p--;
      if (p <= tree) return;
    }
    sscanf(p+1, "%lf", &l2);
    q = close + 1;
    while (*q != ':') {
      if (*q == 0 || *q == ')') return;
      q++;
    }
    sscanf(q+1, "%lf", &l3);
    q = strchr(q, ')');
    if (q == NULL) return;
    *tree = ' ';
    r = p+1;
    do *r++ = ' '; while (r < q);
    sprintf(length_value, "%f", l2+l3);
    len = strlen(length_value);
    if (p + len >= q) len = q - p - 1;
    memcpy(p + 1, length_value, len);
  }
}

/*
 void check_tree(struct noeud *from, struct noeud *parent)
{
  struct noeud *left, *right;
  double ll, lr;
  if(from->v1 == parent) {left = from->v2; right=from->v3; ll=from->l2; lr=from->l3;}
  else if(from->v2 == parent) {left = from->v1; right=from->v3; ll=from->l1; lr=from->l3;}
  else {left = from->v1; right=from->v2; ll=from->l1; lr=from->l2;}
  if (left==NULL && right==NULL) {
    if(!from->nom[0]) {
      printf("pb nom\n");
      }
    return;
    }
  if (left==gracine || right==gracine) {
    printf( "pb racine %s\n",from->nom);
    }
  if ((!left || !right) && parent) goto bad;
  if ( left ) {
    if ( !(left->v1==from || left->v2==from || left->v3==from) ) goto bad;
    if ( !(left->l1==ll || left->l2==ll || left->l3==ll) ) printf( "bad l %s\n",from->nom);
    check_tree(left, from);
    }
  if ( right ) {
  if ( !(right->v1==from || right->v2==from || right->v3==from) ) goto bad;
  if ( !(right->l1==lr || right->l2==lr || right->l3==lr) ) printf( "bad l %s\n",from->nom);
    check_tree(right, from);
    }
  return;
bad:
  printf( "pb %s\n",from->nom);
}
*/
