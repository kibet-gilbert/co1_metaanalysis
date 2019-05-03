#include "seaview.h"
#include "pdf_or_ps.h"
#include "svg.h"
#include <ctype.h>
#include <time.h>
#ifndef WIN32
#include <unistd.h>
#endif

/* included functions */
int read_mase_seqs_header(const char *masefname, char ***pseq, char ***pseqname, 
		char ***pcomments, char **pheader, char **err_message);
int one_more_seq_found(int count1, char ***pseq, char ***pseqname, char ***pcomments);
int read_fasta_align(const char *fname, char ***pseq, char ***pseqname, 
		char ***pcomments, char **pheader, char **err_message, int spaces_in_names);
int read_phylip_align(const char *fname, char ***pseq, char ***pseqname, 
		char ***pcomments, char **pheader, char **err_message);
int read_clustal_align(const char *fname, char ***pseq, char ***pseqname, 
		char ***pcomments, char **pheader, char **err_message);
int read_msf_align(const char *fname, char ***pseq, char ***pseqname, 
		char ***pcomments, char **pheader, char **err_message);
int is_a_protein_seq(char *seq);
int save_fasta_file(const char *fname, char **seq, char **comments,
	char **seqname, int totseqs, int *eachlength, region *region_used,
	int *sel_seqs, int tot_sel_seqs, int spaces_in_names, int pad_to_max_length);
int save_phylip_file(const char *fname, char **seq,
	char **seqname, int totseqs, int *eachlength, region *region_used, 
	int *sel_seqs, int tot_sel_seqs, int phylipwidnames);
int output_next_res_from_region(char *seq, int lenseq, 
	list_segments **segment, int *current, FILE *out, int total, 
	int use_dots);
void save_regions(list_regions *regions, FILE *out);
int save_mase_file(const char *fname, char **seq, char **comments,
	char *header, char **seqname, int totseqs, int *eachlength,
	list_regions *regions, region *region_used, int numb_species_sets,
	int **list_species_sets, char **name_species_sets,
	int *sel_seqs, int tot_sel_seqs, int tot_comment_lines, 
	char **comment_name, char **comment_line, 
	int tot_trees, char **trees, const Fl_Menu_Item *menu_tree_items);
int save_clustal_file(const char *fname, char **seq,
	char **seqname, int totseqs, int *eachlength, region *region_used,
	int *sel_seqs, int tot_sel_seqs);
int calc_gcg_check(list_segments *psegment, char *seq);
int save_msf_file(const char *fname, char **seq,
	char **seqname, int totseqs, int *eachlength, region *region_used,
	int protein, int *sel_seqs, int tot_sel_seqs);
char *save_alignment_or_region(const char *fname, char **seq, char **comments,
	char *header, char **seqname, int totseqs, int *eachlength,
	list_regions *regions, region *region_used, known_format format,
	int numb_species_sets, int **list_species_sets, 
	char **name_species_sets, int *sel_seqs, int tot_sel_seqs, int protein,
	int tot_comment_lines, char **comment_name, char **comment_line, int phylipwidnames, 
	int tot_trees, char **trees, const Fl_Menu_Item *menu_tree_items, int spaces_in_fasta_names);
char *get_full_path(const char *fname);
static void save_species_sets(int numb_species_sets, int **list_species_sets, 
	char **name_species_sets, int totseqs, FILE *out);
void save_comment_lines(int tot_comment_lines, char **names, char **lines, 
	FILE *out);
known_format what_format(const char *filename);
char* seaview_file_chooser_save_as(const char* message, const char* fname, SEA_VIEW *view, known_format* new_format);
const char *extract_dirname(const char *pathname);
int printout(SEA_VIEW *view, const char *filename, 
	     int fontsize, int block_size, Fl_Paged_Device::Page_Format pageformat, int vary_only, int ref0, 
	     int pdfkindvalue, Fl_Paged_Device::Page_Layout layout, int svg_width = 0);
static void color_pdf_display(SEA_VIEW *view, int (*calc_color_function)( int ), char *oneline, 
			      int widnames, double x, double y, int fontsize, double char_width, double descender,int num, int current);
static void color_svg_display(SEA_VIEW *view, int (*calc_color_function)( int ), char *oneline, 
			      int widnames, double x, double y, int fontsize, double char_width);
static int calc_vary_lines(int *vary_pos, int widpos);
static void out_vary_pos(int *vary_pos, int widnames, int widpos, int nl, FILE *textfile, double x, double y);
SEA_VIEW* read_alignment_file(const char *infile);
SEA_VIEW *cmdline_read_input_alignment(int argc, char **argv);
void format_conversion(int argc, char **argv);
char *process_output_options(int argc, char **argv, known_format& out_format, bool& std_output);
#ifndef NO_PDF
void printout_cmdline(int argc, char **argv);
#endif


/* external */
extern char *f_format_names[];
extern char *f_format_exts[];
extern int nbr_formats;
extern float argval(int argc, char *argv[], const char *arg, float defval);
extern int calc_max_seq_length(int seq_length, int tot_seqs);
extern int max_protcolors;
extern char def_stdcolorgroups[];
extern int def_protcolors_rgb[];
char *get_res_value(const char *name, const char *def_value);
extern int prep_custom_colors(int *colors, char *customcolors, int max_colors);
extern color_choice prep_aa_color_code(char *list_std, char *list_alt, 
				       int maxprotcolors, int *numb_stdprotcolors, int *numb_altprotcolors);
extern void load_resources(const char *progname);
extern void allonge_seqs(char **seq, int totseqs, int maxlen, int *eachlength,
			 int tot_comment_lines, char **comment_line, char **pregion_line);
extern int int_res_value(const char *name, int def_value);
extern const char *progname;
#if defined(__APPLE__)
extern const char *MG_GetBundleResourcesDir(void);
#else
extern char *get_prog_dir(void); 
#endif


extern int save_nexus_file(const char *fname, int ntaxa, int protein,
	char **seqs, char **taxnames, char **notes, char *header,
	int num_species_sets, int **list_species_sets, 
	char **name_species_sets,
	list_regions *charsets,
	int tot_comment_lines, char **comment_name, char **comment_line,
	region *region_used, int *sel_seqs, int tot_sel_seqs, int *eachlength,
	int tot_trees, char **trees, const Fl_Menu_Item *items);
extern char *my_fgets(char *s, int n, FILE *f);
extern char *argname(int argc, char *argv[], const char *arg);
extern int isarg(int argc, char *argv[], const char *arg);
extern char *create_tmp_filename(void);
extern void delete_tmp_filename(const char *base_fname);
extern char *translate_with_gaps(char *seq, int gc);
extern int get_ncbi_gc_from_comment(char *comment);
extern int create_gblocks_mask(SEA_VIEW *view, region *myregion, int no_gui, int b5_val, int b4_val, int b3_val, int b2_val);
extern char *back_translate_with_gaps(char *prot, char *dna);
extern void del_gap_only_sites(SEA_VIEW *view);
extern void save_bootstrap_replicates(const char *fname, int replicates, SEA_VIEW *view);
extern "C" {
  int get_acnuc_gc_number(int ncbi_gc);
  int get_ncbi_gc_number(int ncbi_gc);
}


int read_mase_seqs_header(const char *masefname, char ***pseq, char ***pseqname, 
		char ***pcomments, char **pheader, char **err_message)
{
#define MAXLENSEQ 10000 /* unite d'allocation de memoire */
#define lline  2000
FILE *masef;
char line[lline], *i, *base, *header = NULL, *provseq = NULL, *p;
int  l, lenseqs, lpre, lseq, l2, totseqs = -1, want_header, curr_max_header;
static char ret_message[200];
char **seq, **seqname, **comments;

*ret_message = 0;
*err_message = ret_message;
if( (masef=fopen(masefname,"r")) == NULL) {
	sprintf(ret_message,"File not found:%s",masefname);
	return 0;
	}
want_header = (pheader != NULL);

if(fgets(line, lline, masef)==NULL)goto fini;
if(strchr(line, '\n') == NULL) {
	strcpy(ret_message,"Not a mase file!");
	goto fini;
	}
if(strncmp(line,";;",2)==0) {
	if(want_header) {
		if( (header=(char *)malloc(MAXLENCOM+1)) ==
							 NULL)goto nomem;
		curr_max_header = MAXLENCOM;
		strcpy(header,line);
		lpre=strlen(line);
		}
	do	{
		if( fgets(line,lline,masef)==NULL ) goto fini;
		if(strncmp(line,";;",2)!=0) break;
		if(header != NULL) {
			lseq=strlen(line);
			if(lpre+lseq > curr_max_header) {
				curr_max_header += MAXLENCOM;
				if( (p=(char *)malloc(curr_max_header+1)) 
					== NULL ) goto nomem;
				memcpy(p, header, lpre);
				free(header);
				header = p;
				}
			memcpy(header+lpre,line, lseq);
			lpre += lseq;
			}
		}
	while (1);
	if( want_header ) {
		header[lpre] = 0;
		header=(char *)realloc(header,lpre+1);
		}
	}
if(*line != ';' ) {
	strcpy(ret_message,"Not a mase file!");
	goto fini;
	}

lenseqs=MAXLENSEQ;
if( (provseq=(char *)malloc(lenseqs+1)) ==NULL)goto nomem;

i=line;
while(i!=NULL){
	totseqs = one_more_seq_found(totseqs, &seq, &seqname, &comments);
	if(totseqs == -1) goto nomem;
	if(comments!=NULL) {
		if( (comments[totseqs]=(char *)malloc(MAXLENCOM+1)) ==
							 NULL)goto nomem;
		strcpy(comments[totseqs],line);
		lpre=strlen(line); l=MAXLENCOM;
		while(*fgets(line,lline,masef)==';') {
			lseq=strlen(line);
			if(lpre+lseq <= l) {
				strcpy(comments[totseqs]+lpre,line);
				lpre += lseq;
				}
			else l=lpre-1;
			}
		if(lpre<MAXLENCOM)
		   comments[totseqs]=(char *)realloc(comments[totseqs],lpre+1);
		}
	else	while(*fgets(line,lline,masef)==';');
	l = strlen(line);
	while((line[l-1] == ' ' || line[l-1] == '\n') && l>0 ) l--; line[l] = 0;
	if( (seqname[totseqs]=(char *)malloc(l+1)) == NULL)goto nomem;
	strcpy(seqname[totseqs],line);
	lseq = 0; /* what is already put in provseq */
	while( (i=fgets(line,lline,masef))!= NULL && *i != ';' ) {
	    	l2 = strlen(line);
		if( line[l2 - 1] == '\n' ) l2--;
	   	while(l2>0 && line[l2-1]==' ')l2--;
		if(lseq + l2 > lenseqs) {
			char *temp;
			lenseqs += MAXLENSEQ;
			temp = (char *)malloc(lenseqs+1);
			if(temp == NULL) goto nomem;
			memcpy(temp, provseq, lseq);
			free(provseq);
			provseq = temp;
			}
	   	memcpy(provseq+lseq, line, l2);
		lseq += l2;
		}
	provseq[lseq]='\0';
	seq[totseqs] = (char *)malloc(lseq+1);
	if(seq[totseqs] == NULL) goto nomem;
/* ignore space or non printable characters */
	base=provseq - 1; p = seq[totseqs] - 1;
	while ( *(++base) != 0) {
		if(isprint(*base) && ! isspace(*base) ) {
//			*(++p) = toupper(*base);
			*(++p) = *base;
			}
		}
	*(++p) = 0;
	}
seq = (char **)realloc(seq, (totseqs + 1)*sizeof(char *));
seqname = (char **)realloc(seqname, (totseqs + 1)*sizeof(char *));
comments = (char **)realloc(comments, (totseqs + 1)*sizeof(char *));
*pseq = seq; *pseqname = seqname; *pcomments = comments;
fini:
fclose(masef);
if(want_header) *pheader = header;
if(provseq != NULL) free(provseq);
return totseqs+1;
nomem:
sprintf(ret_message,"Error: Not enough memory!");
totseqs = -1;
goto fini;
}


