#include "matpt.h"
#include "pdf_or_ps.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#ifndef NO_PDF
#include <pdflib.h>
#endif


#define MAXSEQLENGTH 10000 /* pas utilise pour interface avec seaview */
#define MAXSEGMENTS 20000
#define segment_x_ou_y(num, offset) \
			(*(int *)(((char *)(segments + num)) + offset))
/* globals */
struct segment {
	int x, y, longueur;
	};
static struct segment segments[MAXSEGMENTS+1];
gap_site gap_sites[MAX_GAP_SITES];
extern Fl_Paged_Device::Page_Format printout_pageformat;

/* prototypes */
void show_dot_plot(char *seq1, char *seq2, char *seqname1, char *seqname2,
	int l1, int l2, int maxseqlength, void *seaview_data);
FD_matpt *create_form_matpt(void);
static void err_message(const char *text);
static char *check_alloc(int nbrelt, int sizelt);
void log_to_phys(int lx, int ly, int *px, int *py, FD_matpt *matpt);
void phys_to_log(int px, int py, int *lx, int *ly, FD_matpt *matpt);
void reset_mat_data(FD_matpt *matpt);
void reset_sliders_data(FD_matpt * matpt);
void init_mat_data(FD_matpt * matpt, int seqlong1, int seqlong2,
	char *seqname1, char *seqname2, char *seq1, char *seq2, char *name,
	int maxseqlength);
void set_size_data(FD_matpt *matpt);
void compute_region_text(FD_matpt *matpt);
void draw_rect(FD_matpt *matpt);
int mat_handle_proc(Fl_Widget *ob, int event, float x, float y, 
	char key);
void center_view_on_hit(FD_matpt *matpt);
void magnify_reduce_proc(Fl_Widget *ob, void *extra);
void change_comput_params(Fl_Widget *ob, void *data);
void compute_proc(Fl_Widget *ob, void *data);
void interrupt_callback(Fl_Widget *ob, void *data);
void ps_draw_line(int x1, int y1, int x2, int y2, FD_matpt *matpt);
void ps_set_mat_data(FD_matpt *matpt);
void plot_button_proc(Fl_Widget *ob, void *data);
int really_close(FD_matpt *matpt);
void exit_button_proc(Fl_Widget *ob, FD_matpt *data);
void move_mat_proc(Fl_Widget *ob, void *data);
void move_box_step(Fl_Widget *ob, void *extra);
void region_size_proc(Fl_Widget *ob, void *data);
char *readseq(FILE *infile, int *slong, char **seqname);
char *readseqfile(char *fname, int *seqlong1, int *seqlong2,
	char **name1, char **name2, char **seq1, char **seq2);
void compute_diags(FD_matpt *matpt);
void compute_title(FD_matpt *matpt);
int find_next_gap_site(int pos, gap_site *gap_sites, int tot_gap_sites);
void new_gap_site(int pos, int l, gap_site *gap_sites, int *p_tot_gap_sites,
	int seqrank);
int delete_gaps_bef_pos(char *seq, int pos, int number, int length, 
	int totsegments, int offset);
int insert_gaps_bef_pos(char *seq, int pos, int number, int length, 
	int totsegments, int offset, int maxlen);
int dispatch_gaps_bef(int pos, int number, char *seq, gap_site * gap_sites, 
	int *p_tot_gap_sites, int offset, int seqlen, int totsegments,
	int seqrank, char *other_seq, int *other_seqlen, int other_offset,
	int maxlen);
void align_callback(Fl_Widget *ob, void *data);
void record_alignment_callback(Fl_Widget *ob, void *data);
extern int insert_gaps_at(SEA_VIEW *view, int seq, int site, int total);
extern void update_current_seq_length(int newlength, SEA_VIEW *view);
void update_col_lines(int num, SEA_VIEW *view);
void matpt_idle_cb(FD_matpt *matpt);

class matpt_panel : public Fl_Widget {
    FL_EXPORT void draw(void);
    FL_EXPORT int handle(int);
public:
    FL_EXPORT matpt_panel(int x,int y,int w,int h, void *data) :
		Fl_Widget(x,y,w,h,NULL) {
	this->user_data(data);
	}
};


void my_watch_cursor(Fl_Window *win)
{
if (win) win->default_cursor(FL_CURSOR_WAIT);
}

void fl_reset_cursor(Fl_Window *win)
{
if (win) win->default_cursor(FL_CURSOR_DEFAULT); 
}


void show_dot_plot(char *seq1, char *seq2, char *seqname1, char *seqname2,
	int l1, int l2, int maxseqlength, void *seaview_data)
{
static int first = TRUE;
static FD_matpt *matpt;
char *new_seq1, *new_seq2;
static char *old_seq1, *old_seq2;
SEA_VIEW *view = (SEA_VIEW *)seaview_data;
if(first) {
	first = FALSE;
   	matpt = create_form_matpt();
	matpt->form->callback((Fl_Callback*)exit_button_proc, matpt);
	matpt->seq1 = matpt->seq2 = NULL;
	matpt->seaview_data = seaview_data;
	old_seq1 = NULL; old_seq2 = NULL;
	if(view->protein) {
		((Fl_Input *)matpt->win_size_type_box)->value("10");
		((Fl_Input *)matpt->ident_type_box)->value("5");
		}
	}
if(matpt->seq1 == NULL || seq1 != old_seq1 || seq2 != old_seq2) {
	if(matpt->seq1 != NULL) {
		free(matpt->seq1+1);
		free(matpt->seq2+1);
		}
	new_seq1 = (char *)malloc(maxseqlength+1); 
	memcpy(new_seq1, seq1, l1 + 1);
	new_seq2 = (char *)malloc(maxseqlength+1); 
	memcpy(new_seq2, seq2, l2 + 1);
	init_mat_data(matpt, l1, l2, seqname1, seqname2, 
		new_seq1 - 1, new_seq2 - 1, view->masename, maxseqlength);
	set_size_data(matpt);
	old_seq1 = seq1; old_seq2 = seq2;
	}
  compute_region_text(matpt);
  ((Fl_Slider*)(matpt->region_slider))->value(matpt->region_size);
matpt->form->show();
#ifndef MICRO
	matpt->form->hotspot(matpt->form);
#endif
compute_title(matpt);
}


static Fl_Widget *cre_button(int x, int y, int *w, int h, 
	int si, const char *label)
{
Fl_Widget *obj;
fl_font(FL_HELVETICA, si);
*w = (int)fl_width(label) + 4 * 2;
obj = new Fl_Button(x, y, *w, h, label);
obj->labelsize(si);
return obj;
}

