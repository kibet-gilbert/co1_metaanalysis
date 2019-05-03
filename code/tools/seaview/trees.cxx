#ifndef WIN32
#include <sys/times.h>
#endif
#include "seaview.h"
#include "treedraw.h"
#include "tbe.h"
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#ifdef __APPLE__
#include <FL/x.H> // for fl_mac_os_version
#endif

extern "C" {
#include "phyml_util.h"
	}

typedef enum {observed_pdist, Poisson_pdist, Kimura_pdist} protein_distances;
typedef enum {observed_ndist, Jukes_Cantor, Kimura2P, HKY, LogDet, Ka, Ks} nucleotide_distances;
typedef enum {more_thorough, less_thorough, rearrange_best_tree} dnapars_S_option;

allseq *view_to_allseq(SEA_VIEW *view, int remove_all_gaps);
void gaps_as_unknown_states(allseq *phyml_seqs, int protein);
void free_after_view_to_allseq(allseq *phyml_seqs);
//void initialize_random_series(void);
extern "C" {
  double next_random(void);
  }
void bootstrap_weights(allseq *seqs, int use_codons);
void savedistinfile(matrix *mat, const char *dirname, const char *outfname);
matrix *calc_dist_matrix(allseq *seqs, int distkind, int protein, char **lwlseqs, int in_bootstrap, char **p_err_mess);
void phyml_dialog(SEA_VIEW *view);
static void change_searching_callback(Fl_Widget *ob, void *data);
static void cancel_callback(Fl_Widget *ob, void *data);
static void model_callback(Fl_Widget *ob, void *data);
void run_phyml_callback(Fl_Widget *ob, void *data);
void distance_method_dialog(SEA_VIEW *view);
matrix *run_calc_dist_matrix(allseq *seqs, int distkind, int protein, char **lwlseqs, int in_bootstrap, char **p_err_mess, int no_gui);
char *run_distance_method(SEA_VIEW *view, int distkind, int remove_all_gaps, int use_bionj, int use_bootstrap,
			 int replicates, Fl_Box *box, const char *distance_name, int using_kaks,
			  const char* distancefname, const char *user_tree, int no_gui, void (*alert)(const char*, ...), int keep_b_trees, int compute_tbe );
char *bootstrap_reformat(char *tree, int replicates);
matrix *Obs_Dist(allseq *data, model *mod);
matrix *Kimura_p_Dist(allseq *data);
char *alrt_reformat(char *tree);
matrix *K80_dist_nogamma(allseq *data);
matrix *HKY_dist(allseq *data);
double hky(char* seq1, char* seq2, int lg, int *wght);
int freq_obs(char* seq1, char* seq2, double* freq, int lgseq, int *wght);
int informative(char** sequ, int nb);
void jumble_fct(char** seq, char** name, int notu);
static char *run_parsimony_method(SEA_VIEW *view, int nogaps, const char *gapsunknown, int nreplicates, int njumbles,
			   int maxbest, int user_tree, Fl_Box *w_count, dnapars_S_option s_option,
			   const char **pprogname, float consensus_threshold, int no_gui, char **p_bootstrap_trees, void (*alert)(const char *,...) );
void parsimony_dialog(SEA_VIEW *view);
char *parsimony_with_jumble(char **pars_seqs, char **pars_names, int n_otu, int njumbles, int protein, int *psteps,
	int *pcount, int maxbest, Fl_Widget *w_count, dnapars_S_option s_option, int no_gui, float consensus_threshold);
static char *parsimony_with_bootstrap(allseq *tmpallseq, char **pars_seqs, char **pars_names, int n_otu, int njumbles,
	int protein, int *psteps, int maxbest, int replicates, Fl_Box *w_count, int *preplicates_done, dnapars_S_option s_option, int no_gui,
			       float consensus_threshold, int *pcount_best, char **p_bootstrap_trees);
double *vector(long nl, long nh);
void free_vector(double *v, long nl, long nh);
int ludcmp(double **a, int n, int *indx, double *d);
double det4(double mat[4][4]);
double logdet(char* seq1, char* seq2, int lg, int *wght);
matrix *LOGDET_dist(allseq *data);
char *put_names_back_in_tree(char *oldtree, char **names);
char *replace_tree_names_by_rank(const char *tree, char **names, int notu);
void command_line_phylogeny(int argc, char **argv);
void save_bootstrap_replicates(const char *fname, int replicates, SEA_VIEW *view);


extern void interrupt_callback(Fl_Widget *ob, void *data);
extern int save_phylip_file(const char *fname, char **seq,
					 char **seqname, int totseqs, int *eachlength, region *region_used, 
					 int *sel_seqs, int tot_sel_seqs, int phylipwidnames);
extern int run_external_prog_in_pseudoterm(char *cmd, const char *dialogfname, const char *label);
extern int init_add_bootstrap(char *full_tree);
extern char *get_res_value(const char *name, const char *def_value);
extern int process_one_replicate(char *one_tree, float w);
extern char *finish_add_bootstrap(int replicates);
extern int compare_newick_with_names(const char *tree, char **names, int notu, char **pname);
extern const char *make_binary_or_unrooted(char *arbre);
extern int save_resources(void);
extern int set_res_value(const char *name, const char *value);
extern char *create_tmp_filename(void);
extern void delete_tmp_filename(const char *base_fname);
extern int is_view_valid(SEA_VIEW *view);
char *parsimony(char** seq, char** seqname, int notu, int njumbles, int* steps, char* toevaluate, int arg_maxtrees, 
		int *bt_weights, bool protein, dnapars_S_option s_option, int no_gui, Fl_Widget *w_count);
extern int fl_create_thread(void (*f) (void *), void* p, unsigned);
extern char *argname(int argc, char *argv[], const char *arg);
extern float argval(int argc, char *argv[], const char *arg, float defval);
extern bool isarg(int argc, char *argv[], const char *arg);
extern SEA_VIEW *cmdline_read_input_alignment(int argc, char **argv);
extern void init_consensus_calculation(int n);
extern int process_one_consensus_item(char *one_tree, char **seqnames);
extern int after_consensus_items(float threshold, int item_count, int ***prows);
extern char *compute_consensus_tree(char **seqnames, int **rows, int branch_count);
extern "C" {
  void prefastlwl(void);
  void loadrl(void);
  int fastlwl(char **seq, int nbseq, int lgseq, double **ka, double **ks, 
					double **vka, double **vks, int* sat1, int* sat2, int *wght);
  char* dnapars(char** seq, char** seqname, int notu, int njumbles, int *jumble_no, int *steps, char* toevaluate, int arg_maxtrees, int *bt_weights, dnapars_S_option s_option); 
  char* protpars(char** seq, char** seqname, int notu, int njumbles, int *jumble_no, int *steps, char* toevaluate, int arg_maxtrees, int *bt_weights, dnapars_S_option s_option); 
  char *create_tmp_filename_from_C(void);
  FILE *fl_fopen_from_C(const char *fname, const char *mode);
  int fl_unlink_from_C(const char*fname);
  void awake_from_C(void);
}

int tree_build_interrupted;

char *create_tmp_filename_from_C(void)
{
  return   create_tmp_filename();
}

FILE *fl_fopen_from_C(const char *fname, const char *mode)
{
  return fl_fopen(fname, mode);
}

int fl_unlink_from_C(const char*fname)
{
  return fl_unlink(fname);
}

void trees_callback(Fl_Widget *ob, void *data)
/* 0:Distance  1:PhyML 2:import
 */
{
	SEA_VIEW *view = (SEA_VIEW *)ob->user_data();
	int reponse = ((Fl_Menu_*)ob)->mvalue() - view->menu_trees->get_menu();
	if(reponse <= 2) {
		if(view->tot_seqs < 3 || (view->tot_sel_seqs > 0 && view->tot_sel_seqs < 3) ) {
			fl_alert("Can't draw tree for less than 3 sequences");
			return;
		}
		for(int i = 0; i < view->tot_seqs; i++) {//check that no ,():; in sequence names
			if(view->tot_sel_seqs > 0 && !view->sel_seqs[i]) continue;
			if(strchr(view->seqname[i], '(') != NULL || strchr(view->seqname[i], ')') != NULL || 
			   strchr(view->seqname[i], ',') != NULL || strchr(view->seqname[i], ':') != NULL
			   || strchr(view->seqname[i], ';') != NULL) {
				fl_alert("Sequence: %s\nTrees can't be built with any of (),:; in sequence names", view->seqname[i]);
				return;
				}
			}
		}
	if(reponse == 0) {//Parsimony methods 
	  for (int i = 0; i < view->tot_seqs; i++) {//check that no repeated sequence names
	    if (view->tot_sel_seqs > 0 && !view->sel_seqs[i]) continue;
	    for (int j = i+1; j < view->tot_seqs; j++) {
	      if (view->tot_sel_seqs > 0 && !view->sel_seqs[j]) continue;
	      if (strcmp(view->seqname[i], view->seqname[j]) == 0) {
		fl_alert("The parsimony tree can't be computed if different sequences (#%d, #%d) have the same name: %s", i+1, j+1, view->seqname[i]);
		return;
		}
	      }
	    }
	    parsimony_dialog(view);
	}
	else if(reponse == 1) {//Distance methods 
		distance_method_dialog(view);
	}
	else if(reponse == 2) {//PhyML
		phyml_dialog(view);
	}
	else if(reponse == 3) {//Import tree
		Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
		chooser->type(Fl_Native_File_Chooser::BROWSE_FILE);   // let user browse a single file
		chooser->title("Import treefile");                        
		char *filename = run_and_close_native_file_chooser(chooser);
		if(filename == NULL) return;
		FILE *in = fopen(filename, "r");
		fseek(in, 0, SEEK_END);
		long l = ftell(in);
		fseek(in, 0, SEEK_SET);
		char *tree = (char *)malloc(l + 1);
		char c, *p = tree;
		while((c = fgetc(in)) != EOF) {
			if(c != '\n' && c != '\r') *(p++) = c;
			}
		*p = 0;
		fclose(in);
		treedraw(tree, view, extract_filename(filename), FALSE);
	}
	else if(reponse == 4) {//New empty tree window
		treedraw(NULL, view, NULL, FALSE);
		}
	else {
	        reponse = ((Fl_Menu_*)ob)->mvalue() - view->menu_trees->vitem(0);
		char *p = view->trees[reponse];
		Fl_Window *w = Fl::first_window();
		while(w != NULL) {
			const char *c = w->xclass();
			if(c != NULL && strcmp(c, TREE_WINDOW) == 0 &&
				strcmp( ((FD_nj_plot*)w->user_data())->trees, p) == 0) {
					w->show();
					return;
				}
			w = Fl::next_window(w);
			}
		char *tree = strdup(p);
		treedraw(tree, view, ((Fl_Menu_*)ob)->mvalue()->label(), TRUE);
		}
}


static void distance_callback(Fl_Widget *wgt, void *data)
{
	if( ((Fl_Choice *)wgt)->value() >= Ka) ((Fl_Widget *)data)->deactivate();
	else ((Fl_Widget *)data)->activate();
}

static void dist_b_cb(Fl_Widget *wgt, void *data)
{
  Fl_Button **buttons = (Fl_Button **)data;
  Fl_Button *me = (Fl_Button*)wgt;
  for (int i=0; i<2; i++){
    Fl_Button *target = buttons[i];
    if (target){
      if (me->value()) target->activate();
      else {
        target->deactivate();
        target->value(0);
      }
    }
  }
}

void distance_method_dialog(SEA_VIEW *view)
{
	static int first = TRUE;
	static int def_replicates = 100;
	static int distance_choice = Jukes_Cantor;
	static Fl_Window *w;
	static Fl_Int_Input *replicates;
	static Fl_Box *box;
	static Fl_Choice *distance;
	static Fl_Check_Button *ignore_all_gaps;
	static Fl_Round_Button *nj;
	static Fl_Round_Button *bionj;
	static Fl_Round_Button *savetofile;
	static Fl_Round_Button *usertree;
	static Fl_Choice *treechoice;
	static Fl_Check_Button *bootstrap;
	static Fl_Button *interrupt;
	static Fl_Return_Button *go;
	static int started, maxchoice;
	static Fl_Check_Button *keep_b_trees;
	static Fl_Check_Button *compute_tbe;
	if(first) {
		first = FALSE;
		w = new Fl_Window(270, 265);
		w->label("Distance analysis");
		w->set_modal();
		nj = new Fl_Round_Button(3, 3, 60, 20, "NJ");
		nj->type(FL_RADIO_BUTTON);
		bionj = new Fl_Round_Button(nj->x() + nj->w() + 10, nj->y(), nj->w(), nj->h(), "BioNJ");
		bionj->type(FL_RADIO_BUTTON);
		savetofile = new Fl_Round_Button(bionj->x() + bionj->w() + 10, nj->y(), 110, nj->h(), "Save to File");
		savetofile->type(FL_RADIO_BUTTON);
		distance = new Fl_Choice(90, nj->y() + nj->h() + 15, 100, 20, "Distance");
		distance->align(FL_ALIGN_LEFT);
		ignore_all_gaps = new Fl_Check_Button(160, distance->y() + distance->h() + 5, 50, 20, 
						      "Ignore all gap sites");
		ignore_all_gaps->value(1);
		ignore_all_gaps->align(FL_ALIGN_LEFT);
		int y = ignore_all_gaps->y() + ignore_all_gaps->h() + 25;
		bootstrap = new Fl_Check_Button(3, y, 80, 20, "Bootstrap");
		replicates = new Fl_Int_Input(bootstrap->x() + bootstrap->w() + 5, y, 100, 20, "# of replicates");
		replicates->align(FL_ALIGN_TOP|FL_ALIGN_CENTER);
		char tmp[10];
		sprintf(tmp, "%d", def_replicates);
		replicates->value(tmp);

		compute_tbe = new Fl_Check_Button(3, bootstrap->y() + bootstrap->h() + 5, 280, 20, "Transfer Bootstrap Expectation method");
		compute_tbe->deactivate();

		keep_b_trees = new Fl_Check_Button(3, compute_tbe->y() + compute_tbe->h() + 5, 180, 20, "Show bootstrap trees");
		keep_b_trees->deactivate();

    static Fl_Check_Button *buttons[2];
		buttons[0]=compute_tbe;
		buttons[1]=keep_b_trees;
		bootstrap->callback(dist_b_cb, buttons);
		bootstrap->when(FL_WHEN_CHANGED);
		
		box = new Fl_Box(3, keep_b_trees->y() + keep_b_trees->h() + 5, 150, 20, "");
		usertree = new Fl_Round_Button(nj->x(), box->y() + box->h() + 15, 85, nj->h(), "User tree:");
		usertree->type(FL_RADIO_BUTTON);
		treechoice = new Fl_Choice(usertree->x() + usertree->w(), usertree->y(), 155, 20, "Optimize branch lengths");
		treechoice->align(FL_ALIGN_TOP|FL_ALIGN_RIGHT);
		bionj->setonly();
	
		interrupt = new Fl_Button(3, w->h() - 25, 70, 20, "");
		interrupt->callback(interrupt_callback, &tree_build_interrupted);
		go = new Fl_Return_Button(w->w() - 70 - 3, interrupt->y() , 70, 20, "Go");
		go->callback(interrupt_callback, &started);
		w->end();
		w->callback(interrupt_callback, &tree_build_interrupted);
		}
	interrupt->label("Cancel");
	distance->clear();
	if(view->protein) {
		distance->add("Observed");
		distance->add("Poisson");
		distance->add("Kimura");
		maxchoice = Kimura_pdist;
		ignore_all_gaps->activate();
	}
	else {
		distance->add("Observed");
		distance->add("J-C");
		distance->add("K2P");
		distance->add("HKY");
		distance->add("LogDet");
		distance->add("Ka");
		distance->add("Ks");
		maxchoice = Ks;
	}
	distance->value( FL_min(distance_choice, maxchoice) );
	distance->callback(distance_callback, ignore_all_gaps);
        keep_b_trees->value(0);
	treechoice->clear();
	vlength_menu *vmenu = view->menu_trees;
	int count = view->menu_trees->vlength();
	for (int i = 0; i < count; i++) 
	  treechoice->add( vmenu->vitem(i)->label() );
	treechoice->value(0);
	treechoice->deactivate(); 
        bootstrap->activate();
	if (usertree->value()) bionj->setonly();
	if (count) usertree->activate(); 
	else usertree->deactivate(); 
	started = tree_build_interrupted = 0;
	box->label("");
	go->show();
	w->show();
  while(!started && !tree_build_interrupted) {
    Fl_Widget *o = Fl::readqueue();
    if (!o) Fl::wait();
    else if(o == usertree || o == nj || o == bionj || o == savetofile) {
      if( usertree->value() || savetofile->value() ) {
        bootstrap->value(0);
        bootstrap->deactivate();
        bootstrap->do_callback();
      }
      else {
        bootstrap->activate();
      }
      if( usertree->value()  ) {
        treechoice->activate();
      }
      else {
        treechoice->deactivate();
      }
    }
  }
	distance_choice = distance->value();
	int using_kaks = ((!view->protein) && (distance_choice == Ka || distance_choice == Ks) );
	if(!tree_build_interrupted) {
	  sscanf(replicates->value(), "%d", &def_replicates);
	  interrupt->label("Interrupt");
	  if(bootstrap->value()) {
		  box->label("Count: 0");
		  }
	  go->hide();
	  Fl::flush();
	  char *tree = run_distance_method(view, distance_choice, ignore_all_gaps->value(), bionj->value(), 
						  bootstrap->value(), def_replicates, box, distance->text(), using_kaks,
			      savetofile->value()?"":NULL, usertree->value() ? view->trees[treechoice->value()] : NULL,
					   false, fl_alert, keep_b_trees->value(), compute_tbe->value());
	  const char *choice = bionj->value()? "BioNJ_tree" : "NJ_tree";
	  const char *n = extract_filename(view->masename);
	  char *title = NULL;
	  if (usertree->value()) { title = new char[20]; strcpy(title, "Least_squares_brl");}
	  else if (n) {
	    char *q = (char*)strchr(n, '.');
	    if (q) *q = 0;
	    title = new char[strlen(n) + 12];
	    sprintf(title, "%s-%s", n, choice);
	    if (q) *q = '.';
	  }
    Fl_Window *treew = NULL;
	  if (tree) treew = treedraw(tree, view, n || usertree->value() ? title:choice, FALSE);
#if !defined(__APPLE__)
    if (keep_b_trees->value() && treew) {
      treew->position(treew->x() + 40, treew->y());
    }
#endif
	  delete[] title;
	}
	w->hide();
}

