#include "seaview.h"
#include <stdlib.h>
#include <ctype.h>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Round_Button.H>
#ifdef unix
#include <unistd.h>
#endif
#ifdef WIN32
#define tempnam _tempnam
#endif

#define MAX_MSA_ALGOS 10 //max # of alignment methods allowed in Props menu

/* local prototypes */
int save_part_as_pir(int debut, int fin, char **sequence, char **seqname, 
	int *sel_seqs, int tot_seqs, char *fname, int *withU, int *num_longest, int protein);
int replace_align_part(SEA_VIEW *view, int debut, int fin, char *fname, int withU, int num_longest,
	int *tmpselseqs, align_choice mychoice, const char **err_message);
int calc_gap_sites(char *old_seq, char *new_seq, int lold, int lnew, 
	gap_site *gap_sites, int maxsites);
int insert_gaps_new_align(char **seq, int site, int number, int numseqs, 
	int lseqs);
int reset_stars(SEA_VIEW *view, int debut, int lpart, char **seq, int lfrag, int *tmpselseqs);
int confirm_refer_seq(int num_longest, SEA_VIEW *view, int *tmpselseqs);
char *calc_msa_command(SEA_VIEW *view, char *base_fname, void (*alert)(const char *,...) );
void cre_align_menu(SEA_VIEW *view);
void align_opt_callback(Fl_Widget *ob, long unused);
char *get_algo_opts(int algonum /* from 0 */, char **presname);
char *get_opt_resname(int algonum);
void set_algo_option_item(char *opts, Fl_Menu_Item *menu, int itemrank, int checkit);
void add_align_callback(Fl_Widget *ob, void *data);
void delete_align_callback(Fl_Widget *ob, void *data);
void add_align_w_callback(Fl_Widget *ob, void *data);
void edit_align_callback(Fl_Widget *ob, void *data);
void init_msa_algos(void);
void profile_align(SEA_VIEW *view);
void replace_stars(char *fname);
void back_to_stars(char *aligned, char *ori);
int save_fasta_file_no_gaponly(char *fname, char **sequence, int tot_seqs, int *each_length, int *sel_seqs);
void unalign(SEA_VIEW *view);
int reorder_seqs(int tot, char **seq, char **name);
int check_alignment_characters(SEA_VIEW *view, int *sel_seqs);
int command_line_align(int argc, char **argv);

/* extern proto */
extern const char *progname;
extern int run_external_prog_in_pseudoterm(char *cmd, const char *dialogfname, const char *label);
extern int insert_gaps_at(SEA_VIEW *view, int seq, int site, int total);
extern int insert_gap_all_comments(int numgaps, int pos,  SEA_VIEW *view);
extern void del_gap_only_sites(SEA_VIEW *view);
extern char *get_res_value(const char *name, const char *def_value);
extern int int_res_value(const char *name, int def_value);
extern int set_res_value(const char *name, const char *value);
extern void delete_res_value(const char *name);
extern void save_resources(void);
extern char *create_tmp_filename(void);
extern void delete_tmp_filename(const char *base_fname);
extern void direct_help_callback(Fl_Widget *wgt, void *data);
extern int compute_size_params(SEA_VIEW *view, int force_recompute);
extern char **prepcolranks_by_codon(char **seq, int totseqs, int maxlen, int *eachlength, char **comments);
extern int is_view_valid(SEA_VIEW *view);
extern int calc_max_seq_length(int seq_length, int tot_seqs);
extern void allonge_seqs(char **seq, int totseqs, int maxlen, int *eachlength,
			 int tot_comment_lines, char **comment_line, char **pregion_line);
extern SEA_VIEW *cmdline_read_input_alignment(int argc, char **argv);
extern bool isarg(int argc, char *argv[], const char *arg);
extern float argval(int argc, char *argv[], const char *arg, float defval);
extern char *argname(int argc, char *argv[], const char *arg);
extern int get_ncbi_gc_from_comment(char *comment);
extern char *translate_with_gaps(char *seq, int gc);
extern char *back_translate_with_gaps(char *prot, char *dna);
extern char *process_output_options(int argc, char **argv, known_format& out_format, bool& std_output);
extern void load_resources(const char *progname);
extern "C" {
  int get_acnuc_gc_number(int ncbi_gc);
}


/* extern variables */
extern gap_site gap_sites[];


int save_part_as_pir(int debut, int fin, char **sequence, char **seqname, 
	int *sel_seqs, int tot_seqs, char *fname, int *withU,
	int *num_longest, int protein)
/* returns TRUE iff error */
{
FILE *out;
int num, pos, l_line, retval, maxlen, seqlen, current, empty_seq;
char line[90], lname[25];

out = fopen(fname,"w");
if( out == NULL ) return TRUE;
retval = TRUE;
*withU = FALSE;
maxlen = 0; current = 0;
for(num = 0; num < tot_seqs; num++) {
	if(! sel_seqs[num] ) continue;
	empty_seq = TRUE;
	l_line = 0;
	seqlen = 0;
	for ( pos = debut - 1 ; pos < fin; pos++) {
		if( sequence[num][pos] == 0 ) break;
		if( sequence[num][pos] == '-' || sequence[num][pos] == '!') continue;
		if(empty_seq) {
			sprintf(lname, "%d_%.10s", ++current, seqname[num]);
			fprintf(out, ">%.10s\n", lname);
 			if(ferror(out)) goto fin;
			empty_seq = FALSE;
			}
		if(l_line >= 70) {
			line[l_line] = 0;
			if(!*withU) *withU = (strchr(line, 'U') != NULL);
			fprintf(out, "%s\n", line);
 			if(ferror(out)) goto fin;
			seqlen += l_line;
			l_line = 0;
			}
		line[l_line++] = sequence[num][pos];
		if(sequence[num][pos] == '*') line[l_line - 1] = 'X';
		if(sequence[num][pos] == '?') line[l_line - 1] = 'X';
		}
	line[l_line] = 0;
	seqlen += l_line;
	if(seqlen > maxlen) {
		maxlen = seqlen; *num_longest = num;
		}
	if(!*withU) *withU = (strchr(line, 'U') != NULL);
	fprintf(out, "%s\n", line);
	if(ferror(out)) goto fin;
	if(empty_seq) sel_seqs[num] = FALSE;
	}
retval = FALSE;
fin:
if( fclose(out) != 0) return TRUE;
return retval;		
}


#if ! ( defined(WIN32)  ||  defined(__APPLE__) )
void align_listener(int fd, void *data)
{
char line[100];
int l = read(fd, line, sizeof(line));
if(l == 0) *(int *)data = TRUE;
}
#endif


