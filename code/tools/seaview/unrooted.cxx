#include <ctype.h>
#include <time.h>
#include "unrooted.h"
#include "treedraw.h"
#include "pdf_or_ps.h"
#include <FL/Fl_Printer.H>
#include <FL/Fl_PostScript.H>


/* prototypes of included functions */
void rooted_unrooted_callback(Fl_Widget *o, void *data);
void unrooted_scroller_cb(Fl_Widget *wgt, void *data);
void unrooted_zoom_cb(Fl_Widget *o, void *data);
void frame_and_draw_unrooted(FD_unrooted *fd_unrooted);
void draw_tree(FD_unrooted *ob);
int debut_arbre(const char *tree, FD_unrooted *fd_unrooted);
void my_draw_text(Fl_Align align, int x, int y, int size, int font, char *chaine);
hit draw_line(unrooted_branch *br, FD_unrooted *ob, int doit, int char_width,
        int char_height, int descend, int ascent);
hit try_angle_draw(float w, int *px, int *py, float *pdegrees /* clockwise degrees */, 
				   int bx, int by, int bw, int bh);
hit draw_name_angle(cp_point *phys_pos, const char *nom, FD_unrooted *ob, double angle,
        int doit, int char_width, int char_height, int descend, int color,
        int ascent);
void draw_scale(FD_unrooted *data, int char_height, int ascent);
int my_get_char_height(int *ascend, int *descend);
void free_unrooted(FD_unrooted *data);
void unrooted_search(FD_unrooted *fd_unrooted, const char *select);
const char *preptree_unrooted(const char *tree, FD_unrooted *data);
static void loadphylip(char *arbre, FD_unrooted *data);
static struct noeud *unrootedset(char *arbre, char *deb, char *fin, branche_noeud **p_int_br, FD_unrooted *data);
double calc_echelle(FD_unrooted *data, int larg);
bignoeud *cre_new_tree(struct noeud *debut, struct noeud *parent, bignoeud *bigparent);
void remove_big_root(bignoeud *bigracine);
double calc_dist_centre_feuilles(bignoeud *debut, bignoeud *parent);
double proc_null_neg_branches(bignoeud *debut, bignoeud *parent);
int set_angles_noeuds(bignoeud *debut, bignoeud *parent, double delta,
					  double *p_current_angle, double rayon);
void calc_cartesienne( cp_point *p);
void calc_polaire( cp_point *p);
cp_point calc_point_direction( cp_point *depart, cp_point *direction, 
							  double longueur, double mini_br_length);
unrooted_branch *calc_position_noeuds(bignoeud *debut, bignoeud *parent,
									  unrooted_branch *curr_branche, double mini_br_length);
void mem_line(cp_point *debut, cp_point *fin, bignoeud *noeud_term, 
			  unrooted_branch *br);
void free_bignoeud(bignoeud *debut, bignoeud *parent);
int chg_phys(FD_unrooted *ob, hits_field *hits );
void log_to_phys(FD_unrooted *data, cp_point *log_pos, cp_point *phys_pos);
double length_log_phys(FD_unrooted *data, double p);
double length_phys_log(FD_unrooted *data, double p);
void squared2circular(FD_nj_plot *fd_nj_plot, Fl_Choice *o);
void circular2squared(FD_nj_plot *fd_nj_plot);
void print_unrooted(FD_unrooted *fd_unrooted, bool to_ps_file, const char *directname = NULL);


/* external functions */
extern const char *extract_filename(const char *fname);
extern void majuscules(char *p);
extern void *check_alloc(int nbrelt, int sizelt);
extern void my_watch_cursor(Fl_Window *win);
extern void fl_reset_cursor(Fl_Window *win);
extern char *ecrit_arbre_parenth_unrooted(FD_nj_plot *fd_nj_plot, struct noeud *root);
extern double place_midpoint_root(struct noeud *from, struct noeud *racine, int notu, int fixed_outgroup);
extern const char *make_binary_or_unrooted(char *arbre);
extern int make_binary(char *arbre, char *debut, char *fin, int go_down);
extern char *nextpar(char *pospar);
extern double arrondi_echelle(double x);
extern void zoom_callback(Fl_Widget *wgt, void *data);
extern void scroller_callback(Fl_Widget *wgt, void *data);
extern int calc_brl_for_lengthless(struct noeud *centre, struct noeud *pere);
extern const char *preptree(FD_nj_plot *fd_nj_plot);
extern void free_tree(FD_nj_plot *fd_nj_plot);
extern char *ecrit_arbre_parenth(FD_nj_plot *fd_nj_plot, struct noeud *root);
extern void replace_with_new_tree(FD_nj_plot *fd_nj_plot, char *newtree);


/* globals */
static int nextotu, num_noeud;
extern Fl_Paged_Device::Page_Format printout_pageformat;
extern Fl_Paged_Device::Page_Layout printout_layout;


void rooted_unrooted_callback(Fl_Widget *o, void *data)
{
  FD_nj_plot *fd_nj_plot = (FD_nj_plot *)data;
  char *keep, *keepn, *keepl;
  if(((Fl_Choice *)o)->value() == 0) {//goto squared display
	if(fd_nj_plot->fd_unrooted != NULL) circular2squared(fd_nj_plot);
	if(fd_nj_plot->lost_br_lengths) {
	  fd_nj_plot->lost_br_lengths = FALSE;
    keep = fd_nj_plot->trees;
	  keepn = fd_nj_plot->tree_name;
	  keepl = fd_nj_plot->tree_label;
	  fd_nj_plot->trees = NULL;
	  fd_nj_plot->tree_name = NULL;
	  fd_nj_plot->tree_label = NULL;
	  free_tree(fd_nj_plot);
    fd_nj_plot->trees = keep;
	  fd_nj_plot->tree_name = keepn;
	  fd_nj_plot->tree_label = keepl;
	  preptree(fd_nj_plot);//reinterpret tree for its branch lengths
	}
	if(fd_nj_plot->has_br_length) fd_nj_plot->l_button->activate();
	fd_nj_plot->new_outgroup->activate();
	fd_nj_plot->swap_button->activate();
	fd_nj_plot->need_runtree = TRUE;
	fd_nj_plot->panel->show();
	fd_nj_plot->panel->window()->redraw();
  }
  else if(((Fl_Choice *)o)->value() == 1) {//goto circular display
    if (fd_nj_plot->notu < 3) {
      fd_nj_plot->root_unroot->value(0);
      return;
    }
	if(fd_nj_plot->lost_br_lengths) {
	  fd_nj_plot->lost_br_lengths = FALSE;
	  keep = fd_nj_plot->trees;
	  keepn = fd_nj_plot->tree_name;
	  keepl = fd_nj_plot->tree_label;
	  fd_nj_plot->trees = NULL;
	  fd_nj_plot->tree_name = NULL;
	  fd_nj_plot->tree_label = NULL;
	  free_tree(fd_nj_plot);
	  fd_nj_plot->trees = keep;
	  fd_nj_plot->tree_name = keepn;
	  fd_nj_plot->tree_label = keepl;
	  preptree(fd_nj_plot);//reinterpret tree for its branch lengths
	  for(int i=0; i<fd_nj_plot->notu; i++) {
		strcpy(fd_nj_plot->tabtax[i]->nom, fd_nj_plot->labels[i]);
	  }
	}
	if(fd_nj_plot->fd_unrooted == NULL) squared2circular(fd_nj_plot, (Fl_Choice *)o);
	}
  else if(((Fl_Choice *)o)->value() == 2) {//goto Cladogram display
    if(fd_nj_plot->fd_unrooted != NULL) circular2squared(fd_nj_plot);
    keep = ecrit_arbre_parenth(fd_nj_plot, fd_nj_plot->racine); // to share reroot/swaps between squared/cladogram
    replace_with_new_tree(fd_nj_plot, keep);
    fd_nj_plot->lost_br_lengths = TRUE;
    fd_nj_plot->has_br_length = FALSE;
    calc_brl_for_lengthless(fd_nj_plot->racine, NULL);
    fd_nj_plot->need_runtree = TRUE;
    fd_nj_plot->l_button->deactivate();
    fd_nj_plot->new_outgroup->deactivate();
    fd_nj_plot->swap_button->deactivate();
    fd_nj_plot->choix = show_tree;
    fd_nj_plot->full->setonly();
    fd_nj_plot->panel->show();
    fd_nj_plot->panel->window()->redraw();
	}
}

