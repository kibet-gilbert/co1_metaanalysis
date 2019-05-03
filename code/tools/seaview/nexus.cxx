#include "seaview.h"
#include <ctype.h>
#include <time.h>


typedef struct _linked_strings {
	void *data;
	char *name;
	struct _linked_strings *next;
	} linked_strings;


/* prototypes of local functions */
int read_nexus_align(char *fname, char ***pseqs, char ***pseqnames, 
	char ***pcomments, char **pheader, char **err_message, 
	list_regions **charsets, int *pnum_species_sets,
	int ***list_species_sets, char ***name_species_sets,
	int *ptot_comment_lines, char ***comment_name, char ***comment_line,
	int **comment_length, int *protein, SEA_VIEW *view);
int read_nexus_file(FILE *in, char ***pseqs, int *nchars, int *protein,
	char ***taxnames, char ***notes, char **header,
	int *pnum_species_sets, int ***list_species_sets, 
	char ***name_species_sets,
	list_regions **charsets,
	int *tot_comment_lines, char ***comment_name, char ***comment_line,
	int *p_ntrees, char ***p_trees, char ***p_tree_names);
int save_nexus_file(const char *fname, int ntaxa, int protein,
	char **seqs, char **taxnames, char **notes, char *header,
	int num_species_sets, int **list_species_sets, 
	char **name_species_sets,
	list_regions *charsets,
	int tot_comment_lines, char **comment_name, char **comment_line,
	region *region_used, int *sel_seqs, int tot_sel_seqs, int *eachlength,
	int tot_trees, char **trees, const Fl_Menu_Item *items);
int nextbracket(FILE *in);
char *next_token(FILE *in);
FILE *is_nexus_file(char *nom);
char *next_block_name(FILE *in);
int process_block_taxa(FILE *in, char ***taxnames);
int is_block_end(char *p);
void skip_block(FILE *in);
void skip_command(FILE *in);
int process_taxa_dim(FILE *in);
int process_chars_dim(FILE *in, int *pntaxa);
int process_chars_format(FILE *in, int *protein, int *interleave,
	int *matchchar, int *missing);
char **process_taxa_taxlabels(FILE *in, int ntax);
char **process_block_characters(FILE *in, int *pntaxa, char ***taxnames,
	int *nchars, int *protein);
char **process_chars_m_interleave(FILE *in, int nsites, int gap, int ntaxa,
	char **taxnames, int define_taxlabels, int matchchar, int missing,
	int protein);
char **process_chars_matrix(FILE *in, int nsites, int gap, int ntaxa, 
	char **taxnames, int define_taxlabels, int matchchar, int missing,
	int protein);
char **process_block_data(FILE *in, int *pntaxa, char ***ptaxnames, int
	*nsites, int *protein);
char **process_block_notes(FILE *in, int ntaxa, char **taxnames);
int find_by_name_num(char *token, int nelt, char **elts);
char *build_list(FILE *in, int nelt, char **elts);
list_segments *build_list_pairs(FILE *in, int maxi);
void process_notes_text(FILE *in, int ntaxa, char **taxnames, char **notes);
char *process_sets_taxset(FILE *in, int ntaxa, char **taxnames, char
	**setname);
list_segments *process_sets_charset(FILE *in, char **setname, int max_chars);
void process_block_sets(FILE *in, int ntaxa, char **taxnames, int nchars,
	linked_strings **taxsets, list_regions **charsets);
linked_strings *process_block_seaview(FILE *in, char **header);
char *process_seaview_text(FILE *in, char **name);
char **process_block_unaligned(FILE *in, int *ntaxa, char ***taxnames, int
	*nchars, int *protein);
int process_unal_dim(FILE *in);
int process_unal_format(FILE *in);
char **process_unal_matrix(FILE *in, int ntaxa, char **taxnames);
char *out_with_apostrophe(char *p);
char *out_with_optional_protection(char *p);
int linked_to_series(linked_strings *linked, char ***names, char ***lines);
void out_table_by_series(FILE *out, int *table, int dim);


/* used functions */
int output_next_res_from_region(char *seq, int lenseq, 
	list_segments **segment, int *current, FILE *out, int total, 
	int use_dots);



int nextbracket(FILE *in)
{
int pos;
pos = fgetc(in);
while(pos != ']') {
	if(pos == EOF) return EOF;
	if(pos == '[') pos = nextbracket(in);
	pos = fgetc(in);
	}
return pos;
}


#define PUNCTUATION "(){}/\\,;:=*\"+-<>"
#define APOSTROPHE '\''

char *next_token(FILE *in)
{
static char *big_token = NULL, *fin_token;
int c, punct;
char *p, *q;
const int slice = 500;
static int l_token;

if(big_token == NULL) {
	l_token = slice;
	big_token = (char *)malloc(l_token);
	if(big_token == NULL) return NULL;
	fin_token = big_token + l_token;
	}

do	{
	c = fgetc(in);
	if(c == '[' ) {
		c = nextbracket(in);
		c = fgetc(in);;
		}
	if(c == EOF) return NULL;
	}
while(isspace(c));
if(strchr(PUNCTUATION, c) != NULL) {
	big_token[0] = c; big_token[1] = 0;
	return big_token;
	}

p = big_token;
if(c == APOSTROPHE) {
	while (1)	{
		c = fgetc(in);
		if(c == '\r') {
			punct = fgetc(in);
			if(punct != '\n') ungetc(punct, in);
			c = '\n';
			}
		if(c == EOF) break;
		if(c == APOSTROPHE) {
			punct = fgetc(in);
			if(punct != APOSTROPHE) {
				ungetc(punct, in);
				break;
				}
			}
		*p = c; p++;
		if(p >= fin_token) {
			l_token += slice;
			q = (char *)realloc(big_token, l_token);
			if(q == NULL) return NULL;
			p = q + (p - big_token);
			big_token = q;
			fin_token = big_token + l_token;
			}
		}
	*p = 0;
	return big_token;
	}
do	{
	if(c == '[' ) {
		c = nextbracket(in);
		}
	else 	{ 
		*p = c; p++; 
		if(p >= fin_token) {
			l_token += slice;
			q = (char *)realloc(big_token, l_token);
			if(q == NULL) return NULL;
			p = q + (p - big_token);
			big_token = q;
			fin_token = big_token + l_token;
			}
		}
	c = fgetc(in);
	punct = FALSE;
	if(c == EOF) break;
	punct = (strchr(PUNCTUATION, c) != NULL);
	}
while( ! (isspace(c) || punct) );
*p = 0;
if(punct) ungetc(c, in);
return big_token;
}