void align_selected_parts(SEA_VIEW *view, align_choice my_align_choice, bool no_gui)
{
int debut, fin, status, withU, *tmpselseqs, num_longest, l;
char *p;
char base_fname[PATH_MAX], fname[PATH_MAX];
char *commande;
FILE *in;
  void (*alert)(const char *,...) = no_gui ? Fl::fatal : fl_alert;

if( my_align_choice == ALIGN_SITES && (view->active_region == NULL || view->active_region->list == NULL ||
	view->active_region->list->next != NULL || view->tot_sel_seqs <= 1) ) {
	alert("Need to have exactly one block of selected sites\n"
		"and some selected sequences");
	return;
	}
if(my_align_choice == ALIGN_SELECTED_SEQS) {
	if(view->tot_sel_seqs == 0) return;
	if ( !no_gui && ! fl_choice("Confirm alignment of %d selected sequences ?\n"
					"(Any previous alignment between selected and other sequences will disappear)", 
					"Cancel", "Align", NULL, view->tot_sel_seqs) ) return;
	}
if(my_align_choice == ALIGN_ALL || my_align_choice == ALIGN_SELECTED_SEQS) {
	debut = 1;
	fin = view->seq_length;
	}
else	{
	debut = view->active_region->list->debut;
	fin = view->active_region->list->fin;
	}
p = create_tmp_filename();
if(p == NULL) return;
strcpy(base_fname, p);
sprintf(fname, "%s.pir", base_fname);
/* allonger les seqs plus courtes que la region traitee sans compter comme modif alignment*/
int old_modif_but_not_saved = view->modif_but_not_saved;
for(l = 0; l < view->tot_seqs; l++) {
	if(my_align_choice != ALIGN_ALL && !view->sel_seqs[l]) continue;
	if(fin > view->each_length[l]) insert_gaps_at(view, l + 1, 
			view->each_length[l] + 1, fin - view->each_length[l]);
	}
if (!no_gui) set_seaview_modified(view, old_modif_but_not_saved);
/* prepare list of empty seqs */
tmpselseqs = (int *)malloc(view->tot_seqs * sizeof(int));
if(my_align_choice == ALIGN_ALL)
	memset(tmpselseqs, 1, view->tot_seqs * sizeof(int));
else
	memcpy(tmpselseqs, view->sel_seqs, view->tot_seqs * sizeof(int));
if ( check_alignment_characters(view, tmpselseqs) ) return;
status = save_part_as_pir(debut, fin, view->sequence, view->seqname, 
	tmpselseqs, view->tot_seqs, fname, &withU, &num_longest, view->protein);
//if(view->protein) //clustalw2 now accepts U and muscle also
	withU = FALSE;
if(status) {
	alert("Cannot write sequences to filename\n%s", fname);
	free(tmpselseqs);
	delete_tmp_filename(base_fname);
	return;
	}
if( !no_gui && my_align_choice == ALIGN_SITES ) num_longest = confirm_refer_seq(num_longest, view, tmpselseqs);
if(num_longest == -1 ) {
	free(tmpselseqs);
	delete_tmp_filename(base_fname);
	return;
	}

commande = calc_msa_command(view, base_fname, alert);
status = (commande == NULL);
if(status) {
	free(tmpselseqs);
	delete_tmp_filename(base_fname);
	return;
	}
  if (no_gui) {
#ifdef WIN32
#define system(a) mysystem(a)
    extern int mysystem(const char *command);
#endif
    status = system(commande);
    }
  else status = run_external_prog_in_pseudoterm(commande, NULL, "alignment");
free(commande);
sprintf(fname, "%s.out", base_fname);
if( status == 0 ) {
	in = fopen(fname, "r");
        if(in != NULL) {
		fseek(in, 0, SEEK_END);
		status = (ftell(in) < 10);
		fclose(in);
		}
	else	status = 1;
	}
if (no_gui || is_view_valid(view)) {
  if( status != 0 )
	  alert("%s: alignment operation ignored", extract_filename(view->masename));
  else	{
	  const char *err_message;
	  status = replace_align_part(view, debut, fin, fname, withU, num_longest, tmpselseqs, 
				      my_align_choice, &err_message );
	  if (status) alert("%s", err_message);
	  else if(my_align_choice == ALIGN_ALL) del_gap_only_sites(view);
	  }
}
free(tmpselseqs);
delete_tmp_filename(base_fname);
}


int calc_gap_sites(char *old_seq, char *new_seq, int lold, int lnew, 
	gap_site *gap_sites, int maxsites)
{
char *fin_old, *fin_new;
int tot_sites = 0, posalign = 0;
fin_old = old_seq + lold - 1; fin_new = new_seq + lnew - 1;
while( old_seq <= fin_old || new_seq <= fin_new ) {
	if(old_seq <= fin_old && new_seq <= fin_new && 
		( ( *old_seq != '-' && *new_seq != '-') ||
		  ( *old_seq == '-' && *new_seq == '-') ) ){
		old_seq++; new_seq++;
		posalign++;
		continue;
		}
	if(tot_sites >= maxsites) return -1;
	gap_sites[tot_sites].l[1] = 0;
	gap_sites[tot_sites].l[0] = 0;
	if(old_seq <= fin_old && *old_seq == '-') {
		gap_sites[tot_sites].pos = posalign;
		do	{ old_seq++; ++(gap_sites[tot_sites].l[1]); }
		while( *old_seq == '-' && old_seq < fin_old);
		posalign += gap_sites[tot_sites].l[1];
		}
	else 	{
		gap_sites[tot_sites].pos = posalign;
		do	{ new_seq++; ++(gap_sites[tot_sites].l[0]); }
		while( *new_seq == '-' && new_seq < fin_new);
		posalign += gap_sites[tot_sites].l[0];
		}
	tot_sites++;
	}
return tot_sites;
}


int replace_align_part(SEA_VIEW *view, int debut, int fin, char *fname, 
	int withU, int num_longest, int *tmpselseqs, align_choice mychoice,
		       const char **err_message)