void squared2circular(FD_nj_plot *fd_nj_plot, Fl_Choice *o)
{
	Fl_Menu_Item *item = fd_nj_plot->menu_file->get_menu();
  	Fl_Menu_Item *edit_item = fd_nj_plot->menu_edit->get_menu();
	Fl_Widget *panel = fd_nj_plot->panel;
	FD_unrooted *fd_unrooted;
	char *tree = ecrit_arbre_parenth_unrooted(fd_nj_plot, fd_nj_plot->racine);
	if(tree == NULL) { o->value(0); return; }
	fd_unrooted = (FD_unrooted *) calloc(1, sizeof(FD_unrooted));
	int err = debut_arbre(tree, fd_unrooted);
	free(tree);
	if(err) {
	  o->value(0);
	  free(fd_unrooted);
	  return;
	}
	panel->hide();
	fd_nj_plot->scroller->hide();
	fd_nj_plot->l_button->deactivate();
	fd_nj_plot->bt_button->deactivate();
	fd_nj_plot->full->deactivate();
	fd_nj_plot->swap_button->deactivate();
	fd_nj_plot->subtree->deactivate();
	fd_nj_plot->new_outgroup->deactivate();
  fd_nj_plot->select->deactivate();
	edit_item[4].deactivate();//edit header
	edit_item[5].deactivate();//bootstrap threshold
	edit_item[9].deactivate();//root at center
	edit_item[fd_nj_plot->edit_shape_rank].deactivate();//edit shape
	item[2].deactivate();//save rooted tree
	item[3].deactivate();//save unrooted tree
	item[9].deactivate();//Page count
	fd_unrooted->zoomvalue = 1;
	fd_nj_plot->zoom->value(1);
	Fl_Window *w2 = (Fl_Window *)panel->parent();
	Fl_Group::current(w2);
	fd_unrooted->vert_scroller = new Fl_Scrollbar(w2->w() - 15, 0, 15, fd_nj_plot->scroller->h());
	fd_unrooted->vert_scroller->hide();
	fd_unrooted->vert_scroller->bounds(0, 1);
	fd_unrooted->vert_scroller->slider_size(1);
	((Fl_Slider *)fd_unrooted->vert_scroller)->value(0);
	fd_unrooted->y = 0;
	fd_unrooted->y_offset = 0;
	fd_unrooted->vert_scroller->callback(unrooted_scroller_cb, fd_unrooted);		
	fd_nj_plot->zoom->callback(unrooted_zoom_cb, fd_unrooted);
	fd_unrooted->unrooted_plot = new unrooted_panel(0, 0, w2->w(), w2->h());
	Fl_Box *box = new Fl_Box(0, 0, w2->w() - 15, w2->h() - 15);
	box->hide();
	w2->resizable(box);
	fd_unrooted->unrooted_plot->color(FL_WHITE);
	fd_unrooted->unrooted_plot->user_data(fd_unrooted);
	fd_unrooted->hor_scroller = new Fl_Scrollbar(0, w2->h() - 15, w2->w() - 15, 15);
	fd_unrooted->hor_scroller->hide();
	fd_unrooted->hor_scroller->type(FL_HORIZONTAL);
	fd_unrooted->hor_scroller->bounds(0, 1);
	fd_unrooted->hor_scroller->slider_size(1);
	((Fl_Slider *)fd_unrooted->hor_scroller)->value(0);
	fd_unrooted->x = 0;
	fd_unrooted->x_offset = 0;
	fd_unrooted->hor_scroller->callback(unrooted_scroller_cb, fd_unrooted);
	fd_unrooted->current_font = fd_nj_plot->font_family;
	fd_unrooted->font_size = fd_nj_plot->font_size;
	fd_nj_plot->fd_unrooted = (void *)fd_unrooted;
  fd_nj_plot->leaf_trimmer->process_unrooted();
}

void circular2squared(FD_nj_plot *fd_nj_plot)
{
  Fl_Menu_Item *item = fd_nj_plot->menu_file->get_menu();
  Fl_Menu_Item *edit_item = fd_nj_plot->menu_edit->get_menu();
  FD_unrooted *fd_unrooted = (FD_unrooted *)fd_nj_plot->fd_unrooted;
  fd_unrooted->unrooted_plot->hide();
  fd_unrooted->vert_scroller->hide();
  fd_unrooted->hor_scroller->hide();
  //dont't call delete but Fl::delete_widget after having removed it from its group!!!
  fd_unrooted->unrooted_plot->window()->remove( *fd_unrooted->vert_scroller );
  fd_unrooted->unrooted_plot->window()->remove( *fd_unrooted->hor_scroller );
  fd_unrooted->unrooted_plot->window()->remove( *fd_unrooted->unrooted_plot );
  Fl::delete_widget(fd_unrooted->unrooted_plot);
  Fl::delete_widget(fd_unrooted->vert_scroller);
  Fl::delete_widget(fd_unrooted->hor_scroller);
  free_unrooted(fd_unrooted);
  fd_nj_plot->fd_unrooted = NULL;
  if(fd_nj_plot->has_br_length) fd_nj_plot->l_button->activate();
  if(fd_nj_plot->has_internal) fd_nj_plot->bt_button->activate();
  if(fd_nj_plot->zoomvalue > 1.1) fd_nj_plot->scroller->show();
  fd_nj_plot->zoom->value(fd_nj_plot->zoomvalue);
  fd_nj_plot->zoom->callback(zoom_callback, fd_nj_plot);
  fd_nj_plot->full->activate();
  fd_nj_plot->swap_button->activate();
  fd_nj_plot->subtree->activate();
  fd_nj_plot->new_outgroup->activate();
  fd_nj_plot->select->activate();
  edit_item[4].activate();//edit header
  edit_item[5].activate();
  edit_item[9].activate();//root at center
  edit_item[fd_nj_plot->edit_shape_rank].activate();//edit shape
  item[2].activate();
  item[3].activate();
  item[9].activate();
}

void unrooted_scroller_cb(Fl_Widget *wgt, void *data)
{
	FD_unrooted *fd_unrooted = (FD_unrooted *)data;
	int val = ((Fl_Scrollbar *)wgt)->value();
	if(wgt->type() == FL_VERTICAL) fd_unrooted->y_offset = - val; 
	else fd_unrooted->x_offset = - val;
	fd_unrooted->unrooted_plot->parent()->redraw();	
}