FD_matpt *create_form_matpt(void)
{
Fl_Widget *obj;
FD_matpt *fdui = (FD_matpt *) check_alloc(1, sizeof(*fdui));
static char def_title[] = "Dot Plot";
Fl_Window *form;
int curr_x, curr_y, fin, width, fontsize, but_height;
user_data_plus *data;

fdui->title = def_title;

#if defined(WIN32) || defined(__APPLE__)
fontsize = 10;
fdui->form = form = new Fl_Window(560, 540);
#else
fontsize = 12;
fdui->form = form = new Fl_Window(680, 815);
#endif
form->box(FL_FLAT_BOX);
form->label("Dot Plot");

curr_y  = fin = 5;
but_height = fontsize + 4 * 2;
Fl_Group *top = new Fl_Group(fin, curr_y, form->w() - 2 * fin, but_height);
obj = cre_button(fin, curr_y, &width, but_height, fontsize, "Fit to window");
fin += width + 2;
data = new user_data_plus;
data->p = fdui;
data->value = 1;
obj->callback(magnify_reduce_proc, data);

obj = cre_button(fin, curr_y, &width, but_height, fontsize, "Reduce");
fin += width + 2;
data = new user_data_plus;
data->p = fdui;
data->value = 2;
obj->callback(magnify_reduce_proc, data);

Fl_Widget *magnify = cre_button(fin, curr_y, &width, but_height, fontsize, "Magnify");
fin += width + 2;
data = new user_data_plus;
data->p = fdui;
data->value = 3;
magnify->callback(magnify_reduce_proc, data);

curr_x = fin;
Fl_Widget *compute = cre_button(fin, curr_y, &width, but_height, fontsize, "Compute");
fin += width + 2;
compute->callback(compute_proc, fdui);
fdui->compute_butt = compute;

Fl_Widget *postscript = cre_button(fin, curr_y, &width, but_height, fontsize, "Write "PDF_OR_PS);
fin += width;
postscript->callback(plot_button_proc, fdui);

#ifndef WIN32
fin += 2;
Fl_Widget *close = cre_button(fin, curr_y, &width, but_height, fontsize, "Close");
((Fl_Button *)close)->shortcut(FL_COMMAND | 'w');
fin += width;
close->callback((Fl_Callback*)exit_button_proc, fdui);
#endif

width = fin - curr_x;
width = top->x() + top->w() - width;
width = width - curr_x;
compute->position(compute->x() + width, compute->y() );
postscript->position(postscript->x() + width, postscript->y() );
#ifndef WIN32
close->position(close->x() + width, close->y() );
#endif

static char wsize_label[] = "Window size:";
fl_font(FL_HELVETICA, fontsize);
width = (int)fl_width(wsize_label) + 2;
curr_x = magnify->x() + magnify->w() + 2;
obj = new Fl_Input(curr_x + width, curr_y, 30, but_height, wsize_label);
obj->type(FL_INT_INPUT);
fin = obj->x() + obj->w() + 2;
obj->labelsize(fontsize); ((Fl_Input *)obj)->textsize(fontsize);
obj->callback(change_comput_params, fdui);
((Fl_Input *)obj)->value("20");
fdui->win_size_type_box = obj;

static char wmatch_label[] = "matches/window:";
fl_font(FL_HELVETICA, fontsize);
width = (int)fl_width(wmatch_label) + 2;
obj = new Fl_Input(fin + width, curr_y, 30, but_height, wmatch_label);
obj->type(FL_INT_INPUT);
fin = obj->x() + obj->w();
obj->labelsize(fontsize); ((Fl_Input *)obj)->textsize(fontsize);
obj->callback(change_comput_params, fdui);
((Fl_Input *)obj)->value("13");
fdui->ident_type_box = obj;

curr_x = magnify->x() + magnify->w() + 1;
width = compute->x() - curr_x - 1;
top->add_resizable(*new Fl_Box(curr_x, curr_y, width, but_height) );
top->end();

curr_y += but_height + 2;
Fl_Group *align_group = new Fl_Group(top->x(), curr_y, top->w(), but_height);
fin = align_group->x();

obj = cre_button(fin, curr_y, &width, but_height, fontsize, "Align");
fin += width + 2;
obj->callback(align_callback, fdui);
fdui->align_button = obj;

obj = cre_button(fin, curr_y, &width, but_height, fontsize, "Record alignment");
fin += width + 5;
obj->callback(record_alignment_callback, fdui);

static char ref_choice_label[] = "Ref. sequence";
fl_font(FL_HELVETICA, fontsize);
width = (int)fl_width(ref_choice_label) + 4 * 2;
obj = new Fl_Choice(fin + width, curr_y, 150, but_height, ref_choice_label);
obj->type(0);
obj->labelfont(FL_HELVETICA); obj->labelsize(fontsize);
((Fl_Choice *)obj)->textfont(FL_HELVETICA); ((Fl_Choice *)obj)->textsize(fontsize);
fdui->choice_ref_seq = obj;

obj = cre_button(0, curr_y, &width, but_height, fontsize, "Interrupt");
curr_x = align_group->x() + align_group->w() - width;
obj->position(curr_x, obj->y());
obj->callback(interrupt_callback, &(fdui->interrupted));
fdui->interrupt_butt = obj;

curr_x = fdui->choice_ref_seq->x() + fdui->choice_ref_seq->w() + 1;
width = fdui->interrupt_butt->x() - curr_x - 1;
align_group->add_resizable(*new Fl_Box(curr_x, curr_y, width, but_height) );
align_group->end();

curr_y += but_height + 2;
Fl_Group *region_group = new Fl_Group(top->x(), curr_y, top->w(), 80);

#ifdef WIN32
#define SQW 15
#else
#define SQW 20
#endif
fin = region_group->x();
obj = new Fl_Repeat_Button(fin,curr_y,SQW,SQW,"@<-");
obj->labeltype(FL_SYMBOL_LABEL);
obj->box(FL_ENGRAVED_BOX);
data = new user_data_plus;
data->p = fdui;
data->value = 1;
obj->callback(move_box_step, data);
obj = new Fl_Repeat_Button(fin + SQW,curr_y,SQW,SQW,"@->");
obj->labeltype(FL_SYMBOL_LABEL);
obj->box(FL_ENGRAVED_BOX);
data = new user_data_plus;
data->p = fdui;
data->value = 2;
obj->callback(move_box_step, data);
curr_y += SQW;
obj = new Fl_Repeat_Button(fin,curr_y,SQW,SQW,"@1->");
obj->labeltype(FL_SYMBOL_LABEL);
obj->box(FL_ENGRAVED_BOX);
data = new user_data_plus;
data->p = fdui;
data->value = 3;
obj->callback(move_box_step, data);
obj = new Fl_Repeat_Button(fin + SQW,curr_y,SQW,SQW,"@9->");
obj->labeltype(FL_SYMBOL_LABEL);
obj->box(FL_ENGRAVED_BOX);
data = new user_data_plus;
data->p = fdui;
data->value = 4;
obj->callback(move_box_step, data);
curr_y += SQW;
obj = new Fl_Repeat_Button(fin,curr_y,SQW,SQW,"@2->");
obj->labeltype(FL_SYMBOL_LABEL);
obj->box(FL_ENGRAVED_BOX);
data = new user_data_plus;
data->p = fdui;
data->value = 5;
obj->callback(move_box_step, data);
obj = new Fl_Repeat_Button(fin + SQW,curr_y,SQW,SQW,"@8->");
obj->labeltype(FL_SYMBOL_LABEL);
obj->box(FL_ENGRAVED_BOX);
data = new user_data_plus;
data->p = fdui;
data->value = 6;
obj->callback(move_box_step, data);

curr_x = fin + 2*SQW + 3;
curr_y -= 2 * SQW;
width = form->w() - curr_x - 3;
strcpy(fdui->regionboxtext, "");
obj = new Fl_Box(FL_ENGRAVED_BOX,curr_x,curr_y,width,3*SQW,fdui->regionboxtext);
obj->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
obj->labelsize(fontsize);
obj->labelfont(FL_COURIER);
fdui->region_box = obj;

fl_font(fdui->region_box->labelfont(), fdui->region_box->labelsize() );
int debut = 4 + 10 * (int)fl_width("A");
int maxbound = (fdui->region_box->w() - 8) / (int)fl_width("A") - 6;
obj = new Fl_Slider(FL_HOR_FILL_SLIDER, fdui->region_box->x() + debut,
	fdui->region_box->y() + fdui->region_box->h(),
	fdui->region_box->w() - debut, SQW, "");
obj->callback(region_size_proc, fdui);
((Fl_Slider *)obj)->when((Fl_When)(FL_WHEN_CHANGED | FL_WHEN_RELEASE));
((Fl_Slider *)obj)->bounds(4, maxbound);
fdui->region_slider = obj;

region_group->resizable(fdui->region_slider);
region_group->end();

curr_y += fdui->region_box->h() + fdui->region_slider->h() + 5;
Fl_Group *panel_group = new Fl_Group(top->x(), curr_y, top->w(), form->h() - curr_y - 5);

obj = new Fl_Slider(FL_VERT_SLIDER, panel_group->x() + panel_group->w() - SQW,
	panel_group->y(), SQW, panel_group->h() - SQW - 5, "");
data = new user_data_plus;
data->value = 1;
data->p = fdui;
obj->callback(move_mat_proc, data);
((Fl_Slider*)obj)->when((Fl_When)(FL_WHEN_CHANGED | FL_WHEN_RELEASE));
fdui->y_slider = obj;

obj = new Fl_Slider(FL_HOR_SLIDER, panel_group->x(),
	panel_group->y() + panel_group->h() - SQW, panel_group->w() - SQW -5, SQW, "");
data = new user_data_plus;
data->value = 2;
data->p = fdui;
obj->callback(move_mat_proc, data);
((Fl_Slider*)obj)->when((Fl_When)(FL_WHEN_CHANGED | FL_WHEN_RELEASE));
fdui->x_slider = obj;

/* mat_panel */
obj = new Fl_Box(FL_ENGRAVED_FRAME, panel_group->x()-3, panel_group->y()-3,
	fdui->x_slider->w()+6, fdui->y_slider->h()+6, "");
obj = new matpt_panel(panel_group->x(), panel_group->y(),
	fdui->x_slider->w(), fdui->y_slider->h(), fdui);

obj->color(FL_BLACK, FL_GRAY);
fdui->mat_panel = obj;
fdui->gap_sites = gap_sites;

panel_group->resizable(fdui->mat_panel);
panel_group->end();

form->end();
form->resizable(panel_group);
return fdui;
}