/* returns TRUE if error, FALSE if ok */
{
int num, lfrag, lpart, i, rang, retval, newlength, lfrag2, total;
char **seq, **comments, **seqname, *pheader;
int (*calc_color_function)(int);
int totgapsites, num1, site, l_copy, res;
char *tmp;

total = 0;
for(num = 0; num < view->tot_seqs; num++) if(tmpselseqs[num]) total++;
view->cursor_in_comment = FALSE;
view->cursor_seq = view->first_seq;
/* lecture de l'alignement multiple produit par clustalw */
num = read_fasta_align(fname, &seq, &seqname, &comments, &pheader, (char **)err_message, FALSE);
if(num == total && reorder_seqs(num, seq, seqname) ) {
  *err_message = "Error: not enough memory.";
  return TRUE;
  }
for(i = 0; i < num; i++) free(seqname[i]);
if(num > 0) { free(seqname); free(comments); }
if( num != total) {
	if(num > 0) free(seq); 
  static char *message = NULL;
  if (message) free(message);
  message = (char*)malloc(strlen(fname) + 100);
  sprintf(message, "Error in file of aligned sequences:\n%s", fname);
  *err_message = message;
	return TRUE; 
	}

retval = TRUE;
if(withU) { 
/* si seq avec U au depart, les remettre car ont ete changes en T par clustalw */
	char *p, *q;
 	for(num = 0; num < total; num++) {
 		p = seq[num];
 		while( (q = strchr(p, 'T')) != NULL) { *q = 'U'; p = q; }
		}
	}	
lfrag = strlen(seq[0]); /* long alignement de clustalw */
lpart = fin - debut + 1; /*long region traitee de l'ancien alignement multiple*/
if( (num = reset_stars(view, debut, lpart, seq, lfrag, tmpselseqs)) ) {
  *err_message = "Error: mismatch between alignment input and output.";
  return TRUE;
  }
if(view->numb_gc > 1) {
	calc_color_function = ( view->protein ? get_color_for_aa : get_color_for_base );
	}
if(mychoice == ALIGN_SITES) {
	/* num1 = rang dans align clustalw de num_longest dans align multiple */
	num1 = -1;
	for(i = 0; i < view->tot_seqs; i++) {
		if( ! tmpselseqs[i] ) continue;
		num1++;
		if( i == num_longest) break;
		}
	/* calcul des pos et longs de gaps a inserer:
	gap_sites[x].pos = position a droite du gap a inserer (from 0)
	gap_sites[x].l[0] = longueur a inserer dans alignement multiple
	gap_sites[x].l[1] = longueur a inserer dans alignement produit par clustalw
	*/
	totgapsites = calc_gap_sites(view->sequence[num_longest] + debut - 1, seq[num1],
		FL_min(lpart, view->each_length[num_longest] - debut + 1),
		lfrag, gap_sites, MAX_GAP_SITES);
	if(totgapsites == -1) {
	  *err_message = "Error: reduce length of aligned sequences or increase parameter MAX_GAP_SITES.";
	  goto fin;
	  }
	/* calcul long region traitee apres ajout des gaps */
	for(site=0; site<totgapsites; site++) 
		lpart += gap_sites[site].l[0];
	/* l'alignement multiple serait-il trop long en fin d'operation? */
	if( view->seq_length + lpart - (fin - debut + 1) > view->max_seq_length ) {
	  *err_message = "Error: reduce length of aligned sequences.";
		goto fin;
	  }
	/* calcul long alignement de clustalw apres ajout des gaps */
	lfrag2 = lfrag;
	for(site = 0; site < totgapsites; site++) 
		lfrag2 += gap_sites[site].l[1];
	/* allongement memoire pour seqs de l'alignement de clustalw */
	for(num=0; num<total; num++) {
		tmp = (char *)malloc(lfrag2+1);
		if(tmp == NULL) {
		  *err_message = "Error: not enough memory.";
		  goto fin;
		  }
		memcpy(tmp, seq[num], lfrag+1);
		free(seq[num]);
		seq[num] = tmp;
		}
	/* allongement des seqs de l'alignement de clustalw */
	for(site = 0; site < totgapsites; site++) {
		if(gap_sites[site].l[1] == 0) continue;
		lfrag = insert_gaps_new_align(seq, gap_sites[site].pos, 
			gap_sites[site].l[1],
			total, lfrag);
		}
	newlength = view->seq_length;
	/* allongement des seqs de l'alignement multiple */
	for(site = 0; site < totgapsites; site++) {
		if(gap_sites[site].l[0] == 0) continue;
		for(num = 0; num < view->tot_seqs; num++) {
			insert_gaps_at(view, num + 1, 
				debut + gap_sites[site].pos, gap_sites[site].l[0]);
			}
		newlength += gap_sites[site].l[0];
		insert_region_part(view, debut + gap_sites[site].pos, 
			gap_sites[site].l[0]);
		if(view->tot_comment_lines > 0) insert_gap_all_comments(
			gap_sites[site].l[0], debut + gap_sites[site].pos, view);
		}

	/* copie des sequences de clustalw vers alignement multiple */
	rang = -1;
	for(num = 0; num < view->tot_seqs; num++) {
		if(  ! tmpselseqs[num] ) continue;
		rang++;
	/* on met les nouvelles seqs */ 
		l_copy = FL_min(lpart, lfrag);
		memcpy( view->sequence[num] + debut - 1, seq[rang], l_copy);
		if(debut + l_copy - 1 > view->each_length[num])
			view->each_length[num] = debut + l_copy - 1;
		newlength = FL_max(newlength, view->each_length[num]);
		if(view->each_length[num] == debut + l_copy - 1) 
			view->sequence[num][debut + l_copy - 1] = 0;
		if(view->numb_gc == 1) continue;
		for(i = debut - 1; i < debut + l_copy - 1; i++) {
			res = view->sequence[num][i];
			if (view->col_rank) view->col_rank[num][i] = (char)calc_color_function( res );
			}
		}
	}
else {
  if (lfrag > view->max_seq_length) {
    view->max_seq_length = calc_max_seq_length(lfrag, view->tot_seqs);
    for (num = 0; view->col_rank && num < view->tot_seqs; num++) {
      free(view->col_rank[num]);
      }
    if (view->col_rank) free(view->col_rank);
    allonge_seqs(view->sequence, view->tot_seqs, view->max_seq_length, view->each_length, 
		 view->tot_comment_lines, view->comment_line, &view->region_line);
    if (view->col_rank) view->col_rank = prepcolranks(view->sequence, view->tot_seqs, view->max_seq_length, view->each_length, 
				  ( view->protein ? get_color_for_aa : get_color_for_base ), 
				  view->numb_gc, view->allow_lower);
  }	   
  newlength = view->seq_length;
	rang = -1;
	for(num = 0; num < view->tot_seqs; num++) {
		if(  ! tmpselseqs[num] ) continue;
		rang++;
		/* on met les nouvelles seqs */
		l_copy = strlen(seq[rang]);
		strcpy(view->sequence[num], seq[rang]);
		view->each_length[num] = l_copy;
		newlength = FL_max(newlength, l_copy);
		if(view->numb_gc > 1) {
			for (i = 0; view->col_rank && i < view->each_length[num]; i++) {
				res = view->sequence[num][i];
				view->col_rank[num][i] = (char)calc_color_function( res );
				}
			}
	}
}
  if (view->col_rank) {
    /* mettre a jour horsli */
    if(mychoice == ALIGN_ALL) {
	    view->seq_length = 0;
	    for(num = 0; num < view->tot_seqs; num++) view->seq_length = FL_max(view->seq_length, view->each_length[num]);
	    compute_size_params(view, TRUE);
	    view->horsli->redraw();
	    }
    else update_current_seq_length(newlength, view);
    set_seaview_modified(view, TRUE);
  }
  retval = FALSE;
fin:
for(num = 0; num < total; num++) 
	free(seq[num]);
free(seq);
return retval;
}


int insert_gaps_new_align(char **seq, int site, int number, int numseqs, 
	int lseqs)
{
int num;
char *pos;
for(num=0; num < numseqs; num++) {
	pos = seq[num] + site;
	memmove(pos + number, pos, lseqs - site + 1);
	memset(pos, '-', number);
	}
return lseqs + number;
}


int reset_stars(SEA_VIEW *view, int debut, int lpart, char **seq, int lfrag, int *tmpselseqs)
/* returns 0 iff OK, or mismatch between alignment input and output
 */
{
int oldseq, newseq;
char *p, *q;

newseq = -1;
for(oldseq = 0; oldseq < view->tot_seqs; oldseq++) {
	if(! tmpselseqs[oldseq] ) continue;
	newseq++;
	p = view->sequence[oldseq] + debut - 2;
	q = seq[newseq] - 1;
	while(TRUE) {
		p++; q++;
		while (*p == '-' || *p == '!') p++;
		while (*q == '-' || *q == '!') q++;
		if(*p == 0 || *q == 0) break;
		if( *p == '*' && *q == 'X') *q = '*';
		if( *p == '?' && *q == 'X') *q = '?';
		if( toupper(*p) != toupper(*q) ) return 1;
		if( islower(*p) ) *q = *p;
		}
	}
return 0;
}


int confirm_refer_seq(int num_longest, SEA_VIEW *view, int *tmpselseqs)
{
static Fl_Window *form;
static int first = TRUE;
static Fl_Browser *browser_noms;
static Fl_Button *ok_button, *cancel_button;
int lnum, i;
if(first) {
	first = FALSE;
	form = new Fl_Window( 300, 400);
	form->label("Reference Sequence");
	form->box(FL_FLAT_BOX);
	fl_font(FL_HELVETICA, FL_NORMAL_SIZE);
	browser_noms = new Fl_Browser(5, 5 + fl_height(), form->w() - 10, 
		form->h() - 35 - fl_height(), "Choose one seq.");
	browser_noms->type(FL_HOLD_BROWSER);
	browser_noms->textsize(FL_NORMAL_SIZE);
	browser_noms->has_scrollbar(Fl_Browser_::VERTICAL);
	browser_noms->align(FL_ALIGN_TOP);
	browser_noms->color(FL_LIGHT1, browser_noms->selection_color());
	ok_button = new Fl_Return_Button(5, form->h() - 25, 
		browser_noms->w() / 2, 20, "OK");
	cancel_button = new Fl_Button(ok_button->x() + ok_button->w(), 
		ok_button->y(), ok_button->w(), ok_button->h(), "Cancel");
	form->resizable(browser_noms);
	form->end();
	form->set_modal();
	}
browser_noms->clear();
lnum = 0;
for(i = 0; i < view->tot_seqs; i++) {
	if(!tmpselseqs[i]) continue;
	lnum++;
	browser_noms->add(view->seqname[i]);
	if(i == num_longest) browser_noms->value(lnum);
	}
form->show();
for (;;) {
	Fl_Widget *o = Fl::readqueue();
	if (!o) Fl::wait();
	else if (o == cancel_button || o == form) { num_longest = -1; break; }
	else if(o == ok_button && browser_noms->value() != 0) break;
	}
form->hide();
if(num_longest == -1) return -1;
Fl::flush();

lnum = browser_noms->value();
for(i = 0; i < view->tot_seqs; i++) {
		if(!tmpselseqs[i]) continue;
		if(strcmp(view->seqname[i], browser_noms->text(lnum)) == 0) {
			num_longest = i;
			break;
			}
		}
return num_longest;
}


