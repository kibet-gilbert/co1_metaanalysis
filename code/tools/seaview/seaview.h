#ifndef SEAVIEW_H
#define SEAVIEW_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <FL/Fl.H>
#if 100*FL_MAJOR_VERSION + FL_MINOR_VERSION < 103
#error Seaview requires FLTK version 1.3.0 or higher
#endif
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>
#ifndef FALSE
#define FALSE 0
#define TRUE (!FALSE)
#endif

#if defined(__APPLE__) || defined(WIN32) 
#define MICRO
#endif

#ifdef NO_PDF
#define PDF_OR_PS "PostScript"
#define PDF_OR_PS_EXT "ps"
#else
#define PDF_OR_PS "PDF"
#define PDF_OR_PS_EXT "pdf"
#endif

#define fgets(a,b,c) my_fgets(a,b,c)
extern char *my_fgets(char *s, int n, FILE *f);

// FL_COMMAND is the modifier for menu shortcuts (Meta on Apple and Ctrl otherwise)
#ifndef FL_COMMAND
#if defined(__APPLE__) 
#define FL_COMMAND FL_META
#else
#define FL_COMMAND FL_CTRL
#endif
#endif

#ifdef WIN32
extern void my_bell(void); 
#define fopen(a,b) fl_fopen(a, b)
#define memccpy _memccpy
#elif defined __APPLE__
#else
#include "FL/x.H"
#endif
#define FL_max(a,b)      ( (a) > (b) ? (a):(b) )
#define FL_min(a,b)      ( (a) < (b) ? (a):(b) )
#ifdef __APPLE__
#define PREPARE_LABEL(p) mac_fname_to_roman(p)
extern void set_seaview_modified(struct _SEA_VIEW *view, int ismodified);
#else
#if defined(WIN32)
extern const char *win32_UTF8_to_CP1252(const char *in); 
#define PREPARE_LABEL(p) win32_UTF8_to_CP1252(p)
#else
#define PREPARE_LABEL(p) p
#endif
#define set_seaview_modified(view, ismodif) view->modif_but_not_saved = ismodif
#endif
typedef struct _user_data_plus {
	void *p;
	int value;
	} user_data_plus;
extern void fl_reset_cursor(Fl_Window *win);


#define MAXLENCOM 50000 /* long max des commentaires sous mase */
#define MAX_SPECIES_SETS 50 /* nbre max de species sets */
#define MSF_WID_NAME 15
#define CLU_BLOCK_LEN 5000 /* block pour allocation mem format Clustal */
#define MAX_GAP_SITES 1000
#define MOVEKEYS_DEFAULT "][><"

typedef struct _list_segments { /* one segment + pointer to its successor */
	int debut, fin;
	struct _list_segments *next;
	} list_segments;
	
typedef struct { /* one region = name + pointer to list of segments */
  char *name;
  int hide_when_viewasprots; /* TRUE iff this region shd not be enabled in view as protein mode */
  list_segments *list;
  } region;

typedef struct _list_regions { /* region list = pointer to region + 
				pointer to successor in list */
	region *element;
	struct _list_regions *next;
	} list_regions;

typedef struct {
	int pos;
	int l[2];
	} gap_site;
	
typedef enum {MASE_FORMAT, PHYLIP_FORMAT, CLUSTAL_FORMAT, MSF_FORMAT, 
	FASTA_FORMAT, NEXUS_FORMAT } 
	known_format;

typedef enum {OPEN_ANY, OPEN_MASE, OPEN_PHYLIP, OPEN_CLUSTAL, OPEN_MSF, OPEN_FASTA,
	OPEN_NEXUS, ACNUC_IMPORT, SAVE, SAVE_AS, SAVE_REGIONS, SAVE_PROT_ALIGN, SAVE_BOOTSTRAPS, PRINTOUT,
#if !defined(__APPLE__)
	PDFOPTIONS,
#endif
	CONCATENATE, NEW_WINDOW, CLOSE_WINDOW,QUIT
	} file_menu_choice;

typedef enum {NO_ALT_COLORS, USING_STANDARD_COLORS, USING_ALT_COLORS}
	color_choice;

typedef enum {COPY_SEQS, PASTE_SEQS, SELECT_ALL, RENAME_SEQ, EDIT_COMMENTS, EDIT_SEQ, DELETE_SEQ, CREATE_SEQ, 
	LOAD_SEQ, DUPLICATE_SEQ,
	COMPLEMENT_SEQ, REVERSE_SEQ, EXCHANGE_UT, DOT_PLOT,
	CONSENSUS_SEQ, DELETE_GAP_ONLY_SITES, SET_GCODE}
	edit_choice;