FILE *is_nexus_file(char *fname) /* return !0 iff seems a NEXUS file */
{
FILE *in;
char *p;

in = fopen(fname, "r");
if(in == NULL) return NULL;
p = next_token(in);
if(p != NULL) majuscules(p);
if(p == NULL || strcmp(p, "#NEXUS") != 0 ) { fclose(in); return NULL; }
else return in;
}


char *next_block_name(FILE *in)
{
char *p;
static char name[81];

do	{
	p = next_token(in);
	if(p == NULL) return NULL;
	majuscules(p);
	}
while(strcmp(p, "BEGIN") != 0);
p = next_token(in);
if(p == NULL) return NULL;
majuscules(p);
strcpy(name, p);
next_token(in); /* skip the ; */
return name;
}


int is_block_end(char *p)
{
if(p!=NULL)majuscules(p);
return p == NULL || strcmp(p, "END") == 0 || strcmp(p, "ENDBLOCK") == 0;
}


void skip_command(FILE *in)
{
char *p;

do	p = next_token(in);
while(p != NULL && *p != ';');
}


void skip_block(FILE *in)
{
char *p;

do	p = next_token(in);
while (!is_block_end(p));
}


int process_taxa_dim(FILE *in)
{
char *p;
int ntax = 0;

p = next_token(in);
if(p == NULL) return 0;
majuscules(p);
if(strcmp(p, "NTAX") == 0) {
	next_token(in);
	p = next_token(in);
	if(p == NULL) return 0;
	sscanf(p, "%d", &ntax);
	}
skip_command(in);
return ntax;
}


int process_chars_dim(FILE *in, int *pntaxa)
{
char *p;
int sites = 0;

do	{
	p = next_token(in);
	if(p == NULL) break;
	majuscules(p);
	if(strcmp(p, "NCHAR") == 0) {
		next_token(in); /* skip = */
		p = next_token(in);
		if(p == NULL) break;
		sscanf(p, "%d", &sites);
		}
	else if(strcmp(p, "NEWTAXA") == 0) ;
	else if(strcmp(p, "NTAX") == 0) {
		next_token(in); /* skip = */
		p = next_token(in);
		if(p == NULL) break;
		sscanf(p, "%d", pntaxa);
		}
	}
while(*p != ';');
return sites;
}


int process_chars_format(FILE *in, int *protein, int *interleave,
	int *matchchar, int *missing)
{
char *p;
int gap=0;
*interleave = FALSE;

do	{
	p = next_token(in);
	if(p == NULL) break;
	majuscules(p);
	if(strcmp(p, "DATATYPE") == 0) {
		next_token(in); /* skip = */
		p = next_token(in);
		if(p == NULL) break;
		majuscules(p);
		*protein = strcmp(p, "PROTEIN") == 0;
		}
	else if(strcmp(p, "GAP") == 0) {
		next_token(in); /* skip = */
		do gap = fgetc(in);
		while(isspace(gap));
		if(gap == EOF) break;
		}
	else if(strcmp(p, "MATCHCHAR") == 0) {
		next_token(in); /* skip = */
		p = next_token(in);
		if(p == NULL) break;
		*matchchar = *p;
		}
	else if(strcmp(p, "MISSING") == 0) {
		next_token(in); /* skip = */
		do *missing = fgetc(in);
		while(isspace(*missing));
		if(*missing == EOF) break;
		}
	else if(strcmp(p, "INTERLEAVE") == 0) {
		*interleave = TRUE;
		}
	}
while(*p != ';');
return gap;
}


char **process_taxa_taxlabels(FILE *in, int ntax)
{
char *p, **names;
int i;

names = (char **)malloc(ntax * sizeof(char *));
if(names == NULL) ntax  = 0;
for(i = 0; i < ntax; i++) names[i] = NULL;
for(i = 0; i < ntax; i++) {
	p = next_token(in);
	if(p == NULL) break;
	if(*p == ';') break;
	names[i] = (char *)malloc(strlen(p)+1);
	if(names[i] == NULL) break;
	strcpy(names[i], p);
	}
if(p != NULL && *p != ';') skip_command(in);
return names;
}


int process_block_taxa(FILE *in, char ***taxnames)
{
char *p;
int ntax = 0;

while(TRUE) {
	p = next_token(in);
	if(is_block_end(p)) break;
	if(strcmp(p, "DIMENSIONS") == 0) ntax = process_taxa_dim(in);
	else if(strcmp(p, "TAXLABELS") == 0) 
		*taxnames = process_taxa_taxlabels(in, ntax);
	else skip_command(in);
	}
return ntax;
}


int find_by_name_num(char *token, int nelt, char **elts)
{
int num;

for(num = 0; num < nelt; num++) {
	if(strcmp(token, elts[num]) == 0) return num;
	}
if(num >= nelt) {num = -1; sscanf(token, "%d", &num); num--; }
if(num < 0 || num >= nelt) num = -1;
return num;
}


char *build_list(FILE *in, int nelt, char **elts)
{
char *p, *list;
int num, previous = -1, need_range = FALSE, i;

list = (char *)malloc(nelt + 1);
if(list == NULL) { skip_command(in); return NULL; }
memset(list, '0', nelt); list[nelt] = 0;
while(TRUE) {
	p = next_token(in);
	if(p == NULL || *p == ';') break;
	if(strcmp(p, "-") == 0) {
		if(previous >= 0) need_range = TRUE;
		continue;
		}
	if(strcmp(p, ".") == 0) num = nelt - 1;
	else num = find_by_name_num(p, nelt, elts);
	if(num == -1) continue;
	list[num] = '1';
	if(need_range) {
		for(i = previous + 1; i < num; i++) list[i] = '1';
		need_range = FALSE;
		}
	previous = num;
	}
return list;
}