void parse_trees_from_header(char *header, SEA_VIEW *view)
{
	char *new_header, *old_header, *fin_new_header, *p, *q;
	int l_header, l;
	if(header == NULL) return;
	old_header = header;
	l_header=strlen(header);
	if( (new_header = (char *)malloc(l_header+1)) == NULL) out_of_memory();
	fin_new_header = new_header;
	*new_header = 0;
	while (*header!= 0) {
		if(strncmp(header,";;$",3) == 0) {
			p = header + 3;
			while(*p == ' ') p++;
			q = strchr(p, '\n');
			*q = 0;
		  view->menu_trees->add(p, trees_callback, NULL, 0);
		  int rank = view->menu_trees->vlength();
		  (view->menu_trees->vitem(rank - 1))->labelfont(FL_HELVETICA_ITALIC);
			*q = '\n';
			p = q + 1;
			l = 1;
			while(TRUE) {
				q = strchr(p, '\n');
				if(strncmp(q+1, ";;", 2) != 0) break;
				if (*(q+3) == '$' && *(q+4) != '\n') break;
				if (strncmp(q+1, ";;@ of species =", 16) == 0) break;
				if (strncmp(q+1, ";;# of segments=", 16) == 0) break;
				p = q + 1;
				l++;
				}
			if(view->tot_trees == 0) view->trees = (char **)malloc(sizeof(char *));
			else view->trees = (char **)realloc(view->trees, (view->tot_trees + 1) * sizeof(char *));
			view->trees[view->tot_trees] = (char *)malloc(q - header + 1);
			p = view->trees[view->tot_trees];
			for(int i = 0; i < l; i++) {
				header = strchr(header, '\n') + 1;
				q = (char *)memccpy(p, header + 2, '\n', l_header);
				p += (q - p - 1);
				}
			*p = 0;
			view->tot_trees++;
		}
		else	{
			p=(char *)memccpy(fin_new_header, header, '\n', l_header);
			fin_new_header += (p - fin_new_header);
		}
		header = strchr(header,'\n') + 1;
	}
	*fin_new_header = 0;
	strcpy(old_header, new_header);
	free(new_header);
}


int one_more_seq_found(int count1, char ***pseq, char ***pseqname, char ***pcomments)
{
static int max_count;
char **seq, **seqname, **comments;

if(count1 == -1) max_count = 0;

if(count1 + 1 < max_count) return count1 + 1;

count1++;
if(max_count == 0) {
	max_count = 100;
	seq = (char **)malloc(max_count * sizeof(char *));
	if(seq == NULL) return -1;
	seqname = (char **)malloc(max_count * sizeof(char *));
	if(seqname == NULL) return -1;
	comments = (char **)malloc(max_count * sizeof(char *));
	if(comments == NULL) return -1;
	}
else {
	seq = *pseq; seqname = *pseqname; comments = *pcomments;
	max_count = 3 * max_count;
	seq = (char **)realloc(seq, max_count * sizeof(char *));
	if(seq == NULL) return -1;
	seqname = (char **)realloc(seqname, max_count * sizeof(char *));
	if(seqname == NULL) return -1;
	comments = (char **)realloc(comments, max_count * sizeof(char *));
	if(comments == NULL) return -1;
	}

*pseq = seq; *pseqname = seqname; *pcomments = comments;
return count1;
}


int read_fasta_align(const char *fname, char ***pseq, char ***pseqname, 
		char ***pcomments, char **pheader, char **err_message, int spaces_in_names)
{
FILE *in;
int totseqs, lseq, l2, l, lenseqs;
char line[300], *p, *i, c, *q, *r;
static char ret_message[200];
char **seq, **seqname, **comments, *tmpseq = NULL;

*ret_message = 0;
*err_message = ret_message;
if( (in=fopen(fname,"r")) == NULL) {
	sprintf(ret_message,"File not found:%s", fname);
	return 0;
	}

/* calcul du nombre de sequences dans le fichier */
totseqs = 0;
while(fgets(line, sizeof(line), in) != NULL) {
	if(*line == '>') totseqs++;
	}
rewind(in);
seq = (char **)malloc(totseqs * sizeof(char *));
if(seq == NULL) goto nomem;
comments = (char **)malloc(totseqs * sizeof(char *));
if(comments == NULL) goto nomem;
seqname = (char **)malloc(totseqs * sizeof(char *));
if(seqname == NULL) goto nomem;
*pseq = seq; *pcomments = comments; *pseqname = seqname;

lenseqs = MAXLENSEQ;
tmpseq = (char *)malloc(lenseqs + 1);
if(tmpseq == NULL) goto nomem;
totseqs = -1;
i = fgets(line, sizeof(line), in);
if(line[0] != '>') {
	strcpy(ret_message,"File not in Fasta format!");
	totseqs = -1; goto fini;
	}
while( i != NULL ){
	/* finish reading very long title line */
	c = line[strlen(line) - 1];
	while(c != '\n' && c != '\r' && c != EOF) c = getc(in);
	q = line + strlen(line) - 1;
	while(q > line + 1 && (*q == '\n' || *q == '\r')) *(q--) = 0;
	totseqs++;
	p = line + 1; 
  while (*p == ' ') p++;
	if(spaces_in_names) {
	  while(*p != '\n') p++;
	  while(*(p-1) == ' ') p--;
	  }
	else {
	  while(*p != ' ' && *p != '\n') p++;
	  }
  r = line + 1;
  while (*r == ' ') r++;
	l = p - r;
	if( (seqname[totseqs] = (char *)malloc(l+1)) == NULL)goto nomem;
	memcpy(seqname[totseqs], r, l); seqname[totseqs][l] = 0;
	/* use rest of title line, if any, as comment */
	while(*p == ' ') p++;
	l = q - p + 1;
	if( l > 0) {
		comments[totseqs] = (char *)malloc(l + 3);
		if(comments[totseqs] != NULL) {
			strcpy(comments[totseqs], ";");
			strcpy(comments[totseqs] + 1, p);
			strcpy(comments[totseqs] + l + 1, "\n");
			}
		}
	else comments[totseqs] = NULL;
	lseq = 0;
	while( (i=fgets(line, sizeof(line), in))!= NULL && *i != '>' ) {
		l2 = strlen(line);
		if( line[l2 - 1] == '\n' ) l2--;
	   	while(l2>0 && line[l2-1]==' ')l2--;
		if(lseq + l2 > lenseqs) {
			lenseqs += MAXLENSEQ;
			tmpseq= (char *)realloc(tmpseq, lenseqs + 1);
			if(tmpseq == NULL) goto nomem;
			}
/* copy seq data excluding spaces (because of gblocks) */
		p = tmpseq+lseq;
		q = line;
		while (q < line + l2) {
			if(*q != ' ') *(p++) = *q;
			q++;
			}
		lseq += p - (tmpseq+lseq);
		}
	tmpseq[lseq]='\0';
	seq[totseqs] = (char *)malloc(lseq + 1);
	if(seq[totseqs] == NULL) goto nomem;
	memcpy(seq[totseqs], tmpseq, lseq + 1);
	}
fini:
fclose(in);
if(tmpseq != NULL) free(tmpseq);
*pheader = NULL;
return totseqs+1;
nomem:
sprintf(ret_message,"Error: Not enough memory!");
totseqs = -1;
goto fini;
}


int read_phylip_align(const char *fname, char ***pseq, char ***pseqname, 
		char ***pcomments, char **pheader, char **err_message)
{
FILE *in;
char *p, *q;
int c;
static char line[300];
char **seq=0, **comments=0, **seqname=0;
int totseqs, lenseqs, i, l;
static char ret_message[200];
*ret_message = 0;
*err_message = ret_message;
in=fopen(fname,"r");
if(in==NULL) {
	sprintf(ret_message,"File not found:%s",fname);
	return 0;
	}
fgets(line,sizeof(line),in);
if( sscanf(line, "%d%d", &totseqs, &lenseqs) != 2) {
		sprintf(ret_message,"Not a PHYLIP file");
		totseqs = 0;
		goto fini;
		}
seq = (char **)malloc(totseqs * sizeof(char *));
if(seq == NULL) goto nomem;
seqname = (char **)malloc(totseqs * sizeof(char *));
if(seqname == NULL) goto nomem;
comments = (char **)malloc(totseqs * sizeof(char *));
if(comments == NULL) goto nomem;
for(i=0; i<totseqs; i++) {
	if( (seq[i] = (char *)malloc(lenseqs+1) ) == NULL ) goto nomem;
	comments[i] = NULL;
	}
for(i=0; i<totseqs; i++) {
  fgets(line,sizeof(line),in);
  p = strstr(line, " "); if(p == NULL) p = line + 10;
  if( (seqname[i] = (char *)malloc(p - line + 1) ) == NULL ) goto nomem;
  memcpy(seqname[i], line, p - line); seqname[i][p - line] = 0;
  q = seq[i];
  while(*p != 0 && *p != '\n') {
	  if(*p != ' ') {
		if(q - seq[i] >= lenseqs) goto badfile;
		*(q++) = *p;
		}
	  p++;
	  }
  c = *p;
  if(c == '\n') continue;
  while(TRUE) {
	c = fgetc(in);
	if(c == EOF) goto badfile;
	if(c == '\n' || c == '\r') break;
	if(c != ' ') {
	  if(q - seq[i] >= lenseqs) goto badfile;
	  *(q++) = (char)c;
	  }
	}
  if(c == '\r') {c = fgetc(in); if(c != '\n') ungetc(c, in); }
  }
l = q - seq[totseqs - 1];
while( l < lenseqs) {
  do c = fgetc(in); while(c != '\n' && c != '\r' && c != EOF);
  if(c == EOF) goto badfile;
  if(c == '\r') {c = fgetc(in); if(c != '\n') ungetc(c, in); }
  for(i=0; i<totseqs; i++) {
	q = seq[i] + l;
	while(TRUE) {
	  c = fgetc(in);
	  if(c == EOF) goto badfile;
	  if(c == '\n' || c == '\r') break;
	  if(c != ' ') {
		if(q - seq[i] >= lenseqs) goto badfile;
		*(q++) = (char)c;
		}
	  }
	if(c == '\r') {c = fgetc(in); if(c != '\n') ungetc(c, in); }
	}
  l = q - seq[totseqs - 1];
  }
for(i=0; i<totseqs; i++) seq[i][l] = 0;
fini:
*pheader = NULL;
fclose(in);
*pseq = seq; *pseqname = seqname; *pcomments = comments;
return totseqs;
nomem:
	sprintf(ret_message,"Not enough memory!");
	totseqs = 0;
	goto fini;
badfile:
	sprintf(ret_message,"Bad file format");
	totseqs = 0;
	goto fini;
}