static void err_message(const char *text)
{
fl_alert("%s", text);
exit(1);
}

static char *check_alloc(int nbrelt, int sizelt)
{
char *retval;
if( (retval = (char *)calloc(nbrelt,sizelt)) != NULL ) return retval;
err_message("ERROR: I cannot obtain enough memory.");
return NULL;
}


void log_to_phys(int lx, int ly, int *px, int *py, FD_matpt *matpt)
{
*px = (int)floor(lx * matpt->factor + matpt->kx + 0.5);
*py = (int)floor( - ly * matpt->factor + matpt->ky + 0.5);
}


void phys_to_log(int px, int py, int *lx, int *ly, FD_matpt *matpt)
{
*lx = (int)floor( (px - matpt->kx) / matpt->factor + 0.5 );
*ly = (int)floor( (matpt->ky - py) / matpt->factor + 0.5 );
}


void reset_mat_data(FD_matpt *matpt)
{
int a, b, c, d;
matpt->factor = ((double) matpt->phys_width) / matpt->view_size;
a = matpt->view_x; b = a + matpt->view_size; 
c = matpt->mat_panel->x() + matpt->margin; d = c + matpt->phys_width;
matpt->kx = (b*c - a*d) / (double) matpt->view_size;
a = matpt->view_y; b = a + matpt->view_size; 
c = matpt->mat_panel->y() + matpt->mat_panel->h() - matpt->margin; 
d = c - matpt->phys_width;
matpt->ky = (b*c - a*d) / (double) matpt->view_size;
}


void reset_sliders_data(FD_matpt * matpt)
{
((Fl_Slider *)matpt->x_slider)->bounds(0, matpt->longmax - matpt->view_size);
((Fl_Slider *)matpt->x_slider)->slider_size(matpt->view_size / (double)matpt->longmax);
((Fl_Slider *)matpt->x_slider)->step(1);
((Fl_Slider *)matpt->y_slider)->bounds( - (matpt->longmax - matpt->view_size), 0);
((Fl_Slider *)matpt->y_slider)->slider_size(matpt->view_size / (double)matpt->longmax);
((Fl_Slider *)matpt->y_slider)->step(1);
((Fl_Slider *)matpt->x_slider)->value(matpt->view_x);
((Fl_Slider *)matpt->y_slider)->value(- matpt->view_y);
}