char *calc_msa_command(SEA_VIEW *view, char *base_fname, void (*alert)(const char *,...) )
{
char *command;
char *algo_name;
char local[400], tmp[400], *p, *optargs;
int status;

sprintf(tmp, "msa_name_%d", view->alignment_algorithm +  1);
algo_name = strdup( get_res_value(tmp, NULL) );
	
/* check if program present */
#if defined(WIN32)
char w32path[400];
strcpy(tmp, algo_name);
if(strchr(algo_name, '\\') == NULL && strchr(algo_name, '.') == NULL) strcat(tmp, ".exe");
p = get_full_path(tmp);
if(p != NULL) strcpy(w32path, p);
status = (p == NULL);
#else
status = (get_full_path(algo_name) == NULL);
#endif
if(status) {
	alert("Alignment is impossible because program  '%s'  is not found\n"
	"in PATH or seaview's directory", algo_name);
	free(algo_name);
	return NULL;
	}
	
sprintf(tmp, "msa_args_%d", view->alignment_algorithm +  1);
strcpy(local, get_res_value(tmp, NULL));

/* add user-entered options to command */
int clustalopt = ALIGN_OPTIONS + 1;
optargs = get_algo_opts(view->alignment_algorithm, NULL);
status = view->menu_align[clustalopt + MAX_MSA_ALGOS ].flags;
if(optargs != NULL && (status & FL_MENU_VALUE) ) {
	if (strchr(local, '>') != NULL) {
	  sprintf(tmp, "%s %s", optargs, local);
	  }
	else {
		sprintf(tmp, "%s %s", local, optargs);
		}
	strcpy(local, tmp);
	}
  
  if(view->alignment_algorithm == 1 && (p = strstr(local, " -stable")) != NULL) {
    //make sure -stable is not in muscle arguments because it's buggy
    memmove(p, p+8, strlen(p+8)+1);
  }
  
/* replace all %f by file names */
while((p = strstr(local, "%f")) != NULL) {
  char *q, *r, s;
  *p = 0;
  q = p;
  do q--; while (q > local && *q != ' ');
  q++;
  r = p;
  do r++; while (*r != ' ' && *r != 0);
  s = *q;
  *q = 0;
  strcpy(tmp, local);
  *q = s;
  s = *r;
  *r = 0;
  sprintf(tmp + strlen(tmp), "\"%s%s%s\"", q, base_fname, p+2);
  *r = s;
  if (*r) strcat(tmp, r);
  strcpy(local, tmp);
  }
/* build complete command string */
#if defined(WIN32)
	command = (char *)malloc(strlen(w32path) + strlen(local) + 30);
	sprintf(command, "\"%s\" %s ", w32path, local);
#else
	command = (char *)malloc(strlen(get_full_path(algo_name)) + strlen(local) + 10);
	sprintf(command, "\"%s\" %s ", get_full_path(algo_name), local);
#endif
free(algo_name);
return command;
}


//extern void fix_paste_timeout(void *u);

void align_menu_callback(Fl_Widget *ob, void *data)
{
  SEA_VIEW *view = (SEA_VIEW *) ob->user_data();
  int reponse = ((Fl_Menu_*)ob)->mvalue() - view->menu_align;
	//Fl::remove_timeout(fix_paste_timeout);
	if(reponse <= ALIGN_SITES) { 
	  align_selected_parts(view, (align_choice)reponse );
	  if(is_view_valid(view)) {
	    view->DNA_obj->redraw();
	    view->horsli->redraw();
	    fl_reset_cursor(view->dnawin);
	    }
	}
	else if(reponse == PROFILE) {
	  if(view->tot_sel_seqs == 0) return;
	  profile_align(view);
	  if(is_view_valid(view)) {
	    view->DNA_obj->redraw();
	    view->horsli->redraw();
	    fl_reset_cursor(view->dnawin);
	    }
	}
	else if(reponse == UNALIGN) {
		if(view->tot_sel_seqs == 0) return;
		unalign(view);
		view->DNA_obj->redraw();
		view->horsli->redraw();
	}
	//Fl::add_timeout(0.5, fix_paste_timeout);
}


void cre_align_menu(SEA_VIEW *view)
{
  char msa_name[60], *name; 
  int elt, attr;
  const int clustalopt = ALIGN_OPTIONS + 1;//keep to rank in menu of first alignment option
  
  static const Fl_Menu_Item alignitems_static[] = {
    {"Align all", 0, align_menu_callback, 0, 0},
    {"Align selected sequences", 0, align_menu_callback, 0, 0},
    {"Align selected sites", 0, align_menu_callback, 0, 0},
    {"Profile alignment", 0, align_menu_callback, 0, FL_MENU_DIVIDER},
    {"De-align selection", 0, align_menu_callback, 0, FL_MENU_DIVIDER},
    {"Alignment options", 0, align_menu_callback, 0, FL_SUBMENU},
    
    {"<none>", 0, NULL, 0, FL_MENU_TOGGLE | FL_MENU_INACTIVE},
    {"Edit options", 0, edit_align_callback, 0, FL_MENU_DIVIDER},
    {"Add external method", 0, add_align_w_callback, 0, 0},
    {"Delete method", 0, delete_align_callback, 0, 0},
    {0},
    {0}
  };
  
  view->alignment_algorithm = int_res_value("alignment", 0);
  view->count_msa_algos = int_res_value("msa_algo_count", 0);

  Fl_Menu_Item* alignitems = new Fl_Menu_Item[sizeof(alignitems_static)/sizeof(Fl_Menu_Item) + MAX_MSA_ALGOS];
  memcpy(alignitems, alignitems_static, 6 * sizeof(Fl_Menu_Item));
  memset(alignitems + 6, 0, MAX_MSA_ALGOS * sizeof(Fl_Menu_Item));
  memcpy(alignitems + 6 + MAX_MSA_ALGOS, alignitems_static + 6, 6 * sizeof(Fl_Menu_Item));

  for(elt = 1; elt <= MAX_MSA_ALGOS; elt++) {
    attr = FL_MENU_RADIO; 
    alignitems[clustalopt + elt - 1].callback(align_opt_callback, elt - 1);
    if (elt <= view->count_msa_algos) {
      sprintf(msa_name, "msa_name_%d", elt);
      name = get_res_value(msa_name, NULL);
      alignitems[clustalopt + elt - 1].label(strdup(name));
      if (elt == view->count_msa_algos) attr |= FL_MENU_DIVIDER;
#ifndef WIN32
      if (get_full_path(name) == NULL) {
	attr |= FL_MENU_INACTIVE;
	} 
#endif
      alignitems[clustalopt + elt - 1].flags = attr;
      }
    else {
      alignitems[clustalopt + elt - 1].label("");
      alignitems[clustalopt + elt - 1].flags = FL_MENU_INVISIBLE;
      }
    }
  (alignitems + clustalopt + view->alignment_algorithm)->set();
  name = get_algo_opts(view->alignment_algorithm, NULL);
  char *options = new char[100 + (name ? strlen(name) : 0) ];
  sprintf(options, "%s", name == NULL ? "<none>" : name);
  attr = FL_MENU_TOGGLE;
  if(name == NULL) attr |= FL_MENU_INACTIVE;
  alignitems[clustalopt + MAX_MSA_ALGOS].label(strdup(options));
  alignitems[clustalopt + MAX_MSA_ALGOS].flags = attr;
  if (view->alignment_algorithm < 2) alignitems[clustalopt + MAX_MSA_ALGOS + 3].flags = FL_MENU_INACTIVE;
  delete[] options;
  view->menu_align = alignitems;
  view->menubar->add("Align", 0, NULL, (void*)view->menu_align, FL_SUBMENU_POINTER);
  if (view->count_msa_algos >= MAX_MSA_ALGOS) (alignitems + clustalopt + MAX_MSA_ALGOS + 2)->deactivate();
}