struct dist_struct {
  allseq *seqs;
  int distkind;
  int protein;
  char **lwlseqs;
  int in_bootstrap;
  char *err_mess;
  matrix *result;
  bool done;
};

void prep_calc_dist_matrix(dist_struct *ds)
{
  ds->result = calc_dist_matrix(ds->seqs, ds->distkind, ds->protein, ds->lwlseqs, ds->in_bootstrap, &(ds->err_mess));
  if (tree_build_interrupted && ds->result) {
    Free_Mat(ds->result);
    ds->result = NULL;
    }
  Fl::lock();
  ds->done = true;
  Fl::awake();
  Fl::unlock();
}

matrix *run_calc_dist_matrix(allseq *seqs, int distkind, int protein, char **lwlseqs, int in_bootstrap, char **p_err_mess, int no_gui)
{
  dist_struct *ds = new dist_struct;
  ds->seqs = seqs;
  ds->distkind = distkind;
  ds->protein = protein;
  ds->lwlseqs =lwlseqs;
  ds->in_bootstrap = in_bootstrap;
  ds->err_mess = NULL;
  ds->result = NULL;
  ds->done = false;
  tree_build_interrupted = false;
  if (no_gui) {
    ds->result = calc_dist_matrix(ds->seqs, ds->distkind, ds->protein, ds->lwlseqs, ds->in_bootstrap, &(ds->err_mess));
  }
  else {
    fl_create_thread( (void (*)(void*))prep_calc_dist_matrix, ds, 0);
    while (!ds->done) Fl::wait();
  }
  matrix* m = ds->result;
  if (p_err_mess) *p_err_mess = ds->err_mess;
  delete ds;
  return m;
}


char *run_distance_method(SEA_VIEW *view, int distkind, int remove_all_gaps, int use_bionj, int use_bootstrap,
	int replicates, Fl_Box *box, const char *distance_name, int using_kaks, const char* distancefname,
			  const char *user_tree, int no_gui, void (*alert)(const char*, ...), int keep_b_trees, int compute_tbe )
{
	char *display_tree, *bootstrap_trees = NULL, *err_mess, tree_label[100]="";
	int total = 0, i, j, lbtrees;
	char **lwlseqs = NULL;
  	if(using_kaks) remove_all_gaps = FALSE;//keep gaps not to alter reading frame
	allseq *phyml_seqs = view_to_allseq(view, remove_all_gaps);
	if(use_bootstrap) {//for bootstrap, seq names must be all distinct
		for(i = 0; i < phyml_seqs->n_otu - 1; i++) {
			for(j = i+1; j < phyml_seqs->n_otu; j++) {
				if(strcmp(phyml_seqs->c_seq[i]->name, phyml_seqs->c_seq[j]->name) == 0) {
					alert("Can't run bootstrap because sequence name %s is used twice", 
							 phyml_seqs->c_seq[i]->name);
					free_after_view_to_allseq(phyml_seqs);
					return NULL;
					}
				}
			}
		}
	if(using_kaks) {
		lwlseqs = (char **)malloc(phyml_seqs->n_otu * sizeof(char *));
		for(i = 0; i < phyml_seqs->n_otu; i++) lwlseqs[i] = phyml_seqs->c_seq[i]->state;
		}
	matrix *phyml_mat = run_calc_dist_matrix(phyml_seqs, distkind, view->protein, lwlseqs, FALSE, &err_mess, no_gui);
	if(phyml_mat == NULL) {
	  free_after_view_to_allseq(phyml_seqs);
	  if( using_kaks ) {
	    free(lwlseqs);
	    if (err_mess) {
	      alert("%s", err_mess);
	      delete[] err_mess;
	      }
	    }
	  else if (!tree_build_interrupted) alert("Can't compute distances because sequences are too divergent.\nAre they aligned?");
	  return NULL;
	  }
	if(user_tree != NULL) {//process user tree
		extern char *least_squares_brl(const char *tree, int nbseq, double **dist, char **names);
		char *p, **usednames;
		int i, j;
		p = (char *)user_tree;
		if(*p== '[')p=strchr(p,']')+1;
		if(view->tot_sel_seqs > 0) {
			usednames = (char **)malloc(view->tot_sel_seqs * sizeof(char *));
			j = 0;
			for(i = 0; i < view->tot_seqs; i++) {
				if(view->sel_seqs[i]) usednames[j++] = view->seqname[i];
				}
			}
		else usednames = view->seqname;
		p = least_squares_brl(p, phyml_seqs->n_otu, phyml_mat->dist, usednames);
		if(view->tot_sel_seqs > 0) free(usednames);
		if(p != NULL) {
			display_tree = (char *)malloc(strlen(p) + strlen(distance_name) + 60);
			sprintf(display_tree, "[User-tree w/ least-squares br lengths, %d sites %s]%s", 
					phyml_seqs->clean_len, distance_name, p);
			free(p);
			if (box) box->window()->hide();//necessary under X11 so tree is in foreground
			}
		Free_Mat(phyml_mat);
		free_after_view_to_allseq(phyml_seqs);
		return display_tree;
		}
	if(distancefname) {
		savedistinfile(phyml_mat, extract_dirname(view->masename), no_gui ? distancefname : NULL);
		Free_Mat(phyml_mat);
		free_after_view_to_allseq(phyml_seqs);
		return NULL;
		}
	phyml_mat->method = use_bionj; //0:NJ 1:BioNJ
	phyml_mat->tree = Make_Tree_From_Scratch(phyml_seqs->n_otu, phyml_seqs);
	Bionj(phyml_mat);
	sprintf(tree_label, "%s %d sites %s", use_bionj ? "BioNJ" : "NJ",
		phyml_seqs->clean_len, distance_name);
	if (use_bootstrap) {
		char *full_tree = Write_Tree(phyml_mat->tree);
		Free_Tree(phyml_mat->tree);
		Free_Mat(phyml_mat);
		int error = init_add_bootstrap(full_tree);
		free(full_tree);
		if(!error) {
      char *p;
			for (int r = 0; r < replicates; r++) {
				if (tree_build_interrupted) break;
				bootstrap_weights(phyml_seqs, using_kaks );
				phyml_mat = run_calc_dist_matrix(phyml_seqs, distkind, view->protein, lwlseqs, TRUE, &err_mess, no_gui);
				if(phyml_mat == NULL) {
					if (!tree_build_interrupted) total = 0;
					break;
					}
				phyml_mat->method = use_bionj; // 0:NJ 1:BioNJ
				phyml_mat->tree = Make_Tree_From_Scratch(phyml_seqs->n_otu, phyml_seqs);
				Bionj(phyml_mat);
				char *one_replicate_tree = Write_Tree(phyml_mat->tree);
        if (keep_b_trees || compute_tbe) {
          if (r == 0) {
            lbtrees = replicates * (strlen(one_replicate_tree) + 1);
            bootstrap_trees = (char *)malloc(lbtrees + 1);
            p = bootstrap_trees;
            *p = 0;
          }
          int l = strlen(one_replicate_tree);
          if ((p-bootstrap_trees) + l + 1 > lbtrees) {
            lbtrees = (p-bootstrap_trees) + l + 1000;
            bootstrap_trees = (char*)realloc(bootstrap_trees, lbtrees + 1);
            p = bootstrap_trees + strlen(bootstrap_trees);
          }
          memcpy(p, one_replicate_tree, l); p += l;
          *p++ = '\n';
          *p = 0;
        }
				Free_Tree(phyml_mat->tree);
				Free_Mat(phyml_mat);
				total += process_one_replicate(one_replicate_tree, 1.);
				free(one_replicate_tree);
				char newbox[20];
				sprintf(newbox,"Count: %d", total);
				if (box) {
				  box->label(newbox);
				  box->redraw();
				  Fl::wait(0);
				}
			}
		}
		if(total > 0) {
			display_tree = finish_add_bootstrap(total);
			sprintf(tree_label + strlen(tree_label), " %d repl.%s", total, compute_tbe ? " (TBE)" : "" );
			}
		else display_tree = NULL;
	}
	else {
		display_tree = Write_Tree(phyml_mat->tree);
		Free_Tree(phyml_mat->tree);
		Free_Mat(phyml_mat);
	}
	free_after_view_to_allseq(phyml_seqs);
	if(using_kaks) {
		free(lwlseqs);
		}
	if(display_tree == NULL) {
	  if( !using_kaks) alert("Can't compute distances because sequences are too divergent.\nAre they aligned?");
	  return NULL;
	}
	if (bootstrap_trees && compute_tbe) {
	  display_tree=TBE_Compute(display_tree,bootstrap_trees);
	}
	//add label to tree
	char *tree = (char *)malloc(strlen(tree_label) + strlen(display_tree) + 4);
	sprintf(tree, "[%s] %s", tree_label, display_tree);
	free(display_tree);
	if (box) box->window()->hide();//necessary under X11 so tree is in foreground
  if (bootstrap_trees && keep_b_trees) {
    treedraw(bootstrap_trees, view, "bootstrap trees", FALSE);
  }
  return tree;
}

void savedistinfile(matrix *mat, const char *dirname, const char *outfname)
{
	FILE *out;
	int i, j;
	if (!outfname) {
	  Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
	  chooser->type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	  chooser->options(Fl_Native_File_Chooser::SAVEAS_CONFIRM | chooser->options());
	  chooser->title("Set distance output filename");  
	  chooser->directory(dirname);
	  chooser->preset_file("seqs.dist");
	  outfname = run_and_close_native_file_chooser(chooser);
	  if(outfname == NULL) return;
	}
	out = fopen(outfname, "w");
	if(out == NULL) return;
	fputs("#distances order: d(1,2),...,d(1,n) <new line> d(2,3),...,d(2,n) <new line>...\n", out);
	fprintf(out, "%d\n", mat->n_otu);
	for( i = 0; i < mat->n_otu - 1; i++) {
		for( j = i + 1; j < mat->n_otu; j++) {
			fprintf(out, "%g ", mat->dist[i][j]);
			}
		fputs("\n", out);
		}
	for( i = 0; i < mat->n_otu ; i++) {
		fprintf(out, "%s ", mat->name[i]);
		}
	fputs("\n\n", out);
  fputs("#pairwise distances\n", out);
  for (i = 0; i < mat->n_otu; i++) {
    for (j = i + 1; j < mat->n_otu; j++) {
      char *n1 = mat->name[i];
      char *n2 = mat->name[j];
      if (strcmp(n1, n2) > 0) { char *p = n1; n1 = n2; n2 = p; }
      fprintf(out, "%s,%s: %g\n", n1, n2, mat->dist[i][j]);
    }
  }
  fclose(out);
}

