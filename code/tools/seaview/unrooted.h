#include <FL/Fl_Widget.H>
#include <FL/Fl_Scrollbar.H>
#include <math.h>
#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif
#ifndef M_PI_2
#define	M_PI_2		(M_PI/2)
#endif
#ifndef FALSE
#define FALSE 0
#define TRUE (!FALSE)
#endif

typedef enum { NO_HIT, LEFT_HIT, RIGHT_HIT, TOP_HIT, BOTTOM_HIT } hit;

typedef struct {
	int x, y;
} point;


typedef struct _cp_point {
	double x, y, r, angle;
} cp_point;


typedef struct _unrooted_branch {
	cp_point debut, fin;
	int color;
	char *nom;
  int rank;
} unrooted_branch;


typedef struct { /* une branche definie par ses deux extremites */
	struct noeud *bouta;
	struct noeud *boutb;
} branche_noeud;


typedef struct _bignoeud {
	struct _bignoeud *v1, *v2, *v3;
	double l1, l2, l3;
	cp_point position;
	char *nom;
  int rank;
} bignoeud;



 typedef struct {
	unsigned int left : 1;
	unsigned int right : 1;
	unsigned int top : 1;
	unsigned int bottom : 1;
} hits_field; 


class unrooted_panel : public Fl_Widget {
public:
	void draw(void);
	int handle(int);
	unrooted_panel(int x,int y,int w,int h) :
	Fl_Widget(x,y,w,h,NULL) {
		;
	}
};


typedef struct _FD_unrooted {
	unrooted_panel *unrooted_plot;
	Fl_Scrollbar *vert_scroller, *hor_scroller;
	int notu;
	int rooted;
	int x, y, w, h; //dimensions of plot
	int x_offset, y_offset; //offset when zoomed plot is scrolled around
	int previous_w, previous_h;
	int current_font;
	int font_size;
	int comp_phys_bounds;
	struct noeud **tabtax;
	struct noeud *racine;
	double root_br_l;
	int has_br_length;
	unrooted_branch *branches;
	branche_noeud *branche_noeuds;
	int totbranches;
	double log_min_x, log_min_y, log_max_x, log_max_y, mini_br_length;
	int phys_min_x, phys_min_y, phys_max_x, phys_max_y;
	float zoomvalue;
} FD_unrooted;
