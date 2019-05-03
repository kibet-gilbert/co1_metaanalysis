#include "seaview.h"
#include <ctype.h>
#include <FL/Fl_Multiline_Input.H>

#define ISSEQCHAR(c) (isalpha(c) || c=='-' || c=='*' )

typedef struct {
	SEA_VIEW *view;
	Fl_Input *seq_name_field;
	Fl_Input *seq_field;
	} view_name_struct;

/* included prototypes */
void clear_callback(Fl_Widget *ob, void *data);
void refresh_callback(Fl_Widget *ob, void *data);
void remove_gaps_callback(Fl_Widget *ob, void *data);
void remove_numbers_callback(Fl_Widget *ob, void *data);
void to_upcase_callback(Fl_Widget *ob, void *data);
void load_seq_callback(void *data);
void load_seq_dialog(SEA_VIEW *view);
char complement_base(char old);
void edit_comments_dialog(SEA_VIEW *view);
void edit_sequence_dialog(SEA_VIEW *view);
int load_sequence_for_edit(struct editseq *editdata);
char *renumber_seq(char *seq);
void renumber_callback(Fl_Widget *ob, void *data);
void edit_sequence_callback(struct editseq *editdata);
void update_comments_callback(void *data);
int load_comments(SEA_VIEW *view, Fl_Input *input, Fl_Widget *name);
char *cre_consensus(SEA_VIEW *view, char *newname);
Fl_Widget *cre_adjusted_button(int x, int y, int *w, int h, 
	const char *label);


/* external prototypes */
extern void hide_window_callback(Fl_Widget *ob, void *data);
extern void allonge_seqs(char **seq, int totseqs, int maxlen, int *eachlength,
	int tot_comment_lines, char **comment_line, char **pregion_line);
extern int compute_wid_names(SEA_VIEW *view, int totseqs);
extern int compute_size_params(SEA_VIEW *view, int force_recompute);
extern int calc_max_seq_length(int seq_length, int tot_seqs);

	
void clear_callback(Fl_Widget *ob, void *data)
{
Fl_Input *seq_input, *seq_name_input;
seq_name_input = ((view_name_struct *)data)->seq_name_field;
seq_input = ((view_name_struct *)data)->seq_field;
seq_input->value("");
if(seq_name_input != NULL) seq_name_input->value("");
}


void refresh_callback(Fl_Widget *ob, void *data)
{
Fl_Input *seq_input;
seq_input = ((view_name_struct *)data)->seq_field;
seq_input->redraw();
}


void remove_gaps_callback(Fl_Widget *ob, void *data)
{
Fl_Input *seq_input;
char *old_seq, *new_seq, *p, *q;
size_t lseq;

seq_input = (Fl_Input *)data;
old_seq = (char *)seq_input->value();
lseq = strlen(old_seq);
new_seq = (char *)malloc(lseq+1);
if(new_seq == NULL) return;
p = old_seq; q = new_seq;
while(*p != 0) {
	if(*p != '-' ) *(q++) = *p;
	p++;
	}
*q = 0;
seq_input->value(new_seq);
}


void remove_numbers_callback(Fl_Widget *ob, void *data)
{
Fl_Input *seq_input;
char *old_seq, *new_seq, *p, *q;
size_t lseq;

seq_input = (Fl_Input *)data;
old_seq = (char *)seq_input->value();
lseq = strlen(old_seq);
new_seq = (char *)malloc(lseq+1);
if(new_seq == NULL) return;
p = old_seq; q = new_seq;
while(*p != 0) {
	if( ! isdigit(*p) ) *(q++) = *p;
	p++;
	}
*q = 0;
seq_input->value(new_seq);
}


void to_upcase_callback(Fl_Widget *ob, void *data)
{
Fl_Input *seq_input = (Fl_Input *)data;
char *old_seq = (char *)seq_input->value();
int lseq = strlen(old_seq);
char *new_seq = (char *)malloc(lseq+1);
if(new_seq == NULL) return;
char *p = old_seq; 
char *q = new_seq - 1;
do *(++q) = toupper(*(p++)); while(*q != 0);
seq_input->value(new_seq);
}