int read_clustal_align(const char *fname, char ***pseq, char ***pseqname, 
		char ***pcomments, char **pheader, char **err_message)
{
FILE *in;
char line[200], *p;
int i, l, curr_spec, first=TRUE, curr_len, next_len, tot_spec, curr_max_len,
	carac, wid_name;
static char ret_message[200];
char **seq, **comments, **seqname = NULL;

*ret_message = 0;
*err_message = ret_message;
in=fopen(fname,"r");
if(in==NULL) {
	sprintf(ret_message,"File not found:%s",fname);
	return 0;
	}
fgets(line,sizeof(line),in);
if(strncmp(line,"CLUSTAL",7) != 0) { /* skip 1st line with CLUSTAL in it */
	strcpy(ret_message,"File not in CLUSTAL format!");
	tot_spec = -1; goto fini;
	}
/* skip next empty lines */
do	{
	carac = getc(in);
	if(carac == ' ') {
		fgets(line,sizeof(line),in);
		carac = getc(in);
		}
	}
while(carac == '\n' || carac == '\r');
ungetc(carac, in); /* back to start of 1st non-empty line */
tot_spec = curr_spec = -1; curr_len = next_len = 0;
while( fgets(line, sizeof(line), in) != NULL ) {
	if(*line == '\n' || *line == ' ') {
		curr_spec = -1;
		curr_len = next_len;
		first = FALSE;
		continue;
		}
	else if(tot_spec >= 0 && curr_spec == -1 &&
		strncmp(line, seqname[0], strlen(seqname[0]) ) != 0) {
		break;
		}
	else {
		if(first) {
			curr_spec = one_more_seq_found(curr_spec, &seq, &seqname, &comments);
			if(curr_spec == -1) goto nomem;
			}
		else	curr_spec++;
		}
	if(first && curr_spec == 0) {
/* calcul long partie nom: enlever tout ce qui n'est pas espace en fin */
		p = line + strlen(line) - 2; 
		while(*p == ' ' || isdigit(*p) ) p--; 
		while (*p != ' ') p--;
		wid_name = p - line + 1;
		}
	if(first) {
		seqname[curr_spec] = (char *)malloc(wid_name+1);
		if(seqname[curr_spec]==NULL) {
			goto nomem;
			}
		memcpy(seqname[curr_spec], line, wid_name);
		p = seqname[curr_spec] + wid_name - 1;
		while(*p==' ') p--; *(p+1)=0;
		if(curr_spec > tot_spec) tot_spec = curr_spec;
		seq[curr_spec] = (char *)malloc(CLU_BLOCK_LEN+1);
		curr_max_len = CLU_BLOCK_LEN;
		if(seq[curr_spec]==NULL) {
			goto nomem;
			}
		comments[curr_spec] = NULL;
		}
	if(curr_spec == 0) {
		l = strlen(line) - 1;
		p = line + l - 1; 
		while(*p == ' ' || isdigit(*p) ) { p--; l--; }
		l -= wid_name;
		if(curr_len + l > curr_max_len) {
			curr_max_len += CLU_BLOCK_LEN;
			for(i=0; i<=tot_spec; i++) {
				p = (char *)malloc(curr_max_len+1);
				if(p == NULL) goto nomem;
				memcpy(p, seq[i], curr_len);
				free(seq[i]);
				seq[i] = p;
				}
			
			}
		next_len = curr_len + l;
		}
	memcpy(seq[curr_spec]+curr_len, line + wid_name, l);
	}
for(i=0; i<=tot_spec; i++) seq[i][next_len] = 0;
seq = (char **)realloc(seq, (tot_spec + 1)*sizeof(char *));
seqname = (char **)realloc(seqname, (tot_spec + 1)*sizeof(char *));
comments = (char **)realloc(comments, (tot_spec + 1)*sizeof(char *));
*pseq = seq; *pseqname = seqname; *pcomments = comments;
fini:
*pheader = NULL;
fclose(in);
return tot_spec + 1;
nomem:
sprintf(ret_message,"Error: Not enough memory!");
tot_spec = -1;
goto fini;
}


int read_msf_align(const char *fname, char ***pseq, char ***pseqname, 
		char ***pcomments, char **pheader, char **err_message)
{
FILE *in;
char line[100], *p, *q;
int l, curr_spec, maxwidname=0, curr_len, tot_spec, wid_1_line, wid_block;
static char ret_message[200];
char **seq, **seqname, **comments;

*ret_message = 0;
*err_message = ret_message;
in=fopen(fname,"r");
if(in==NULL) {
	sprintf(ret_message,"File not found:%s",fname);
	return 0;
	}

/* compter le nbre de seqs dans le fichier */
tot_spec = 0;
while(fgets(line, sizeof(line), in) != NULL) {
	if(strncmp(line, "//", 2) == 0) break;
	if(strstr(line, "Name: ") != NULL) tot_spec++;
	}
rewind(in);
seq = (char **)malloc(tot_spec * sizeof(char *));
if(seq == NULL) goto nomem;
comments = (char **)malloc(tot_spec * sizeof(char *));
if(comments == NULL) goto nomem;
seqname = (char **)malloc(tot_spec * sizeof(char *));
if(seqname == NULL) goto nomem;
*pseq = seq; *pcomments = comments; *pseqname = seqname;

p = NULL;
while( fgets(line,sizeof(line),in) != NULL) {
	if( (p = strstr(line, "MSF: ")) != NULL) break;
	}
if(p == NULL || tot_spec == 0) {
	strcpy(ret_message,"File not in MSF format!");
	tot_spec = -1; goto fini;
	}
tot_spec = -1;
do	{
	fgets(line,sizeof(line),in);
	if( (p = strstr(line, "Name:") ) == NULL) continue;
	tot_spec++;
	q = strstr(p, " Len: "); 
	sscanf(q + 5, "%d", &l);
	seq[tot_spec] = (char *)malloc(l + 1);
	if(seq[tot_spec]==NULL) goto nomem;
	p += 5; while(*p == ' ') p++;
	while(*q == ' ') q--;
	l = q - p + 1;
	seqname[tot_spec] = (char *)malloc(l + 1);
	if(seqname[tot_spec]==NULL) goto nomem;
	memcpy(seqname[tot_spec], p, l); seqname[tot_spec][l] = 0;
	if(l > maxwidname) maxwidname = l;
	comments[tot_spec] = NULL;
	}
while(strncmp(line, "//", 2) != 0);
curr_spec = 0; curr_len = 0; wid_block = 0;
while( fgets(line, sizeof(line), in) != NULL ) {
	p = line; while(*p == ' ') p++;
	l = strlen(seqname[curr_spec]);
	if(strncmp(p, seqname[curr_spec], l) != 0) continue;
	p += l; while(*p == ' ') p++; p--;
	q = seq[curr_spec] + curr_len;
	while( *(++p) != '\n') {
		if( *p == ' ') continue;
		if(*p == '.') *p = '-';
		*(q++) = *p;
		}
	*q = 0;
	wid_1_line = q - (seq[curr_spec] + curr_len);
	wid_block = (wid_1_line > wid_block ? wid_1_line : wid_block);
	if(curr_spec == tot_spec) {
		curr_len += wid_block;
		curr_spec = 0;
		wid_block = 0;
		}
	else	curr_spec++;
	}
fini:
*pheader = NULL;
fclose(in);
return tot_spec + 1;
nomem:
sprintf(ret_message,"Error: Not enough memory!");
tot_spec = -1;
goto fini;
}


int is_a_protein_seq(char *seq)
/* returns TRUE if seq looks like a protein sequence (less than 80% ACGTU) */
{
static char dna[]="ACGTURY";
int total=0, length=0;
while(*seq != 0) {
	if(*seq != '-' && *seq != '?' && toupper(*seq) != 'N') { 
		if( strchr(dna, toupper(*seq)) != NULL ) total++;
		length++; 
		}
	seq++;
	}
return ( (float)(total) / length ) <= 0.8 ;
}


int is_a_protein_alignment(SEA_VIEW *view)
/* returns TRUE if alignment looks like protein data (less than 80% ACGTU) */
{
  char *seq;
  for (int i = 0; i < view->tot_seqs; i++) {
    seq = view->sequence[i];
    while (*seq == '-') seq++;
    if (*seq != 0) return is_a_protein_seq(view->sequence[i]);
  }
  return false;
}

int save_phylip_file(const char *fname, char **seq,
	char **seqname, int totseqs, int *eachlength, region *region_used, 
	int *sel_seqs, int tot_sel_seqs, int phylipwidnames)
/* sauver des sequences ou des regions au format phylip
region_used pointe vers la region a sauver
si region_used == NULL, toutes les sequences sont sauvees
rend 0 si ok, 
	1 si erreur d'ecriture dans le fichier
	2 si tentative de depasser la longueur d'une sequence
*/
{
const int widphylin = 60;
FILE *out;
int lenseqs, i, j, retval = 1, current, save_current, err, vtotseqs, lu, maxlname;
list_segments *psegment, all_sequence, *curr_segment;
region maregion;
char c;

if(totseqs == 0) return 0;
if( (out = fopen(fname,"w")) == NULL) return 1;
if(region_used == NULL) { /* on veut tout sauver */
	tot_sel_seqs = 0;
	all_sequence.debut = 1;
	all_sequence.fin = eachlength[0];
	for(i = 1; i < totseqs; i++) /* calcul long max des seqs */
		if( eachlength[i] > all_sequence.fin )  
			 all_sequence.fin = eachlength[i];
	all_sequence.next = NULL;
	maregion.list = &all_sequence;
	region_used = &maregion;
	}
/* calcul longueur des regions */
lenseqs = 0;
psegment = region_used->list;
while(psegment != NULL) {
	lenseqs += psegment->fin - psegment->debut + 1;
	psegment = psegment->next;
	}
/* longest seq name */
vtotseqs = 0;
maxlname = 10; // 10 is the minimum name length
for(i=0; i < totseqs; i++) {
	if(tot_sel_seqs == 0 || sel_seqs[i]) {
		++vtotseqs;
		if( (j = strlen(seqname[i])) > maxlname) maxlname = j;
		}
	}
if(maxlname > phylipwidnames) maxlname = phylipwidnames;
fprintf(out,"%d   %d\n", vtotseqs, lenseqs);
for(i=0; i < totseqs; i++) {
	if(tot_sel_seqs != 0 && ! sel_seqs[i]) continue;
	psegment = region_used->list; current = 0;
	for(j = 0; j < maxlname; j++) {
		if( (c = seqname[i][j]) == 0) break;
		putc(c == ' ' ? '_' : c, out);
		}
	while(j <= maxlname) { putc( ' ', out ); j++; }
	lu = 0;
	while(lu < widphylin && psegment != NULL) {
		putc( ' ', out );
		err = output_next_res_from_region(seq[i], eachlength[i], &psegment, 
			&current, out, 10, FALSE);
		lu += err;
		}
	putc('\n', out);
	if(ferror(out)) goto fin;
	}
while( psegment != NULL ) {
	putc('\n',out);
	curr_segment = psegment; save_current = current;
	for(i=0; i < totseqs; i++) {
		if(tot_sel_seqs != 0 && ! sel_seqs[i]) continue;
		psegment = curr_segment; current = save_current;
		for(j = 0; j <= maxlname; j++) putc( ' ', out );
		lu = 0;
		while(lu < widphylin && psegment != NULL) {
			putc( ' ', out );
			err = output_next_res_from_region(seq[i], eachlength[i], 
				&psegment, &current, out, 10, FALSE);
			lu += err;
			}
		putc('\n', out);
		if(ferror(out)) goto fin;
		}
	}
retval = 0;
fin:
fclose(out);
return retval;
}


int output_next_res_from_region(char *seq, int lenseq, 
	list_segments **segment, int *current, FILE *out, int total, 
	int use_dots)
/* ecrire dans le fichier out les total residus a partir de la position courante
dans une liste de regions. lenseq est la longueur de la sequence seq.
La position courante est determinee par le segment courant (*segment, qui peut
etre modifie par la fonction) et par la position (from 0) dans celui-ci (*current
qui est modifie par la fonction pour etre pret pour l'appel suivant).
Si le segment demande va au dela de la fin de la seq, des - sont ecrits.
Rend le nombre de residus effectivement ecrits, qui est tjrs celui demande.
Doit etre appelle la premiere fois avec *current = 0
*/
{
int debut, fin, vfin, ecrit = 0, nombre;
char *p;
static char line[500];
if( *segment == NULL) 
	return 0;
do	{
	debut = (*segment)->debut; fin = (*segment)->fin; 
	vfin = fin; if(fin > lenseq) vfin = lenseq;
	nombre = total;
	if( nombre - 1 + *current + debut > vfin)
		nombre = vfin + 1 - *current - debut;
	if(nombre > 0) {
		memcpy(line, seq + *current + debut - 1, nombre); 
		line[nombre] = 0;
		if(use_dots) {
			p = line;
			while( (p = strchr(p, '-')) != NULL) *p = '.';
			}
		fwrite(line, 1, nombre, out);
		ecrit += nombre; total -= nombre; (*current) += nombre;
		}
	if( fin > lenseq && total > 0 ) {
		nombre = total;
		if( nombre - 1 + *current + debut > fin)
			nombre = fin + 1 - *current - debut;
		ecrit += nombre; (*current) += nombre; total -= nombre;
		while(nombre-- > 0) putc('-', out); 
		}
	if( *current + debut > fin) {
		*segment = (*segment)->next;
		if(*segment == NULL) break;
		*current = 0;
		}
	}
while(total > 0);
return ecrit;
}


