//
// Copyright 1996-2018 by Manolo Gouy.
//
// This program, except where indicated, is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; 
// either version 3 of the License, or (at your option) any later version.
//
// Three files (dnapars.c, protpars.c and seq.c) have a distinct license
// which authorizes distribution. They are Copyright Joseph Felsenstein
//
// Three files (bionj.c, phyml_util.c and phyml_util.h) are also covered by the 
// GNU General Public License but are copyright Stephane Guindon.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// Additional permission under GNU GPL version 3 section 7
// 
// If you modify this Program, or any covered work, by linking or combining it with PHYLIP 
// (or a modified version of that library), containing parts covered by the terms of 
// the University of Washington license, the licensor of this Program grants you additional 
// permission to convey the resulting work. 
//
// A copy of the GNU General Public License is available
// from http://www.gnu.org/licenses/gpl.txt .
//
// Please report all bugs and problems to "manolo.gouy@univ-lyon1.fr".

/*
 Version 4.2    10_OCT_2009
  Added cladogram tree display and copy fasta-seqs to clipboard.
 Version 4.1    27_JUN_2009
  Added nucleotide ambiguity symbols in consensus DNA sequences
 Version 4.0    27_MAY_2009
  Added tree computing and drawing
 Version 3.2    22_OCT_2008
	Import from databases: can use keyword
                15_OCT_2008
	WIN32: correct display of UTF8 filenames in window and messages
 Version 3.1    08_OCT_2008
	now use Fl_Native_File_Chooser (with slightly changed macOS version)
	MacOS: correct display of UTF8 filenames in windows and menu and in messages for
		left-to-right scripts
 Version 3.0    11_SEP_2008
	added File:Import from DB
 Version 2.4    04_MAR_2008
                phylip format allows to customize max name length
 Version 2.3    22_FEB_2008
		corrected bug if deleting/adding/moving seqs when in 'view as proteins' mode
 Version 2.2    6_OCT_2007
		added support for multiple alignment windows
 Version 2.1    19_SEP_2007
		added interface to external multiple alignment programs
 Version 2.0	22_AUG_2007
		added Props:View as proteins
		added Edit:Set genetic code
		added Edit:Align all sequences
		added File:Save prot alignmnt
*/
#if !defined(WIN32)
#include <unistd.h>
#endif

#include "seaview.h"
#include "treedraw.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Help_Dialog.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Paged_Device.H>

#if !defined(__APPLE__)
#include "seaview.xpm"
#if !defined(WIN32) && SEAVIEW_FLTK_VERSION < 133
Fl_Offscreen seaview_X11_icon;
#endif
#endif


/* allongement maximal prevu pour les seqs suite a edition */
#define MAX_SEQ_ALLONG 5000
#define CLIPBOARDMARK "seaview_alignment_clipboard:"
/* 
Purpose
resource	argument	type		default			exemple

Default file format for saving operations
save		-save		{mase, phylip,	mase			phylip
			    clustal, msf, fasta, nexus}

Control of printout options
printoutblock	N/A		integer		10			3
printoutcpl	N/A		integer		80			90
	[this is the paper width, not the # of residues desired on each line]
printoutlpp	N/A		integer		66			90
Name of help file
helpfile	N/A		string		"seaview.help"

Standard coloring of protein sequences (<= 10 colors + white for gaps)
(colors are red, green, yellow, blue, cyan, magenta, salmon, purple, aquamarine,
and dark-gray)
stdcolorgroups	N/A		string		EDQNHRKBZ,ILMV,APSGT,FY,WC
						      BZDE,ACWY,FGHIK,LMNPQRSTV
Alternate coloring of protein sequences
altcolorgroups	N/A		string		\0	AC,DEFGHIK,LMNPQRS,WY,TV

Faster but less smooth writing
fast		-fast		no-value	False

Residues colored on background
inverted	-inverted	no-value	False

N/A		filename
*/	

Fl_Group *create_dna_scroller(SEA_VIEW *view, int x, int y, int w, int h, 
	int dbl_buff);
SEA_VIEW *create_the_form(int double_buffer);
int prep_custom_colors(int *colors, char *customcolors, 
	int max_colors);
color_choice prep_aa_color_code(char *list_std, char *list_alt, 
	int maxprotcolors, int *numb_stdprotcolors, int *numb_altprotcolors);
void deplacer_grp_seqs(SEA_VIEW *view, int target);
void del_gap_only_sites(SEA_VIEW *view);
void reference_toggle(SEA_VIEW *view, int on);
void help_callback(Fl_Widget *ob, void *unused);
void help_items_callback(Fl_Widget *ob, void *data);
void direct_help_callback(Fl_Widget *wgt, void *data);
void handle_paste(SEA_VIEW *view, char *clipboard, int doing_dnd);
void handle_mouse(SEA_VIEW *view, int mx, int my, 
	int *p_selecting_seqs, int *p_sel_seq_move, int *p_modifying_segment);
void handle_keyboard(SEA_VIEW *view, unsigned int key, int istext);
void handle_push(SEA_VIEW *view, int mx, int my, int key, 
	int *p_modifying_segment, int *p_selecting_seqs, int *p_sel_seq_move);
int delete_gaps_before(SEA_VIEW *view, int numseq, int numsite, int total);
void set_save_format(SEA_VIEW *view, int val);
Fl_Window *use_initial_file(SEA_VIEW *view, char *masename, int doing_dnd);
Fl_Window* pdfps_options_dialog(SEA_VIEW *view, bool autonomous);
void mainwin_close_callback(Fl_Widget *form, void *data);
void to_do_at_exit(void);
SEA_VIEW *newwindow_callback(SEA_VIEW *old_view);
//void fix_paste_timeout(void *);
void hide_window_callback(Fl_Widget *ob, void *data);
void free_colranks_by_difference(char **alt_col_rank, int total);
char *create_tmp_filename(void);
void delete_tmp_filename(const char *base_fname);
void draw_seq_names(Fl_Widget *ob, SEA_VIEW *view);

/* external functions */
void draw_comment_lines(Fl_Widget *ob, SEA_VIEW *view);
char* seaview_file_chooser_save_as(const char* message, 
	const char* fname, SEA_VIEW *view, known_format* new_format);
void load_resources(const char *progname);
char *get_res_value(const char *name, const char *def_value);
int int_res_value(const char *name, int def_value);
int bool_res_value(const char *name, int def_value);
void minuscules(char *);
known_format what_format(const char *filename);
extern void custom_callback(Fl_Widget *obj, void *data);
extern void stats_callback(Fl_Widget *obj, void *data);
void adjust_menu_edit_modes(SEA_VIEW *view);
extern void *set_viewasprots(SEA_VIEW * view, int onoff);
extern void racnuc_dialog(SEA_VIEW *view);
extern void disconnect_tree_windows(SEA_VIEW *view);
extern void concatenate_dialog(SEA_VIEW *view);
extern int get_ncbi_gc_from_comment(char *comment);
extern "C" {
  char codaa(char *codon, int code);
  int get_acnuc_gc_number(int ncbi_gc);
  void compact(char *chaine);
  }
#ifdef __APPLE__
extern char *mac_GetOutputFName_Plus(const char *dfault, const char *message, int use_only_button,
									 const char *dirname);
#if 100*FL_MAJOR_VERSION + FL_MINOR_VERSION < 104
  extern int find_windowmenuitem(Fl_Window *w);
  extern int add_windowmenuitem(const char *name, Fl_Window *w);
  extern void rename_windowmenuitem(const char *name, int rank);
  extern void delete_windowmenuitem(int rank);
#endif
extern int set_res_value(const char *name, const char *value);
extern void fl_mac_set_about( Fl_Callback *cb, void *user_data, int shortcut );
extern void MG_apple_inits(void);
extern void mac_tmp_dir_name(char *buffer, int l);
#elif defined(WIN32)
extern char **getutf8args(int *pargc);
#else
extern void inform_prog_dir(const char *arg0);
#endif
extern int printout(SEA_VIEW *view, const char *filename, 
	int fontsize, int block_size, Fl_Paged_Device::Page_Format pageformat, int vary_only, int ref0, 
	int pdfkindvalue, Fl_Paged_Device::Page_Layout layout, int svg_width = 0);
extern void set_ncbi_genetic_code(SEA_VIEW *view);
extern void cre_align_menu(SEA_VIEW *view);
extern void init_msa_algos(void);
extern void parse_trees_from_header(char *header, SEA_VIEW *view);
extern void decode_codon_colors(int *colors);
extern void select_deselect_in_tree(SEA_VIEW *view);
extern void format_conversion(int argc, char **argv);
extern void command_line_phylogeny(int argc, char **argv);
extern int command_line_align(int argc, char **argv);
extern void concatenate_no_gui(int argc, char *argv[]);
extern void save_bootstrap_replicates(const char *fname, int replicates, SEA_VIEW *view);

/* global variables */
/* to control parameters of option printout */
Fl_Paged_Device::Page_Format printout_pageformat;
Fl_Paged_Device::Page_Layout printout_layout = Fl_Paged_Device::PORTRAIT;
int printout_block, printout_fontsize, printout_black = PDF_COLOR, print;
int printout_vary = FALSE;
const char *f_format_names[] = {"Mase", "Phylip", "Clustal", "MSF", "Fasta", "NEXUS"};
const char *f_format_exts[] = {"mase", "phy", "aln", "msf", "fst", "nxs"};
int nbr_formats = sizeof(f_format_names)/sizeof(char *);
//char def_stdcolorgroups[] = "EDQNHRKBZ,ILMV,APSGT,FY,WC";
char def_stdcolorgroups[] = "KR,AFILMVW,NQST,HY,C,DE,P,G";
//int def_protcolors[] =
//	{ FL_WHITE, FL_RED, FL_GREEN, FL_YELLOW, FL_BLUE, FL_CYAN,
//	FL_MAGENTA, 9/*salmon*/, 13/*purple*/, 14/*aquamarine*/, 8/*darkgray*/};
int def_protcolors_rgb[] = 
	{255,255,255, /* white */
	230,51,25,   25,128,230,   25,204,25,   25,179,179,   230,128,128,
	204,77,204,   204,204,0,   230,153,77,
	128,128,128,    100,100,100 };
int numb_stdprotcolors, numb_altprotcolors;
color_choice curr_color_choice;
int max_protcolors = sizeof(def_protcolors_rgb) / sizeof(int) / 3;
int *def_protcolors;
const char *progname = "seaview";
char *clipboardfname = NULL; // name of temporary file used to hold copied data
extern char aminoacids[]; // from misc_acnuc.c
static int defaultfontsize = 
#ifdef WIN32
	12;
#else
	14;
#endif

#ifdef WIN32
#define LINE_HEIGHT_FACTOR  0.85
#else
#define LINE_HEIGHT_FACTOR  1.
#endif
static int pasted_from_what_source; // gives what source was used for paste: 0=selection, 1=clipboard

extern void plotonly(int argc, char *argv[]);
extern void printout_cmdline(int argc, char **argv);
extern bool isarg(int argc, char *argv[], const char *arg);
extern char *argname(int argc, char *argv[], const char *arg);


int main(int argc, char *argv[])
{
#if defined(WIN32)	
  argv = getutf8args(&argc);
#endif
  if (isarg(argc, argv, "-h")) {
    fputs("Usage:\nseaview [options] [alignment-or-tree-file]\n"
	    "where alignment-or-tree-file is an optional sequence alignment or tree file to be read (always the last argument) and options are:\n"
	    "-h            display all program options and exit\n"
	    "-fontsize n   font size used for the tree plot or alignment windows\n"
	    "-fast         sequences will be displayed faster but less smoothly\n"
	    "     Options for non-interactive usage driven by command-line arguments\n"
	    "(Use exactly one for a non-interactive seaview run. Add any sub-option described below)\n"
	    "-convert      convert an input alignment to another format (no window creation)\n"
	    "-concatenate  concatenate alignment(s) to the end of an input alignment (no window creation)\n"
	    "-align        align an input sequence file (no window creation)\n"
	    "-build_tree   compute a phylogenetic tree from an input alignment file (no window creation)\n"
#ifndef NO_PDF
	    "-printout     draws a multiple sequence alignment to PDF/SVG (no window creation)\n"
	    "-reroot       modify the rooting of, or process otherwise, an input tree (no window creation)\n"
	    "-plotonly     draw an input tree in a pdf or svg file (no window creation)\n"
#endif
	    
	    "\n"
	    "           Sub-options for -convert\n"
	    "           Use '-' as last argument to read alignment from standard input\n"
	    "-output_format fmt    format of the converted alignment file (mase, phylip, clustal, msf, fasta, or nexus)\n"
	    "-o fname      use fname as name of the converted alignment (default is built from input filename)\n"
	    "-o -          write the output alignment to standard output\n"
	    "-translate    translate input sequences to protein before outputting them (don't use -sites)\n"
	    "-no_terminal_stop   translate terminal stop codons as a gap (with -translate option)\n"
	    "-del_gap_only_sites  remove all gap-only sites from alignment (don't use the -sites option)\n"
	    "-def_species_group group_name,group_member_ranks   create a species group of given name and members\n"
	    "                  (species group members are expressed with their ranks as in this example: 3-8,12,19)\n"
	    "-def_site_selection name,endpoints   create a selection of sites of given name and endpoints\n"
	    "                  (site selection endpoints are expressed as in this example: 10-200,305,310-342)\n"
	    "-gblocks      create under the name 'Gblocks' a set of blocks of conserved sites with the Gblocks program\n"
      "                  (requires the nexus or mase output formats)\n"
	    "      -gblocks-specific options:\n"
	    "              -b4 allow smaller final blocks\n"
	    "              -b5 allow gaps within final blocks \n"
	    "              -b2 less strict flanking positions \n"
	    "              -b3 don't allow many contiguous nonconserved positions\n"
	    "-sites selection_name    use the named selection of sites from the input alignment \n"
	    "-species species_group_name    use the named group of species from the input alignment \n"
      "-bootstrap n    writes n bootstrap replicates of the input alignment to the output file \n"
	    
	    "\n"
	    "           Sub-options for -concatenate \n"
	    "           Use '-' as last argument to read alignment from standard input\n"
	    "-concatenate align1,...    name(s) of alignment files to add at the end of the input alignment\n"
	    "-by_rank      identify sequences by their rank in alignments (rather than by their name)\n"
      "-record_partition record the locations of the concatenated pieces in the final concatenate\n"
	    "-output_format fmt    format of the concatenated alignment (mase, phylip, clustal, msf, fasta, or nexus)\n"
	    "-o fname      use fname as name of the concatenated alignment (default is built from input filename)\n"
	    "-o -          write the concatenated alignment to standard output\n"
	    
	    "\n"
	    "           Sub-options for -align \n"
	    "           Use '-' as last argument to read sequence file from standard input\n"
	    "-align_algo n  rank (in seaview from 0) of alignment algorithm, otherwise use seaview's default alignment algorithm\n"
	    "-align_extra_opts \"option1 ...\"  additional options to use when running the alignment algorithm\n"
	    "-align_at_protein_level  translate and align input sequences and reproduce alignment at DNA level.\n"
	    "-output_format fmt    format of the output alignment (mase, phylip, clustal, msf, fasta, or nexus)\n"
	    "-o fname      use fname as name of the output alignment (default is built from input filename)\n"
	    "-o -          write the output alignment to standard output\n"
	    
	    "\n"
	    "           Sub-options for -build_tree (either -distance or -parsimony is required)\n"
	    "           Use '-' as last argument to read alignment from standard input\n"
	    "-o fname      use fname as name of the output tree\n"
	    "-o -          write the output tree to standard output\n"
	    "-distance dist_name   computes the tree with a distance method using dist_name (observed, JC, K2P, logdet, Ka, Ks, Poisson or Kimura)\n"
	    "-distance_matrix fname  don't compute the tree, but write to fname the matrix of pairwise distances\n"
	    "-NJ           compute the distance tree by the Neighbor-Joining method (default is BioNJ)\n"
	    "-parsimony    compute the tree by the parsimony method \n"
	    "-search more|less|one  controls how much rearrangement is done to find better trees (DNA parsimony only)\n"
	    "-nogaps       remove all gap-containing sites before computations\n"
	    "-replicates n use n bootstrap replicates to compute tree branch support\n"
	    "-jumbles n    jumble sequence order n times (parsimony only)\n"
	    "-gaps_as_unknown  encode gaps as unknown character state (parsimony only)\n"
	    "-sites selection_name    use the named selection of sites from the input alignment \n"
	    "-species species_group_name    use the named group of species from the input alignment \n"

#ifndef NO_PDF
	  "\n"
	  "           Sub-options for -printout (-o fname is required)\n"
	  "           Use '-' as last argument to read alignment from standard input\n"
	  "-o fname     use fname as name of the output PDF/SVG file\n"
	  "-o -         write the printout to standard output\n"
	  "-fontsize n  font size used for the alignment printout (10 by default)\n"
	  "-blocksize n divide sequences in blocks of size n (10 by default) separated by one space\n"
	  "-svg  w      produce a file in SVG format of width w pixels\n"
	  "               (without this option, the output is in PDF format)\n"
	  "-landscape   write the alignment in landscape orientation (PDF output only)\n"
	  "-letter      write the alignment using Letter-sized pages (A4 by default, PDF output only)\n"

	    "\n"
	    "           Sub-options for -reroot\n"
	    "           Use '-' as last argument to read input tree from standard input\n"
	    "-outnewick filename   write the resulting tree to named file in Newick format\n"
	    "-outgroup name   use tree leaf labelled 'name' as tree outgroup\n"
	    "-outgroup \"name1,name2,name3\" use node at center of the 3 names as tree center\n"
	    "               and name3-containing clade as outgroup\n"
	    "-ingroup \"name1,name2,name3\" use node at center of the 3 names as tree center\n"
	    "               and name1-containing clade as ingroup\n"
	    "-root_at_center   reroot the tree at its center\n"
	    "-unroot       remove the root from the input tree, if any\n"
            "-trim r       rule to trim long tree leaf names in output tree file (def=no trimming)\n"
	    "-patristic_distances fname    write to named file patristic distances for leaves of input tree\n"
	    "-remove_bootstrap    remove branch support values present in input tree\n"

	    "\n"
	    "           Sub-options for -plotonly\n"
	    "options -outgroup, -ingroup, -outnewick above plus the following:\n"
	    "           Use '-' as last argument to read tree from standard input\n"
	    "-o fname      use fname as name of the output plot (.pdf or .svg)\n"
	    "-o -          write the output plot to standard output (svg only)\n"
	    "-svg          draw the input tree in a .svg file (default is .pdf file)\n"
	    "-unrooted     draw the input tree in unrooted (circular) form\n"
	    "-pagecount n  number of pages used for the tree plot (pdf only)\n"
	    "-letter       use letter-sized paper for the tree plot\n"
	    "-fontsize n   font size used in the tree plot\n"
	    "-lengths      display branch lengths in the tree plot\n"
	    "-bootstrap    display branch support values in the tree plot\n"
	    "-bootstrap_threshold x    display branch supports if >= x\n"
	    "-size WxH     pixel Width and Height of tree plot (svg only) (default is 595x842)\n"
	    "-match text   display in color (red by default) tree labels containing this text (case is not significant)\n"
	    "-color RRGGBB hexadecimal color used for matching labels (example: 00FF00 means green)\n"
	    "-branch_width_as_support  draw plot with branches of increasing widths with increasing support\n"
	    "-support_threshold_high f support above which a branch is drawn with maximal width (def=.95)\n"
	    "-support_threshold_low f support below which a branch is drawn with minimal width (def=.8)\n"
            "-trim r       rule to trim long tree leaf names (def=no trimming)\n"
#endif
	    , stdout);
    fputs("\nSee http://doua.prabi.fr/software/seaview_data/seaview#Program%20arguments for more details\n", stdout);
    return 0;
    }
#if !(defined(WIN32) || defined(__APPLE__))
  inform_prog_dir(argv[0]);
#endif
#ifndef NO_PDF
  if ( isarg(argc, argv, "-remove_bootstrap") && !isarg(argc, argv, "-reroot")) {
    fprintf(stderr, "Must use also -reroot when -remove_bootstrap is used\n");
    exit(1);
  }
  if (isarg(argc, argv, "-plotonly") || isarg(argc, argv, "-reroot")) {
    plotonly(argc, argv);
    return 0;
    }
  if (isarg(argc, argv, "-printout")) {
    printout_cmdline(argc, argv);
    return 0;
  }
#endif
  if (isarg(argc, argv, "-convert")) {
    format_conversion(argc, argv);
    return 0;
  }
  if (isarg(argc, argv, "-build_tree")) {
    command_line_phylogeny(argc, argv);
    return 0;
  }
  if (isarg(argc, argv, "-concatenate")) {
    concatenate_no_gui(argc, argv);
    return 0;
  }
  if (isarg(argc, argv, "-align")) {
    return command_line_align(argc, argv);
  }
#ifdef __APPLE__ 
  MG_apple_inits();
#elif SEAVIEW_FLTK_VERSION >= 133
  Fl_Pixmap* px = new Fl_Pixmap(seaview_xpm);
  const Fl_RGB_Image* icon = new Fl_RGB_Image(px);
  delete px;
  Fl_Window::default_icon(icon);
  delete icon;  
#elif !defined(WIN32)
  fl_open_display();
  Fl_Pixmap *pixmap = new Fl_Pixmap(seaview_xpm);
  seaview_X11_icon = XCreatePixmap(fl_display, RootWindow(fl_display, fl_screen), 
			       pixmap->w(), pixmap->h(), fl_visual->depth);
  fl_gc = XCreateGC(fl_display, seaview_X11_icon, 0, 0);
  fl_begin_offscreen(seaview_X11_icon);
  pixmap->draw(0,0);
  fl_end_offscreen();
  delete pixmap;
  XFreeGC(fl_display, fl_gc);
#endif
  Fl_Window::default_xclass("FLTK");
char *masename;
int i;

static int quick_and_dirty; 

load_resources(progname);
init_msa_algos();

// access to resources 
#ifdef __APPLE__
quick_and_dirty = TRUE;
#else
quick_and_dirty = bool_res_value("fast", FALSE);
#endif
printout_block = int_res_value("printoutblock", 10);
printout_fontsize = int_res_value("printoutfontsize", 10);
if(strcmp( get_res_value("printoutpageformat", "A4") , "A4") == 0)
  printout_pageformat = Fl_Paged_Device::A4;
else printout_pageformat = Fl_Paged_Device::LETTER;
  defaultfontsize = int_res_value("sequencefontsize", defaultfontsize);
fl_message_font(FL_HELVETICA_BOLD, FL_NORMAL_SIZE );

// argument processing 
masename = NULL;
i = 1;
while(i < argc) {
	if(argv[i][0] != '-') masename = argv[i];
	if(strcmp(argv[i], "-fast") == 0) quick_and_dirty = TRUE;
	if(strcmp(argv[i], "-fontsize") == 0) {
		if(++i < argc) sscanf(argv[i], "%d", &defaultfontsize);
		}
	i++;
	}
  clipboardfname = strdup( create_tmp_filename() );
  atexit(to_do_at_exit);
#ifdef __APPLE__
  SEA_VIEW *view = NULL;
  Fl::wait(0); // creates the windows related to files dropped at launch
  if (Fl::first_window() == 0) {
    // if there is no window already, create an empty one
    view = create_the_form(!quick_and_dirty);
  } else { // a window is created, do not process file in argument list
    masename = 0;
  }
#else
SEA_VIEW *view = create_the_form(!quick_and_dirty);
#endif
//Fl::add_timeout(0.5, fix_paste_timeout);
if (masename != NULL) use_initial_file(view, masename, FALSE);
Fl::lock();
Fl::run();
return 0;
}


Fl_Window *use_initial_file(SEA_VIEW *view, char *masename, int doing_dnd)
/* returns the window containing the loaded data (alignment or tree) or NULL
 */
{
known_format defaultformat;
Fl_Window *w = NULL;

defaultformat = what_format(masename);
if ((int)defaultformat >= 0) {
	w = load_alignment_file(view, masename, NULL, defaultformat, doing_dnd);
	}
else if((int)defaultformat == -2) {// a Newick tree
	FILE *in = fopen(masename, "r");
	fseek(in, 0, SEEK_END);
	long l = ftell(in);
	fseek(in, 0, SEEK_SET);
	char *tree = (char *)malloc(l + 1);
	char *p = tree;
	while(l-- > 0) {
		char c = fgetc(in);
		if(c != '\n' && c != '\r') *(p++) = c;
		}
	*p = 0;
	fclose(in);
	w = treedraw(tree, view, extract_filename(masename), FALSE);
	if (view != NULL && view->tot_seqs == 0) { // closes an empty alignment window that opened the tree
    view->dnawin->do_callback();
	}
}
else
		fl_alert("File %s\nis not of a format readable by seaview", masename);
return w;
}


#ifdef _AIX
/* sur IBM RISC __filbuf est en fait _filbuf utilise a l'interieur de xforms */
int __filbuf(FILE *fich)
{
return _filbuf(fich);
}
#endif


class DNA_obj : public Fl_Widget {
    FL_EXPORT void draw(void);
    FL_EXPORT int handle(int);
public:
    FL_EXPORT DNA_obj(int x,int y,int w,int h, void *view) :
		Fl_Widget(x,y,w,h,NULL) {
	this->user_data(view);
	}
};


void out_of_memory(void)
{
Fl_Window *w;
while((w = Fl::first_window()) != NULL) w->hide();
fl_alert("Seaview error: Not enough memory!");
exit(1);
}


void minuscules(char *p)
{
if(p == NULL) return;
while(*p) {
	*p = tolower(*p);
	p++;
	}
return;
}


const char *extract_filename(const char *fname)
{
static const char *p, *q;
q = fname;
if(q == NULL) return (char *)"";
do	{ 
#ifdef __VMS
	p = strchr(q,']'); 
	if(p == NULL) p = strchr(q,':'); 
#elif defined(WIN32)
	p = strchr(q,'\\'); 
#else
	p = strchr(q,'/'); 
#endif
	if(p != NULL) q = p+1; 
	}
while 	(p != NULL);
return q;
}