list_segments *build_list_pairs(FILE *in, int maxi)
{
char *p;
list_segments *list = NULL, *next, *elt;
int num, previous = -1, need_range = FALSE, debut, fin;

while(TRUE) {
	p = next_token(in);
	if(p == NULL || *p == ';') break;
	if(strcmp(p, "-") == 0) {
		if(previous >= 0) need_range = TRUE;
		continue;
		}
	if(strcmp(p, ".") == 0) num = maxi;
	else if(strcmp(p, "\\") == 0) { /* process syntax: from-to\step */
		p = next_token(in);
		if(p == NULL) return NULL;
		num = -1; sscanf(p, "%d", &num); /* num has the step value */
		if(num == -1 || list == NULL) { skip_command(in); return NULL; }
		debut = next->debut; fin = next->fin;
		next->fin = debut;
		debut += num;
		while(debut <= fin) {
			elt = (list_segments *)malloc(sizeof(list_segments));
			if(elt == NULL) { skip_command(in); return NULL; }
			elt->debut = elt->fin = debut;
			elt->next = NULL;
			next->next = elt;
			next = elt;
			debut += num;
			}
		continue;
		}
	else {num = -1; sscanf(p, "%d", &num); }
	if(num == -1) { skip_command(in); return NULL; }
	if(need_range) {
		next->fin = num;
		need_range = FALSE;
		}
	else	{
		elt = (list_segments *)malloc(sizeof(list_segments));
		if(elt == NULL) { skip_command(in); return NULL; }
		elt->debut = elt->fin = num;
		elt->next = NULL;
		if(list == NULL) list = elt;
		else	next->next = elt;
		next = elt;
		}
	previous = num;
	}
return list;
}


char **process_chars_m_interleave(FILE *in, int nsites, int gap, int ntaxa,
	char **taxnames, int define_taxlabels, int matchchar, int missing,
	int protein)
{
int num, c, newtaxa = 0;
char *p, **seq;

seq = (char **)malloc(ntaxa * sizeof(char *));
if(seq == NULL) { skip_command(in); return NULL;}
for(num = 0; num < ntaxa; num++) seq[num] = NULL;
while(TRUE) {
	p = next_token(in);
	if(p == NULL || *p == ';') break;
	if(define_taxlabels) {
		if(newtaxa >= ntaxa) {
			num = find_by_name_num(p, ntaxa, taxnames);
			}
		else	{
			taxnames[newtaxa] = (char *)malloc(strlen(p) + 1);
			if(taxnames[newtaxa] == NULL) num = -1;
			else 	{
				strcpy(taxnames[newtaxa], p);
				num = newtaxa++;
				}
			}
		}
	else num = find_by_name_num(p, ntaxa, taxnames);
	if(num == -1) { /* bad format */
		skip_command(in); free(seq);
		return NULL;
		}
	if(seq[num] == NULL) {
		seq[num] = (char *)malloc(nsites + 1);
		if(seq[num] == NULL) { 
			skip_command(in); free(seq); return NULL;
			}
		seq[num][0] = 0;
		}
	p = seq[num]; p += strlen(p);
	while(TRUE) {
		c = fgetc(in);
		if(c == ';' || c == '\n' || c == '\r' || c == EOF) break;
		if(isspace(c)) continue;
		if(c == gap) c = '-';
		if(p - seq[num] < nsites) *(p++) = c;
		}
	*p = 0;
	}
if(matchchar != 0) {
	for(num = 1; num < ntaxa; num++) {
		p = seq[num] - 1;
		while(*(++p) != 0) if(*p == matchchar) *p = seq[0][p - seq[num]];
		}
	}
/*
if(missing != 0) {
	char miss_char = (protein ? 'X' : 'N');
	for(num = 0; num < ntaxa; num++) {
		p = seq[num] - 1;
		while(*(++p) != 0) if(*p == missing) *p = miss_char;
		}
	}
*/
return seq;
}


char **process_chars_matrix(FILE *in, int nsites, int gap, int ntaxa, 
	char **taxnames, int define_taxlabels, int matchchar, int missing,
	int protein)
{
int num, c, newtaxa = 0, offset;
char *p, **seq;

seq = (char **)calloc(ntaxa , sizeof(char *));
if(seq == NULL) { skip_command(in); return NULL;}
for(num = 0; num < ntaxa; num++) seq[num] = NULL;
while(TRUE) {
	p = next_token(in);
	if(p == NULL || *p == ';') break;
	if(define_taxlabels) {
		if(newtaxa >= ntaxa) num = -1;
		else	{
			taxnames[newtaxa] = (char *)malloc(strlen(p) + 1);
			if(taxnames[newtaxa] == NULL) num = -1;
			else	{
				strcpy(taxnames[newtaxa], p);
				num = newtaxa++;
				}
			}
		}
	else 	{
		offset = 0;
		do 	{/* handle repetitions of names */
			num = find_by_name_num(p, ntaxa - offset, taxnames + offset);
			if(num != -1) num += offset;			
			offset = num + 1;
			}
		while(num != -1 && seq[num] != NULL);
		}
	if(num == -1) { /* bad format */
		skip_command(in);
		free(seq);
		return NULL;
		}
	seq[num] = (char *)malloc(nsites + 1);
	if(seq[num] == NULL)  { skip_command(in); free(seq); return NULL;}
	p = seq[num];
	do	{
		c = fgetc(in);
		if(c == ';' || c == EOF) break;
		if(c == '[')  {
			c = nextbracket(in);
			if(c == EOF) break;
			continue;
			}
		if(isspace(c)) continue;
		if(c == gap) c = '-';
		*(p++) = c;
		}
	while(p < seq[num] + nsites);
	*p = 0;
	if(c == ';' || c == EOF) break;
	}
for(num = 0; num < ntaxa; num++) {
	if(seq[num] == NULL) { 
		seq[num] = (char *)malloc(1);
		if(seq[num] == NULL)  { free(seq); return NULL;}
		seq[num][0] = 0;
		}
	}
if(matchchar != 0) {
	for(num = 1; num < ntaxa; num++) {
		p = seq[num] - 1;
		while(*(++p) != 0) if(*p == matchchar) *p = seq[0][p - seq[num]];
		}
	}
/*
if(missing != 0) {
	char miss_char = (protein ? 'X' : 'N');
	for(num = 0; num < ntaxa; num++) {
		p = seq[num] - 1;
		while(*(++p) != 0) if(*p == missing) *p = miss_char;
		}
	}
*/
return seq;
}