typedef enum { ALIGN_ALL, ALIGN_SELECTED_SEQS, ALIGN_SITES, PROFILE, UNALIGN, ALIGN_OPTIONS } align_choice;

typedef enum {SHOW_HIDE_FOOTERS, CREATE_FOOTER, DELETE_FOOTER }
	footers_menu_choice;
	
typedef enum {PDF_COLOR, PDF_BW, TEXT_ONLY, SVG} pdf_kind;

class vlength_menu // an FL_SUBMENU_POINTER-attached variable-length series of Fl_Menu_Item's
{
  int offset; // # of fixed items
  int vlen; // # of variable items
  Fl_Menu_Item *items; // array of fixed + variable menu items
  Fl_Menu_ *menu_; // menu of which this is a submenu
  int rank; // rank of menu item to which this is a submenu
  int val; // rank (from 1) of currently selected variable item
public:
  vlength_menu(Fl_Menu_ *m, const char *name, const Fl_Menu_Item *init, int clength);
  int vlength() {return vlen;} // lenth of variable part
  int value() { return val; } // 0 if unset, >=1 if one if set
  void value(int v); // 0 to clear, >=1 to set one
  Fl_Menu_Item *get_menu() {return items;} // fixed + variable parts
  void add(const char *txt, Fl_Callback *cb, void *data, int flags); // add at end of variable part
  Fl_Menu_Item* vitem(int r) { return items + offset + r; } // one of the variable items
  Fl_Menu_Item* bar_item() { return (Fl_Menu_Item*)menu_->menu() + rank; }
  void remove(int rank); // remove variable item of rank (from 0)
  Fl_Menu_Item* find_item(const char*txt);
  void vclear(); // remove all the variable part
  ~vlength_menu();
};

#define TREE_WINDOW "Tree"
#define SEAVIEW_WINDOW "Seaview"