void save_regions(list_regions *regions, FILE *out)
{
  int total;
  unsigned l_line;
list_segments *segment;
char line[80];
do	{
	total = 0;
	segment = regions->element->list;
	while(segment != NULL) {
		total++;
		segment = segment->next;
		}
	if(total == 0) continue;
	fprintf(out,";;# of segments=%d %s\n",total,regions->element->name);
	strcpy(line, ";;"); l_line=2;
	segment = regions->element->list;
	while(segment != NULL) {
		if(l_line + 12 >= sizeof(line)-1) {
			fputs(line,out); putc('\n',out);
			strcpy(line,";;"); l_line=2;
			}
		sprintf(line+l_line," %d,%d", segment->debut, segment->fin);
		l_line += strlen(line+l_line);
		segment= segment->next;
		}
	fputs(line,out); putc('\n',out);
	}
while(	regions = regions->next, regions != NULL );
}


int save_mase_file(const char *fname, char **seq, char **comments,
	char *header, char **seqname, int totseqs, int *eachlength,
	list_regions *regions, region *region_used, int numb_species_sets,
	int **list_species_sets, char **name_species_sets,
	int *sel_seqs, int tot_sel_seqs, int tot_comment_lines, 
	char **comment_name, char **comment_line,
	int tot_trees, char **trees, const Fl_Menu_Item *menu_tree_items)
/* sauver un alignement au format mase
regions: l'ensemble des regions a ecrire (si on sauve tout l'alignement)
	NULL si on ne sauve que des regions
region_used: pointeur vers la region a sauver 
	     et on ne sauve que les seqs selectionnees s'il y en a, 
	ou NULL pour sauver tout l'alignement
rend 0 si OK, 
	1 si erreur ecriture du fichier
	2 si depassement de longueur d'une sequence (avec region seulement)
*/
{
FILE *out;
int num, retval = 1, current, ecrit, maxlength;
time_t heure;
list_segments *psegment, all_sequence;
region maregion;

if(totseqs == 0) return 0;
out=fopen(fname,"w");
if(out == NULL) return 1;

maxlength = 0;

time(&heure);
fprintf(out,";; saved by seaview on %s",ctime(&heure));
if(region_used == NULL) { /* on veut tout sauver */
	all_sequence.debut = 1;
	all_sequence.next = NULL;
	maregion.list = &all_sequence;
	maregion.name = NULL;
	region_used = &maregion;
	tot_sel_seqs = 0;
	maxlength = eachlength[0];
	for(num=1; num<totseqs; num++) {
		if(maxlength < eachlength[num]) maxlength = eachlength[num];
		}
	}
else
	fprintf(out,";; region choice only: %s\n",region_used->name);
if(ferror(out)) goto fin;
if(header != NULL && *header != 0) {
	fputs(header,out);
	if(ferror(out)) goto fin;
	}
if(regions != NULL) {
	save_regions(regions, out);
	if(ferror(out)) goto fin;
	}
if(tot_comment_lines > 0) {
	save_comment_lines(tot_comment_lines, comment_name, comment_line, out);
	if(ferror(out)) goto fin;
	}
if(numb_species_sets != 0) { /* sauver les species sets */
	save_species_sets(numb_species_sets, list_species_sets, 
		name_species_sets, totseqs, out);
	if(ferror(out)) goto fin;
	}
if(tot_trees > 0) { /* write trees out */
	for(num = 0; num < tot_trees; num++) {
		fprintf(out, ";;$ %s\n", menu_tree_items[num].label());
    char *tree = strdup(trees[num]);
    char *p;
    while ((p=strchr(tree, '\n')) != NULL) *p = ' ';
    int l = strlen(tree);
		for (p = tree; p < tree + l; p += 80) {
			while (*p == '$') { // avoid begin line with ";;$..." that marks a new tree
			  fputs(";;$\n", out);
			  p++;
			  }
			fprintf(out, ";;%.80s\n", p);
			}
    free(tree);
		}
	}
for(num=0; num<totseqs; num++) {
	if( tot_sel_seqs != 0 && ! sel_seqs[num] ) continue;
	current = 0; psegment = region_used->list;
	all_sequence.fin = eachlength[num];
	if(comments != NULL && comments[num] != NULL)
		fputs(comments[num], out);
	else fputs(";no comment\n", out);
	if(ferror(out)) goto fin;
	fprintf(out,"%s\n",seqname[num]);
	if(ferror(out)) goto fin;
	do	{
		ecrit = output_next_res_from_region(seq[num], eachlength[num], 
			&psegment, &current, out, 60, FALSE);
 		if(ferror(out)) goto fin;
		if( ecrit > 0) putc('\n', out);
		else if(ecrit == -1) {retval = 2; goto fin; }
		}
	while(ecrit != 0);
	if( (ecrit = maxlength - eachlength[num]) > 0) {
		int n;
		for(n = 1; n <= ecrit; n++) { 
			putc('-', out); if(n % 60 == 0) putc('\n', out); 
			}
		putc('\n', out);
		}
	if(ferror(out)) goto fin;
	}
retval = 0;
fin:
if( fclose(out) != 0 ) retval = 1;
return retval;
}


int save_fasta_file(const char *fname, char **seq, char **comments,
	char **seqname, int totseqs, int *eachlength, region *region_used,
	int *sel_seqs, int tot_sel_seqs, int spaces_in_names, int pad_to_max_length)
/* sauver des sequences ou des regions au format fasta
region_used pointe vers la liste des regions a sauver
si region_used == NULL, toutes les sequences sont sauvees
si pad_to_max_length == TRUE && region_used == NULL, les seqs sont allongŽes a leur longueur max
rend 0 si ok, 1 si erreur d'ecriture dans le fichier
	2 si tentative de depasser la longueur d'une sequence
*/
{
FILE *out;
int num, retval = 1, current, ecrit, save_full = (region_used == NULL);
list_segments *psegment, all_sequence;
region maregion;
char *p;

if(totseqs == 0) return 0;
if( (out = fopen(fname,"w")) == NULL) return 1;
if(region_used == NULL) { /* on veut tout sauver */
	tot_sel_seqs = 0;
	all_sequence.debut = 1;
	all_sequence.fin = eachlength[0];
	for(num = 1; num < totseqs; num++)
		if( eachlength[num] < all_sequence.fin )  
			 all_sequence.fin = eachlength[num];
	all_sequence.next = NULL;
	maregion.list = &all_sequence;
	region_used = &maregion;
	}
for(num=0; num<totseqs; num++) {
	if( tot_sel_seqs != 0 && ! sel_seqs[num] ) continue;
	current = 0; psegment = region_used->list;
	if (!save_full || !pad_to_max_length) all_sequence.fin = eachlength[num];
	fputc('>', out);
	p = seqname[num];
	if(spaces_in_names) {
	  fputs(p, out);
	  }
	else {
	  while(*p != 0) { fputc( *p == ' ' ? '_' : *p, out); p++; }
	  if(comments != NULL && comments[num] != NULL) {
		  putc(' ', out); 
		  p = comments[num] + 1;
		  while(*p != '\n' && *p != 0) putc(*(p++), out);
            int gc = get_ncbi_gc_from_comment(comments[num]);
            char *q = strstr(comments[num], "/transl_table=");
            if (gc > 1 && q != NULL && q >= p) {
              fprintf(out, " /transl_table=%d", gc);
            }
		  }
	  }
	putc('\n', out);
	if(ferror(out)) goto fin;
	do	{
		ecrit = output_next_res_from_region(seq[num], eachlength[num], 
			&psegment, &current, out, 60, FALSE);
		if( ecrit > 0) putc('\n', out);
		else if(ecrit == -1) {retval = 2; goto fin; }
		}
	while(ecrit != 0);
	if(ferror(out)) goto fin;
	}
retval = 0;
fin:
if( fclose(out) != 0 ) return 1;
return retval;
}



int save_clustal_file(const char *fname, char **seq,
	char **seqname, int totseqs, int *eachlength, region *region_used,
	int *sel_seqs, int tot_sel_seqs)
/* sauver des sequences ou des regions au format clustal
region_used pointe vers la liste des regions a sauver
si region_used == NULL, toutes les sequences sont sauvees
rend 0 si ok, 1 si erreur d'ecriture dans le fichier
	2 si tentative de depasser la longueur d'une sequence
*/
{
const int widcluslin = 60;
FILE *out;
int i, j, retval = 1, current, save_current, err, l, lmax;
list_segments *psegment, all_sequence, *curr_segment;
region maregion;

if(totseqs == 0) return 0;
if( (out = fopen(fname,"w")) == NULL) return 1;
if(region_used == NULL) { /* on veut tout sauver */
	tot_sel_seqs = 0;
	all_sequence.debut = 1;
	all_sequence.fin = eachlength[0];
	for(i = 1; i < totseqs; i++)
		if( eachlength[i] > all_sequence.fin )  
			 all_sequence.fin = eachlength[i];
	all_sequence.next = NULL;
	maregion.list = &all_sequence;
	region_used = &maregion;
	}
lmax = 0;
for(i=0; i < totseqs; i++) {
	if( tot_sel_seqs != 0 && ! sel_seqs[i] ) continue;
	l = strlen(seqname[i]);
	if(l > lmax) lmax = l;
	}
lmax += 2;

fprintf(out,"CLUSTAL W (1.7) multiple sequence alignment\n\n\n");
current = 0; psegment = region_used->list;
while( psegment != NULL ) {
	curr_segment = psegment; save_current = current;
	for(i=0; i < totseqs; i++) {
		if( tot_sel_seqs != 0 && ! sel_seqs[i] ) continue;
		psegment = curr_segment; current = save_current;
/* remplacer espaces internes par _ */
		for(j = 0; j < lmax; j++) {
			if(seqname[i][j] == 0) break;
			putc( (seqname[i][j] == ' ' ? '_' : seqname[i][j] ), 
				out);
			}
		while( j < lmax) {
			putc(' ', out); j++;
			}
		err= output_next_res_from_region(seq[i], eachlength[i], 
			&psegment, &current, out, widcluslin, FALSE);
		putc('\n', out);
		if(err == -1) {retval = 2; goto fin; }
		if(ferror(out)) goto fin;
		}
	fprintf(out, "\n\n");
	}
retval = 0;
fin:
if( fclose(out) != 0 ) return 1;
return retval;
}


int calc_gcg_check(list_segments *psegment, char *seq)
{
int  i, debut, fin, residue, pos = 0;
long check = 0;
while(psegment != NULL) {
	debut = psegment->debut; fin = psegment->fin;
        for( i=debut; i<= fin; i++) {
		residue = toupper(seq[i - 1]);
		if(residue == '-') residue = '.';
                check += (( (pos++) % 57)+1) * residue;
		}
	psegment = psegment->next;
	}
return (check % 10000);
}


int save_msf_file(const char *fname, char **seq,
	char **seqname, int totseqs, int *eachlength, region *region_used,
	int protein, int *sel_seqs, int tot_sel_seqs)