char **process_block_data(FILE *in, int *pntaxa, char ***ptaxnames, 
		int *nsites, int *protein)
{
char **seqs, **taxnames, *p;
int gap=0, interleave = FALSE, matchchar = 0, missing = 0;

seqs = NULL; *nsites = 0; taxnames = NULL; *pntaxa = 0;
while(TRUE) {
	p = next_token(in);
	if(is_block_end(p)) break;
	if(strcmp(p, "DIMENSIONS") == 0) { 
		*nsites = process_chars_dim(in, pntaxa);
		if(*pntaxa > 0) taxnames = 
			(char **)malloc(*pntaxa * sizeof(char *));
		if(*pntaxa > 0 && taxnames == NULL) {
			*pntaxa = 0; seqs = NULL;
			skip_block(in);
			break;
			}
		}
	else if(strcmp(p, "FORMAT") == 0) 
		gap = process_chars_format(in, protein, &interleave, &matchchar,
			&missing);
	else if(strcmp(p, "MATRIX") == 0) {
		if(interleave)
			seqs = process_chars_m_interleave(in, *nsites, gap,
				*pntaxa, taxnames, TRUE, matchchar, missing, *protein);
		else
			seqs = process_chars_matrix(in, *nsites, gap,
				*pntaxa, taxnames, TRUE, matchchar, missing, *protein);
		}
	else skip_command(in);
	}
*ptaxnames = taxnames;
return seqs;
}


char **process_block_characters(FILE *in, int *pntaxa, char ***taxnames,
int *nsites, int *protein)
{
char *p, **seqs;
int interleave=0, gap, matchchar = 0, missing = 0;

seqs = NULL; *nsites = 0;
while(TRUE) {
	p = next_token(in);
	if(is_block_end(p)) break;
	if(strcmp(p, "DIMENSIONS") == 0) 
		*nsites = process_chars_dim(in, pntaxa);
	else if(strcmp(p, "FORMAT") == 0) 
		gap = process_chars_format(in, protein, &interleave, &matchchar,
			&missing);
	else if(strcmp(p, "TAXLABELS") == 0) 
		*taxnames = process_taxa_taxlabels(in, *pntaxa);
	else if(strcmp(p, "MATRIX") == 0) {
		if(interleave)
			seqs = process_chars_m_interleave(in, *nsites, gap,
				*pntaxa, *taxnames, FALSE, matchchar, missing, *protein);
		else
			seqs = process_chars_matrix(in, *nsites, gap,
				*pntaxa, *taxnames, FALSE, matchchar, missing, *protein);
		}
	else skip_command(in);
	}
return seqs;
}


char **process_block_notes(FILE *in, int ntaxa, char **taxnames)
{
char *p, **notes;
int num;

/* notes[0 - ntaxa[ comments des seqs indiv */
notes = (char **)malloc(ntaxa * sizeof(char *));
if(notes == NULL) {skip_block(in); return NULL; }
for(num = 0; num < ntaxa; num++) notes[num] = NULL;
while(TRUE) {
	p = next_token(in);
	if(is_block_end(p)) break;
	if(strcmp(p, "TEXT") == 0) 
		process_notes_text(in, ntaxa, taxnames, notes);
	else skip_command(in);
	}
return notes;
}


char *process_sets_taxset(FILE *in, int ntaxa, char **taxnames, char **setname)
{
char *p, *set = NULL;

p = next_token(in);
if(p != NULL) *setname = (char *)malloc(strlen(p) + 1);
if( p == NULL || *setname == NULL) { skip_command(in); return NULL; }
strcpy(*setname, p);
next_token(in); /* skip = */
set = build_list(in, ntaxa, taxnames);
return set;
}


list_segments *process_sets_charset(FILE *in, char **setname, int max_chars)
{
char *p;

p = next_token(in);
if(p != NULL) *setname = (char *)malloc(strlen(p) + 1);
if( p == NULL || *setname == NULL) { skip_command(in); return NULL; }
strcpy(*setname, p);
next_token(in); /* skip = */
return build_list_pairs(in, max_chars);
}


int process_translate(FILE *in, char ***ptaxnames)
{
  char *p, **taxnames;
  int rank;
  
  taxnames = (char **) malloc(10000*sizeof(char*));
  while (TRUE)
  {
    p = next_token(in);
    if (*p == ',') continue;
    if (*p == ';') break;
    sscanf(p, "%d", &rank);
    p = next_token(in);
    taxnames[rank - 1] = strdup(p);
    }
  taxnames = (char **)realloc(taxnames, rank * sizeof(char*));
  *ptaxnames = taxnames;
  return rank;
}

char *replace_ranks_by_names(char *tree, int ntaxa, char **taxnames)
{
  char *p = tree, *q, *r;
  char *newtree;
  char bootstrap_comment[50];
  int rank, l;
  float pp;
  
  l = strlen(tree);
  for (int i = 0; i < ntaxa; i++) {
    l += strlen(taxnames[i]);
    }
  newtree = (char*)malloc(l);
  q = newtree;
  while (TRUE) {
    if (*p == ';') break;
    if (*p == '[') { // skip comments
      do p++; while (*p != ']');
      p++;
      continue;
    }
    if (*p == ')' && *(p+1) == '[') { // a comment with a PP value
      *q++ = *p++;
      r = bootstrap_comment;
      do {
	*r = *p++;
	if ((unsigned)(r - bootstrap_comment) < sizeof(bootstrap_comment) - 1) r++;
      }
      while (*p != ']');
      p++;
      *r = 0;
      if ( strncmp(bootstrap_comment, "[&prob=", 7) == 0) {
	sscanf(bootstrap_comment + 7, "%f", &pp);
	sprintf(q, "%.2f", pp);
	q += strlen(q);
	}
      continue;
      }
    if ( (*p == '(' || *p == ',') && isdigit(*(p+1))) { // a taxon rank is found
      *q++ = *p++;
      sscanf(p, "%d", &rank);
      sprintf(q, "%s", taxnames[rank - 1]);
      q += strlen(q);
      p++;
      while (isdigit(*p)) p++;
      continue;
      }
    *q++ = *p++;
    }
  *q++ = ';';
  *q = 0;
  free(tree);
  newtree = (char *)realloc(newtree, (q - newtree) + 1);
  return newtree;
}