typedef struct _SEA_VIEW {
  Fl_Widget *DNA_obj; /* free object for names + seqs */
  Fl_Scrollbar *horsli; /* horizontal scrollbar */
  Fl_Scrollbar *vertsli; /* vertical scrollbar */
  Fl_Widget *right_screen_move, *left_screen_move, *up_screen_move, 
		  *down_screen_move; /* 4 big jump arrows */
  Fl_Widget *multipl; /* box displaying the multiplier value */
  Fl_Menu_Bar *menubar;
  const Fl_Menu_Item *menu_file; /* menu items of file operations */
  const Fl_Menu_Item *menu_edit; /* menu items of edit operations */
  const Fl_Menu_Item *menu_align; /* menu of align operations */
  const Fl_Menu_Item *menu_props; /* menu of properties */
  vlength_menu *menu_sites; /* menu of site operations */
  vlength_menu *menu_species; /* menu of species */
  const Fl_Menu_Item *menu_footers; /* menu of footers */
  vlength_menu *menu_trees; /* menu of trees */
  Fl_Widget *bouton_reference; /* bouton_reference */
  char **sequence; /* array of sequences */
  char **comments; /* array of sequence comments */
  char *header; /* header of mase file */
  char **col_rank; /* array of color ranks of sequence sites */
  char **alt_col_rank; /* alternate array of color ranks of sequence sites for by reference */
  char **seqname; /* array of sequence names */
  char *masename; /* mase file name */
  int protein; /* TRUE iff protein data */
  known_format format_for_save; /* what file format used when saving */
  int wid_names; /* width of largest sequence name */
  int first_seq, first_site; /* first seq and site currently displayed */
  int tot_seqs; /* total number of sequences in memory */
  int seq_length; /* current length of longest sequence */
  int max_seq_length; /*max length allowed for longest sequence to grow */
  int *each_length; /* array of each individual sequence length */
  int tot_sites, tot_lines; /* sites and lines fitting in DNA_obj */
  int x_name, y_name, x_seq, y_seq; /* top left to start writing 
					  names and sequences */
  int draw_names; /*controle ecriture noms 0: pas ecrits; 
		  -1: tous ecrits puis aussi les seqs;
		  -2: tous ecrits mais pas les seqs;
		  >=1: 1 seul ecrit et pas de seq ni de curseur ecrit*/
  int mod_seq; /* -1: seqs selectionnees; 
		  0: normal; 
		  >= 1: 1 seul seq ecrite */
  int mod_cursor; /* FALSE: cursor ancien ignore; TRUE: ancien efface */
#if (100*FL_MAJOR_VERSION + FL_MINOR_VERSION >= 104) && !defined(__APPLE__)
  float scale; /* scaling factor used to compute char_width */
#endif
  float char_width; /* largeur en pixels des caracteres */
  int line_height; /* hauteur en pixels des lignes */
  int cursor_seq, cursor_site; /* position courante du curseur */
  int old_cursor_seq, old_cursor_site; /* position precedente du curseur*/
  int tot_sel_seqs; /* nbre courant de seqs selectionnees */
  int *sel_seqs; /* tableau des seqs selectionnees: T/F pour chacune */
  int *curr_colors; /*tableau des couleurs pour chaque valeur col_rank */
  int inverted_colors; /* TRUE iff use black letters on colored square */
  int namecolor; /* couleur utilisee pour les noms */
  int numb_gc; /* 1 en N&B, >1 en couleurs */
  int *dnacolors; /* tableau des couleurs pour DNA/RNA */
  int numb_dnacolors; /* nbre de couleurs pour DNA/RNA */
  int *stdprotcolors; /*tableau des couleurs standard pour prots*/
  int numb_stdprotcolors; /* nbre de couleurs standard pour prots */
  int *altprotcolors;/*tableau des couleurs alternatives pour prots*/
  int numb_altprotcolors; /*nbre de couleurs alternatives pour prots*/
  int *codoncolors; /*tableau des couleurs standard pour codons*/
  color_choice alt_colors; /* current aa color use */
  list_regions *regions; /* list of known regions */
  region *active_region; /* active region (NULL when none) */
  char *region_line; /* region line (of length region_length) */
  int region_color; /* color used to display region line */
  int region_length; /* length of region_line (==seq_length) */
  int mod_region_line; /* TRUE region line changed=>redraw only it */
  int numb_species_sets; /* number of known species sets */
  int *list_species_sets[MAX_SPECIES_SETS];/*list of known species sets*/
  char *name_species_sets[MAX_SPECIES_SETS];/*names of species sets*/
  int allow_seq_edit; /* when TRUE can type sequence data in */
  void *viewasprots; /* !=NULL when DNA displayed as proteins */
  int hjkl; /* when TRUE, keys hjklHJKL produce TCGA */
  int modif_but_not_saved; /* set/unset through function set_seaview_modified */
  Fl_Window *dnawin; /* full seaview window */
  int double_buffer; /* true iff DNA_obj is double buffered */
  int alignment_algorithm; /* rank (from 0) of chosen alignment algorithm */
  int count_msa_algos; /* number of usable alignment algorithms */
  char *clustal_options; /* options for clustalw if != NULL */
  int consensus_threshold; /* min threshold (%) for residue in consensus*/
  int consensus_allowgaps; /* TRUE iff gaps count as residue for consensus*/
  int consensus_allowiupac; /* TRUE iff IUPAC codes are used in consensus DNA sequences */
  int tot_comment_lines; /* number of comment lines */
  char **comment_line; /* array of comment lines */
  char **comment_name; /* array of comment names */
  int *comment_length; /* array of comment lengths */
  int show_comment_lines; /* TRUE when displayed */
  int pos_first_comment_line; /* rank on screen of this line */
  int mod_comment_line; /* 0: normal, >=1 celle-ci ecrite */
  int active_comment_line; /* 0: none, >=1 target of keyboard */
  int cursor_in_comment; /* TRUE when cursor is in comments not in seqs */
  int old_cursor_in_comment; /*previous place of cursor seq vs. comments*/
  char *movekeys; /* default ][><  keys to move seq right or left */
  int allow_lower; /* TRUE iff lowercase displayed in seqs */
  int phylipwidnames; /* max name length when writing PHYLIP files */
  int spaces_in_fasta_names; //TRUE iff spaces dont mark end of seq name n FASTA format
  int tot_trees; /* number of trees in next array */
  char **trees; /* array of trees */
} SEA_VIEW;
	

typedef struct _props_menu_parts {
	int colors, viewasprots, inverted, edit, reference, keys,
		consensusopt, slow_fast, allow_lower;
	int usingcodoncolors;
	} props_menu_parts;


extern void out_of_memory(void);
extern "C" void majuscules(char *s);
extern void init_dna_scroller(SEA_VIEW *view, int totseqs, 
	const char *masename, int protein, char *header);
extern const char *extract_filename(const char *fname);
extern const char *extract_dirname(const char *pathname);
extern void draw_region_line(Fl_Widget *ob, SEA_VIEW *view);
extern void props_button_callback(Fl_Widget *ob, long mainpop);
extern void regions_menu_callback(Fl_Widget *ob, void *extra);
extern void species_menu_callback(Fl_Widget *ob, void *extra);
extern void footers_menu_callback(Fl_Widget *ob, void *extra);
extern int suppr_segment(region *maregion, int site, char *line);
extern int begin_change_segment(SEA_VIEW *view, int new_site);
extern int continue_change_segment(SEA_VIEW *view, int new_site);
extern void end_change_segment(SEA_VIEW *view);
extern int extend_segment_at_left(SEA_VIEW *view, int new_site);
extern void delete_region_part(SEA_VIEW *view, int numsite, int total);
extern void insert_region_part(SEA_VIEW *view, int numsite, int total);
extern void delete_region(SEA_VIEW *view, int rang);
extern list_regions *parse_regions_from_header(char *header);
extern int parse_species_sets_from_header(char *header, int **list_species_sets,
	char **name_species_sets, int totseqs);