char *create_tmp_filename()
// returns a temporary file name in private memory or NULL
// need to call delete_tmp_filename() after all temporary files usage
{
#if defined(WIN32)
  static char *buff = NULL;
  static int lbuff = 0;
  wchar_t *wc = _wtempnam(NULL, L"seaview_tmp");
  if (wc != NULL) {
    FILE *in = _wfopen(wc, L"w");
    if (in != NULL) fclose(in);
    int l = wcslen(wc);
    unsigned l2 = fl_utf8fromwc(NULL, 0, wc, l);
    if (l2+1 > lbuff) {
      lbuff = l2+1;
      buff = (char*)realloc(buff, lbuff);
      }
    fl_utf8fromwc(buff, l2+1, wc, l);
  }
  return buff;
#else
  char *retval;
  int fd;
  static char buffer[PATH_MAX];
#ifdef __APPLE__
  mac_tmp_dir_name(buffer, PATH_MAX);
#else
  strcpy(buffer, "/tmp/");
#endif
	strcat(buffer, "seaview.XXXXXX");
	if ((fd = mkstemp(buffer)) == -1) return NULL;
	close(fd);
	retval = buffer;
  return retval;
#endif // WIN32
}


#ifndef WIN32
void delete_tmp_filename(const char *base_fname)
{
  char fname[PATH_MAX];
  sprintf(fname, "PATH=/bin:/usr/bin rm \"%s\"*", base_fname);
  system(fname);
}
#endif


void allonge_seqs(char **seq, int totseqs, int maxlen, int *eachlength,
	int tot_comment_lines, char **comment_line, char **pregion_line)
{
int num, l;
char *newseq;

for(num = 0; num < totseqs; num++) {
	l = eachlength[num];
	newseq = (char *)malloc(maxlen+1);
	if(newseq==NULL) out_of_memory();
	memcpy(newseq,seq[num],l+1);
	free(seq[num]); 
	seq[num] = newseq;
	}
for(num = 0; num < tot_comment_lines; num++) {
	l = strlen(comment_line[num]);
	newseq = (char *)malloc(maxlen+1);
	if(newseq == NULL) out_of_memory();
	memcpy(newseq, comment_line[num], l+1);
	free(comment_line[num]); 
	comment_line[num] = newseq;
	}
  if (pregion_line) *pregion_line = (char*)realloc(*pregion_line, maxlen + 1);
}


/* memoire pour contenir coloriage standard */
static int color_for_aa_gaps;
static char std_aminoacids[30];
static int std_aa_color[30];
/* memoire pour contenir coloriage alternatif */
static char alt_aminoacids[30] = "";
static int alt_aa_color[30];
/* pointeurs vers coloriage courant */
static char *current_aminoacids;
static int *current_aa_color;


int decode_color_scheme(char *vlist,  char *aas, int *aa_color,
	int maxprotcolors)
{
int nbr_colors = 1, current = 0, i;
char *p, *list;
list = strdup(vlist);
if(list == NULL) return 1;
aas[0] = 0;
p=strtok(list,",");
while( p!= NULL && nbr_colors < maxprotcolors) {
	strcat(aas,p);
	for(i=0; i < (int) strlen(p); i++)
		aa_color[current++] = nbr_colors;
	nbr_colors++;
	p=strtok(NULL,",");
	}
free(list);
return nbr_colors;
}


int prep_custom_colors(int *colors, char *customcolors, int max_colors)
{
char *nom;
int rank = 0, r, g, b;
Fl_Color col;

if(*customcolors == 0) return max_colors;
nom = strtok(customcolors, ",");
while(nom != NULL && rank < max_colors - 1) {
	r = g = b = -1;
	sscanf(nom, "%d%d%d", &r, &g, &b);
	if( r>=0 && g>=0 && b>=0 ) 
		col = fl_rgb_color(r, g, b);
	else col = FL_BLACK;
	colors[++rank] = (int)col;
	nom = strtok(NULL, ",");
	}
return rank + 1;
}


color_choice prep_aa_color_code(char *list_std, char *list_alt, 
	int maxprotcolors, int *numb_stdprotcolors, int *numb_altprotcolors)
{
/* couleur pour gaps = 1ere couleur connue (comptee a partir de 0) */
color_for_aa_gaps = 0;
current_aminoacids = std_aminoacids;
current_aa_color = std_aa_color;
/* decodage du coloriage standard des proteines */
*numb_stdprotcolors = 
	decode_color_scheme(list_std, std_aminoacids, std_aa_color,
		maxprotcolors);
*numb_altprotcolors = 0;
if(*list_alt == 0) return NO_ALT_COLORS;
/* decodage du coloriage alternatif des proteines */
*numb_altprotcolors = 
	decode_color_scheme(list_alt, alt_aminoacids, alt_aa_color,
		maxprotcolors);
return USING_STANDARD_COLORS;
}


void set_aa_color_mode(color_choice choice)
{
if(choice == USING_ALT_COLORS) {
	current_aminoacids = alt_aminoacids;
	current_aa_color = alt_aa_color;
	}
else	{
	current_aminoacids = std_aminoacids;
	current_aa_color = std_aa_color;
	}
}


int get_color_for_aa( int key )
/* returns the color # used to display character key in protein */
{
char *pos;
pos = strchr(current_aminoacids, toupper(key) ); 
return ( pos == NULL ? 
	color_for_aa_gaps : current_aa_color[pos - current_aminoacids] );
}


int get_color_for_base( int key )
/* returns the color # used to display character key in DNA */
{
	int retval;
	if(key=='A' || key == 'a') retval = 1;
	else if(key=='C' || key == 'c') retval = 2;
	else if(key=='G' || key == 'g') retval = 3;
	else if(key=='T' || key == 't' || key=='U' || key == 'u') retval = 4;
	else retval = 0;
	return retval; 
}


char **prepcolranks(char **seq, int totseqs, int maxlen, int *eachlength,
					int (*calc_color_function)( int ), int numb_gc, int allow_lower)
{
	int num, l, i;
	char **colrank;
	if(totseqs == 0 || numb_gc == 1) return NULL;
	colrank=(char **)malloc(totseqs*sizeof(char *));
	if(colrank == NULL) return NULL;
	for(num=0; num<totseqs; num++) {
		l = eachlength[num];
		colrank[num]=(char *)malloc(maxlen + 1);
		if(colrank[num] == NULL) return NULL;
		for(i=0; i<l; i++) {
			colrank[num][i] = (char)calc_color_function( seq[num][i] );
		}
	}
	return colrank;
}

char **prepcolranks_by_codon(char **seq, int totseqs, int maxlen, int *eachlength, char **comments)
{
	int num, l, i, c, gc;
	char **colrank, *p, aa;
	if(totseqs == 0) return NULL;
	colrank = (char **)malloc(totseqs*sizeof(char *));
	if(colrank == NULL) return NULL;
	for(num=0; num<totseqs; num++) { /* allocation memoire */
		l = eachlength[num];
		colrank[num]=(char *)malloc(maxlen + 1);
		if(colrank[num] == NULL) return NULL;
		gc = get_acnuc_gc_number(get_ncbi_gc_from_comment(comments[num]));
		for(i = 0; i < l; i += 3) {
			if(i+2 < l) aa = codaa(seq[num] + i, gc);
			else aa = 'X';
			p = strchr(aminoacids, aa);
			c = (p != NULL ? p - aminoacids : 21);
			if(c == 21) c = 0;
			else c++;
			colrank[num][i] = c;
			if(i + 1 < l) colrank[num][i + 1] = c;
			if(i + 2 < l) colrank[num][i + 2] = c;
			}
	}
	return colrank;
}


char **prepcolranks_by_difference(char **seq, int totseqs, int ref_seq0,
	int maxlen, int *eachlength,
	int (*calc_color_function)( int ), int numb_gc, int allow_lower)
{
int num, l, i, res;
char **colrank;
if(totseqs == 0) return NULL;
colrank = (char **)malloc(totseqs*sizeof(char *));
if(colrank == NULL) return NULL;
for(num=0; num<totseqs; num++) { /* allocation memoire */
	l = eachlength[num];
	colrank[num]=(char *)malloc(maxlen + 1);
	if(colrank[num] == NULL) return NULL;
	}
for(i=0; i<eachlength[ref_seq0]; i++) { /* coloration seq de reference */
	res = seq[ref_seq0][i];
	colrank[ref_seq0][i] = (char)calc_color_function( res );
	}
for(num=0; num<totseqs; num++) { /* coloration des autres sequences */
	if(num == ref_seq0) continue;
	l = eachlength[num];
	for(i=0; i<l; i++) {
		res = seq[num][i];
		if(toupper(res) != toupper(seq[ref_seq0][i])) {
			colrank[num][i] = (char)calc_color_function( res );
			}
		else	{
			colrank[num][i] = 0;
			}
		}
	}
return colrank;
}


void draw_cursor(Fl_Widget *ob, int on_off, int site, int seq, 
	int cursor_in_comment)
{
SEA_VIEW *view = (SEA_VIEW *)ob->user_data() ;
int  x, y, cursor_x, cursor_y, c, max_curs_coord;
char *debut, *fin;
int background, foreground;
static char lettre[]="A";
static char cursor_coord[200];
Fl_Menu_Item *menu_props = (Fl_Menu_Item *)view->menu_props;
props_menu_parts *props_parts = (props_menu_parts *)menu_props->user_data();
int byreference = (menu_props + props_parts->reference)->value();
		
if( (!cursor_in_comment) && (seq == 0 || site > view->each_length[seq-1] + 1) ) 
	return;
cursor_x = site - view->first_site;
if(cursor_in_comment) {
	if(view->tot_comment_lines == 0 || !view->show_comment_lines) return;
	if(cursor_x < 0 || cursor_x >= view->tot_sites ) return;
	if(seq <= 0 || seq > view->tot_comment_lines ) return;
	y = view->y_seq + (seq + view->pos_first_comment_line - 2) * 	
		view->line_height;
	x = view->x_seq + cursor_x * view->char_width;
	lettre[0] = view->comment_line[seq-1][site-1];
	}
else	{
	if(view->tot_seqs == 0) return;
	if (view->old_cursor_seq != seq) {
	  view->draw_names = view->old_cursor_seq; // ecrire en noir nom ancien curseur
	  draw_seq_names(view->DNA_obj, view);
	  }
	view->draw_names = seq; // ecrire en jaune nom nouveau curseur
	draw_seq_names(view->DNA_obj, view);
	view->draw_names = -1;
	cursor_y = seq - view->first_seq;
	if(cursor_x < 0 || cursor_x >= view->tot_sites || cursor_y < 0 || cursor_y >= view->tot_lines ) {
	  return;
	  }
	y = view->y_seq + cursor_y * view->line_height;
	x = view->x_seq + cursor_x * view->char_width;
	lettre[0] = view->sequence[seq-1][site-1];
	if(!view->allow_lower) lettre[0] = toupper(lettre[0]);
	if(byreference && (!on_off) && seq >= 2) {
		if(toupper(lettre[0]) == toupper(view->sequence[0][site-1])) lettre[0] = '.';
		}
	}
if(view->numb_gc == 1) { /* cas noir et blanc */
	if(on_off) {
		foreground = FL_WHITE;
		background = FL_BLACK;
		}
	else	{
		foreground = FL_BLACK;
		background = FL_WHITE;
		}
	}
else if( (!cursor_in_comment) && view->inverted_colors) { 
	/* cas inverted colors */
	if(site <= view->each_length[seq - 1]) {
		if( (c = (int)view->col_rank[seq-1][site-1]) == 0) background = ob->color();
		else background = view->curr_colors[c];
		}
	else
		background = ob->color();
	if(on_off) {
		foreground = background;
		background = FL_BLACK;
		}
	else	{
		if(view->active_region == NULL || 
				view->region_line[site - 1] == 'X')
			foreground = FL_BLACK;
		else
			foreground = ob->selection_color();
		}
	}
else if(cursor_in_comment) { /* dans les lignes comment sur ecran couleur */
	if(on_off) {
		foreground = FL_WHITE; background = FL_BLACK;
		}
	else	{
		foreground = FL_BLACK; background = ob->color();
		}
	}
else	{ /* cas colored letters */
	if( site <= view->each_length[seq - 1]) {
		foreground = view->curr_colors[(unsigned)view->col_rank[seq-1][site-1]];
		}
	else	{
		lettre[0] = ' ';
		foreground = FL_BLACK;
		}
	if(on_off) {
		background = FL_BLACK;
		}
	else	{
		if( (!cursor_in_comment) && view->active_region != NULL && 
				view->region_line[site - 1] == 'X')
			background = ob->selection_color();
		else
			background = ob->color();
		}
	}
fl_font(ob->labelfont(), ob->labelsize());
fl_color(background);
fl_rectf( x, y - view->line_height + fl_descent(), 
	view->char_width +1,
	view->line_height);
fl_color(foreground); 
fl_draw(lettre, 1, x, y);
if(on_off) {
/* ecriture des coordonnees du curseur */
	max_curs_coord = view->tot_sites - 14;
	x = view->x_seq + 7 * view->char_width;
	y = view->y_seq - view->line_height;
	fl_font(FL_COURIER, ob->labelsize());
	fl_color(ob->color());
	fl_rectf( x, y - view->line_height, 
		(max_curs_coord ) * view->char_width, 
		view->line_height + fl_descent());
	if(!cursor_in_comment) {
		debut = view->sequence[seq-1] - 1; fin = debut + site; c = 0;
		if(*fin == 0) fin--;
		while(++debut <= fin) if( *debut != '-' ) c++;	
		sprintf(cursor_coord, "Seq:%d", seq);
		if(view->curr_colors == view->codoncolors) {//codon mode: write amino acid name
			int aanum;
			static char aa3[] = "ArgLeuSerThrProAlaGlyValLysAsnGlnHisGluAspTyrCysPheIleMetTrpTER???   ";
			char *codonstart = view->sequence[seq - 1] + ((site - 1)/3) * 3;
			if(codonstart+2 - view->sequence[seq - 1] < view->each_length[seq-1] && memchr(codonstart, '-', 3) == NULL) {//gap-less codon
				char aa = codaa(codonstart, 
							get_acnuc_gc_number(get_ncbi_gc_from_comment(view->comments[seq - 1])));
				aanum = strchr(aminoacids, aa) - aminoacids;
				}
			else aanum = 22;//write spaces for gap-with codon
			sprintf(cursor_coord + strlen(cursor_coord), " %.3s", aa3 + 3 * aanum);
			}
		sprintf(cursor_coord + strlen(cursor_coord), " Pos:%d|%d [%.150s]", site, c, view->seqname[seq - 1]);
		}
	else
		sprintf(cursor_coord, "Pos:%d", site);
	fl_color(view->namecolor); 
	if (max_curs_coord >= 0 && (unsigned)max_curs_coord < sizeof(cursor_coord)) cursor_coord[max_curs_coord] = 0;
	fl_draw(cursor_coord, x, y);
	}
}


void draw_seq_names(Fl_Widget *ob, SEA_VIEW *view)
{
int x, y, num, debut, fin;
int couleur;
static char trunc_name[20];

x = view->x_name; y = view->y_name;
fl_font(FL_COURIER, ob->labelsize() );
fl_push_clip(ob->x(), ob->y(), view->x_seq - ob->x(), ob->h() - 4);
debut = view->first_seq - 1; 
fin = view->first_seq - 2 + view->tot_lines;
if(fin >= view->tot_seqs) fin = view->tot_seqs - 1;
if(view->draw_names == -1) { /* ecrire tous les noms */
	fl_color(ob->color());
	fl_rectf( ob->x(), ob->y(),  /* fond pour les noms */
		view->x_seq - ob->x(),
		ob->h() - 4 /* 2 * BORDER_WIDTH */); 
/* write region name */
	if(view->active_region != NULL) {
		fl_color(view->region_color); 
		fl_font(ob->labelfont(), ob->labelsize());
		fl_draw(view->active_region->name, 
		   FL_min((unsigned)view->wid_names, strlen(view->active_region->name)), 
			view->x_name,
			view->y_name + FL_min(view->tot_lines, view->tot_seqs) *
			view->line_height );
		fl_font(FL_COURIER, ob->labelsize() );
		}
	}
else if(view->draw_names == -2) { /* ecrire tous les noms mais rien qu'eux */
	fl_color(ob->color());
	fl_rectf( ob->x(),   /* fond pour les noms */
		y - view->line_height + fl_descent(), 
		view->x_seq - ob->x(),
		(fin - debut + 1) * view->line_height);
	}
else	{ /* ecrire un seul nom */
	debut = fin = view->draw_names - 1;
  if (debut < view->first_seq - 1) {
    fl_pop_clip();
    return;
  }
	y += view->line_height * (view->draw_names - view->first_seq);
	if( !view->sel_seqs[view->draw_names - 1] ) {
		fl_color(ob->color());
		fl_rectf( x, y - view->line_height + fl_descent(), 
			(view->wid_names+1) * view->char_width, 
			view->line_height);
		}
	}
if(view->tot_seqs > 0) { /* nbre de seqs selectionnees */
	fl_color(ob->color());
	fl_rectf( x, view->y_name - 2 * view->line_height, 
		(view->wid_names+1) * view->char_width, view->line_height);
	sprintf(trunc_name, "sel=%d", view->tot_sel_seqs);
	fl_color(view->namecolor); 
	fl_draw(trunc_name, x, view->y_name - view->line_height );
	}
/* le(s) nom(s) a ecrire */
for(num = debut; num <= fin; num++) {
	if(view->sel_seqs[num]) {
		fl_color(FL_BLACK);
		fl_rectf( x, y - view->line_height + fl_descent(), 
			(view->wid_names+1) * view->char_width, 
			view->line_height);
		couleur = FL_WHITE;
		}
	else
		couleur = view->namecolor;
	if (num == view->cursor_seq - 1) { // set name of cursor-containing seq in bold
	  if (view->sel_seqs[num]) couleur = FL_DARK2;
	  fl_font(fl_font()|FL_BOLD, fl_size());
	  }
	fl_color(couleur);
    int lbytes = strlen(view->seqname[num]); // length in bytes
    int lc = fl_utf_nb_char((uchar*)view->seqname[num], lbytes); // length in UTF characters
    int draw_bytes; // # of bytes of the name string to draw
    if (lc == lbytes) draw_bytes = FL_min(view->wid_names, lbytes);
    else if (lc <= view->wid_names) draw_bytes = lbytes;
    else { // name contains non ASCII: count characters not bytes
        lc = 0;
        char *p = view->seqname[num], *last = view->seqname[num] + lbytes;
        while (p < last && lc < view->wid_names) {
            int len;
            fl_utf8decode(p, last, &len);
            lc++;
            p += len;
        }
        draw_bytes = p - view->seqname[num];
    }
	fl_draw(view->seqname[num], draw_bytes, x, y );
	if (num == view->cursor_seq - 1) {
	  fl_font(fl_font()&~FL_BOLD, fl_size());
	  }
	y += view->line_height;
	}
    fl_pop_clip();
}


void draw_header(Fl_Widget *ob, SEA_VIEW *view)
{
static char site_line[10];

if(view->mod_seq) return;
if(view->tot_seqs == 0) return;
if(view->active_region != NULL) draw_region_line(ob, view);
/* write site numbers */
if (view->tot_sites < 7) return;
fl_color(ob->color());
fl_rectf( view->x_seq, 
	view->y_seq - 2 * view->line_height + fl_descent(),
	view->tot_sites * view->char_width, view->line_height);
fl_color(view->namecolor); fl_font(FL_COURIER, ob->labelsize() );
  sprintf(site_line, "%d", view->first_site); 
  fl_draw(site_line, view->x_seq, view->y_seq - view->line_height);
  sprintf(site_line, "%7d", view->first_site + view->tot_sites - 1);
  fl_draw(site_line, view->x_seq + (view->tot_sites - 7)*view->char_width, view->y_seq - view->line_height);
}  


void draw_dna_seqs(Fl_Widget *ob, SEA_VIEW *view)
{
int nline, offset, x, y, i, l_line, y_back, need_back, isreference;
char *pos;
int debut, fin, use_region;
list_segments *segment, *first_segment;
  Fl_Menu_Item *menu_props = (Fl_Menu_Item *)view->menu_props;
props_menu_parts *props_parts = (props_menu_parts *)menu_props->user_data();
int byreference = (menu_props + props_parts->reference)->value();
static int first = TRUE;
static char **clines;//holds color-specific lines
if(first) {
	first = FALSE;
	clines = new char*[22];
	for(i=0; i < 22; i++) clines[i] = new char[1000];
	}

if(view->mod_seq == 0) { /* draw all lines */
	debut = view->first_seq - 1;
	fin = FL_min(view->first_seq + view->tot_lines - 1, view->tot_seqs) - 1;
	}
else if(view->mod_seq == -1) { /* draw only selected lines */
	debut = view->first_seq - 1;
	fin = FL_min(view->first_seq + view->tot_lines - 1, view->tot_seqs) - 1;
	while(fin >= debut && !view->sel_seqs[fin]) fin--;
	if(debut > fin) return;
	while(!view->sel_seqs[debut]) debut++;
	}
else	{ /* draw just line # view->mod_seq */
	debut = fin = view->mod_seq - 1;
	if(debut < view->first_seq - 1) fin = debut - 1;
	}
// prepare for drawing region background
use_region = (view->active_region != NULL && view->numb_gc > 1 &&
		(first_segment = view->active_region->list) != NULL);
if(use_region) {
	do	{
		if(first_segment->debut > 
			view->first_site + view->tot_sites - 1) {
				use_region = FALSE;
				break;
			}
		if(first_segment->fin >= view->first_site) break;
		first_segment = first_segment->next;
		}
	while(first_segment != NULL);
	if(first_segment == NULL) use_region = FALSE;
	}
need_back = TRUE;
if(use_region) {
	if(first_segment->debut <= view->first_site &&
		first_segment->fin >= view->first_site + view->tot_sites - 1)
		need_back = FALSE;
	}

/* write sequences */
fl_font( ob->labelfont(), ob->labelsize() );
x = view->x_seq;
y = view->y_seq + (debut - (view->first_seq - 1)) * view->line_height;
y_back = y - view->line_height + fl_descent();
offset = view->first_site - 1;
for(nline = debut; nline <= fin; nline++) {
	isreference = (byreference && (nline > 0));
	if( view->mod_seq != -1 || view->sel_seqs[nline] ) {
		if(need_back) { /* write seq background */
			fl_color(ob->color());
			fl_rectf( x, y_back, 
				view->tot_sites * view->char_width, 
				view->line_height);
			}
		if(use_region) { /* write regions background */
			int deb_block, fin_block;
			fl_color(ob->selection_color());
			segment = first_segment;
			do	{
				deb_block = FL_max(
					segment->debut, view->first_site);
				fin_block = FL_min(segment->fin, 
					view->first_site + view->tot_sites - 1);
				fl_rectf(x+(deb_block - view->first_site)*
						view->char_width, 
					y_back, 
					(fin_block - deb_block + 1) *
						view->char_width, 
					view->line_height);
				segment = segment->next;
				}
			while(segment != NULL && segment->debut <= 
				view->first_site + view->tot_sites - 1);
			}
		if(view->each_length[nline] > offset) {
			l_line = FL_min( view->tot_sites, view->each_length[nline] - offset );
			// write DNA seqs 
			if(view->numb_gc > 1) {
				char residue;
				pos = view->sequence[nline] + offset;
				char *pos0 = view->sequence[0] + offset;
				for(i=0; i < view->numb_gc; i++) {
					memset(clines[i], ' ', l_line);
					clines[i][l_line] = 0;
					}
				for( i = 0; i < l_line; i++) {//fill color-specific lines
					if(isreference && toupper(*pos) == toupper(*pos0)) residue = '.';
					else residue = *pos;
					if(!view->allow_lower) residue = toupper(residue);
					clines[ (unsigned)view->col_rank[nline][offset + i] ][i] = residue;
					pos++; pos0++;
					}
				for(i=0; i < view->numb_gc; i++) {//draw color-specific lines
					fl_color(view->curr_colors[i]);
					fl_draw(clines[i], l_line, x, y);
					}
				}
			else {
				pos = view->sequence[nline];
				fl_color(view->curr_colors[0]);
				fl_draw(pos + offset, l_line, x, y );
				}
			}
		}
	y += view->line_height;
	y_back += view->line_height;
	}
draw_header(ob, view);
}  


void draw_dna_seqs_inverted(Fl_Widget *ob, SEA_VIEW *view)
{
int nline, c, offset, x, y, l_line, yy, firstline, lastline, use_region,
	debut, fin, der_site;
  float xx;
int pos;
list_segments *segment, *first_segment;
int save_col0;
static char up_line[1000]; char *p, *q, *r; int i;
  Fl_Menu_Item *menu_props = (Fl_Menu_Item *)view->menu_props;
props_menu_parts *props_parts = (props_menu_parts *)menu_props->user_data();
int byreference = (menu_props + props_parts->reference)->value();

if(view->tot_seqs == 0) {
	return;
	}
fl_font(ob->labelfont(), ob->labelsize() );
save_col0 = view->curr_colors[0];
view->curr_colors[0] = ob->color();
/* write sequences */
x = view->x_seq;
offset= view->first_site - 1;
if(view->mod_seq <= 0) {
	firstline = view->first_seq-1;
	lastline = FL_min(view->first_seq + view->tot_lines -1, view->tot_seqs);
	}
else	{
	firstline = view->mod_seq - 1;
	lastline = firstline + 1;
	if(firstline < view->first_seq - 1) lastline = firstline - 1;
	}
	use_region = (view->active_region != NULL &&
		(first_segment = view->active_region->list) != NULL);
if(use_region) {
	do	{
		if(first_segment->fin >= view->first_site) break;
		first_segment = first_segment->next;
		}
	while(first_segment != NULL);
	}
y = view->y_seq + (firstline - view->first_seq + 1) * view->line_height; 
yy = y - view->line_height + fl_descent();
//ecriture du fond gris
fl_color(ob->color());
fl_rectf( x, yy, view->tot_sites * view->char_width, (lastline - firstline) * view->line_height);
//
for(nline=firstline; nline < lastline; /* ecriture des fonds de couleur */
		nline++, y += view->line_height, yy += view->line_height ) {
	l_line = ( offset + view->tot_sites < view->each_length[nline] ? 
		view->tot_sites : view->each_length[nline] - offset);
	l_line = FL_max(l_line, 0); /* nbre de caract a ecrire */
	if(l_line > 0) {
			xx = x; 
			for(pos = offset; pos < offset + l_line; pos++) {
				if( (c = (int)view->col_rank[nline][pos]) != 0 ) {
					fl_color(view->curr_colors[c]);
					fl_rectf( xx, yy, 
						view->char_width+1,
						view->line_height);
						}
				xx += view->char_width;
				}
			
		}
	if(l_line < view->tot_sites) { /* au dela de la fin de la seq */
		fl_color(ob->color());
	 	fl_rectf( x + l_line * view->char_width, yy,
			view->char_width * (view->tot_sites - l_line), 
			view->line_height);
		}
	if(l_line == 0) continue;
	
/*ecriture des seqs: regions en noir et autres en col2 */
	fl_color( use_region ? ob->selection_color() : FL_BLACK );
	p = view->sequence[nline] + offset; 
	memcpy(up_line, p, l_line); up_line[l_line] = 0;
	p = up_line;
	if(!view->allow_lower) majuscules(up_line);
	if(byreference && nline != 0) {
		q = p + l_line; i = 0;
		r= view->sequence[0] + offset; 
		while( p < q) {
			up_line[i++] = (toupper(*p) == toupper(*r) ? '.' : *p);
			p++; r++;
			}
		p = up_line;
		}
	fl_draw(p, l_line, x, y);
	if(use_region) { /* re-ecrire les regions en noir */
		der_site = view->first_site + l_line - 1;
		segment = first_segment;
		fl_color( FL_BLACK );
		while( segment  != NULL ) {
			if(segment->debut > der_site) break;
			debut = FL_max(segment->debut, view->first_site);
			fin = FL_min(segment->fin, der_site);
			p = up_line + debut - offset - 1;
			fl_draw(p,
				fin - debut + 1, 
				x + (debut - offset - 1)*view->char_width, y);
			segment = segment->next;
			}
		}
	}
view->curr_colors[0] = save_col0;
draw_header(ob, view);
}