void load_seq_callback(void *data)
{
Fl_Input *seq_name_input, *seq_input;
SEA_VIEW *view;
char *name, *newseq, *tmp, *p, *q;
int lenseq;

seq_name_input = ((view_name_struct *)data)->seq_name_field;
view = ((view_name_struct *)data)->view;
seq_input = ((view_name_struct *)data)->seq_field;
name = (char *)seq_name_input->value();
if(strlen(name) == 0) {
	fl_alert(
		"`Seq. name' field is empty\nPlease enter a sequence name");
	return;
	}
tmp = (char *)seq_input->value();
lenseq = (int)strlen(tmp);
newseq = (char *)malloc(lenseq+1);
if(newseq == NULL) {
	fl_alert("Not enough memory");
	return;
	}
p = tmp; q = newseq; lenseq = 0;
while(*p != 0) {
	if(*p != ' ' && *p != '\n' && *p != '\t') {
		if(view->allow_lower) *(q++) = (*p);
		else *(q++) = toupper(*p); 
		lenseq++;
		}
	p++;
	}
*q = 0;
add_seq_to_align(view, name, newseq, lenseq);
free(newseq);
}


Fl_Widget *cre_adjusted_button(int x, int y, int *w, int h, 
	const char *label)
{
Fl_Button *obj;
const int lsize = FL_NORMAL_SIZE;
fl_font(FL_HELVETICA, lsize);
// *w = (int) ( fl_width(label) + 2 * fl_width("R") + 0.5 );
*w = (int) ( fl_width(label) + 10 + 0.5 );
obj = new Fl_Button(x, y, *w, h, label);
obj->labelsize(lsize);
obj->labelfont(FL_HELVETICA);
return (Fl_Widget *)obj;
}


void load_seq_dialog(SEA_VIEW *view)
{
  Fl_Window *load_form;
  Fl_Input *seq_input, *seq_name_input;
  view_name_struct view_name;
  Fl_Widget *cancel, *add, *obj;
  int fin, width;
  load_form = new Fl_Window(490,530);
  load_form->box(FL_FLAT_BOX);
  load_form->label("Sequence Loading");
  
  int curr_y = 5;
  fin = 5;
  Fl_Group *top = new Fl_Group(fin,curr_y,load_form->w() - 2 * fin, 25);
  static char label[] = "Seq. name:";
  fl_font(FL_HELVETICA, FL_NORMAL_SIZE);
  width = (int)fl_width(label) + 4 * 2;
  Fl_Box *name_box = new Fl_Box(FL_UP_BOX, 
	  fin, top->y(), width, top->h(), label);
  name_box->labelsize(FL_NORMAL_SIZE);
  name_box->box(FL_NO_BOX);
  fin += width + 5;

  seq_name_input = new Fl_Input(fin,top->y(),100,top->h(),"");
  seq_name_input->type(FL_NORMAL_INPUT);
  fin += seq_name_input->w() + 5;
  
  add = cre_adjusted_button(fin,top->y(),&width,top->h(),
	  "Add to alignment");
  fin += width + 5;
  
  cancel = cre_adjusted_button(fin,top->y(),&width,top->h(),
	  "Cancel");
  ((Fl_Button *)cancel)->shortcut(FL_COMMAND | 'w');
  fin += width;
  
  top->resizable(NULL);
  top->size(fin - top->x(), top->h());
  top->end();
  
  curr_y += top->h() + 15;
  seq_input = new Fl_Input(top->x(), curr_y, load_form->w() - 2 * top->x(), 450,
#if defined(__APPLE__)
	    ""
#elif defined(WIN32) 
	    "Type sequence or paste it with CRTL-V in panel below"
#else
	    "Type or paste sequence with middle mouse button in panel below"
#endif
	    );
  seq_input->type(FL_MULTILINE_INPUT);
  seq_input->align(FL_ALIGN_TOP);
  seq_input->textfont(FL_COURIER);
  seq_input->textsize(12);
  fin = top->x();
  curr_y += seq_input->h() + 5;
  Fl_Group *bottom = new Fl_Group(fin, curr_y, top->w(), top->h() );
  obj = cre_adjusted_button(fin, curr_y, &width, bottom->h(), "Refresh");
  obj->callback(refresh_callback, &view_name);
  fin += width + 5;

  obj = cre_adjusted_button(fin, curr_y, &width, bottom->h(), "Clear");
  obj->callback(clear_callback, &view_name);
  fin += width + 5;

  obj = cre_adjusted_button(fin, curr_y, &width, bottom->h(),
	"Remove gaps");
  obj->callback(remove_gaps_callback, seq_input);
  fin += width + 5;

  obj = cre_adjusted_button(fin,curr_y,&width,bottom->h(),
	"Remove numbers");
  obj->callback(remove_numbers_callback, seq_input);
  fin += width + 5;

  obj = cre_adjusted_button(fin, curr_y, &width, bottom->h(),
	"to UPPER");
  obj->callback(to_upcase_callback, seq_input);
  fin += width;

  bottom->resizable(NULL);
  bottom->size(fin - bottom->x(), bottom->h());
  bottom->end();

  load_form->resizable(seq_input);

  view_name.seq_name_field = seq_name_input;
  view_name.seq_field = seq_input;
  load_form->end();
  load_form->position((Fl::w() - load_form->w())/2, (Fl::h() - load_form->h())/2);
  seq_name_input->value("");
  seq_input->value("");
  view_name.view = view;
#if defined(__APPLE__)
	seq_input->value("\n\n\n\n\n\nType sequence or paste it with cmd-V\n\n\n");
	seq_input->position(seq_input->size(), 0);
#endif	
  seq_input->take_focus();
  load_form->show();
#ifndef MICRO
  load_form->hotspot(load_form);
#endif
  while (TRUE) {
    obj = Fl::readqueue();
    if (!obj) Fl::wait();
    else if(obj == add || obj == cancel || obj == load_form) {
      if (obj == add) {
	load_seq_callback(&view_name);
      }
      delete load_form;
      break;
    }
  }
}