/* sauver des sequences ou des regions au format MSF
region_used pointe vers la liste des regions a sauver
si region_used == NULL, toutes les sequences sont sauvees
rend 0 si ok, 1 si erreur d'ecriture dans le fichier
	2 si tentative de depasser la longueur d'une sequence
*/
{
FILE *out;
int i, j, k, retval = 1, current, save_current, err, lenseqs, gen_check,
	*check_val, curr_len, toprint, save_complete, fromseq, new_current;
list_segments *psegment, all_sequence, *curr_segment, *new_segment;
region maregion;

if(totseqs == 0) return 0;
if( (out = fopen(fname,"w")) == NULL) return 1;
save_complete = (region_used == NULL);
if(save_complete) { /* on veut tout sauver */
	tot_sel_seqs = 0;
	all_sequence.debut = 1;
	all_sequence.fin = 0;
	for(i = 0; i < totseqs; i++) {
		if( eachlength[i] > all_sequence.fin )  
			 all_sequence.fin = eachlength[i];
		}
	lenseqs = all_sequence.fin;
	all_sequence.next = NULL;
	maregion.list = &all_sequence;
	region_used = &maregion;
	}
else	{
	/* calcul longueur des regions */
	lenseqs = 0;
	psegment = region_used->list;
	while(psegment != NULL) {
		lenseqs += psegment->fin - psegment->debut + 1;
		psegment = psegment->next;
		}
	}
for(i = 0, k = 0; i < totseqs; i++) /* nbre de seqs editees */
	if( tot_sel_seqs == 0 || sel_seqs[i] ) k++;
check_val = (int *)malloc( k * sizeof(int) );
if(check_val == NULL) {
	fclose(out);
	return 1; /* pas tres precis */
	}
gen_check = 0;
for(i = 0, j = 0; i < totseqs; i++) {
	if( tot_sel_seqs != 0 && ! sel_seqs[i] ) continue;
	check_val[j] = calc_gcg_check(region_used->list, seq[i]);
	gen_check += check_val[j++];
	}
gen_check = gen_check % 10000;
  fprintf(out, "!!%2s_MULTIPLE_ALIGNMENT 1.0\n %s", (protein?"AA":"NA"), extract_filename(fname) );
fprintf(out,"   MSF: %d  Type: %c    Check:%6d   .. \n\n",
	lenseqs, (protein ? 'P' : 'N'), gen_check);
for(i = 0 , j = 0; i < totseqs; i++) {
	if( tot_sel_seqs != 0 && ! sel_seqs[i] ) continue;
	fprintf(out, " Name: %-15.15s   Len:%5d  Check:%6d  Weight:  1.00\n",
		seqname[i], lenseqs, check_val[j++]);
	}
fprintf(out,"\n//\n\n\n");
new_current = 0; new_segment = region_used->list; curr_len = 0;
while( new_segment != NULL && curr_len < lenseqs) {
	curr_segment = new_segment; save_current = new_current;
	fprintf(out, "\n");
	for(i=0; i < totseqs; i++) {
		if( tot_sel_seqs != 0 && ! sel_seqs[i] ) continue;
		psegment = curr_segment; current = save_current;
		for(j = 0; j < MSF_WID_NAME; j++) {
			if(seqname[i][j] == 0) break;
			putc(seqname[i][j],out);
			}
		while( j < MSF_WID_NAME + 1) {
			putc(' ', out); j++;
			}
		for(k = curr_len; k < curr_len + 50 && k < lenseqs; k += 10) {
			toprint = 10;
			if(k + toprint > lenseqs) toprint = lenseqs - k;
			fromseq = toprint;
			if(save_complete && k + fromseq > eachlength[i])
					fromseq = eachlength[i] - k;
			if(fromseq < 0) fromseq = 0;
			if(fromseq > 0) {
				err= output_next_res_from_region(
					seq[i], eachlength[i], &psegment, 
					&current, out, fromseq, TRUE);
 				if(ferror(out)) goto fin;
				if(err == -1) {retval = 2; goto fin; }
				}
			while(fromseq < toprint) {
				putc('.', out); fromseq++;
				}
			putc(' ', out);
			}
		putc('\n', out);
		if( (!save_complete) || eachlength[i] == lenseqs) {
			new_current = current;
			new_segment = psegment;
			}
		if(ferror(out)) goto fin;
		}
	curr_len += 50;
	fprintf(out, "\n");
	}
retval = 0;
fin:
if( fclose(out) != 0 ) retval = 1;
free(check_val);
return retval;
}


char *save_alignment_or_region(const char *fname, char **seq, char **comments,
	char *header, char **seqname, int totseqs, int *eachlength,
	list_regions *regions, region *region_used, known_format format,
	int numb_species_sets, int **list_species_sets, 
	char **name_species_sets, int *sel_seqs, int tot_sel_seqs, int protein,
	int tot_comment_lines, char **comment_name, char **comment_line, int phylipwidnames, 
	int tot_trees, char **trees, const Fl_Menu_Item *items, int spaces_in_fasta_names)
/* sauver des sequences ou des regions au format de fichier format
region_used pointe vers la liste des regions a sauver
si region_used == NULL, toutes les sequences entieres sont sauvees
rend NULL si ok
	un message d'erreur sinon.
*/
{
int err=0;
static char err_message[200];

if(format == MASE_FORMAT) 
	err = save_mase_file(fname, seq, comments,
		header, seqname, totseqs, eachlength,
		regions, region_used, numb_species_sets, list_species_sets,
		name_species_sets, sel_seqs, tot_sel_seqs,
		tot_comment_lines, comment_name, comment_line,
		tot_trees, trees, items);
else if(format == NEXUS_FORMAT) {
	err = save_nexus_file(fname, totseqs, protein,
		seq, seqname, comments, header,
		numb_species_sets, list_species_sets, name_species_sets,
		regions, tot_comment_lines, comment_name, comment_line,
		region_used, sel_seqs, tot_sel_seqs, eachlength, tot_trees, trees, items);
	}
else if(format == PHYLIP_FORMAT) 
	err = save_phylip_file(fname, seq,
		seqname, totseqs, eachlength, region_used, 
		sel_seqs, tot_sel_seqs, phylipwidnames);
else if(format == CLUSTAL_FORMAT) 
	err = save_clustal_file(fname, seq,
		seqname, totseqs, eachlength, region_used, 
		sel_seqs, tot_sel_seqs);
else if(format == MSF_FORMAT) 
	err = save_msf_file(fname, seq,
		seqname, totseqs, eachlength, region_used, protein,
		sel_seqs, tot_sel_seqs);
else if(format == FASTA_FORMAT) 
	err = save_fasta_file(fname, seq, comments,
		seqname, totseqs, eachlength, region_used,
		sel_seqs, tot_sel_seqs, spaces_in_fasta_names);
if(err == 0)
	return NULL;
else if(err == 1) 
	sprintf(err_message,"Error while writing to file %s",fname);
else if(err == 2) 
	strcpy(err_message,
		"Error: region goes beyond the end of one sequence");
return err_message;
}


#if !(defined(WIN32) || defined(__APPLE__))
static char seaview_prog_dir[200] = "";
void inform_prog_dir(const char *arg0)
{
char *p;
if((p = (char*)strrchr(arg0, '/')) != NULL) {
	memcpy(seaview_prog_dir, arg0, p - arg0 + 1);
	seaview_prog_dir[p - arg0 + 1] = 0;
	}
else seaview_prog_dir[0] = 0;
}

char *get_prog_dir(void)
{
return seaview_prog_dir;
}
#endif


char *get_full_path(const char *fname)  
/* to get full pathname to file fname searching for its name, for it in the prog dir
and then for it through all path directories 
returns NULL if not found 
*/
{
#define Mxdir 600
#ifdef WIN32
#define PATH_SEPAR ';'
#define DIR_SEPAR '\\'
#else
#define PATH_SEPAR ':'
#define DIR_SEPAR '/'
#endif
static char dir[Mxdir+1];
char *path, *deb, *fin;
FILE *fich;
int lf, ltot;

strcpy(dir, fname);
if(strchr(fname, DIR_SEPAR) != NULL) {// if fname is a pathname
	fich = fopen(dir, "r"); /* try first explicit filename */
	goto way_out; // and don't search more
	}
#if defined(__APPLE__)
sprintf(dir, "%s/%s", MG_GetBundleResourcesDir(), fname);
fich = fopen(dir, "r");
if(fich != NULL) goto way_out;
#else
/* try dir where program was launched */
deb = get_prog_dir();
if(deb != NULL && *deb != 0) {
	strcpy(dir, deb); 
	strcat(dir, fname);
	fich = fopen(dir, "r");
	if(fich != NULL) goto way_out;
	}
#endif
path = getenv("PATH"); // get the list of path directories, separated by : or ;
if (path == NULL ) return NULL;
lf = strlen(fname);
deb = path;
do      {
		fin = strchr(deb,PATH_SEPAR);
		if(fin != NULL)
				{ ltot = fin-deb; if(ltot > 0) strncpy(dir,deb,ltot);  }
		else
				{ strcpy(dir,deb); ltot=strlen(dir); }
		/* now one directory is in string dir */
		if( ltot > 0 && ltot + lf + 1 <= Mxdir)
				{
				dir[ltot] = DIR_SEPAR;
				strcpy(dir+ltot+1,fname); /* now dir is appended with filename */
				fich = fopen(dir,"r");
				if( fich != NULL) break;
				}
		else fich = NULL;
		deb = fin+1;
		}
while (fin != NULL);
way_out:
	if(fich == NULL) return NULL;
	fclose(fich);
#ifndef WIN32
	if(*dir != '/') {
		if(strncmp(dir, "./", 2) == 0) memmove(dir, dir + 2, strlen(dir) - 1);
		char *p, *q;
		char *cdir = (char *)malloc(PATH_MAX);
		p = getcwd(cdir, PATH_MAX);
		q = (char *)malloc(strlen(p) + 1 + strlen(dir) + 1);
		sprintf(q, "%s/%s", p, dir);
		strcpy(dir, q);
		free(q); 
		free(cdir);
		}
#endif
	return dir;
#undef Mxdir
}


static void save_species_sets(int numb_species_sets, int **list_species_sets, 
	char **name_species_sets, int totseqs, FILE *out)
{
int num, i, previous, total;
for(num=0; num < numb_species_sets; num++) {
	total = 0;
	for(i=0; i< totseqs; i++) 
		if( list_species_sets[num][i] ) total++;
	if( total == 0 ) continue;
	fprintf(out,";;@ of species = %d %s\n;;", total, 
		name_species_sets[num]);
	for(previous = 0; previous < totseqs; previous++) 
		if( list_species_sets[num][previous] ) break;
	total = 0;
	for(i = previous+1; i < totseqs; i++) {
		if( list_species_sets[num][i] ) {
			fprintf(out," %d,", previous+1);
			previous = i;
			total++;
			if( total >= 15 ) {
				fprintf(out, "\n;;");
				total = 0;
				}
			}
		}
	fprintf(out," %d\n", previous+1);
	}
}


void save_comment_lines(int tot_comment_lines, char **names, char **lines, 
	FILE *out)
{
int num, l, pos;

for(num = 0; num < tot_comment_lines; num++) {
	if( (l = strlen(lines[num]) ) == 0) continue;
	fprintf(out, ";;|%s\n", names[num]);
	for(pos = 0; pos < l; pos += 60)
		fprintf(out, ";;%.60s\n", lines[num]+pos);
	fprintf(out, ";;||\n");
	}
}