void unrooted_zoom_cb(Fl_Widget *wgt, void *data)
{
	FD_unrooted *fd_unrooted = (FD_unrooted *)data;
	if(((Fl_Counter *)wgt)->value() > fd_unrooted->zoomvalue) fd_unrooted->zoomvalue *= 1.4;
	else fd_unrooted->zoomvalue /= 1.4;
	fd_unrooted->zoomvalue = ((Fl_Counter *)wgt)->clamp(fd_unrooted->zoomvalue);
	((Fl_Counter *)wgt)->value(fd_unrooted->zoomvalue);
	if(fd_unrooted->zoomvalue <= 1.1) {
		fd_unrooted->unrooted_plot->size(fd_unrooted->unrooted_plot->w(), 
										 fd_unrooted->unrooted_plot->parent()->h());
		fd_unrooted->vert_scroller->hide();
		fd_unrooted->hor_scroller->hide();
		}
	else {
		fd_unrooted->unrooted_plot->size(fd_unrooted->unrooted_plot->parent()->w() - 15, 
										 fd_unrooted->unrooted_plot->parent()->h() - 15);
		if(!fd_unrooted->vert_scroller->visible()) fd_unrooted->vert_scroller->show();
		if(!fd_unrooted->hor_scroller->visible()) fd_unrooted->hor_scroller->show();
		}
	float count = fd_unrooted->unrooted_plot->h() * (fd_unrooted->zoomvalue - 1);
	count = FL_max(count, 1);
	float old = fd_unrooted->vert_scroller->value() / fd_unrooted->vert_scroller->maximum();
	fd_unrooted->vert_scroller->maximum(count);
	((Fl_Slider *)fd_unrooted->vert_scroller)->value(myrint(old * count));
	fd_unrooted->y_offset = -(int)(old *count);
	fd_unrooted->vert_scroller->linesize(myrint(count/9));
	fd_unrooted->vert_scroller->slider_size(1/fd_unrooted->zoomvalue);
	
	count = fd_unrooted->unrooted_plot->w() * (fd_unrooted->zoomvalue - 1);
	count = FL_max(count, 1);
	old = fd_unrooted->hor_scroller->value() / fd_unrooted->hor_scroller->maximum();
	fd_unrooted->hor_scroller->maximum(count);
	((Fl_Slider *)fd_unrooted->hor_scroller)->value(myrint(old * count));
	fd_unrooted->x_offset = -(int)(old *count);
	fd_unrooted->hor_scroller->linesize(myrint(count/9));
	fd_unrooted->hor_scroller->slider_size(1/fd_unrooted->zoomvalue);
	
	fd_unrooted->comp_phys_bounds = TRUE;
	fd_unrooted->previous_w = 0; 
	fd_unrooted->previous_h = 0;
	fd_unrooted->unrooted_plot->parent()->redraw();
}


void frame_and_draw_unrooted(FD_unrooted *fd_unrooted)
{
	if( fd_unrooted->comp_phys_bounds ) {
        /* valeurs initiales des extr physiques des lignes du graphique */
        fd_unrooted->phys_max_x = fd_unrooted->x + fd_unrooted->w;
        fd_unrooted->phys_max_y = fd_unrooted->y + fd_unrooted->h;
        fd_unrooted->phys_min_x = fd_unrooted->x;
        fd_unrooted->phys_min_y = fd_unrooted->y;
	}
	fl_line_style(FL_SOLID, 2, 0);
	fl_color(FL_BLACK);
	fl_font(fd_unrooted->current_font, fd_unrooted->font_size);
	draw_tree(fd_unrooted);
	fl_line_style(FL_SOLID, 0, 0);
	fd_unrooted->comp_phys_bounds = FALSE;
}


void unrooted_panel::draw(void)
{
	int width, height;
	
	FD_unrooted *fd_unrooted = (FD_unrooted *)this->user_data();
	if(fd_unrooted->notu == 0) return;
	width = myrint(this->w() * fd_unrooted->zoomvalue);
	height = myrint(this->h() * fd_unrooted->zoomvalue);
	if( width != fd_unrooted->previous_w || 
				height != fd_unrooted->previous_h ) {
		/* si on a redimensionne la fenetre ou la 1ere fois */
		fd_unrooted->previous_w = width; 
		fd_unrooted->previous_h = height;
		fd_unrooted->comp_phys_bounds = TRUE;
		fd_unrooted->w = width;
		fd_unrooted->h = height;
	}
	fl_push_clip(this->x(), this->y(), this->w(), this->h());
	frame_and_draw_unrooted(fd_unrooted);
	fl_pop_clip();
}


int unrooted_panel::handle(int event)
{
	FD_unrooted *fd_unrooted = (FD_unrooted *)this->user_data();
	int mx = Fl::event_x();
	int my = Fl::event_y();
	static int start_x, start_y;
	static int start_sx, start_sy;
	switch(event) {
		case FL_PUSH:
			if(fd_unrooted->zoomvalue > 1) {
				this->window()->cursor(FL_CURSOR_MOVE); 
				Fl::flush();
				}
			start_x = mx;
			start_y = my;
			start_sx = fd_unrooted->hor_scroller->value();
			start_sy = fd_unrooted->vert_scroller->value();
			break;
		case FL_DRAG:
			if(fd_unrooted->zoomvalue > 1) {
				int val = (int)fd_unrooted->hor_scroller->clamp(start_sx - (mx - start_x));
				((Fl_Slider *)fd_unrooted->hor_scroller)->value(val);
				val = (int)fd_unrooted->vert_scroller->clamp(start_sy - (my - start_y));
				((Fl_Slider *)fd_unrooted->vert_scroller)->value(val);
				fd_unrooted->hor_scroller->do_callback();
				fd_unrooted->vert_scroller->do_callback();
			}
			break;
		case FL_RELEASE:
			if(fd_unrooted->zoomvalue > 1) {
				fl_reset_cursor(this->window());
				}
			break;
		default :
	return 0; // other events are not processed here
	}
	return 1; // from processed events
}


void draw_tree(FD_unrooted *ob)
{
	int char_width, char_height, num, dernier, encore = FALSE, ascend, descend;
	hit result;
	hits_field hits;
#if !(defined(__APPLE__) || defined(WIN32))
        if (fl_display && Fl::event() != FL_DRAG) {
          ob->unrooted_plot->window()->cursor(FL_CURSOR_WAIT);
          Fl::flush();
        }
#endif
	char_width = myrint(fl_width("M"));
	char_height = my_get_char_height(&ascend, &descend);
	dernier = 2*ob->notu-3;
	if( ob->comp_phys_bounds ) {
		/* rendre dimensions carrees */
		if (ob->phys_max_x - ob->phys_min_x > ob->phys_max_y - ob->phys_min_y)
			ob->phys_max_x = ob->phys_min_x + (ob->phys_max_y - ob->phys_min_y);
		else if(ob->phys_max_y - ob->phys_min_y > ob->phys_max_x - ob->phys_min_x)
			ob->phys_max_y = ob->phys_min_y + (ob->phys_max_x - ob->phys_min_x);
		
		do	{
			hits.left = hits.right = hits.top = hits.bottom = 0;
			for(num = 0; num < dernier; num++) {
				result = draw_line(ob->branches+num, ob, FALSE, 
								   char_width, char_height, descend, ascend);
				if(result == LEFT_HIT) hits.left = 1;
				else if(result == RIGHT_HIT) hits.right = 1;
				else if(result == TOP_HIT) hits.top = 1;
				else if(result == BOTTOM_HIT) hits.bottom = 1;
			}
			encore = chg_phys(ob, &hits);
		}
		while (encore);
		if (Fl_Surface_Device::surface() == Fl_Display_Device::display_device()) {
		  for(num = 0; num < dernier; num++) ob->branches[num].color = FALSE;
		}
	}
	for(num=0; num< dernier; num++) {
		draw_line(ob->branches+num, ob, TRUE, char_width, char_height, 
				  descend, ascend);
	}
	if(ob->has_br_length) draw_scale(ob, char_height, ascend);
#if !(defined(__APPLE__) || defined(WIN32))
        if (fl_display && Fl::event() != FL_DRAG) ob->unrooted_plot->window()->cursor(FL_CURSOR_DEFAULT);
#endif
}