void init_mat_data(FD_matpt * matpt, int seqlong1, int seqlong2,
	char *seqname1, char *seqname2, char *seq1, char *seq2, char *name,
	int maxseqlength)
{
const int margin = 5;
int fenetre, identites;

matpt->rect_only = FALSE;
matpt->longmax = FL_max(seqlong1, seqlong2);
matpt->margin = margin;
matpt->seqlong1 = seqlong1;
matpt->seqlong2 = seqlong2;
matpt->seq1 = seq1;
matpt->seq2 = seq2;
matpt->seqname1 = seqname1;
matpt->seqname2 = seqname2;
matpt->view_x = matpt->view_y = 0;
matpt->view_size = matpt->longmax;
sscanf( ((Fl_Input *)matpt->win_size_type_box)->value(), "%d", &fenetre);
matpt->fenetre = fenetre;
sscanf( ((Fl_Input *)matpt->ident_type_box)->value(), "%d", &identites);
matpt->identites = identites;
matpt->region_size = fenetre;
matpt->hitx = seqlong1/2;
matpt->hity = seqlong2/2;
matpt->totsegments = -1;
matpt->need_compute = FALSE;
matpt->modif_but_not_saved = FALSE;
matpt->plotname = (char *)malloc(strlen(name) + 1);
strcpy(matpt->plotname, name);
if(matpt->align_button != NULL) {
	char temp[200], name1[40], name2[40], *p;
	sprintf(name1, "%.30s", seqname1);
	if((p = strchr(name1,'|')) != NULL) *p = 0;
	sprintf(name2, "%.30s", seqname2);
	if((p = strchr(name2,'|')) != NULL) *p = 0;
	sprintf(temp, "align to %s|align to %s", name1, name2);
	((Fl_Choice*)matpt->choice_ref_seq)->clear();
	((Fl_Choice *)matpt->choice_ref_seq)->value(0);
	((Fl_Choice *)matpt->choice_ref_seq)->add(temp);
	((Fl_Choice *)matpt->choice_ref_seq)->mode(0, FL_MENU_RADIO);
	((Fl_Choice *)matpt->choice_ref_seq)->mode(1, FL_MENU_RADIO);
	matpt->tot_gap_sites = 0;
	matpt->maxseqlength = maxseqlength;
	}
}


void set_size_data(FD_matpt *matpt)
{
matpt->phys_width = FL_min(matpt->mat_panel->w(), matpt->mat_panel->h()) - 
	2 * matpt->margin;
reset_mat_data(matpt);
reset_sliders_data(matpt);
}


void compute_region_text(FD_matpt *matpt)
{
int i, matches=0, l;
char *p;
static char newtext[sizeof(matpt->regionboxtext)];

if(matpt->longmax == 0) return;
p = newtext;
sprintf(p, "%5d ", matpt->hitx);
l = FL_min(matpt->region_size, matpt->seqlong1 - matpt->hitx + 1);
l = FL_min(l, matpt->seqlong2 - matpt->hity + 1);
memcpy(p+6, (matpt->seq1 + matpt->hitx), l);
*(p + l + 6) = '\n';
p += l + 7;

strcpy(p, "      ");
for(i=0; i < l; i++) {
	if( toupper(*(matpt->seq1 + matpt->hitx + i)) ==
		 toupper(*(matpt->seq2 + matpt->hity + i))  &&
		 *(matpt->seq2 + matpt->hity + i) != '-' ) {
		*(p + i + 6) = '|';
		matches++;
		}
	else
		*(p + i + 6)=' ';
	}
sprintf(p,"%d/%d", matches, matpt->region_size);
*(p + strlen(p)) = ' ';
*(p + 6 + l)='\n';
p += l + 7;

sprintf(p, "%5d ", matpt->hity);
memcpy(p+6, (matpt->seq2 + matpt->hity), l);
*(p + l + 6)='\0';
if(strcmp(matpt->regionboxtext, newtext) != 0) {
	strcpy(matpt->regionboxtext, newtext);
	matpt->region_box->redraw();
	}
}


void draw_rect(FD_matpt *matpt)
{
int px, py, pxf, pyf;
rectangle old_rect;
log_to_phys(matpt->hitx, matpt->hity, &px, &py, matpt);
log_to_phys(matpt->hitx + matpt->region_size, matpt->hity + matpt->region_size, 
	&pxf, &pyf, matpt);
// rectangle mis pour avoir h > 0
old_rect.x = px; old_rect.y = pyf; old_rect.w = pxf - px; old_rect.h = py - pyf;
  if (old_rect.x + old_rect.w > 0 && old_rect.y + old_rect.h > 0 && 
      old_rect.y <= matpt->form->h() && old_rect.x <= matpt->form->w()) {
      fl_overlay_rect(old_rect.x, old_rect.y, old_rect.w, old_rect.h);
    }
fl_color(FL_BLACK);
matpt->rect_only = FALSE;
}


void matpt_panel::draw(void)
{
FD_matpt *matpt = (FD_matpt *)this->user_data();
int i, pxd, pyd, pxf, pyf;
static int old_w = 0, old_h = 0;
	this->clear_damage(); // tres important!
	if( this->w() != old_w || this->h() != old_h ) { /* was resized */
		old_w = this->w(); old_h = this->h();
		set_size_data(matpt);
		((Fl_Slider *)matpt->region_slider)->value(matpt->region_size);
		}
	fl_push_clip(this->x(), this->y(), this->w(), this->h());
	if(matpt->rect_only) goto label_draw_rect;
	fl_overlay_clear();
/* ecriture du fond */
	fl_color(this->selection_color());
	fl_rectf(this->x(), this->y(), this->w(), this->h());
	fl_color(FL_BLACK);
/* draw the diagonal line */
	if( ( (SEA_VIEW *) matpt->seaview_data )->numb_gc > 1) {
/* in red on color displays */
		log_to_phys(0, 0, &pxd, &pyd, matpt);
		i = FL_min(matpt->seqlong1, matpt->seqlong2);
		log_to_phys(i, i, &pxf, &pyf, matpt);
		fl_color(FL_RED);
		fl_line(pxd, pyd, pxf, pyf);
		fl_color(FL_BLACK);
		}
	else	{
/* as a dotted line on B&W displays */
		int p, q;
		log_to_phys(0, 0, &pxd, &pyd, matpt);
		i = FL_min(matpt->seqlong1, matpt->seqlong2);
		log_to_phys(i, i, &pxf, &pyf, matpt);
		q = pyd;
		for(p=pxd; p<=pxf; p += 2) {
			fl_line(p, q, p, q);
			q -= 2;
			}
		}
	log_to_phys(0, 0, &pxd, &pyd, matpt);
	log_to_phys(matpt->seqlong1, 0, &pxf, &pyf, matpt);
	fl_line(pxd, pyd, pxf, pyf);
	log_to_phys(0, matpt->seqlong2, &pxf, &pyf, matpt);
	fl_line(pxd, pyd, pxf, pyf);
	if(matpt->need_compute) {
		compute_diags(matpt);
		fl_pop_clip();
		return;
		}
	else	{
		for (i = 0; i <= matpt->totsegments; i++) {
			log_to_phys(segments[i].x,
				segments[i].y, 
				&pxd, &pyd, matpt);
			log_to_phys(segments[i].x+segments[i].longueur, 
				segments[i].y+segments[i].longueur, 
				&pxf, &pyf, matpt);
			fl_line(pxd, pyd, pxf, pyf);
			}
		}
label_draw_rect:
	draw_rect(matpt);
	fl_pop_clip();
	compute_region_text(matpt);
	fl_reset_cursor(this->window());
}