int add_seq_to_align(SEA_VIEW *view, char *newname, char *newseq, int lenseq, bool no_gui)
/* returns 0 if failed or the rank of newly added sequence
*/
{
int num, *newsel, numset;
char **newcol;
static char defname[]="newfile";
char **new_s, **new_n, **new_c;

if(view->tot_seqs == 0) {
	view->max_seq_length = lenseq;//the true value is set later in init_dna_scroller
	}
num = view->tot_seqs + 1;
if(view->sequence != NULL) new_s = (char **)realloc(view->sequence, num * sizeof(char *));
else new_s = (char **)malloc(num * sizeof(char *));
if(new_s != NULL) view->sequence = new_s;
if(view->comments != NULL) {
	new_c = (char **)realloc(view->comments, num * sizeof(char *));
	if(new_c != NULL) view->comments = new_c;
	else goto nomem;
	}
else view->comments = (char **)calloc(num, sizeof(char *));
if(view->seqname != NULL) new_n = (char **)realloc(view->seqname, num * sizeof(char *));
else new_n = (char **)malloc(num * sizeof(char *));
if(new_n != NULL) view->seqname = new_n;

if(new_s == NULL || new_n == NULL) goto nomem;
if(lenseq > view->max_seq_length) {
  view->max_seq_length = calc_max_seq_length(lenseq, view->tot_seqs);
  allonge_seqs(view->sequence, view->tot_seqs, view->max_seq_length, view->each_length, 
	       view->tot_comment_lines, view->comment_line, &view->region_line);
  if (!no_gui) {
    for (int i = 0; i < view->tot_seqs; i++) free(view->col_rank[i]);
    free(view->col_rank);
    view->col_rank = prepcolranks(view->sequence, view->tot_seqs, view->max_seq_length, view->each_length, 
		 ( view->protein ? get_color_for_aa : get_color_for_base ), 
		 view->numb_gc, view->allow_lower);
    }
  }
if( (view->seqname[num-1] = (char *)malloc(strlen(newname)+1)) == NULL) 
	goto nomem;
strcpy(view->seqname[num-1], newname);
if(view->comments != NULL) {
	view->comments[num-1] = NULL;
	}
if( (view->sequence[num-1] = (char *)malloc(view->max_seq_length + 1)) == NULL) 
	goto nomem;
memcpy(view->sequence[num-1], newseq, lenseq+1);
if(view->masename == NULL && !no_gui) {
	newname = (char *) fl_input("Name of the new alignment?", "");
	if(newname == NULL) newname = defname;
	}
else newname = view->masename;
if (view->tot_seqs == 0 && !no_gui) {
	init_dna_scroller(view, 1, newname, is_a_protein_seq(newseq), NULL);
	set_seaview_modified(view, TRUE);
	view->DNA_obj->redraw();
	((Fl_Menu_Item*)view->menu_file + SAVE_AS)->activate();
	return 1;
	}
if( (newsel = (int *)malloc(num*sizeof(int))) == NULL) goto nomem;
memcpy(newsel, view->each_length, (num-1)*sizeof(int) );
free(view->each_length);
view->each_length = newsel;
view->each_length[num-1] = lenseq;
if (lenseq > view->seq_length && !no_gui) {
	double x; int l;
	view->seq_length = lenseq;
	x = ( (double) view->tot_sites ) / ( view->seq_length + 3 ); 
	if(x>1) x=1;
	((Fl_Slider*)view->horsli)->slider_size(x);
	l = view->seq_length - view->tot_sites+3;
	if(l<1) l=1;
	((Fl_Slider*)view->horsli)->bounds(1,l);
	}
if(view->numb_dnacolors > 1 && !no_gui) {
	if( (newcol = (char **)malloc(num*sizeof(char *))) == NULL) 
		goto nomem;
	memcpy(newcol, view->col_rank, (num-1)*sizeof(char *) );
  char **tmpcol = prepcolranks(view->sequence+num-1, 1, 
		view->max_seq_length,
		view->each_length+num-1, 
		( view->protein ? get_color_for_aa : get_color_for_base ), 
		view->numb_gc, view->allow_lower);
	if(tmpcol == NULL) out_of_memory();
  newcol[num-1] = tmpcol[0];
  free(tmpcol);
	free(view->col_rank);
	view->col_rank = newcol;
	}
if( (newsel = (int *)malloc(num*sizeof(int))) == NULL) goto nomem;
  if (!no_gui) {
    memcpy(newsel, view->sel_seqs, (num-1)*sizeof(int) );
    free(view->sel_seqs);
    view->sel_seqs = newsel;
    view->sel_seqs[num-1] = FALSE;
  }
for(numset = 0; numset < view->numb_species_sets; numset++) {
	if( (newsel = (int *)malloc(num*sizeof(int))) == NULL) goto nomem;
	memcpy(newsel, view->list_species_sets[numset], (num-1)*sizeof(int) );
	free(view->list_species_sets[numset]);
	view->list_species_sets[numset] = newsel;
	view->list_species_sets[numset][num-1] = FALSE;
	}
view->tot_seqs = num;
view->cursor_seq = num;
view->cursor_site = 1;
view->first_site = 1;
  if (no_gui) return num;
if(strlen(view->seqname[num - 1]) > (unsigned)view->wid_names) {
  view->wid_names = compute_wid_names(view, num);
  compute_size_params(view, TRUE);
}
set_seaview_modified(view, TRUE);
((Fl_Slider*)view->horsli)->value(1);
set_tot_lines(view, view->tot_lines);
view->first_seq = FL_max(num - view->tot_lines + 1, 1);
((Fl_Slider*)view->vertsli)->value(view->first_seq);
view->DNA_obj->redraw();
view->vertsli->redraw();
view->horsli->redraw();
return num;
nomem:
  void (*alert)(const char *,...) = no_gui ? Fl::fatal : fl_alert;
  alert("Not enough memory\nto create the new sequence %s", newname);
return 0;
}