void print_unrooted(FD_unrooted *fd_unrooted, const char *name, bool to_ps_file, const char *directname)
{
  Fl_Printer* myprinter;
  int old_x, old_y, error;
  old_x = fd_unrooted->x_offset;
  old_y = fd_unrooted->y_offset;
  if (to_ps_file) {
    myprinter = (Fl_Printer*)new Fl_PDF_or_PS_File_Device();
    if (directname) 
      error = ((Fl_PDF_or_PS_File_Device*)myprinter)->begin_document(directname, printout_pageformat, printout_layout);
    else
      error = ((Fl_PDF_or_PS_File_Device*)myprinter)->start_job(name, printout_pageformat, printout_layout) ;
    if (error) return;
    }
  else {
    int from, to;
    myprinter = new Fl_Printer();
    if (myprinter->start_job(1, &from, &to) ) return;
  }
#ifndef NO_PDF
  jmp_buf *jbuf;
  if (to_ps_file) ((Fl_PDF_File_Device*)myprinter)->surface_try(&jbuf);
  if ( (!to_ps_file) || setjmp(*jbuf) == 0) { // replaces PDF_TRY
#endif    
    myprinter->start_page();
    fd_unrooted->previous_w = fd_unrooted->previous_h = 0;
    myprinter->printable_rect( &fd_unrooted->w, &fd_unrooted->h);
    fd_unrooted->x = fd_unrooted->y = 0;
    fd_unrooted->x_offset = fd_unrooted->y_offset = 0;
    fd_unrooted->y += 20;
    fd_unrooted->h -= 20;
    time_t heure;
    time(&heure);
    char bigline[200];
    sprintf(bigline, "%s  %s", extract_filename(name), ctime(&heure));
    fl_font(FL_TIMES, 12);
    fl_color(FL_BLACK);
    fl_draw(bigline, fd_unrooted->x, fd_unrooted->y - 5);
    fl_color(FL_GRAY);
    fl_rect(fd_unrooted->x, fd_unrooted->y, fd_unrooted->w, fd_unrooted->h);
    fl_color(FL_BLACK);
    fd_unrooted->comp_phys_bounds = TRUE;	
    frame_and_draw_unrooted(fd_unrooted);
    myprinter->end_page();
    myprinter->end_job();
#ifndef NO_PDF
    } // end of PDF_TRY
    if (to_ps_file && ((Fl_PDF_File_Device*)myprinter)->surface_catch()) { // replaces PDF_CATCH
      ((Fl_PDF_File_Device*)myprinter)->error_catch();
    }
#endif
  delete myprinter;
  fd_unrooted->x_offset = old_x;
  fd_unrooted->y_offset = old_y;
}


int debut_arbre(const char *tree, FD_unrooted *fd_unrooted)
//returns 0 iff OK
{
bignoeud *centre, *bigracine;
const char *pname;
int tot;
double delta, current_angle, width, height;
unrooted_branch *fin_branche, *br;
double radius;

/* read tree string */
if( (pname=preptree_unrooted(tree, fd_unrooted)) != NULL ) {
        char mess[150];
        strcpy(mess, pname);
        fl_alert("%s", mess);
        fd_unrooted->notu = 0;
        return 1;
        }
bigracine = cre_new_tree(fd_unrooted->racine, NULL, NULL);
if(!fd_unrooted->has_br_length) 
        bigracine->l2 = 0;
centre = bigracine->v1;
/* changer de centre si c'est une feuille */
tot=0;
if(centre->v1 == NULL) tot++;
if(centre->v2 == NULL) tot++;
if(centre->v3 == NULL) tot++;
if(tot>= 2) centre = bigracine->v2;
remove_big_root(bigracine);
radius = 1.1 * calc_dist_centre_feuilles(centre, NULL);
/* on va remplacer les branches nulles par des branches
100 fois plus courtes que la plus courte de l'arbre
*/
fd_unrooted->mini_br_length = proc_null_neg_branches(centre, NULL);
fd_unrooted->mini_br_length /= 100; 
delta = 2*M_PI / fd_unrooted->notu;
current_angle = 0;
set_angles_noeuds(centre, NULL, delta, &current_angle, radius);
centre->position.r = 0;
centre->position.angle = 0;
calc_cartesienne(&(centre->position));
fd_unrooted->branches = (unrooted_branch *)check_alloc(2*fd_unrooted->notu-3, sizeof(unrooted_branch));
fin_branche = calc_position_noeuds(centre, NULL, fd_unrooted->branches, fd_unrooted->mini_br_length);
free_bignoeud(centre, NULL);
/* calcul extremites du graphique des branches */
fd_unrooted->log_min_x = fd_unrooted->log_max_x = fd_unrooted->branches->debut.x;
fd_unrooted->log_min_y = fd_unrooted->log_max_y = fd_unrooted->branches->debut.y;
for(br = fd_unrooted->branches; br < fin_branche; br++) {
        if(br->debut.x < fd_unrooted->log_min_x) fd_unrooted->log_min_x = br->debut.x;
        if(br->debut.x > fd_unrooted->log_max_x) fd_unrooted->log_max_x = br->debut.x;
        if(br->fin.x < fd_unrooted->log_min_x) fd_unrooted->log_min_x = br->fin.x;
        if(br->fin.x > fd_unrooted->log_max_x) fd_unrooted->log_max_x = br->fin.x;
        if(br->debut.y < fd_unrooted->log_min_y) fd_unrooted->log_min_y = br->debut.y;
        if(br->debut.y > fd_unrooted->log_max_y) fd_unrooted->log_max_y = br->debut.y;
        if(br->fin.y < fd_unrooted->log_min_y) fd_unrooted->log_min_y = br->fin.y;
        if(br->fin.y > fd_unrooted->log_max_y) fd_unrooted->log_max_y = br->fin.y;
        }
width = fd_unrooted->log_max_x - fd_unrooted->log_min_x;
height = fd_unrooted->log_max_y - fd_unrooted->log_min_y;
if( width > height ) {
        fd_unrooted->log_min_y -= (width - height)/2;
        fd_unrooted->log_max_y += (width - height)/2;
        }
else if( height > width ) {
        fd_unrooted->log_min_x -= (height - width)/2;
        fd_unrooted->log_max_x += (height - width)/2;
        }
return 0;
}


void my_draw_text(Fl_Align align, int x, int y, int size, int current_font, char *chaine)
{
  fl_font(current_font, size);
  if(align == FL_ALIGN_CENTER) x -= (int)(fl_width(chaine)/2);
  fl_draw(chaine, x, y);
}