void set_tot_lines(SEA_VIEW *view, int new_val)
{
int l;
double x;
view->tot_lines = new_val;
l = view->tot_seqs - view->tot_lines + 1;
if(l<1) l=1;
if(view->first_seq > l) {
	view->first_seq = l;
	}
if(view->tot_seqs > 0) {
	x = view->tot_lines / ( (double) view->tot_seqs ); 
	if(x>1) x=1;
	}
else	x = 1;
view->vertsli->slider_size(x);
view->vertsli->bounds(1, l);
view->vertsli->Fl_Slider::value(view->first_seq);
}


int compute_size_params(SEA_VIEW *view, int force_recompute)
{
static int old_w = 0;
static int old_h = 0;
static Fl_Widget *old_dna_obj = NULL;
Fl_Widget *ob = view->DNA_obj;
double x;
int l, tot_lines, possible_lines;

if( !force_recompute && old_dna_obj == ob && ob->w() == old_w && ob->h() == old_h) return FALSE;
old_w = ob->w(); old_h = ob->h(); old_dna_obj = ob;
view->x_name = view->char_width/2 + ob->x();
view->y_name =  2 * view->line_height + ob->y();
view->x_seq = (view->wid_names +2) * view->char_width + ob->x();
view->y_seq = view->y_name;
view->tot_sites = ( ob->w() - view->x_seq - ob->x() ) / view->char_width;
possible_lines = ( ob->h() - 4 ) / view->line_height  - 1;
tot_lines = possible_lines;
if(view->active_region != NULL) tot_lines--;
if(view->show_comment_lines) {
	tot_lines -= view->tot_comment_lines;
	view->pos_first_comment_line = 
		FL_min(tot_lines, view->tot_seqs) + 1;
	if(view->active_region != NULL) ++(view->pos_first_comment_line);
	}
/* init sliders bounds and size */
if(view->tot_seqs > 0) {
	x = ( (double) view->tot_sites ) / ( view->seq_length + 3 ); 
	if(x>1) x=1;
	}
else	x = 1;
view->horsli->slider_size(x);
l = view->seq_length - view->tot_sites+3;
if(l<1) l=1;
view->horsli->bounds(1,l);
if(view->first_site > l) {
	view->first_site = l;
	}
view->horsli->Fl_Slider::value(view->first_site);
set_tot_lines(view, tot_lines);
if(view->tot_seqs + 
	(view->show_comment_lines ? view->tot_comment_lines : 0) + 
	(view->active_region != NULL ? 1 : 0) < possible_lines) {
/* forcer effacement de tout l'alignement pour ne pas garder 
de mauvaises lignes regions ou footers en bas */
	ob->parent()->redraw();
	}
return TRUE;
}

int compute_wid_names(SEA_VIEW *view, int totseqs)
{
  int i, l, maxnamewidth, wid_names = 0;
  for(i=0; i<totseqs; i++) {
    l=strlen(view->seqname[i]);
    while(l > 0 && view->seqname[i][l-1] == ' ') l--; 
    view->seqname[i][l] = 0;
    if(l > wid_names) wid_names = l;
  }
  if(totseqs > 0 && wid_names < 10) wid_names = 10;
  maxnamewidth = int_res_value("maxnamewidth", 20);
  if(wid_names > maxnamewidth) wid_names = maxnamewidth;
  return wid_names;
}

int calc_max_seq_length(int seq_length, int tot_seqs)
{
  int max_seq_length = (int)(1.3 * seq_length);
  if(tot_seqs <= 5000) { 
    max_seq_length = FL_max(max_seq_length, seq_length + MAX_SEQ_ALLONG);
  }
  else {
    max_seq_length = FL_max(max_seq_length, seq_length + 500);
  }
  return max_seq_length;
}

void init_dna_scroller(SEA_VIEW *view, int totseqs, const char *masename, 
	int protein, char *header)
{
int i, l;
list_regions *mylregion;
  Fl_Menu_Item *menu_props = (Fl_Menu_Item *)view->menu_props;
props_menu_parts *props_parts = (props_menu_parts *)menu_props->user_data();
Fl_Menu_Item *item;

view->header = header;
view->tot_seqs = totseqs;
view->seq_length = 0;
view->protein = protein;
view->allow_seq_edit = FALSE;
view->viewasprots = NULL;
item = menu_props + props_parts->viewasprots;
item->clear();
if(protein) item->deactivate(); else item->activate();
item = menu_props + props_parts->edit;
item->activate(); item->clear();
item = menu_props + props_parts->keys;
item->clear(); (item + 1)->clear(); (item + 2)->clear(); 
item = menu_props + props_parts->keys - 1;
if(protein) item->deactivate();
else item->activate();
item = menu_props + props_parts->reference;
item->clear();
item->deactivate();
view->hjkl = FALSE;
set_seaview_modified(view, FALSE);
if(masename != NULL) {
	view->masename = (char *)malloc(strlen(masename)+1);
	if(view->masename == NULL) out_of_memory();
	strcpy(view->masename, masename);
	view->dnawin->label(extract_filename(view->masename));
#if defined(__APPLE__) && 100*FL_MAJOR_VERSION + FL_MINOR_VERSION < 104
	rename_windowmenuitem(view->dnawin->label(), find_windowmenuitem(view->dnawin));
#endif
	}
else	view->masename = NULL;
if(totseqs > 0) {
	view->each_length = (int *)malloc(totseqs * sizeof(int));
	if(view->each_length == NULL) out_of_memory();
	}
for(i = 0; i < totseqs; i++) {
	l = strlen(view->sequence[i]);
	view->each_length[i] = l;
	if(l > view->seq_length) view->seq_length = l;
	}
for(i = 0; i < view->tot_comment_lines; i++) {
	l = strlen(view->comment_line[i]);
	if(l > view->seq_length) view->seq_length = l;
	}
  view->max_seq_length = calc_max_seq_length(view->seq_length, view->tot_seqs);
view->wid_names = compute_wid_names(view, totseqs);
allonge_seqs(view->sequence, totseqs, view->max_seq_length, 
	view->each_length, view->tot_comment_lines, view->comment_line, NULL);
if(view->numb_dnacolors > 1 && view->menu_props != NULL) { 
	/* DNA/prot modes useful only with colors */
	item = menu_props + props_parts->colors;
	if(view->alt_colors != NO_ALT_COLORS) (item+2)->activate();
	if(protein) {
		if(view->alt_colors == USING_ALT_COLORS) {
			(item+2)->setonly();
			set_aa_color_mode(USING_ALT_COLORS);
			view->numb_gc = view->numb_altprotcolors;
			view->curr_colors = view->altprotcolors;
			}
		else	{
			(item+1)->setonly();
			set_aa_color_mode(USING_STANDARD_COLORS);
			view->numb_gc = view->numb_stdprotcolors;
			view->curr_colors = view->stdprotcolors;
			}
		}
	else	{
		item->setonly();
		view->numb_gc = view->numb_dnacolors;
		view->curr_colors = view->dnacolors;
		}
	}
if(view->numb_gc > 1) {
	view->col_rank = prepcolranks(view->sequence, totseqs, 
		view->max_seq_length, view->each_length, 
		( view->protein ? get_color_for_aa : get_color_for_base ), 
		view->numb_gc, view->allow_lower);
	if(view->col_rank == NULL) out_of_memory();
	view->alt_col_rank = NULL;
	}
view->first_seq = view->first_site = 1;
#ifdef WIN32
	Fl::flush();//necessary against bug with "Edit/Create seq" with FLTK 1.3
#endif
fl_font(view->DNA_obj->labelfont(), view->DNA_obj->labelsize());
view->line_height = (int)( LINE_HEIGHT_FACTOR * fl_height() );
view->char_width = fl_width("W");
view->draw_names = -1;
view->mod_cursor = FALSE;
view->mod_seq = 0;
view->multipl->argument(0);
if(view->tot_seqs == 0)
	view->cursor_seq = view->cursor_site = 0;
else
	view->cursor_seq = view->cursor_site = 1;
view->old_cursor_seq = view->cursor_seq;
view->old_cursor_site = view->cursor_site;
view->cursor_in_comment = view->old_cursor_in_comment = FALSE;
view->tot_sel_seqs = 0;
if(view->menu_edit != NULL) {
	Fl_Menu_Item *items = (Fl_Menu_Item *)view->menu_edit;
	items[RENAME_SEQ].deactivate();
	items[EDIT_COMMENTS].deactivate();
	items[EDIT_SEQ].deactivate();
	items[DUPLICATE_SEQ].deactivate();
	items[DELETE_SEQ].deactivate();
	items[COMPLEMENT_SEQ].deactivate();
	items[REVERSE_SEQ].deactivate();
	items[EXCHANGE_UT].deactivate();
	items[DOT_PLOT].deactivate();
	items[CONSENSUS_SEQ].deactivate();
	items[DELETE_GAP_ONLY_SITES].activate();
	items[SET_GCODE].deactivate();
	items[COPY_SEQS].deactivate();
	}
	if(view->menu_align != NULL) {
		Fl_Menu_Item *items = (Fl_Menu_Item *)view->menu_align;
		items[ALIGN_SELECTED_SEQS].deactivate();
		items[ALIGN_SITES].deactivate();
		items[PROFILE].deactivate();
		items[UNALIGN].deactivate();
		}
if(view->tot_seqs > 0) {
	view->sel_seqs = (int *)calloc(view->tot_seqs, sizeof(int));
	if(view->sel_seqs == NULL) out_of_memory();
	}
/* initialiser les regions et leur menu avec pas de region active */
view->mod_region_line = FALSE;
if(view->tot_seqs > 0) {
	view->region_line = (char *)malloc(view->max_seq_length + 1);
	if(view->region_line == NULL) out_of_memory();
	}
view->region_length = 0;
view->active_region = NULL;
if(view->menu_sites != NULL) {
	if(view->tot_seqs > 0) (view->menu_sites->get_menu())->activate();
	mylregion = view->regions;
  view->menu_sites->vclear();
	while(mylregion != NULL) {
	  view->menu_sites->add(mylregion->element->name, regions_menu_callback, NULL, FL_MENU_RADIO);
		mylregion = mylregion->next;
		}
	}
if(view->menu_species != NULL) {
  view->menu_species->vclear();
	for (i=0; i < view->numb_species_sets; i++) {
	  view->menu_species->add(view->name_species_sets[i], species_menu_callback, NULL, FL_MENU_RADIO);
	  }
	}
view->show_comment_lines = (view->tot_comment_lines > 0);
view->active_comment_line = 0;
update_menu_footers(view);
compute_size_params(view, TRUE);
view->DNA_obj->take_focus();
}


void mod_multipl(SEA_VIEW *view, int newval)
{
Fl_Widget *obj = view->multipl;
static char label[16];
int old = obj->argument();

if(newval == 0) sprintf(label, "%s-+_", view->movekeys);
else	sprintf(label, "mult=%d", newval);
obj->argument(newval);
obj->label(label);
if(newval != old) obj->redraw();
}


void rename_sequence(SEA_VIEW *view)
{
char *new_name;
const char *rep;
int num;
if(view->tot_sel_seqs != 1) return; /* only by security, should not happen */
num = 0;
while( ! view->sel_seqs[num] ) num++;
rep = fl_input("Rename:",view->seqname[num]);
if(rep == NULL) return;
new_name = (char *)malloc(strlen(rep)+1);
if(new_name == NULL) return;
free(view->seqname[num]);
strcpy(new_name, rep);
view->seqname[num] = new_name;
if(num+1 >= view->first_seq && num+1 <= view->first_seq+view->tot_lines-1) {
  view->wid_names = compute_wid_names(view, view->tot_seqs);
  compute_size_params(view, TRUE);
  view->DNA_obj->redraw();
  }
set_seaview_modified(view, TRUE);
}


void del_gap_only_sites(SEA_VIEW *view)
{
int position, numseq, allgaps, inrun, debut, count;

view->seq_length = 0;
for(numseq = 0; numseq < view->tot_seqs; numseq++) 
	if(view->each_length[numseq] > view->seq_length)
		view->seq_length = view->each_length[numseq];
inrun = FALSE;
position = -1;
while( ++position <= view->seq_length) {
	for(numseq = 0; numseq < view->tot_seqs; numseq++) 
		if(position < view->each_length[numseq] &&
			view->sequence[numseq][position] != '-') break;
	allgaps = (numseq >= view->tot_seqs);
	if(position >= view->seq_length) allgaps = FALSE;
	if(inrun == allgaps) continue;
	if(allgaps && !inrun) {
		inrun = TRUE;
		debut = position;
		}
	else	{
		inrun = FALSE;
		count = position - debut;
		for(numseq = 1; numseq <= view->tot_seqs; numseq++)
			delete_gaps_before(view, numseq, debut + count + 1, 
				count);
		view->seq_length -= count;
		if(view->cursor_site > position) view->cursor_site -= count;
		if(view->regions != NULL) delete_region_part(view, 
			debut + count + 1, count);
		if(view->tot_comment_lines > 0)	delete_in_all_comments(count, 
			debut + count + 1, view);
		position -= count;
		}
	}
view->seq_length = 0;
for(numseq = 0; numseq < view->tot_seqs; numseq++) 
	if(view->each_length[numseq] > view->seq_length)
		view->seq_length = view->each_length[numseq];	
}


int delete_selected_seqs(SEA_VIEW *view)
{
void **temp_data;
char **viewasprots = (char **)view->viewasprots;
int new_tot_seqs, i, j, *temp_int, numset;
new_tot_seqs = view->tot_seqs - view->tot_sel_seqs;
if(new_tot_seqs > 0) {
	temp_data = (void **)malloc(new_tot_seqs * sizeof(void *));
	if(temp_data == NULL) return TRUE;
	temp_int = (int *)malloc(new_tot_seqs * sizeof(int));
	if(temp_int == NULL) return TRUE;
	}

for(i = 0, j = 0; i<  view->tot_seqs; i++) /* process sequences */
	if( ! view->sel_seqs[i] ) temp_data[j++] = (void *) view->sequence[i];
	else free(view->sequence[i]);
memcpy(view->sequence , temp_data, new_tot_seqs * sizeof(char *) );

if(viewasprots != NULL) {
	for(i = 0, j = 0; i<  view->tot_seqs; i++) /* process sequences */
		if( ! view->sel_seqs[i] ) temp_data[j++] = (void *) viewasprots[i];
		else free(viewasprots[i]);
	memcpy(viewasprots, temp_data, new_tot_seqs * sizeof(char *) );
	}

if(view->comments != NULL) {
	for(i = 0, j = 0; i<  view->tot_seqs; i++) /* process comments */
		if( ! view->sel_seqs[i] ) temp_data[j++] = 
						(void *) view->comments[i];
		else if( view->comments[i]!= NULL) free(view->comments[i]);
	memcpy(view->comments , temp_data, new_tot_seqs * sizeof(char *) );
	}

for(i = 0, j = 0; i<  view->tot_seqs; i++) /* process seq names */
	if( ! view->sel_seqs[i] ) temp_data[j++] = (void *) view->seqname[i];
	else free(view->seqname[i]);
memcpy(view->seqname , temp_data, new_tot_seqs * sizeof(char *) );

for(i = 0, j = 0; i<  view->tot_seqs; i++) /* process seq lengths */
	if( ! view->sel_seqs[i] ) temp_int[j++]= view->each_length[i];
memcpy(view->each_length , temp_int, new_tot_seqs * sizeof(int) );

if(view->numb_gc > 1) { /* process color-coded sequences */
	for(i = 0, j = 0; i<  view->tot_seqs; i++)
		if( ! view->sel_seqs[i] ) temp_data[j++] = (void *) view->col_rank[i];
		else free(view->col_rank[i]);
		memcpy(view->col_rank , temp_data, new_tot_seqs * sizeof(char *) );
   	}
for(numset = 0; numset < view->numb_species_sets; numset++) {
/* process species sets */
	for(i = 0, j = 0; i < view->tot_seqs; i++) { 
		if( ! view->sel_seqs[i] ) 
			temp_int[j++]= view->list_species_sets[numset][i];
		}
	memcpy(view->list_species_sets[numset], temp_int, 
		new_tot_seqs * sizeof(int) );
	}

if (! view->cursor_in_comment) view->cursor_seq = view->old_cursor_seq = 1;
memset(view->sel_seqs, 0, view->tot_seqs * sizeof(int));
view->tot_seqs = new_tot_seqs;
select_deselect_seq(view, 0);
if(view->menu_species != NULL) {
	numset = 0;
	while(numset < view->numb_species_sets) { /* check for empty sets */
		for(i = 0, j = 0; i < view->tot_seqs; i++) 
			if( view->list_species_sets[numset][i] ) j++;
		if( j == 0 ) { /* set became empty => remove it */
			free(view->list_species_sets[numset]);
			free(view->name_species_sets[numset]);
			memcpy(view->list_species_sets + numset,
			view->list_species_sets + numset + 1,
			(view->numb_species_sets - numset - 1)*sizeof(int *) );
			memcpy(view->name_species_sets + numset,
			view->name_species_sets + numset + 1,
			(view->numb_species_sets - numset - 1)*sizeof(char *) );
		  view->menu_species->remove(numset);
		  view->menu_species->value(0);
			(view->numb_species_sets)--;
			}
		else	numset++;
		}
	}
set_seaview_modified(view, TRUE);
return FALSE;
}


void vh_sliders_callback(Fl_Widget *ob, void *data)
{
int old, new_val, page;
int which = ((user_data_plus *)data)->value;
SEA_VIEW *view = (SEA_VIEW *)((user_data_plus *)data)->p;
Fl_Scrollbar *sli = (Fl_Scrollbar *)ob;

new_val = sli->value();
if (which == 0) { /* vertical */
      	old = view->first_seq;
	if( Fl::event_is_click() ) {
		page = view->tot_lines - 2;
		if(page < 1) page = 1;
		if(new_val > old + sli->linesize() ) new_val = old + page;
		else if(new_val < old - sli->linesize() ) new_val = old - page;
		new_val = (int) sli->clamp(new_val);
		((Fl_Valuator *)sli)->value(new_val);
		}
      	view->first_seq = new_val;
	if(old != new_val) view->DNA_obj->redraw();
	}
else	{ /* horizontal */
      	old = view->first_site;
	if( Fl::event_is_click() ) {
		page = view->tot_sites - 20;
		if(page < 10) page = 10;
		if(new_val > old + sli->linesize() ) new_val = old + page;
		else if(new_val < old - sli->linesize() ) new_val = old - page;
		new_val = (int)sli->clamp(new_val);
		((Fl_Valuator *)sli)->value(new_val);
		}
      	view->first_site = new_val;
	view->draw_names = 0;
	if(old != new_val) view->DNA_obj->damage(1);
	}
view->DNA_obj->take_focus();
}


void lrdu_button_callback(Fl_Widget *ob, void *data)
{
Fl_Slider *sli;
int *pval, newval=0;
int mini, maxi;

int which = ((user_data_plus *)data)->value;
SEA_VIEW *view = (SEA_VIEW *)((user_data_plus *)data)->p;
if(which <= 3) { /* mouvement horizontal */
	sli = view->horsli;
	pval = &view->first_site;
	}
else	{ /* mouvement vertical */
	sli = view->vertsli;
	pval = &view->first_seq;
	}
mini = (int)(sli->minimum());
maxi = (int)(sli->maximum());

if(which == 1 || which == 5) newval = mini;
else if(which == 3 || which == 7) newval = maxi;

if(newval < mini) newval = mini;
if(newval > maxi) newval = maxi;
if(newval != *pval) {
	*pval = newval;
	sli->value(*pval);
	if(which <= 3)  view->draw_names = 0;
	view->DNA_obj->damage(view->draw_names ? FL_DAMAGE_ALL : 1);
	}
view->DNA_obj->take_focus();
}


void font_callback(Fl_Widget *ob, void *data)
{
int taille;
  SEA_VIEW *view = (SEA_VIEW*)ob->user_data();
sscanf( ((Fl_Menu_ *)ob)->text(), "%d", &taille);
if(view->DNA_obj->labelsize() == taille ) return;
my_watch_cursor(view->dnawin);
view->DNA_obj->labelsize(taille);
fl_font( view->DNA_obj->labelfont(), view->DNA_obj->labelsize() );
view->line_height = (int)( LINE_HEIGHT_FACTOR * fl_height() );
view->char_width = fl_width("W");
compute_size_params(view, TRUE);
view->DNA_obj->parent()->redraw();
fl_reset_cursor(view->dnawin);
view->DNA_obj->take_focus();
}


void set_save_format(SEA_VIEW *view, int val)
{
if(val >= nbr_formats) return;
if( view->format_for_save == (known_format)val) return;
view->format_for_save = (known_format)val;
  Fl_Menu_Item *items = (Fl_Menu_Item*)view->menu_file;
if(view->masename != NULL) {
	 items[SAVE].activate();
	}
}


void colors_callback(Fl_Widget *ob, void *data)
{
  SEA_VIEW *view = (SEA_VIEW*)ob->user_data();
  Fl_Menu_Item *menu_props = (Fl_Menu_Item *)view->menu_props;
  props_menu_parts *parts = (props_menu_parts *)view->menu_props->user_data();
int reponse =  ((Fl_Menu_*)ob)->mvalue() - menu_props;
Fl_Menu_Item *item;

/* processing inverted colors */
if(reponse == parts->inverted) {
	int style;
	my_watch_cursor(view->dnawin);
	view->inverted_colors = !view->inverted_colors;
	if(view->inverted_colors) {
		style = FL_COURIER;
		}
	else	{
		style = FL_COURIER_BOLD;
		}
	view->DNA_obj->labelfont(style);
	view->DNA_obj->redraw();
	fl_reset_cursor(view->dnawin);
	return;
	}

/* changing DNA / protein / Alternate protein / Codon mode */
enum {DNARNA, Protein, Alt, Codon };
int debut = parts->colors;
reponse -= debut;
if(reponse == Protein && view->protein && 
	( view->alt_colors == USING_STANDARD_COLORS ||
	view->alt_colors == NO_ALT_COLORS) ) return;
if(reponse == Alt && view->alt_colors == USING_ALT_COLORS) return;
view->protein = (reponse == Protein || reponse == Alt);
my_watch_cursor(view->dnawin);
if(view->numb_gc > 1) { /* free col_rank */
	int num;
	for(num = 0; num < view->tot_seqs; num++) {
		free(view->col_rank[num]);
		}
	free(view->col_rank);
	}
if(reponse == DNARNA) {
	view->numb_gc = view->numb_dnacolors;
	view->curr_colors = view->dnacolors;
	}
else if(reponse == Protein) {
	set_aa_color_mode(USING_STANDARD_COLORS);
	view->numb_gc = view->numb_stdprotcolors;
	view->curr_colors = view->stdprotcolors;
	}
else if(reponse == Alt) {
	set_aa_color_mode(USING_ALT_COLORS);
	view->numb_gc = view->numb_altprotcolors;
	view->curr_colors = view->altprotcolors;
	}
else if(reponse == Codon) {
	view->numb_gc = 22;
	view->curr_colors = view->codoncolors;
	}
item = menu_props + parts->edit;
adjust_menu_edit_modes(view);
if(reponse != Codon) {
	view->col_rank = prepcolranks(view->sequence, view->tot_seqs, 
								  view->max_seq_length, view->each_length, 
								  (view->protein ? get_color_for_aa : get_color_for_base),
								  view->numb_gc, view->allow_lower);
	}
else view->col_rank = prepcolranks_by_codon(view->sequence, view->tot_seqs, view->max_seq_length, 
										   view->each_length, view->comments);

if(view->col_rank == NULL) out_of_memory();
  if (reponse == Protein)  view->alt_colors = NO_ALT_COLORS;
  if (reponse == Alt)  view->alt_colors = USING_ALT_COLORS;
view->DNA_obj->redraw();
view->DNA_obj->take_focus();
fl_reset_cursor(view->dnawin);
}


void allow_edit_callback(Fl_Widget *ob, void *data)
{
  SEA_VIEW *view = (SEA_VIEW*)ob->user_data();
Fl_Menu_Item *item = (Fl_Menu_Item*)((Fl_Menu_*)ob)->mvalue();

view->allow_seq_edit = !view->allow_seq_edit;
if(view->allow_seq_edit) 
	item->set();
else
	item->clear();
view->DNA_obj->take_focus();
}