char complement_base(char old)
{
static char bases[] = "ACGTURYMWSKVHDBacgturymwskvhdb";
static char complement[] = "TGCAAYRKWSMBDHVtgcaayrkwsmbdhv";
char *p;
if( (p = strchr(bases, old)) != NULL )
	return complement[ p - bases ];
else
	return old;
}


void edit_comments_dialog(SEA_VIEW *view)
{
  Fl_Window *comments_form;
  Fl_Input *comments_input;
  Fl_Widget *comments_name;
  view_name_struct comments_data;
  Fl_Widget *o, *cancel, *apply;

  Fl_Widget *obj;
  int fin, width;
  comments_form = new Fl_Window(490,530);
  comments_form->label("Comments Editing");
  comments_form->box(FL_FLAT_BOX);
  
  Fl_Group *top_group = new Fl_Group(5,5,comments_form->w() - 10, 25);
  fin = 5;
  apply = cre_adjusted_button(fin,5,&width,25, "Apply");
  fin += width + 5;
  
  cancel = cre_adjusted_button(fin,5,&width,25,"Cancel");
  ((Fl_Button *)cancel)->shortcut(FL_COMMAND | 'w');
  fin += width + 5;
  
  obj = cre_adjusted_button(fin,5,&width,25,"Refresh");
  obj->callback(refresh_callback, &comments_data);
  fin += width + 5;

  comments_name = new Fl_Box(FL_DOWN_BOX, fin, 5, 
	  top_group->x() +top_group->w() - fin, 25, "");
  comments_name->align(FL_ALIGN_LEFT | FL_ALIGN_CLIP | FL_ALIGN_INSIDE);
  comments_name->labelfont(FL_COURIER);
  comments_name->labelsize(FL_NORMAL_SIZE);

  top_group->resizable(comments_name);
  top_group->end();
  
  comments_input = new Fl_Input(5,35,top_group->w(),
		  comments_form->h() - 5 - 35, "");
  comments_input->type(FL_MULTILINE_INPUT);
  comments_input->textfont(FL_COURIER);
  comments_input->textsize(12);
  comments_data.seq_field = comments_input;
  comments_form->resizable(comments_input);
  comments_form->end();
  comments_form->position( (Fl::w() - comments_form->w())/2, 
	  (Fl::h() - comments_form->h())/2 );
  comments_data.view = view;
  if(load_comments(view, comments_input, comments_name)) {
	delete comments_form;
	fl_alert("Not enough memory");
	return;
	}
  comments_input->take_focus();
  comments_form->show();
#ifndef MICRO
	comments_form->hotspot(comments_form);
#endif
  while (TRUE) {
    o = Fl::readqueue();
    if (!o) Fl::wait();
    else if (o == apply || o == cancel || o == comments_form) {
      if (o == apply) {
	update_comments_callback(&comments_data);
	}
      delete comments_form;
      break;
      }
    }
}