hit draw_line(unrooted_branch *br, FD_unrooted *fd_unrooted, int doit, int char_width, 
        int char_height, int descend, int ascent)
{
double h, w;
double angle;
cp_point phys_debut, phys_fin;
hit result;
static char txt[300];


log_to_phys(fd_unrooted, &br->debut, &phys_debut);
log_to_phys(fd_unrooted, &br->fin, &phys_fin);
w = phys_fin.x - phys_debut.x;
h = phys_fin.y - phys_debut.y;
if(doit) {
	phys_fin.x += fd_unrooted->x_offset;
	phys_fin.y += fd_unrooted->y_offset;
	phys_debut.x += fd_unrooted->x_offset;
	phys_debut.y += fd_unrooted->y_offset;
	fl_line((int)(phys_debut.x), (int)(phys_debut.y ), (int)(phys_fin.x ), (int)(phys_fin.y ));
	}
if(br->nom != NULL) {
/* calcul de l'angle tel que vu sur le dessin: il faut utiliser -h
   car les coord en y sont calculees avec 0 en haut
   aussi mettre angle dans [0 , 2*pi[
*/
        angle = atan( (-h) / w );
        if( w < 0 ) angle = M_PI + angle;
        if(angle < 0) angle += 2*M_PI;
		if(angle > M_PI_2 && angle < 3 * M_PI_2) sprintf(txt,"%s ", br->nom);
		else sprintf(txt, " %s", br->nom);
		result = draw_name_angle(&phys_fin, txt, fd_unrooted, angle, doit, 
								 char_width, char_height, descend, br->color, ascent);
        if( result != NO_HIT ) return result;
        }
return NO_HIT;
}


hit try_angle_draw(float w, int *px, int *py, float *pdegrees /* clockwise degrees */, 
				   int bx, int by, int bw, int bh)
{
	//compute bounds x1,y1  x2,y2  x3,y3  x4,y4 of to be drawn string
	int x1, y1, x2, y2, x3, y3, x4, y4;
	float degrees = *pdegrees;
	x1 = *px; y1 = *py;
	double radians = - M_PI * degrees / 180; /* counterclockwise radians */
	if(radians < 0) radians += 2 * M_PI;
	float cosa = cos(radians);
	float sina = sin(radians);
	
	const float ch = 0.7;
	x2 = x1 - (int)(ch * fl_size() * sina);
	y2 = y1 - (int)(ch * fl_size() * cosa);
	x3 = x2 + (int)(w * cosa);
	y3 = y2 - (int)(w * sina);
	x4 = x1 + (int)(w * cosa);
	y4 = y1 - (int)(w * sina);
	int xmax = x1, ymax = y1, xmin = x1, ymin = y1;
	if(x2 > xmax) xmax = x2; if(x3 > xmax) xmax = x3; if(x4 > xmax) xmax = x4;
	if(x2 < xmin) xmin = x2; if(x3 < xmin) xmin = x3; if(x4 < xmin) xmin = x4;
	if(y2 > ymax) ymax = y2; if(y3 > ymax) ymax = y3; if(y4 > ymax) ymax = y4;
	if(y2 < ymin) ymin = y2; if(y3 < ymin) ymin = y3; if(y4 < ymin) ymin = y4;
	
	*px = x1; *py = y1; *pdegrees = degrees;
	if(xmin < bx) return LEFT_HIT;
	if(xmax > bx + bw) return RIGHT_HIT;
	if(ymin < by) return TOP_HIT;
	if(ymax > by + bh) return BOTTOM_HIT;
	return NO_HIT;
}


hit draw_name_angle(cp_point *phys_pos, const char *nom, FD_unrooted *fd_unrooted, double radians,
					int doit, int char_width, int char_height, int descend, int color,
					int ascent)
{	
	int x, y;
	float degrees = - int(radians * 180/M_PI);
	x = (int)phys_pos->x;
	y = (int)phys_pos->y;
        int dx, dy;
        fl_measure(nom, dx, dy);
        float w = dx, l = dy/4., fdx;
	hit result = try_angle_draw(w, &x, &y, &degrees, fd_unrooted->x,  fd_unrooted->y,  fd_unrooted->w,  fd_unrooted->h);
	if( (!doit) && result != NO_HIT) return result;
	if( ! doit) return NO_HIT;
	if(color) fl_color(FL_RED);
        int rt_angle = -(int)degrees;
        if (rt_angle <= 90 || rt_angle >= 270) {
          if (rt_angle >= 270) rt_angle -= 360;
          fdx =  l * sin(M_PI*rt_angle/180.);
          dx = (int)fdx;
          dy =  sqrt(l*l - fdx*fdx);
          fl_draw(rt_angle, nom, x + dx, y + dy);
        } else {
          rt_angle -= 180;
          float ddy = -sin(M_PI*(rt_angle+180)/180.) * dx;
          float ddx = cos(M_PI*(rt_angle+180)/180.) * dx;
          fdx = l * sin(M_PI*rt_angle/180.);
          dx = fdx + ddx;
          dy = sqrt(l*l - fdx*fdx) + ddy;
          fl_draw(rt_angle, nom, x + dx, y + dy);
        }
	if(color) fl_color(FL_BLACK);
	return NO_HIT;
}


void draw_scale(FD_unrooted *fd_unrooted, int charheight, int ascent)
{
char ech_name[20];
int phys_w, y, xd, xf;
double log_val;

log_val = calc_echelle(fd_unrooted, fd_unrooted->w);
phys_w = myrint(length_log_phys(fd_unrooted, log_val));
y = (int)(1.5 * charheight) + fd_unrooted->y_offset;
xf = (int)(fd_unrooted->w * 0.97) + fd_unrooted->x_offset;
xd = xf - phys_w;
fl_line_style(FL_SOLID, 0, 0);
fl_line(xd , y, xf, y);
sprintf(ech_name, "%.1g", log_val);
my_draw_text(FL_ALIGN_CENTER, (xd + xf)/2, y - charheight/3, 12, FL_TIMES, ech_name);
fl_line(xd, y - charheight/3, xd, y + charheight/3);
fl_line(xf + fd_unrooted->x, y - charheight/3, xf, y + charheight/3);
}


int my_get_char_height(int *ascend, int *descend)
{
	*ascend = fl_height() - fl_descent();
	*descend = fl_descent();
return *ascend + *descend ; 
}


void free_unrooted(FD_unrooted *data)
{
  int i;
  if(data->notu == 0) return;
  /* de-allocate all memory */
  for(i=0; i<2*data->notu - 1; i++) {
    if(data->tabtax[i]->nom != NULL) free(data->tabtax[i]->nom);
    free(data->tabtax[i]);
  }
  free(data->tabtax);
  if(data->branche_noeuds != NULL) free(data->branche_noeuds);
  for (i = 0; i < 2*data->notu - 3; i++) {
    char *p = (data->branches+i)->nom;
    if (p) free(p);
  }
  if(data->branches != NULL) free(data->branches);
  data->notu = 0;
  free(data);
}


void unrooted_search(FD_unrooted *fd_unrooted, const char *select)
{
	int trouve, totnoms, num;
	char aux[500];
	trouve = FALSE; totnoms = 2 * fd_unrooted->notu  - 3;
	for(num = 0; num < totnoms; num++) {
		if(fd_unrooted->branches[num].nom == NULL) continue;
		strcpy(aux, fd_unrooted->branches[num].nom);
		majuscules(aux);
		if(strstr( aux, select) != NULL) {
			trouve = TRUE;
			fd_unrooted->branches[num].color = TRUE;
			}
		}
	if(trouve) {
		fd_unrooted->unrooted_plot->parent()->redraw();
		}
}