void viewasprots_callback(Fl_Widget *ob, void *data)
{
  SEA_VIEW *view = (SEA_VIEW*)ob->user_data();
  props_menu_parts *parts = (props_menu_parts *)view->menu_props->user_data();
int oldedit, num;
Fl_Menu_Item *menu_props = (Fl_Menu_Item *)view->menu_props;
Fl_Menu_Item *item = (Fl_Menu_Item*)((Fl_Menu_*)ob)->mvalue();
Fl_Menu_Item *item2 = menu_props + parts->edit;
Fl_Menu_Item *fileitems = (Fl_Menu_Item *)view->menu_file;
  Fl_Menu_Item *edititems = (Fl_Menu_Item *)view->menu_edit;

if(view->tot_seqs == 0) return;
oldedit = item2->value();
if(view->viewasprots != NULL) {
	view->viewasprots = set_viewasprots(view, FALSE);
	if(parts->usingcodoncolors) {
		for(num = 0; num < view->tot_seqs; num++) free(view->col_rank[num]);
		free(view->col_rank);
		Fl_Menu_Item *codoncolor = menu_props + parts->colors + 3;
		codoncolor->setonly();
		view->numb_gc = 22;
		view->curr_colors = view->codoncolors;
		view->col_rank = prepcolranks_by_codon(view->sequence, view->tot_seqs, view->max_seq_length, 
											   view->each_length, view->comments);
		}
	}
else {
	parts->usingcodoncolors = (view->curr_colors == view->codoncolors);
	view->viewasprots = set_viewasprots(view, TRUE);
	}
if(oldedit) item2->set();
if(view->viewasprots != NULL) {
	item->set();
	item->activate();
	item2->deactivate();
	view->allow_seq_edit = FALSE;
	fileitems[ACNUC_IMPORT].deactivate();
	fileitems[SAVE_PROT_ALIGN].activate();
	fileitems[SAVE].deactivate();
	fileitems[SAVE_AS].deactivate();
	fileitems[SAVE_REGIONS].deactivate();
	edititems[PASTE_SEQS].deactivate();
	edititems[CREATE_SEQ].deactivate();
	edititems[LOAD_SEQ].deactivate();
	edititems[EDIT_SEQ].deactivate();
	edititems[DUPLICATE_SEQ].deactivate();
	edititems[COMPLEMENT_SEQ].deactivate();
	edititems[REVERSE_SEQ].deactivate();
	edititems[CONSENSUS_SEQ].deactivate();
	}
else	{
	item->clear();
	item2->activate();
	view->allow_seq_edit = item2->value();
	fileitems[ACNUC_IMPORT].activate();
	fileitems[SAVE_PROT_ALIGN].deactivate();
	fileitems[SAVE].activate();
	fileitems[SAVE_AS].activate();
	if (view->active_region) fileitems[SAVE_REGIONS].activate();
	edititems[PASTE_SEQS].activate();
        edititems[CREATE_SEQ].activate();
	edititems[LOAD_SEQ].activate();
	}
view->DNA_obj->take_focus();
}


void consensus_allowgaps_callback(Fl_Widget *ob, void *data)
{
SEA_VIEW *view = (SEA_VIEW*)ob->user_data();
int *poption = &(view->consensus_allowgaps);
*poption = !(*poption);
}

void consensus_iupac_callback(Fl_Widget *ob, void *data)
{
  SEA_VIEW *view = (SEA_VIEW*)ob->user_data();
  int *poption = &(view->consensus_allowiupac);
  *poption = !(*poption);
}


void slow_fast_callback(Fl_Widget *ob, void *data)
{
  SEA_VIEW *view = (SEA_VIEW*)ob->user_data();
  Fl_Menu_Item *item = (Fl_Menu_Item*)((Fl_Menu_*)ob)->mvalue();
view->double_buffer = !view->double_buffer;
if(!view->double_buffer) 
	item->set();
else
	item->clear();
Fl_Window *win = (Fl_Window *)view->DNA_obj->parent();
Fl_Group *win_parent = (Fl_Group *)win->parent();
int x,y,w,h;
x = win->x(); y = win->y(); w = win->w(); h = win->h();
int size = view->DNA_obj->labelsize();
int font = view->DNA_obj->labelfont();
Fl_Color col = view->DNA_obj->color();
Fl_Color sel_col = view->DNA_obj->selection_color();
win->hide();
win_parent->remove(*win);
delete win;
Fl_Group::current(win_parent);
if(view->double_buffer)	
	win = new Fl_Double_Window(x, y, w, h );
else 	win = new Fl_Window(x,y,w,h);
win->box(FL_DOWN_BOX);
win->resizable(win);
view->DNA_obj = (Fl_Widget*)new DNA_obj(3, 3, w - 6, h - 6, view);
view->DNA_obj->labelfont(font);
view->DNA_obj->labelsize(size);
view->DNA_obj->color(col, sel_col);
win->color(col);
win->end();
win->show();
view->DNA_obj->take_focus();
}


void allow_lower_callback(Fl_Widget *ob, void *data)
{
SEA_VIEW *view = (SEA_VIEW*)ob->user_data();
Fl_Menu_Item *item = (Fl_Menu_Item *)((Fl_Menu_*)ob)->mvalue();

view->allow_lower = !view->allow_lower;
if(view->allow_lower) 
	item->set();
else
	item->clear();
view->DNA_obj->parent()->redraw();
view->DNA_obj->take_focus();
}


void toggle_reference_callback(Fl_Widget *ob, void *data)
{
int on;
  SEA_VIEW *view = (SEA_VIEW*)ob->user_data();
  props_menu_parts *parts = (props_menu_parts *)view->menu_props->user_data();
Fl_Menu_Item *item = (Fl_Menu_Item *)view->menu_props + parts->edit;
on = (view->menu_props + parts->reference)->value();
reference_toggle(view, on);
if(on) 	{
	item->deactivate();
	}
else	{
	item->activate();
	if(view->allow_seq_edit) item->set();
	else item->clear();
	}
}


void dnakeys_callback(Fl_Widget *ob, void *data)
{
  SEA_VIEW *view = (SEA_VIEW*)ob->user_data();
  props_menu_parts *parts = (props_menu_parts *)view->menu_props->user_data();
int reponse =  (((Fl_Menu_*)ob)->mvalue() - view->menu_props) - parts->keys;
if( ! view->hjkl ) { /* activer un choix */
	view->hjkl = reponse + 1;
	}
else if(view->hjkl == reponse + 1) { /* desactiver choix courant */
	((Fl_Menu_Item*)((Fl_Menu_*)ob)->mvalue())->clear();
	view->hjkl = 0;
	}
else	{ /* changer de choix */
	view->hjkl = reponse + 1;
	}
}


void consensus_opt_callback(Fl_Widget *ob, void *data)
{
props_menu_parts *parts = (props_menu_parts *)data;
  SEA_VIEW *view = (SEA_VIEW*)ob->user_data();
Fl_Menu_Item *menu_props = (Fl_Menu_Item *)view->menu_props;
const char *rep;
char tmp[100];
int value;
sprintf(tmp, "%d %%", view->consensus_threshold);
rep = fl_input("Set consensus threshold value in %%", tmp );
if(rep == NULL) return;
value = -1;
sscanf(rep, "%d", &value);
if(value <= 0 || value > 100) return;
view->consensus_threshold = value;
sprintf(tmp, "%d %%", view->consensus_threshold);
  (menu_props + parts->consensusopt)->label(strdup(tmp));
}


void create_props_menu(SEA_VIEW *view, 
	int my_color_choice, int inverted, int black_and_white, 
	int def_fontsize)
{
#define NBER_FONTSIZES 10
  int defaut;
  static int first = TRUE;
  static props_menu_parts *parts;
  
  if (first) {
    first = FALSE;
    parts = (props_menu_parts *)malloc(sizeof(props_menu_parts));
    
    parts->colors = 1 + NBER_FONTSIZES + 3;
    parts->viewasprots = parts->colors - 2;
    parts->inverted = parts->colors + 4;
    parts->reference = parts->inverted + 3;
    parts->edit = parts->reference - 1;
    parts->keys = parts->reference + 2;
    parts->consensusopt = parts->keys + 5;
    parts->slow_fast = parts->consensusopt + 5;
    parts->allow_lower = parts->slow_fast + 1;
    }
  
  static const Fl_Menu_Item propsitems_static[] = {
    {"Fontsize", 0, 0, 0, FL_SUBMENU},
    {"8", 0, font_callback, 0, FL_MENU_RADIO},
    {"10", 0, font_callback, 0, FL_MENU_RADIO},
    {"12", 0, font_callback, 0, FL_MENU_RADIO},
    {"14", 0, font_callback, 0, FL_MENU_RADIO},
    {"16", 0, font_callback, 0, FL_MENU_RADIO},
    {"18", 0, font_callback, 0, FL_MENU_RADIO},
    {"24", 0, font_callback, 0, FL_MENU_RADIO},
    {"28", 0, font_callback, 0, FL_MENU_RADIO},
    {"32", 0, font_callback, 0, FL_MENU_RADIO},
    {"36", 0, font_callback, 0, FL_MENU_RADIO},
    {0},
    {"View as proteins", 0, viewasprots_callback, 0, FL_MENU_TOGGLE | FL_MENU_INACTIVE},
    {"Colors", 0, 0, 0, FL_SUBMENU},
    {"DNA-RNA colors", 0, colors_callback, 0, FL_MENU_RADIO},
    {"Protein colors", 0, colors_callback, 0, FL_MENU_RADIO},
    {"Alt. colors", 0, colors_callback, 0, FL_MENU_RADIO},
    {"Codon colors", 0, colors_callback, 0, FL_MENU_RADIO | FL_MENU_DIVIDER},
    {"Inverted colors", 0, colors_callback, 0, FL_MENU_TOGGLE},
    {0},
    {"Allow seq. editing", 0, allow_edit_callback, 0, FL_MENU_TOGGLE},
    {"by Reference", FL_COMMAND | 'r', toggle_reference_callback, 0, FL_MENU_TOGGLE},
    {"DNA keys", 0, 0, 0, FL_SUBMENU},
    {"hjkl<space> => GATCN", 0, dnakeys_callback, 0, FL_MENU_RADIO},
    {"hjkl<space> => TCGAN", 0, dnakeys_callback, 0, FL_MENU_RADIO},
    {"hjkl<space> => ACGTN", 0, dnakeys_callback, 0, FL_MENU_RADIO},
    {0},
    {"Consensus options", 0, 0, 0, FL_SUBMENU},
    {"%", 0, NULL, 0, FL_MENU_DIVIDER},
    {"allow gaps", 0, consensus_allowgaps_callback, 0, FL_MENU_TOGGLE},
    {"use IUPAC symbols", 0, consensus_iupac_callback, 0, FL_MENU_TOGGLE},
    {"Edit threshold", 0, consensus_opt_callback, parts, 0},
    {0},
    {"Fast-Rough", 0, slow_fast_callback, 0, FL_MENU_TOGGLE},
    {"Allow lowercase", 0, allow_lower_callback, 0, FL_MENU_TOGGLE},
    {"Customize", 0, custom_callback, NULL, 0},
    {"Statistics", 0, stats_callback, NULL, 0},
    {0}
  };

  Fl_Menu_Item *propsitems = new Fl_Menu_Item[sizeof(propsitems_static)/sizeof(Fl_Menu_Item)];
  memcpy(propsitems, propsitems_static, sizeof(propsitems_static));
  view->menu_props = propsitems;
  view->menubar->add("Props", 0, NULL, (void*)view->menu_props, FL_SUBMENU_POINTER);
  propsitems->user_data(parts);

/* partie Fontsize */
for(defaut = 1; defaut <= NBER_FONTSIZES; defaut++) {
	int taille;
	sscanf( propsitems[defaut].label(), "%d", &taille);
	if(taille >= def_fontsize) break;
	}
if(defaut > NBER_FONTSIZES) defaut = 4;
(propsitems + defaut)->set();

/* partie View as proteins */

/* partie Colors */
if(black_and_white) {
	(propsitems + parts->colors - 1)->deactivate();
	}
else	{
	defaut = parts->colors; if(view->protein) defaut++;
	(propsitems + defaut)->set();
	if(my_color_choice == NO_ALT_COLORS) 
		(propsitems + parts->colors + 2)->deactivate();
	if(inverted) (propsitems + parts->inverted)->set();
	}

/* partie Edit */

/* partie by reference */

/* partie keys */

/* partie consensus options */
char tmp[50];
sprintf(tmp, "%d %%", view->consensus_threshold);
  (propsitems + parts->consensusopt)->label(strdup(tmp));

/* partie Fast/Slow */
  (propsitems + parts->slow_fast)->flags = 
#ifdef __APPLE__
	FL_MENU_INVISIBLE;
#else
	(view->double_buffer ? FL_MENU_TOGGLE : FL_MENU_TOGGLE | FL_MENU_VALUE);
#endif

/* partie allow lower case */
  if (view->allow_lower) (propsitems + parts->allow_lower)->set();
  if (black_and_white) (propsitems + parts->allow_lower)->deactivate();

/* partie customize */
/* partie statistiques */
}


/*int clipboard_contains_alignment(const char *clip)
{
extern char *fl_selection_buffer[];
if(clip == NULL) clip = fl_selection_buffer[0];
return (clip != NULL && memcmp(clip, CLIPBOARDMARK, strlen(CLIPBOARDMARK)) == 0);
}

void fix_all_paste_items(const char *p)
{
Fl_Window *w;
SEA_VIEW *view;
int mode;

if(clipboard_contains_alignment(p) ) mode = 0;
else mode = FL_MENU_INACTIVE;
#ifdef WIN32
  Fl::check(); // calls Fl::do_widget_deletion(), crashes without this when closing window
#endif
w = Fl::first_window();
while(w != NULL) {
	if(w->callback() == mainwin_close_callback) {
		view = (SEA_VIEW *)w->user_data();
		if (view) {
		  Fl_Menu_Item* items = (Fl_Menu_Item*)view->menu_edit;
		  if (view->viewasprots || mode == FL_MENU_INACTIVE) items[PASTE_SEQS].deactivate();
		  else items[PASTE_SEQS].activate();
		  }
		}
	w = Fl::next_window(w);
	}
}

void fix_paste_timeout(void *u)
{
fix_all_paste_items(NULL);
Fl::repeat_timeout(0.5, fix_paste_timeout);
}*/


void edit_menu_callback(Fl_Widget *ob, void* data)
{
SEA_VIEW *view = (SEA_VIEW *)ob->user_data();
  int reponse = ((Fl_Menu_*)ob)->mvalue() - view->menu_edit;

if(reponse == COPY_SEQS) { /* copy selected sequences */
	region *myregion;
	myregion = view->active_region;
	if(myregion == NULL) {
		myregion = (region *)malloc(sizeof(region));
		if(myregion == NULL) return;
		myregion->list = (list_segments *)malloc(sizeof(list_segments));
		if(myregion->list == NULL) return;
		myregion->list->debut = 1;
		myregion->list->fin = view->seq_length;
		myregion->list->next = NULL;
		myregion->name = (char *)"all";
		}
	char *err = save_alignment_or_region(clipboardfname, view->sequence, view->comments,
		NULL, view->seqname, view->tot_seqs, view->each_length,
		NULL, myregion, MASE_FORMAT,
		0, NULL, NULL, view->sel_seqs, view->tot_sel_seqs, 
		view->protein, 0, NULL, NULL, view->phylipwidnames, 
		0, NULL, NULL, FALSE);
	if(view->active_region == NULL) { free(myregion->list); free(myregion); }
	if(err == NULL) {
	  //put filename on the selection clipboard (0)
		char tmp[200];
		sprintf(tmp, CLIPBOARDMARK"%s", clipboardfname);
		Fl::copy(tmp, strlen(tmp), 0);
	  //put Fasta sequences on the primary clipboard (1)
		char *buf, *p;
		int i, l = 0;
		for( i = 0; i < view->tot_seqs; i++) {
		  if(!view->sel_seqs[i]) continue;
		  l += strlen(view->seqname[i]) + view->each_length[i] + 3;
		  }
		buf = (char *)malloc(l + 10);
		if(buf != NULL) {
			p = buf;
			for( i = 0; i < view->tot_seqs; i++) {
			  if(!view->sel_seqs[i]) continue;
			  sprintf(p, ">%s\n", view->seqname[i]);
			  p += strlen(p);
			  strcpy(p, view->sequence[i]);
			  p += strlen(p);
			  *(p++) = '\n';
			  }
			Fl::copy(buf, p - buf, 1);
			free(buf);
		  }
		}
	else {
		FILE *tmpf = fopen(clipboardfname, "w");
		if (tmpf) fclose(tmpf);
		Fl::copy("", 0, 0);
		Fl::copy("", 0, 1);
		}
	}
else if(reponse == PASTE_SEQS) {
#ifdef WIN32
  pasted_from_what_source = 1;
#else
  pasted_from_what_source = 0; /* paste first from the selection buffer */
#endif
  Fl::paste(*(view->DNA_obj), pasted_from_what_source);
	}
else if(reponse == SELECT_ALL) { /* select all sequences */
  if(view->multipl->argument() > 0) mod_multipl(view,0);
  select_deselect_seq(view, -1);
  view->draw_names = -2;
  // ceci signifie redraw partiel commande' par draw_names, mod_cursor, ...
  view->DNA_obj->damage(1);
}
else if(reponse == RENAME_SEQ) { /* rename the selected sequence */
	rename_sequence(view);
	}
else if(reponse == EDIT_COMMENTS) {
	edit_comments_dialog(view);
	}
else if(reponse == EDIT_SEQ) {
	edit_sequence_dialog(view);
	}
else if(reponse == DELETE_SEQ) { /* delete selected sequences from alignment */
	if( fl_choice("Confirm request of deletion of %d sequence(s)", "Cancel", "Delete", NULL, view->tot_sel_seqs) ) { 
		if( delete_selected_seqs(view) )
			fl_alert("Not enough memory for this operation");
		else	{
			compute_size_params(view, TRUE);
			update_menu_footers(view);
			view->DNA_obj->redraw();
			view->vertsli->redraw();
			}
		}
	}
else if(reponse == CREATE_SEQ) { /* create a new sequence */
	char *newname;
	newname = (char *) fl_input("Name of the new sequence?", "");
	if(newname == NULL || strlen(newname) == 0) return;
	add_seq_to_align(view, newname, (char *)"-", 1);
	}
else if(reponse == LOAD_SEQ) { /* load a new sequence */
	load_seq_dialog(view);
	}
else if(reponse == DUPLICATE_SEQ || reponse == COMPLEMENT_SEQ ||
		 reponse == REVERSE_SEQ) { 
	int num, lenseq, old_first_seq;
	char *newseq, *p, *q, *newname;
	if(view->tot_sel_seqs != 1 ) return; /* par securite */
	if(reponse != DUPLICATE_SEQ && view->protein) return; /* par securite */
	for(num = 0; num < view->tot_seqs; num++)
		if(view->sel_seqs[num]) break;
	lenseq = view->each_length[num];
	newseq = (char *)malloc(lenseq + 1);
	if(newseq == NULL) {
		fl_alert("Not enough memory\nto create a new sequence");
	  Fl_Menu_Item *items = (Fl_Menu_Item *)view->menu_edit;
		items[reponse].deactivate();
		return;
		}
	if(reponse == DUPLICATE_SEQ) {
		strcpy(newseq, view->sequence[num]);
		}
	else	{
		p = view->sequence[num]; q = newseq + lenseq - 1;
		while( *p != 0) {
			if(reponse == COMPLEMENT_SEQ) *q = complement_base(*p);
			else *q = *p;
			p++; q--;
			}
 		newseq[lenseq] = 0;
		}
	newname = (char*)malloc(strlen(view->seqname[num]) + 3);
	if(reponse == COMPLEMENT_SEQ) strcpy(newname,"C_");
	else if(reponse == REVERSE_SEQ) strcpy(newname,"R_");
	else	strcpy(newname,"D_");
	strcat(newname,view->seqname[num]);
	old_first_seq = view->first_seq;
	add_seq_to_align(view, newname , newseq, lenseq);
	free(newseq);
	free(newname);
/* placer la nouvelle seq apres celle de depart */
	view->sel_seqs[num] = FALSE;
	view->sel_seqs[view->tot_seqs - 1] = TRUE;
	deplacer_grp_seqs( view, FL_min(num + 2, view->tot_seqs) );
/* montrer les 2 seqs concernees */
	if(old_first_seq > num + 1) view->first_seq = 
		FL_min(num + 1, view->tot_seqs - view->tot_lines + 1);
	else if(old_first_seq + view->tot_lines - 1 < num + 2) 
		view->first_seq = FL_min(num + 2, 
			view->tot_seqs - view->tot_lines + 1);
	else
		view->first_seq = old_first_seq;
	view->vertsli->Fl_Slider::value(view->first_seq);
	}
else if(reponse == EXCHANGE_UT) { /* exchange Us and Ts */
	int num; char *p;
	if(view->tot_sel_seqs == 0 || view->protein) return; /* par securite */
	my_watch_cursor(view->dnawin);
	for(num = 0; num < view->tot_seqs; num++) {
		if( ! view->sel_seqs[num] ) continue;
		p = view->sequence[num] - 1;
		while( *(++p) != 0 ) {
			if( *p == 'U' ) {
			   	*p = 'T'; 
			   	}
			else if( *p == 'T' ) {
			   	*p = 'U'; 
			   	}
			else if( *p == 't' ) {
			   	*p = 'u';
				}
			else if( *p == 'u' ) {
			   	*p = 't';
			   	}
			}
		}
	set_seaview_modified(view, TRUE);
	view->draw_names = 0;
	view->DNA_obj->damage(1);
	fl_reset_cursor(view->dnawin);
	}
else if(reponse == DOT_PLOT) { /* dot plot */
	int num1, num2;
	extern void show_dot_plot(char *seq1, char *seq2, char *seqname1, 
		char *seqname2, int l1, int l2, int maxseqlength, 
		void *seaview_data);

	if(view->tot_sel_seqs != 2) return;
	for(num1 = 0; num1 < view->tot_seqs; num1++)
		if(view->sel_seqs[num1]) break;
	for(num2 = num1 + 1; num2 < view->tot_seqs; num2++)
		if(view->sel_seqs[num2]) break;
	show_dot_plot(view->sequence[num1], view->sequence[num2],
		view->seqname[num1], view->seqname[num2],
		view->each_length[num1], view->each_length[num2],
		view->max_seq_length, (void *)view);
	}
else if (reponse == CONSENSUS_SEQ) {
	char *newseq, newname[100];
	int old_total, *tmp, new_pos, i, old_first_seq;

	if(view->tot_sel_seqs <= 1 ) return; /* par securite */
	newseq = cre_consensus(view, newname);
	if(newseq == NULL) {
		fl_alert("Not enough memory\nto create a new sequence");
		Fl_Menu_Item *items = (Fl_Menu_Item *)view->menu_edit;
		items[reponse].deactivate();
		return;
		}
	old_first_seq = view->first_seq;
	old_total = view->tot_seqs;
	add_seq_to_align(view, newname , newseq, strlen(newseq));
	free(newseq);	
	for(i=0; i < view->tot_seqs; i++)
		if(view->sel_seqs[i]) new_pos = i;
	new_pos += 2;
	if(view->tot_seqs == old_total || new_pos == view->tot_seqs) {
		return; 
		}
/* placer la nouvelle seq apres la derniere des selectionnees */
	tmp = (int *)calloc(view->tot_seqs, sizeof(int));
	if(tmp == NULL) {
		return; 
		}
	memcpy(tmp, view->sel_seqs, view->tot_seqs * sizeof(int) );
	memset(view->sel_seqs, 0, view->tot_seqs * sizeof(int) );
	view->sel_seqs[view->tot_seqs - 1] = TRUE;
	old_total = view->tot_sel_seqs;
	view->tot_sel_seqs = 1;
	deplacer_grp_seqs( view, new_pos );
	memcpy(view->sel_seqs, tmp, view->tot_seqs * sizeof(int) );
	view->tot_sel_seqs = old_total;
	free(tmp);
/* montrer la seq concernee */
	if(old_first_seq > new_pos ) view->first_seq = 
		FL_min(new_pos - 2,  1);
	else if(old_first_seq + view->tot_lines - 1 < new_pos) 
		view->first_seq = FL_min(new_pos - 2, 
			view->tot_seqs - view->tot_lines + 1);
	else
		view->first_seq = old_first_seq;
	view->vertsli->Fl_Slider::value(view->first_seq);
	}
else if(reponse == DELETE_GAP_ONLY_SITES) {
	if( !fl_choice("Confirm remove all gap_containing sites?", "Cancel", "Remove", NULL) )  
		return;
	my_watch_cursor(view->dnawin);
	del_gap_only_sites(view);
	compute_size_params(view, TRUE);
	view->DNA_obj->redraw();
	view->horsli->redraw();
	fl_reset_cursor(view->dnawin);
	}
else if(reponse == SET_GCODE) {
	if( (!view->protein) && view->tot_sel_seqs != 0 ) set_ncbi_genetic_code(view);
	}
}


void set_and_show_new_cursor_site(SEA_VIEW *view, int new_pos, int center,
	int force_redraw)
{
int old_pos;
old_pos = view->cursor_site;
if(new_pos != old_pos)
	view->cursor_site = new_pos;
if(new_pos >= view->first_site && new_pos < view->first_site +
	view->tot_sites - 1) {
	if( !force_redraw && 
		( (view->cursor_in_comment && view->mod_comment_line == 0) ||
		(!view->cursor_in_comment && view->mod_seq == 0) ) )
		view->mod_cursor = TRUE; 
	}
else	{
	if(center) 
		view->first_site = 
			view->cursor_site - view->tot_sites/2;
	else	{
		if(new_pos >= old_pos) 
			view->first_site = view->cursor_site + 10 - 
				view->tot_sites;
		else
			view->first_site = view->cursor_site - 10;
		}
	if(view->first_site + view->tot_sites - 1 >
		view->seq_length + 1 )
		view->first_site = view->seq_length - view->tot_sites + 2;
	if(view->first_site <=0 ) 
		view->first_site = 1;
	view->horsli->Fl_Slider::value(view->first_site);
	view->mod_seq = 0;
	view->mod_comment_line = 0;
	}
view->draw_names = 0;
view->DNA_obj->damage(1);
}