matrix *calc_dist_matrix(allseq *seqs, int distkind, int protein, char **lwlseqs, int in_bootstrap, char **p_err_mess)
{
	matrix *phyml_mat = NULL;
	if(distkind == observed_pdist || distkind == observed_ndist) {//observed
		model jcmodel;
		jcmodel.stepsize = 1;
		jcmodel.datatype = (protein ? 1 : 0);
		jcmodel.ns = (protein ? 20 : 4);
		phyml_mat = Obs_Dist(seqs, &jcmodel);
	}
	else if(distkind == Kimura_pdist ) {//Kimura protein distance
	  phyml_mat = Kimura_p_Dist(seqs);
	  if(phyml_mat == NULL) return NULL;
	}
	else if(distkind == Poisson_pdist || distkind == Jukes_Cantor) {//JC or Poisson
		model jcmodel;
		jcmodel.stepsize = 1;
		jcmodel.datatype = (protein ? 1 : 0);
		jcmodel.ns = (protein ? 20 : 4);
		phyml_mat = JC69_Dist(seqs, &jcmodel);
		if(phyml_mat == NULL) return NULL;
	}
	else if((distkind == Kimura2P) && ! protein) {//K2P
		phyml_mat = K80_dist_nogamma(seqs);
		if(phyml_mat == NULL) return NULL;
	}
	else if(distkind == HKY && ! protein) {//HKY
		phyml_mat = HKY_dist(seqs);
		if(phyml_mat == NULL) return NULL;
	}
	else if(distkind == LogDet && ! protein) {//LogDet
		phyml_mat = LOGDET_dist(seqs);
		if(phyml_mat == NULL) return NULL;
	}
	else if( (distkind == Ka || distkind == Ks) && ! protein) {//Ka or Ks
		int sat1, sat2, count = 0, err = 0, i, j;
		int l3 = 3*(seqs->clean_len / 3);//dismiss partial last codon
		*p_err_mess = NULL;
		static int first = TRUE;
		if(first) {
			first = FALSE;
			loadrl(); // needed once only
			prefastlwl();
			}
		phyml_mat = Make_Mat(seqs->n_otu);
		Init_Mat(phyml_mat, seqs);
// Ks distances can become saturated, re-run a bootstrap replicate in this case at most 50 times
		do {
			if(in_bootstrap && err) bootstrap_weights(seqs, TRUE );
			count++;
			if(distkind == Ka) //Ka
				err = fastlwl(lwlseqs, seqs->n_otu, l3, phyml_mat->dist, NULL, NULL, NULL, &sat1, &sat2, seqs->wght);
			else //Ks
				err = fastlwl(lwlseqs, seqs->n_otu, l3, NULL, phyml_mat->dist, NULL, NULL, &sat1, &sat2, seqs->wght);
			}
		while (in_bootstrap && err && count <=50);
		if (err) {
		    if (!in_bootstrap && !tree_build_interrupted) {
			char *err_mess = new char[200];
			snprintf(err_mess, 200, "Saturation between %s and %s", phyml_mat->name[sat1], phyml_mat->name[sat2]);
			*p_err_mess = err_mess;
		    }
		    Free_Mat(phyml_mat);
		    return NULL;
		    }
		for(i = 0; i < seqs->n_otu - 1; i++) {
			for(j = i + 1; j < seqs->n_otu; j++) {
				phyml_mat->dist[j][i] = phyml_mat->dist[i][j];
			}
		} 
		for(i = 0; i < seqs->n_otu; i++) phyml_mat->dist[i][i] = 0;
	}
	return phyml_mat;
}

/* a window where the return key always triggers a button
 */
class return_window : public Fl_Window {
  Fl_Return_Button *run_button;
public:
  return_window(int w, int h, const char *t) : Fl_Window(w, h, t) {}
  void run(Fl_Return_Button *b) { run_button = b; }
  int handle(int event) {
    if (event == FL_KEYBOARD && (Fl::event_key() == FL_Enter || Fl::event_key() == FL_KP_Enter)) {
      run_button->do_callback();
      return 1;
      }
    return Fl_Window::handle(event);
    }
};

Fl_Input *name;
Fl_Choice *mymodel;
Fl_Round_Button *b_alrt; Fl_Round_Button *b_no; Fl_Int_Input *b_count;
Fl_Round_Button *f_empir;
Fl_Round_Button *r_estim; Fl_Float_Input *r_val;
Fl_Round_Button *i_no; Fl_Round_Button *i_estim; Fl_Float_Input *i_val;
Fl_Round_Button *g_no; Fl_Choice *g_cats; Fl_Round_Button *a_est; Fl_Float_Input *a_val;
Fl_Round_Button *nni_b, *spr_b, *nni_spr_b;
Fl_Round_Button *u_bionj, *u_menutree, *u_random;
Fl_Choice *u_choice;
Fl_Check_Button *u_optimize, *u_quiet, *b_keep_trees, *b_tbe;
Fl_Int_Input *u_random_count;
char *phyml_path;

static void support_cb(Fl_Widget *wid, void *data)
{
  Fl_Button *b = (Fl_Button*)data;
  if (b->value()) {
    b_keep_trees->activate();
    b_tbe->activate();
  }
  else {
    b_keep_trees->deactivate();
    b_keep_trees->value(0);
    b_tbe->deactivate();
  }
}

void phyml_dialog(SEA_VIEW *view)
{
	int y = 3;
	static int first = TRUE;
	if(first) {
		char *p, *q;
		p = get_res_value("phyml",
#ifdef WIN32
					"PhyML-3.1_win32.exe"
#elif defined(__APPLE__)
#ifdef __ppc__
				  "phyml_3.0_universal"
#else
				  "PhyML-3.1_macOS-MountainLion"
#endif
#elif defined(sparc)
						  "phyml_3.0_sparc"
#else
#ifndef PHYMLNAME
#ifdef __i386__
#define PHYMLNAME "PhyML-3.1_linux32"
#else
#define PHYMLNAME "PhyML-3.1_linux64"
#endif
#endif
					PHYMLNAME
#endif
		);
#ifdef __APPLE__
	  if (fl_mac_os_version < 100800) p = (char*)"phyml_3.0_universal";
#elif defined(__linux__)
	  if ((q = get_full_path(p)) == NULL) p = (char*)"phyml";
#endif
		if ((q = get_full_path(p)) == NULL) {
			int rep = fl_choice("Seaview does not find PhyML under the name %s.\n"
					  "Is PhyML available on your computer ?", "No", "Available", NULL, p);
			if(rep == 0) return;
			Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
			chooser->type(Fl_Native_File_Chooser::BROWSE_FILE);   // let user browse a single file
			chooser->title("Find PhyML on your computer");                        
			q = run_and_close_native_file_chooser(chooser);
			if(q == NULL) return;
			set_res_value("phyml", q);
			save_resources();
			}
		first = FALSE;
		phyml_path = (char *)malloc(strlen(q) + 3);
		sprintf(phyml_path, "\"%s\"", q);
		}
	return_window *w = new return_window(270,130, "PhyML options");
	w->set_modal();
	//model
	mymodel = new Fl_Choice(50, y, 90, 20, "Model:");
	mymodel->align(FL_ALIGN_LEFT);
	if(view->protein) {
		mymodel->add("LG");
		mymodel->add("WAG");
		mymodel->add("Dayhoff");
		mymodel->add("JTT");
		mymodel->add("Blosum62");
		mymodel->add("MtREV");
		mymodel->add("RtREV");
		mymodel->add("CpREV");
		mymodel->add("DCMut");
		mymodel->add("VT");
		mymodel->add("MtMam");
		mymodel->add("MtArt");
		mymodel->add("HIVw");
		mymodel->add("HIVb");
	  mymodel->add("FLU");
		mymodel->value(0);
		}
	else {
		mymodel->add("JC69");
		mymodel->add("K80");
		mymodel->add("F81");
		mymodel->add("HKY85");
		mymodel->add("F84");
		mymodel->add("TN93");
		mymodel->add("GTR");
		mymodel->value(6);
		}
	y += mymodel->h() + 25;
	//bootstrap
	Fl_Group *b_group = new Fl_Group(0, y, w->w(), 95, "Branch Support");
	b_group->box(FL_ROUNDED_BOX);
	b_group->align(FL_ALIGN_TOP|FL_ALIGN_CENTER);
	y += 5;
	b_alrt = new Fl_Round_Button(3, y, 110, 20, "aLRT (SH-like)");
	b_alrt->type(FL_RADIO_BUTTON);
	b_no = new Fl_Round_Button(b_alrt->x() + b_alrt->w() + 10, y, 60, 20, "None");
	b_no->type(FL_RADIO_BUTTON);
	y += b_alrt->h() + 3;
	Fl_Round_Button *b_yes = new Fl_Round_Button(3, y, 115, 20, "Bootstrap with");
	b_yes->type(FL_RADIO_BUTTON);
	b_alrt->setonly();
	b_count = new Fl_Int_Input(b_yes->x() + b_yes->w() + 5, y, 50, 20, "replicates");
	b_count->align(FL_ALIGN_RIGHT);
	b_count->static_value("100");
  
  y += b_count->h();
  b_tbe = new Fl_Check_Button(b_yes->x(), y, 265, 20, "Transfer Bootstrap Expectation method");
  b_tbe->deactivate();
  y += b_tbe->h();
  b_keep_trees = new Fl_Check_Button(b_yes->x(), y, 180, 20, "Show bootstrap trees");
  b_keep_trees->deactivate();

  b_alrt->callback(support_cb, b_yes);
  b_no->callback(support_cb, b_yes);
  b_yes->callback(support_cb, b_yes);
	b_group->end();
	y += b_count->h() + 30;
  // freq + Ts/Tv group
  Fl_Group *fr_group = new Fl_Group(0, y, w->w(), 75, "");
  // frequencies group
  Fl_Group *f_group = new Fl_Group(0, y, w->w(), 30, view->protein?"Amino acid equilibrium frequencies":"Nucleotide equilibrium frequencies");
  f_group->box(FL_ROUNDED_BOX);
  f_group->align(FL_ALIGN_TOP|FL_ALIGN_CENTER);
  y += 5;
  f_empir = new Fl_Round_Button(3, y, 85, 20, "Empirical");
  f_empir->type(FL_RADIO_BUTTON);
  Fl_Round_Button *f_m = new Fl_Round_Button(f_empir->x() + f_empir->w() + 5, y, 60, 20, view->protein?"Model-given":"Optimized");
  f_m->type(FL_RADIO_BUTTON);
  (view->protein?f_m:f_empir)->setonly();
  f_group->end();
  y += f_group->h() + 15;
	//Ts/Tv ratio
	Fl_Group *r_group = new Fl_Group(0, y, w->w(), 30, "Ts/Tv ratio");
	r_group->box(FL_ROUNDED_BOX);
	r_group->align(FL_ALIGN_TOP|FL_ALIGN_CENTER);
	y += 5;
	r_estim = new Fl_Round_Button(3, y, 85, 20, "Optimized");
	r_estim->type(FL_RADIO_BUTTON);
	Fl_Round_Button *r_fix = new Fl_Round_Button(r_estim->x() + r_estim->w() + 5, y, 60, 20, "Fixed");
	r_fix->type(FL_RADIO_BUTTON);
	r_fix->setonly();
	r_val = new Fl_Float_Input(r_fix->x() + r_fix->w() + 10, y, 40, 20, "");
	r_val->value("4.0");
	r_group->end();
	r_group->deactivate();
	if (view->protein) r_group->hide();
	else y += r_group->h() + 15;
  fr_group->end();
  if (!view->protein) mymodel->callback(model_callback, fr_group);
	//invariable
	Fl_Group *i_group = new Fl_Group(0, y, w->w(), 30, "Invariable sites");
	i_group->box(FL_ROUNDED_BOX);
	i_group->align(FL_ALIGN_TOP|FL_ALIGN_CENTER);
	y += 5;
	i_no = new Fl_Round_Button(3, y, 52, 20, "None");
	i_no->type(FL_RADIO_BUTTON);
	i_estim = new Fl_Round_Button(i_no->x() + i_no->w() + 5, y, 85, 20, "Optimized");
	i_estim->type(FL_RADIO_BUTTON);
	Fl_Round_Button *i_fix = new Fl_Round_Button(i_estim->x() + i_estim->w() + 2, y, 60, 20, "Fixed");
	i_fix->type(FL_RADIO_BUTTON);
	i_no->setonly();
	i_val = new Fl_Float_Input(i_fix->x() + i_fix->w() + 10, y, 40, 20, "");
	i_val->value("0.10");
	i_group->end();
	y += i_group->h() + 15;
	//gamma categories
	Fl_Group *g_group = new Fl_Group(0, y, w->w(), 55, "Across site rate variation");
	g_group->box(FL_ROUNDED_BOX);
	g_group->align(FL_ALIGN_TOP|FL_ALIGN_CENTER);
	y += 5;
	g_no = new Fl_Round_Button(3, y, 75, 20, "None");
	g_no->type(FL_RADIO_BUTTON);
	g_cats = new Fl_Choice(g_no->x() + g_no->w() + 130, y, 50, 20, "# of rate categories");
	mymodel->align(FL_ALIGN_LEFT);
	g_cats->add("4");
	g_cats->add("6");
	g_cats->add("8");
	g_cats->add("10");
	g_cats->add("12");
	g_cats->value(0);
	y = g_cats->y() + g_cats->h() + 5;
	a_est = new Fl_Round_Button(3, y, 85, 20, "Optimized");
	a_est->type(FL_RADIO_BUTTON);
	a_est->setonly();
	Fl_Round_Button *a_fixed = new Fl_Round_Button(a_est->x() + a_est->w() + 5, y, 60, 20, "Fixed");
	a_fixed->type(FL_RADIO_BUTTON);
	a_val = new Fl_Float_Input(a_fixed->x() + a_fixed->w() + 10, y, 40, 20, "");
	a_val->value("2.0");	
	g_group->end();
	y += a_val->h() + 25;	
	//tree searching
	Fl_Group *s_group = new Fl_Group(0, y, w->w(), 35, "Tree searching operations");
	s_group->box(FL_ROUNDED_BOX);
	s_group->align(FL_ALIGN_TOP|FL_ALIGN_CENTER);
	y += 5;
	nni_b = new Fl_Round_Button(3, y, 45, 20, "NNI");
	nni_b->type(FL_RADIO_BUTTON);
	spr_b = new Fl_Round_Button(nni_b->x() + nni_b->w() + 5, y, 50, 20, "SPR");
	spr_b->type(FL_RADIO_BUTTON);
	nni_spr_b = new Fl_Round_Button(spr_b->x() + spr_b->w() + 5, y, 150, 20, "Best of NNI && SPR");
	nni_spr_b->type(FL_RADIO_BUTTON);
	nni_b->setonly();
	nni_b->callback(change_searching_callback, NULL);
	spr_b->callback(change_searching_callback, NULL);
	nni_spr_b->callback(change_searching_callback, NULL);
	s_group->end();
	y = s_group->y() + s_group->h() + 5;
	//starting tree
	y += 15;
	Fl_Group *u_group = new Fl_Group(0, y, w->w(), 80, "Starting tree");
	u_group->box(FL_ROUNDED_BOX);
	u_group->align(FL_ALIGN_TOP|FL_ALIGN_CENTER);
	y += 5;
	u_bionj = new Fl_Round_Button(3, y, 45, 20, "BioNJ");
	u_bionj->type(FL_RADIO_BUTTON);
	u_bionj->setonly();
	u_optimize = new Fl_Check_Button(u_bionj->x() + u_bionj->w() + 30, y, 170, 20, "Optimize tree topology");
	u_optimize->value(1);
	y += u_bionj->h() + 5;
	u_menutree = new Fl_Round_Button(3, y, 95, 20, "User given:");
	u_menutree->type(FL_RADIO_BUTTON);
	u_choice = new Fl_Choice(u_menutree->x() + u_menutree->w() + 5, y, 100, 20, "");
	vlength_menu *vmenu = view->menu_trees;
	int count = view->menu_trees->vlength();
	for(int i = 0; i < count; i++) u_choice->add( vmenu->vitem(i)->label() );
	u_choice->value(0);
	if(count <= 0) {u_menutree->deactivate(); u_choice->deactivate(); }
	u_random = new Fl_Round_Button(3, u_menutree->y() + u_menutree->h() + 5, 80, 20, "Add: ");
	u_random->deactivate();
	u_random->type(FL_RADIO_BUTTON);
	u_random_count = new Fl_Int_Input(u_random->x() + u_random->w(), u_random->y(), 35, 20, "random starts");
	u_random_count->align(FL_ALIGN_RIGHT);
	u_random_count->value("5");
	u_group->end();
	y = u_group->y() + u_group->h() + 5;
	// Run
	Fl_Button *cancel = new Fl_Button(3, y, 50, 20, "Cancel");
	cancel->callback(cancel_callback, NULL);
	Fl_Return_Button *ok = new Fl_Return_Button(w->w() - 60, y, 55, 20, "Run");
	w->run(ok);
	ok->callback(run_phyml_callback, view);
  u_quiet = new Fl_Check_Button(cancel->x() + cancel->w() + 40, y, 65, 20, "Quiet");
  u_quiet->value(0);
#ifndef WIN32
  //u_quiet->hide();
#endif
	w->end();
	w->size(w->w(), y + cancel->h() + 5);
	w->callback(cancel_callback, NULL);
	w->show();
}