struct editseq {
	Fl_Multiline_Input *input;
	Fl_Widget *name;
	SEA_VIEW *view;
	int seqnum;
	};


void edit_sequence_dialog(SEA_VIEW *view)
{
struct editseq *editdata;
int fin, width, num;
Fl_Window *form;
Fl_Widget *obj, *apply, *cancel;

for(num = 0; num <view->tot_seqs; num++) { if(view->sel_seqs[num]) break; }
if(num >= view->tot_seqs) return;
editdata = (struct editseq *)malloc(sizeof(struct editseq));
if(editdata == NULL) return;
editdata->seqnum = num;
editdata->view = view;
form = new Fl_Window(770,530);
form->label("Sequence Editing");
form->box(FL_FLAT_BOX);
form->user_data(editdata);

Fl_Group *top_group = new Fl_Group(5,5,form->w() - 10, 25);
fin = 5;
apply = cre_adjusted_button(fin,5,&width,25, "Apply");
fin += width + 5;

obj = cre_adjusted_button(fin,5,&width,25,"Renumber");
obj->callback(renumber_callback, 0);
fin += width + 5;

cancel = cre_adjusted_button(fin,5,&width,25,"Cancel");
((Fl_Button *)cancel)->shortcut(FL_COMMAND | 'w');
fin += width + 5;

editdata->name = new Fl_Box(FL_DOWN_BOX, fin, 5, 
	top_group->x() +top_group->w() - fin, 25, "");
editdata->name->align(FL_ALIGN_LEFT | FL_ALIGN_CLIP | FL_ALIGN_INSIDE);
editdata->name->labelfont(FL_COURIER);
editdata->name->labelsize(FL_NORMAL_SIZE);

top_group->resizable(editdata->name);
top_group->end();

editdata->input = new Fl_Multiline_Input(5,35,top_group->w(),
		form->h() - 5 - 35, "");
editdata->input->type(FL_MULTILINE_INPUT);
editdata->input->textfont(FL_COURIER);
editdata->input->textsize(12);
form->resizable(editdata->input);
form->end();
form->position( (Fl::w() - form->w())/2, 
		(Fl::h() - form->h())/2 );
	
if(load_sequence_for_edit(editdata)) {
  free(editdata);
	delete form;
	fl_alert("Not enough memory");
	return;
	}
form->show();
#ifndef MICRO
form->hotspot(form);
#endif
editdata->input->take_focus();
  while (TRUE) {
    obj = Fl::readqueue();
    if (!obj) Fl::wait();
    else if(obj == apply || obj == cancel || obj == form) {
      if (obj == apply) {
	edit_sequence_callback(editdata);
      }
      free(editdata);
      delete form;
      break;
    }
  }
}