extern void set_tot_lines(SEA_VIEW *view, int new_val);
extern int read_mase_seqs_header(const char *, char ***pseq,char ***pseqname,
		char ***pcomments, char **pheader, char **err_message);
extern int read_fasta_align(const char *fname, char ***pseq, char ***pseqname, 
		char ***pcomments, char **pheader,  char **err_message, int spaces_in_names);
extern int read_phylip_align(const char *fname, char ***pseq, char ***pseqname, 
		char ***pcomments, char **pheader, char **);
extern int read_clustal_align(const char *fname, char ***pseq, char ***pseqname, 
		char ***pcomments, char **pheader, char **err_message);
extern int read_msf_align(const char *fname, char ***pseq, char ***pseqname, 
		char ***pcomments, char **pheader, char **err_message);
extern int read_nexus_align(char *fname, char ***pseqs, char ***pseqnames, 
	char ***pcomments, char **pheader, char **err_message, 
	list_regions **charsets, int *pnum_species_sets,
	int ***list_species_sets, char ***name_species_sets,
	int *ptot_comment_lines, char ***comment_name, char ***comment_line,
	int **comment_length, int *protein, SEA_VIEW *view);
extern char *save_alignment_or_region(const char *fname, char **seq, 
	char **comments,
	char *header, char **seqname, int totseqs, int *eachlength,
	list_regions *regions, region *region_used, known_format format,
	int numb_species_sets, int **list_species_sets, 
	char **name_species_sets, int *sel_seqs, int tot_sel_seqs, int protein,
	int tot_comment_lines, char **comment_name, char **comment_line, int phylipwidnames,
	int tot_trees, char **trees, const Fl_Menu_Item *menu_tree_items, int spaces_in_fasta_names);
extern Fl_Window *load_alignment_file(SEA_VIEW *view, char *filename, 
	const char *message, known_format file_format, int doing_dnd);
extern int is_a_protein_seq(char *seq);
extern int is_a_protein_alignment(SEA_VIEW *view);
extern char *get_full_path(const char *fname);
extern void draw_region_background(SEA_VIEW *view, int f_seq0, int l_seq0);
extern void load_seq_dialog(SEA_VIEW *view);
extern void edit_comments_dialog(SEA_VIEW *view);
extern void edit_sequence_dialog(SEA_VIEW *view);
extern int add_seq_to_align(SEA_VIEW *view, char *newname, char *newseq, int lenseq, bool no_gui = false);
extern int get_color_for_base( int key );
extern int get_color_for_aa( int key );
extern char **prepcolranks(char **seq, int totseqs, int maxlen, int *eachlength,
	int (*calc_color_function)( int ), int numb_gc, int allow_lower);
extern char complement_base(char old);
extern void select_deselect_seq(SEA_VIEW *view, int new_seq, int from_tree = 0);
extern void save_active_region(SEA_VIEW *view);
extern void align_selected_parts(SEA_VIEW *view, align_choice my_align_choice, bool no_gui = false);
extern void free_region(region *reg);
extern void my_watch_cursor(Fl_Window *win);
extern void update_current_seq_length(int newlength, SEA_VIEW *view);
extern char *cre_consensus(SEA_VIEW *view, char *newname);
extern int parse_comment_lines_from_header(char *header, char ***plines, 
	char ***pnames, int **plengths, int *pmax_seq_length);
extern int insert_char_in_comment(int key, int num, SEA_VIEW *view);
extern int delete_char_in_comment(SEA_VIEW *view, int count, int comnum, 
	int depart, int protect);
extern void update_menu_footers(SEA_VIEW *view);
extern int insert_gap_all_comments(int numgaps, int pos,  SEA_VIEW *view);
extern int delete_in_all_comments(int numdels, int pos,  SEA_VIEW *view);
extern char* fl_file_chooser(const char* message, const char* pat, const char* fname, int);
extern char *run_and_close_native_file_chooser(Fl_Native_File_Chooser *chooser, int keepalive=0);
extern void trees_callback(Fl_Widget *ob, void *data);
extern int save_fasta_file(const char *fname, char **seq, char **comments,
		    char **seqname, int totseqs, int *eachlength, region *region_used,
		    int *sel_seqs, int tot_sel_seqs, int spaces_in_names, int pad_to_max_length=FALSE);
#ifdef __APPLE__
extern char *mac_fname_to_roman(const char *in); 
#endif

#endif