int matpt_panel::handle(int event) 
{
FD_matpt *matpt = (FD_matpt *)this->user_data();

if(event == FL_PUSH) {
	int lx, ly;
	phys_to_log( Fl::event_x(), Fl::event_y(), &lx, &ly, matpt);
	if(lx <= 0 || ly <= 0 || lx > matpt->seqlong1 || ly > matpt->seqlong2)
		return 1;
	matpt->hitx = lx; matpt->hity = ly;
	matpt->rect_only = TRUE;
  matpt->mat_panel->redraw();
  compute_region_text(matpt);
	Fl::focus(this);
	return 1;
	}
else if(event == FL_KEYBOARD) {
	int key = Fl::event_key();
	if( key != FL_Right && key != FL_Left && 
		key != FL_Up && key != FL_Down ) return 0;	
	if(key == FL_Left) {
		matpt->hitx = FL_max(matpt->hitx - 5, 1);
		matpt->hity = FL_max(matpt->hity - 5, 1);
		}
	else if(key == FL_Right) {
		matpt->hitx = FL_min(matpt->hitx + 5, matpt->seqlong1);
		matpt->hity = FL_min(matpt->hity + 5, matpt->seqlong2);
		}
	else if(key == FL_Up) {
		matpt->hity = FL_min(matpt->hity + 5, matpt->seqlong2);
		}
	else 	{ /* FL_Down */
		matpt->hity = FL_max(matpt->hity - 5, 1);
		}
	matpt->rect_only = TRUE;
	matpt->mat_panel->redraw();
  compute_region_text(matpt);
	return 1;
	}
else if(event == FL_FOCUS) {
	return 1;
	}
return 0;
}


void center_view_on_hit(FD_matpt *matpt)
{
int newval;
newval = matpt->hitx + (matpt->region_size - matpt->view_size) / 2;
newval = FL_min( FL_max(newval, 0), matpt->longmax - matpt->view_size);
matpt->view_x = newval;
newval = matpt->hity + (matpt->region_size - matpt->view_size) / 2;
newval = FL_min( FL_max(newval, 0), matpt->longmax - matpt->view_size);
matpt->view_y = newval;
reset_mat_data(matpt);
reset_sliders_data(matpt);
}


void magnify_reduce_proc(Fl_Widget *ob, void *extra)
{
const int zoom = 2;
FD_matpt *matpt = (FD_matpt *)((user_data_plus *)extra)->p;
int data = ((user_data_plus *)extra)->value;

if( data == 3 ) { 	/* magnify */
	matpt->view_size /= zoom;
	}
else if ( data == 1 ) { /* fit to window */
	matpt->view_size = matpt->longmax;
	}
else	{ 		/* reduce */
	matpt->view_size = FL_min( matpt->view_size * zoom, matpt->longmax );
	}
center_view_on_hit(matpt);
matpt->mat_panel->redraw();
matpt->x_slider->redraw();
matpt->y_slider->redraw();
}


void change_comput_params(Fl_Widget *ob, void *data)
{
const char *p;
int value = -1;
FD_matpt *fdui = (FD_matpt *)data;

p= ((Fl_Input *)ob)->value();
sscanf(p,"%d",&value);
if(value<=0 || value>50) {
	((Fl_Input *)ob)->value("??");
	value = -1;
	}
if(ob == fdui->win_size_type_box)
	fdui->fenetre = value;
else
	fdui->identites = value;
if(fdui->identites > fdui->fenetre && fdui->fenetre>0) {
	char tmp[10];
	fdui->identites = fdui->fenetre;
	sprintf(tmp,"%d", fdui->identites);
	((Fl_Input *)fdui->ident_type_box)->value(tmp);
	}
}


void compute_proc(Fl_Widget *ob, void *data)
{
FD_matpt *fdui = (FD_matpt *)data;
change_comput_params(fdui->win_size_type_box, fdui);
change_comput_params(fdui->ident_type_box, fdui);
if(fdui->fenetre == -1 || fdui->identites == -1) return;
my_watch_cursor(fdui->mat_panel->window());
fdui->need_compute = TRUE;
fdui->interrupted = FALSE;
fdui->mat_panel->redraw();
  fdui->region_box->redraw();
if(!fdui->form->visible()) return;
if(fdui->fenetre > fdui->region_size) fdui->region_size = fdui->fenetre;
((Fl_Slider *)fdui->region_slider)->value(fdui->region_size);
fdui->region_slider->redraw();
}


void interrupt_callback(Fl_Widget *ob, void *data)
{
  *(int *)data = TRUE;
}


void ps_draw_line(int x1, int y1, int x2, int y2, FD_matpt *matpt)
{
int px, py, px2, py2;
px = (int)floor(x1 * matpt->factor + matpt->kx + 0.5);
py = (int)floor(y1 * matpt->factor + matpt->ky + 0.5);
px2 = (int)floor(x2 * matpt->factor + matpt->kx + 0.5);
py2 = (int)floor(y2 * matpt->factor + matpt->ky + 0.5);
  fl_line(px, py, px2, py2);
}

void ps_set_mat_data(FD_matpt *matpt)
{
int a, b, c, d;
matpt->factor = ((double) matpt->phys_width) / matpt->view_size;
a = matpt->view_x; b = a + matpt->view_size; 
c = matpt->margin; d = c + matpt->phys_width;
matpt->kx = (b*c - a*d) / (double) matpt->view_size;
a = matpt->view_y; b = a + matpt->view_size; 
c = matpt->margin; 
d = c + matpt->phys_width;
matpt->ky = (b*c - a*d) / (double) matpt->view_size;
}