known_format what_format(const char *filename)
/*
 returns an alignment format (>= 0)
         -1 unknown format
         -2 a Newick tree
 */
{
FILE *in;
char line[100], *p;
int format = -1;
int nseq, lseq;

in = fl_fopen(filename, "r");
if(in == NULL) return (known_format)-1;
p = fgets(line, sizeof(line), in);
if( p == NULL) { fclose(in); return (known_format)-1; }
while(*p) { *p = toupper(*p); p++; }
if(*line == ';') format = MASE_FORMAT;
else if(*line == '>') format = FASTA_FORMAT;
else if(*line == '(' || *line == '[') format = -2;
else if(strncmp(line, "CLUSTAL", 7) == 0) format = CLUSTAL_FORMAT;
else if(strncmp(line, "#NEXUS", 6) == 0) format = NEXUS_FORMAT;
else {
	nseq = lseq = -1;
	sscanf(line, "%d%d", &nseq, &lseq);
	if(nseq != -1 && lseq != -1) format = PHYLIP_FORMAT; 
	else {
		/* try MSF format */
		do 	{
			p = fgets(line, sizeof(line), in);
			if(p != NULL && strstr(p, " MSF: ") !=  NULL) format = MSF_FORMAT;
			}
		while(p != NULL  && strncmp(p, "//", 2) != 0 );
		}
	}
fclose(in);
return (known_format)format;
}


char *my_fgets(char *s, int n, FILE *f)
{
int next_char, ahead;
char *p;

p = s;
while(--n > 0) {
	next_char = getc(f);
	if( next_char == '\r' || next_char == '\n' ) {
		*(p++) = '\n';
		ahead = getc(f);
		if(ahead == EOF) break;
		if( (next_char == '\r' && ahead != '\n') || (next_char == '\n' && ahead != '\r') ) {
			ungetc(ahead, f);
			}
		break;
		}
 	else if (next_char == EOF) 
		break;
	*(p++) = next_char;
	}
*p = 0;
return (p == s ? NULL : s);
}


char *seaview_file_chooser_save_as(const char* message, const char* fname, SEA_VIEW *view, known_format* new_format)
{
#ifndef MICRO
  const char *prev_label = Fl_File_Chooser::show_label;
  Fl_File_Chooser::show_label = "Format";
#endif

char *p, types_list[500] = "";
Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
#ifndef MICRO
  Fl_File_Chooser::show_label = prev_label;
#endif

chooser->type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);   
chooser->options(Fl_Native_File_Chooser::SAVEAS_CONFIRM | chooser->options());
#if SEAVIEW_FLTK_VERSION >= 133 || defined(__APPLE__)
chooser->options(Fl_Native_File_Chooser::USE_FILTER_EXT | chooser->options());
#endif
chooser->title(message);  
chooser->directory(extract_dirname(fname)); 
chooser->preset_file(extract_filename(fname)); 
	
p = types_list;
for(int f = 0; f < nbr_formats; f++) {
	sprintf(p, "%s\t*.%s\n", f_format_names[f], f_format_exts[f]);
	p += strlen(p);
	}
chooser->filter(types_list);    
chooser->filter_value(view->format_for_save);    
char *filename = run_and_close_native_file_chooser(chooser, TRUE);
if (filename && new_format) *new_format = (known_format)chooser->filter_value();
delete chooser;
return filename;
}


const char *extract_dirname(const char *pathname)
{
	static char dirname[300];
	char *p;
	
#if defined(WIN32)
	p = strrchr(pathname,'\\'); 
#else
	p = (char*)strrchr(pathname,'/'); 
#endif
	if(p == NULL) dirname[0] = 0;
	else {
		memcpy(dirname, pathname, p - pathname);
		dirname[p - pathname] = 0;
		}
	return dirname;
}

int printout(SEA_VIEW *view, const char *filename, 
	     int fontsize, int block_size, Fl_Paged_Device::Page_Format pageformat, int vary_only, int ref0, 
	     int pdfkindvalue, Fl_Paged_Device::Page_Layout layout, int svg_width)
{
  int num, i, j, k, current, max_seq_length, fin, curr_lines, widnames, 
  res_per_line, nl, firstpage, lines_per_page, use_pdf, use_svg, top_margin;
  Fl_Surface_Device *surface;
  FILE *textfile = NULL;
  time_t heure;
  static char unnamed[] = "<unnamed>";
  static char num_line[200];
  int lettre, char_per_line;
  short *vary_need = NULL;
  int *vary_pos; /* rang ds alignement de la colonne imprimŽe */
  char *p, oneline[500];
  int (*calc_color_function)(int);
  double	char_width = fontsize/2, descender, margin = 25;
  if(view->tot_seqs == 0) return 0;
  if(view->protein) calc_color_function = get_color_for_aa;
  else  calc_color_function = get_color_for_base;
  use_pdf = (pdfkindvalue == PDF_COLOR || pdfkindvalue == PDF_BW);
  use_svg = (pdfkindvalue == SVG);
  max_seq_length = 0; widnames = 0;
  for (i=0; i < view->tot_seqs; i++) {
    if (view->each_length[i] > max_seq_length) max_seq_length = view->each_length[i];
    if ( ( fin=strlen(view->seqname[i]) ) > widnames) widnames = fin;
  }
  widnames += 2;
  if (use_pdf) {
    surface = new Fl_PDF_or_PS_File_Device();
    if (((Fl_PDF_or_PS_File_Device*)surface)->begin_document(filename, pageformat, layout)) {
      delete surface;
      return 0;
    }
  }
  else if(use_svg) {
    FILE *out = fl_fopen(filename, "w");
    if (out == NULL) exit(1);
#ifndef NO_PDF
    Fl_SVG_PDF_width_Graphics_Driver *pdf_d = new Fl_SVG_PDF_width_Graphics_Driver(NULL);
    pdf_d->font(FL_COURIER, fontsize);
    char_width = pdf_d->width("X", 1);
    delete pdf_d;
#endif
    margin = char_width;
    char_per_line = (int)( (svg_width - 2*margin) / char_width + 0.5);
    fin = (char_per_line - widnames + 1) / (block_size + 1);
    if (fin < 1) { /* garde fou */
      fin = 1; block_size = char_per_line - widnames;
    }
    res_per_line = fin * block_size;
    int nl = (max_seq_length + res_per_line - 1) /res_per_line;
    int totl = nl * (view->tot_seqs + 2) + 2;
    surface = new Fl_SVG_File_Surface(svg_width, totl * fontsize, out, true);
    surface->set_current();
    fl_color(210,210,210); // draw grey background
    fl_rectf(0,0,svg_width, totl * fontsize);
    }
  else {
    textfile = fopen(filename, "w");
    if(textfile == NULL) return TRUE;
  }
#ifndef NO_PDF
  jmp_buf* jbuf;
  if (use_pdf) ((Fl_PDF_or_PS_File_Device*)surface)->surface_try(&jbuf);
  if ( (!use_pdf) || (setjmp(*jbuf) == 0)) {
#endif
    if (use_pdf) {
      surface->driver()->font(FL_COURIER, fontsize);
      char_width = fl_width("X");
      int pwidth, pheight;
      ((Fl_PDF_or_PS_File_Device*)surface)->printable_rect(&pwidth, &pheight);
      int l, r, t, b;
      ((Fl_PDF_or_PS_File_Device*)surface)->margins(&l, &t, &r, &b);
      margin -= l;
      char_per_line = (int)((pwidth - 2*margin) / char_width + 0.5);
      lines_per_page = (int)((pheight - 2*margin) / fontsize + 0.5);
      descender = fl_descent();
      top_margin = margin;
    }
    else if (use_svg) {
      surface->driver()->font(FL_COURIER, fontsize);
      lines_per_page = 10000000;
      top_margin = fontsize;
      }
    else char_per_line = 90;
    firstpage = TRUE;
    
    if(ref0 < 0) vary_only = FALSE;
    time(&heure);
    sprintf(oneline,"Alignment: %s", view->masename == NULL ? unnamed : PREPARE_LABEL(view->masename) );
    if (use_pdf) {
      ((Fl_PDF_or_PS_File_Device*)surface)->start_page();
      ((Fl_PDF_or_PS_File_Device*)surface)->origin(0, fontsize);
      fl_draw(oneline, margin, top_margin); 
    }
    else if(!use_svg) {fputs(oneline, textfile); fputs("\n", textfile);}
    curr_lines = 1;
    if(vary_only) {
      const char fixed[] = "Displaying variable sites only.";
      if (use_pdf || use_svg) fl_draw(fixed, margin, top_margin + curr_lines * fontsize); 
      else {fputs(fixed, textfile); fputs("\n", textfile);}
      ++curr_lines;
    }
    if (use_pdf) {
      sprintf(oneline,"Seaview [blocks=%d fontsize=%d %s%s] on %s",
	    block_size, fontsize, pageformat == Fl_Paged_Device::A4 ? "A4" : "LETTER", 
	    layout == Fl_Paged_Device::LANDSCAPE ? "-landscape" : "", ctime(&heure));
      p = strchr(oneline, '\n'); if (p) *p = 0;
      fl_draw(oneline, margin, top_margin + curr_lines * fontsize); 
      curr_lines += 2;
    }
    else if(use_svg) {
      sprintf(oneline,"created by Seaview on %s", ctime(&heure));
      p = strchr(oneline, '\n'); if (p) *p = 0;
      fl_color(FL_BLACK);
      fl_draw(oneline, margin, top_margin + curr_lines * fontsize); 
      curr_lines += 1;
    }
    else {
      fputs("Seaview text-only output\n", textfile);
      curr_lines += 2;
    }
    if(vary_only) {
      vary_need = (short *)calloc(max_seq_length, sizeof(short));
      if(vary_need == NULL) return TRUE;
      vary_pos = (int *)calloc(char_per_line, sizeof(int));
      if(vary_pos == NULL) return TRUE;
      for(i = 0; i < max_seq_length; i++) {
	for(num = 0; num < view->tot_seqs; num++) {
	  if( toupper(view->sequence[num][i]) != toupper(view->sequence[ref0][i]) ) { 
	    vary_need[i] = TRUE;
	    break;
	  }
	}
      }
    }
    /* nombre max de blocks qui tiennent sur une ligne de cpl chars */
    fin = (char_per_line - widnames + 1) / (block_size + 1);
    if(fin < 1) { /* garde fou */
      fin = 1; block_size = char_per_line - widnames;
    }
    res_per_line = fin * block_size;
    current = 0; 
    while( current < max_seq_length ) {
      nl = 1;
      if(vary_only) { 
	memset(vary_pos, 0, res_per_line * sizeof(int) );
	i = -1; j = 0; k = 0;
	while( j < res_per_line) {
	  if(current + i >= max_seq_length) break;
	  if( !vary_need[current + ++i] ) continue;
	  j++;
	  vary_pos[k++] = current + i + 1;
	  if( j % block_size == 0) k++;
	}
	nl = calc_vary_lines(vary_pos,  k);
      }
      if( use_pdf && (!firstpage) && (curr_lines + view->tot_seqs + nl > lines_per_page)) {
	((Fl_PDF_or_PS_File_Device*)surface)->end_page();
	((Fl_PDF_or_PS_File_Device*)surface)->start_page();
	((Fl_PDF_or_PS_File_Device*)surface)->origin(0, fontsize);
	surface->driver()->font(FL_COURIER, fontsize);
	curr_lines = 0;
      }
      if(vary_only) {
	out_vary_pos(vary_pos, widnames, k, nl, textfile, margin, top_margin + curr_lines * fontsize);
	curr_lines += nl;
      }
      else	{
	sprintf(num_line, "%d", current + 1);
	fin = strlen(num_line);
	memmove(num_line + widnames - fin + 1, num_line, fin+1);
	if(fin <= widnames) memset(num_line, ' ', widnames - fin + 1);
	if( use_pdf || use_svg) fl_draw(num_line, margin, top_margin + curr_lines * fontsize); 
	else {fputs(num_line, textfile);fputs("\n",textfile);}
	++curr_lines;
      }
      for(num=0; num < view->tot_seqs; num++) {
	k = 0;
	for(j = 0; j < widnames; j++) {
	  if(view->seqname[num][j] == 0) break;
	  oneline[k++] = view->seqname[num][j];
	}
	while( j < widnames) {
	  j++;
	  oneline[k++] = ' ';
	}
	if(vary_only) {
	  i = -1; j = 0;
	  while( j < res_per_line) {
	    if(current + i >= max_seq_length) break;
	    if( !vary_need[current + ++i] ) continue;
	    j++;
	    if(current + i < view->each_length[num]) {
	      if(num != ref0) lettre = ( toupper(view->sequence[num][current+i]) == 
					toupper(view->sequence[ref0][current+i]) ? '.' : view->sequence[num][current+i] );
	      else lettre = view->sequence[ref0][current+i];
	      oneline[k++] = lettre;
	    }
	    if( j % block_size == 0) oneline[k++] = ' ';
	  }
	  if(num == view->tot_seqs - 1) current = current + i + 1;
	}
	
	else	{
	  fin = res_per_line;
	  if(current+fin > view->each_length[num]) 
	    fin = view->each_length[num] - current;
	  if(ref0 != -1 && num != ref0) {
	    /* ecriture par reference a seq ref0 */
	    for(i=0; i<fin; i++) {
	      lettre = ( toupper(view->sequence[num][current+i]) == 
			toupper(view->sequence[ref0][current+i]) ? '.' : view->sequence[num][current+i] );
	      oneline[k++] = lettre;
	      if( i < fin-1 && (i+1)%block_size == 0) 
		oneline[k++] = ' ';
	    }
	  }
	  else	{ /* ecriture normale de seq */
	    for(i=0; i<fin; i++) {
	      oneline[k++] = view->sequence[num][current+i];
	      if( i < fin-1 && (i+1)%block_size == 0) 
		oneline[k++] = ' ';
	    }
	  }
	}
	oneline[k] = 0;
	if(!view->allow_lower) majuscules(oneline + widnames);
	if(use_pdf && (curr_lines >= lines_per_page)) {
	  ((Fl_PDF_or_PS_File_Device*)surface)->end_page();
	  ((Fl_PDF_or_PS_File_Device*)surface)->start_page();
	  ((Fl_PDF_or_PS_File_Device*)surface)->origin(0, fontsize);
	  surface->driver()->font(FL_COURIER, fontsize);
	  curr_lines = 0;
	}
	if(!use_pdf && !use_svg) {
	  fputs(oneline, textfile); fputs("\n", textfile);
	}
	else if(pdfkindvalue == PDF_BW) {
	  fl_draw(oneline, margin, top_margin + curr_lines * fontsize);
	}
	else {
	  if (use_pdf) color_pdf_display(view, calc_color_function, oneline, widnames, margin, 
			    top_margin + curr_lines * fontsize, 
			    fontsize, char_width, descender, num, current);
	  else color_svg_display(view, calc_color_function, oneline, widnames, margin, 
				 top_margin + curr_lines * fontsize, fontsize, char_width);
	}
	++curr_lines;
	firstpage = FALSE;
      }
      if(!use_pdf && !use_svg) {
	fputs("\n", textfile);
      }
      else if(curr_lines + 1 <= lines_per_page) {
	++curr_lines;
      }
      if( ! vary_only ) current += res_per_line;
    }
    if(use_pdf) {
      ((Fl_PDF_or_PS_File_Device*)surface)->end_page();
      ((Fl_PDF_or_PS_File_Device*)surface)->end_job();
    }
    else if(use_svg) {
      fl_color(FL_GRAY);
      fl_line_style(0, 3);
      fl_rect(1,1, ((Fl_SVG_File_Surface*)surface)->width()-2, ((Fl_SVG_File_Surface*)surface)->height()-2);
      ((Fl_SVG_File_Surface*)surface)->end();
      }
    else fclose(textfile);
#ifndef NO_PDF
  } /* end of PDF_TRY */
  if (use_pdf && ((Fl_PDF_or_PS_File_Device*)surface)->surface_catch()) {
    ((Fl_PDF_or_PS_File_Device*)surface)->error_catch();
  }
#endif
  if (use_pdf || use_svg) delete surface;
  return FALSE;
}