static void change_searching_callback(Fl_Widget *ob, void *unused)
{
	if(ob == nni_b) u_random->deactivate();
	else u_random->activate();
}

static void cancel_callback(Fl_Widget *ob, void *data)
{
	Fl_Window *w = ob->window();
	if(w == NULL) w = (Fl_Window *)ob;
	w->hide();
	Fl::delete_widget(w);
}

static void model_callback(Fl_Widget *ob, void *data)
{
  Fl_Choice *c = (Fl_Choice *)ob;
  Fl_Group *fr_group = (Fl_Group*)data;
  Fl_Group *f_group = (Fl_Group *)fr_group->child(0);
  Fl_Group *r_group = (Fl_Group *)fr_group->child(1);
  int v = c->value();
  if (v >=0 && v <= 2) f_group->deactivate(); else f_group->activate();
  if (v == 1 || v == 3 || v == 4 || v == 5) r_group->activate();
  else r_group->deactivate();
}


void run_phyml_callback(Fl_Widget *ob, void *data)
{
  FILE *in;
  SEA_VIEW *view = (SEA_VIEW *)data;
  char input[PATH_MAX], base_fname[PATH_MAX], invar[10], alpha[10], *p;
  int replicates, cats, status, seqlen, user_tree = FALSE, i;
  float logL;
  char **tmpseqs, **tmpnames, **truenames;
  int tot_seqs = view->tot_seqs;
  
  p = create_tmp_filename();
  if(p == NULL) return;
  strcpy(base_fname, p);
  strcpy(input, base_fname);
  strcat(input, ".phy");
  region r, *pr = &r;
  list_segments ls;
  if(view->tot_sel_seqs > 0 && view->active_region == NULL) {
	  ls.debut = 1; ls.fin = view->seq_length; ls.next = NULL;
	  r.list = &ls;
	  }
  else {
	  pr = view->active_region;
	  }
  if(pr != NULL) {
	  list_segments *pls;
	  pls = pr->list;
	  seqlen = 0;
	  while(pls != NULL) {
		  seqlen += pls->fin - pls->debut + 1;
		  pls = pls->next;
	  }
  }
  else seqlen = view->seq_length;
  //remove * from saved protein data
  if(view->protein) {
	  tmpseqs = (char **)calloc(view->tot_seqs, sizeof(char *));
	  for( i = 0; i < view->tot_seqs; i++) {
		  if(view->tot_sel_seqs > 0 && !view->sel_seqs[i]) continue;
		  tmpseqs[i] = strdup(view->sequence[i]);
		  char *p = tmpseqs[i];
		  while((p = strchr(p, '*')) != NULL) *p = '-';
		  }
	  }
  else tmpseqs = view->sequence;
  //Use Seq##_ as sequence names
  tmpnames = (char **)calloc(view->tot_seqs, sizeof(char *));
  //memorize seq names because can be changed while phyml runs
  truenames = (char **)calloc(view->tot_seqs, sizeof(char *));
  int length = 0;
  for(i = 0; i < view->tot_seqs; i++) {
    if(view->tot_sel_seqs > 0 && !view->sel_seqs[i]) continue;
    tmpnames[i] = (char *)malloc(15);
    truenames[i] = strdup(view->seqname[i]);
    sprintf(tmpnames[i], "Seq%d_", i);
    length = FL_max(length, (int)strlen(tmpnames[i]));
    }
  status = save_phylip_file(input, tmpseqs,
				   tmpnames, view->tot_seqs, view->each_length, pr, 
				   view->sel_seqs, view->tot_sel_seqs, length);
  for(i = 0; i < view->tot_seqs; i++) if(tmpnames[i] != NULL) free(tmpnames[i]);
  free(tmpnames);
  if(view->protein) {
	  for( i = 0; i < view->tot_seqs; i++) {
		  if(tmpseqs[i] != NULL) free(tmpseqs[i]);
		  }
	  free(tmpseqs);
	  }
  if(status) goto way_out;
  if(b_alrt->value()) replicates = -4;
  else if (b_no->value()) replicates = 0;
  else sscanf(b_count->value(), "%d", &replicates);
  if (!u_optimize->value()) replicates = 0;
  if(i_no->value()) strcpy(invar, "0.0");
  else if(i_estim->value()) strcpy(invar, "e");
  else strcpy(invar, i_val->value());
  if(g_no->value()) { cats = 1; strcpy(alpha, "2.0"); }
  else {
	  sscanf(g_cats->mvalue()->label(), "%d", &cats);
	  if(a_est->value())  strcpy(alpha, "e");
	  else strcpy(alpha, a_val->value());
	  }
  char modelname[20], *args;
  strcpy(modelname, mymodel->mvalue()->label());
  //build command line
  args = (char *)malloc(1000);
  sprintf(args, "%s -d %s -m %s -b %d -v %s -c %d -a %s",
		  phyml_path, view->protein?"aa":"nt", modelname, replicates, invar, cats, alpha); 
  if( (!view->protein) && 
  (strcmp(modelname,"K80")==0 || strcmp(modelname,"HKY85")==0 || strcmp(modelname,"F84")==0 || strcmp(modelname,"TN93")==0) ) {
  if(r_estim->value()) {
	  strcat(args, " -t e");
	  }
  else {
	  sprintf(args + strlen(args), " -t %s", r_val->value());
	  }
  }
  if( (view->protein) || 
     (strcmp(modelname,"JC69")!=0 && strcmp(modelname,"K80")!=0 && strcmp(modelname,"F81")!=0) ) {
    if(f_empir->value()) {
      strcat(args, " -f e");
    }
    else {
      strcat(args, " -f m");
    }
  }
  if(spr_b->value()) strcat(args, " -s SPR");
  else if(nni_spr_b->value()) strcat(args, " -s BEST");
  if(!u_optimize->value()) { strcat(args, " -o lr"); user_tree = TRUE; }
  if (u_quiet->value()) strcat(args, " --quiet");
  else                  strcat(args, " --no_memory_check");
  if(u_menutree->value()) {
	   char *tree = view->trees[u_choice->value()];
	   if(*tree == '[' && (p = strchr(tree, ']')) != NULL) { //remove tree header
		   tree = p + 1;
		   while(*tree == ' ') tree++;
		   }
	   tree = replace_tree_names_by_rank(tree, view->seqname, view->tot_seqs);
	  if(tree == NULL) {
		  fl_alert("Names in starting tree and alignment don't match.\nStarting tree will be ignored.");
		  }
	  else {
		  char tmp[PATH_MAX];
		  sprintf(tmp, "%s.startingtree", base_fname);
		   FILE *out = fopen(tmp, "w");
		   fputs(tree, out);
		   free(tree);
		   fclose(out);
		   sprintf(args + strlen(args), " -u \"%s\"", tmp);
		  }
   }
  if(u_random->value()) {
		  sprintf(args + strlen(args), " --rand_start --n_rand_starts %d", atoi(u_random_count->value()));
		  }
  //finish with (long) input filename
  sprintf(args + strlen(args), " -i \"%s\"", input);
  ob->window()->hide(); //deletion of this window should be done totally at the end of this callback
  status = run_external_prog_in_pseudoterm(args, NULL, "tree-building");
  free(args);
  sprintf(input, "%s.phy_phyml_stats.txt", base_fname);
  in = fopen(input, "r");
  if (in == NULL) {
    sprintf(input, "%s.phy_phyml_stat.txt", base_fname);
    in = fopen(input, "r");
    }
  if (in == NULL) {
    sprintf(input, "%s.phy_phyml_stats", base_fname);
    in = fopen(input, "r");
    }
  logL = 0;
  if(in != NULL) {//search tree likelihood
	  while(TRUE) {
		  char line[100];
		  p = fgets(line, sizeof(line), in);
		  if(p == NULL) break;
		  if( (p=strstr(line, "Log-likelihood:")) != NULL) sscanf(p+15, "%f", &logL);
		  else if( (p=strstr(line, "Log-likelihood :")) != NULL) sscanf(p+16, "%f", &logL);
		  }
	  fclose(in);
	  }
  sprintf(input, "%s.phy_phyml_tree.txt", base_fname);
  in = fopen(input, "r");
  if (in == NULL) {
    sprintf(input, "%s.phy_phyml_tree", base_fname);
    in = fopen(input, "r");
    }
  if( status != 0 || in == NULL ) {
	  if(in != NULL) fclose(in);
	  if (is_view_valid(view)) fl_message("%s: tree building cancelled", extract_filename(view->masename));
	  }
  else {
	  char *display_tree;
	  fseek(in, 0, SEEK_END);
	  long l = ftell(in);
	  display_tree = (char *)malloc(l + 1);
	  int c; p = display_tree;
	  fseek(in, 0, SEEK_SET);
	  while( (c = getc(in)) != EOF) {
		  if(c != '\n' && c != '\r') *(p++) = c;
		  }
	  *p = 0;
	  fclose(in);
	  display_tree = put_names_back_in_tree(display_tree, truenames);
    //process bootstrap trees
    char *bootstrap_trees = NULL;
    if (replicates > 0) {
      sprintf(input, "%s.phy_phyml_boot_stats.txt", base_fname);
      in = fopen(input, "r");
      if (!in) {
        sprintf(input, "%s.phy_phyml_boot_stats", base_fname);
        in = fopen(input, "r");
      }
      if (in) { // compute the true # of bootstrap replicates because parallelized phyML can compute a few more
        char line[150];
        while ((p = fgets(line, sizeof(line), in))) {
          while (*p == ' ') p++;
          if (*p == '#') {
            sscanf(p+1, "%d", &replicates);
          }
        }
        fclose(in);
      }
    }
    if (replicates > 0 && (b_keep_trees->value() || b_tbe->value())) {
      sprintf(input, "%s.phy_phyml_boot_trees.txt", base_fname);
      in = fopen(input, "r");
      if (!in) {
      	sprintf(input, "%s.phy_phyml_boot_trees", base_fname);
      	in = fopen(input, "r");
      }
      if (in) {
        FILE *out = NULL;
        sprintf(input, "%s.phy_phyml_boot_trees_truename.txt", base_fname);
        out = fopen(input, "w");
        long l = 1000;
        while (out) { // for all bootstrap trees
          char *b_tree, *q, *tree2;
          int c;
          b_tree = (char*)malloc(l + 1);
          q = b_tree;
          do { // read one line = one tree
            c = fgetc(in);
            if (c == EOF) {
              free(b_tree);
              break;
            }
            if (q - b_tree >= l) {
              l += 1000;
              tree2 = (char *)realloc(b_tree, l + 1);
              q = tree2 + (q - b_tree);
              b_tree = tree2;
            }
            *q++ = c;
          } while (c != '\n');
          if (c == EOF) break;
          *q = 0;
          b_tree = put_names_back_in_tree(b_tree, truenames);
          fputs(b_tree, out);
          free(b_tree);
        }
        fclose(in);
        if (out) {
          fclose(out);
          in = fopen(input, "r");
          fseek(in, 0, SEEK_END);
          l = ftell(in);
          fseek(in, 0, SEEK_SET);
          bootstrap_trees = (char *)malloc(l+1);
          fread(bootstrap_trees, l, 1, in);
          bootstrap_trees[l] = 0;
          fclose(in);
        }
      }
    }
	  //clean tree internal labels
    char *display_tree_tbe = NULL;
    if(replicates > 0) {
      if (b_tbe->value()){
        char *tmp = strdup(display_tree);
        display_tree_tbe = TBE_Compute(tmp, bootstrap_trees);
      }
      display_tree = bootstrap_reformat(display_tree, replicates);
      if(bootstrap_trees && !b_keep_trees->value()) {
        free(bootstrap_trees);
        bootstrap_trees = NULL;
      }
    }
	  else if(replicates < 0) display_tree = alrt_reformat(display_tree);
	  
	  //assemble tree description
	  char tree_label[100];
	  sprintf(tree_label, "PhyML ln(L)=%.1f %d sites %s", logL, seqlen, modelname);
	  if(replicates > 0) {
      sprintf(tree_label + strlen(tree_label), " %d replic.", replicates);
	  }
	  if(cats > 1) sprintf(tree_label + strlen(tree_label), " %d rate classes", cats);
	  if(user_tree) strcat(tree_label, " User-tree");
	  //add label to tree
	  char *tree = (char *)malloc(strlen(tree_label) + strlen(display_tree) + 4);
	  sprintf(tree, "[%s] %s", tree_label, display_tree);
	  free(display_tree);
    
    if (replicates > 0 && b_tbe->value()) {
      //assemble TBE tree description
      char tree_label_tbe[100];
      sprintf(tree_label_tbe, "PhyML ln(L)=%.1f %d sites %s", logL, seqlen, modelname);
      sprintf(tree_label_tbe + strlen(tree_label_tbe), " %d replic. (TBE)", replicates);
      if(cats > 1) sprintf(tree_label_tbe + strlen(tree_label_tbe), " %d rate classes", cats);
      if(user_tree) strcat(tree_label_tbe, " User-tree");
      //add label to tree
      char *tree = (char *)malloc(strlen(tree_label_tbe) + strlen(display_tree_tbe) + 4);
      sprintf(tree, "[%s] %s", tree_label_tbe, display_tree_tbe);
      free(display_tree_tbe);
      display_tree_tbe = tree;
    }
    
	  if (is_view_valid(view)) {
	    const char *n = extract_filename(view->masename);
	    char *title = NULL;
	    if (n) {
	      char *q = (char*)strchr(n, '.');
	      if (q) *q = 0;
	      title = new char[strlen(n) + 12];
	      sprintf(title, "%s-%s", n, "PhyML_tree");
	      if (q) *q = '.';
	    }
      Fl_Window *btreew=0, *treew=0;
      if (replicates > 0 && b_keep_trees->value()) {
        btreew = treedraw(bootstrap_trees, view, "bootstrap trees", FALSE);
      }
      if (replicates > 0 && b_tbe->value()) {
        const char *p = n ? title:"PhyML_tree";
        char *q = (char*)malloc(strlen(p) + 5);
        sprintf(q, "%s_TBE", p);
        treedraw(display_tree_tbe, view, q, FALSE);
      }
	    treew = treedraw(tree, view, n ? title:"PhyML_tree", FALSE);
#if !defined(__APPLE__)
      if (treew && btreew) treew->position(btreew->x() + 30, btreew->y() + 40);
#endif
	    delete[] title;
	  }
	}
  Fl::delete_widget(ob->window());
way_out:
  for(i = 0; i < tot_seqs; i++) {
    if(truenames[i]) free(truenames[i]);
    }
  free(truenames);
  delete_tmp_filename(base_fname);
}