void plot_button_proc(Fl_Widget *ob, void *data)
{
FD_matpt *matpt = (FD_matpt *)data;
int i;
char message[200];
int old_phys;
  
  Fl_PDF_or_PS_File_Device* surface = new Fl_PDF_or_PS_File_Device();
  if (surface->start_job(matpt->plotname, printout_pageformat)) return;
// we use bottom-up coords for PDF and PS output, it's rather tricky.  
#ifndef NO_PDF
jmp_buf *jbuf;
surface->surface_try(&jbuf);
if (setjmp(*jbuf) == 0) {
  PDF_set_parameter((PDF*)(surface->pdf()), "topdown", "false"); // set coords bottom-up
#endif
  surface->start_page();
#ifdef NO_PDF
  int w, h;
  surface->printable_rect(&w, &h);
  surface->scale(1, -1); // set coords bottom-up
  surface->origin(30, -h+200);
#else
  surface->origin(30, 200);
#endif
  fl_color(FL_BLACK);
  fl_line(-10,-10,510,-10);
  fl_line(510,-10,510,510);
  fl_line(-10,-10,-10,510);
  fl_line(-10,510,510,510);
  old_phys = matpt->phys_width; matpt->phys_width = 500;
  ps_set_mat_data(matpt);

  ps_draw_line(0, 0, matpt->seqlong1, 0, matpt);
  ps_draw_line(0, 0, 0, matpt->seqlong2, matpt);
  for (i = 0; i <= matpt->totsegments; i++)	{
	  ps_draw_line(segments[i].x, segments[i].y,
		  segments[i].x+segments[i].longueur,
		  segments[i].y+segments[i].longueur, matpt);

	  }
  matpt->phys_width = old_phys;
  reset_mat_data(matpt);

  sprintf(message,
	  "Horizontal: %.30s (%d)  Vertical: %.30s (%d)  Window: %d Matches: %d",
	  matpt->seqname1, matpt->seqlong1, matpt->seqname2, matpt->seqlong2, 
		  matpt->fenetre, matpt->identites);
#ifdef NO_PDF
  surface->scale(1, 1); // back to top-down coords
  surface->origin(20, h-200+30);
#else
  surface->origin(20, 170);
#endif
  surface->driver()->font(FL_HELVETICA, 10);
  fl_draw(message, 0, 0);
  surface->end_page();
  surface->end_job();
#ifndef NO_PDF
} /* end of PDF_TRY */
  if( surface->surface_catch() ) {
    surface->error_catch();
    delete surface;
    return;
    }
#endif
#ifndef MICRO
  matpt->form->hide(); Fl::flush(); // because of strange bug on 32-bit Linux only
#endif
  fl_message("Dot plot is now in file\n%s\nin "PDF_OR_PS" format", surface->outfname());
  delete surface;
#ifndef MICRO
  matpt->form->show(); Fl::flush();
#endif
}


int really_close(FD_matpt *matpt)
{
int rep;
if(matpt->modif_but_not_saved) {
	rep = fl_choice("Changes in dot plot were not saved in alignment\n"
		"Do you really want to close the dot plot?", "Cancel", "Close", NULL);
	if(!rep) return FALSE;
	}
free(matpt->seq1 + 1);
free(matpt->seq2 + 1);
matpt->seq1 = matpt->seq2 = NULL;
return TRUE;
}


void exit_button_proc(Fl_Widget *ob, FD_matpt *matpt)
{
if ( really_close(matpt) )  {
  matpt->form->hide();
  }
}


void move_mat_proc(Fl_Widget *ob, void *data)
{
int rank = ((user_data_plus *)data)->value;
FD_matpt *matpt = (FD_matpt *)((user_data_plus *)data)->p;
int val;
val = (int)( ((Fl_Slider *)ob)->value() );
if(rank == 1)
	matpt->view_y = - val;
else
	matpt->view_x = val;
reset_mat_data(matpt);
matpt->mat_panel->redraw();
}


void move_box_step(Fl_Widget *ob, void *extra)
{
int data = ((user_data_plus *)extra)->value;
FD_matpt *matpt = (FD_matpt *)((user_data_plus *)extra)->p;
if(data == 1) {
	matpt->hitx = FL_max(matpt->hitx - 1, 1);
	}
else if(data == 2) {
	matpt->hitx = FL_min(matpt->hitx + 1, matpt->seqlong1);
	}
else if(data == 3) {
	matpt->hitx = FL_max(matpt->hitx - 1, 1);
	matpt->hity = FL_max(matpt->hity - 1, 1);
	}
else if(data == 4) {
	matpt->hitx = FL_min(matpt->hitx + 1, matpt->seqlong1);
	matpt->hity = FL_min(matpt->hity + 1, matpt->seqlong2);
	}
else if(data == 5) {
	matpt->hity = FL_max(matpt->hity - 1, 1);
	}
else	{
	matpt->hity = FL_min(matpt->hity + 1, matpt->seqlong2);
	}
matpt->rect_only = TRUE;
matpt->mat_panel->redraw();
  compute_region_text(matpt);
}


void region_size_proc(Fl_Widget *ob, void *data)
{
FD_matpt *matpt = (FD_matpt *)data;
matpt->region_size = (int)( ((Fl_Slider *)ob)->value() );
matpt->rect_only = TRUE;
matpt->mat_panel->redraw();
  matpt->region_box->redraw();
}


void compute_title(FD_matpt *matpt)
{
static char title[150];
sprintf(title, "Horizontal: %.30s (%d)   Vertical: %.30s (%d)",
	matpt->seqname1, matpt->seqlong1, matpt->seqname2, matpt->seqlong2);
matpt->title = title;
matpt->mat_panel->window()->label(title);
}


int find_next_gap_site(int pos, gap_site *gap_sites, int tot_gap_sites)
{
int num;
for(num = 0; num < tot_gap_sites; num++)
	if(gap_sites[num].pos > pos) return num;
return -1;
}


void new_gap_site(int pos, int l, gap_site *gap_sites, int *p_tot_gap_sites,
	int seqrank)
{
int num, i;
for(num = 0; num < *p_tot_gap_sites; num++)
	if(gap_sites[num].pos >= pos) break;
if(num < *p_tot_gap_sites && gap_sites[num].pos == pos)
	gap_sites[num].l[seqrank-1] += l;
else 	{
	if(*p_tot_gap_sites >= MAX_GAP_SITES) return;
	if(num < *p_tot_gap_sites) {
		for(i = *p_tot_gap_sites; i > num; i--)
			gap_sites[i] = gap_sites[i - 1];
		}
	(*p_tot_gap_sites)++;
	gap_sites[num].pos = pos;
	gap_sites[num].l[0] = 0;
	gap_sites[num].l[1] = 0;
	gap_sites[num].l[seqrank-1] = l;
	}
}


int delete_gaps_bef_pos(char *seq, int pos, int number, int length, 
	int totsegments, int offset)
{
int i;
if(number == 0) return length;
memmove(seq + pos - number, seq + pos, length - pos + 2);
for(i=0; i <= totsegments; i++)
	if(segment_x_ou_y(i, offset) + segments[i].longueur - 1 >= pos ) 
		segment_x_ou_y(i, offset) -= number;
return length - number;
}


int insert_gaps_bef_pos(char *seq, int pos, int number, int length, 
	int totsegments, int offset, int maxlen)
{
int i;
if(length + number > maxlen) return length;
memmove(seq + pos + number, seq + pos, length - pos + 2);
memset(seq + pos, '-', number);
for(i=0; i <= totsegments; i++)
	if(segment_x_ou_y(i, offset) + segments[i].longueur - 1 >= pos ) 
		segment_x_ou_y(i, offset) += number;
return length + number;
}