void set_and_show_new_cursor_seq(SEA_VIEW *view, int new_pos)
{
if(view->cursor_in_comment) {
	if(view->comment_length[new_pos - 1] + 1 < view->cursor_site) {
		fl_beep(FL_BEEP_DEFAULT); 
		return;
		}
	if(new_pos == view->cursor_seq) return;
	view->cursor_seq = new_pos;
	view->mod_cursor = TRUE;
	view->draw_names = 0;
	view->DNA_obj->damage(1);
	return;
	}
if(view->each_length[new_pos - 1] + 1 < view->cursor_site) {
	fl_beep(FL_BEEP_DEFAULT); return;
	}
if(new_pos != view->cursor_seq || new_pos < view->first_seq ||
	view->cursor_site != view->old_cursor_site ||
	new_pos >= view->first_seq + view->tot_lines) {
	view->cursor_seq = new_pos;
	if(new_pos >= view->first_seq && new_pos < view->first_seq +
		view->tot_lines) {
		view->mod_cursor = TRUE;
		view->draw_names = 0;
		view->DNA_obj->damage(1);
		}
	else	{
		view->first_seq = view->cursor_seq - view->tot_lines/2;
		if(view->first_seq + view->tot_lines >= 
			view->tot_seqs )
			view->first_seq = view->tot_seqs - view->tot_lines + 1;
		if(view->first_seq <=0 ) 
			view->first_seq = 1;
		view->vertsli->Fl_Slider::value(view->first_seq);
		view->draw_names = -1;
		view->DNA_obj->redraw();
		}
	}
}


void goto_callback(Fl_Widget *ob, void *data)
{
Fl_Input *input;
SEA_VIEW *view;
int maxi, num, new_pos = -1, numerique = TRUE;
  unsigned l;
char *p, *q, target[50];
static char *upname;
static int l_upname = 0;

user_data_plus *data_plus = (user_data_plus *)data;
int which = data_plus->value;
if(which == 0) { /* appele par bouton goto */
	input = (Fl_Input *)data_plus->p;
	data_plus = (user_data_plus *)input->user_data();
	}
else	input = (Fl_Input*)ob;
view = (SEA_VIEW *)data_plus->p;
if(view->tot_seqs == 0) return;
p = (char *)input->value();
q = p - 1; while(*(++q) != 0) {
	if(!isdigit(*q)) numerique = FALSE;
	}
if(numerique) { /* aller a une position de l'alignement */
	sscanf(p,"%d",&new_pos);
	if(view->cursor_in_comment)
		maxi = view->comment_length[view->cursor_seq - 1];
	else
		maxi = view->each_length[view->cursor_seq - 1];
	if( new_pos <= 0 || new_pos > maxi ) {
		input->value("");
		fl_beep(FL_BEEP_DEFAULT);
		}
	else	{
		set_and_show_new_cursor_site(view, new_pos, TRUE, FALSE);
		}
	}
else	{ /* recherche d'une seq par son nom */
	l = strlen(p);
	if (l > sizeof(target) - 1) l = sizeof(target) - 1;
	strncpy(target, p, l); target[l] = 0; majuscules(target);
	if(view->wid_names > l_upname) {
		if(l_upname > 0) free(upname);
		upname = (char *)malloc(view->wid_names + 1);
		l_upname = view->wid_names;
		}
	for(num = 0; num < view->tot_seqs; num++) {
		strncpy(upname, view->seqname[num], l_upname);
		upname[l_upname] = 0; majuscules(upname);
		if(strstr(upname, target) != NULL) break;
		}
	if(num >= view->tot_seqs) {
		fl_beep(FL_BEEP_DEFAULT);
		return;
		}
	if(view->cursor_site < view->first_site || 
		view->cursor_site >= view->first_site + view->tot_sites)
		view->cursor_site = view->first_site;
	set_and_show_new_cursor_seq(view, num + 1);
	}
view->DNA_obj->take_focus();
}


char *search_with_gaps(char *target, char *debut)
{
char *cherche, *trouve = debut - 1;
do	{
	debut = trouve + 1;
	trouve = NULL;
	cherche = target;
	do	{
		while( *debut == '-' ) debut++;
		if(trouve == NULL) trouve = debut;
		if ( toupper(*debut) != *cherche ) break;
		cherche++; debut++;
		}
	while( *cherche != 0 );
	}
while( *trouve != 0  && *cherche != 0);
return ( *cherche == 0 ? trouve : (char*)NULL );
}


void search_callback(Fl_Widget *ob, void *data)
{
  SEA_VIEW *view;
  char *target, *pos, *debut;
  int new_pos;

  user_data_plus *data_plus = (user_data_plus *)data;
  int which = data_plus->value;
  if(which == 1) { /* appele par champ input */
	  pos = (char *)((Fl_Input*)ob)->value();
	  }
  else	{ /* appele par bouton search */
	  Fl_Input *champ = (Fl_Input *)data_plus->p;
	  pos = (char *)champ->value();
	  data_plus = (user_data_plus *) champ->user_data();
	  }
  view = (SEA_VIEW *)data_plus->p;
  if(view->tot_seqs == 0) return;
  target = strdup(pos);
  while((pos = strchr(target, '\n')) != NULL) *pos = ' ';
  while((pos = strchr(target, '\r')) != NULL) *pos = ' ';
  compact(target);
  if( strlen(target) == 0 ) {free(target); return; }
  majuscules(target);
  if(view->cursor_in_comment)
	  debut = view->comment_line[view->cursor_seq - 1] + view->cursor_site;
  else
	  debut = view->sequence[view->cursor_seq - 1] + view->cursor_site;
  pos = search_with_gaps(target, debut);
  free(target);
  if(pos == NULL) fl_beep(FL_BEEP_DEFAULT);
  else	{
	  if(view->cursor_in_comment)
		  new_pos = pos - view->comment_line[view->cursor_seq - 1] + 1;
	  else
		  new_pos = pos - view->sequence[view->cursor_seq - 1] + 1;
	  set_and_show_new_cursor_site(view, new_pos, TRUE, FALSE);
	  }
  view->DNA_obj->take_focus();
}


void free_alignment(SEA_VIEW *view)
{
int num;
if(view->header!=NULL) { free(view->header); view->header = NULL; }
if(view->alt_col_rank != NULL) free_colranks_by_difference(view->alt_col_rank, view->tot_seqs);
for(num = 0; num < view->tot_seqs; num++) {
	free(view->sequence[num]);
	free(view->seqname[num]);
	if(view->comments != NULL && view->comments[num] != NULL) {
		free(view->comments[num]);
		view->comments[num] = NULL;
		}
	if(view->numb_gc > 1) {
		free(view->col_rank[num]);
		}
	}
if( view->tot_seqs > 0 ) {
	free(view->sequence);
	free(view->seqname);
	if(view->comments != NULL) free(view->comments);
	}
if( view->numb_gc > 1 && view->tot_seqs > 0 ) free(view->col_rank);
if(view->masename != NULL) {
	free(view->masename);
	view->masename = NULL;
	}
if( view->tot_seqs >= 1 ) {
	free(view->each_length);
	free(view->sel_seqs);
	free(view->region_line);
	}
if(view->viewasprots != NULL) {
	char **seqs = (char **)view->viewasprots;
	for(num = 0; num < view->tot_seqs; num++) {
		free(seqs[num]);
		}
	free(seqs);
	}
view->tot_seqs = 0;
while (view->regions != NULL) delete_region(view, 1); 
for(num = 0; num < view->numb_species_sets; num++) {
	free(view->list_species_sets[num]);
	free(view->name_species_sets[num]);
	}
view->numb_species_sets = 0;
view->tot_sel_seqs = 0;
view->cursor_seq = 0;
if(view->active_region != NULL) {
	free_region(view->active_region);
	view->active_region = NULL;
	}
if(view->menu_file != NULL) {
	Fl_Menu_Item *items = (Fl_Menu_Item *)view->menu_file;
	items[SAVE].deactivate();
	items[SAVE_AS].deactivate();
	items[SAVE_REGIONS].deactivate();
  items[SAVE_BOOTSTRAPS].deactivate();
	}
if(view->tot_comment_lines > 0) {
	for(num = 0; num < view->tot_comment_lines; num++) {
		free(view->comment_name[num]);
		free(view->comment_line[num]);
		}
	free(view->comment_name);
	free(view->comment_line);
	free(view->comment_length);
	view->tot_comment_lines = 0;
	view->show_comment_lines = FALSE;
	}
for(num = 0; num < view->tot_trees; num++) free(view->trees[num]);
if(view->tot_trees > 0) free(view->trees);
view->tot_trees = 0;
}


char *run_and_close_native_file_chooser(Fl_Native_File_Chooser *chooser, int keepalive)
//returns chosen file in static memory or NULL if user cancelled
{
	static char filename[PATH_MAX];
	char *retval = NULL;
#ifndef MICRO
	static char last_visited_directory[PATH_MAX] = "";
	char *p;
	if ( chooser->directory() == NULL && 
	  (chooser->preset_file() == NULL || *chooser->preset_file() != '/') ) {
	  if( *last_visited_directory )chdir(last_visited_directory);
	  chooser->directory(last_visited_directory);
	  p = (char *)chooser->preset_file();
	  if(p != NULL && *p != 0) {
		  p = strdup(extract_filename(p));
		  if(p != NULL) {
			  chooser->preset_file(p);
			  free(p);
			  }
		  }
	}
#endif
	if ( chooser->show() == 0 ) {
		strcpy(filename, chooser->filename());
#ifndef MICRO
		strcpy(last_visited_directory, filename);
		p = strrchr(last_visited_directory, '/');
		if(p != NULL) *p = 0;
#endif
		retval = filename;
		}
	if(!keepalive) delete chooser;
	return retval;
}


Fl_Window *load_alignment_file(SEA_VIEW *view, char *filename, const char *message, 
	known_format file_format, int doing_dnd)
/* returns the window containing the new alignment or NULL
 */
{
char *err_message;
int protein;

if(filename == NULL) {
	Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
	chooser->title(message);
	chooser->type(Fl_Native_File_Chooser::BROWSE_FILE);   // let user browse a single file
	if ((int)file_format != -1) {
		char wpattern[100];
		sprintf(wpattern, "%s \t*.%s", f_format_names[file_format], f_format_exts[file_format]); 
		chooser->filter(wpattern);                 
		}
	filename = run_and_close_native_file_chooser(chooser);
	if(filename == NULL) return NULL;
	}
if ((int)file_format == -1) {
	file_format = what_format(filename);
	if(file_format < 0) {
		fl_alert("File %s\nis not of a format readable by seaview", filename);
		return NULL;
		}
	}
if(view != NULL && view->alt_col_rank != NULL) {
	reference_toggle(view, FALSE);
	}
// search for empty alignment window; if none, create a new one
Fl_Window *w = Fl::first_window();
while(w != NULL) {
	if(w->callback() == mainwin_close_callback) {
		view = (SEA_VIEW *)w->user_data();
		if(view && view->tot_seqs == 0) break;
		}
	w = Fl::next_window(w);
	}
if(w == NULL) {
	view = newwindow_callback(view);
	}
#ifdef WIN32
	Fl::check();//to redraw window after file dialog box closes (FLTK bug fixed in FLTK 1.3.4)
#endif
my_watch_cursor(view->dnawin);
if(file_format == MASE_FORMAT) {
	view->tot_seqs = read_mase_seqs_header(filename, &view->sequence,
		&view->seqname, &view->comments, &view->header,
		&err_message);
/* interpreter les regions du header du fichier mase */
	view->regions = parse_regions_from_header(view->header);
/* interpreter les species sets du fichier mase */
	view->numb_species_sets = parse_species_sets_from_header(view->header,
	view->list_species_sets, view->name_species_sets, view->tot_seqs);
/* interpreter les trees du fichier mase */
	parse_trees_from_header(view->header, view);
/* interpreter les comment lines du header */
view->tot_comment_lines = parse_comment_lines_from_header(view->header,
	&(view->comment_line), &(view->comment_name), 
	&(view->comment_length) , &(view->max_seq_length));
	}
else if(file_format == FASTA_FORMAT)
	view->tot_seqs = read_fasta_align(filename, &view->sequence,
		&view->seqname, &view->comments, &view->header, &err_message, view->spaces_in_fasta_names);
else if(file_format == PHYLIP_FORMAT)
	view->tot_seqs = read_phylip_align(filename, &view->sequence,
		&view->seqname, &view->comments, &view->header, &err_message);
else if(file_format == CLUSTAL_FORMAT)
	view->tot_seqs = read_clustal_align(filename, &view->sequence,
		&view->seqname, &view->comments, &view->header, &err_message);
else if(file_format == MSF_FORMAT)
	view->tot_seqs = read_msf_align(filename, &view->sequence,
		&view->seqname, &view->comments, &view->header, &err_message);
else if(file_format == NEXUS_FORMAT) {
	int **list_sp = NULL; char **name_sp = NULL; int i;
	view->tot_seqs = read_nexus_align(filename, &view->sequence,
		&view->seqname, &view->comments, &view->header,
		&err_message, &view->regions, &view->numb_species_sets,
		&list_sp, &name_sp, &view->tot_comment_lines, 
		&view->comment_name, &view->comment_line, 
		&view->comment_length, &protein, view);
	for(i= 0; i < view->numb_species_sets; i++) {
		view->list_species_sets[i] = list_sp[i];
		view->name_species_sets[i] = name_sp[i];
		}
	if(list_sp != NULL) free(list_sp); 
	if(name_sp != NULL) free(name_sp);
	}
if(view->tot_seqs == 0 && view->tot_trees == 0) {
	fl_reset_cursor(view->dnawin);
	fl_alert("Error while reading file %s\n%s", filename,
		err_message);
	view->DNA_obj->parent()->redraw();
	return NULL;
	}
if(file_format != NEXUS_FORMAT) protein = is_a_protein_alignment(view);
if(view->menu_file != NULL) {
	Fl_Menu_Item *items = (Fl_Menu_Item *)view->menu_file;
	if(file_format == view->format_for_save) {
		items[SAVE].activate();
		items[SAVE_AS].activate();
		}
	else	{
		items[SAVE_AS].activate();
		if(view->masename != NULL) {
			free(view->masename);
			view->masename = filename = NULL;
			}
		}
		items[SAVE_BOOTSTRAPS].activate();
	}
int keep_dnawin = TRUE;
if(view->tot_seqs > 0) {
  extern char *position_mask_names[];
  extern int position_mask_count;
  list_regions *mylist = view->regions;
  while(mylist) { // hide each region named as in position_mask_names array
    for(int l = 0; l < position_mask_count; l++) {
      if(strcmp(mylist->element->name, position_mask_names[l]) == 0) {
	mylist->element->hide_when_viewasprots = TRUE;
	}
      }
    mylist = mylist->next;
    }
  init_dna_scroller(view, view->tot_seqs, filename, protein, view->header);
	set_save_format(view, file_format);
	view->DNA_obj->parent()->redraw();
	w = view->dnawin;
	}
else 	{//there was only a tree in the file read
	const char *p = view->menu_trees->vitem(0)->label();
	keep_dnawin = doing_dnd || view->tot_trees > 1;
	w = treedraw(strdup(view->trees[0]), view, p, keep_dnawin);
	if(keep_dnawin) w = view->dnawin;
	else view->dnawin->do_callback();//closes an empty alignment window that opened the tree
	}
if( keep_dnawin) fl_reset_cursor(view->dnawin);
return w;
}


void mainwin_close_callback(Fl_Widget *form, void *data)
{
	SEA_VIEW *view = (SEA_VIEW *)data;
	Fl_Window *w , *w2;
	if(view->modif_but_not_saved) {
		if( fl_choice("Alignment %s was modified but not saved\n"
			"Do you want to close it anyway?", "Cancel", "Close", NULL, extract_filename(view->masename)) == 0) return;
		}
	free_alignment(view);
#if defined(__APPLE__)
	char value[50];
	sprintf(value, "%dx%d", view->dnawin->w(),  view->dnawin->h() );
# if 100*FL_MAJOR_VERSION + FL_MINOR_VERSION < 104
    delete_windowmenuitem(find_windowmenuitem(view->dnawin));
# endif
#endif
	//disconnect all tree windows pointing to the deleted seaview window
	disconnect_tree_windows(view);
	//when a callback destroys its own widget delete is bad; Fl::delete_widget is to be called 
	Fl::delete_widget(form);
	//search for last alignment window and not the one scheduled for deletion
	w = Fl::first_window();
	w2 = NULL;
	while(w != NULL) {
		if(w != form && w->callback() == mainwin_close_callback) w2 = w;
		w = Fl::next_window(w);
		}
	if(w2 != NULL) {
		// set focus to last open alignment window
		((SEA_VIEW *)w2->user_data())->DNA_obj->take_focus();
		}
	else {
		// this was the last alignment window 
#ifdef __APPLE__
		set_res_value("window", value);
#endif
		}
	view->DNA_obj->user_data(NULL);
  delete[] view->menu_file;
  delete[] view->menu_edit;
  delete[] view->menu_align;
  delete[] view->menu_props;
  delete view->menu_sites;
  delete view->menu_species;
  delete[] view->menu_footers;
  delete view->menu_trees;
	free(view);
	form->user_data(NULL);
}

void to_do_at_exit(void)
{
  delete_tmp_filename(clipboardfname);
}

void close_all_saved_wins(Fl_Widget *ob)
{
//this rather complicated stuff seems necessary for WIN32
	Fl_Window *w, *mine;
	int count = 0, i = 0;
	mine = ob->window();
	//count and memorize all top-level windows except that containing the calling menu
	w = Fl::first_window();
	while( w != NULL) {
		if(w->window() == NULL && w != mine) count++;
		w = Fl::next_window(w);
	}
	Fl_Window **tabwins = (Fl_Window **)malloc(count * sizeof(Fl_Window *));
	w = Fl::first_window();
	while( w != NULL) {
		if(w->window() == NULL && w != mine) tabwins[i++] = w;
		w = Fl::next_window(w);
	}
	//attempt closing all these windows
	for(i = 0; i < count; i++) {
		Fl::handle(FL_CLOSE, tabwins[i]);
		Fl::wait(0);//this does close operation for good if needed
	}
	free(tabwins);
	//finally attempt closing the calling menu-containing window
	Fl::handle(FL_CLOSE, mine);
	return;
}


SEA_VIEW *newwindow_callback(SEA_VIEW *old_view)
{
SEA_VIEW *view = create_the_form( old_view != NULL ? old_view->double_buffer : TRUE );
return view;
}


void file_menu_callback(Fl_Widget *ob, void *data)
{
SEA_VIEW *view;
char pattern[20];
static char fsel_message[] = "seaview: choose file and format";
char *filename;
  view = (SEA_VIEW *)ob->user_data();
int reponse = ((Fl_Menu_*)ob)->mvalue() - view->menu_file;

sprintf(pattern, "*.%s", f_format_exts[view->format_for_save]);
if(reponse == CLOSE_WINDOW) { 
	ob->window()->do_callback();
	}
else if(reponse == QUIT) { 
	close_all_saved_wins(ob);
	}
else if(reponse == NEW_WINDOW) { 
	newwindow_callback(view);
}
else if(reponse == CONCATENATE) { 
	concatenate_dialog(view);
}
else if(reponse == OPEN_ANY) {
#ifdef WIN32
  Fl::e_state = 0; // fix bug where CTRL remains ON if Ctrl-O shortcut was used
#endif
  Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
  chooser->title("Choose an alignment/tree file");
  chooser->type(Fl_Native_File_Chooser::BROWSE_FILE);   // let user browse a single file
  filename = run_and_close_native_file_chooser(chooser);
  if(filename == NULL) return;
  use_initial_file(view, filename, FALSE);
  }
else if(reponse == OPEN_MASE) { 
	load_alignment_file(view, NULL, "Choose a .mase file",  MASE_FORMAT, FALSE);
	}
else if(reponse == OPEN_PHYLIP )	{  
	load_alignment_file(view, NULL, "Choose a Phylip file" ,  PHYLIP_FORMAT, FALSE);
	}
else if(reponse == OPEN_CLUSTAL )	{  
	load_alignment_file(view, NULL, "Choose a Clustal file" ,  CLUSTAL_FORMAT, FALSE);
	}
else if(reponse == OPEN_MSF )	{  
	load_alignment_file(view, NULL, "Choose an MSF file",  MSF_FORMAT, FALSE);
	}
else if(reponse == OPEN_FASTA )	{  
	load_alignment_file(view, NULL, "Choose a Fasta file",  FASTA_FORMAT, FALSE);
	}
else if(reponse == OPEN_NEXUS )	{  
	load_alignment_file(view, NULL, "Choose a NEXUS file",  NEXUS_FORMAT, FALSE);
	}
else if(reponse == ACNUC_IMPORT ) {
	racnuc_dialog(view);
	}
else if(reponse == SAVE || reponse == SAVE_AS) 	{ 
	char *err;
	if(reponse == SAVE_AS) { /* Save as */
		known_format new_format;
		filename = seaview_file_chooser_save_as(fsel_message, view->masename, view, &new_format);
		if(filename==NULL) return;
		set_save_format(view, new_format);
		}
	else {
		filename = view->masename;
		}
	my_watch_cursor(view->dnawin);
	save_active_region(view);
	err = save_alignment_or_region(filename, view->sequence, view->comments,
		view->header, view->seqname, view->tot_seqs, view->each_length,
		view->regions, NULL, view->format_for_save,
		view->numb_species_sets, view->list_species_sets,
		view->name_species_sets, NULL, 0, view->protein,
		view->tot_comment_lines, view->comment_name, 
		view->comment_line, view->phylipwidnames,
		view->tot_trees, view->trees, 
		view->menu_trees->vitem(0), view->spaces_in_fasta_names);
	fl_reset_cursor(view->dnawin);
	if(err != NULL) fl_alert("%s", err);
	else 	{
		if(reponse == SAVE_AS) {
			if(view->masename != NULL) free(view->masename);
			view->masename=(char *)malloc(strlen(filename)+1);
			if(view->masename == NULL) out_of_memory();
			strcpy(view->masename,filename);
			view->dnawin->label(extract_filename(filename));
#if defined(__APPLE__) && 100*FL_MAJOR_VERSION + FL_MINOR_VERSION < 104
			rename_windowmenuitem(view->dnawin->label(), find_windowmenuitem(view->dnawin));
#endif
		  Fl_Menu_Item *items = (Fl_Menu_Item *)view->menu_file;
			items[SAVE].activate();
			}
		set_seaview_modified(view, FALSE);
		}
	}
else if(reponse == SAVE_REGIONS) {  /* save current regions choice */
	char *err;
	region *myregion;
	known_format region_format;
	static char regions_only_filename[200];
	static int first = TRUE;
	if(first) {
	 	strcpy(regions_only_filename, "regions");
		if( strchr(pattern,'.') != NULL)
			strcat(regions_only_filename, 
				pattern+1);
		first = FALSE;
		}
	filename = seaview_file_chooser_save_as(fsel_message, regions_only_filename, view, &region_format);
	if(filename==NULL) return;
	my_watch_cursor(view->dnawin);
	myregion = view->active_region;
	if(myregion == NULL) {
		myregion = (region *)malloc(sizeof(region));
		if(myregion == NULL) return;
		myregion->list = (list_segments *)malloc(sizeof(list_segments));
		if(myregion->list == NULL) return;
		myregion->list->debut = 1;
		myregion->list->fin = view->seq_length;
		myregion->list->next = NULL;
		myregion->name = (char *)"all";
		}
	err = save_alignment_or_region(filename, view->sequence, view->comments,
		view->header, view->seqname, view->tot_seqs, view->each_length,
		NULL, myregion, region_format,
		0, NULL, NULL, view->sel_seqs, view->tot_sel_seqs, 
		view->protein, 0, NULL, NULL, view->phylipwidnames,
		view->tot_trees, view->trees, 
		view->menu_trees->vitem(0), view->spaces_in_fasta_names);
	if(view->active_region == NULL) { free(myregion->list); free(myregion); }
	fl_reset_cursor(view->dnawin);
	if(err != NULL) fl_alert("%s", err);
	else strcpy(regions_only_filename, extract_filename(filename));
	}
else if(reponse == SAVE_PROT_ALIGN) {  /* save alignmt at protein level */
	char *err;
	known_format new_format;
	filename = seaview_file_chooser_save_as(fsel_message, view->masename, view, &new_format);
	if(filename==NULL) return;
	my_watch_cursor(view->dnawin);
	err = save_alignment_or_region(filename, view->sequence, view->comments,
								   view->header, view->seqname, view->tot_seqs, view->each_length,
								   view->regions, NULL, new_format,
								   view->numb_species_sets, view->list_species_sets,
								   view->name_species_sets, NULL, 0, TRUE,
								   0, NULL, NULL, view->phylipwidnames,
								   view->tot_trees, view->trees, 
								   view->menu_trees->vitem(0),
								   view->spaces_in_fasta_names);
	fl_reset_cursor(view->dnawin);
	if(err != NULL) fl_alert("%s", err);
	}
else if(reponse == SAVE_BOOTSTRAPS && view->tot_seqs > 0) {  /* save bootstrap replicates */
  char suggested[300], *p;
  int replicates = -1;
  strcpy(suggested, view->masename);
  p = strrchr(suggested, '.');
  if (p != NULL) *p = 0;
  strcat(suggested, "_bootstraps.phy");
  Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
  chooser->type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  chooser->options(Fl_Native_File_Chooser::SAVEAS_CONFIRM | chooser->options());
  chooser->title("Enter output bootstrap Phylip filename");
  chooser->directory(extract_dirname(suggested));
  chooser->preset_file(extract_filename(suggested));
  filename = run_and_close_native_file_chooser(chooser);
  if (filename == NULL) return;
  const char *rep = fl_input("Desired number of bootstrap replicates:", "100");
  if (!rep) return;
  sscanf(rep, "%d", &replicates);
  if (replicates == -1) return;
  my_watch_cursor(view->dnawin);
  save_bootstrap_replicates(filename, replicates, view);
  fl_reset_cursor(view->dnawin);
  }
else if(reponse == PRINTOUT && view->tot_seqs > 0) {
	int anerr;
	char suggested[200], *p;
	
	strcpy(suggested, view->masename);
	p = strrchr(suggested, '.');
	if(p != NULL) *p = 0;
	if(printout_black == TEXT_ONLY) strcat(suggested, ".txt");
	else {
		strcat(suggested, "."PDF_OR_PS_EXT);
		}
if( view->alt_col_rank != NULL ) {
		for(anerr = 0; anerr < view->tot_seqs; anerr++)
			if(view->sel_seqs[anerr]) break;
		}
	else	anerr = -1;
#if defined(__APPLE__)
	filename = mac_GetOutputFName_Plus(extract_filename(suggested), "Enter an output file name", 
									   (anerr >= 0 ? TRUE : FALSE), extract_dirname(view->masename) );
#else
	Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
	chooser->type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  chooser->options(Fl_Native_File_Chooser::SAVEAS_CONFIRM | chooser->options());
	chooser->title("Enter an output file name");   
	chooser->directory(extract_dirname(suggested));
	chooser->preset_file(extract_filename(suggested));
	if(printout_black == TEXT_ONLY)
		chooser->filter("Text Files\t*.txt");
	else {
#ifdef NO_PDF
		chooser->filter("PostScript Files\t*.ps");
#else
		chooser->filter("PDF Files\t*.pdf");
#endif
	}
	filename = run_and_close_native_file_chooser(chooser);
#endif
	if(filename == NULL) return;
	my_watch_cursor(view->dnawin);
  anerr = printout(view, filename, printout_fontsize,
	       printout_block, printout_pageformat, printout_vary, anerr, printout_black, printout_layout);
  
	fl_reset_cursor(view->dnawin);
	if( anerr ) fl_alert("Error while writing to file %s", filename);
	}
#if !defined(__APPLE__)
else if(reponse == PDFOPTIONS ) {
	pdfps_options_dialog(view, true);
	}
#endif
}