int load_sequence_for_edit(struct editseq *editdata)
{
char *temp;

editdata->name->label(editdata->view->seqname[editdata->seqnum]);
editdata->name->redraw();

temp = renumber_seq(editdata->view->sequence[editdata->seqnum]);
if(temp == NULL) return TRUE;
editdata->input->value(temp);
free(temp);
editdata->input->position(0);
return FALSE;
}


char *renumber_seq(char *seq)
{
int l, nl, pos;
char *p, *q, *temp;
const int w = 100;

p = seq - 1;
l = 0;
while(*(++p) != 0) {
	if(ISSEQCHAR(*p)) l++;
	}
nl = l / w + 2;
temp = (char *)malloc(l + 7 * nl + 100);
if(temp == NULL) return NULL;
p = seq - 1;
q = temp;
pos = 0;
while(*(++p) != 0) {
	if( ! ISSEQCHAR(*p)) continue;
	*q++ = *p; pos++;
	if(pos % w == 0) {
		sprintf(q, "%6d\n", pos);
		q += strlen(q);
		}
	}
strcpy(q, "\n");
return temp;
}


void renumber_callback(Fl_Widget *ob, void *data)
{
char *temp;
struct editseq *editdata = (struct editseq *)ob->window()->user_data();

temp = renumber_seq((char *)editdata->input->value());
if(temp == NULL) return;
editdata->input->value(temp);
free(temp);
}