allseq *view_to_allseq(SEA_VIEW *view, int remove_all_gaps)
{
	int i, j, l;
	list_segments *ls;
	char *p;
	allseq *phyml_seqs = (allseq *)calloc(1, sizeof(allseq));
	phyml_seqs->n_otu = view->tot_sel_seqs == 0 ? view->tot_seqs : view->tot_sel_seqs;
	phyml_seqs->c_seq = (struct __Seq **)calloc(phyml_seqs->n_otu, sizeof(struct __Seq*));
	l = 0;
	if(view->active_region == NULL) {
		for( i = 0; i < view->tot_seqs; i++) {
			if(view->tot_sel_seqs != 0 && !view->sel_seqs[i]) continue;
			if(view->each_length[i] > l) l = view->each_length[i];
		}
	}
	else {
		ls = view->active_region->list;
		while(ls != NULL) {
			l += ls->fin - ls->debut + 1;
			ls = ls->next;
			}
		}
	phyml_seqs->clean_len = l;
	phyml_seqs->wght = (int *)calloc(1, l * sizeof(int));
	for( i = 0; i < l; i++) {
		phyml_seqs->wght[i] = 1;
		}
	j = 0;
	for( i = 0; i < view->tot_seqs; i++) {
		if(view->tot_sel_seqs != 0 && !view->sel_seqs[i]) continue;
		phyml_seqs->c_seq[j] = (struct __Seq *)calloc(1, sizeof(struct __Seq));
		phyml_seqs->c_seq[j]->name = view->seqname[i];
		phyml_seqs->c_seq[j]->len = l;
		phyml_seqs->c_seq[j]->state = (char *)malloc(l + 1);
		if(view->active_region == NULL) {
			memcpy(phyml_seqs->c_seq[j]->state, view->sequence[i], view->each_length[i] );
			if( l > view->each_length[i] ) {
				memset(phyml_seqs->c_seq[j]->state + view->each_length[i], '-', l - view->each_length[i]);
			}
		}
		else {
			ls = view->active_region->list;
			p = phyml_seqs->c_seq[j]->state;
			while(ls != NULL) {
				if(ls->fin <= view->each_length[i]) {
					memcpy(p, view->sequence[i] + ls->debut - 1, ls->fin - ls->debut + 1 );
					}
				else {
					int lrem = view->each_length[i] - ls->debut + 1;
					if(lrem > 0) memcpy(p, view->sequence[i] + ls->debut - 1, lrem );
					if(lrem < 0) lrem = 0;
					memset(p + lrem, '-', ls->fin - ls->debut + 1 - lrem);
					}
				p += ls->fin - ls->debut + 1;
				ls = ls->next;
			}
		}
		phyml_seqs->c_seq[j]->state[l] = 0;
		majuscules(phyml_seqs->c_seq[j]->state);
		p = phyml_seqs->c_seq[j]->state;
		if(!view->protein) { while((p = strchr(p, 'U')) != NULL) *p = 'T'; }
		else { while((p = strchr(p, '*')) != NULL) *p = '-'; }//replace stops by gaps
		if (!view->protein) { // remove non-nucleotide characters
		  p = phyml_seqs->c_seq[j]->state;
		  while (*p) {
		    if (strchr("ABCDGHKMNRSTUVWXY-", *p) == NULL) *p = 'N';
		    p++;
		  }
		}
		j++;
	}
	//remove gap-only or gap-with sites
	for( j = 0; j < phyml_seqs->clean_len; j++) {
		if(remove_all_gaps) {//remove any gap-containing site
			for( i = 0; i < phyml_seqs->n_otu; i++) {
				if(phyml_seqs->c_seq[i]->state[j] == '-')  break;
			}
			if(i == phyml_seqs->n_otu) continue;
		}
		else {//remove gap-only sites
			for( i = 0; i < phyml_seqs->n_otu; i++) {
				if(phyml_seqs->c_seq[i]->state[j] != '-')  break;
			}
			if(i != phyml_seqs->n_otu) continue;
		}
		for( i = 0; i < phyml_seqs->n_otu; i++) {
			memmove(phyml_seqs->c_seq[i]->state + j, phyml_seqs->c_seq[i]->state + j + 1, 
					phyml_seqs->clean_len - j);
			}
		j--; phyml_seqs->clean_len--;
	}
	return phyml_seqs;
}


void gaps_as_unknown_states(allseq *phyml_seqs, int protein)
{
  char unknown = (protein ? 'X' : 'N');
  for( int i = 0; i < phyml_seqs->n_otu; i++) {
	for( int j = 0; j < phyml_seqs->clean_len; j++) {
	  if(phyml_seqs->c_seq[i]->state[j] == '-') phyml_seqs->c_seq[i]->state[j] = unknown;
	  }
	}
}

void free_after_view_to_allseq(allseq *phyml_seqs)
{
	int i;
	for( i = 0; i < phyml_seqs->n_otu; i++) {
		free(phyml_seqs->c_seq[i]->state);
		free(phyml_seqs->c_seq[i]);
	}
	free(phyml_seqs->c_seq);
	free(phyml_seqs->wght);
	free(phyml_seqs);
}

/*void initialize_random_series(void)
{
  static int first = TRUE;
  unsigned seed;
  if(first) {
    first = FALSE;
#ifdef WIN32
    SYSTEMTIME timing;
    GetSystemTime(&timing);
    seed = (unsigned)(timing.wMilliseconds + 1000*(timing.wSecond + 60*(timing.wMinute + 60*(timing.wHour + 24*timing.wDay))));
#else
    struct tms timing;
    seed = (unsigned)times(&timing);
#endif
    srand(seed);
  }
}*/


double next_random(void)
{
  static int first = TRUE;
  unsigned seed;
  if (first) {
    first = FALSE;
#ifdef WIN32
    SYSTEMTIME timing;
    GetSystemTime(&timing);
    seed = (unsigned)(timing.wMilliseconds + 1000*(timing.wSecond + 60*(timing.wMinute + 60*(timing.wHour + 24*timing.wDay))));
    srand(seed);
#else
    struct tms timing;
    seed = (unsigned)times(&timing);
    srandom(seed);
#endif
  }
  
#ifdef WIN32
  return rand() / (RAND_MAX +(double)1.);
#else
  return random() / (RAND_MAX +(double)1.);
#endif
}

void bootstrap_weights(allseq *seqs, int use_codons)
{
	phydbl buff;
	int j, position;
	//initialize_random_series();
	memset(seqs->wght, 0, seqs->clean_len * sizeof(int));
	if(!use_codons) {//bootstrap sites
		for(j = 0; j < seqs->clean_len; j++) {
			/*buff  = rand();
			buff /= (RAND_MAX+1.);*/
		  buff = next_random();
			buff *= seqs->clean_len;
			position = (int)floor(buff);
			seqs->wght[position] += 1;
			}
		}
	else {//bootstrap codons
		int ncodons = (seqs->clean_len/3);
		int l3 = 3*ncodons;
		for(j = 0; j < l3; j += 3) {
			/*buff  = rand();
			buff /= (RAND_MAX+1.);*/
		  buff = next_random();
			buff *= ncodons;
			position = (int)floor(buff);
			seqs->wght[3 * position] += 1;//only 0, 3, 6,... have weights
			}
		}
}


matrix *Obs_Dist(allseq *data, model *mod)
{
	int site,i,j,k;
	phydbl unc_len;
	matrix *mat;
	phydbl **len;
	
	
	len = (phydbl **)mCalloc(data->n_otu,sizeof(phydbl *));
	For(i,data->n_otu)
    len[i] = (phydbl *)mCalloc(data->n_otu,sizeof(phydbl));
	
	unc_len = .0;
	
	mat = Make_Mat(data->n_otu);
	Init_Mat(mat,data);
	
	Fors(site,data->c_seq[0]->len,mod->stepsize)
    {
      if (tree_build_interrupted) break;
		For(j,data->n_otu-1)
		{
			for(k=j+1;k<data->n_otu;k++)
			{
				if((!Is_Ambigu(data->c_seq[j]->state+site,mod->datatype,mod->stepsize)) &&
				   (!Is_Ambigu(data->c_seq[k]->state+site,mod->datatype,mod->stepsize)))
				{
					len[j][k]+=data->wght[site];
					len[k][j]=len[j][k];
					if(strncmp(data->c_seq[j]->state+site,
							   data->c_seq[k]->state+site,
							   mod->stepsize))
						mat->P[j][k]+=data->wght[site];
				}
			}
		}
    }
	
	
	For(i,data->n_otu-1)
    for(j=i+1;j<data->n_otu;j++)
	{
		if(len[i][j])
		{
			mat->P[i][j] /= len[i][j];
		}
		else
		{
			mat->P[i][j] = 1.;
		}
		
		mat->P[j][i] = mat->P[i][j];
		
		mat->dist[i][j] = mat->P[i][j];
		
		mat->dist[j][i] = mat->dist[i][j];
	}
	
	For(i,data->n_otu) free(len[i]);
	free(len);
	
	return mat;
}


matrix *Kimura_p_Dist(allseq *data)
{
	int i, j;
	model jcmodel;
	jcmodel.stepsize = 1;
	jcmodel.datatype = 1;
	jcmodel.ns = 20;
	matrix *mat = Obs_Dist(data, &jcmodel);
	for(i = 0; i < data->n_otu; i++) {
	  if (tree_build_interrupted) break;
		for(j = i + 1; j < data->n_otu; j++) {
		  phydbl x = 1 - mat->dist[i][j] - 0.2 * mat->dist[i][j] * mat->dist[i][j];
		  if(x <= 0) {
			Free_Mat(mat);
			return NULL;
			}
		  mat->dist[i][j] = -log(x);
		  mat->dist[j][i] = mat->dist[i][j];
		  }
		}
	return mat;
}


char *alrt_reformat(char *tree)
//set aLRT support values to 2 decimal digits
{
	char *p, *q, tmp[20];
	float alrt;
	int l;
	p = tree;
	while((p = strchr(p, ')')) != NULL) {
		q = p++;
		while(TRUE) {
			q++;
			if(*q == 0 ) break;
			if(strchr("0123456789.-+ ", *q) == NULL) break;
			}
		if(*q != ':' ) continue;
		sscanf(p, "%f", &alrt);
		sprintf(tmp, "%.2f", alrt);
		l = strlen(tmp);
		if(l < q - p) { memset(p, ' ', q - p); memcpy(p, tmp, l); }
		}
	return tree;
}

char *bootstrap_reformat(char *tree, int replicates)
//replace absolute bootstrap values by percentages
{
	char *p, *q, *newtree, *pnew, *next;
	int count;
	newtree = (char *)malloc(500000);
	p = tree; pnew = newtree; next = tree;
	while((p = strchr(p, ')')) != NULL) {
		q = p++;
		while(TRUE) {
			q++;
			if(*q == 0 ) break;
			if(strchr("0123456789 ", *q) == NULL) break;
		}
		if(*q != ':' ) continue;
		memcpy(pnew, next, p - next);
		pnew += p - next;
		next = p;
		sscanf(p, "%d", &count);
		sprintf(pnew, "%d", (int)(100*count/float(replicates) + .5));
		pnew += strlen(pnew);
		next = q;
	}
	memcpy(pnew, next, strlen(next));
	pnew += strlen(next);
	*pnew = 0;
	free(tree);
	return newtree;
}

matrix *K80_dist_nogamma(allseq *data)
{
	int i,j,k;
	phydbl unc_len;
	matrix *mat;
	phydbl **len;
	
	len = (phydbl **)mCalloc(data->n_otu,sizeof(phydbl *));
	For(i,data->n_otu)
    len[i] = (phydbl *)mCalloc(data->n_otu,sizeof(phydbl));
	
	unc_len = .0;
	
	mat = Make_Mat(data->n_otu);
	Init_Mat(mat,data);
	
	
	For(i,data->c_seq[0]->len)
    {
      if (tree_build_interrupted) break;
		For(j,data->n_otu-1)
		{
			for(k=j+1;k<data->n_otu;k++)
			{
				if(((data->c_seq[j]->state[i] == 'A' || data->c_seq[j]->state[i] == 'G') &&
					(data->c_seq[k]->state[i] == 'C' || data->c_seq[k]->state[i] == 'T'))||
				   ((data->c_seq[j]->state[i] == 'C' || data->c_seq[j]->state[i] == 'T') &&
					(data->c_seq[k]->state[i] == 'A' || data->c_seq[k]->state[i] == 'G')))
				{
					mat->Q[j][k]+=data->wght[i];
					len[j][k]+=data->wght[i];
					len[k][j]=len[j][k];
				}
				
				else
					if(((data->c_seq[j]->state[i] == 'A' && data->c_seq[k]->state[i] == 'G') ||
						(data->c_seq[j]->state[i] == 'G' && data->c_seq[k]->state[i] == 'A'))||
					   ((data->c_seq[j]->state[i] == 'C' && data->c_seq[k]->state[i] == 'T') ||
						(data->c_seq[j]->state[i] == 'T' && data->c_seq[k]->state[i] == 'C')))
					{
						mat->P[j][k]+=data->wght[i];
						len[j][k]+=data->wght[i];
						len[k][j]=len[j][k];
					}
					else
						if((data->c_seq[j]->state[i] == 'A' ||
							data->c_seq[j]->state[i] == 'C' ||
							data->c_seq[j]->state[i] == 'G' ||
							data->c_seq[j]->state[i] == 'T')&&
						   (data->c_seq[k]->state[i] == 'A' ||
							data->c_seq[k]->state[i] == 'C' ||
							data->c_seq[k]->state[i] == 'G' ||
							data->c_seq[k]->state[i] == 'T'))
						{
							len[j][k]+=data->wght[i];
							len[k][j]=len[j][k];
						}
			}
		}
    }
	
	
	For(i,data->n_otu-1)
    for(j=i+1;j<data->n_otu;j++)
	{
		if(len[i][j])
		{
			mat->P[i][j] /= len[i][j];
			mat->Q[i][j] /= len[i][j];
		}
		else
		{
			mat->P[i][j] = .5;
			mat->Q[i][j] = .5;
		}
		
		mat->P[j][i] = mat->P[i][j];
		mat->Q[j][i] = mat->Q[i][j];
		
		
		if((1-2*mat->P[i][j]-mat->Q[i][j] <= .0) || (1-2*mat->Q[i][j] <= .0))
		{
		  Free_Mat(mat);
		  mat = NULL;
		  goto out;
		}
		
/*		mat->dist[i][j] = (g_shape/2)*
		(pow(1-2*mat->P[i][j]-mat->Q[i][j],-1./g_shape) +
		 0.5*pow(1-2*mat->Q[i][j],-1./g_shape) - 1.5); */
		
		mat->dist[i][j] = -0.5 * log(1-2*mat->P[i][j]-mat->Q[i][j]) - 0.25 * log(1-2*mat->Q[i][j]);
		mat->dist[j][i] = mat->dist[i][j];
	}
	
out:For(i,data->n_otu) free(len[i]);
	free(len);
	return mat;
}