#if !defined( __APPLE__)

static void printout_callback(Fl_Widget *obj, void *data)
{
const char *p;

p = ((Fl_Input *)obj)->value();
sscanf(p, "%d", (int *)data);
}


static void ok_callback(Fl_Widget *obj)
{
if(obj->window() == NULL) obj->hide();
else obj->window()->hide();
}


static void paper_callback(Fl_Widget *obj)
{
  printout_pageformat = ((Fl_Choice *)obj)->value() == 0 ? Fl_Paged_Device::A4 : Fl_Paged_Device::LETTER;
}

static void layout_callback(Fl_Widget *obj)
{
  printout_layout = ((Fl_Check_Button *)obj)->value() == 0 ? Fl_Paged_Device::PORTRAIT : Fl_Paged_Device::LANDSCAPE;
}

static void variable_callback(Fl_Widget *obj)
{
printout_vary = ! printout_vary;
}


static void radio_callback(Fl_Widget *obj)
{
Fl_Group *group;

if( ! ((Fl_Round_Button *)obj)->value() ) {
	((Fl_Round_Button *)obj)->setonly();
	return;
	}
group = obj->parent();
if ( ((Fl_Round_Button *)group->child(0))->value() ) printout_black = PDF_COLOR;
if ( ((Fl_Round_Button *)group->child(1))->value() )printout_black = PDF_BW;
if ( ((Fl_Round_Button *)group->child(2))->value() ) printout_black = TEXT_ONLY;
}

Fl_Window* pdfps_options_dialog(SEA_VIEW *view, bool autonomous)
{
static Fl_Window *pdf_form = NULL;
static Fl_Input *sizeinput, *blockinput;
static Fl_Choice *paper;
static Fl_Round_Button *colorb, *blackb, *tob;
static Fl_Check_Button *variable, *landscape;
Fl_Button *ok;
Fl_Group *radiog;
int x, y, w, h;
char txt[20];

if(pdf_form == NULL) {
fl_font(FL_HELVETICA, FL_NORMAL_SIZE);

pdf_form = new Fl_Window(415, 90);
pdf_form->box(FL_FLAT_BOX);
pdf_form->label("Set "PDF_OR_PS" output options");

x = 5 + (int)fl_width("block size:"); y = 5; w = 50; h = 25;
sizeinput = new Fl_Input(x, y, w, h, "font size:");
sizeinput->callback(printout_callback, &printout_fontsize);
blockinput = new Fl_Input(x, y + sizeinput->h() + 5 , w, h, "block size:");
blockinput->callback(printout_callback, &printout_block);
paper = new Fl_Choice(x + sizeinput->w() + (int)fl_width("paper size:") + 15, y, 
	(int)fl_width("LETTER") + 30, h, "paper size:");
paper->add("A4|LETTER");
paper->callback(paper_callback);
  
  landscape = new Fl_Check_Button(paper->x(), paper->y() + 30, 20, 20, "landscape");
  landscape->callback(layout_callback);
  landscape->align(FL_ALIGN_LEFT);

x = paper->x() + paper->w() + 5;
radiog = new Fl_Group(x, y, (int)fl_width(PDF_OR_PS" color") + 25, 3 * h + 3);
radiog->box(FL_DOWN_FRAME);
colorb = new Fl_Round_Button(x, y, radiog->w(), h, PDF_OR_PS" color");
blackb = new Fl_Round_Button(x, y + colorb->h() + 5, radiog->w(), h, PDF_OR_PS" B&&W");
tob = new Fl_Round_Button(x, y + colorb->h() + blackb->h() + 5, radiog->w(), h, "Text File");
colorb->callback(radio_callback);
colorb->type(FL_RADIO_BUTTON);
blackb->callback(radio_callback);
blackb->type(FL_RADIO_BUTTON);
tob->callback(radio_callback);
tob->type(FL_RADIO_BUTTON);
radiog->end();

x = blockinput->x() + blockinput->w() + 5;
variable = new Fl_Check_Button(x, blockinput->y() + blockinput->h(), (int)fl_width("Variable sites only") + 25, h, "Variable sites only");
variable->box(FL_DOWN_FRAME);
variable->callback(variable_callback);

ok = new Fl_Button(radiog->x() + radiog->w() + 5, variable->y(), 25, h, "OK");
ok->callback(ok_callback);

pdf_form->end();
pdf_form->size(ok->x() + ok->w() + 5 , pdf_form->h() );
pdf_form->resizable(NULL);
if (autonomous) pdf_form->set_modal();
else {
  ok->hide();
  pdf_form->box(FL_DOWN_BOX);
  pdf_form->set_visible();
  }
}

sprintf(txt, "%d", printout_fontsize);
sizeinput->value(txt);
sprintf(txt, "%d", printout_block);
blockinput->value(txt);
paper->value(printout_pageformat == Fl_Paged_Device::A4 ? 0 : 1);
  landscape->value(printout_layout == Fl_Paged_Device::LANDSCAPE ? 1 : 0);
if(printout_black == PDF_COLOR) colorb->setonly() ;
else if(printout_black == PDF_BW) blackb->setonly() ;
else  tob->setonly() ;
variable->value(printout_vary);
if(view->alt_col_rank != NULL && view->tot_sel_seqs == 1) variable->activate();
else variable->deactivate();
if (autonomous) pdf_form->show();
return pdf_form;
}
#endif




void hide_window_callback(Fl_Widget *ob, void *data)
{
ob->window()->hide();
}


void free_colranks_by_difference(char **alt_col_rank, int total)
{
int num;

if(alt_col_rank == NULL) return;
for(num = 0; num < total; num++) {
	free(alt_col_rank[num]);
	}
if( total > 0 ) free(alt_col_rank);
}


void reference_toggle(SEA_VIEW *view, int on)
{
char **tmp;
static int old_pos;

if(view->numb_gc == 1) return;
  Fl_Menu_Item *menu_props = (Fl_Menu_Item *)view->menu_props;
props_menu_parts *props_parts = (props_menu_parts *)menu_props->user_data();
if(on) { /* tenter de passer en mode par reference */
	if( view->tot_sel_seqs != 1 || view->numb_gc == 1) {
		(menu_props + props_parts->reference)->clear();
		return;
		}
	my_watch_cursor(view->dnawin);
	for(old_pos = 0; old_pos < view->tot_seqs; old_pos++)
		if(view->sel_seqs[old_pos]) break;
	deplacer_grp_seqs(view, 1);
	set_seaview_modified(view, FALSE);
	view->first_seq = 1;
	view->vertsli->Fl_Slider::value(1);
	view->alt_col_rank = prepcolranks_by_difference(view->sequence, 
		view->tot_seqs, 0, 
		view->max_seq_length,
		view->each_length, 
		( view->protein ? get_color_for_aa : get_color_for_base ), 
		view->numb_gc, view->allow_lower);
	fl_reset_cursor(view->dnawin);
	if(view->alt_col_rank == NULL) {
		view->DNA_obj->redraw();
		(menu_props + props_parts->reference)->clear();
		return;
		}
	tmp = view->alt_col_rank;
	view->alt_col_rank = view->col_rank;
	view->col_rank = tmp;		
	view->DNA_obj->redraw();
	(menu_props + props_parts->colors - 1)->deactivate();
	(menu_props + props_parts->reference)->set();
  ((Fl_Menu_Item*)view->menubar->find_item("Edit"))->deactivate();
  view->menu_species->bar_item()->deactivate();
  view->menubar->redraw();
	}
else	{ /* retour mode normal */
	my_watch_cursor(view->dnawin);
	tmp = view->alt_col_rank;
	view->alt_col_rank = view->col_rank;
	view->col_rank = tmp;		
	free_colranks_by_difference(view->alt_col_rank, view->tot_seqs);
	view->alt_col_rank = NULL;
	deplacer_grp_seqs(view, old_pos + 1);
	set_seaview_modified(view, FALSE);
	view->DNA_obj->redraw();
	(menu_props + props_parts->colors - 1)->activate();
	(menu_props + props_parts->reference)->clear();
  ((Fl_Menu_Item*)view->menubar->find_item("Edit"))->activate();
  view->menu_species->bar_item()->activate();
  view->menubar->redraw();
	fl_reset_cursor(view->dnawin);
	}
}


static Fl_Help_Dialog *help_viewer = NULL;
void help_callback(Fl_Widget *ob, void *unused)
{
	if(help_viewer == NULL) {
		char *help_file;
#ifndef DEFAULT_HELPFILE   /* to ease FreeBSD port */
#define DEFAULT_HELPFILE "seaview.html"
#endif
		help_file = get_res_value("helpfile", DEFAULT_HELPFILE);
		help_file = get_full_path(help_file);
		if(help_file == NULL) {
			fl_alert(
#ifdef __APPLE__
					 "No help information in program resources"
#else
					 "Help file %s\nnot found in PATH directories nor in current directory", help_file
#endif
					 );
			return;
		}
		my_watch_cursor(ob->window());
		help_viewer = new Fl_Help_Dialog();
		if(help_viewer == NULL) return;
#ifdef MICRO
		help_viewer->resize(help_viewer->x(), help_viewer->y(), 700, 600);
		help_viewer->textsize(14);
#else
		help_viewer->resize(help_viewer->x(), help_viewer->y(), 740, 500);
		help_viewer->textsize(12);
#endif
		help_viewer->load(help_file);
		help_viewer->show();
		fl_reset_cursor(ob->window());
#ifndef MICRO
		//for X11 control where the window appears, useful if multiple screens
		Fl_Window *w = Fl::first_window();// gives the help_viewer window
		if(w) w->hotspot(w);//so the window is close to the mouse
#endif
		}
	else help_viewer->show();
}


void direct_help_callback(Fl_Widget *wgt, void *data)
{
	help_callback(wgt, NULL);
	if(help_viewer != NULL) help_viewer->topline((char *)data);
}


int insert_gaps_at(SEA_VIEW *view, int seq, int site, int total)
{
char *pos, **psequence;
int l, gapcolor, *plength;
if(view->cursor_in_comment) {
	psequence = view->comment_line;
	plength = view->comment_length;
	}
else {
	psequence = view->sequence;
	plength = view->each_length;
	if (view->col_rank) gapcolor = ( view->protein ?
		get_color_for_aa('-') : get_color_for_base('-') );
	}
l = plength[seq-1];
if(site > l + 1) return total;
if( l + total > view->max_seq_length) total = view->max_seq_length - l;
pos = psequence[seq-1] + site - 1;
memmove(pos+total, pos, l - site + 2);
memset(pos, '-', total);
if( (!view->cursor_in_comment) && view->col_rank) {
	pos = view->col_rank[seq-1] + site - 1;
	memmove(pos+total, pos, l - site + 1);
	memset(pos, gapcolor, total);
	}
plength[seq-1] += total;
if( (!view->cursor_in_comment) && view->curr_colors == view->codoncolors && view->col_rank) {
	char **tmp = prepcolranks_by_codon(&view->sequence[seq-1], 1, view->max_seq_length, &view->each_length[seq-1], 
									   &view->comments[seq-1]);
	free(view->col_rank[seq-1]);
	view->col_rank[seq-1] = tmp[0];
	free(tmp);
	}
if (view->col_rank) set_seaview_modified(view, TRUE);
return total;
}


int delete_gaps_before(SEA_VIEW *view, int numseq, int numsite, int total)
{
char *site, *finseq, **psequence;
int count = -1, l, retval, *plength;

psequence = view->sequence;
plength = view->each_length;
site = psequence[numseq-1] + numsite - 1;
finseq = psequence[numseq-1] + plength[numseq-1] - 1;
do	{ site--; count++; }
while ( count < total && site >= psequence[numseq-1] && 
	( view->allow_seq_edit || *site == '-' || site > finseq) );
if(count == 0) return 0;
/* ne rien faire si on efface au dela de la fin de la seq */
if(numsite - count > plength[numseq-1]) return count;
l = plength[numseq-1];
retval = count;
if(numsite > l) { /* effacer depuis au dela fin jusqu'a interieur seq */
	count -= (numsite - l - 1);
	numsite = l + 1;
	}
site = psequence[numseq-1] + numsite - 1;
memmove(site-count, site, l - numsite + 2);
if( view->numb_gc > 1) {
		site= view->col_rank[numseq-1] + numsite - 1;
		memmove(site - count, site, l - numsite + 1);
	}
plength[numseq-1] -= count;
if ( view->curr_colors == view->codoncolors && view->col_rank) {
	char **tmp = prepcolranks_by_codon(&view->sequence[numseq-1], 1, view->max_seq_length, 
									   &view->each_length[numseq-1], &view->comments[numseq-1]);
	free(view->col_rank[numseq-1]);
	view->col_rank[numseq-1] = tmp[0];
	free(tmp);
	}
if (view->col_rank) set_seaview_modified(view, TRUE);
return retval;
}


void adjust_menu_edit_modes(SEA_VIEW *view)
{
if(view->menu_edit != NULL) {
	Fl_Menu_Item *items = (Fl_Menu_Item *)view->menu_edit;
  Fl_Menu_Item *align_items = (Fl_Menu_Item *)view->menu_align;
	if(view->tot_sel_seqs != 0) {
		items[DELETE_SEQ].activate();
		items[COPY_SEQS].activate();
		align_items[PROFILE].activate();
		align_items[UNALIGN].activate();
	  ((Fl_Menu_Item*)view->menu_file)[SAVE_REGIONS].activate();
		if(view->protein) items[EXCHANGE_UT].deactivate();
		else items[EXCHANGE_UT].activate();
		}
	else	{
		items[DELETE_SEQ].deactivate();
		items[EXCHANGE_UT].deactivate();
		items[COPY_SEQS].deactivate();
		align_items[PROFILE].deactivate();
		align_items[UNALIGN].deactivate();
	  if (view->active_region == NULL) ((Fl_Menu_Item*)view->menu_file)[SAVE_REGIONS].deactivate();
		}
	if(view->tot_sel_seqs != 0 && ! view->protein) items[SET_GCODE].activate();
	else items[SET_GCODE].deactivate();
  Fl_Menu_Item *menu_props = (Fl_Menu_Item *)view->menu_props;
	props_menu_parts *props_parts = (props_menu_parts *)menu_props->user_data();
	Fl_Menu_Item *byref = menu_props + props_parts->reference;
	if(view->tot_sel_seqs == 1 && view->curr_colors != view->codoncolors) byref->activate();
	else byref->deactivate();
	if(view->tot_sel_seqs == 1) {
		items[RENAME_SEQ].activate();
		if(view->viewasprots == NULL) items[DUPLICATE_SEQ].activate();
		items[EDIT_COMMENTS].activate();
		if(view->viewasprots == NULL) items[EDIT_SEQ].activate();
		if(view->protein) {
			items[COMPLEMENT_SEQ].deactivate();
			items[REVERSE_SEQ].deactivate();
			}
		else if(view->viewasprots == NULL) {
			items[COMPLEMENT_SEQ].activate();
			items[REVERSE_SEQ].activate();
			}
		}
	else	{
		items[RENAME_SEQ].deactivate();
		items[DUPLICATE_SEQ].deactivate();
		items[EDIT_COMMENTS].deactivate();
		items[EDIT_SEQ].deactivate();
		items[COMPLEMENT_SEQ].deactivate();
		items[REVERSE_SEQ].deactivate();
		}
	if(view->tot_sel_seqs == 2) 
		items[DOT_PLOT].activate();
	else
		items[DOT_PLOT].deactivate();
	if(view->tot_sel_seqs >= 2) {
		align_items[ALIGN_SELECTED_SEQS].activate();
		align_items[ALIGN_SITES].activate();
		if(view->viewasprots == NULL) items[CONSENSUS_SEQ].activate();
		}
	else	{
		align_items[ALIGN_SELECTED_SEQS].deactivate();
		align_items[ALIGN_SITES].deactivate();
		items[CONSENSUS_SEQ].deactivate();
		}
	}
}


void select_deselect_seq(SEA_VIEW *view, int new_seq, int from_tree)
{
/* new_seq = # seq a select/deselect; si 0: tout deselectionner; 
   si -1: tout selectionner 
   si -2: ne pas changer la selection mais ajuster l'interface selon son etat
*/
if(new_seq > 0) { /* traiter une sequence */
	view->sel_seqs[new_seq-1] = !view->sel_seqs[new_seq-1];
	if(view->sel_seqs[new_seq-1])
		++view->tot_sel_seqs;
	else
		--view->tot_sel_seqs;
	}
else if(new_seq == 0)	{ /* tout deselectionner */
	view->tot_sel_seqs = 0;
	memset(view->sel_seqs, 0, view->tot_seqs * sizeof(int));
	}
else if(new_seq == -1)	{ /* tout selectionner */
	int i;
	view->tot_sel_seqs = view->tot_seqs;
	for(i=0; i < view->tot_seqs; i++) view->sel_seqs[i] = TRUE;
	}
adjust_menu_edit_modes(view);
if(view->menu_species != NULL) {
	Fl_Menu_Item *items = (Fl_Menu_Item *)view->menu_species->get_menu();
	if(items != NULL) {
		if(view->tot_sel_seqs == 0)
			items[0].deactivate();
		else
			items[0].activate();
		items[1].deactivate();
		view->menu_species->value(0);
		}
	}
  if (!from_tree) {
    select_deselect_in_tree(view);
    }
}


void deplacer_grp_seqs(SEA_VIEW *view, int target)
{
/* deplacer toutes les seqs selectionnees pour positionner la premiere
d'entre elles en position target */
int *new_rank, *old_rank, old, new_val, numset;
char **aux;
-- target;
new_rank = (int *)malloc(view->tot_seqs * sizeof(int));
old_rank = (int *)malloc(view->tot_seqs * sizeof(int));
aux = (char **)malloc( view->tot_seqs * sizeof(char *) );
if(new_rank == NULL || old_rank == NULL || aux == NULL) out_of_memory();
/* compute old_rank[new_val] = old */
new_val = -1;
/* place first all non selected seqs */
for(old = 0; old < view->tot_seqs; old++) {
	if(!view->sel_seqs[old]) old_rank[++new_val] = old;
	}
/* allocate room for selected seqs */
if(target + view->tot_sel_seqs > view->tot_seqs)
	target = view->tot_seqs - view->tot_sel_seqs;
old = view->tot_seqs - view->tot_sel_seqs - target;
if(old != 0)
	memmove(old_rank + target + view->tot_sel_seqs, old_rank + target,
		old * sizeof(int));
/* insert selected seqs */
for(old = 0; old < view->tot_seqs; old++)
	if(view->sel_seqs[old]) old_rank[target++] = old;
/* compute new_rank[old] = new_val */
for(new_val = 0; new_val < view->tot_seqs; new_val++)
	new_rank[old_rank[new_val]] = new_val;
/* displace all sequence order-dependent ingredients */
/* deplacer la position du curseur */
if(!view->cursor_in_comment) {
	view->cursor_seq = new_rank[view->cursor_seq - 1] + 1;
	view->old_cursor_seq = view->cursor_seq;
	}
/* deplacer les seqs */
for(old = 0; old < view->tot_seqs; old++)
	aux[new_rank[old]] = view->sequence[old];
memcpy(view->sequence, aux, view->tot_seqs * sizeof(char *) );
/* deplacer les noms */
for(old = 0; old < view->tot_seqs; old++)
	aux[new_rank[old]] = view->seqname[old];
memcpy(view->seqname, aux, view->tot_seqs * sizeof(char *) );
if(view->comments != NULL) {
	/* deplacer les commentaires */
	for(old = 0; old < view->tot_seqs; old++)
		aux[new_rank[old]] = view->comments[old];
	memcpy(view->comments, aux, view->tot_seqs * sizeof(char *) );
	}
if(view->viewasprots != NULL) {
	/* deplacer les DNA seqs conservees pour mode vision traduction */
	char **viewasprots = (char **)view->viewasprots;
	for(old = 0; old < view->tot_seqs; old++)
		aux[new_rank[old]] = viewasprots[old];
	memcpy(viewasprots, aux, view->tot_seqs * sizeof(char *) );
	}
/* deplacer les seqs en couleurs */
if(view->numb_gc > 1) {
	for(old = 0; old < view->tot_seqs; old++) aux[new_rank[old]] = view->col_rank[old];
	memcpy(view->col_rank, aux, view->tot_seqs * sizeof(char *) );
	}
/* deplacer les sequences selectionnees */
for(old = 0; old < view->tot_seqs; old++)
	old_rank[new_rank[old]] = view->sel_seqs[old];
memcpy(view->sel_seqs, old_rank, view->tot_seqs * sizeof(int) );
/* deplacer les longueurs de sequences */
for(old = 0; old < view->tot_seqs; old++)
	old_rank[new_rank[old]] = view->each_length[old];
memcpy(view->each_length, old_rank, view->tot_seqs * sizeof(int) );
/* process species sets */
for(numset = 0; numset < view->numb_species_sets; numset++) {
	for(old = 0; old < view->tot_seqs; old++)
		old_rank[new_rank[old]] = view->list_species_sets[numset][old];
	memcpy(view->list_species_sets[numset], old_rank, 
		view->tot_seqs * sizeof(int) );
	}
free(aux); free(old_rank); free(new_rank);
set_seaview_modified(view, TRUE);
}


void update_current_seq_length(int newlength, SEA_VIEW *view)
{
double x; int l;
if(newlength > view->seq_length) {
	view->seq_length = 
		( newlength+20 < view->max_seq_length ? 
		newlength+20 : view->max_seq_length );
	l = view->seq_length - view->tot_sites+3;
	if(l<1) l=1;
	view->horsli->bounds(1,l);
	x = ( (double) view->tot_sites ) / 
		( view->seq_length + 3 ) ;
	if(x>1) x=1;
	view->horsli->slider_size(x);
	}
}


int insert_char_in_seq( int key, int total, SEA_VIEW *view)
/* to insert the typed key in the sequence at cursor location if it is visible
returns # of inserted chars if ok, 0 if error (= cursor not visible or 
max seq size is reached) 
*/
{
char *pos;
int l, c, *plength;
if(view->cursor_in_comment) {
	if( view->cursor_seq < 1 ||
	   view->cursor_seq >= view->tot_comment_lines ||
	   view->cursor_site < view->first_site ||
	   view->cursor_site >= view->first_site + view->tot_sites ) return 0;
	l = view->comment_length[view->cursor_seq - 1];
	}
else	{
	if( view->cursor_seq < view->first_seq ||
	   view->cursor_seq >=view->first_seq+view->tot_lines ||
	   view->cursor_site < view->first_site ||
	   view->cursor_site >= view->first_site + view->tot_sites ) return 0;
	l = view->each_length[view->cursor_seq-1];
	}
if(view->cursor_site > l + 1) return 0;
if( l + total > view->max_seq_length) total = view->max_seq_length - l;
if(total <= 0) return 0;
if(view->cursor_in_comment) 
	pos = view->comment_line[view->cursor_seq - 1] + view->cursor_site - 1;
else
	pos = view->sequence[view->cursor_seq - 1] + view->cursor_site - 1;
memmove(pos+total, pos, l - view->cursor_site + 2);
memset(pos, view->cursor_in_comment || view->allow_lower ? key : toupper(key) ,
	 total);
if( (!view->cursor_in_comment) && view->numb_gc > 1) {
	   pos= &view->col_rank[view->cursor_seq-1][view->cursor_site-1];
	   memmove(pos+total, pos, l - view->cursor_site + 1);
	c = (view->protein ? get_color_for_aa(key) : get_color_for_base(key) );
	memset(view->col_rank[view->cursor_seq-1] + view->cursor_site - 1, 
		c, total);
	}
if(view->cursor_in_comment) 
	plength = &(view->comment_length[view->cursor_seq-1]);
else
	plength = &(view->each_length[view->cursor_seq-1]);
(*plength) += total;
update_current_seq_length(*plength, view);
if( (!view->cursor_in_comment) && view->curr_colors == view->codoncolors) {
	char **tmp = prepcolranks_by_codon(&view->sequence[view->cursor_seq-1], 1, view->max_seq_length, 
								&view->each_length[view->cursor_seq-1], &view->comments[view->cursor_seq-1]);
	free(view->col_rank[view->cursor_seq-1]);
	view->col_rank[view->cursor_seq-1] = tmp[0];
	free(tmp);
	}
set_seaview_modified(view, TRUE);
return total;
}