int process_block_trees(FILE *in, char ***ptrees, char ***pnames)
{
	char **trees, **names, **taxnames;
	char *p, c;
	int maxi = 5, ntaxa = 0;
	trees = (char **)calloc(maxi, sizeof(char *));
	names = (char **)calloc(maxi, sizeof(char *));
	int count = 0;
  while(TRUE) {
    p = next_token(in);
    if(is_block_end(p)) break;
    if(strcmp(p, "TRANSLATE") == 0) {
      ntaxa = process_translate(in, &taxnames);
		  }
    if(strcmp(p, "TREE") == 0) {
      if(count >= maxi) {
        maxi += 5;
        trees = (char **)realloc(trees, maxi * sizeof(char *));
        names = (char **)realloc(names, maxi * sizeof(char *));
      }
      names[count] = strdup(next_token(in));
      next_token(in); //skip =
      while ( isspace(c=fgetc(in)) ) ; // skip white space
      trees[count] = (char *)malloc(500000);
      p = trees[count];
      do {
        *p++ = c; // first char (can be '[' for tree header)
        while(TRUE) {
          *p = fgetc(in);
          if(*p == '\n' || *p == '\r') continue;
          if(*p++ == ';') break;
        }
        do c = fgetc(in); while (isspace(c));
      }
      while (c == '(' || c == '[');
      ungetc(c, in);
      *p = 0;
      trees[count] = (char *)realloc(trees[count] , p - trees[count] + 1);
      if (ntaxa > 0) trees[count] = replace_ranks_by_names(trees[count], ntaxa, taxnames);
      count++;
    }
		}
	trees = (char **)realloc(trees, count * sizeof(char *));
	names = (char **)realloc(names, count * sizeof(char *));
	*ptrees = trees;
	*pnames = names;
  if (ntaxa > 0) {
    for (int i = 0; i < ntaxa; i++) free(taxnames[i]);
    free(taxnames);
    }
	return count;
}

void process_block_sets(FILE *in, int ntaxa, char **taxnames, int max_chars,
		linked_strings **taxsets, list_regions **charsets)
{
char *p, *set, *setname;
list_segments *limits;
linked_strings *chain_tax = *taxsets, *elt;
list_regions *elt_reg, *chain_char = *charsets, *end_chain;
region *maregion;

end_chain = chain_char;
if(end_chain != NULL) while(end_chain->next != NULL) end_chain =
end_chain->next;
while(TRUE) {
	p = next_token(in);
	if(is_block_end(p)) break;
	if(strcmp(p, "TAXSET") == 0 && ( set = process_sets_taxset(in, 
			ntaxa, taxnames, &setname)) != NULL ) {
		elt = (linked_strings *)malloc(sizeof(linked_strings));
		if(elt != NULL) {
			elt->data = (void *)set;
			elt->name = setname;
			elt->next = chain_tax;
			chain_tax = elt;
			}
		}
	else if(strcmp(p, "CHARSET") == 0 && (limits = 
		   process_sets_charset(in, &setname, max_chars)) != NULL) {
		maregion = (region *)calloc(1, sizeof(region));
		elt_reg = (list_regions *)malloc(sizeof(list_regions));
		if(maregion != NULL && elt_reg != NULL) {
			maregion->list = limits;
			maregion->name = setname;
			elt_reg->element = maregion;
			elt_reg->next = NULL;
			if(end_chain != NULL) end_chain->next = elt_reg;
			else chain_char = elt_reg;
			end_chain = elt_reg;
			}
		}
	else skip_command(in);
	}
*taxsets = chain_tax;
*charsets = chain_char;
return;
}


void process_notes_text(FILE *in, int ntaxa, char **taxnames, char **notes)
{
char *p, *q;
int num, count;

num = -1;
do	{
	p = next_token(in);
	if(p == NULL) break;
	majuscules(p);
	if(strcmp(p, "TAXON") == 0) {
		next_token(in); /* skip = */
		p = next_token(in);
		if(p == NULL) break;
		num = find_by_name_num(p, ntaxa, taxnames);
		if(num == -1) { skip_command(in); break; }
		}
	else if(strcmp(p, "TEXT") == 0) {
		if(num == -1) { skip_command(in); break; }
		next_token(in); /* skip = */
		p = next_token(in);
		if(p == NULL) break;
		q = p; count = 0;
		while( (q = strchr(q, '\n')) != NULL) { count++; q++; }
		notes[num] = (char *)malloc(strlen(p) + 5 + count);
		if(notes[num] != NULL) {
			q = notes[num];
			*q = ';'; q++;
			while(*p != 0) {
				*q = *p;
				if(*p == '\n' && *(p+1) != 0) *(++q) = ';';
				q++; p++;
				}
			*q = 0;
			}
		}
	}
while(*p != ';');
}


linked_strings *process_block_seaview(FILE *in, char **header)
{
char *p, *q, *name, *text;
int count;

linked_strings *chain = NULL, *elt;
while(TRUE) {
	p = next_token(in);
	if(is_block_end(p)) break;
	if(strcmp(p, "TEXT") == 0 ) {
		text = process_seaview_text(in, &name);
		if(text == NULL) continue;
		if(name == NULL) {
			/* text contains a header: add ;; at start of lines */
			q = text; count = 0;
			while( (q = strchr(q, '\n')) != NULL) { count++; q++; }
			*header = (char *)malloc(strlen(text) + 5 + 2 * count);
			if(*header != NULL) {
				q = *header; p = text;
				*q = ';'; q++; *q = ';'; q++;
				while(*p != 0) {
					*q = *p;
					if(*p == '\n' && *(p+1) != 0) {
						*(++q) = ';'; *(++q) = ';';
						}
					q++; p++;
					}
				*q = 0;
				}
			}
		else	{ /* text contains a footer */
			/* remove line breaks from footer */
			p = text;
			q = (char *)malloc(strlen(p) + 1);
			elt = (linked_strings *)malloc(sizeof(linked_strings));
			if(q != NULL && elt != NULL) {
				elt->data = (void *)q;
				do 	{
					if(*p != '\n' && *p != '\r') *(q++)= *p;
					}
				while (*(p++) != 0);
				elt->name = name;
				elt->next = chain;
				chain = elt;
				}
			}
		free(text);
		}
	else skip_command(in);
	}
return chain;
}