const char *preptree_unrooted(const char *tree, FD_unrooted *data)
{
	int i, c, v;
	char *arbre, *finarbre, *p;
	
	arbre = strdup(tree);
	p = arbre; while(isspace(*p)) p++;
	data->notu = 2; i = 3; v = 0;
	if(*p == '(') {
		if(p > arbre) memmove(arbre, p, strlen(p) + 1);
		p = arbre + 1;
		while( (c=*(p++)) != 0 && c != ';') {
			if(c == ')') data->notu++;
			if(c == '(') i++;
			if(c == ',') v++;
		}
	}
	if(i != data->notu) {
		free(arbre);
		data->notu = 0;
		return "Incorrect tree data.";
	}
	finarbre = nextpar(arbre);
	if(finarbre == NULL) {
		data->notu = 0;
		return ("Unbalanced parentheses in tree.");
	}
	
	arbre = (char *)realloc(arbre, strlen(arbre) + 4 * v + 5 ); /* worst case add 4 chars for each , */
	p = (char *)make_binary_or_unrooted(arbre);
	if(p != NULL) {
		data->notu = 0;
		free(arbre);
		return p;
	}
	data->notu = v + 1 ; /* after this notu = number of OTUs  */
	data->totbranches = -1;
	
	/* allocate all memory */
	data->tabtax = (struct noeud **)check_alloc(2*data->notu - 1,sizeof(struct noeud *));
	if(data->notu > 3) data->branche_noeuds = 
		(branche_noeud *)check_alloc(data->notu-3, sizeof(branche_noeud));
	for (i = 0; i < 2*data->notu - 1; i++) {
	  data->tabtax[i] = (struct noeud *)check_alloc(1, sizeof(struct noeud));
	  data->tabtax[i]->rank = i;
	  }
	loadphylip(arbre, data);
	free(arbre);
	if(data->notu == 0) goto erreur;
	if(!data->rooted) {
		data->racine = *(data->tabtax+(++num_noeud));
		if(num_noeud >= 2*data->notu - 1) return("Error: incorrect tree file");
	  data->root_br_l = place_midpoint_root(data->tabtax[0], data->racine, data->notu, FALSE);
	}
	else	{
		return ("Unexpected rooted tree.");
	}
	if(data->notu < 3) return ("Tree should contain at least 3 elements.");
  return (NULL);
	
erreur:
	return ("Not a valid tree.");
} /* end of preptree_unrooted */


static void loadphylip(char *arbre, FD_unrooted *data)
{
	char *deba,*debb,*debc, *finarbre;
	struct noeud *p1, *p2, *p3, *p;
	branche_noeud *int_br_g, *int_br_d;
	
	data->has_br_length=2;
	/* ignore all stuff after last closing parenthesis 
	 (needed for fastDNAml output)
	 */
	finarbre= nextpar(arbre);
	data->rooted=0;
	deba=arbre+1;
	debb=deba;
	while(*debb != ',') {
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
		data->rooted = 1;
	}
	num_noeud = data->notu - 1;
	nextotu = -1;
	p1=unrootedset(arbre, deba, debb-2, &int_br_g, data);
	if(p1 != NULL) p2=unrootedset(arbre, debb, debc-2, &int_br_d, data);
	if(p1 == NULL || p2 == NULL || num_noeud + 1 >= 2*data->notu - 1) {
		data->notu = 0;
		return;
	}
	p = *(data->tabtax+(++num_noeud));
	if(!data->has_br_length) {
		p1->l3 = 0.5*p1->l3;
		p2->l3 = 0.5*p2->l3;
	}
	p->v1=p1; p1->v3=p; p->l1=p1->l3;
	if(int_br_g!=NULL) { int_br_g->bouta=p; int_br_g->boutb=p1; }
	p->v2=p2; p2->v3=p; p->l2=p2->l3;
	if(int_br_d!=NULL) { int_br_d->bouta=p; int_br_d->boutb=p2; }
	if(!data->rooted) {
		p3=unrootedset(arbre, debc, finarbre-1, &int_br_g, data);
		if(p3 == NULL ) {
			data->notu = 0;
			return;
		}
		if(int_br_g!=NULL) { int_br_g->bouta=p; int_br_g->boutb=p3; }
		p->v3=p3; p3->v3=p; p->l3=p3->l3;
	}
	else	{
		p->v3=NULL;
		/* recherche d'un dernier label interne */
		debc=finarbre+1;
		while(*debc!=0 && *debc!=':' && *debc!='[') debc++;
		if(debc-finarbre>1) {
			data->totbranches++;
			data->branche_noeuds[data->totbranches].bouta=p1;
			data->branche_noeuds[data->totbranches].boutb=p2;
		}
	}
}


static struct noeud *unrootedset(char *arbre, char *deb, char *fin, branche_noeud **p_int_br, FD_unrooted *data)
{
	struct noeud *p;
	char *virg;
	int l;
	branche_noeud *int_br_g, *int_br_d;
	
	*p_int_br=NULL;
	while(*deb==' ' || *deb=='\'')deb++;
	while(*fin==' ')fin--;
	virg=deb;
	while(*virg != ',' && virg < fin) {
		if(*virg == '(') virg=nextpar(virg);
		virg++;
	}
	if(virg>fin) virg=deb;
	if(*virg == ',') {
		struct noeud *p1,*p2;
		p1=unrootedset(arbre,deb,virg-1,&int_br_g, data);
		if(p1 == NULL) return NULL;
		p2=unrootedset(arbre,virg+1,fin,&int_br_d, data);
		if(p2 == NULL) return NULL;
		if(num_noeud + 1 >= 2*data->notu - 1) 
			return NULL;
		p = *(data->tabtax+(++num_noeud));
		p->v1=p1; p1->v3=p; p->l1=p1->l3;
		if(int_br_g!=NULL) { int_br_g->bouta=p; int_br_g->boutb=p1; }
		p->v2=p2; p2->v3=p; p->l2=p2->l3;
		if(int_br_d!=NULL) { int_br_d->bouta=p; int_br_d->boutb=p2; }
	}
	else	{
		double brlength;
		virg=deb;
		while(*virg != ':' && virg < fin) {
			if(*virg=='(')virg=nextpar(virg);
			virg++;
		}
		if(virg>fin) virg=deb;
		if(*virg == ':') {
			sscanf(virg+1,"%le",&brlength);
			virg--;
			data->has_br_length=1;
		}
		else	{
			brlength=1;
			data->has_br_length=0;
		}
		if(*deb == '(') {
			char *fpar;
			branche_noeud *prov;
			fpar=nextpar(deb)-1;
			p=unrootedset(arbre,deb+1,fpar,&prov, data);
			if(p == NULL) return NULL;
			/* recherche internal label */
			l=virg-fpar-1;
			if(l>0) {
				data->totbranches++;
				*p_int_br= &data->branche_noeuds[data->totbranches];
			}
		}
		else	{
			size_t n;
			if( virg-1>=deb && *virg=='\'' )virg--;
			n=virg-deb+1;
			++nextotu;
			p= *(data->tabtax+nextotu);
			p->nom = (char *)check_alloc(n+1,1);
			memcpy(p->nom, deb, n); p->nom[n] = 0;
			p->v1=p->v2=p->v3=NULL;
		}
		p->l3=brlength;
	}
	return p;
}


