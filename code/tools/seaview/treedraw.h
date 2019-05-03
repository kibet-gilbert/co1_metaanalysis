#ifndef TREEDRAW_H
#define TREEDRAW_H

#include "seaview.h"
#include <FL/Fl_Window.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/Fl_Simple_Counter.H>
#define myrint(x) (int)floor(x + 0.5)


struct noeud {
	double l1,l2,l3;
	struct noeud *v1,*v2,*v3;
	char *nom;
	int rank;
	// Branches may be associated with id
	// And this may be used in some cases
	int br_id1, br_id2, br_id3;
};

struct nom {
	double x,y;
	char *nom;
	char disp_option;
};
struct mon_point {
	double x,y;
	int number;
};
struct trait {
	double xd,yd,xf,yf;
	int width;
};

enum operations {show_tree = 1, depl_racine, permutation, subtree, selection, prune_clade, prune_clade_found};

class tree_panel : public Fl_Widget {
    FL_EXPORT int handle(int);
public:
    FL_EXPORT void draw(void);
    FL_EXPORT tree_panel(int x,int y,int w,int h) :
	Fl_Widget(x,y,w,h,NULL) {
		;
	}
};

class LeafNameTrimmer : public Fl_Widget {
public:
  LeafNameTrimmer(int x, int y, int w, int h);
  int handle(int event);
  void process();
  void process_to_labels();
  void process_unrooted();
  void draw();
  int by_position();
};

struct FD_nj_plot {
	char *tree_name;
	char *tree_label;
  char *trees; // a Newick string with possibly several trees in it
  int rank; // rank in trees of current_tree (from 0)
  char *current_tree; // start of current tree within ->trees string
	int notu;
	int totbranches;
	int has_br_length;
	int lost_br_lengths;
	int has_internal;
	int root_num;
	int rooted; // true if tree is currently rooted
	int input_was_rooted; // true if tree read from Newick string was rooted
	struct noeud **tabtax;
	struct noeud *racine;
	char **labels;
	struct branche *branches;
	struct nom *noms;
	struct trait *traits;
	struct mon_point *points;
	char *br_length_txt;
	int totnoms;
	int totpoints;
	int tottraits;
	double deltay;
	double tek_dx, tek_dy, tek_ymin;
	int show_bootstrap;
	int need_runtree;
	enum operations choix;
	int *widnames;
	double *profs;
	int long_arbre_parenth; /* long de la chaine decrivant l'arbre lu */
	struct noeud *subtree_center, *subtree_ascend;
	int subtree_notu;
	int plot_br_l;
	double root_br_l;
	int char_height;
	int ascent;
	int font_size;
	int font_family;
	int swap;
	float bootstrap_threshold;
	SEA_VIEW *view;
	Fl_Round_Button *full;
	Fl_Round_Button *swap_button;
	Fl_Round_Button *subtree;
  Fl_Round_Button *select;
	Fl_Round_Button *new_outgroup;
	Fl_Button *up;
	Fl_Check_Button *l_button;
	Fl_Check_Button *bt_button;
	tree_panel *panel;
	void *fd_unrooted;
	Fl_Scrollbar *scroller;
  vlength_menu *menu_file;
  vlength_menu* menu_edit;
  vlength_menu* menu_font;
	double zoomvalue;
	struct noeud *prune_clade_node;
	int colored_names_1, colored_traits_1, colored_names_2, colored_traits_2;
	int edit_shape_rank;
	int center_rank;
	Fl_Button *select_clade_button, *delete_clade_button, *complete_edit_button;
	Fl_Simple_Counter *zoom;
  Fl_Simple_Counter *change_tree;
	Fl_Choice *root_unroot;
	int page_count;
  bool branch_width_as_support;
  float support_threshold_high;
  float support_threshold_low;
  LeafNameTrimmer *leaf_trimmer;
};

extern Fl_Window *treedraw(char *tree, SEA_VIEW *view, const char *name, int from_tree_menu, int count = 1);
char *get_next_tree(FD_nj_plot *fd_nj_plot);
void free_tree(FD_nj_plot *fd_nj_plot);

#endif // TREEDRAW_H