void add_align_callback(Fl_Widget *ob, void *data)
{
if(strcmp(ob->label(), "OK") != 0) {
	ob->window()->hide();
	return;
	}
char tmp[200];
int doit = TRUE, x;
int clustalopt = ALIGN_OPTIONS + 1;
SEA_VIEW *view = (SEA_VIEW *)ob->window()->user_data();
Fl_Menu_Item *menu_align = (Fl_Menu_Item *)view->menu_align;

Fl_Window *w = ob->window();
const char *name = ((Fl_Input *)w->child(0))->value();
const char *args = ((Fl_Input *)w->child(1))->value();
for(x = 0; x < view->count_msa_algos; x++) {
	if(strcmp(name, (menu_align + clustalopt + x)->label() ) == 0) doit = FALSE;
	}
if(doit) doit = (name != NULL && args != NULL && strlen(name) >= 2 && strlen(args) >= 2);
#ifdef WIN32
strcpy(tmp, name);
if(strchr(tmp, '\\') == NULL && strchr(tmp, '.') == NULL) strcat(tmp, ".exe");
if(get_full_path(tmp) == NULL) 
#else
if(get_full_path(name) == NULL) 
#endif
	{
	fl_alert("Program  '%s'  not found.\nMethod won't be added.", name);
	doit = FALSE;
	}
if(doit) {
	menu_align[clustalopt + view->count_msa_algos].label(strdup(name));
	int stat = menu_align[clustalopt + view->count_msa_algos - 1 ].flags;
  menu_align[clustalopt + view->count_msa_algos - 1 ].flags = stat & ~FL_MENU_DIVIDER;
  menu_align[clustalopt + view->count_msa_algos ].flags = FL_MENU_RADIO | FL_MENU_DIVIDER;
	view->alignment_algorithm = view->count_msa_algos;
	(menu_align + clustalopt + view->alignment_algorithm)->setonly();
	set_algo_option_item(NULL, menu_align, clustalopt + MAX_MSA_ALGOS, FALSE);
	(menu_align + clustalopt + MAX_MSA_ALGOS + 3)->activate();
	++(view->count_msa_algos);
	sprintf(tmp, "%d", view->count_msa_algos);
	set_res_value("msa_algo_count", tmp);
	sprintf(tmp, "msa_name_%d", view->count_msa_algos);
	set_res_value(tmp, name);
	sprintf(tmp, "msa_args_%d", view->count_msa_algos);
	set_res_value(tmp, args);
	save_resources();
  if (view->count_msa_algos == MAX_MSA_ALGOS) (menu_align + clustalopt + MAX_MSA_ALGOS + 2)->deactivate();
	}
w->hide();
}


void delete_align_callback(Fl_Widget *ob, void *data)
{
int clustalopt = ALIGN_OPTIONS + 1;
SEA_VIEW *view = (SEA_VIEW *)ob->user_data();
Fl_Menu_Item *menu_align = (Fl_Menu_Item*)view->menu_align;
char msa_name[20], *name;
int num, status;

sprintf(msa_name, "msa_name_%d", view->alignment_algorithm + 1);
name = get_res_value(msa_name, NULL);
if(name == NULL) return;
int rep = fl_choice("Delete alignment method  '%s'  ?", "Keep", "Delete", NULL, name);
if(rep == 0) return;
  free((void*) (menu_align + clustalopt + view->alignment_algorithm)->label() );
for(num = view->alignment_algorithm + 1; num <= view->count_msa_algos - 1; num++) {
	name = (char *)(menu_align + clustalopt + num)->label();
	(menu_align + clustalopt + num - 1)->label(name);
	
	sprintf(msa_name, "msa_name_%d", num + 1);
	name = get_res_value(msa_name, NULL);
	sprintf(msa_name, "msa_name_%d", num);
	set_res_value(msa_name, name);

	sprintf(msa_name, "msa_args_%d", num + 1);
	name = get_res_value(msa_name, NULL);
	sprintf(msa_name, "msa_args_%d", num);
	set_res_value(msa_name, name);
	
	name = get_opt_resname(num);
	name = get_res_value(name, NULL);
	if(name != NULL) {
	  char *tmp = strdup(name);
	  name = get_opt_resname(num - 1);
	  set_res_value(name, tmp);
	  free(tmp);
	  }
	else delete_res_value(get_opt_resname(num - 1));
	}
  menu_align[clustalopt + view->count_msa_algos - 1].flags = FL_MENU_INVISIBLE;
  menu_align[clustalopt + view->count_msa_algos - 1].label("");
sprintf(msa_name, "msa_name_%d", view->count_msa_algos);
delete_res_value(msa_name);
sprintf(msa_name, "msa_args_%d", view->count_msa_algos);
delete_res_value(msa_name);
name = get_opt_resname(view->count_msa_algos - 1);
delete_res_value(name);
--(view->count_msa_algos);
sprintf(msa_name, "%d", view->count_msa_algos);
set_res_value("msa_algo_count", msa_name);
status = menu_align[ clustalopt + view->count_msa_algos - 1 ].flags;
menu_align[ clustalopt + view->count_msa_algos - 1 ].flags = status | FL_MENU_DIVIDER ;
num = int_res_value("alignment", 0);
view->alignment_algorithm = (num < view->count_msa_algos ? num : 0);
  sprintf(msa_name, "%d", view->alignment_algorithm);
  set_res_value("alignment", msa_name);
(menu_align + clustalopt + view->alignment_algorithm)->setonly();
name = get_algo_opts(view->alignment_algorithm, NULL);
set_algo_option_item(name, menu_align, clustalopt + MAX_MSA_ALGOS, FALSE);
  Fl_Menu_Item* item = (Fl_Menu_Item*)view->menubar->mvalue();
if (view->alignment_algorithm >= 2) item->activate();
else item->deactivate();
save_resources();
}


void choose_align_callback(Fl_Widget *ob, void *data)
{
Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
chooser->type(Fl_Native_File_Chooser::BROWSE_FILE);   // let user browse a single file
chooser->title("Select external alignment program");                        
#if defined(WIN32)
	chooser->filter("*.exe");                 
#endif
char *filename = run_and_close_native_file_chooser(chooser);
if(filename == NULL) return;
((Fl_Input*)data)->value(filename);
((Fl_Input*)data)->position(strlen(filename));
}


void add_align_w_callback(Fl_Widget *ob, void *data)
{
static Fl_Window *w = NULL;
static Fl_Input *name, *args;
Fl_Button *chooser;
SEA_VIEW *view = (SEA_VIEW *)ob->user_data();
if(view->count_msa_algos >= MAX_MSA_ALGOS) return;
if(w == NULL) {
	w = new Fl_Window(530, 100, "alignment method creation");
	name = new Fl_Input(85, 10, 250, 20, "Name");
	name->tooltip("Enter name of program from PATH"
#ifdef WIN32
	" or seaview's directory"
#endif
	" or select program using button at right");
	args = new Fl_Input(85, 40, 430, 20, "Arguments");
	args->tooltip(
		"probcons example: %f.pir   >   %f.out\n"
		"t_coffee example: %f.pir -outfile=%f.out -output=fasta_aln\n"
		"mafft example: --auto %f.pir  >  %f.out"
	);
	chooser = new Fl_Button(345, 10, 160, 20, "Select external program");
	chooser->callback(choose_align_callback, (void *)name);
	Fl_Return_Button *b = new Fl_Return_Button(480, 70, 40, 20, "OK");
	b->callback(add_align_callback, data);
	Fl_Button *b2 = new Fl_Button(10, 70, 50, 20, "Cancel");
	b2->callback(add_align_callback, NULL);
	}
name->value(NULL);
args->value(NULL);
name->take_focus();
w->user_data(view);
w->show();
#ifndef MICRO
w->hotspot(w);
#endif
}


void edit_align_callback(Fl_Widget *ob, void *data)
{
SEA_VIEW *view = (SEA_VIEW *)ob->user_data();
Fl_Menu_Item *menu_align = (Fl_Menu_Item *)view->menu_align;
char *opts, *optresname;
char resname[20], *name;
int clustalopt = ALIGN_OPTIONS + 1;

sprintf(resname, "msa_name_%d", view->alignment_algorithm + 1);
name = get_res_value(resname, NULL);
opts = get_algo_opts(view->alignment_algorithm, &optresname);
opts = (char *)fl_input("Optional arguments for method   '%s'\n"
"(Clustal ex: --threads=4  | Muscle ex: -maxiters 2 -diags)",
	opts, name);
if(opts == NULL) return;
if(strlen(opts) == 0) opts = NULL;
set_algo_option_item(opts, menu_align, clustalopt + MAX_MSA_ALGOS, TRUE);
if(opts != NULL) set_res_value(optresname, opts);
else delete_res_value(optresname);
save_resources();
}