char *process_seaview_text(FILE *in, char **name)
{
char *p, *text = NULL;

*name = NULL;
do	{
	p = next_token(in);
	if(p == NULL) break;
	majuscules(p);
	if(strcmp(p, "FOOTER") == 0) {
		next_token(in); /* skip = */
		p = next_token(in);
		if(p == NULL) break;
		*name = (char *)malloc(strlen(p) + 1);
		if(*name == NULL) { skip_command(in); return NULL; }
		strcpy(*name, p);
		}
	else if(strcmp(p, "HEADER") == 0) {
		*name = NULL;
		}
	else if(strcmp(p, "TEXT") == 0) {
		next_token(in); /* skip = */
		p = next_token(in);
		if(p == NULL) break;
		text = (char *)malloc(strlen(p) + 1);
		if(text != NULL) strcpy(text, p);
		}
	}
while(*p != ';');
return text;
}


char **process_block_unaligned(FILE *in, int *ntaxa, char ***taxnames, int
*nsites, int *protein)
{
char *p, **seqs = NULL;
int i, l, max_l;

*protein = FALSE;
while(TRUE) {
	p = next_token(in);
	if(is_block_end(p)) break;
	if(strcmp(p, "DIMENSIONS") == 0) *ntaxa = process_unal_dim(in);
	else if(strcmp(p, "FORMAT") == 0) *protein = process_unal_format(in);
	else if(strcmp(p, "TAXLABELS") == 0) 
		*taxnames = process_taxa_taxlabels(in, *ntaxa);
	else if(strcmp(p, "MATRIX") == 0) {
		seqs = process_unal_matrix(in, *ntaxa, *taxnames);
		}
	else skip_command(in);
	}
l = max_l = 0;
for(i = 0; seqs != NULL && i < *ntaxa; i++) {
	if(seqs[i] != NULL) l = strlen(seqs[i]);
	if(l > max_l) max_l = l;
	}
*nsites = max_l;
return seqs;
}


int process_unal_dim(FILE *in)
{
char *p;
int number = 0;

p = next_token(in);
if(p!=NULL)majuscules(p);
if(p == NULL || strcmp(p, "NEWTAXA") != 0) { skip_command(in); return 0; }
p = next_token(in);
if(p!=NULL)majuscules(p);
if(p == NULL || strcmp(p, "NTAX") != 0) { skip_command(in); return 0; }
next_token(in); /* skip = */
p = next_token(in);
if(p != NULL) sscanf(p, "%d", &number);
if(number <= 0) number = 0;
if(p != NULL && *p != ';') skip_command(in);
return number;
}


int process_unal_format(FILE *in)
{
char *p;
int protein = 0;

p = next_token(in);
if(p!=NULL)majuscules(p);
if(p == NULL || strcmp(p, "DATATYPE") != 0) { skip_command(in); return 0; }
next_token(in); /* skip = */
p = next_token(in);
if(p!=NULL)majuscules(p);
if(p != NULL) protein = (strcmp(p, "PROTEIN") == 0);
if(p != NULL && *p != ';') skip_command(in);
return protein;
}


char **process_unal_matrix(FILE *in, int ntaxa, char **taxnames)
{
int num, c, lseq;
char *p, **seq, *q;
const int block = 5000;

seq = (char **)malloc(ntaxa * sizeof(char *));
if(seq == NULL) { skip_command(in); return NULL;}
for(num = 0; num < ntaxa; num++) seq[num] = NULL;
while(TRUE) {
	p = next_token(in);
	if(p == NULL) break;
	num = find_by_name_num(p, ntaxa, taxnames);
	if(num == -1) { /* bad format */
		skip_command(in);
		return seq;
		}
	lseq = 0;
	p = seq[num];
	while(TRUE) {
		c = fgetc(in);
		if(c == ';' || c == ',' || c == EOF) break;
		if(isspace(c)) continue;
		if(p >= seq[num] + lseq) {
			lseq += block;
			q = (char *)malloc(lseq + 1);
			if(q == NULL)  { 
				skip_command(in); free(seq); return NULL;
				}
			if(p > seq[num]) memcpy(q, seq[num], p - seq[num]);
			p = q + (p - seq[num]);
			if(seq[num] != NULL) free(seq[num]);
			seq[num] = q;
			}
		*(p++) = c;
		}
	*p = 0;
	if(c == ';' || c == EOF) break;
	}
return seq;
}


int read_nexus_align(char *fname, char ***pseqs, char ***pseqnames, 
	char ***pcomments, char **pheader, char **err_message, 
	list_regions **charsets, int *pnum_species_sets,
	int ***list_species_sets, char ***name_species_sets,
	int *ptot_comment_lines, char ***comment_name, char ***comment_line,
	int **comment_length, int *protein, SEA_VIEW *view)
{
FILE *in;
int lseqs, i, tot, tot_trees = 0;
char **tree_names, **trees = NULL;

if( (in = is_nexus_file(fname) ) == NULL) {
	*err_message = (char *)"not a NEXUS file";
	return 0;
	}
tot = read_nexus_file(in, pseqs, &lseqs, protein, pseqnames, 
	pcomments, pheader,
	pnum_species_sets, list_species_sets, name_species_sets,
	charsets, ptot_comment_lines, comment_name, comment_line,
	&tot_trees, &trees, &tree_names);
if(tot == 0 && tot_trees == 0) {
  *err_message = (char *)"format error";
  return 0;
  }
for(i = 0; i < tot_trees; i++) {
  view->menu_trees->add(tree_names[i], trees_callback, NULL, 0);
  int rank = view->menu_trees->vlength();
  (view->menu_trees->vitem(rank - 1))->labelfont(FL_HELVETICA_ITALIC);
  free(tree_names[i]);
  }
if(tot_trees > 0) {
  free(tree_names);
  view->trees = (char**)realloc(view->trees, (view->tot_trees + tot_trees) * sizeof(char*));
  memcpy(view->trees + view->tot_trees, trees, tot_trees * sizeof(char*));
  }
view->tot_trees += tot_trees;
if(*pcomments == NULL) {
	*pcomments = (char **)calloc(tot, sizeof(char *));
	}
if(*ptot_comment_lines > 0) {
	*comment_length = (int *)malloc(*ptot_comment_lines * sizeof(int));
	if(*comment_length == NULL) *ptot_comment_lines = 0;
	for(i = 0; i < *ptot_comment_lines; i++) 
		(*comment_length)[i] = strlen((*comment_line)[i]);
	}
return tot;
}