/*  The routine that does drawing */
void DNA_obj::draw(void)
{
SEA_VIEW *view = (SEA_VIEW *)this->user_data();
if(view == NULL) return; //this is necessary after deletion of an alignment window

#if (100*FL_MAJOR_VERSION + FL_MINOR_VERSION >= 104) && !defined(__APPLE__)
  if (view->scale != fl_graphics_driver->scale()) {
    view->scale = fl_graphics_driver->scale();
    fl_font(view->DNA_obj->labelfont(), view->DNA_obj->labelsize());
    view->char_width = fl_width('W');
  }
#endif
  
/* returns TRUE if window size was changed by user */
if( compute_size_params( view, FALSE) ) {
	view->horsli->redraw();
	view->vertsli->redraw();
	}
if( ( this->damage() & FL_DAMAGE_ALL ) != 0 ) {
// appel autre que uniquement par damage partiel
	view->draw_names = -1;
	view->mod_cursor = view->mod_region_line = view->mod_comment_line = 
		FALSE; 
	view->mod_seq = 0;
	}
if(view->draw_names) { /* soit tous (<= -1) soit un seul ( >= 1) */
	draw_seq_names(view->DNA_obj, view);
	if(view->draw_names > 0 || view->draw_names == -2){
		/* si > 0 ou -2, ne pas ecrire les seqs*/
		view->draw_names = -1;
		return;
		}
	}
if(view->mod_cursor) {
	/* effacer old_cursor en ecrivant dessus */
	draw_cursor(view->DNA_obj, FALSE, view->old_cursor_site, 
		view->old_cursor_seq, view->old_cursor_in_comment);
	view->mod_cursor = FALSE;
	}
else if(view->mod_region_line) {
	draw_region_line(view->DNA_obj, view);
	view->mod_region_line = FALSE;
	}
else if(view->mod_comment_line) {
	draw_comment_lines(view->DNA_obj, view);
	view->mod_comment_line = FALSE;
	}
else	{
	if(view->tot_seqs == 0) {
#define DRAG_MESS "alignment/tree file drop zone"
		fl_color(FL_WHITE);
		fl_font(FL_TIMES_BOLD_ITALIC, 28 );
		fl_draw(DRAG_MESS, (int)(this->x()+this->w()/2-fl_width(DRAG_MESS)/2 + 0.5), this->y()+this->h()/2);
		}
	if(view->inverted_colors)
		    draw_dna_seqs_inverted(view->DNA_obj, view);
	else
		    draw_dna_seqs(view->DNA_obj, view);
	draw_comment_lines(view->DNA_obj, view);
	}
view->mod_seq = 0;
view->draw_names = -1;
draw_cursor(view->DNA_obj, TRUE , view->cursor_site, view->cursor_seq, 
	view->cursor_in_comment);
view->old_cursor_seq = view->cursor_seq;
view->old_cursor_site = view->cursor_site;
view->old_cursor_in_comment = view->cursor_in_comment;
}


int my_event_button(void)
/* returns mouse button number 1=left, 2=centre, 3=right.
on Mac or Win32, extra buttons can also be emulated by modifier keys (shift for right, ctrl for middle)
Also returns 4 for cmd-Click (Mac) alt-Click (win32) meta-Click (X11)
*/
{
#ifdef __APPLE__
#define EXTRA_MODIF_KEY FL_META
#elif defined(WIN32)
#define EXTRA_MODIF_KEY FL_ALT
#else
#define EXTRA_MODIF_KEY FL_META
#endif
int key;

key = Fl::event_button();  /* key: 1=gauche, 2=centre, 3=droit souris */
if(key == 1) {//emulate mouse buttons by modifiers
	if(Fl::event_state(FL_CTRL)) key = 2;// bouton du milieu par Ctrl-Click
	else if(Fl::event_state(FL_SHIFT)) key = 3;// bouton  droit par Shift-Click
	}
if(Fl::event_state(EXTRA_MODIF_KEY)) key = 4;// cmd-Click (Mac) alt-Click (win32) meta-Click (X11)
return key;
}



int DNA_obj::handle(int event) 
{
  SEA_VIEW *view;
  /* numero courant de la derniere seq selectionnee pendant selection de seqs
   par glissement de la souris
   */
  static int selecting_seqs = 0, sel_seq_move = 0;
  static int modifying_segment = 0;
  int mx = Fl::event_x();
  int my = Fl::event_y();
  static int doing_dnd = FALSE;
  
  view = (SEA_VIEW *) ( this->user_data() );
  if(view == NULL) return 1;
  switch (event)
  {
    case FL_DND_ENTER:
      return view->tot_seqs == 0;
    case FL_DND_DRAG:
      return view->tot_seqs == 0;
    case FL_DND_RELEASE:
   	 	if(view->tot_seqs == 0) {
        doing_dnd = TRUE;
        return 1;
      }
      else return 0;
    case FL_PASTE:
      char *p;
      p = (char*)Fl::event_text();
#if ! ( defined(__APPLE__) || defined(WIN32) )
      if(doing_dnd) {
        char *q;
        if(strncmp(p, "file://", 7) == 0) p += 7;
        if((q = strchr(p, '\r')) != NULL) *q = 0;
        if((q = strchr(p, '\n')) != NULL) *q = 0;
#if 100*FL_MAJOR_VERSION + 10*FL_MINOR_VERSION  + FL_PATCH_VERSION == 130
        char *zero = p + strlen(p);
        q = p;
        while (*q) {
          if (*q == '%') {
            // non-ascii chars are recoded by %## using their hexadecimal value
            int h;
            sscanf(q+1, "%2X", &h);
            *q = h;
            memmove(q+1, q+3, zero - (q+2));
            zero -= 2;
          }
          q++;
        }
#else
        fl_decode_uri(p);
#endif
      }
#endif
      handle_paste(view, p, doing_dnd);
      doing_dnd = FALSE;
      Fl::focus(this);
      return 1;
    case FL_FOCUS:
    case FL_UNFOCUS:
      return 1;
    case FL_PUSH:
      int key;
      key = my_event_button();
      if( key == 1 && Fl::event_clicks() ) { /* left double click */
        int new_seq;
        Fl::event_clicks(0);
        new_seq = (my + view->line_height/2 -
                   view->y_seq)/view->line_height + view->first_seq;
        if( new_seq < view->first_seq || new_seq > view->tot_seqs ||
           new_seq >= view->first_seq + view->tot_lines ) break;
        if(mx < view->x_name || mx >= view->x_seq - view->char_width ||
           key != 1) break;
        /* double click sur nom de seq: selection de toutes les seqs */
        /*		if(view->alt_col_rank != NULL) break;
         if(view->multipl->argument() > 0) mod_multipl(view,0);
         select_deselect_seq(view, -1);
         selecting_seqs = 0;
         view->draw_names = -2;
         // ceci signifie redraw partiel commande' par draw_names, mod_cursor, ...
         this->damage(1); */
      }
      else	{ /* simple click */
        Fl::focus(this);
        handle_push(view, mx, my, key, &modifying_segment,
                    &selecting_seqs, &sel_seq_move);
      }
      break;
    case FL_DRAG: /* mouvement avec souris enfoncee */
      handle_mouse(view, mx, my, &selecting_seqs, &sel_seq_move,
                   &modifying_segment);
      break;
    case FL_RELEASE:
      //     case FL_LEAVE:
      if(selecting_seqs) {
        if(sel_seq_move) {
          select_deselect_seq(view, sel_seq_move);
          view->draw_names = sel_seq_move;
          this->damage(1);
        }
        else
          select_deselect_seq(view, -2);
        selecting_seqs = 0;
      }
      else if(modifying_segment) {
        end_change_segment(view);
        view->draw_names = 0;
        this->damage(1);
        modifying_segment = 0;
      }
      break;
    case FL_KEYBOARD:
      if( Fl::event_state(FL_CTRL) || Fl::event_state(FL_META)) {
        return 0; /* ne pas traiter ici car shortcut d'autres widgets */
      }
      key = Fl::event_key();
      if(key == FL_Delete || key == FL_BackSpace || 
         key == FL_Right || key == FL_Left || key == FL_Up || key == FL_Down  ||
         key == FL_Page_Up || key == FL_Page_Down) {
        handle_keyboard(view, key, FALSE );
        return 1;	
      }
      if(Fl::event_length() > 0) { // du vrai texte 
        handle_keyboard(view, Fl::event_text()[0], TRUE );
      }
      break;
    default :
      return 0;
  }
  return 1;
}


void handle_paste(SEA_VIEW *view, char *clipboard, int doing_dnd)
{
  if (doing_dnd) {
    use_initial_file(view, clipboard, doing_dnd);
    return;
  }

  char **seqs, **seqnames, **comments, *p, *message;
  int count, i, num, changedwname = FALSE;
  if (pasted_from_what_source == 1 || memcmp(clipboard, CLIPBOARDMARK, strlen(CLIPBOARDMARK)) ) {
    if (pasted_from_what_source == 0) {
      pasted_from_what_source = 1; // try to paste from clipboard that may contain fasta data
      Fl::paste(*(view->DNA_obj), 1);
      return;
    }
    if (*clipboard != '>') return;
    char *header, *err_message;
    char *fname = create_tmp_filename();
    FILE *out = fopen(fname, "w");
    fputs(clipboard, out);
    fclose(out);
    count = read_fasta_align(fname, &seqs, &seqnames, &comments, &header, &err_message, false);
    delete_tmp_filename(fname);
    if (count) { // caution here: fl_choice may change Fl::e_text which may be reused after return
      char *save = Fl::e_text;
      int savel = Fl::e_length;
      Fl::e_text = NULL;
      i = fl_choice("You are about to add to this alignment %d sequence%s (%s) \n"
		    "present in the clipboard.", "Cancel", "Confirm", 
		    NULL, count, (count > 1 ? "s" : ""), seqnames[0]);
      Fl::e_text = save;
      Fl::e_length = savel;
      if (!i) return;
    }
  }
  else {
    p = (char*)strchr(clipboard, ':') + 1;
    count = read_mase_seqs_header(p, &seqs, &seqnames, &comments, NULL, &message);
  }
  if (count == 0) return;
  if (view->tot_seqs == 0 && view->masename == NULL) {
	  view->masename = strdup("newfile");
	  changedwname = TRUE;
	  }
  for (i = 0; i < count; i++) {
	  num = add_seq_to_align(view, seqnames[i], seqs[i], strlen(seqs[i]));
	  if(num != 0) view->comments[view->tot_seqs - 1] = comments[i];
	  }
  if (changedwname) {
#if defined(__APPLE__) && 100*FL_MAJOR_VERSION + FL_MINOR_VERSION < 104
    rename_windowmenuitem(view->dnawin->label(), find_windowmenuitem(view->dnawin));
#endif
    ((Fl_Menu_Item*)view->menu_file)[SAVE].deactivate();
  }
}


void handle_mouse(SEA_VIEW *view, int mx, int my, 
	int *p_selecting_seqs, int *p_sel_seq_move, int *p_modifying_segment)
{ /* mouvement avec souris enfoncee */
int debut, fin, step, num, new_seq, new_site;

if(*p_selecting_seqs != 0) {
	new_seq = (my + view->line_height/2 - 
		view->y_seq)/view->line_height + view->first_seq;
	if(new_seq == *p_selecting_seqs) return;
	if( new_seq < view->first_seq || new_seq > view->tot_seqs ||
		new_seq >= view->first_seq + view->tot_lines ) return;
	if(!view->sel_seqs[new_seq - 1]) 
		{ debut= new_seq; fin = *p_selecting_seqs; }
	else
		{ debut= *p_selecting_seqs; fin = new_seq; }
	if(debut < fin) step = 1;
	else	step = -1;
	*p_selecting_seqs = new_seq;
	for(num = debut; num != fin; num += step) {
		new_seq = debut + fin - step - num - 1;
		if(view->sel_seqs[new_seq]) {
			view->sel_seqs[new_seq] = FALSE;
			--(view->tot_sel_seqs);
			}
		else	{
			view->sel_seqs[new_seq] = TRUE;
			++(view->tot_sel_seqs);
			}
		if(*p_sel_seq_move == new_seq + 1) *p_sel_seq_move = 0;
		}
	if(*p_sel_seq_move) {
		if( view->sel_seqs[*p_sel_seq_move - 1] ) {
			view->sel_seqs[*p_sel_seq_move - 1] = FALSE;
			--(view->tot_sel_seqs);
			}
		else	{
			view->sel_seqs[*p_sel_seq_move - 1] = TRUE;
			++(view->tot_sel_seqs);
			}
		*p_sel_seq_move = 0;
		}
	view->draw_names = -2;
	view->DNA_obj->damage(1);
	}
else if(*p_modifying_segment != 0) {
	new_site = (mx - view->x_seq )/view->char_width + 
		view->first_site;
	if(new_site == *p_modifying_segment) return;
	if( new_site < view->first_site || 
		new_site > view->first_site + view->tot_sites ||
		new_site > view->region_length ) return;
	if( continue_change_segment(view, new_site) ) {
		*p_modifying_segment = new_site;
		view->draw_names = 0;
		view->mod_region_line = TRUE;
		view->DNA_obj->damage(1);
		}
	}
}


void handle_push(SEA_VIEW *view, int mx, int my, int key, 
	int *p_modifying_segment, int *p_selecting_seqs, int *p_sel_seq_move)
/* key: 1=bouton gauche, 2=centre, 3=droit de la souris */
{
int new_site, new_seq, new_line;

if(view->multipl->argument() > 0) mod_multipl(view,0);
new_seq = (my + view->line_height/2 - view->y_seq)/view->line_height + 
	view->first_seq;
new_line = new_seq - view->first_seq + 1;
new_site = (mx - view->x_seq )/view->char_width + 
	view->first_site;
if(view->active_region != NULL && 
  new_seq == view->first_seq + FL_min(view->tot_lines,view->tot_seqs) &&
	new_site >= view->first_site && 
	new_site < view->first_site + view->tot_sites && 
	new_site <= view->region_length ) {
/* work with segments: extend, or create, or delete */
	if(key == 2) { /* middle button:extend left neighbor segment */
		new_seq = extend_segment_at_left(view, new_site);
		if(new_seq) fl_beep(FL_BEEP_DEFAULT);
		else	{
			view->draw_names = 0;
			}
		}
	else if(key == 3) { /* right button=>delete segment */
		new_seq = suppr_segment(view->active_region, new_site,
			view->region_line);
		if(new_seq) fl_beep(FL_BEEP_DEFAULT);
		else	{
			view->draw_names = 0;
			}
		}
	else	{ /* left button=>extend or create segment */
		new_seq = begin_change_segment(view, new_site);
		if(new_seq) {
			view->mod_region_line = TRUE;
			*p_modifying_segment = new_site;
			}
		view->draw_names = 0;
		}
	if(view->draw_names == 0) {
		view->DNA_obj->damage(1);
		}
	return;
	}
if( view->show_comment_lines && new_line >= view->pos_first_comment_line &&
	new_line < view->pos_first_comment_line + view->tot_comment_lines ) {
/* dans les comment lines */
	int num, old;
	if(key != 1) return;
	num = new_line - view->pos_first_comment_line + 1;
	if(mx >= view->x_name && mx < view->x_seq - view->char_width) {
	/* click sur nom de comment line: selection/deselection */
		old = view->active_comment_line;
		if(old == num)
			view->active_comment_line = 0;
		else
			view->active_comment_line = num;
		if(old == num || old == 0)
			view->mod_comment_line = num;
		else	{
			view->mod_comment_line = 0;
			view->mod_seq = 1; // astuce
			}
		view->draw_names = 0;
		update_menu_footers(view);
		view->DNA_obj->damage(1);
		}
	else if( new_site >= view->first_site && 
		new_site < view->first_site + view->tot_sites &&
		new_site <= view->comment_length[num - 1] + 1 ) {
		/* click sur comment: positionnement du curseur */
		view->cursor_site = new_site;
		view->cursor_seq = num;
		view->draw_names = 0;
		view->mod_cursor = TRUE;
		view->cursor_in_comment = TRUE;
		view->DNA_obj->damage(1);
		}
	return;
	}
	
if( new_seq < view->first_seq || new_seq > view->tot_seqs ||
	new_seq >= view->first_seq + view->tot_lines ) return;
if(mx >= view->x_name && mx < view->x_seq - view->char_width) {
/* click sur nom de seq: selection/deselection */
	if(view->alt_col_rank != NULL) return;
	if(key == 1) {
		*p_selecting_seqs = new_seq;
		*p_sel_seq_move = new_seq;
		return;
		}
	else if(key == 3) {
		select_deselect_seq(view, 0);
		view->draw_names = -2;
		view->DNA_obj->damage(1);
		return;
		}
	else if(key == 4) { //cmd-Click select from last selected to here
		int i;
		for(i = new_seq; i > 0; i--) {
			if(view->sel_seqs[i - 1]) break;
			}
		if(i == 0) return;
		while(++i <= new_seq) { 
			if(!view->sel_seqs[i - 1]) {
				view->sel_seqs[i - 1] = TRUE; 
				view->tot_sel_seqs++; 
				}
			}
		select_deselect_seq(view, -2);
		view->draw_names = -2;
		view->DNA_obj->damage(1);
		return;
		}
	else	{ /* milieu: depl des seqs selectionnees */
		if(view->tot_sel_seqs == 0 || 
			view->sel_seqs[new_seq - 1]) 
			{ fl_beep(FL_BEEP_DEFAULT); return; }
		deplacer_grp_seqs(view, new_seq);
		view->DNA_obj->redraw();
		}
	return;
	}
if(key != 1)  return;
/* click sur seq: positionnement du curseur */
if( new_site >= view->first_site && 
	new_site < view->first_site + view->tot_sites &&
	new_site <= view->each_length[new_seq-1] + 1 ) {
	view->cursor_site = new_site;
	view->cursor_seq = new_seq;
	view->cursor_in_comment = FALSE;
	view->draw_names = 0;
	view->mod_cursor = TRUE;
	view->DNA_obj->damage(1);
	}
return;
}


void handle_keyboard(SEA_VIEW *view, unsigned int key, int istext)
{
int new_pos, multipl, num;

multipl = view->multipl->argument();
if(multipl == 0) multipl = 1;
if(key == FL_Right) { /* right arrow */		
	new_pos = view->cursor_site + multipl;
    if(view->cursor_in_comment) {
	if(new_pos > view->comment_length[view->cursor_seq-1]+1)
	   new_pos = view->comment_length[view->cursor_seq-1]+1;
	}
    else {
	if(new_pos > view->each_length[view->cursor_seq-1] + 1) 
	   new_pos = view->each_length[view->cursor_seq-1] + 1;
	}
    set_and_show_new_cursor_site(view, new_pos,FALSE,FALSE);
    }
else if(key == FL_Left) { /* left arrow */		
	new_pos = FL_max(1, view->cursor_site - multipl);
	set_and_show_new_cursor_site(view, new_pos,FALSE,FALSE);
	}
else if(key == FL_Up) { /* up arrow */
	new_pos = FL_max(1, view->cursor_seq - multipl);
	set_and_show_new_cursor_seq(view, new_pos);
	}
else if(key == FL_Down){ /* down arrow */
	new_pos = view->cursor_seq + multipl;
	if(view->cursor_in_comment) {
		if(new_pos > view->tot_comment_lines) 
			new_pos = view->tot_comment_lines;
		}
	else	{
		if(new_pos > view->tot_seqs) 
			new_pos = view->tot_seqs;
		}
	set_and_show_new_cursor_seq(view, new_pos);
	}
else if(key == FL_Page_Up) { /* page up key */
  new_pos = FL_max(1, view->cursor_seq - view->tot_lines + 1);
  set_and_show_new_cursor_seq(view, new_pos);
  }
else if(key == FL_Page_Down) { /* page down key */
  new_pos = FL_min(view->tot_seqs, view->cursor_seq + view->tot_lines - 1);
  set_and_show_new_cursor_seq(view, new_pos);
}
else if(view->cursor_in_comment && 
	view->active_comment_line == view->cursor_seq) {
	unsigned char c_key = (unsigned)key;
	if(view->alt_col_rank != NULL) return;
	if( key == 0x7f || key == 0x8 || key == FL_Delete || key == FL_BackSpace ) /* del or BS */
		delete_char_in_comment(view, 1, 
			view->active_comment_line, 
			view->cursor_site, FALSE);
	else if( istext && ( ( c_key >= 32 && c_key <= 126 ) || 
		( c_key >= 160 /* && c_key <= 255 */ )  ) ) 
		insert_char_in_comment(c_key, 1, view);
	else
		return;
	}
else if(strchr(view->movekeys, key) != NULL) { 
					/* ][>< touches depl droite/gauche */
	int oldpos;
	int upper_step=50, bracket_step=5 ;
	int max_w;
	max_w = (int)(view->horsli->maximum());
	oldpos = (int)( view->horsli->value() );
	new_pos = oldpos;
	upper_step *= multipl;
	bracket_step *= multipl;
	if ((int)key == view->movekeys[2] /* > */ ) {
		new_pos=oldpos+upper_step;
		if(new_pos>max_w) new_pos=max_w;
		}
	else if((int)key == view->movekeys[3] /* < */ ) {
		new_pos=oldpos-upper_step;
		if(new_pos<1) new_pos=1;
		}
	else if((int)key == view->movekeys[0] /* ] */ ) {
		new_pos=oldpos+bracket_step;
		if(new_pos>max_w) new_pos=max_w;
		}
	else if((int)key == view->movekeys[1] /* [ */ ) {
		new_pos=oldpos-bracket_step;
		if(new_pos<1) new_pos=1;
		}
	if(new_pos!=oldpos) {
		view->horsli->Fl_Slider::value(new_pos);
		view->draw_names = 0;
		view->first_site = new_pos;
		view->DNA_obj->damage(1);
		}
	}
else if(key == 0x7f || key == 0x8 || key == FL_Delete || key == FL_BackSpace ) { /* delete or backspace */
	int count, count_each, debut, fin, test;
	    if(view->multipl->argument() > 0)
		mod_multipl(view,0);
	if(view->cursor_in_comment)
		test = view->cursor_seq < 1 ||
	   		view->cursor_seq > view->tot_comment_lines;
	else
		test = view->cursor_seq < view->first_seq ||
	   		view->cursor_seq >= view->first_seq+view->tot_lines;
	if( test ||
	   view->cursor_site < view->first_site ||
	   view->cursor_site >=view->first_site+view->tot_sites)
		{ fl_beep(FL_BEEP_DEFAULT); return; }
	if(view->cursor_in_comment) {
		if( delete_char_in_comment(view, multipl, 
			view->cursor_seq, view->cursor_site,
			TRUE) != multipl) fl_beep(FL_BEEP_DEFAULT);
		return;
		}
	if(view->alt_col_rank != NULL) return;
	if(view->tot_sel_seqs > 1 &&
		view->sel_seqs[view->cursor_seq - 1])
		{ debut = 1; fin = view->tot_seqs; test = TRUE;}
	else
		{ debut = fin = view->cursor_seq; test = FALSE;}
	if(multipl > view->cursor_site - 1) multipl = view->cursor_site - 1;
	for(num = debut; num<=fin; num++) 
		{
		if(test && !view->sel_seqs[num-1]) continue;
		count_each = delete_gaps_before(view,
			num, 
			view->cursor_site, multipl);
		if(count_each < multipl) fl_beep(FL_BEEP_DEFAULT);
		if(num == view->cursor_seq) count = count_each;
		}
	/* si ttes seqs selectionnees, traiter aussi regions et comments */
	if(count_each == multipl && 
		(!view->cursor_in_comment) &&
		view->tot_sel_seqs == view->tot_seqs ) {
		if(view->regions != NULL)
			delete_region_part(view, 
				view->cursor_site,multipl);
		if(view->tot_comment_lines > 0)
			delete_in_all_comments(multipl,
				view->cursor_site, view);
		}
	new_pos = view->cursor_site - count;
	if(new_pos <= 0) new_pos = 1;
	if(view->cursor_in_comment) {
		view->mod_comment_line = view->cursor_seq;
		}
	else	{
		if(view->tot_sel_seqs > 1 && 
			view->tot_sel_seqs != view->tot_seqs &&
			view->sel_seqs[view->cursor_seq - 1])
			view->mod_seq = -1;
		else if(view->tot_sel_seqs <= 1 || 
			!view->sel_seqs[view->cursor_seq - 1])
			view->mod_seq = view->cursor_seq;
		}
	set_and_show_new_cursor_site(view, new_pos, 
		FALSE,TRUE);
	}
else if(key == '_' ) { /* del gap in all but current seq(s) */
	int count_each;
	    if(view->multipl->argument() > 0)
		mod_multipl(view,0);
	if(view->cursor_in_comment) return;
	if( view->cursor_seq < view->first_seq ||
	   	view->cursor_seq >=view->first_seq+view->tot_lines ||
	  	view->cursor_site < view->first_site ||
	   	view->cursor_site >= 
	   	view->first_site + view->tot_sites ||
	   	view->tot_sel_seqs == view->tot_seqs )
		 { fl_beep(FL_BEEP_DEFAULT); return; }
	if(view->alt_col_rank != NULL) return;
	for( num = 1; num <= view->tot_seqs; num++) {
		if(num == view->cursor_seq || 
			(view->sel_seqs[view->cursor_seq-1] &&
			view->sel_seqs[num-1] ) ) continue;
		count_each = delete_gaps_before(view, 
		    	num, view->cursor_site, multipl);
		if(count_each < multipl) {
			fl_beep(FL_BEEP_DEFAULT);
			return;
			}
		}
	if(count_each == multipl && view->regions != NULL)
		delete_region_part(view, view->cursor_site, multipl);
	if(count_each == multipl && view->tot_comment_lines > 0)
		delete_in_all_comments(multipl, view->cursor_site, view);
	new_pos = view->cursor_site - multipl;
	if(new_pos <= 0) new_pos = 1;
	set_and_show_new_cursor_site(view, new_pos, FALSE, TRUE);
	}
else if( key == '-' || (key == ' ' && !view->hjkl)
			  /* gap key = - or space */
	|| key == '+' ) { /* insert gap in other seqs key */
	int newlength = 0, count = 0, count_each, debut, fin, test;
	    if(view->multipl->argument() > 0)
		mod_multipl(view,0);
	if(view->cursor_in_comment && key == '+') return;
	if(view->cursor_in_comment)
		test = FALSE;
	else
		test = view->cursor_seq < view->first_seq ||
	   		view->cursor_seq >=view->first_seq+view->tot_lines;
	if( test || view->cursor_site < view->first_site ||
	   	view->cursor_site >= 
		view->first_site + view->tot_sites )
		 { fl_beep(FL_BEEP_DEFAULT); return; }
	if(view->alt_col_rank != NULL) return;
	if(key != '+') { /* gap key */
	    if(view->tot_sel_seqs > 1 && 
		(!view->cursor_in_comment) &&
		view->sel_seqs[view->cursor_seq - 1])
		{ debut = 1; fin = view->tot_seqs; test = TRUE;}
	    else
		{ debut = fin = view->cursor_seq; test = FALSE;}
	    for(num = debut; num<=fin; num++) 
		{
		if(test && !view->sel_seqs[num-1]) continue;
		count_each = insert_gaps_at(view, num, 
			view->cursor_site, multipl);
		if(count_each < multipl) fl_beep(FL_BEEP_DEFAULT);
		if(num == view->cursor_seq) count = count_each;
		if(view->cursor_in_comment) {
		   if(newlength < view->comment_length[num-1])
			newlength = view->comment_length[num-1];
		     }
		else {
		   if(newlength < view->each_length[num-1])
			   newlength = view->each_length[num-1];
		     }
		}
/* si ttes seqs selectionnees, traiter aussi regions et comments */
	    if(count_each == multipl && 
		(!view->cursor_in_comment) &&
		view->tot_sel_seqs == view->tot_seqs) {
		if(view->regions != NULL)
			insert_region_part(view, view->cursor_site, multipl);
		if(view->tot_comment_lines > 0)
			insert_gap_all_comments(multipl,view->cursor_site, 
				view);
		}
	    }
	else	{ /* + ==> gap in other sequences */
		if(view->tot_sel_seqs == view->tot_seqs) {
			fl_beep(FL_BEEP_DEFAULT); return;
			}
		for( num = 1; num <= view->tot_seqs; num++) {
			if(num == view->cursor_seq || 
			     (view->sel_seqs[view->cursor_seq-1] &&
			      view->sel_seqs[num-1] ) ) continue;
			count_each = insert_gaps_at(view, 
			    num, view->cursor_site, multipl);
			if(count_each < multipl) {
				fl_beep(FL_BEEP_DEFAULT); return;
				}
			if(newlength < view->each_length[num-1])
			   	newlength = view->each_length[num-1];
			}
		count = multipl;
		if(count_each == multipl && 
			view->regions != NULL)
			insert_region_part(view, view->cursor_site, multipl);
		if(count_each == multipl && 
			view->tot_comment_lines > 0) {
			insert_gap_all_comments(multipl,view->cursor_site, 
				view);
			   }
		}
	new_pos = view->cursor_site + count;
	if(view->cursor_in_comment) {
	 	if(new_pos> view->comment_length[view->cursor_seq-1]+1)
	    		new_pos= view->comment_length[view->cursor_seq-1]+1;
	 	}
	else 	{
	 	if(new_pos > view->each_length[view->cursor_seq-1] + 1)
	   		 new_pos = view->each_length[view->cursor_seq-1] + 1;
	 	}
	if(view->cursor_in_comment)
		view->mod_comment_line = view->cursor_seq;
	else if(key != '+' ) {
		if(view->tot_sel_seqs > 1 && 
			view->tot_sel_seqs != view->tot_seqs &&
			view->sel_seqs[view->cursor_seq - 1])
			view->mod_seq = -1;
		else if(view->tot_sel_seqs <= 1 ||
			!view->sel_seqs[view->cursor_seq - 1] )
			view->mod_seq = view->cursor_seq;
		}
	update_current_seq_length(newlength, view);
	set_and_show_new_cursor_site(view, new_pos, FALSE,TRUE);
	}
else if( key >= '0' && key <= '9' ) { /* multiplicateur */
	multipl = view->multipl->argument() * 10;
	multipl += (key - '0');
	mod_multipl(view, multipl);
	return;
	}
else if( view->allow_seq_edit && (view->alt_col_rank == NULL) &&
	(!view->cursor_in_comment) &&
	isprint(key) /* a user asked for inserting special characters in sequences */
	) {
	if(view->hjkl) {
		static char typedkey[]= "hjklHJKL ";
		static char dnaequivs[3][10]={
			"gatcGATCN", "tcgaTCAGN", "acgtACGTN"};
		char *p;
		p = strchr(typedkey, key);
		if(p != NULL) 
			key = *( dnaequivs[view->hjkl - 1] + (p - typedkey) );
		}
	if(key == ' ') num = 0;
	else	num = insert_char_in_seq(key, multipl, view);
	if( num == 0 ) fl_beep(FL_BEEP_DEFAULT);
	else 	{
		view->mod_seq = view->cursor_seq;
		set_and_show_new_cursor_site(view, 
			view->cursor_site + num, FALSE, TRUE);
		}
	}
else	return;
if(view->multipl->argument() > 0) mod_multipl(view, 0);
}


