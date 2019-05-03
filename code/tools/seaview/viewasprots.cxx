#include "seaview.h"
#include <FL/Fl_Hold_Browser.H>
#include <ctype.h>

char *translate_with_gaps(char *seq, int gc);
char *back_translate_with_gaps(char *prot, char *dna);
void free_col_seqs(SEA_VIEW * view);
int get_ncbi_gc_from_comment(char *comment);
void set_ncbi_genetic_code(SEA_VIEW *view);
void update_comment_gc(int seqnum, int ncbigc, SEA_VIEW *view);


extern "C" {
	char codaa(char *codon, int code);
	int get_acnuc_gc_number(int ncbi_gc);
	int get_ncbi_gc_number(int ncbi_gc);
        char *get_code_descr(int code);
	char *get_code_target(int code);
	}
extern int totcodes;
extern void set_and_show_new_cursor_seq(SEA_VIEW *view, int new_pos);
extern void set_and_show_new_cursor_site(SEA_VIEW *view, int new_pos, int center,
	int force_redraw);


void *set_viewasprots(SEA_VIEW * view, int onoff)
{
int num, oldmodif, gc, oldtotsel, *oldselseqs, oldsites, oldcursorseq, oldcursorsite;
char **protseq, **dnaseq, **olddna, **retval, *masename;

oldmodif = view->modif_but_not_saved;
oldtotsel = view->tot_sel_seqs;
oldselseqs = view->sel_seqs;
oldcursorseq = view->cursor_seq;
oldcursorsite = view->cursor_site;
save_active_region(view);
masename = view->masename;
  oldsites = view->menu_sites->value();
if(onoff) { /* ON */
	protseq = (char **)malloc(view->tot_seqs * sizeof(char *));
	if(protseq == NULL) return NULL;
	for(num=0; num < view->tot_seqs; num++) {
		gc = (view->comments != NULL ? get_ncbi_gc_from_comment(view->comments[num]) : 1);
		protseq[num] = translate_with_gaps(view->sequence[num], get_acnuc_gc_number(gc));
		if(protseq[num] == NULL) {
			while(--num >= 0) free(protseq[num]);
			free(protseq);
			return NULL;
			}
		}
	retval = view->sequence;
	view->sequence = protseq;
	free(view->each_length);
	free_col_seqs(view);
	if(view->region_line != NULL) free(view->region_line);
	init_dna_scroller(view, view->tot_seqs, masename, TRUE, view->header);
	set_and_show_new_cursor_site(view, (oldcursorsite - 1)/3 + 1, TRUE, FALSE);
	if(view->regions != NULL) {
	  list_segments *lseg;
	  int i = 1;
	  list_regions *lreg = view->regions;
	  while(lreg != NULL) { // for each region
	    lseg = lreg->element->list; // don't change its coordinates if hide_when_viewasprots is on
	    while( (!lreg->element->hide_when_viewasprots) && lseg != NULL) {
	      lseg->debut = (lseg->debut - 1)/3 + 1;
	      lseg->fin = (lseg->fin - 1)/3 + 1;
	      lseg = lseg->next;
	      }
	    if(lreg->element->hide_when_viewasprots) { // inactivate in menu if hide_when_viewasprots is on
	      view->menu_sites->vitem(i - 1)->deactivate();
	      if(i == oldsites) oldsites = 0; // and don't show it as active region
	    }
	    lreg = lreg->next;
	    i++;
	  }
	}
  }
else	{ /* OFF */
  int l, maxl = 0;
  char *p;
  dnaseq = (char **)malloc(view->tot_seqs * sizeof(char *));
  if(dnaseq == NULL) return view->viewasprots;
  olddna = (char **)view->viewasprots;
  for(num=0; num < view->tot_seqs; num++) {
    dnaseq[num] = back_translate_with_gaps(view->sequence[num], olddna[num]);
    if(dnaseq[num] == NULL) return view->viewasprots;
    l = view->each_length[num] = strlen(dnaseq[num]);
    if(l > maxl) maxl = l;
    }
  maxl = 3*((maxl+2)/3); // desired alignment length as multiple of 3
  for(num = 0; num < view->tot_seqs; num++) { // extend all seqs to maxl length with gaps
    free(olddna[num]);
    l = view->each_length[num];
    if(l == maxl) continue;
    p = (char *)realloc(dnaseq[num], maxl + 1);
    if(p == NULL) continue;
    dnaseq[num] = p;
    p += l;
    while(l++ < maxl) *(p++) = '-';
    *p = 0;
    }
  free(olddna);
  for(num=0; num < view->tot_seqs; num++) free(view->sequence[num]);
  free(view->sequence);
  free(view->each_length);
  view->sequence = dnaseq;
  free_col_seqs(view);
  if(view->region_line != NULL) free(view->region_line);
  init_dna_scroller(view, view->tot_seqs, masename, FALSE, view->header);
  set_and_show_new_cursor_site(view, 3*oldcursorsite, TRUE, FALSE);
  if(view->regions != NULL) {
    list_segments *lseg;
    list_regions *lreg = view->regions;
    while(lreg != NULL) {
      lseg = lreg->element->list;
      while( (!lreg->element->hide_when_viewasprots) && lseg != NULL) {
	lseg->debut = 3*(lseg->debut - 1) + 1;
	lseg->fin = 3*(lseg->fin);
	lseg = lseg->next;
	}
      lreg = lreg->next;
      }
    }
  retval = NULL;
  }
set_seaview_modified(view, oldmodif);
view->tot_sel_seqs = oldtotsel;
set_and_show_new_cursor_seq(view, oldcursorseq);
free(view->sel_seqs);
free(masename);
view->sel_seqs = oldselseqs;
if(oldsites) {
  view->menu_sites->vitem(oldsites-1)->set();
  view->menubar->value(view->menu_sites->vitem(oldsites-1));
  view->menu_sites->vitem(oldsites-1)->callback()(view->menubar, 0);
  }
select_deselect_seq(view, -2);
view->DNA_obj->redraw();
return retval;
}