int read_nexus_file(FILE *in, char ***pseqs, int *nchars, int *protein,
	char ***taxnames, char ***notes, char **header,
	int *pnum_species_sets, int ***list_species_sets, 
	char ***name_species_sets,
	list_regions **charsets,
	int *tot_comment_lines, char ***comment_name, char ***comment_line,
	int *p_ntrees, char ***p_trees, char ***p_tree_names)
{
char *p, **seqs = NULL;
linked_strings *footers, *taxsets, *tmplist;
int ntaxa, count, num;

ntaxa = *nchars = *tot_comment_lines = *pnum_species_sets = 0; 
*protein = FALSE;
*taxnames = *notes = NULL; *header = NULL;
taxsets = footers = NULL;
*charsets = NULL;
*name_species_sets = NULL; *list_species_sets = NULL;
while(TRUE) {
	p = next_block_name(in);
	if(p == NULL) break;
	if(strcmp(p, "TAXA") == 0) ntaxa = process_block_taxa(in, taxnames);
	else if(strcmp(p, "CHARACTERS") == 0) 
		seqs = process_block_characters(in, &ntaxa, taxnames, 
			nchars, protein);
	else if(strcmp(p, "UNALIGNED") == 0) 
		seqs = process_block_unaligned(in,
		&ntaxa, taxnames, nchars, protein);
	else if(strcmp(p, "DATA") == 0) seqs = process_block_data(in, &ntaxa,
		taxnames, nchars, protein);
	else if(strcmp(p, "NOTES") == 0) *notes = process_block_notes(in,
		ntaxa, *taxnames);
	else if(strcmp(p, "SETS") == 0) process_block_sets(in, ntaxa,
		*taxnames, *nchars, &taxsets, charsets);
	else if(strcmp(p, "TREES") == 0) *p_ntrees = process_block_trees(in, p_trees, p_tree_names);
	else if(strcmp(p, "SEAVIEW") == 0) 
		footers = process_block_seaview(in, header);
	else skip_block(in);
	}
fclose(in);
if(seqs == NULL) {
	*pseqs = NULL;
	if(*taxnames != NULL) 
		for(num = 0; num < ntaxa; num++) 
			if((*taxnames)[num] != NULL) free((*taxnames)[num]);
	return 0;
	}
if(footers != NULL)
	*tot_comment_lines = linked_to_series(footers, comment_name,
		comment_line);

count = 0; tmplist = taxsets;
while(tmplist != NULL) {
	tmplist = tmplist->next; count++;
	}
if(count > 0) {
	*name_species_sets = (char **)malloc(count * sizeof(char *));
	if(*name_species_sets == NULL) count = 0;
	*list_species_sets = (int **)malloc(count * sizeof(int *));
	if(*list_species_sets == NULL) count = 0;
	else	{
		for(num = 0; num < count; num++) {
			(*list_species_sets)[num] = 
					(int *)calloc(ntaxa, sizeof(int));
			if((*list_species_sets)[num] == NULL) count = 0;
			}
		}
	if(count == 0) taxsets = NULL;
	}
*pnum_species_sets = count;
while(taxsets != NULL) {
	count--;
	(*name_species_sets)[count] = taxsets->name;
	p = (char *)taxsets->data - 1;
	while( (p = strchr(p + 1, '1')) != NULL) {
		num = p - (char *)taxsets->data;
		(*list_species_sets)[count][num] = TRUE;
		}
	free(taxsets->data);
	tmplist = taxsets;
	taxsets = taxsets->next;
	free(tmplist);
	}
*pseqs = seqs;
return ntaxa;
}


int linked_to_series(linked_strings *linked, char ***names, char ***lines)
{
int i, count = 0;
linked_strings *elt, *old;

elt = linked;
while(elt != NULL) { count++; elt = elt->next; }
*names = (char **)malloc(count *sizeof(char *));
*lines = (char **)malloc(count *sizeof(char *));
if(*names == NULL || *lines == NULL) return 0;
elt = linked; i = count;
while(elt != NULL) {
	i--;
	(*names)[i] =  elt->name;
	(*lines)[i] = (char *)elt->data;
	old = elt;
	elt = elt->next;
	free(old);
	}
return count;
}


char *out_with_apostrophe(char *p)
{
static char out[1000];
char *q = out;

if (p == NULL) return NULL;
if(strchr(p, APOSTROPHE) == NULL) return p;
while (TRUE) {
	*(q++) = *p;
	if(*p == 0) break;
	if(*p == APOSTROPHE) *(q++) = APOSTROPHE;
	p++;
	}
return out;
}


char *out_with_optional_protection(char *p)
{
static char out[1000];
char *q = p - 1;
int protect = FALSE;

while(*(++q) != 0) {
	if(*q == ' ' || strchr(PUNCTUATION, *q) != NULL) {
		protect = TRUE;
		break;
		}
	}
if(!protect) return p;
else 	{
	sprintf(out, "'%s'", out_with_apostrophe(p));
	return out;
	}
}


void out_table_by_series(FILE *out, int *table, int dim)
{
int num, pre = -1;

for(num = 0; num < dim; num++) {
	if(!table[num]) { pre = -1; continue; }
	if(pre == - 1) {
		fprintf(out, " %d", num + 1);
		pre = num;
		continue;
		}
	if(num < dim - 1 && table[num + 1]) continue;
	fprintf(out, "-%d", num + 1);
	pre = -1;
	}
}