Fl_Group *create_dna_scroller(SEA_VIEW *view, int x, int y, int w, int h, 
	int double_buffer)
{
  Fl_Group *dna_group;
  Fl_Widget *obj;
  int wmultipl, x_pos;
  user_data_plus *data;
dna_group = new Fl_Group(x,y,w,h);
int scroll_w = 15;
#ifdef __APPLE__
#define DELTA_G  13 // laisser place pour poignée de dimensionnement de fenetre
#else
#define DELTA_G  0
#endif
/* screen move haut */
view->up_screen_move = obj = 
(Fl_Widget*)new Fl_Repeat_Button(x+3, y,scroll_w,scroll_w,"@8>>");
obj->labeltype(FL_SYMBOL_LABEL);
data = new user_data_plus;
data->p = view;
data->value = 5;
obj->callback(lrdu_button_callback, data);
/* ascenc. vertical */
view->vertsli =  new Fl_Scrollbar(x+3, y + scroll_w + 2, scroll_w, 
		h - 3*scroll_w - 12, "");
view->vertsli->box(FL_DOWN_BOX);
data = new user_data_plus;
data->p = view;
data->value = 0;
((Fl_Scrollbar *)view->vertsli)->linesize(1);
view->vertsli->callback(vh_sliders_callback, data);
  view->vertsli->bounds(1,1);
  view->vertsli->slider_size(1);
  view->vertsli->Fl_Slider::value(1);
  view->vertsli->step(1);
view->vertsli->when(FL_WHEN_CHANGED);
/* screen move bas */
view->down_screen_move = obj = 
(Fl_Widget*)new Fl_Repeat_Button(x+3,
	y + h - 2 * scroll_w - 8, scroll_w, scroll_w, "@2>>");
obj->labeltype(FL_SYMBOL_LABEL);
data = new user_data_plus;
data->p = view;
data->value = 7;
obj->callback(lrdu_button_callback, data);

int y_scroll = y + h - scroll_w - 3;
/* valeur du multiplicateur */
fl_font(FL_HELVETICA, FL_NORMAL_SIZE);
  wmultipl = (int)fl_width("mult=9999");
  x_pos = 5;
  view->multipl = obj = (Fl_Widget*)new Fl_Box(FL_FLAT_BOX,
	x+x_pos, y_scroll - 2, wmultipl, FL_NORMAL_SIZE + 4, "");
  x_pos += wmultipl + 5;
  obj->labelfont(FL_HELVETICA);
  obj->labelsize(FL_NORMAL_SIZE);
  obj->align(FL_ALIGN_CENTER);


/* screen move gauche */
  view->left_screen_move = obj = 
(Fl_Widget*)new Fl_Repeat_Button(x+x_pos,y_scroll,scroll_w,scroll_w,"@<<");
obj->labeltype(FL_SYMBOL_LABEL);
  x_pos += scroll_w + 2;
data = new user_data_plus;
data->p = view;
data->value = 1;
obj->callback(lrdu_button_callback, data);
/* ascens. horizontal */ 
view->horsli =  
new Fl_Scrollbar(x + x_pos, y_scroll, w - x_pos - scroll_w - 2 - DELTA_G, scroll_w,"");
view->horsli->type(FL_HORIZONTAL);
view->horsli->box(FL_DOWN_BOX);
((Fl_Scrollbar *)view->horsli)->linesize(1);
data = new user_data_plus;
data->p = view;
data->value = 1;
view->horsli->callback(vh_sliders_callback, data);
view->horsli->bounds(1,1);
view->horsli->slider_size(1);
view->horsli->Fl_Slider::value(1);
view->horsli->step(1);
view->horsli->when(FL_WHEN_CHANGED);
/* screen move a droite */
  view->right_screen_move = obj = 
(Fl_Widget*)new Fl_Repeat_Button(x+w - scroll_w - DELTA_G, y_scroll, scroll_w, scroll_w,"@>>");
obj->labeltype(FL_SYMBOL_LABEL);
data = new user_data_plus;
data->p = view;
data->value = 3;
obj->callback(lrdu_button_callback, data);

/* noms + sequences */
Fl_Window *viewer;
if(double_buffer)
	  viewer = (Fl_Window *)new Fl_Double_Window(
					x+25-3, y, w - 25 - 1 + 6, h - 30 + 6);
else
	  viewer = new Fl_Window(x+25, y+3, w - 25 - 1, h - 30);
viewer->box(FL_DOWN_BOX);
viewer->resizable(viewer);
view->double_buffer = double_buffer;
view->DNA_obj = (Fl_Widget*)new DNA_obj(3, 3, w - 25 - 1, h - 30, view);
view->DNA_obj->labelfont(FL_COURIER_BOLD);
viewer->end();

dna_group->end();
Fl_Box *resizable_box =	new Fl_Box(x + x_pos, y + scroll_w + 2, w - x_pos - scroll_w - 2 - DELTA_G, 
		h - 3*scroll_w - 12);
dna_group->add_resizable(*resizable_box);
resizable_box->hide();
return dna_group;
}


SEA_VIEW *create_the_form(int double_buffer)
{
Fl_Group *dna_group;
Fl_Window *my_form;
Fl_Widget *obj, *bouton_search, *champ_search, *bouton_goto, *champ_goto, *bouton_help;
int black_and_white, inverted;
SEA_VIEW *view;
user_data_plus *data;
int labelSize = 12;
int borderWidth = 2;
int i, w_w, w_h, back_color, region_back_color, defaultformat;
char *win_size, *save_format;
static int first = TRUE;
static int dnacolors[] = { FL_WHITE, FL_RED, FL_GREEN, FL_YELLOW, FL_BLUE };
static int numb_dnacolors = sizeof(dnacolors) / sizeof(int);
static char *customdnacolors;
static int *protcolors;
static int *codoncolors = NULL;
static char **possible_formats;
	
if(first) {
	first = FALSE;
	//DNA colors
	customdnacolors = get_res_value("dnacolors", "");
	prep_custom_colors(dnacolors, customdnacolors, numb_dnacolors);
	//protein colors
	static char stdcolorgroups[50], altcolorgroups[50], customprotcolors[300];
	strcpy(stdcolorgroups, get_res_value("stdcolorgroups", def_stdcolorgroups));
	strcpy(altcolorgroups, get_res_value("altcolorgroups", ""));
	strcpy(customprotcolors, get_res_value("protcolors", ""));
	protcolors = (int *)malloc(max_protcolors * sizeof(int));
	def_protcolors = (int *)malloc(max_protcolors * sizeof(int));
	for(i=0; i<max_protcolors; i++) def_protcolors[i] = 
		fl_rgb_color(def_protcolors_rgb[3*i], def_protcolors_rgb[3*i+1], def_protcolors_rgb[3*i+2]);
	
	memcpy(protcolors, def_protcolors, max_protcolors * sizeof(int) );
	int cur_protcolors = prep_custom_colors(protcolors, customprotcolors, max_protcolors);
	/* process resource-read stdcolorgroups and altcolorgroups */
	curr_color_choice = prep_aa_color_code(stdcolorgroups, altcolorgroups, 
					       cur_protcolors, &numb_stdprotcolors, &numb_altprotcolors);
	codoncolors = (int *)malloc(22 * sizeof(int));
//codoncolors[1..21] correspond to aas RLSTPAGVKNQHEDYCFIMW*
	decode_codon_colors(codoncolors);
	possible_formats = (char **)malloc(nbr_formats * sizeof(char *));
	for(i= 0; i < nbr_formats; i++) {
		possible_formats[i] = (char  *)malloc( strlen(f_format_names[i]) + 1 );
		strcpy(possible_formats[i], f_format_names[i]);
		minuscules(possible_formats[i]);
		}
	}
save_format = get_res_value("save", possible_formats[NEXUS_FORMAT]);
for(i = 0; i < nbr_formats; i++) 
	if(strcmp(save_format, possible_formats[i]) == 0) break;
if( i >= nbr_formats ) 
	defaultformat = NEXUS_FORMAT;
else
	defaultformat = i;
#ifdef MICRO
  black_and_white = FALSE;
#else
  black_and_white = (fl_xpixel(FL_BLACK) == fl_xpixel(FL_RED));
#endif
  if (black_and_white) inverted = FALSE;
  else inverted = bool_res_value("inverted", TRUE);
#ifdef WIN32
	back_color = FL_DARK1; region_back_color = 43;
#else
	back_color = FL_GRAY; region_back_color = FL_DARK2;
#endif
back_color = int_res_value("background", back_color);
region_back_color = int_res_value("sites_background", region_back_color);
win_size = get_res_value("window", "790x500");
sscanf(win_size, "%dx%d", &w_w, &w_h);
my_form = new Fl_Window( FL_min(w_w, Fl::w()), FL_min(w_h, Fl::h()-22) );
my_form->xclass(SEAVIEW_WINDOW);
#ifdef MICRO
int w_x = (Fl::w() - my_form->w())/2;
int w_y =  (Fl::h() - my_form->h())/2;
	{static int count = 0;
	w_x += count * 30;
	w_y += (count++) * 30;
	count = count % 5;
	}
my_form->position(w_x, w_y);
#endif
my_form->color(FL_LIGHT1);
my_form->label(progname);
my_form->box(FL_FLAT_BOX);

  view = (SEA_VIEW *)calloc(1, sizeof(SEA_VIEW));
  if(view == NULL) out_of_memory();
  
Fl_Pack *menus = new Fl_Pack(1,5,10,25); /* groupe fixe des menus */
menus->type(FL_HORIZONTAL);
menus->spacing(2);

#ifdef MICRO
  const int mbw = 415;
#else
  const int mbw = 438;
#endif
  Fl_Menu_Bar *menubar = new Fl_Menu_Bar(0,0, mbw, 30);
  menubar->user_data(view);
  menubar->box(FL_UP_BOX);
  view->menubar = menubar;
  /* menu File */
  static const Fl_Menu_Item fileitems_static[] = {
    {"Open", FL_COMMAND | 'o',file_menu_callback, 0, FL_MENU_DIVIDER},
    {"Open Mase", 0,file_menu_callback, 0, 0},
    {"Open Phylip", 0,file_menu_callback, 0, 0},
    {"Open Clustal", 0,file_menu_callback, 0, 0},
    {"Open MSF", 0,file_menu_callback, 0, 0},
    {"Open Fasta", 0,file_menu_callback, 0, 0},
    {"Open NEXUS", 0,file_menu_callback, 0, FL_MENU_DIVIDER},
    {"Import from DBs", 0,file_menu_callback, 0, FL_MENU_DIVIDER},
    {"Save", FL_COMMAND | 's',file_menu_callback, 0, FL_MENU_INACTIVE},
    {"Save as...", 0,file_menu_callback, 0, FL_MENU_INACTIVE},
    {"Save selection", 0,file_menu_callback, 0, FL_MENU_INACTIVE},
    {"Save prot alignmt", 0,file_menu_callback, 0, FL_MENU_INACTIVE},
    {"Save bootstrap replicates", 0,file_menu_callback, 0, FL_MENU_INACTIVE | FL_MENU_DIVIDER},
#if !defined(__APPLE__)
    {"Prepare "PDF_OR_PS"", 0, file_menu_callback, 0, 0},
    {""PDF_OR_PS" options...", 0, file_menu_callback, 0, FL_MENU_DIVIDER},
#else
    {"Prepare "PDF_OR_PS"", 0, file_menu_callback, 0, FL_MENU_DIVIDER},
#endif
    {"Concatenate", 0,file_menu_callback, 0, FL_MENU_DIVIDER},
    {"New window", FL_COMMAND | 'n', file_menu_callback, 0, 0},
    {"Close window", FL_COMMAND | 'w', file_menu_callback, 0, 0},
#if ! defined( __APPLE__) 
    {"Quit", FL_COMMAND | 'q', file_menu_callback, 0, 0 },
#endif
    {0}
  };
  Fl_Menu_Item *fileitems = new Fl_Menu_Item[sizeof(fileitems_static)/sizeof(Fl_Menu_Item)];
  memcpy(fileitems, fileitems_static, sizeof(fileitems_static));
  menubar->add("File", 0, 0, fileitems, FL_SUBMENU_POINTER);
  view->menu_file = fileitems; // to delete[] at window close

/* menu Edit */
  static const Fl_Menu_Item edititems_static [] = {
    {"Copy selected seqs", FL_COMMAND | 'c', edit_menu_callback, 0, FL_MENU_INACTIVE},
    {"Paste alignment data", FL_COMMAND | 'v', edit_menu_callback, 0, 0},
    {"Select All", FL_COMMAND | 'a', edit_menu_callback, 0, 0},
    {"Rename sequence", 0, edit_menu_callback, 0, 0},
    {"Edit comments", 0, edit_menu_callback, 0, 0},
    {"Edit sequence", 0, edit_menu_callback, 0, 0},
    {"Delete sequence(s)", 0, edit_menu_callback, 0, 0},
    {"Create sequence", 0, edit_menu_callback, 0, 0},
    {"Load sequence", 0, edit_menu_callback, 0, 0},
    {"Duplicate sequence", 0, edit_menu_callback, 0, 0},
    {"Complement sequence", 0, edit_menu_callback, 0, 0},
    {"Reverse sequence", 0, edit_menu_callback, 0, 0},
    {"Exchange Us and Ts", 0, edit_menu_callback, 0, 0},
    {"Dot plot", 0, edit_menu_callback, 0, 0},
    {"Consensus sequence", 0, edit_menu_callback, 0, 0},
    {"Del. gap-only sites", 0, edit_menu_callback, 0, 0},
    {"Set genetic code", 0, edit_menu_callback, 0, 0},
    {0}
  };
  Fl_Menu_Item *edititems = new Fl_Menu_Item[sizeof(edititems_static)/sizeof(Fl_Menu_Item)];
  memcpy(edititems, edititems_static, sizeof(edititems_static));
  menubar->add("Edit", 0, 0, edititems, FL_SUBMENU_POINTER);
  view->menu_edit = edititems; // to delete[] at window close
	
/* menu align */
  cre_align_menu(view);
	
/* menu Props */
  view->inverted_colors = inverted;
  view->consensus_threshold = 60;
  view->double_buffer = double_buffer;
  create_props_menu(view, curr_color_choice, view->inverted_colors, black_and_white, defaultfontsize);

/* menu regions */
  static const Fl_Menu_Item siteitems_static[] = {
    {"Create set", 0, regions_menu_callback, 0, FL_MENU_INACTIVE},
    {"Duplicate set", 0, regions_menu_callback, 0,FL_MENU_INACTIVE},
    {"Hide set", 0, regions_menu_callback, 0, FL_MENU_INACTIVE},
    {"Delete set", 0, regions_menu_callback, 0, FL_MENU_INACTIVE | FL_MENU_DIVIDER},
  };
  view->menu_sites = new vlength_menu(view->menubar, "Sites", siteitems_static, 4);

/* menu species */
  static const Fl_Menu_Item speciesitems_static[] = {
    {"Create group", 0, species_menu_callback, 0, FL_MENU_INACTIVE},
    {"Delete group", 0, species_menu_callback, 0, FL_MENU_INACTIVE | FL_MENU_DIVIDER},
  };
  view->menu_species = new vlength_menu(view->menubar, "Species", speciesitems_static, 2);
  
/* menu footers */
  const static Fl_Menu_Item footeritems_static[] = {
    {"Show footers", 0, footers_menu_callback, NULL, FL_MENU_INACTIVE},
    {"Create footer", 0, footers_menu_callback, NULL, 0},
    {"Delete footer", 0, footers_menu_callback, NULL, FL_MENU_INACTIVE},
    {0}
  };
  Fl_Menu_Item *footeritems = new Fl_Menu_Item[4];
  memcpy(footeritems, footeritems_static, sizeof(footeritems_static));
  menubar->add("Footers", 0, 0, footeritems, FL_SUBMENU_POINTER);
  view->menu_footers = footeritems;
  
  /* menu trees */
  const static Fl_Menu_Item treeitems_static[] = {
    {"Parsimony", 0, trees_callback, 0, 0},
    {"Distance methods", 0, trees_callback, 0, 0},
    {"PhyML", 0, trees_callback, 0, FL_MENU_DIVIDER},
    {"Import tree", 0, trees_callback, 0, 0},
    {"New tree window", 0, trees_callback, 0, FL_MENU_DIVIDER},
  };
  view->menu_trees = new vlength_menu(view->menubar, "Trees", treeitems_static, 5);

#define calc_width(nom) \
(fl_font(FL_HELVETICA, labelSize), (int)fl_width(nom) + 4 * borderWidth)
#define add_button(nom) new Fl_Button(0,0,calc_width(nom),25,nom)
  
  /* bouton search + champ Search */
bouton_search = obj = add_button("Search:");
obj->labelsize(labelSize);
data = new user_data_plus;
data->value = 0;
obj->callback(search_callback, data);
champ_search = obj = (Fl_Widget*)new Fl_Input(0,0, 80, 25, "");
((Fl_Input*)obj)->type(FL_NORMAL_INPUT);
((Fl_Input*)obj)->when(FL_WHEN_ENTER_KEY);
data = new user_data_plus;
data->value = 1;
champ_search->callback(search_callback, data);
data = (user_data_plus *)bouton_search->user_data();
data->p = champ_search;

/*  bouton + champ Goto */
bouton_goto = obj = add_button("Goto:");
obj->labelsize(labelSize);
data = new user_data_plus;
data->value = 0;
obj->callback(goto_callback, data);
champ_goto = obj = (Fl_Widget*)new Fl_Input(0, 0, 80, 25, "");
((Fl_Input*)obj)->type(FL_NORMAL_INPUT);
data = new user_data_plus;
data->value = 1;
champ_goto->callback(goto_callback, data);
data = (user_data_plus *)bouton_goto->user_data();
data->p = champ_goto;
((Fl_Input*)obj)->when(FL_WHEN_ENTER_KEY);

/* bouton help */
bouton_help = add_button("Help");
bouton_help->labelsize(labelSize);
bouton_help->callback(help_callback);
	
#undef add_button
#undef calc_width

menus->end();
menus->resizable(NULL);

/* tout le groupe scroller */
dna_group = create_dna_scroller(view, 0, 35, my_form->w() - 5, my_form->h() - 35, double_buffer);
my_form->resizable( dna_group );
my_form->end();
my_form->callback(mainwin_close_callback, view);
/* initialisation independantes des sequences */
static char movekeys[5];
strcpy(movekeys, get_res_value("movekeys", MOVEKEYS_DEFAULT));
view->movekeys = movekeys;
data = (user_data_plus *)champ_search->user_data();
data->p = view;
data = (user_data_plus *)champ_goto->user_data();
data->p = view;
view->format_for_save = (known_format)defaultformat; /* default format for saving */
view->tot_seqs = 0;
view->first_seq = 1; view->tot_sites = 1;
view->numb_gc = 1;
view->line_height = 1;
view->char_width = 1;
view->draw_names = -1;
view->mod_seq = 0;
view->mod_comment_line = 0;
view->consensus_allowgaps = FALSE;
view->consensus_allowiupac = FALSE;
view->dnawin = my_form;
view->max_seq_length = 0;
mod_multipl(view, 0);
view->spaces_in_fasta_names = bool_res_value("spacesinfastanames", FALSE);

if ( ! black_and_white) { 
	/* couleurs du fond pour seqs */
  	view->DNA_obj->color(back_color, region_back_color); 
	view->region_color = FL_WHITE;
	if(view->inverted_colors) {
		view->DNA_obj->labelfont( FL_COURIER);
		}
	view->allow_lower = bool_res_value("lowercase", 0);
	}
else	{ /* the Black and White case */
	numb_dnacolors = numb_stdprotcolors = numb_altprotcolors = 1;
	dnacolors[0] = FL_BLACK;
	/* couleur du fond pour seqs */
  	view->DNA_obj->color(FL_WHITE, FL_WHITE); 
	view->region_color = FL_BLACK;
	view->numb_gc = 1;
	view->curr_colors = dnacolors;
	view->allow_lower = TRUE;
	}
view->DNA_obj->parent()->color( view->DNA_obj->color() );
/* taille par defaut des lettres des sequences */
view->DNA_obj->labelsize(defaultfontsize);
  view->dnacolors = dnacolors;
view->numb_dnacolors = numb_dnacolors;
view->stdprotcolors = protcolors;
view->numb_stdprotcolors = numb_stdprotcolors;
view->altprotcolors = protcolors;
view->numb_altprotcolors = numb_altprotcolors;
view->codoncolors = codoncolors;
view->namecolor = FL_BLACK;
view->alt_colors = curr_color_choice;
view->clustal_options = NULL;
view->show_comment_lines = FALSE;
view->tot_comment_lines = 0;
view->phylipwidnames = int_res_value("phylipwidnames", 30);
#if !(defined(__APPLE__) || defined(WIN32)) && SEAVIEW_FLTK_VERSION < 133
  my_form->icon( (void*)seaview_X11_icon );
#endif
#if defined(__APPLE__) && 100*FL_MAJOR_VERSION + FL_MINOR_VERSION < 104
  add_windowmenuitem(NULL, my_form);
#endif
my_form->show();
#ifndef MICRO
my_form->hotspot(my_form);
#endif
#if !(defined(__APPLE__) && !defined(WIN32)) //for X11 only
  my_form->make_current(); // 3 lines so the Fl_Pack gets its true width
  menus->damage(FL_DAMAGE_ALL);
  ((Fl_Widget*)menus)->draw();
  int min_w = menus->x() + menus->w() + 5; //min_w = width of the menu bar
  if (min_w > my_form->w()) {//make sure window is as wide as menu bar
    my_form->size(min_w, my_form->h());
  }
#endif
view->DNA_obj->take_focus();
return view;
}