static void color_pdf_display(SEA_VIEW *view, int (*calc_color_function)( int ), char *oneline, 
			      int widnames, double x, double y, int fontsize, double char_width, double descender,
			      int num, int current)
{
  double  xx;
  int c, l, count = 0;
  char *p, **clines;
  
  clines = (char **)malloc(sizeof(char *) * view->numb_gc); if(clines==NULL) return;
  l = strlen(oneline);
  for(c = 1; c < view->numb_gc; c++) {
    clines[c] = (char *)malloc(l + 1); if(clines[c] == NULL) return;
    memset(clines[c], ' ', l); clines[c][l] = 0;
  }
  for(p = oneline + widnames; *p != 0; p++) {
    if(*p == ' ') continue;
    if(view->curr_colors != view->codoncolors) c = calc_color_function(*p);
    else c = view->col_rank[num][current + count++];
    if(c > 0) clines[c][p - oneline] = 'X';
  }
  for(c = 1; c < view->numb_gc; c++) {
    if(strchr(clines[c], 'X') == NULL) continue;
    fl_color(view->curr_colors[c]);
    for(xx = x + widnames*char_width, p = clines[c] + widnames; *p != 0; p++, xx += char_width) {
      if(*p == ' ') continue;
      fl_rectf(xx, y - fontsize + descender, char_width+1, fontsize);
    }
  }
  fl_color(FL_BLACK);
  fl_draw(oneline, x, y);
  for(c = 1; c < view->numb_gc; c++) free(clines[c]);
  free(clines);
}

static void color_svg_display(SEA_VIEW *view, int (*calc_color_function)( int ), char *oneline, 
			      int widnames, double x, double y, int fontsize, double char_width)
{
  double  xx;
  int c, l;
  char *p;
  
  l = strlen(oneline);
  char *aux = (char*)malloc(l+1);
  fl_font(FL_COURIER_BOLD, fontsize);
  xx = x + widnames*char_width;
  for (c = 0; c < view->numb_gc; c++) {
    memset(aux, ' ', l-widnames);
    for (p = oneline+widnames; p < oneline+l; p++) {
      if (calc_color_function(*p) == c) aux[p-(oneline+widnames)] = *p;
      }
    fl_color(view->curr_colors[c]);
    fl_draw(aux, l - widnames, xx, y);
  }
  fl_font(FL_COURIER, fontsize);
  fl_color(FL_BLACK);
  fl_draw(oneline, widnames, x, y);
}

static int calc_vary_lines(int *vary_pos, int widpos)
{
  int maxi = 0, num, nl;
  
  for(num = 0; num < widpos; num++) 
    if(vary_pos[num] > maxi) maxi = vary_pos[num];
  if(maxi >= 100000)
    nl = 6;
  else if(maxi >= 10000)
    nl = 5;
  else if(maxi >= 1000)
    nl = 4;
  else if(maxi >= 100)
    nl = 3;
  else if(maxi >= 10)
    nl = 2;
  else 	
    nl = 1;
  return nl;
}


static void out_vary_pos(int *vary_pos, int widnames, int widpos, int nl, FILE *textfile, double x, double y)
{
  int num, l, k, echelle, digit, val;
  static char chiffre[] = "0123456789";
  char oneline[300];
  
  echelle = 1; k = 0;
  for(l = 2; l <= nl; l++) echelle *= 10;
  for(l = nl; l > 0; l--) {
    for(num = 0; num < widnames; num++) oneline[k++] = ' ';
    for(num = 0; num < widpos; num++) {
      val = vary_pos[num];
      if(val < echelle)
	oneline[k++] = ' ';
      else	{
	digit = (val / echelle) % 10 ;
	oneline[k++] = *(chiffre + digit);
      }
    }
    oneline[k] = 0;
    if(textfile == NULL) {
      fl_draw(oneline, x, y);
      y += fl_height();
    }
    else {fputs(oneline, textfile); fputs("\n",textfile); }
    k = 0;
    echelle /= 10;
  }
}