int save_nexus_file(const char *fname, int ntaxa, int protein,
	char **seqs, char **taxnames, char **notes, char *header,
	int num_species_sets, int **list_species_sets, 
	char **name_species_sets,
	list_regions *charsets,
	int tot_comment_lines, char **comment_name, char **comment_line,
	region *region_used, int *sel_seqs, int tot_sel_seqs, int *eachlength,
	int tot_trees, char **trees, const Fl_Menu_Item *items)
{
FILE *out;
int i, j, lmax, num, vtotseqs, current, ecrit;
char *p, date_ligne[50];
list_segments *psegment, all_sequence;
region maregion;
time_t heure;

out = fopen(fname, "w");
if(out == NULL) return TRUE;

if(region_used == NULL) { /* on veut tout sauver */
	tot_sel_seqs = 0;
	all_sequence.debut = 1;
	all_sequence.fin = eachlength[0];
	for(i = 1; i < ntaxa; i++)
		if( all_sequence.fin < eachlength[i] )  
			 all_sequence.fin = eachlength[i];
	all_sequence.next = NULL;
	maregion.list = &all_sequence;
	region_used = &maregion;
	}
/* calcul longueur des regions */
lmax = 0;
psegment = region_used->list;
while(psegment != NULL) {
	lmax += psegment->fin - psegment->debut + 1;
	psegment = psegment->next;
	}
vtotseqs = 0;
for(i=0; i < ntaxa; i++) 
	if(tot_sel_seqs == 0 || sel_seqs[i]) ++vtotseqs;


time(&heure);
strcpy(date_ligne, ctime(&heure));
num = strlen(date_ligne) - 1; if(date_ligne[num] == '\n') date_ligne[num] = 0;
fprintf(out,"#NEXUS\n[saved by seaview on %s]\n", date_ligne);
fprintf(out, "BEGIN DATA;\n  DIMENSIONS NTAX=%d NCHAR=%d;\n", vtotseqs, lmax);
fprintf(out, "  FORMAT DATATYPE=%s\n  GAP=-\n  ;\nMATRIX\n",
	(protein ? "PROTEIN" : "DNA") );

num = 0;
for(i = 0; i < ntaxa; i++) {
	if(tot_sel_seqs != 0 && ! sel_seqs[i]) continue;
	fprintf(out, "[%d] %s\n", ++num, out_with_optional_protection(taxnames[i]));

	current = 0; psegment = region_used->list;
	all_sequence.fin = eachlength[i];
	do	{
		ecrit = output_next_res_from_region(seqs[i], eachlength[i], 
			&psegment, &current, out, 60, FALSE);
		if( ecrit > 0) putc('\n', out);
		else if(ecrit == -1) break;
		}
	while(ecrit != 0);
	if( (ecrit = lmax - eachlength[i]) > 0) {
		for(j = 1; j <= ecrit; j++) { 
			putc('-', out); if(j % 60 == 0) putc('\n', out); 
			}
		putc('\n', out);
		}
	if(ferror(out)) break;
	}
fprintf(out, ";\nEND;\n");

if(notes != NULL) {
	num = 0; current = 0;
	for(i = 0; i < ntaxa; i++) {
		if(tot_sel_seqs != 0 && ! sel_seqs[i]) continue;
		++num;
		if(notes[i] == NULL || strcmp(notes[i], ";") == 0 ||
			strcmp(notes[i], ";\n") == 0) continue;
		if(current == 0) { fputs("BEGIN NOTES;\n", out); current++; }
		fprintf(out, "  TEXT TAXON=%d TEXT='", num);
		p = notes[i] + 1;
		while(*p != 0) {
			fputc(*p, out);
			if(*p == APOSTROPHE) fputc(APOSTROPHE, out);
			if(*p == '\n' && *(p+1) != 0) p++;
			p++;
			}
		fputs("';\n", out);
		}
	if(current != 0) fprintf(out, "END;\n");
	}

if(num_species_sets != 0 || charsets != NULL) {
	fprintf(out, "BEGIN SETS;\n");
	for(num = 0; num < num_species_sets; num++) {
		fprintf(out, "  TAXSET '%s' =",
out_with_apostrophe(name_species_sets[num]));
		out_table_by_series(out, list_species_sets[num], ntaxa);
		fputs(";\n", out);
		}
	while(charsets != NULL) {
		list_segments *pair;
		fprintf(out, "  CHARSET '%s' =",
out_with_apostrophe(charsets->element->name));
		pair = charsets->element->list;
		while(pair != NULL) {
			if(pair->debut != pair->fin)
				fprintf(out, " %d-%d", pair->debut, pair->fin);
			else fprintf(out, " %d", pair->debut);
			pair = pair->next;
			}
		fputs(";\n", out);
		charsets = charsets->next;
		}
	fprintf(out, "END;\n");
	}
if(tot_trees != 0) {
	fprintf(out, "BEGIN TREES;\n");
	for(num = 0; num < tot_trees; num++) {
		fprintf(out, " TREE '%s'= ", (items+num)->label());
		int l = 0;
		p = trees[num];
		do {
			fputc(*p, out);
			if(++l > 70 && (*p == ',' || *p == ')')) { fputc('\n', out); l = 0; }
			p++;
			}
		while(*p != 0);
		fputc('\n', out);
		}
	fprintf(out, "END;\n");
	}
if(header != NULL || tot_comment_lines != 0) {
	fprintf(out, "BEGIN seaview;\n");
	if(header != NULL) {
		fputs("  TEXT HEADER TEXT='", out);
		p = header + 2;
		while(*p != 0) {
			fputc(*p, out);
			if(*p == APOSTROPHE) fputc(APOSTROPHE, out);
			if(*p == '\n' && *(p+1) != 0) p += 2;
			p++;
			}
		fputs("';\n", out);
		}
	for(i = 0; i < tot_comment_lines; i++) {
		fprintf(out, "  TEXT FOOTER='%s' TEXT='",
			out_with_apostrophe(comment_name[i]));
		p = comment_line[i];
		j = 0;
		while(*p != 0) {
			fputc(*p, out); j++;
			if(*p == APOSTROPHE) fputc(APOSTROPHE, out);
			if(j >= 50) { fputc('\n', out); j = 0; }
			p++;
			}
		fputs("';\n", out);
		}
	fprintf(out, "END;\n");
	}

fclose(out);
return FALSE;
}