matrix *HKY_dist(allseq *data)
{
  int i,j;
  matrix *mat;
  double x;
		  
  mat = Make_Mat(data->n_otu);
  Init_Mat(mat,data);
  For(i,data->n_otu - 1)
  {
	  for(j = i + 1; j < data->n_otu; j++)
	  {
	    if (tree_build_interrupted) return mat;
		x = hky(data->c_seq[i]->state, data->c_seq[j]->state, data->clean_len, data->wght);
		if(x == -1) {
		  Free_Mat(mat);
		  return NULL;
		  }
		mat->dist[i][j] = x;
		mat->dist[j][i] = mat->dist[i][j];
	  }
  }
  return mat;
}
			

/* Distance for Hasegawa, Kishino and Yano model */
double hky(char* seq1, char* seq2, int lg, int *wght)
{
	double d, freq[16], a, c, g, t, r, y, P, P1, P2, Q, A1, A2, A3, gamma, cc, ee, 
		va1, va2, cova1a2, cova1a3, cova2a3, delta, epsilon, ksi, eta, nu, ff;
	double larg1, larg2, larg3;
		
	freq_obs(seq1, seq2, freq, lg, wght);
	
	P1=freq[2]+freq[8]; 
	P2=freq[7]+freq[13];
	P=P1+P2;
	Q=freq[1]+freq[3]+freq[4]+freq[6]+freq[9]+freq[11]+freq[12]+freq[14];
	
	if(P+Q == 0) return 0;
	
	a=freq[0]+(freq[1]+freq[2]+freq[3]+freq[4]+freq[8]+freq[12])/2; 
	c=freq[5]+(freq[1]+freq[4]+freq[6]+freq[7]+freq[9]+freq[13])/2; 
	g=freq[10]+(freq[2]+freq[6]+freq[8]+freq[9]+freq[11]+freq[14])/2;
	t=1.-a-c-g;
	r=a+g; 
	y=c+t;
	
	larg1=1-Q/(2*r*y);
	larg2=1-Q/(2*r)-(r*P1)/(2*a*g);
	larg3=1-Q/(2*y)-(y*P2)/(2*c*t);
	
	if(larg1<=0. || larg2<=0. || larg3<=0.)
		return -1.;
	
	A1=(y/r)*log(larg1)-log(larg2)/r;
	A2=(r/y)*log(larg1)-log(larg3)/y;
	A3=-log(larg1);
	
	cc=1-Q/(2*r*y);
	ee=1-(r*P1)/(2*a*g)-Q/(2*r);
	ff=1-(y*P2)/(2*c*t)-Q/(2*y);
	
	delta=1/(2*ee*r*r)-1/(2*cc*r*r);
	epsilon=1/(2*ee*a*g);
	ksi=1/(2*y*y*ff)-1/(2*y*y*cc);
	eta=1/(2*c*t*ff);
	nu=1/(2*r*y*cc);
	
	va1=((delta*delta*Q+epsilon*epsilon*P1)-(delta*Q+epsilon*P1)*(delta*Q+epsilon*P1))/lg;
	va2=((ksi  *ksi  *Q+eta    *eta    *P2)-(ksi  *Q+eta    *P2)*(ksi  *Q+eta    *P2))/lg;
	cova1a2=(delta*ksi*Q*(1-Q)-delta*eta*Q*P2-epsilon*eta*P1*P2)/lg;
	cova1a3=nu*Q*(delta*(1-Q)-epsilon*P1)/lg;
	cova2a3=nu*Q*(ksi  *(1-Q)-eta    *P2)/lg;
	
	gamma=(va2-cova1a2)/(va1+va2-2*cova1a2) + ((r*y)/(a*g+c*t)) * ((cova1a3-cova2a3)/(va1+va2-2*cova1a2));
	
	d=2*(a*g+c*t)*(gamma*A1+(1-gamma)*A2)+2*r*y*A3;
	
	return(d);
}


/* freq_obs */
/* Write at address freq observed frequencies of 16 di-nucleotides XY */
/* (X= A,C,G,T  ,  Y=A,C,G,T) X and Y being homologous nucleotides of sequences */
/* seq1 and seq2. Alphabetic order is used : freq[0]=AA frequency, freq[1]=AC, */
/* ..., freq[15]=TT. */
int freq_obs(char* seq1, char* seq2, double* freq, int lgseq, int *wght)
{
	int i, lgseqvrai, w;
	
	for(i=0;i<16;i++) freq[i]=0;
	lgseqvrai = lgseq;
	for(i=0;i<lgseq;i++){
		if( (w = wght[i]) == 0) continue;
		switch(seq1[i]){
			case 'A':
				switch(seq2[i]){
					case 'A' : freq[0] += w; break;
					case 'C' : freq[1] += w; break;
					case 'G' : freq[2] += w; break;
					case 'T' : freq[3] += w; break;
					default : lgseqvrai -= w; break;
				}
				break;
			case 'C':
				switch(seq2[i]){
					case 'A' : freq[4] += w; break;
					case 'C' : freq[5] += w; break;
					case 'G' : freq[6] += w; break;
					case 'T' : freq[7] += w; break;
					default : lgseqvrai -= w; break;
				}
				break;
			case 'G':
				switch(seq2[i]){
					case 'A' : freq[8] += w; break;
					case 'C' : freq[9] += w; break;
					case 'G' : freq[10] += w; break;
					case 'T' : freq[11] += w; break;
					default : lgseqvrai -= w; break;
				}
				break;
			case 'T':
				switch(seq2[i]){
					case 'A' : freq[12] += w; break;
					case 'C' : freq[13] += w; break;
					case 'G' : freq[14] += w; break;
					case 'T' : freq[15] += w; break;
					default : lgseqvrai -= w; break;
				}
				break;
			default :
				lgseqvrai -= w;
		}
	}
	if(lgseqvrai != 0){
		for(i=0;i<16;i++) freq[i] /= lgseqvrai;
		return 1;
	}
	else return 0;
}

int informative(char** sequ, int nb)
{
	
	int i, j, k, info=0, siteok=0;
	char firstpair;
	
	
	for(i=0;sequ[0][i];i++){
		firstpair=0;
		siteok=0;
		for(j=0;j<nb-1;j++){
			for(k=j+1;k<nb;k++){
				if(sequ[j][i]==sequ[k][i]){
					if(firstpair){
						if(sequ[j][i]!=firstpair) { info++; siteok=1; break; }
					}
					else
						firstpair=sequ[j][i];
				}
			}
			if (siteok) break;
		}
	}
	
	return info;
}


/*void jumble_fct(char** seq, char** name, int notu)
{
	int i, j, rando;
	double buff;
	char *tmp;
	//initialize_random_series();
	for(i = 0; i<notu - 1; i++){
		j = notu - i - 1;
		//buff  = rand();
		//buff /= (RAND_MAX+1.);
		buff = next_random();
		buff *= j + 1;
		rando = (int)floor(buff);
		if(j != rando) {
			tmp = seq[j],
			seq[j] = seq[rando];
			seq[rando] = tmp;
			tmp = name[j],
			name[j] = name[rando];
			name[rando] = tmp;
			}
		}
}*/

char *add_equally_best(char *tree, char *all_best, int count)
{
  char *p, *q, *r, *final_tree;
  final_tree = (char*)malloc(strlen(tree) + strlen(all_best)+ count * 30);
  strcpy(final_tree, tree);
  q = final_tree + strlen(final_tree);
  *q++ = '\n';
  p = all_best;
  if (count > 1) {
    int rank = 0;
    while (count-- > 0) {
      r = strchr(p, '\n');
      sprintf(q, "[equally best tree #%d]", ++rank);
      q += strlen(q);
      memcpy(q, p, r - p + 1);
      q += r-p+1;
      p = r+1;
    }
  }
  *q = 0;
  free(tree);
  free(all_best);
  return final_tree;
}

static char *run_parsimony_method(SEA_VIEW *view, int nogaps, const char *gapsunknown, int nreplicates, int njumbles,
			   int maxbest, int user_tree, Fl_Box *w_count, dnapars_S_option s_option,
			   const char **pprogname, float consensus_threshold, int no_gui, char **p_bootstrap_trees, void (*alert)(const char *,...) )
{
  char *tree = 0, *final_tree = 0, *all_best;
  int i, steps, info, count, replicates_done, count_best;
  const char *p;
  allseq *tmpallseqs = view_to_allseq(view, nogaps);
  if(*gapsunknown) gaps_as_unknown_states(tmpallseqs, view->protein);
  char **pars_seqs = (char **)malloc(tmpallseqs->n_otu * sizeof(char *));
  char **pars_names = (char **)malloc(tmpallseqs->n_otu * sizeof(char *));
  for( i = 0; i < tmpallseqs->n_otu; i++) {
    pars_seqs[i] = tmpallseqs->c_seq[i]->state;
    pars_names[i] = tmpallseqs->c_seq[i]->name;
  }
  info = informative(pars_seqs, tmpallseqs->n_otu);
  if(view->protein) *pprogname = "Protpars"; 
  else {
    *pprogname = "Dnapars";
    if (tmpallseqs->clean_len >= SHRT_MAX) {
      alert("Can't compute parsimony with DNA seqs longer than %d.", SHRT_MAX);
      goto way_out;
    }
  }
  if (nreplicates) {
    tree = parsimony_with_bootstrap(tmpallseqs, pars_seqs, pars_names, tmpallseqs->n_otu, njumbles, 
				    view->protein, &steps, maxbest, nreplicates, w_count, &replicates_done, s_option, no_gui, consensus_threshold, &count_best,
                                    p_bootstrap_trees);
    if (tree) {
      final_tree = (char *)malloc(strlen(tree) + 200);
      char best[35] = "";
      if (count_best > 1) sprintf(best, "consensus of %d best trees, ", count_best);
      sprintf(final_tree, "[%s, bootstrap with %d replic., %d steps, %s%d sites (%d informative)%s]%s",
	      *pprogname, replicates_done, steps, best, tmpallseqs->clean_len, info, gapsunknown, tree);
    }
  }
  else if(user_tree) {
    char *differs, *input_tree;
    p = view->trees[user_tree - 1];
    while(*p == ' ') p++;
    if(*p == '[') p = strchr(p, ']')+ 1;
    while(*p == ' ') p++;
    input_tree = strdup(p);
    i = compare_newick_with_names(input_tree, pars_names, tmpallseqs->n_otu, &differs);
    if (i != 0) {
      alert("Names in user-tree and target sequences differ:\n%s in %s only", 
	       differs, i == 1 ? "user-tree" : "target sequences");
      tree = NULL;
    }
    else {
      input_tree = (char *)realloc(input_tree, strlen(input_tree) + 4 * tmpallseqs->n_otu + 6 );
      if( make_binary_or_unrooted(input_tree) != NULL) return NULL;
      strcat(input_tree, ";");
      tree = parsimony(pars_seqs, pars_names, tmpallseqs->n_otu, 0, &steps, input_tree, maxbest, NULL, view->protein, s_option, no_gui, NULL);
      free(input_tree);
      if(tree == NULL) {
        alert("Error in user-tree");
      }
      else {
        final_tree = (char *)malloc(strlen(tree) + 150);
        sprintf(final_tree, "[%s, user-tree, %d steps, %d sites (%d informative)%s]%s",
                *pprogname, steps, tmpallseqs->clean_len, info, gapsunknown, tree);
      }
    }
  }
  else {
    tree = parsimony_with_jumble(pars_seqs, pars_names, tmpallseqs->n_otu, njumbles, view->protein, 
				 &steps, &count, maxbest, w_count, s_option, no_gui, consensus_threshold);
    if (count > 1) { // memorize all equally best trees in all_best string
      char *q = strchr(tree, '\n');
      all_best = strdup(q + 1);
      *q = 0;
    }
    if (tree && count > 1 && !view->protein) { /* with DNA, fit branch lengths to consensus of most parsimonious trees */
      char *input_tree = tree;
      int steps2; /* don't use the steps of this consensus tree because it can be larger than parsimonious steps */
      tree = parsimony(pars_seqs, pars_names, tmpallseqs->n_otu, 0, &steps2, input_tree, maxbest, NULL, false, s_option, no_gui, NULL);
      free(input_tree);
      }
    if (tree) {
      char aux[30];
      final_tree = (char *)malloc(strlen(tree) + 150);
      if (count > 1) sprintf(aux, " (%d%% consensus)", int(consensus_threshold*100 + 0.5));
      else *aux = 0;
      sprintf(final_tree, "[%s, %d best trees%s, %d steps, %d sites (%d informative)%s]%s",
	      *pprogname, count, aux, steps, tmpallseqs->clean_len, info, gapsunknown, tree);
      if (count > 1) final_tree = add_equally_best(final_tree, all_best, count);
    }
  }
way_out:
  if (tree) free(tree);
  free_after_view_to_allseq(tmpallseqs);
  free(pars_seqs);
  free(pars_names);
  return final_tree;
}

static void user_tree_cb(Fl_Widget *wid, void *data)
{
  Fl_Button *b = (Fl_Button*)data;
  b->value(0);
  b->do_callback();
}