SEA_VIEW* read_alignment_file(const char *infile )
{
  char *err_message;
  int i;
  SEA_VIEW* view = new SEA_VIEW;
  memset(view, 0, sizeof(SEA_VIEW));
  view->menubar = new Fl_Menu_Bar(0,0, 30, 30);
  view->menu_trees = new vlength_menu(view->menubar, "Trees", NULL, 0);

#ifndef R_OK
#define R_OK 04
#endif
  if (fl_access(infile, R_OK)) {
    fprintf(stderr, "File %s\nis not readable or does not exist", infile);
    return NULL;
    }
  known_format file_format = what_format(infile);
  if (file_format < 0) {
    fprintf(stderr, "File %s\nis not of a format readable by seaview", infile);
    return NULL;
  }
  if (file_format == MASE_FORMAT) {
    view->tot_seqs = read_mase_seqs_header(infile, &view->sequence,
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
    view->tot_seqs = read_fasta_align(infile, &view->sequence,
				      &view->seqname, &view->comments, &view->header, &err_message, view->spaces_in_fasta_names);
  else if(file_format == PHYLIP_FORMAT)
    view->tot_seqs = read_phylip_align(infile, &view->sequence,
				       &view->seqname, &view->comments, &view->header, &err_message);
  else if(file_format == CLUSTAL_FORMAT)
    view->tot_seqs = read_clustal_align(infile, &view->sequence,
					&view->seqname, &view->comments, &view->header, &err_message);
  else if(file_format == MSF_FORMAT)
    view->tot_seqs = read_msf_align(infile, &view->sequence,
				    &view->seqname, &view->comments, &view->header, &err_message);
  else if(file_format == NEXUS_FORMAT) {
    int **list_sp = NULL; char **name_sp = NULL; int i;
    view->tot_seqs = read_nexus_align((char*)infile, &view->sequence,
				      &view->seqname, &view->comments, &view->header,
				      &err_message, &view->regions, &view->numb_species_sets,
				      &list_sp, &name_sp, &view->tot_comment_lines, 
				      &view->comment_name, &view->comment_line, 
				      &view->comment_length, &view->protein, view);
    for(i= 0; i < view->numb_species_sets; i++) {
      view->list_species_sets[i] = list_sp[i];
      view->name_species_sets[i] = name_sp[i];
    }
    if(list_sp != NULL) free(list_sp); 
    if(name_sp != NULL) free(name_sp);
  }
  if (view->tot_seqs == 0) return NULL;
  if (file_format != NEXUS_FORMAT) view->protein = is_a_protein_alignment(view);
  view->each_length = new int[view->tot_seqs];
  for (i = 0; i < view->tot_seqs; i++) {
    view->each_length[i] = strlen(view->sequence[i]);
  }
  view->seq_length = 0;
  for (i = 0; i < view->tot_seqs; i++) {
    if (view->each_length[i] > view->seq_length) view->seq_length = view->each_length[i];
  }
  view->masename = strdup(infile);
  view->format_for_save = file_format;
  return view;
}

SEA_VIEW *cmdline_read_input_alignment(int argc, char **argv)
{
  char *fname, line[500];
  fname = argv[argc-1];
  if (strcmp(fname, "-") == 0) {
    fname = create_tmp_filename();
    FILE *out = fopen(fname, "w");
    while (fgets(line, sizeof(line), stdin) != NULL) {
      fputs(line, out);
    }
    fclose(out);
  }
  SEA_VIEW *view = read_alignment_file(fname);
  if (strcmp(argv[argc-1], "-") == 0) delete_tmp_filename(fname);
  if (!view) {
    fputs("\n", stderr);
    exit(1);
  }
  view->max_seq_length = calc_max_seq_length(view->seq_length, view->tot_seqs);
  allonge_seqs(view->sequence, view->tot_seqs, view->max_seq_length, view->each_length, 
	       view->tot_comment_lines, view->comment_line, NULL);
  return view;
}

void format_conversion(int argc, char **argv)
{
  char *err_message, *p;
  int i, j, gc;
  bool std_output = false;
  bool save_fragment = false;
  bool bootstrap = false;
  region *myregion;
  
  SEA_VIEW *view = cmdline_read_input_alignment(argc, argv);
  if (view == NULL) {
    fprintf(stderr, "No sequence found in %s", argv[argc-1]);
    exit(1);
  }  
  known_format out_format = view->format_for_save;
  load_resources(progname);
  view->phylipwidnames = int_res_value("phylipwidnames", 30);
  char *outfile = process_output_options(argc, argv, out_format, std_output);
  
  if ( !view->protein && isarg(argc, argv, "-translate")) {
    bool no_terminal_stop = isarg(argc, argv, "-no_terminal_stop");
    for (i = 0; i < view->tot_seqs; i++) {
      gc = (view->comments != NULL ? get_ncbi_gc_from_comment(view->comments[i]) : 1);
      p = translate_with_gaps(view->sequence[i], get_acnuc_gc_number(gc));
      free(view->sequence[i]);
      view->sequence[i] = p;
      if (no_terminal_stop) {
        char *q = p + strlen(p) - 1;
        while (q > p && *q == '-') q--;
        if (q >= p && *q == '*') *q = '-';
      }
      view->each_length[i] = strlen(p);
      view->regions = NULL;
      }
    view->protein = true;
    }
  
  while ( (p = argname(argc, argv, "-def_species_group")) != NULL) {
    p = strtok(p, ",");
    view->name_species_sets[view->numb_species_sets] = strdup(p);
    view->list_species_sets[view->numb_species_sets] = (int*)calloc(view->tot_seqs, sizeof(int));
    while (true) {
      p = strtok(NULL, ",");
      if (!p) break;
      if (strchr(p, '-') == NULL) {
	sscanf(p, "%d", &i);
	view->list_species_sets[view->numb_species_sets][i-1] = 1;
      }
      else {
	sscanf(p, "%d-%d", &i, &j);
	if (i <= j) {
	  while (i <= j) view->list_species_sets[view->numb_species_sets][i++ - 1] = 1;
	  }
      }
    }
    view->numb_species_sets++;
    for (i = 0; i < argc; i++) {
      if (strcmp(argv[i], "-def_species_group") == 0) {
	*argv[i] = '+';
	break;
      }
    }
  }
  
  while ( (p = argname(argc, argv, "-def_site_selection")) != NULL) {
    int from, to;
    list_segments *current_seg;
    region *site_selection = (region*)calloc(1, sizeof(region));
    list_regions *last_reg = (list_regions*)calloc(1, sizeof(list_regions));
    last_reg->element = site_selection;
    if (!view->regions) {
      view->regions = last_reg;
      }
    else {
      list_regions *current_reg;
      current_reg = view->regions;
      while (current_reg->next) current_reg = current_reg->next;
      current_reg->next = last_reg;
      }
    p = strtok(p, ",");
    site_selection->name = strdup(p);
    while (true) {
      p = strtok(NULL, ",");
      if (!p) break;
      if (strchr(p, '-') == NULL) {
	sscanf(p, "%d", &from);
	to = from;      }
      else {
	sscanf(p, "%d-%d", &from, &to);
	if (from > to) continue;
      }
      if (to > view->seq_length) to = view->seq_length;
      if (from > view->seq_length) continue;
      list_segments *seg = (list_segments*)malloc(sizeof(list_segments));
      seg->debut = from;
      seg->fin = to;
      seg->next = NULL;
      if (!site_selection->list) {
	site_selection->list = seg;
	}
      else {
	current_seg->next = seg;
	}
      current_seg = seg;
    }
    for (i = 0; i < argc; i++) {
      if (strcmp(argv[i], "-def_site_selection") == 0) {
	*argv[i] = '+';
	break;
      }
    }
  }  

  if (isarg(argc, argv, "-gblocks")) { // gblocks method
    myregion = (region*)malloc(sizeof(region));
    myregion->name = strdup("Gblocks");
    view->region_line = (char*)malloc(view->seq_length + 1);
    create_gblocks_mask(view, myregion, true, isarg(argc, argv, "-b5"), isarg(argc, argv, "-b4"), 
			isarg(argc, argv, "-b3"), isarg(argc, argv, "-b2"));
    list_regions *r = view->regions, *pre = NULL;
    while (r) {
      if (strcmp(r->element->name, myregion->name) == 0) break;
      pre = r;
      r = r->next;
    }
    if (r) {
      free(r->element->name);
      r->element = myregion;
    }
    else {
      list_regions *elt = (list_regions*)malloc(sizeof(list_regions));
      elt->element = myregion;
      elt->next = NULL;
      if (pre) pre->next = elt;
      else view->regions = elt;
    }
  }
  
  if ( (p = argname(argc, argv, "-sites")) ) {
    save_fragment = true;
    list_regions *elt = view->regions;
    while (elt) {
      if (strcmp(elt->element->name, p) == 0) break;
      elt = elt->next;
    }
    if (elt) view->active_region = elt->element;
  }
  
  if ( (p = argname(argc, argv, "-species")) ) {
    save_fragment = true;
    for (i = 0; i < view->numb_species_sets; i++) {
      if (strcmp(view->name_species_sets[i], p) == 0) break;
    }
    if (i < view->numb_species_sets) {
      view->sel_seqs = view->list_species_sets[i];
      view->tot_sel_seqs = 0;
      for (i = 0; i < view->tot_seqs; i++) if (view->sel_seqs[i]) view->tot_sel_seqs++;
    }
  }
  if (isarg(argc, argv, "-del_gap_only_sites") && !isarg(argc, argv, "-sites")) del_gap_only_sites(view);
  
  if ( (p = argname(argc, argv, "-bootstrap")) ) {
    int replicates = -1;
    sscanf(p, "%d", &replicates);
    if (replicates == -1) replicates = 1;
    bootstrap = true;
    save_fragment = false;
    save_bootstrap_replicates(outfile, replicates, view);
  }

  
  if (save_fragment) {
    myregion = view->active_region;
    if (myregion == NULL) {
      myregion = (region *)malloc(sizeof(region));
      myregion->list = (list_segments *)malloc(sizeof(list_segments));
      myregion->list->debut = 1;
      myregion->list->fin = view->seq_length;
      myregion->list->next = NULL;
      myregion->name = (char *)"all";
    }
    err_message = save_alignment_or_region(outfile, view->sequence, view->comments,
					   view->header, view->seqname, view->tot_seqs, view->each_length,
					   NULL, myregion, out_format,
					   0, NULL, NULL, view->sel_seqs, view->tot_sel_seqs, 
					   view->protein, 0, NULL, NULL, view->phylipwidnames,
					   view->tot_trees, view->trees, 
					   view->menu_trees->vitem(0), view->spaces_in_fasta_names);
  }
  else if (!bootstrap) {
    err_message = save_alignment_or_region(outfile, view->sequence, view->comments,
				 view->header, view->seqname, view->tot_seqs, view->each_length,
				 view->regions, NULL, out_format,
				 view->numb_species_sets, view->list_species_sets,
				 view->name_species_sets, NULL, 0, view->protein,
				 view->tot_comment_lines, view->comment_name, 
				 view->comment_line, view->phylipwidnames,
				 view->tot_trees, view->trees, 
				 view->menu_trees->vitem(0), view->spaces_in_fasta_names);
  }
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
    fl_unlink(outfile);
    }
  exit(0);
}


char *process_output_options(int argc, char **argv, known_format& out_format, bool& std_output)
{
  int i;
  char upper_format[20], *p;
  std_output = false;
  char *outformatname = argname(argc, argv, "-output_format");
  if (outformatname) {
    majuscules(outformatname);
    for (i = 0; i < nbr_formats; i++) {
      strcpy(upper_format, f_format_names[i]);
      majuscules(upper_format);
      if (strcmp(upper_format, outformatname)== 0) break;
    }
    if (i >= nbr_formats) {
      fprintf(stderr, "'%s' is not of a format known by seaview", outformatname);
      exit(1);
    }
    out_format = (known_format)i;
  }
  char *outfile = argname(argc, argv, "-o");
  if (outfile == NULL) {
    outfile = new char[strlen(argv[argc-1]) + 12];
    strcpy((char*)outfile, argv[argc-1]);
    p = strrchr(outfile, '.');
    if (!p) { p = (char*)outfile + strlen(outfile); *p = '.'; }
    strcpy(p+1, f_format_exts[out_format]);
    if (strcmp(outfile, argv[argc-1]) == 0) {
      strcpy(p, "_2.");
      strcpy(p+3, f_format_exts[out_format]);
    }
  }
  else if (strcmp(outfile, "-") == 0) {
    outfile = strdup(create_tmp_filename());
    std_output = true;
  }
  return outfile;
}


#ifndef NO_PDF
void printout_cmdline(int argc, char **argv)
{
  bool std_output = false;
  SEA_VIEW *view = cmdline_read_input_alignment(argc, argv);
  if (view == NULL) {
    fprintf(stderr, "No sequence found in %s", argv[argc-1]);
    exit(1);
  }  
  load_resources(progname);
  //protein colors
  static char stdcolorgroups[50], customprotcolors[300];
  strcpy(stdcolorgroups, get_res_value("stdcolorgroups", def_stdcolorgroups));
  strcpy(customprotcolors, get_res_value("protcolors", ""));
  int *protcolors = (int *)malloc(max_protcolors * sizeof(int));
  int *def_protcolors = (int *)malloc(max_protcolors * sizeof(int));
  for(int i=0; i<max_protcolors; i++) def_protcolors[i] = 
    fl_rgb_color(def_protcolors_rgb[3*i], def_protcolors_rgb[3*i+1], def_protcolors_rgb[3*i+2]);
  
  memcpy(protcolors, def_protcolors, max_protcolors * sizeof(int) );
  int cur_protcolors = prep_custom_colors(protcolors, customprotcolors, max_protcolors);
  static int dnacolors[] = { FL_BLACK, FL_RED, FL_DARK_GREEN, FL_YELLOW, FL_BLUE };
  /* process resource-read stdcolorgroups and altcolorgroups */
  prep_aa_color_code(stdcolorgroups, (char*)"", 
					 cur_protcolors, &view->numb_stdprotcolors, &view->numb_altprotcolors);
  
  view->max_seq_length = calc_max_seq_length(view->seq_length, view->tot_seqs);
  if (view->protein) {
    view->numb_gc = view->numb_stdprotcolors;
    view->curr_colors = view->stdprotcolors = protcolors;
    protcolors[0] = FL_BLACK;
    }
  else {
    view->numb_gc = view->numb_dnacolors = 5;
    view->curr_colors = view->dnacolors = dnacolors;
    }
  known_format out_format = view->format_for_save;
  char *outfile = process_output_options(argc, argv, out_format, std_output);
  int fontsize = (int)argval(argc, argv, "-fontsize", 10);
  int blocksize = (int)argval(argc, argv, "-blocksize", 10);
#ifdef NO_PDF
  int use_svg = false;
  int svg_width = 0;
#else
  int use_svg = isarg(argc, argv, "-svg");
  int svg_width = (int)argval(argc, argv, "-svg", 1000);
#endif
  int landscape = isarg(argc, argv, "-landscape");
  int letter = isarg(argc, argv, "-letter");
  printout(view, outfile, 
	    fontsize,  blocksize, 
	   letter ? Fl_Paged_Device::LETTER : Fl_Paged_Device::A4, 
	   false, -1, 
	   use_svg ? SVG : PDF_COLOR, 
	   landscape ? Fl_Paged_Device::LANDSCAPE : Fl_Paged_Device::PORTRAIT, 
	   svg_width);
  if (std_output) {
    char line[200];
    FILE *in = fopen(outfile, "r");
    if (in) {
      while (fgets(line, sizeof(line), in) != NULL) {
	fputs(line, stdout);
      }
      fclose(in);
      fflush(stdout);
    }
    delete_tmp_filename(outfile);
  }
  exit(0);
}
#endif // ! NO_PDF