int dispatch_gaps_bef(int pos, int number, char *seq, gap_site * gap_sites, 
	int *p_tot_gap_sites, int offset, int seqlen, int totsegments,
	int seqrank, char *other_seq, int *other_seqlen, int other_offset,
	int maxlen)
{
int site, lnextgap, other_rank, i, seuil;
site = find_next_gap_site(pos + number, gap_sites, *p_tot_gap_sites);
if(site != -1) {
	if(gap_sites[site].l[seqrank-1] >= number) {
		seqlen = delete_gaps_bef_pos(seq, gap_sites[site].pos, number, 
			seqlen, totsegments, offset);
		seqlen = insert_gaps_bef_pos(seq, pos, number, seqlen, 
			totsegments, offset, maxlen);
		gap_sites[site].l[seqrank-1] -= number;
		}
	else	{
		lnextgap = gap_sites[site].l[seqrank-1];
		other_rank = (seqrank == 1 ? 2 : 1);
		seqlen = delete_gaps_bef_pos(seq, gap_sites[site].pos, lnextgap,
			seqlen, totsegments, offset);
		*other_seqlen = insert_gaps_bef_pos(other_seq, 
			gap_sites[site].pos, 
			number - lnextgap, *other_seqlen, 
			totsegments, other_offset, maxlen);
		seqlen = insert_gaps_bef_pos(seq, pos, number, seqlen, 
			totsegments, offset, maxlen);
		gap_sites[site].l[seqrank-1] = 0;
		gap_sites[site].l[other_rank-1] += number - lnextgap;
		seuil = gap_sites[site].pos;
		for(i = 0; i < *p_tot_gap_sites; i++) {
			if(gap_sites[i].pos < seuil) continue;
			gap_sites[i].pos += number - lnextgap;
			}
		}
	}
else	{
	seqlen = insert_gaps_bef_pos(seq, pos, number, seqlen, totsegments, 
		offset, maxlen);
	}
new_gap_site(pos + number, number, gap_sites, p_tot_gap_sites, seqrank);
return seqlen;
}


void align_callback(Fl_Widget *ob, void *data)
{
int number, offset_1, offset_2;
FD_matpt *matpt = (FD_matpt *)data;
if(matpt->hitx == matpt->hity) return;
my_watch_cursor(ob->window());
offset_1 = (char *)&(segments[0].x) - (char *)&(segments[0]);
offset_2 = (char *)&(segments[0].y) - (char *)&(segments[0]);
if(matpt->hitx > matpt->hity) {
	number = matpt->hitx - matpt->hity;
	matpt->seqlong2 = dispatch_gaps_bef(matpt->hity, number, 
		matpt->seq2, matpt->gap_sites, 
		&(matpt->tot_gap_sites), offset_2, matpt->seqlong2, 
		matpt->totsegments, 2, matpt->seq1, &matpt->seqlong1, offset_1,
		matpt->maxseqlength);
	matpt->hity = matpt->hitx;
	}
else 	{
	number = matpt->hity - matpt->hitx;
	matpt->seqlong1 = dispatch_gaps_bef(matpt->hitx, number, 
		matpt->seq1, matpt->gap_sites, 
		&(matpt->tot_gap_sites), offset_1, matpt->seqlong1, 
		matpt->totsegments, 1, matpt->seq2, &matpt->seqlong2, offset_2,
		matpt->maxseqlength);
	matpt->hitx = matpt->hity;
	}
matpt->longmax = FL_max(matpt->seqlong1, matpt->seqlong2);
center_view_on_hit(matpt);
matpt->mat_panel->redraw();
matpt->x_slider->redraw();
matpt->y_slider->redraw();
  compute_region_text(matpt);
compute_title(matpt);
matpt->modif_but_not_saved = TRUE;
}


void update_col_lines(int num, SEA_VIEW *view)
{
int l, i, res;
int (*calc_color_function)(int);

if(view->numb_gc == 1) return;
calc_color_function = ( view->protein ? 
				get_color_for_aa : get_color_for_base );
l = view->each_length[num];
for(i=0; i<l; i++) {
	res = view->sequence[num][i]; 
	view->col_rank[num][i] = (char)calc_color_function( res );
	}
}


void record_alignment_callback(Fl_Widget *ob, void *data)
{
int num1, num2, site, need_big_gaps = FALSE, seqrank, num, ngaps,
	newlength;
FD_matpt *matpt = (FD_matpt *)data;
SEA_VIEW *view = (SEA_VIEW *)matpt->seaview_data;
Fl_Menu_ *menu = (Fl_Menu_ *)matpt->choice_ref_seq;
if( ! menu->mvalue() ) {
	fl_message("Please, select which is the reference sequence");
	return;
	}
seqrank = menu->value();
my_watch_cursor(ob->window());
for(site = 0; site < matpt->tot_gap_sites; site++) {
	if(matpt->gap_sites[site].l[seqrank] != 0) {
		need_big_gaps = TRUE;
		break;
		}
	}
for(num1 = 0; num1 < view->tot_seqs; num1++)
	if(view->sel_seqs[num1]) break;
for(num2 = num1 + 1; num2 < view->tot_seqs; num2++)
	if(view->sel_seqs[num2]) break;
memcpy(view->sequence[num1], matpt->seq1 + 1, matpt->seqlong1 + 1);
view->each_length[num1] = matpt->seqlong1;
update_col_lines(num1, view);
memcpy(view->sequence[num2], matpt->seq2 + 1, matpt->seqlong2 + 1);
view->each_length[num2] = matpt->seqlong2;
update_col_lines(num2, view);
if( need_big_gaps ) {
	newlength = FL_max(matpt->seqlong1, matpt->seqlong2);
	for(site = 0; site < matpt->tot_gap_sites; site++) {
		ngaps = matpt->gap_sites[site].l[seqrank];
		if( ngaps == 0) continue;
		for(num = 0; num < view->tot_seqs; num++) {
			if( view->sel_seqs[num] ) continue;
			insert_gaps_at(view, num+1, 
				matpt->gap_sites[site].pos - ngaps, ngaps);
			newlength = FL_max(newlength, view->each_length[num]);
			}
		matpt->gap_sites[site].l[seqrank] = 0;
		insert_region_part(view, matpt->gap_sites[site].pos - ngaps, 
			ngaps);
		}
	update_current_seq_length(newlength, view);
	view->horsli->redraw();
	}
set_seaview_modified(view, TRUE);
matpt->modif_but_not_saved = FALSE;
view->DNA_obj->redraw();
fl_reset_cursor(ob->window());
}