char *translate_with_gaps(char *seq, int gc)
{
	int lp, l = strlen(seq);
	char *p, *last = seq + l;
	lp = 0; 
	p = (char *)malloc(l/3 + 5);
	if(p == NULL) return NULL;
	while(seq + 2 < last) {
		if(strncmp(seq, "---", 3) == 0) {
			p[lp++] = '-';
			}
		else {
			if(*seq == '-') *seq = '@';
			if(*(seq + 1) == '-') *(seq + 1) = '@';
			if(*(seq + 2) == '-') *(seq + 2) = '@';
		  // for macse, translate any !-containing codon to !
		        p[lp++] = ( memchr(seq, '!', 3) ? '!' : codaa(seq, gc) );
			}
		seq += 3;
		}
	p[lp] = 0;
	return p;
}


char *back_translate_with_gaps(char *prot, char *dna)
{
	int l, ld;
	char *newdna, *pd, *q;
	
	l = 3*strlen(prot) + 7;
	newdna = (char *)malloc(l);
	if(newdna == NULL) return NULL;
	pd = newdna;
	prot--;
	while(*(++prot) != 0) {
		while(memcmp(dna, "---", 3) == 0) dna += 3;
		if(*prot == '-') {
			memcpy(pd, "---", 3);
			pd += 3;
			}
		else {
			if(*dna == '@') *dna = '-';
			if(*(dna + 1) == '@') *(dna + 1) = '-';
			if(*(dna + 2) == '@') *(dna + 2) = '-';
			memcpy(pd, dna, 3);
			pd += 3;
			dna += 3;
			}
		}
	*pd = 0;
	while(memcmp(dna, "---", 3) == 0) dna += 3;
	if (*dna != 0 && *dna != '-') {//if a partial codon remains at end
	  while (*(pd-1) == '-') pd--;
	  ld = pd - newdna;
	  q = dna + strlen(dna);
	  while (q > dna && *(q-1) == '-') q--;
	  if (ld+(q-dna) >= l) {
	    l = ld+(q-dna)+5;
	    newdna = (char*)realloc(newdna, l);
	    pd = newdna + ld;
	  }
	  memcpy(pd, dna, q - dna); pd += q - dna; *pd = 0;
	}
	return newdna;
}