void parsimony_dialog(SEA_VIEW *view)
{
	int njumbles, i, count, maxbest = 100, started = 0, use_bootstrap, 
		nreplicates, user_tree, nogaps; 
	char *final_tree, *bootstrap_trees = NULL;
	const char *p, *progname;
  dnapars_S_option s_option = more_thorough;
  
	tree_build_interrupted = 0;
	return_window *w = new return_window(255, view->protein ? 225: 300, "Parsimony analysis");
	w->set_modal();
	Fl_Check_Button *w_dojumble = new Fl_Check_Button(2, 5, 170, 20, "Randomize seq. order");
	w_dojumble->value(0);
	Fl_Int_Input *w_jumbles = new Fl_Int_Input(w_dojumble->x() + w_dojumble->w() + 5, w_dojumble->y(), 30, 20, "times");
	w_jumbles->align(FL_ALIGN_RIGHT);
	w_jumbles->value("5");
	Fl_Check_Button *w_nogaps = new Fl_Check_Button(2, w_dojumble->y() + w_dojumble->h() + 5, 160, 20, "Ignore all gap sites");
	w_nogaps->value(1);
	Fl_Check_Button *w_gapsunknown = new Fl_Check_Button(2, w_nogaps->y() + w_nogaps->h() + 5, 180, 20, 
														 "Gaps as unknown states");
	w_gapsunknown->value(0);
	w_gapsunknown->deactivate();
  Fl_Widget *witness = w_gapsunknown;
  Fl_Round_Button *thorough_b, *less_thorough_b;
  Fl_Int_Input *w_best;
  if (!view->protein) {
    Fl_Group *g = new Fl_Group(2, witness->y() + witness->h() + 5, w->w()-4, 70);
    g->box(FL_DOWN_FRAME);
    thorough_b = new Fl_Round_Button(7, witness->y() + witness->h() + 5, 180, 20, "More thorough tree search");
    thorough_b->type(FL_RADIO_BUTTON);
    less_thorough_b = new Fl_Round_Button(7, thorough_b->y() + thorough_b->h() + 5, 180, 20, "Less thorough tree search");
    less_thorough_b->type(FL_RADIO_BUTTON);
    Fl_Round_Button *rearrange_best_b = new Fl_Round_Button(7, less_thorough_b->y() + less_thorough_b->h() + 5, 180, 20, "Rearrange on best tree");
    rearrange_best_b->type(FL_RADIO_BUTTON);
    rearrange_best_b->activate();
    thorough_b->setonly();
    g->end();
    w_best = new Fl_Int_Input(195, rearrange_best_b->y() + rearrange_best_b->h() + 5, 50, 20, "Equally best trees retained");
    w_best->align(FL_ALIGN_LEFT);
    w_best->value("10000");
    witness = w_best;
  }
  Fl_Value_Slider* slider = new Fl_Value_Slider(2, witness->y() + witness->h() + 25, w->w()-4, 20, "% level for consensus tree building");
  slider->align(FL_ALIGN_TOP);
  slider->type(FL_HOR_NICE_SLIDER);
  slider->box(FL_FLAT_BOX);
  slider->color(FL_YELLOW);
  slider->selection_color(FL_RED);
  slider->bounds(50, 100);
  slider->value(100);
  slider->precision(0);
	Fl_Round_Button *w_dobootstrap = new Fl_Round_Button(2, slider->y() + slider->h() + 10, 115, 20, "Bootstrap with");
	w_dobootstrap->type(FL_RADIO_BUTTON);
	Fl_Int_Input *w_btrepl = new Fl_Int_Input(w_dobootstrap->x() + w_dobootstrap->w() + 5, w_dobootstrap->y() ,
											  40, 20, "replicates");
	w_btrepl->value("100");
	w_btrepl->align(FL_ALIGN_RIGHT);
  Fl_Check_Button *keep_b_trees = new Fl_Check_Button(2, w_dobootstrap->y() + w_dobootstrap->h() + 5, 180, 20, "Show bootstrap trees");
  keep_b_trees->deactivate();
  static Fl_Check_Button *buttons[2] = {keep_b_trees, NULL};
  w_dobootstrap->callback(dist_b_cb, buttons);
  w_dobootstrap->when(FL_WHEN_CHANGED);

	Fl_Box *w_count = new Fl_Box(0, keep_b_trees->y() + keep_b_trees->h() + 5, w->w(), 20, NULL);
	
	Fl_Round_Button *w_user = new Fl_Round_Button(2, w_count->y() + w_count->h() + 5, 85, 20, "User tree:");
	w_user->type(FL_RADIO_BUTTON);
  w_user->callback(user_tree_cb, w_dobootstrap);
	Fl_Choice *w_choice = new Fl_Choice(w_user->x() + w_user->w() + 5, w_user->y(), 130, 20, "");
	count = view->menu_trees->vlength();
	for( i = 0; i < count; i++) w_choice->add( view->menu_trees->vitem(i)->label() );
	w_choice->value(0);
	if (!count) {w_user->deactivate(); w_choice->deactivate(); }
	
	Fl_Button *w_interrupt = new Fl_Button(w_dobootstrap->x(), w_user->y() + w_user->h() + 5, 60, 20, "Cancel");
	Fl_Return_Button *w_ok = new Fl_Return_Button(w->w() - 65, w_interrupt->y(), 60, 20, "OK");
	w->run(w_ok);
	w_interrupt->callback(interrupt_callback, &tree_build_interrupted);
	w_ok->callback(interrupt_callback, &started);
	w->end();
	w->callback(interrupt_callback, &tree_build_interrupted);
	w->size(w->w(), w_ok->y() + w_ok->h() + 5);
	w->show();
	while(!started && !tree_build_interrupted) {
	  Fl::wait();
	  if(w_gapsunknown->value()) { w_nogaps->value(0); w_nogaps->deactivate(); }
	  else w_nogaps->activate();
	  if(w_nogaps->value()) { w_gapsunknown->value(0); w_gapsunknown->deactivate(); }
	  else w_gapsunknown->activate();
	  }
	if (tree_build_interrupted) {
		delete w;
		return;
	}
	w_ok->hide();
	w_interrupt->label("Interrupt");
	if(w_dojumble->value()) {
		p = w_jumbles->value();
		sscanf(p, "%d", &njumbles);
		if(njumbles < 1) njumbles = 1;
		}
	else njumbles = 0;
	use_bootstrap = w_dobootstrap->value();
	user_tree = w_user->value();
	nogaps = w_nogaps->value();
	char gapsunknown[25];
	if(w_gapsunknown->value())
	sprintf(gapsunknown, ", gaps treated as %c", (view->protein ? 'X' : 'N') );
	else gapsunknown[0] = 0;
	if(use_bootstrap) {
		p = w_btrepl->value();
		sscanf(p, "%d", &nreplicates);
		}
	else {
		if(user_tree) user_tree = w_choice->value() + 1;
	}
  if (!view->protein) {
    if (thorough_b->value()) s_option = more_thorough;
    else if (less_thorough_b->value()) s_option = less_thorough;
    else s_option = rearrange_best_tree;
    p = w_best->value();
    sscanf(p, "%d", &maxbest);
    }
	my_watch_cursor(w);

	final_tree = run_parsimony_method(view, nogaps, gapsunknown, 
				    use_bootstrap && nreplicates > 1 ? nreplicates : 0, njumbles, 
					  maxbest, user_tree, w_count, s_option,
                                    &progname, slider->value()/100, false, keep_b_trees->value() ? &bootstrap_trees : NULL, fl_alert);

	fl_reset_cursor(w);
	delete w;
	if (final_tree != NULL) {
	  const char *n = extract_filename(view->masename);
	  char *title = NULL;
	  if (n) {
	    char *q = (char*)strchr(n, '.');
	    if (q) *q = 0;
	    title = new char[strlen(n) + strlen(progname) + 2];
	    sprintf(title, "%s-%s", n, progname);
	    if (q) *q = '.';
	  }
    int tree_count = 0;
    p = final_tree;
    while (TRUE) {
      p = strchr(p+1, '\n');
      if (!p) break;
      tree_count++;
    }
    if (tree_count == 0) tree_count = 1;
    Fl_Window *btreew = NULL;
    if (bootstrap_trees) btreew = treedraw(bootstrap_trees, view, "bootstrap trees", FALSE);
	  Fl_Window *treew = treedraw(final_tree, view, n ? title:progname, FALSE, tree_count);
#if !defined(__APPLE__)
      if (btreew) treew->position(btreew->x() + 30, btreew->y() + 40);
#endif
	  delete[] title;
	}
}


char *parsimony_with_jumble(char **pars_seqs, char **pars_names, int n_otu, int njumbles, int protein, int *psteps,
	int *pcount, int maxbest, Fl_Widget *w_count, dnapars_S_option s_option, int no_gui, float consensus_threshold)
{
	int i, steps, count = 0;
	char *tree, *newtree, *p, *q;
	if (njumbles > 0) {
	  if (w_count) w_count->label("First jumble attempt");
	  }
	tree = parsimony(pars_seqs, pars_names, n_otu, njumbles, &steps, NULL, maxbest, NULL, protein, s_option, no_gui, w_count);
	if (!tree) return NULL;
	p = tree - 1;
	while((p = strchr(p+1, '\n')) != NULL) count++;//count equally parsimonious trees
	if(count > 1) {//compute threshold consensus of all equally parsimonious trees
		p = strchr(tree, '\n');
		newtree = (char *)malloc(p - tree + 1);
		memcpy(newtree, tree, p - tree); newtree[p - tree] = 0;
		p++; i = count; count = 0;
		init_consensus_calculation(n_otu);
		while(TRUE) {
			count += process_one_consensus_item(newtree, pars_names);
			free(newtree);
			i--;
			if(i == 0) break;
			q = strchr(p, '\n');
			newtree = (char *)malloc(q - p + 1);
			memcpy(newtree, p, q - p); newtree[q - p] = 0;
			p = q + 1;
		}
		int **rows;
		int br_count = after_consensus_items(consensus_threshold, count, &rows);
		char *consensus_tree = compute_consensus_tree(pars_names, rows, br_count);
		for (i = 0; i < br_count; i++) free(rows[i]);
		free(rows);
    char *newtree = (char*)malloc(strlen(consensus_tree) + strlen(tree) + 2);
    sprintf(newtree, "%s\n%s", consensus_tree, tree);
    free(consensus_tree);
    free(tree);
    tree = newtree;
	}
	*psteps = steps;
	*pcount = count;
	return tree;
}


static char *parsimony_with_bootstrap(allseq *tmpallseqs, char **pars_seqs, char **pars_names, int n_otu, int njumbles, int protein, int *psteps,
			int maxbest, int replicates, Fl_Box *w_count, int *preplicates_done, dnapars_S_option s_option, int no_gui,
			       float consensus_threshold, int *pcount_best, char **p_bootstrap_trees)
{
	int i, j, steps, newsteps, count = 0, replicates_done = 0, repl_count, lbtrees = 0;
	char *tree, *newtree, *p, *q, *all_best, *bootstrap_trees = NULL;
	
	if (w_count) w_count->label("Full sequence tree"); 
	tree = parsimony_with_jumble(pars_seqs, pars_names, n_otu, njumbles, protein, 
								 &steps, &count, maxbest, NULL, s_option, no_gui, consensus_threshold);
	if (!tree) return NULL;
  if (count > 1) { // memorize all equally best trees in all_best string
    p = strchr(tree, '\n');
    all_best = strdup(p + 1);
    *p = 0;
  }
	if (count > 1 && !protein) { /* with DNA, fit branch lengths to strict consensus of most parsimonious trees */
	  char *input_tree = tree;
	  /* don't use the steps of this consensus tree because it can be larger than parsimonious steps */
	  tree = parsimony(pars_seqs, pars_names, n_otu, njumbles, &newsteps, input_tree, maxbest, NULL, false, s_option, no_gui, NULL);
	  free(input_tree);
	}
	*psteps = steps;
	init_add_bootstrap(tree); // memorize topology and br lengths of strict consensus of most parsimonious trees
	for(j = 0; j < replicates; j++) {
		if (w_count) {
		  char tmp[30];
		  sprintf(tmp, "Bootstrap replicate: %d", j + 1); 
		  w_count->label(tmp);
		}
		if (tree_build_interrupted) break;
		bootstrap_weights(tmpallseqs, FALSE);
		tree = parsimony(pars_seqs, pars_names, n_otu, njumbles, &steps, NULL, maxbest, tmpallseqs->wght, protein, s_option, no_gui, NULL);
		if (!tree) break;
		p = tree - 1; repl_count = 0;
		while((p = strchr(p+1, '\n')) != NULL) repl_count++;//count equally parsimonious trees
		i = repl_count;
		while (TRUE) {
			p = strchr(tree, '\n');
			newtree = (char *)malloc(p - tree + 1);
			memcpy(newtree, tree, p - tree); newtree[p - tree] = 0;
			process_one_replicate(newtree, 1./repl_count);
      if (p_bootstrap_trees) { // memorize all bootstrap trees
        if (lbtrees == 0) {
          lbtrees = 10000;
          bootstrap_trees = (char *)malloc(lbtrees + 1);
          q = bootstrap_trees;
          *q = 0;
        }
        int l = strrchr(newtree, ')') - newtree + 1; // length of (....)
        if ((q-bootstrap_trees) + l + 8 > lbtrees) {
          lbtrees = (q-bootstrap_trees) + l + 8 + 10000;
          char *tmpc = (char*)realloc(bootstrap_trees, lbtrees + 1);
          q = tmpc + (q - bootstrap_trees);
          bootstrap_trees = tmpc;
        }
        sprintf(q, "[%.2f]", 1./repl_count); q += strlen(q); // indicate tree weight
        memcpy(q, newtree, l); q += l; // (...) part of tree
        memcpy(q, ";\n", 3); q += 2; // end of tree
      }
			free(newtree);
			i--;
			if(i == 0) break;
			newtree = strdup(p+1);
			free(tree);
			tree = newtree;
		}
		++replicates_done;
		free(tree);
		}
	*preplicates_done = replicates_done;
	tree = finish_add_bootstrap(replicates_done);
  if (count > 1) tree = add_equally_best(tree, all_best, count);
  *pcount_best = count;
  if (p_bootstrap_trees) *p_bootstrap_trees = bootstrap_trees;
	return tree;
}

struct pars_struct {
  char** seq;
  char** seqname;
  int notu;
  int njumbles;
  int *jumble_no;
  int *steps;
  char* toevaluate;
  int arg_maxtrees;
  int *bt_weights;
  dnapars_S_option s_option;
  char *result;
  bool protein;
  bool done;
};


void awake_from_C(void)
{
  Fl::lock();
  Fl::awake();
  Fl::unlock();
}


static void parsimony_thread(pars_struct* args)
{
  typedef char* (*pars_f)(char** seq, char** seqname, int notu, int njumbles, int *jumble_no, int *steps, char* toevaluate, int arg_maxtrees, int *bt_weights, dnapars_S_option s_option);
  pars_f pf = args->protein ? protpars : dnapars;
  args->result = (*pf)(args->seq, args->seqname, args->notu, args->njumbles, args->jumble_no, args->steps, args->toevaluate, args->arg_maxtrees, args->bt_weights, args->s_option);
  Fl::lock();
  args->done = true;
  Fl::awake();
  Fl::unlock();
}

char *parsimony(char** seq, char** seqname, int notu, int njumbles, int *steps, char* toevaluate, int arg_maxtrees, int *bt_weights, 
		bool protein, dnapars_S_option s_option, int no_gui, Fl_Widget *w_count)
{
  char *p;
  int jumble_no = 0;
  int pre_jumble_no = 0;
  tree_build_interrupted = 0;
  if (no_gui) {
    if (protein) p = protpars(seq, seqname, notu, njumbles, NULL, steps, toevaluate, arg_maxtrees, bt_weights, s_option);
    else  p = dnapars(seq, seqname, notu, njumbles, NULL, steps, toevaluate, arg_maxtrees, bt_weights, s_option);
  }
  else {
    pars_struct *args = (pars_struct*)malloc(sizeof(pars_struct));
    args->seq = seq;
    args->seqname = seqname;
    args->notu = notu;
    args->njumbles = njumbles;
    args->jumble_no = &jumble_no;
    args->steps = steps;
    args->toevaluate = toevaluate;
    args->arg_maxtrees = arg_maxtrees;
    args->bt_weights = bt_weights;
    args->s_option = s_option;
    args->result = NULL;
    args->protein = protein;
    args->done = false;
    if ( ! fl_create_thread((void (*) (void *))parsimony_thread, args, 1024) ) {
      while (!args->done) {
        Fl::wait();
        if (jumble_no > pre_jumble_no && jumble_no < njumbles && w_count) {
          pre_jumble_no = jumble_no;
          char tmp[40];
          sprintf(tmp, "Jumble attempt: %d steps: %d", jumble_no + 1, *args->steps);
          w_count->label(tmp);
        }
      }
    }
    p = args->result;
    free(args);
  }
  if (tree_build_interrupted && p) {
    free(p);
    p = NULL;
  }
  return p;
}