void edit_sequence_callback(struct editseq *editdata)
{
char *p, *newseq, *q, **newcolseq;
SEA_VIEW *view;
int num, l;

view = editdata->view;
num = editdata->seqnum;

l = strlen( editdata->input->value() );
newseq = (char *)malloc(l + 1);
if(newseq == NULL) return;
q = newseq;
p = (char *)editdata->input->value() - 1;
while(*(++p) !=  0) {
	if( ! ISSEQCHAR(*p) ) continue;
	*(q++) = *p;
	}
*q = 0;
l = strlen(newseq);
  if (l > view->max_seq_length) {
    view->max_seq_length = calc_max_seq_length(l, view->tot_seqs);
    allonge_seqs(view->sequence, view->tot_seqs, view->max_seq_length, view->each_length, 
		 view->tot_comment_lines, view->comment_line, &view->region_line);
    for (int i = 0; i < view->tot_seqs; i++) free(view->col_rank[i]);
    free(view->col_rank);
    view->col_rank = prepcolranks(view->sequence, view->tot_seqs, view->max_seq_length, view->each_length, 
				  ( view->protein ? get_color_for_aa : get_color_for_base ), 
				  view->numb_gc, view->allow_lower);
  }
if(view->numb_gc > 1) {
	newcolseq = prepcolranks(&newseq, 1, 
		view->max_seq_length, &l, 
		( view->protein ? get_color_for_aa : get_color_for_base ), 
		view->numb_gc, view->allow_lower);
	if(newcolseq == NULL) return;
	free(view->col_rank[num]);
	view->col_rank[num] = newcolseq[0];
	free(newcolseq);
	}
strcpy(view->sequence[num], newseq); free(newseq);
view->each_length[num] = l;
set_seaview_modified(view, TRUE);
if(l > view->seq_length) {
	double x;
	view->seq_length = l;
	x = ( (double) view->tot_sites ) / ( view->seq_length + 3 ); 
	if(x>1) x=1;
	((Fl_Slider*)view->horsli)->slider_size(x);
	l = view->seq_length - view->tot_sites+3;
	if(l<1) l=1;
	((Fl_Slider*)view->horsli)->bounds(1,l);
	}
else view->mod_seq = num;
view->DNA_obj->redraw();
}


void update_comments_callback(void *data)
{
Fl_Input *comments_input;
SEA_VIEW *view;
int num, l, num_l;
char *temp, *p, *q, *r;

view = ((view_name_struct *)data)->view;
if(view->comments == NULL) {
	view->comments = (char **)calloc(view->tot_seqs , sizeof(char *));
	if(view->comments == NULL) return;
	}
comments_input = ((view_name_struct *)data)->seq_field;
num = comments_input->argument();
num_l = 0;
p = (char *)comments_input->value(); l =strlen(p);
q = p;
while( (q = strchr(q, '\n')) != NULL) {
	q++; num_l++;
	}
if(p[l - 1] != '\n') num_l++;
temp = (char *)malloc(l + num_l + 1);
if(temp == NULL) {
	fl_alert("Not enough memory");
	return;
	}
r = temp;
do	{
	q = strchr(p, '\n'); if(q == NULL) q = strchr(p, 0) - 1;
	*(r++) = ';';
	memcpy(r, p, q - p + 1); r += q - p + 1;
	p = q + 1;
	}
while( *p != 0);
if( *(r - 1) != '\n') *(r++) = '\n';
*r = 0;
if(view->comments[num] != NULL) free(view->comments[num]);
view->comments[num] = temp;
set_seaview_modified(view, TRUE);
}


int load_comments(SEA_VIEW *view, Fl_Input *input, Fl_Widget *name)
{
int num;
char *temp, *p, *q, *r;

for(num = 0; num <view->tot_seqs; num++) 
	if(view->sel_seqs[num]) break;
input->argument( num);
name->label(view->seqname[num]);
name->redraw();
if( view->comments == NULL || view->comments[num] == NULL) {
	input->value("");
	return FALSE;
	}
temp = (char *)malloc(strlen(view->comments[num]) + 1);
if( temp == NULL) return TRUE;
r = temp; p = view->comments[num];
do	{
	q = strchr(p, '\n');
	memcpy(r, p + 1, q - p); r += q - p;
	p = q + 1;
	}
while( *p != 0);
*r = 0;
input->value(temp);
free(temp);
input->position(0);
return FALSE;
}

static int *sort_f_transmit;

static int sort_f(const void *v1, const void *v2) {
  int i1 = *(int*)v1;
  int i2 = *(int*)v2;
  return sort_f_transmit[i2] - sort_f_transmit[i1];
}