void align_opt_callback(Fl_Widget *ob, long unused)
{
SEA_VIEW *view = (SEA_VIEW *)ob->user_data();
char *opts;
int clustalopt = ALIGN_OPTIONS + 1;

int reponse =  ((Fl_Menu_*)ob)->mvalue() - view->menu_align - clustalopt;
if(reponse >= 0 && reponse <  view->count_msa_algos) { 
	view->alignment_algorithm = reponse;  
	opts = get_algo_opts(view->alignment_algorithm, NULL);
	set_algo_option_item(opts, (Fl_Menu_Item*)view->menu_align, clustalopt + MAX_MSA_ALGOS, FALSE);
	if(view->alignment_algorithm >= 2) ((Fl_Menu_Item *)view->menu_align + clustalopt + MAX_MSA_ALGOS + 3)->activate();
	else ((Fl_Menu_Item *)view->menu_align + clustalopt + MAX_MSA_ALGOS + 3)->deactivate();
	}
}


char *get_algo_opts(int algonum /* from 0 */, char **presname)
{
char *resname = get_opt_resname(algonum);
if(presname != NULL) *presname = resname;
return get_res_value(resname, NULL);
}


char *get_opt_resname(int algonum)
{
static char resname[20];
sprintf(resname, "msa_opt_args_%d", algonum + 1);
return resname;
}


void set_algo_option_item(char *opts, Fl_Menu_Item *menu, int itemrank, int checkit)
{
int status;

if(opts != NULL && strlen(opts) == 0) opts = NULL;
  menu[itemrank].label(opts == NULL ? "<none>" : strdup(opts));
status = FL_MENU_TOGGLE;
if(opts == NULL) status |= FL_MENU_INACTIVE;
else if(checkit) status |= FL_MENU_VALUE;
  menu[itemrank].flags = status;
}


void init_msa_algos(void)
{
  int count = int_res_value("msa_algo_count", 0);
  set_res_value("msa_name_1", "clustalo"); 
  set_res_value("msa_args_1", "-v -i %f.pir -o %f.out --outfmt=fasta");
  set_res_value("msa_name_2", "muscle"); 
  set_res_value("msa_args_2", "-in %f.pir -out %f.out");
  if (count < 2) {
    set_res_value("msa_algo_count", "2");
    set_res_value("alignment", "0"); /* clustal-omega is default starting msa method */
  }
}

enum {profile_clustalo, profile_muscle};
static int num_sp_set, profile_to_profile = FALSE, profile_algo = profile_clustalo,
  do_propagate;
int profile_dialog(SEA_VIEW *view)
// returns TRUE iff cancel
{
	int retval, i, y;
	Fl_Window *w = new Fl_Window(270, 185);
	w->label("Profile alignment dialog");
	y = 5;
	Fl_Choice *choice = new Fl_Choice(155, y, 100, 20, "Profile sequences:");
	for(i = 0; i < view->numb_species_sets; i++) {
	  retval = choice->add("TeMpOrArY");
	  choice->replace(retval, view->name_species_sets[i]);
	  }
	choice->value(0);
	y += 25;
	Fl_Choice *choice_algo = new Fl_Choice(155, y, 100, 20, "Alignment algorithm:");
	choice_algo->add("clustalo");
	choice_algo->add("muscle");
	choice_algo->value(profile_algo);
	y = 75;
	Fl_Group *group = new Fl_Group(20, y, w->w() - 40, 55, "Align selected sequences to profile as:");
	group->box(FL_ROUNDED_BOX);
	group->align(FL_ALIGN_TOP|FL_ALIGN_CENTER);
	y += 5;
	Fl_Round_Button *sequence = new Fl_Round_Button(25, y, 200, 20, "each sequence to profile");
	sequence->type(FL_RADIO_BUTTON);
	y += 25;
	Fl_Round_Button *profile = new Fl_Round_Button(25, y, 200, 20, "profile to profile");
	profile->type(FL_RADIO_BUTTON);
	group->end();
	if(profile_algo == profile_muscle) {
		profile->setonly();
		profile_to_profile = TRUE;
		sequence->deactivate();
		}
	else if(profile_to_profile) profile->setonly();
	else sequence->setonly();
	y = group->y() + group->h() + 5;
  Fl_Check_Button *propagate = new Fl_Check_Button(5, y, 170, 20, 
						   "Propagate gaps to sites");
  propagate->type(FL_TOGGLE_BUTTON);
  if(view->regions == NULL && view->tot_comment_lines == 0) propagate->deactivate();
  else propagate->value(1);
	y += 25;
	Fl_Button *cancel_button = new Fl_Button(5, y, 60, 20, "Cancel");
	cancel_button->shortcut(FL_COMMAND | 'w');
	Fl_Button *help_button = new Fl_Button(97, y, 60, 20, "Help");
	Fl_Button *ok_button = new Fl_Return_Button(190, y, 60, 20, "OK");
	w->end();
	w->set_non_modal();
	w->show();
	while (TRUE) {
		Fl_Widget *o = Fl::readqueue();
		if (!o) Fl::wait();
		else if(o == cancel_button || o == w ) { retval = 1; break; }
		else if(o == ok_button ){ retval = 0; break; }
		else if(o == help_button ) { 
		  direct_help_callback(o, (void *)"Align Menu");
		  }
		else if(o == choice_algo) {
			profile_algo = choice_algo->value();
			if(profile_algo == profile_muscle) {
				profile->setonly();
				sequence->deactivate();
				}
			else {
				sequence->activate();
				sequence->setonly();
				}
			}
		}
	num_sp_set = choice->value();
  do_propagate = propagate->value();
	profile_to_profile = profile->value();
	delete w;
	return retval;
}
	
	
void profile_align(SEA_VIEW *view)
{
	char *p, base_fname[200], fname1[200], fname2[200], *command, tmp[200];
	char **seq, **comments, **seqname, *pheader, *err_message, res;
	int withU, num_longest, status, num, total, *table, i, rang, newlength, l_copy;
	int (*calc_color_function)(int);

	if(view->numb_species_sets == 0) {
		fl_alert("Profile alignment requires previous definition of a species group");
		return;
		}
	if( profile_dialog(view) ) return;
	p = create_tmp_filename();
	if(p == NULL) return;
	strcpy(base_fname, p);
	sprintf(fname1, "%s_1.fasta", base_fname);
	total = 0;
	for(num = 0; num < view->tot_seqs; num++) {
		if(view->sel_seqs[num]) total++;
		if(view->list_species_sets[num_sp_set][num] == 0) continue;
		total++;
		if(view->sel_seqs[num]) {
			fl_alert("Selected sequences should not belong to profile sequences");
		  delete_tmp_filename(base_fname);
			return;
			}
		}
	if ( check_alignment_characters(view, view->list_species_sets[num_sp_set]) ) return;
	status = save_fasta_file_no_gaponly(fname1, view->sequence, view->tot_seqs, view->each_length, 
							 view->list_species_sets[num_sp_set]);
	if(status) {
		fl_alert("Cannot write sequences to filename\n%s", fname1);
	  delete_tmp_filename(base_fname);
		return;
		}
	replace_stars(fname1);
	sprintf(fname2, "%s_2.fasta", base_fname);
	if ( check_alignment_characters(view, view->sel_seqs) ) return;
	if(profile_to_profile) {
		status = save_fasta_file_no_gaponly(fname2, view->sequence, view->tot_seqs, view->each_length, 
											view->sel_seqs);
		}
	else status = save_part_as_pir(1, view->seq_length, view->sequence, view->seqname, 
					view->sel_seqs, view->tot_seqs, fname2, &withU, &num_longest, view->protein);
	replace_stars(fname2);
	if(status) {
		fl_alert("Cannot write sequences to filename\n%s", fname2);
	  delete_tmp_filename(base_fname);
		return;
		}
	p = get_res_value( profile_algo == profile_clustalo ? "msa_name_1" : "msa_name_2", NULL);
	strcpy(tmp, p);
#ifdef WIN32
	strcat(tmp, ".exe");
#endif
	p = get_full_path(tmp);
	if(p == NULL) {
		fl_alert("Alignment program %s not found.", tmp);
		status = 1;
		}
	else {
		command = new char[1000];
		if(profile_algo == profile_clustalo) {
			sprintf(command, 
				"\"%s\" -v \"--profile1=%s\" %s%s\"  --outfmt=fasta -o \"%s.aligned\" -t %s", 
				p, fname1, 
				profile_to_profile ? "\"--profile2=" : "-i \"",
				fname2, 
				base_fname,
				view->protein ? "protein" : "dna" );
			}
		else { // muscle
			sprintf(command, 
				"\"%s\" -profile -in1 \"%s\" -in2 \"%s\" -out \"%s.aligned\" ", 
				p, fname1, fname2, base_fname );
			}
		status = run_external_prog_in_pseudoterm(command, NULL, "profile alignment");
		delete[] command;
		}
	sprintf(fname1, "%s.aligned", base_fname);
	/* lecture de l'alignement multiple */
	if( status == 0 ) {
		num = read_fasta_align(fname1, &seq, &seqname, &comments, &pheader, &err_message, FALSE);
		if (num == total && !profile_to_profile) {
		  // clustalo for "each seq to profile" outputs selected seqs first and profile second. We reverse this.
		  for (i = 0; i < total; i++) {
		    if (strncmp(seqname[i], "S_", 2) == 0) break;
		    }
		  if (i > 0) {
		    char **seq2 = (char**)malloc(total * sizeof(char*));
		    memcpy(seq2, seq+i, (total - i)*sizeof(char*));
		    memcpy(seq2+(total-i), seq, i*sizeof(char*));
		    memcpy(seq, seq2, total*sizeof(char*));
		    free(seq2);
		    }
		  }
		if(num > 0) { 	
			for(i = 0; i < num; i++) free(seqname[i]);
			free(seqname); free(comments); 
			}
		if(num != total) { 	
			for(i = 0; i < num; i++) free(seq[i]);
			if(num > 0) free(seq); 
		}
	}
  delete_tmp_filename(base_fname);
	if ( !is_view_valid(view) ) return;
	if( status != 0 || num != total) {
		fl_message("%s: alignment operation ignored", extract_filename(view->masename));
		return; 
		}
	// compute table[rank in file xx.aligned] = rank in seaview alignment
	table = (int *)calloc(total, sizeof(int));
	rang = -1;
	for(num = 0; num < view->tot_seqs; num++) { // seqs from the reference profile
		if(  ! view->list_species_sets[num_sp_set][num] ) continue;
		rang++;
		table[rang] = num;
		back_to_stars(seq[rang], view->sequence[num]);
	}
	for(num = 0; num < view->tot_seqs; num++) { // currently selected seqs
		if(  ! view->sel_seqs[num] ) continue;
		rang++;
		table[rang] = num;
		back_to_stars(seq[rang], view->sequence[num]);
	}
  if(do_propagate && (view->regions != NULL || view->tot_comment_lines > 0) ) {
    //propagate gaps created in profile seqs to regions and comment lines
    int l = 0, num_align = table[0];
    char *p, *q;
    p = view->sequence[num_align]; // one profile seq
    q = seq[0]; // this seq after profile alignment
    while(*p != 0) {
      while(*p != 0 && toupper(*p) == toupper(*q)) {
	p++; q++;
	}
      if(*p == 0 && *q == 0) break;
      if(*p == '-') {
	int from = q - seq[0];
	l = 1;
	p++;
	while(toupper(*p) != toupper(*q)) { p++; l++; }
	for(num = 0; num < total; num++) {
	  seq[num] = (char*)realloc(seq[num], strlen(seq[num]) + l + 1);
	  memmove(seq[num] + from + l, seq[num] + from, strlen(seq[num] + from) + 1);
	  memset(seq[num] + from, '-', l);
	  }
	q = seq[0] + from + l;
	l = 0;
      }
      else if(*q == '-') {
	l = 1;
	q++;
	while(toupper(*p) != toupper(*q)) { q++; l++; }
	}
      if(l > 0) {
	//insert gap of length l in regions
	insert_region_part(view, p - view->sequence[num_align], l);
	if(view->tot_comment_lines > 0) insert_gap_all_comments(l, p - view->sequence[num_align], view);
	l = 0;
      }
    }
  }
	//replace seaview seqs by those from the file
	if(view->numb_gc > 1) {
		calc_color_function = ( view->protein ? get_color_for_aa : get_color_for_base );
	}
	newlength = view->seq_length;
	for(num = 0; num < total; num++) {
		l_copy = strlen(seq[num]);
		if(l_copy > view->max_seq_length) { l_copy = view->max_seq_length; seq[num][l_copy] = 0; }
		strcpy(view->sequence[table[num]], seq[num]);
		free(seq[num]);
		view->each_length[table[num]] = l_copy;
		newlength = FL_max(newlength, l_copy);
		if(view->numb_gc > 1) {
			for(i = 0; i < view->each_length[table[num]]; i++) {
				res = view->sequence[table[num]][i];
				view->col_rank[table[num]][i] = (char)calc_color_function( res );
			}
		}
	}
	free(seq);
	free(table);
	/* mettre a jour horsli */
	update_current_seq_length(newlength, view);
	set_seaview_modified(view, TRUE);
}