bignoeud *cre_new_tree(struct noeud *debut, struct noeud *parent, bignoeud *bigparent)
{
	bignoeud *nouveau;
	if(debut == NULL) return NULL;
	nouveau = (bignoeud *)check_alloc(1, sizeof(bignoeud) );
	if(debut->v1 == parent) {
		nouveau->v1 = bigparent;
		nouveau->v2 = cre_new_tree(debut->v2, debut, nouveau);
		nouveau->v3 = cre_new_tree(debut->v3, debut, nouveau);
	}
	else if(debut->v2 == parent) {
		nouveau->v2 = bigparent;
		nouveau->v1 = cre_new_tree(debut->v1, debut, nouveau);
		nouveau->v3 = cre_new_tree(debut->v3, debut, nouveau);
	}
	else	{
		nouveau->v3 = bigparent;
		nouveau->v1 = cre_new_tree(debut->v1, debut, nouveau);
		nouveau->v2 = cre_new_tree(debut->v2, debut, nouveau);
	}
	nouveau->l1 = debut->l1;
	nouveau->l2 = debut->l2;
	nouveau->l3 = debut->l3;
	nouveau->nom = debut->nom;
  nouveau->rank = debut->rank;
	return nouveau;
}


void remove_big_root(bignoeud *bigracine)
{
	bignoeud *p1, *p2;
	double root_br_l;
	
	p1=bigracine->v1;
	p2=bigracine->v2;
	root_br_l = bigracine->l1 + bigracine->l2;
	if(p1->v1 == bigracine )
	{p1->v1 = p2; p1->l1 = root_br_l;}
	else if (p1->v2 == bigracine)
	{p1->v2 = p2; p1->l2 = root_br_l;}
	else
	{p1->v3 = p2; p1->l3 = root_br_l;}
	if(p2->v1 == bigracine )
	{p2->v1 = p1; p2->l1 = root_br_l;}
	else if (p2->v2 == bigracine)
	{p2->v2 = p1; p2->l2 = root_br_l;}
	else
	{p2->v3 = p1; p2->l3 = root_br_l;}
}


double calc_dist_centre_feuilles(bignoeud *debut, bignoeud *parent)
{
	double valeur, current;
	if(debut == NULL) return 0;
	valeur = 0;  /*  !!!!! conserver cette ecriture sinon plante sur PC !!!!!! */
	if(debut->v1 != parent) {
		current = calc_dist_centre_feuilles(debut->v1, debut);
		current += debut->l1;
		if(current > valeur) valeur = current;
	}
	if(debut->v2 != parent) {
		current = calc_dist_centre_feuilles(debut->v2, debut);
		current += debut->l2;
		if(current > valeur) valeur = current;
	}
	if(debut->v3 != parent) {
		current = calc_dist_centre_feuilles(debut->v3, debut);
		current += debut->l3;
		if(current > valeur) valeur = current;
	}
	return valeur;
}


double proc_null_neg_branches(bignoeud *debut, bignoeud *parent)
/* mettre branches negatives a 0 !attention dans un seul sens!
 retourner la + petite branche non nulle
 */
{
	double valeur, current;
	if(debut == NULL) return 0;
	valeur = 1e50;
	if(debut->v1 != parent) {
		if(debut->l1 < 0) debut->l1 = 0;
		current = debut->l1;
		if(current < valeur && current > 0) valeur = current;
		current = proc_null_neg_branches(debut->v1, debut);
		if(current < valeur && current > 0) valeur = current;
	}
	if(debut->v2 != parent) {
		if(debut->l2 < 0) debut->l2 = 0;
		current = debut->l2;
		if(current < valeur && current > 0) valeur = current;
		current = proc_null_neg_branches(debut->v2, debut);
		if(current < valeur && current > 0) valeur = current;
	}
	if(debut->v3 != parent) {
		if(debut->l3 < 0) debut->l3 = 0;
		current = debut->l3;
		if(current < valeur && current > 0) valeur = current;
		current = proc_null_neg_branches(debut->v3, debut);
		if(current < valeur && current > 0) valeur = current;
	}
	return valeur;
}


int set_angles_noeuds(bignoeud *debut, bignoeud *parent, double delta,
					  double *p_current_angle, double rayon)
{
	int feuille = FALSE;
	int poids, poids1, poids2;
	double angle1 = -1, angle2 = -1;
	static int totfeuilles=0;
	
	if(debut == NULL) return 0;
	if(debut->v1 != parent) {
		poids = set_angles_noeuds(debut->v1, debut, delta, p_current_angle, rayon);
		if(debut->v1 == NULL) feuille = TRUE;
		else if (parent != NULL) {
			angle1 = debut->v1->position.angle;
			poids1 = poids;
		}
	}
	if(debut->v2 != parent) {
		poids = set_angles_noeuds(debut->v2, debut, delta, p_current_angle, rayon);
		if(debut->v2 == NULL) feuille = TRUE;
		else if (parent != NULL) {
			if ( angle1 == -1 ) {
				angle1 = debut->v2->position.angle;
				poids1 = poids;
			}
			else	{
				angle2 = debut->v2->position.angle;
				poids2 = poids;
			}
		}
	}
	if(debut->v3 != parent) {
		poids = set_angles_noeuds(debut->v3, debut, delta, p_current_angle, rayon);
		if(debut->v3 == NULL) feuille = TRUE;
		else if (parent != NULL) {
			angle2 = debut->v3->position.angle;
			poids2 = poids;
		}
	}
	if( feuille ) { totfeuilles++;
		debut->position.angle = *p_current_angle;
		*p_current_angle += delta;
		poids = 1;
	}
	else if(parent != NULL)	{ /* faire angle moyen modulo 2.pi */
		debut->position.angle = (poids1*angle1 + poids2*angle2)/(poids1+poids2);
		poids = poids1 + poids2;
		if( angle1 > angle2 )
			debut->position.angle -= M_PI;
	}
	debut->position.r = rayon;
	calc_cartesienne(&(debut->position));
	return poids;
}


void calc_cartesienne( cp_point *p)
{
	p->x = p->r * cos(p->angle);
	p->y = p->r * sin(p->angle);
	return;
}


void calc_polaire( cp_point *p)
{
	p->r = sqrt(p->x * p->x + p->y * p->y);
	if( p->x == 0 ) {
		if( p->y == 0) p->angle = 0;
		else if (p->y > 0) p->angle = M_PI_2;
		else 	p->angle = 3*M_PI_2;
	}
	else if (p->x > 0) {
		p->angle = atan( p->y / p->x );
		if( p->angle < 0 ) p->angle += 2*M_PI;
	}
	else	{
		p->angle = M_PI - atan( p->y / p->x );
	}
	return;
}


cp_point calc_point_direction( cp_point *depart, cp_point *direction, 
							  double longueur, double mini_br_length)
{
	static cp_point retour;
	double lac, eps, tmp1, tmp2;
	
	tmp1 = direction->x - depart->x;
	tmp2 = direction->y - depart->y;
	lac = sqrt( tmp1*tmp1 + tmp2*tmp2 );
	/* on remplace les branches nulles par des branches tres courtes pour que le
	 calcul de l'angle en double soit bon mais que le dessin en entier soit le
	 meme
	 */
	if(longueur == 0) longueur = mini_br_length;
	eps = longueur / lac;
	retour.x = depart->x + eps * tmp1;
	retour.y = depart->y + eps * tmp2;
	calc_polaire(&retour);
	return retour;
}