/* from Numerical Recipes in C */
/* Allocation function for ludcmp function */
double *vector(long nl, long nh)
{
	double *v;
	
	v=(double *)malloc((size_t) ((nh-nl+1+1)*sizeof(double)));
	if (!v) printf("allocation failure in vector()");
	return v-nl+1;
}


/* free_vector */
/* from Numerical Recipes in C */
/* Memory freeing function for ludcmp function */
void free_vector(double *v, long nl, long nh)
{
	free((char*) (v+nl-1));
}


/* ludcmp */
/* from Numerical Recipes in C */
/* Replace matrix a by a rowwise permutation of its LU decomposition. */

int ludcmp(double **a, int n, int *indx, double *d)
{
	int i,imax=0,j,k;
	double big,dum,sum,temp;
	double *vv;
	
	vv=vector(1,n);
	*d=1.0;
	for (i=1;i<=n;i++) {
		big=0.0;
		for (j=1;j<=n;j++)
			if ((temp=fabs(a[i][j])) > big) big=temp;
		if (big == 0.0) return 0;
		vv[i]=1.0/big;
	}
	for (j=1;j<=n;j++) {
		for (i=1;i<j;i++) {
			sum=a[i][j];
			for (k=1;k<i;k++) sum -= a[i][k]*a[k][j];
			a[i][j]=sum;
		}
		big=0.0;
		for (i=j;i<=n;i++) {
			sum=a[i][j];
			for (k=1;k<j;k++)
				sum -= a[i][k]*a[k][j];
			a[i][j]=sum;
			if ( (dum=vv[i]*fabs(sum)) >= big) {
				big=dum;
				imax=i;
			}
		}
		if (j != imax) {
			for (k=1;k<=n;k++) {
				dum=a[imax][k];
				a[imax][k]=a[j][k];
				a[j][k]=dum;
			}
			*d = -(*d);
			vv[imax]=vv[j];
		}
		indx[j]=imax;
		if (a[j][j] == 0.0) a[j][j]=1.0e-20;
		if (j != n) {
			dum=1.0/(a[j][j]);
			for (i=j+1;i<=n;i++) a[i][j] *= dum;
		}
	}
	free_vector(vv,1,n);
	return 1;
}


/* Returns the determinant of matrix mat(4, 4) */
double det4(double mat[4][4])
{
	
	int i, j, *indx;
	double *lu[5], d;
	
	for(i=1;i<5;i++){
		lu[i]=(double*)calloc(5, sizeof(double));
		for(j=1;j<5;j++) lu[i][j]=mat[i-1][j-1];
	}
	indx=(int*)calloc(5, sizeof(int));
	
	if(!ludcmp(lu, 4, indx, &d)) return 0.;
	
	for(i=1;i<5;i++) d *= lu[i][i];
	for(i=1;i<5;i++)
		free(lu[i]);
	return d;
}


/* logdet distance */
double logdet(char* seq1, char* seq2, int lg, int *wght)
{
	double matxy[4][4];
	double freq[16], d, a1, c1, g1, t1, a2, c2, g2, t2;
	
	if(!freq_obs(seq1, seq2, freq, lg, wght)) return -1.;
	
	matxy[0][0]=freq[0]; matxy[0][1]=freq[1]; matxy[0][2]=freq[2]; matxy[0][3]=freq[3];
	matxy[1][0]=freq[4]; matxy[1][1]=freq[5]; matxy[1][2]=freq[6]; matxy[1][3]=freq[7];
	matxy[2][0]=freq[8]; matxy[2][1]=freq[9]; matxy[2][2]=freq[10]; matxy[2][3]=freq[11];
	matxy[3][0]=freq[12]; matxy[3][1]=freq[13]; matxy[3][2]=freq[14]; matxy[3][3]=freq[15];
	
	a1=matxy[0][0]+matxy[0][1]+matxy[0][2]+matxy[0][3];
	c1=matxy[1][0]+matxy[1][1]+matxy[1][2]+matxy[1][3];
	g1=matxy[2][0]+matxy[2][1]+matxy[2][2]+matxy[2][3];
	t1=matxy[3][0]+matxy[3][1]+matxy[3][2]+matxy[3][3];
	a2=matxy[0][0]+matxy[1][0]+matxy[2][0]+matxy[3][0];
	c2=matxy[0][1]+matxy[1][1]+matxy[2][1]+matxy[3][1];
	g2=matxy[0][2]+matxy[1][2]+matxy[2][2]+matxy[3][2];
	t2=matxy[0][3]+matxy[1][3]+matxy[2][3]+matxy[3][3];
	
	
	d=det4(matxy);
	if(d<=0.) return -1.;
	return (-log(d)+log(a1*c1*g1*t1*a2*c2*g2*t2)/2)/4;
}


matrix *LOGDET_dist(allseq *data)
{
	int i,j;
	matrix *mat;
	
	mat = Make_Mat(data->n_otu);
	Init_Mat(mat,data);
	For(i,data->n_otu - 1)
    {
		for(j = i + 1; j < data->n_otu; j++)
		{
		  if (tree_build_interrupted) return mat;
		  double x = logdet(data->c_seq[i]->state, data->c_seq[j]->state, data->clean_len, data->wght);
		  if(x == -1.) {
			Free_Mat(mat);
			return NULL;
			}
		  mat->dist[i][j] = x;
		  mat->dist[j][i] = x;
		}
	}
	return mat;
}


char *put_names_back_in_tree(char *oldtree, char **names)
//replace Seq###_ in oldtree by names in newtree
{
	char *p, *q, *newtree;
	int rank, l = 0;
	
	p = oldtree;
	while((p = strstr(p, "Seq")) != NULL) {
		p += 3;
		sscanf(p, "%d", &rank);
		l += strlen(names[rank]);
		}
	newtree = (char *)malloc(strlen(oldtree) + l + 1);
	p = newtree;
	q = oldtree;
	while(*q != 0) {
		if(strncmp(q, "Seq", 3) != 0) *p++ = *q++;
		else {
			sscanf(q + 3, "%d", &rank);
			l = strlen(names[rank]);
			strcpy(p, names[rank]);
			p += l;
			do q++; while(*q != '_');
			q++;
			}
		}
	free(oldtree);
	*p = 0;
	newtree = (char *)realloc(newtree, strlen(newtree) + 1);
	return newtree;
}

char *replace_tree_names_by_rank(const char *tree, char **names, int notu)
/* replace names from tree listed in table names by Seq##_
 returns altered tree or NULL if some tree name not found in table names
 */
{
	char *p = (char *)tree - 1;
	char *newtree, *q, *qstart, c;
	int found = 0, i, l;
	char *start = NULL;
	static char name[200];
	newtree = (char *)malloc(strlen(tree) + 10 * notu + 1);
	q = newtree;
	while(*(++p) != 0) {
		*(q++) = *p;
		if(*p == '(' ) {
			start = p + 1;
			qstart = q;
			}
		else if(start == NULL && *p == ',') { start = p + 1; qstart = q; }
		else if(start != NULL && (*p == ')' || *p == ':' || *p == ',') ) {
			found++;
			while(*start == ' ') start++;
			memcpy(name, start, p - start); name[p - start] = 0;
			l = strlen(name) - 1; while( l >= 0 && name[l] == ' ')  name[l--] = 0;
			for(i = 0; i < notu; i++) if(strcmp(name, names[i]) == 0) break;
			if(i == notu) {
				free(newtree);
				return NULL;
			}
			else {
				c = *(q - 1);
				sprintf(qstart, "Seq%d_", i);
				q = qstart + strlen(qstart);
				*q++ = c;
				}
			if(*p != ',') start = NULL; else { start = p + 1; qstart = q; }
		}
	}
	*q = 0;
	return newtree;
}


void command_line_phylogeny(int argc, char **argv)
{
  char *tree, *p;
  int replicates = 0, jumbles = 0, i;
  bool err = false;
  if (!isarg(argc, argv, "-distance") && !isarg(argc, argv, "-parsimony")) {
    fputs("Option -build_tree requires either -distance or -parsimony\n", stderr);
    exit(1);
    }
  if (isarg(argc, argv, "-distance") && !(isarg(argc, argv, "-o") || isarg(argc, argv, "-distance_matrix"))) err = true;
  if (isarg(argc, argv, "-parsimony") && !isarg(argc, argv, "-o")) err = true;
  if (err) {
    fputs("Missing '-o fname' option\n", stderr);
    exit(1);
    }
  SEA_VIEW *view = cmdline_read_input_alignment(argc, argv);
  if ( (p = argname(argc, argv, "-sites")) ) {
    list_regions *elt = view->regions;
    while (elt) {
      if (strcmp(elt->element->name, p) == 0) break;
      elt = elt->next;
    }
    if (elt) view->active_region = elt->element;
  }
  
  if ( (p = argname(argc, argv, "-species")) ) {
    for (i = 0; i < view->numb_species_sets; i++) {
      if (strcmp(view->name_species_sets[i], p) == 0) break;
    }
    if (i < view->numb_species_sets) {
      view->sel_seqs = view->list_species_sets[i];
      view->tot_sel_seqs = 0;
      for (i = 0; i < view->tot_seqs; i++) if (view->sel_seqs[i]) view->tot_sel_seqs++;
    }
  }
  
  replicates = argval(argc, argv, "-replicates", 0);
  
  char *utree = NULL;
  if (isarg(argc, argv, "-usertree")) {
    char *utname;
    if ( (utname = argname(argc, argv, "-usertree"))) {
      FILE *in = fopen(utname, "r");
      if (!in) {
        fprintf(stderr, "Usertree file not found: %s\n",utname);
        exit(1);
      }
      fseek(in, 0, SEEK_END);
      long s = ftell(in);
      fseek(in, 0, SEEK_SET);
      utree = (char*)malloc(s+1);
      fread(utree, s, 1, in);
      utree[s] = 0;
      fclose(in);
    }
  }

  if (isarg(argc, argv, "-distance")) { // distance method
    int distkind = 0; // == observed_p/ndist
    char *distname;
    if ( (distname = argname(argc, argv, "-distance"))) {
      if (strcmp(distname, "JC") == 0) distkind = Jukes_Cantor;
      else if (strcmp(distname, "K2P") == 0) distkind = Kimura2P;
      else if (strcmp(distname, "HKY") == 0) distkind = HKY;
      else if (strcmp(distname, "logdet") == 0) distkind = LogDet;
      else if (strcmp(distname, "Ka") == 0) distkind = Ka;
      else if (strcmp(distname, "Ks") == 0) distkind = Ks;
      else if (strcmp(distname, "Poisson") == 0) distkind = Poisson_pdist;
      else if (strcmp(distname, "Kimura") == 0) distkind = Kimura_pdist;
      }
    if (distkind == 0) distname = strdup("Observed");
    
    view->tot_trees = 0;
    tree = run_distance_method(view, distkind, isarg(argc, argv, "-nogaps"), !isarg(argc, argv, "-NJ"), replicates > 1, replicates, NULL,
			       distname, distkind == Ka || distkind == Ks, argname(argc, argv, "-distance_matrix"), utree, true, Fl::fatal, false, false);
    if (isarg(argc, argv, "-distance_matrix")) exit(0);
  }
  
  if (isarg(argc, argv, "-parsimony")) { // parsimony method
    const char *progname;
    jumbles = argval(argc, argv, "-jumbles", 0);
    char *search = argname(argc, argv, "-search");
    dnapars_S_option s_option;
    if (search && strcmp(search, "less") == 0) s_option = less_thorough;
    else if (search && strcmp(search, "best") == 0) s_option = rearrange_best_tree;
    else s_option = more_thorough;
    int usertreerank = 0;
    if (utree) {
      view->tot_trees = 1;
      view->trees = (char**)malloc(sizeof(char*));
      view->trees[0] = utree;
      usertreerank = 1;
    }
    tree = run_parsimony_method(view, isarg(argc, argv, "-nogaps"), 
				isarg(argc, argv, "-gaps_as_unknown") ? ", gaps treated as ?" : "", 
				replicates, jumbles, 10000, usertreerank, NULL, s_option, &progname, 1, true, 0, Fl::fatal);
    if (tree && (p = strchr(tree, ';')) != NULL) *(p+1) = 0; // tree string can contain several trees, keep first only
  }
  if (tree) {
    char *outfname = argname(argc, argv, "-o");
    FILE *out = (strcmp(outfname, "-") != 0 ? fopen(outfname, "w") : stdout);
    fwrite(tree, strlen(tree), 1, out);
    fputs("\n", out); fflush(out);
    if (out != stdout) fclose(out);
  }
}

void save_bootstrap_replicates(const char *fname, int replicates, SEA_VIEW *view)
{
  allseq *seqs = view_to_allseq(view, FALSE);
  int *each_length = new int[seqs->n_otu];
  for (int i = 0; i < seqs->n_otu; i++) each_length[i] = seqs->clean_len;
  char **sequences = new char*[seqs->n_otu];
  for (int i = 0; i < seqs->n_otu; i++) {
    sequences[i] = new char[seqs->clean_len];
  }
  FILE *out = fl_fopen(fname, "w");
  char *tmp_fname = create_tmp_filename();
  for (int repl = 1; repl <= replicates; repl++) {
    bootstrap_weights(seqs, FALSE);
    for (int i = 0; i < seqs->n_otu; i++) {
      int l = 0;
      for (int j = 0; j < seqs->clean_len; j++) {
        for (int k = 0; k < seqs->wght[j]; k++) {
          sequences[i][l++] = seqs->c_seq[i]->state[j];
        }
      }
    }
    save_alignment_or_region(tmp_fname, sequences, NULL,
                             NULL, view->seqname, seqs->n_otu, each_length,
                             NULL, NULL, PHYLIP_FORMAT,
                             0, NULL,
                             NULL, NULL, 0, view->protein,
                             0, NULL,
                             NULL, view->phylipwidnames,
                             0, NULL,
                             NULL, view->spaces_in_fasta_names);
    // add to growing output file
    FILE *in = fl_fopen(tmp_fname, "r");
    char line[100];
    while (fgets(line, sizeof(line), in)) {
      fputs(line, out);
    }
    fclose(in);
    fl_unlink(tmp_fname);
  }
  fclose(out);
  for (int i = 0; i < seqs->n_otu; i++) delete[] sequences[i];
  delete[] sequences;
  delete[] each_length;
  free_after_view_to_allseq(seqs);
}