void replace_stars(char *fname)
// replace all * and ? in file by X
{
	FILE *in = fopen(fname, "rb+");
	if(in == NULL) return;
	fseek(in, 0, SEEK_END);
	long l = ftell(in);
	char *all = (char *)malloc(l + 1);
	fseek(in, 0, SEEK_SET);
	fread(all, 1, l, in);
	all[l] = 0;
	char *p = all;
	while((p = strchr(p, '*')) != NULL) *p = 'X';
	p = all;
	while((p = strchr(p, '?')) != NULL) *p = 'X';
	fseek(in, 0, SEEK_SET);
	fwrite(all, 1, l, in);
	fclose(in);
}


void back_to_stars(char *aligned, char *ori)
// replaces X in aligned by * when * at same place in ori
// replaces X in aligned by ? when ? at same place in ori
{
	while (TRUE) {
		while(*aligned == '-') aligned++;
		while(*ori == '-') ori++;
		if(*ori == 0 || *aligned == 0) break;
		if(*aligned == 'X' && *ori == '*') *aligned = '*';
		if(*aligned == 'X' && *ori == '?') *aligned = '?';
		aligned++;
		ori++;
		}
}


int save_fasta_file_no_gaponly(char *fname, char **sequence, int tot_seqs, int *each_length, int *sel_seqs)
{
	list_segments *segment, *next;
	region maregion;
	int num, l = 0, pos, insegment, status;
	char **seqname;
	
	segment = NULL;
	insegment = FALSE;
	seqname = (char **)malloc(tot_seqs * sizeof(char *));
	if(seqname == NULL) return 1;
	for(num = 0; num < tot_seqs; num++) {
		if(sel_seqs[num]) l = FL_max(l, each_length[num]);
		seqname[num] = (char *)malloc(10);
		if(seqname[num] == NULL) return 1;
		sprintf(seqname[num], "S_%d", num);
		}
	for(pos = 0; pos < l; pos++) {
		for(num = 0; num < tot_seqs; num++) {
			if(!sel_seqs[num]) continue;
			if(pos < each_length[num] && sequence[num][pos] != '-') break;
			}
		if(num < tot_seqs && ! insegment) {
			next = (list_segments *)malloc(sizeof(list_segments));
			if(segment != NULL) segment->next = next;
			else maregion.list = next;
			segment = next;
			segment->debut = pos + 1;
			insegment = TRUE;
			}
		else if(num >= tot_seqs && insegment) {
			segment->fin = pos;
			insegment = FALSE;
			}
		}
	if(insegment) segment->fin = l;
	segment->next = NULL;
	status = save_fasta_file(fname, sequence, NULL, seqname, tot_seqs, each_length, &maregion, sel_seqs, 1,0);
	segment = maregion.list;
	do	{
		next = segment->next;
		free(segment);
		segment = next;
		}
	while(segment != NULL);
	for(num = 0; num < tot_seqs; num++) {
		free(seqname[num]);
		}
	free(seqname);
	return status;
}