unrooted_branch *calc_position_noeuds(bignoeud *debut, bignoeud *parent,
									  unrooted_branch *curr_branche, double mini_br_length)
{
	if(debut->v1 != parent && debut->v1 != NULL ) {
		debut->v1->position = calc_point_direction( &(debut->position), &(debut->v1->position), debut->l1, mini_br_length);
		curr_branche = calc_position_noeuds(debut->v1, debut, curr_branche, mini_br_length);
		mem_line(&debut->position, &debut->v1->position, debut->v1, 
				 curr_branche);
		curr_branche++;
	}
	if(debut->v2 != parent && debut->v2 != NULL ) {
		debut->v2->position = calc_point_direction( &(debut->position), &(debut->v2->position), debut->l2, mini_br_length);
		curr_branche = calc_position_noeuds(debut->v2, debut, curr_branche, mini_br_length);
		mem_line(&debut->position, &debut->v2->position, debut->v2, 
				 curr_branche);
		curr_branche++;
	}
	if(debut->v3 != parent && debut->v3 != NULL ) {
		debut->v3->position = calc_point_direction( &(debut->position), &(debut->v3->position), debut->l3, mini_br_length);
		curr_branche = calc_position_noeuds(debut->v3, debut, curr_branche, mini_br_length);
		mem_line(&debut->position, &debut->v3->position, debut->v3, 
				 curr_branche);
		curr_branche++;
	}
	return curr_branche;
}


void mem_line(cp_point *debut, cp_point *fin, bignoeud *noeud_term, unrooted_branch *br)
{
  br->debut = *debut;
  br->fin = *fin;
  br->rank = noeud_term->rank;
  br->nom = noeud_term->nom ? strdup(noeud_term->nom) : NULL;
}


void free_bignoeud(bignoeud *debut, bignoeud *parent)
{
	if(debut == NULL) return;
	if(debut->v1 != parent) {
		free_bignoeud(debut->v1, debut);
	}
	if(debut->v2 != parent) {
		free_bignoeud(debut->v2, debut);
	}
	if(debut->v3 != parent) {
		free_bignoeud(debut->v3, debut);
	}
	//	no need to free debut->nom because done by free(data->tabtax[i]->nom)
	free(debut);
}


int chg_phys(FD_unrooted *ob, hits_field *hits )
{
	int encore = FALSE;
	const float delta = 0.05;
	int value;
	static int lr = 0, tb = 0, oldvaluex = 0, oldvaluey = 0;
	
	if(hits->left == 1 && hits->right == 0) {
		if(oldvaluex == 0) value = (int)((ob->phys_max_x - ob->phys_min_x) * delta); 
		else value = oldvaluex;
		if(value < 2) value = 2;
		if(lr == 2) { /* alternance left/right */
			if(oldvaluex <= 2) {
				hits->right = 1;
				return chg_phys(ob, hits);
			}
			value = oldvaluex/2; 
		}
		lr = 1;
		oldvaluex = value;
		ob->phys_min_x += value;
		ob->phys_max_x += value;
		if(ob->phys_max_x <= ob->phys_min_x) {
			ob->phys_max_x = ob->phys_min_x + 1;
			return FALSE;
		}
		encore = TRUE;
	}
	else if(hits->left == 0 && hits->right == 1) {
		if(oldvaluex == 0) value = (int)((ob->phys_max_x - ob->phys_min_x) * delta); 
		else value = oldvaluex;
		if(value < 2) value = 2;
		if(lr == 1) { /* alternance left/right */
			if(oldvaluex <= 2) {
				hits->left = 1;
				return chg_phys(ob, hits);
			}
			value = oldvaluex/2; 
		}
		lr = 2;
		oldvaluex = value;
		ob->phys_max_x -= value;
		ob->phys_min_x -= value;
		if(ob->phys_max_x <= ob->phys_min_x) {
			ob->phys_max_x = ob->phys_min_x + 1;
			return FALSE;
		}
		encore = TRUE;
	}
	else if(hits->left == 1 && hits->right == 1) {
		lr = tb = 0;
		oldvaluex = 0;
		value = (int)((ob->phys_max_x - ob->phys_min_x) * delta / 2); 
		if(value < 2) value = 2;
		ob->phys_min_x += value;
		ob->phys_max_x -= value;
		if(ob->phys_max_x <= ob->phys_min_x) {
			ob->phys_max_x = ob->phys_min_x + 1;
			return FALSE;
		}
		encore = TRUE;
		ob->phys_max_y = ob->phys_min_y + (ob->phys_max_x - ob->phys_min_x);
	}
	
	if(hits->top == 0 && hits->bottom == 1) {
		if(oldvaluey == 0) value = (int)((ob->phys_max_y - ob->phys_min_y) * delta); 
		else value = oldvaluey;
		if(value < 2) value = 2;
		if(tb == 2) {
			if(oldvaluey <= 2) {
				hits->top = 1;
				return chg_phys(ob, hits);
			}
			value = oldvaluey/2; 
		}
		tb = 1;
		oldvaluey = value;
		ob->phys_max_y -= value;
		ob->phys_min_y -= value;
		if(ob->phys_max_y <= ob->phys_min_y) {
			ob->phys_max_y = ob->phys_min_y + 1;
			return FALSE;
		}
		encore = TRUE;
	}
	else if(hits->top == 1 && hits->bottom == 0) {
		if(oldvaluey == 0) value = (int)((ob->phys_max_y - ob->phys_min_y) * delta); 
		else value = oldvaluey;
		if(value < 2) value = 2;
		if(tb == 1) {
			if(oldvaluey <= 2) {
				hits->bottom = 1;
				return chg_phys(ob, hits);
			}
			value = oldvaluey/2; 
		}
		tb = 2;
		oldvaluey = value;
		ob->phys_min_y += value;
		ob->phys_max_y += value;
		if(ob->phys_max_y <= ob->phys_min_y) {
			ob->phys_max_y = ob->phys_min_y + 1;
			return FALSE;
		}
		encore = TRUE;
	}
	else if(hits->top == 1 && hits->bottom == 1) {
		value = (int)((ob->phys_max_y - ob->phys_min_y) * delta/2); 
		if(value < 2) value = 2;
		lr = tb = 0;
		oldvaluey = 0;
		ob->phys_min_y += value;
		ob->phys_max_y -= value;
		if(ob->phys_max_y <= ob->phys_min_y) {
			ob->phys_max_y = ob->phys_min_y + 1;
			return FALSE;
		}
		encore = TRUE;
		ob->phys_max_x = ob->phys_min_x + (ob->phys_max_y - ob->phys_min_y);
	}
	
	return encore;
}


void log_to_phys(FD_unrooted *data, cp_point *log_pos, cp_point *phys_pos)
{
	double factorx, factory;
	
	factorx = (data->phys_max_x - data->phys_min_x) / (data->log_max_x - data->log_min_x);
	factory = (data->phys_max_y - data->phys_min_y) / (data->log_max_y - data->log_min_y);
	phys_pos->x = factorx * ( log_pos->x - data->log_min_x ) + data->phys_min_x;
	phys_pos->y = factory * ( log_pos->y - data->log_min_y ) + data->phys_min_y;
}


double length_log_phys(FD_unrooted *data, double p)
{
	double factor;
	factor = (data->phys_max_x - data->phys_min_x) / (data->log_max_x - data->log_min_x);
	return p * factor;
}

double length_phys_log(FD_unrooted *data, double p)
{
	double factor;
	factor = (data->phys_max_x - data->phys_min_x) / (data->log_max_x - data->log_min_x);
	return  p / factor;
}


double calc_echelle(FD_unrooted *data, int larg)
{ /* rend taille logique pour echelle optimale */
	double log_val, phys_val;
	phys_val = larg/10;
	log_val = length_phys_log(data, phys_val);
	log_val = arrondi_echelle(log_val);
	return log_val;
}

