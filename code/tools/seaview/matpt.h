#ifndef FD_matpt_h_
#define FD_matpt_h_
#include "seaview.h"

typedef struct {
	Fl_Window *form;
	Fl_Widget *mat_panel, *ident_type_box, *win_size_type_box, 
		*compute_butt, *x_slider, *y_slider, *region_box, 
		*region_slider, *align_button, *choice_ref_seq,
		*interrupt_butt;
	int phys_width;
	int view_x, view_y;
	int view_size;
	int longmax;
	int seqlong1, seqlong2;
	int maxseqlength;
	char *seq1, *seq2;
	char *seqname1, *seqname2;
	char *plotname;
	int margin;
	int hitx, hity;
	int region_size;
	int fenetre, identites;
	char regionboxtext[300];
	char *title;
	int totsegments;
	int need_compute;
	int modif_but_not_saved;
	double factor, kx, ky;
	int interrupted;
	int rect_only;
	gap_site *gap_sites;
	int tot_gap_sites;
	void *seaview_data;
  int kk, dmin, dmax, flip, *it, centre, *addgaps1, *addgaps2;
  int seqlong1_nogaps, seqlong2_nogaps;
} FD_matpt;

typedef struct {
	int x, y, w, h;
	} rectangle;

#endif /* FD_matpt_h_ */