void free_col_seqs(SEA_VIEW * view)
{
if(view->numb_gc > 1) { /* free col_rank */
	int num;
	for(num = 0; num < view->tot_seqs; num++) {
		free(view->col_rank[num]);
		}
	free(view->col_rank);
	}
}


int get_ncbi_gc_from_comment(char *comment)
{
char *p;
int gc = -1;

if(comment == NULL) return 1;
p = strstr(comment, "/transl_table=");
if(p == NULL) return 1;
sscanf(p+14, "%d", &gc);
return (gc > 1 ? gc : 1);
}


void ask_gc_callback(Fl_Widget *ob, void *which)
{
int v = ((Fl_Hold_Browser *)which)->value();
int *status = (int *)( ob->window()->user_data() );
*status = v;
}


void ignore_gc_callback(Fl_Widget *ob, void *which)
{
*(int *)which = 0;
}


void set_ncbi_genetic_code(SEA_VIEW *view)
/* sets desired ncbi genetic code to selected sequences (cancel possible)
*/
{
int gc = 1, num;
char line[170];
int status;

if(view->comments == NULL) view->comments = (char **)calloc(view->tot_seqs, sizeof(char *));
if(view->comments == NULL) return;
Fl_Window *win = new Fl_Window(850,400);
win->label("Genetic code selection");
for(num = 0; num < view->tot_seqs; num++)
		if(view->sel_seqs[num]) break;
gc = get_ncbi_gc_from_comment(view->comments[num]);
Fl_Hold_Browser *obj = new Fl_Hold_Browser(5, 25, 840, 350);
obj->textfont(FL_COURIER);
sprintf(line, "Set genetic code for %s", view->tot_sel_seqs > 1 ? "selected sequences" : view->seqname[num]);
obj->copy_label(line);
obj->align(FL_ALIGN_TOP);
for(num = 0; num < totcodes; num++) {
	sprintf(line, "%65.65s|%2d| %s", get_code_target(num), get_ncbi_gc_number(num), get_code_descr(num));
	obj->add(line);
	}
obj->value(get_acnuc_gc_number(gc) + 1);
Fl_Button *b = new Fl_Return_Button(obj->x(), obj->y() + obj->h() + 3, 50, 20, "OK");
b->callback(ask_gc_callback, obj);
b = new Fl_Button(obj->x() + obj->w() - 55 , b->y(), 55, 20, "Cancel");
b->callback(ignore_gc_callback, &status);
b->shortcut(FL_COMMAND | 'w');
win->end();
win->resizable(new Fl_Box(obj->x() + 55, obj->y(), 50, obj->h() ));
win->size_range(100, 100);
win->callback(ignore_gc_callback, &status);
status = -1;
win->show();
#ifndef MICRO
	win->hotspot(win);
#endif
do Fl::wait();
while(status == -1);
delete win;
view->DNA_obj->take_focus();
if(status != 0) {
	gc = get_ncbi_gc_number(status - 1);
	for(num = 0; num < view->tot_seqs; num++) {
		if( view->sel_seqs[num] ) update_comment_gc(num, gc, view);
		}
	}
}


void update_comment_gc(int seqnum, int ncbigc, SEA_VIEW *view)
/* adds or updates /transl_table=xx in seqnum's comment */
{
int l;
char *comm, *p, *q, *r;

comm = view->comments[seqnum];
if(comm == NULL) l = 0;
else l = strlen(comm);

if(l > 0) p = strstr(comm, "/transl_table=");
else p = NULL;
if(p != NULL) {
	q = p+13;
	r = q;
	do r++; while (isdigit(*r));
	*(q+1) = 0;
	p = (char *)malloc(l + 3);
	if(p == NULL) return;
	sprintf(p, "%s%d%s", comm, ncbigc, r);
	}
else {
	p = (char *)malloc(l + 20);
	if(p == NULL) return;
	if(l > 2) strcpy(p, comm); else *p = 0;
	sprintf(p + strlen(p), ";/transl_table=%d\n", ncbigc);
	}
if(comm != NULL) free(comm);
view->comments[seqnum] = p;
set_seaview_modified(view, TRUE);
}