void unalign(SEA_VIEW *view)
{
	int num, i, newlength;
	char *tmpseq, *p, *q, res;
	int (*calc_color_function)(int);
	
	if( fl_choice("This will remove all gaps from selected sequences.", "Cancel", "Remove gaps", NULL)
	   == 0) return;
	if(view->numb_gc > 1) calc_color_function = ( view->protein ? get_color_for_aa : get_color_for_base );
	for(num = 0; num < view->tot_seqs; num++) {
		if(!view->sel_seqs[num]) continue;
		tmpseq = (char *)malloc(view->max_seq_length + 1);
		p = tmpseq; q = view->sequence[num];
		while(*q != 0) {
			if(*q != '-') *(p++) = *q;
			q++;
			}
		*p = 0;
		free(view->sequence[num]);
		view->sequence[num] = tmpseq;
		view->each_length[num] = strlen(tmpseq);
		if(view->numb_gc > 1) {
			if( view->curr_colors == view->codoncolors) {
				char **tmp = prepcolranks_by_codon(&view->sequence[num], 1, view->max_seq_length, 
												   &view->each_length[num], &view->comments[num]);
				free(view->col_rank[num]);
				view->col_rank[num] = tmp[0];
				free(tmp);
				}
			else {
				for(i = 0; i < view->each_length[num]; i++) {
					res = view->sequence[num][i];
					view->col_rank[num][i] = (char)calc_color_function( res );
					}
				}
			}
		}
	newlength = 0;
	for(num = 0; num < view->tot_seqs; num++) newlength = FL_max(newlength, view->each_length[num]);
	view->seq_length = newlength;
	compute_size_params(view, TRUE);
	view->horsli->redraw();
	set_seaview_modified(view, TRUE);
}

int reorder_seqs(int tot, char **seq, char **name)
//reorders sequences in seq in their original order using their names
//that start with 1_, 2_, 3_, etc...
//returns TRUE iff memory error
{
  int num, rank;
  char **newseq = (char **)malloc(tot * sizeof(char *));
  if(newseq == NULL) return TRUE;
  for( num = 0; num < tot; num++) {
    sscanf(name[num], "%d", &rank);
    newseq[rank - 1] = seq[num];
    }
  memcpy(seq, newseq, tot * sizeof(char *));
  free(newseq);
  return FALSE;
}


int check_alignment_characters(SEA_VIEW *view, int *sel_seqs)
//returns TRUE iff unexpected characters are present and alignment shd be cancelled
//otherwise, replaces unexpected characters by X or N and returns FALSE
{
  int i, choice;
  char *p, X;
  const char dna_accepted[] = "ABCDGHKMNRSTUVWXY-*?!";
  const char prot_accepted[] = "ABCDEFGHIKLMNPQRSTVWXYZ-*?!";
  const char *accepted;
  char unexpected[50] = "", single[2] = "";
  
  X = view->protein ? 'X' : 'N';
  accepted = view->protein ? prot_accepted : dna_accepted;
  for(i = 0; i < view->tot_seqs; i++) {
    if(!sel_seqs[i]) continue;
    p = view->sequence[i] - 1;
    while(*++p) {
      if(strchr(accepted, toupper(*p)) == NULL) {
	if(strlen(unexpected) + 1 < sizeof(unexpected) && strchr(unexpected, *p) == NULL) {
	  *single = *p;
	  strcat(unexpected, single);
	}
      }
    }
  }
  if(*unexpected == 0) return FALSE;
  choice = fl_choice("Unexpected characters are present in sequence data '%s'.\n"
		     "You can replace them by '%c' and continue aligning, or cancel alignment:", 
		     "Replace && align", "Cancel", NULL, unexpected, view->protein ? 'X' : 'N');
  if(choice) return TRUE;
  for(i = 0; i < view->tot_seqs; i++) {
    if(!sel_seqs[i]) continue;
    p = view->sequence[i] - 1;
    while(*++p) {
      if(strchr(accepted, toupper(*p)) == NULL) *p = X;
      }
    }
  set_seaview_modified(view, TRUE);
  return FALSE;
}


int command_line_align(int argc, char **argv)
{
  char *p;
  int i;
  bool std_output = false;
  bool need_back_translate = false;
  char *old_opt_args = NULL;
  char opt_name[20];
  SEA_VIEW *DNAview;
  SEA_VIEW *view = cmdline_read_input_alignment(argc, argv);
  known_format out_format = view->format_for_save;
  load_resources(progname);
  init_msa_algos();
  view->alignment_algorithm = (int)argval(argc, argv, "-align_algo", int_res_value("alignment", 0));
  view->menu_align = new Fl_Menu_Item[ALIGN_OPTIONS + MAX_MSA_ALGOS + 2];
  Fl_Menu_Item *item = (Fl_Menu_Item *)&view->menu_align[ALIGN_OPTIONS + MAX_MSA_ALGOS + 1];
  item->clear() ;
  if ( (p = argname(argc, argv, "-align_extra_args")) != NULL) {
    item->set();
    sprintf(opt_name, "msa_opt_args_%d", view->alignment_algorithm + 1);
    old_opt_args = strdup(get_res_value(opt_name, ""));
    set_res_value(opt_name, p);
    }
  
  if ( !view->protein && isarg(argc, argv, "-align_at_protein_level")) {
    need_back_translate = true;
    DNAview = view;
    view = new SEA_VIEW;
    memcpy(view, DNAview, sizeof(SEA_VIEW));
    view->seq_length = 0;
    view->sequence = (char **)malloc(view->tot_seqs * sizeof(char*));
    for (i = 0; i < view->tot_seqs; i++) {
      int gc = (view->comments != NULL ? get_ncbi_gc_from_comment(DNAview->comments[i]) : 1);
      p = translate_with_gaps(DNAview->sequence[i], get_acnuc_gc_number(gc));
      view->sequence[i] = p;
      view->each_length[i] = strlen(p);
      if (view->each_length[i] > view->seq_length) view->seq_length = view->each_length[i];
      view->regions = NULL;
    }
    view->protein = true;
    view->max_seq_length = calc_max_seq_length(view->seq_length, view->tot_seqs);
    allonge_seqs(view->sequence, view->tot_seqs, view->max_seq_length, view->each_length, 
		 view->tot_comment_lines, view->comment_line, &view->region_line);
  }
  
  align_selected_parts(view, ALIGN_ALL, true);

  if (old_opt_args) set_res_value(opt_name, old_opt_args);
  if (need_back_translate) {
    DNAview->seq_length = 0;
    for (i = 0; i < DNAview->tot_seqs; i++) {
      p = back_translate_with_gaps(view->sequence[i], DNAview->sequence[i]);
      free(DNAview->sequence[i]);
      DNAview->sequence[i] = p;
      int l = strlen(p);
      DNAview->each_length[i] = l;
      if (l > DNAview->seq_length) DNAview->seq_length = l;
    }
    view = DNAview;
  }
  
  char *outfile = process_output_options(argc, argv, out_format, std_output);

  char *err_message = save_alignment_or_region(outfile, view->sequence, view->comments,
					       view->header, view->seqname, view->tot_seqs, view->each_length,
					       view->regions, NULL, out_format,
					       view->numb_species_sets, view->list_species_sets,
					       view->name_species_sets, NULL, 0, view->protein,
					       view->tot_comment_lines, view->comment_name, 
					       view->comment_line, view->phylipwidnames,
					       view->tot_trees, view->trees, 
					       view->menu_trees->vitem(0), view->spaces_in_fasta_names);
  if (err_message) {
    fprintf(stderr, "%s\n", err_message);
    if (std_output) delete_tmp_filename(outfile);
    exit(1);
  }
  if (std_output) {
    char line[200];
    FILE *in = fopen(outfile, "r");
    while (fgets(line, sizeof(line), in) != NULL) {
      fputs(line, stdout);
    }
    fclose(in);
    fflush(stdout);
    delete_tmp_filename(outfile);
  }
  return 0;
}