char calc_iupac_consensus(int *freqs, float minval) {
  // order for freqs[1-16]: ACGTUMRWSYKVHDBN
  // code for nucleotide sets: 4 bits representing A,C,G,T=U
  // then coded_set[i-1] is the integer value of the set of count freq[i]
  static int coded_set[16] = {1,2,4,8,8,3,5,9,6,10,12,7,11,13,14,15};
  static char set_to_iupac[18] = {".ACMGRSVTWYHKDBN"};
  int sorted_freqs[16];
  freqs[16] += freqs[0]; // add count of non-IUPAC characters to count of N
  for (int i = 0; i < 16; i++) sorted_freqs[i] = i+1;
  sort_f_transmit = freqs;
  // sort residue counts in decreasing order
  qsort(sorted_freqs, 16, sizeof(int), sort_f);
  int cumul = 0;
  int set = 0;
  // walk through residue counts in decreasing order until cumulated counts exceeds minval
  for (int rank = 0; rank < 16; rank++) {
    int residue = sorted_freqs[rank]; // the next most frequent residue
    cumul += freqs[residue]; // summed frequencies till here
    set |= coded_set[residue - 1]; // the set of all counted residues till here
    // residues of equal frequencies are pooled together
    if (rank < 15 && freqs[residue] == freqs[sorted_freqs[rank+1]]) continue;
    if (cumul >= minval ) break;
  }
  if (set == 8 && sorted_freqs[0] == 5) return 'U';
  return set_to_iupac[set];
}


char calc_site_consensus(int *freqs, int dernier, int threshold, char *residues, char unknown, int total,
						 int protein, int allowgaps, int iupac)
{
	char retval;
	int maxi = 0, num, kind, last;
	maxi = freqs[1];
	kind = 1;
	if(protein) last = dernier;
	else last = 5;
	for(num = 2; num < last; num++) {
		if(freqs[num] > maxi) {
			maxi = freqs[num];
			kind = num;
			}
		}
  if (!protein && !iupac) {
    total -= freqs[16];//DNA: don't count uninformative N's for frequency calculation
    if(total == 0) return 'N';
  }
  float minval = total * (threshold / 100.);
	if (allowgaps) {
		num = dernier - 1; // check if '-' has the highest freq
		if (freqs[num] > maxi) {
                  return freqs[num] > minval ? '-' : unknown;
		}
	}
	if (iupac && !protein ) { // for DNA/RNA with IUPAC codes
          retval = calc_iupac_consensus(freqs, minval);
          }
	else {
		if( maxi >= minval )
			retval = residues[kind - 1];
		else
			retval = unknown;
		}
	return retval;
}
	

char *cre_consensus(SEA_VIEW *view, char *newname)
{
char *newseq, *p, *residues, unknown;
int pos, num, total, kind, dernier, vu;
static char dna_residues[]="ACGTUMRWSYKVHDBN-";
static const char prot_residues[] = "EDQNHRKILMVAPSGTFYWC-";
static int freqs[30];

newseq = (char *)malloc(view->seq_length + 1);
if(newseq == NULL) return NULL;
if(view->protein) {
	residues = (char *)prot_residues;
	unknown = 'X';
	}
else	{
	residues = (char *)dna_residues;
	unknown = 'N';
	}
dernier = strlen(residues) + 1;
	
for (pos = 0; pos < view->seq_length; pos++) {
	vu = total = 0; memset(freqs, 0, dernier * sizeof(int));
	for(num = 0; num < view->tot_seqs; num++) {
		if( !view->sel_seqs[num] ) continue;
		if(pos >= view->each_length[num]) continue;
		vu++;
		if(view->sequence[num][pos] == '-' && ! view->consensus_allowgaps) continue;
		total++;
		p = strchr(residues, toupper(view->sequence[num][pos]));
		if(p == NULL) kind = 0;
		else kind = p - residues + 1;
		++(freqs[kind]);
		}
	if(vu == 0) break;
	if(total == 0)
		newseq[pos] = '-';
	else	{
		newseq[pos] = calc_site_consensus(freqs, dernier, view->consensus_threshold, residues, unknown, 
										  total, view->protein, view->consensus_allowgaps, view->consensus_allowiupac);
		}
	newseq[pos + 1] = 0;
	}
sprintf(newname, "Consensus%d", view->consensus_threshold);
return newseq;
}