void matpt_idle_cb(FD_matpt *matpt)
{
  static char err_seg[]="Only part of the dot plot was computed";
#ifdef __APPLE__
  const int step = 100; // on Mac OS, idle callbacks are slow to be called, so better give them more work
#else
  const int step = 1;
#endif
  int i, deb, fin, nmatch, finw, diff, lfrag, k, pp, d, deb1, fin1, fin2, 
    deb2, pxd, pxf, pyd, pyf, erreur = FALSE;
  int kk0 = matpt->kk;
  if ( !matpt->form->visible() ) {
    goto label_fin;
  }
  matpt->form->make_current();
  if (matpt->interrupted) {
    goto label_fin;
  }
  fl_push_clip(matpt->mat_panel->x(), matpt->mat_panel->y(), matpt->mat_panel->w(), matpt->mat_panel->h());
  for (; matpt->kk < kk0 + step && matpt->kk <= matpt->dmax + 1; matpt->kk++) {
      /* pour faire la boucle k = [dmin,dmax] en partant du centre et s'eloignant */
      matpt->flip = - matpt->flip;
      k = matpt->centre + ((matpt->kk - matpt->dmin)/2) * matpt->flip;
      deb= ( k>=0 ? k+1 : 1);
      fin= FL_min( matpt->seqlong2_nogaps+k , matpt->seqlong1_nogaps);
      /*on met it[1] ssi bases egales en majuscu dans les 2 seqs sur la diagonale k*/
      for(i = deb; i <= fin; i++) {
	diff = matpt->seq1[matpt->addgaps1[i]] - matpt->seq2[matpt->addgaps2[i-k]];
	matpt->it[i] = (diff == 0 || diff == 32 || diff == -32 ? 1 : 0);
      }
      matpt->it[fin + 1] = 0;
      /* on met it[1] ssi au moins matpt->identites bases egales dans fenetre de taille fenetre 
       sur la diagonale k 
       */
      pp=matpt->it[deb];
      nmatch=0;
      for (i = deb; i < deb+matpt->fenetre; i++) nmatch += matpt->it[i];
      if(nmatch>=matpt->identites)
	matpt->it[deb]=1;
      else
	matpt->it[deb]=0;
      finw = fin-matpt->fenetre+1;
      for(i = deb+1; i <= finw; i++) {
	nmatch += (matpt->it[i+matpt->fenetre-1]-pp);
	pp=matpt->it[i];
	if(nmatch>=matpt->identites)
	  matpt->it[i]=1;
	else
	  matpt->it[i]=0;
      }
      i=deb-1;
      /* calcul de d = debut des runs de 1, i = suivant de la fin du run de 1 */
      while(i<finw) {
	i++;
	if(matpt->it[i]==0) continue;
	d=i;
	do	{
	  i++;
	  if(i>finw) break;
	}
	while(matpt->it[i]==1);
	/* decoupage du segment sans gaps en autant de morceaux non coupes de gaps */
	deb1 = matpt->addgaps1[d]; deb2 = matpt->addgaps2[d-k]; lfrag = 0;
	do	{
	  fin1 = deb1; fin2 = deb2;
	  while(lfrag < i - d && matpt->seq1[fin1] != '-' && 
		matpt->seq2[fin2] != '-') {
	    fin1++; fin2++; lfrag++;
	  }
	  fin1--; fin2--;
	  if (matpt->totsegments >= MAXSEGMENTS) {
	    erreur = TRUE;
	    fl_pop_clip();
	    goto label_fin;
	  }
	  matpt->totsegments++;
	  segments[matpt->totsegments].x = deb1;
	  segments[matpt->totsegments].y = deb2;
	  segments[matpt->totsegments].longueur = fin1 - deb1 + 1;
	  log_to_phys(segments[matpt->totsegments].x,
		      segments[matpt->totsegments].y, 
		      &pxd, &pyd, matpt);
	  log_to_phys(segments[matpt->totsegments].x + 
		      segments[matpt->totsegments].longueur, 
		      segments[matpt->totsegments].y + 
		      segments[matpt->totsegments].longueur, 
		      &pxf, &pyf, matpt);
	  fl_line(pxd, pyd, pxf, pyf);
	  deb1 = fin1+1; deb2 = fin2+1;
	  while(matpt->seq1[deb1] == '-') deb1++;
	  while(matpt->seq2[deb2] == '-') deb2++;
	}
	while(lfrag < i - d);
      }
  }
  fl_pop_clip();
  if (matpt->kk <= matpt->dmax + 1) return;
label_fin:
  Fl::remove_idle((Fl_Idle_Handler)matpt_idle_cb, matpt);
  free(matpt->it);
  free(matpt->addgaps1); free(matpt->addgaps2);
  if (matpt->form->visible()) draw_rect(matpt);
  fl_reset_cursor(matpt->form);
  if (erreur) {
    fl_message("%s", err_seg);
    }
}

/* dot plot pour sequences avec gaps:
les segments sont calcules sans les gaps
leurs coordonnees sont reportees sur les seqs avec gaps
puis ils sont coupes en autant de morceaux sans gap
*/
void compute_diags(FD_matpt *matpt)
{
int  longmax;
static char err_mem[]="Not enough memory";
char *p;

matpt->need_compute = FALSE;
  matpt->seqlong1_nogaps = matpt->seqlong1; matpt->seqlong2_nogaps = matpt->seqlong2;

matpt->totsegments= -1;
/* calcul corresp coord sans gap -> coord avec gap */
matpt->addgaps1 = (int *)malloc( (matpt->seqlong1_nogaps+2) * sizeof(int));
matpt->addgaps2 = (int *)malloc( (matpt->seqlong2_nogaps+2) * sizeof(int));
if(matpt->addgaps1 == NULL || matpt->addgaps2 == NULL) {
  goto way_out;
  }
p = matpt->seq1; matpt->seqlong1_nogaps = 0;
while( *(++p) != 0) {
	if( *p == '-') continue;
	matpt->addgaps1[++matpt->seqlong1_nogaps] = p - matpt->seq1;
	}
p = matpt->seq2; matpt->seqlong2_nogaps = 0;
while( *(++p) != 0) {
	if( *p == '-') continue;
	matpt->addgaps2[++matpt->seqlong2_nogaps] = p - matpt->seq2;
	}

matpt->dmin = matpt->fenetre-matpt->seqlong2_nogaps; matpt->dmax = matpt->seqlong1_nogaps-matpt->fenetre-1;
longmax = FL_max(matpt->seqlong1_nogaps, matpt->seqlong2_nogaps);
matpt->it = (int *)malloc( (longmax + 2) * sizeof(int));
if(matpt->it == NULL) {
  goto way_out;
  }
matpt->centre = (matpt->dmin + matpt->dmax)/2; matpt->flip = 1;
matpt->kk = matpt->dmin + 1;
Fl::add_idle((Fl_Idle_Handler)matpt_idle_cb, matpt);
return;
way_out:
  fl_message("%s", err_mem);
}
