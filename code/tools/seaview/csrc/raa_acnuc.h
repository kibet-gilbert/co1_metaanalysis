#ifndef RAA_ACNUC_H
#define RAA_ACNUC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#if defined(WIN32)
#define SOCKBUFS 8192
#endif
#ifdef __alpha
typedef long raa_long;
#define RAA_LONG_FORMAT "%lu"
#else
typedef long long raa_long;
#define RAA_LONG_FORMAT "%llu"
#endif

#ifndef TRUE
#define FALSE 0
#define TRUE (!FALSE)
#endif
#ifndef ERREUR
#define ERREUR 1
#endif

struct rlng {         /* LONGL series of linked records containing lists of SUBSEQ ranks */
	int next; /* to LONGL for next element of the long list, or 0 when list is finished */
	int sub[1]; /* array (length given by global SUBINLNG) of ranks of SUBSEQ records or of 0s */
	};

#define RAA_GFRAG_BSIZE 10000
struct gfrag_aux {
	char buffer[RAA_GFRAG_BSIZE + 1];
	int lbuf, nseq_buf, first_buf, l_nseq_buf;
	};

struct readsub_aux {
	char *name;
	int lname, previous, length, type, toext, lkey, locus, frame, gencode, div;
	raa_long addr;
	};

#define ANNOTCOUNT 40
struct annot_aux {
	 char *annotline[ANNOTCOUNT + 1];
	 int annotcurrent, annotcount, annotdiv;
	 raa_long annotaddr, annotaddrfirst, annotaddrlast;
	 char annotsbuffer[ 300 ];
	 };

struct readsp_kw_aux {
	char name[150], libel[200];
	int lsub, desc, syno, host, previous;
	};

#define BLOCK_ELTS_IN_LIST 500
struct nextelt_aux {
	 int current_rank, previous, total;
	 int tabnum[BLOCK_ELTS_IN_LIST];
	 char *tabname[BLOCK_ELTS_IN_LIST];
	 int tablength[BLOCK_ELTS_IN_LIST];
	 raa_long taboffset[BLOCK_ELTS_IN_LIST];
	 int tabdiv[BLOCK_ELTS_IN_LIST];
	 };
	
#define S_BUF_SHRT 5000 /* number of memorized SHORTL records */
struct readshrt_aux {
	 unsigned shrt_buffer[S_BUF_SHRT][3]; /* [0]=SHORTL_rank [1]=val [2]=next */
	 int shrt_max, shrt_begin, total;
	};

typedef struct shrt2_list {
  int length;
  int size;
  unsigned point;
  unsigned next;
  unsigned vals[1];
} shrt2_list;

struct readsmj_aux {
	 int lastrec;
	 char **names, **libels;
	 unsigned *plongs;
	};

typedef struct raa_node {
	char *name;
	char *libel;
	char *libel_upcase;
	int rank;
	int tid;
	int count;
	struct raa_node *parent;
	struct raa_pair *list_desc;
	struct raa_node *syno;
	} raa_node;
struct raa_pair {
	raa_node *value;
	struct raa_pair *next;
	};

#define WIDTH_MAX 150

typedef enum { raa_sub_of_bib = 0, raa_spec_of_loc, raa_bib_of_loc, raa_aut_of_bib, raa_bib_of_aut,
  raa_sub_of_acc, raa_key_of_sub, raa_acc_of_loc } raa_shortl2_kind;

typedef struct _raa_db_access {
	char *dbname;
	FILE *raa_sockfdr, *raa_sockfdw;
	int genbank, embl, swissprot, nbrf;
	int nseq, longa, maxa;
	int L_MNEMO, WIDTH_SP, WIDTH_KW, WIDTH_SMJ, WIDTH_AUT, WIDTH_BIB, ACC_LENGTH, SUBINLNG, lrtxt, VALINSHRT2;
	char *version_string;
        int maxlists; /* max # of possible lists */
	raa_node **sp_tree; /* NULL or the full taxonomy tree */
	int max_tid; /* largest correct taxon ID value */
	int *tid_to_rank; /* NULL or tid-to-rank table */
	struct rlng *rlng_buffer;
	struct gfrag_aux gfrag_data;
	struct readsub_aux readsub_data;
	int first_recs[20];
	struct annot_aux annot_data;
	struct readsp_kw_aux readspec_data, readkey_data;
	struct nextelt_aux nextelt_data;
	struct readshrt_aux readshrt_data;
	struct shrt2_list *readshrt2_data[raa_acc_of_loc+1];
	struct readsmj_aux readsmj_data;
	void *matchkey_data;
	/* mostly for raa_query/raa_query_win usage */
	int tot_key_annots;
	char **key_annots, **key_annots_min;
	unsigned char *want_key_annots;
#ifdef WIN32
	char sock_input[SOCKBUFS]; /* WIN32 socket input buffer */
	char *sock_input_pos, *sock_input_end;
	char sock_output[SOCKBUFS]; /* WIN32 socket output buffer */
	unsigned sock_output_lbuf;
#endif
  char buffer[5000];
  char remote_file[300];
  char *full_line;
  int max_full_line;
  int was_here;
  char *namestr;
  char residuecount[30];
  char *help;
  int lhelp;
  int tmp_total;
  int tmp_current;
  int *tmp_prelist;
  char *translate_buffer;
  int num_5_partial;
  char mnemo[WIDTH_MAX];
  char species[WIDTH_MAX];
  char access[WIDTH_MAX];
  char author[WIDTH_MAX];
  char biblio[WIDTH_MAX];
  char descript[WIDTH_MAX];
  char date[12];
	} raa_db_access;


typedef enum { raa_sub = 0, raa_loc, raa_key, raa_spec, raa_shrt, raa_lng, raa_ext, raa_smj,
	raa_aut, raa_bib, raa_txt, raa_acc } raa_file;
typedef void (*raa_char_void_function)(raa_db_access *, char *);

/* global variables */
extern raa_char_void_function raa_error_mess_proc;/*this function sd call raa_acnucclose*/


extern int raa_acnucopen (const char *clientid, raa_db_access **psock) ;
extern int raa_decode_address(char *url, char **p_ip_name, int *socket, char **p_remote_db);
extern int raa_acnucopen_alt (const char *serveurName, int port, const char *db_name, const char *clientid, raa_db_access **psock);
extern int raa_open_socket(const char *serveurName, int port, const char *clientid, raa_db_access **psock);
extern int raa_opendb(raa_db_access *raa_current_db, const char *db_name);
int raa_opendb_pw(raa_db_access  *raa_current_db, const char *db_name, void *ptr, char *(*getpasswordf)(void *) );
extern int raa_gfrag(raa_db_access  *raa_current_db, int nsub, int first, int lfrag, char *dseq) ;
extern void raa_acnucclose(raa_db_access  *raa_current_db) ;
extern int raa_prep_acnuc_query(raa_db_access  *raa_current_db) ; 
extern int raa_proc_query(raa_db_access  *raa_current_db, char *query, char **message, char *nomliste, int *numlist, 
	int *count, int *locus, int *type) ;
int raa_nexteltinlist(raa_db_access  *raa_current_db, int first, int lrank, char **pname, int *plength) ;
int raa_nexteltinlist_annots(raa_db_access  *raa_current_db, int first, int lrank, char **pname, int *plength, 
	raa_long *paddr, int *pdiv);
raa_long scan_raa_long(char *txt);
int raa_seq_to_annots(raa_db_access  *raa_current_db, int numseq, raa_long *faddr, int *div);
char *print_raa_long(raa_long val, char *buffer);
char *raa_read_annots(raa_db_access  *raa_current_db, raa_long faddr, int div);
char *raa_next_annots(raa_db_access  *raa_current_db, raa_long *faddr);
char *raa_translate_cds(raa_db_access  *raa_current_db, int seqnum);
char raa_translate_init_codon(raa_db_access  *raa_current_db, int numseq);
int raa_iknum(raa_db_access  *raa_current_db, char *name, raa_file cas);
int raa_isenum(raa_db_access  *raa_current_db, char *name);
int raa_bcount(raa_db_access  *raa_current_db, int lrank);
void raa_bit1(raa_db_access  *raa_current_db, int lrank, int num);
void raa_bit0(raa_db_access  *raa_current_db, int lrank, int num);
int raa_btest(raa_db_access  *raa_current_db, int lrank, int num);
void raa_copylist(raa_db_access  *raa_current_db, int from, int to);
void raa_zerolist(raa_db_access  *raa_current_db, int rank);
void raa_setliststate(raa_db_access  *raa_current_db, int lrank, int locus, int type);
char *raa_getliststate(raa_db_access  *raa_current_db, int lrank, int *locus, int *type, int *count);
char *raa_residuecount(raa_db_access  *raa_current_db, int lrank);
int raa_getemptylist(raa_db_access  *raa_current_db, char *name);
int raa_setlistname(raa_db_access  *raa_current_db, int lrank, char *name);
int raa_getlistrank(raa_db_access  *raa_current_db, char *name);
int raa_releaselist(raa_db_access  *raa_current_db, int lrank);
int raa_countfilles(raa_db_access  *raa_current_db, int lrank);
int raa_alllistranks(raa_db_access  *raa_current_db, int **pranks);
int raa_fcode(raa_db_access  *raa_current_db, raa_file cas, char *name);
int raa_read_first_rec(raa_db_access  *raa_current_db, raa_file cas);
char *raa_readsub(raa_db_access  *raa_current_db, int num, int *plength, int *ptype, int *pext, int *plkey, int *plocus, 
	int *pframe, int *pgencode);
char *raa_readsub_pannots(raa_db_access  *raa_current_db, int num, int *plength, int *ptype, int *pext, int *plkey, int *plocus, 
	int *pframe, int *pgencode, raa_long *paddr, int *pdiv);
char *raa_readloc(raa_db_access  *raa_current_db, int num, int *sub, int *pnuc, int *spec, int *host, int *plref, 
	int *molec, int *placc, int *org);
char *raa_readspec(raa_db_access  *raa_current_db, int num, char **plibel, int *plsub, int *desc, int *syno, int *plhost);
char *raa_readkey(raa_db_access  *raa_current_db, int num, char **plibel, int *plsub, int *desc, int *syno);
char *raa_readsmj(raa_db_access  *raa_current_db, int num, char **plibel, int *plong);
char *raa_readacc(raa_db_access  *raa_current_db, int num, int *plsub);
char *raa_readaut(raa_db_access  *raa_current_db, int num, int *plsub);
char *raa_readbib(raa_db_access  *raa_current_db, int num, int *plsub, int *plaut, int *pj, int *py);
int raa_readext(raa_db_access  *raa_current_db, int num, int *mere, int *deb, int *fin);
int raa_readlng(raa_db_access  *raa_current_db, int num);
unsigned raa_readshrt(raa_db_access  *raa_current_db, unsigned point, int *val);
unsigned raa_followshrt2(raa_db_access *raa_current_db, int *p_point, int *p_rank, raa_shortl2_kind kind);
char *raa_ghelp(raa_db_access  *raa_current_db, char *fname, char *topic);
int raa_nextmatchkey(raa_db_access  *raa_current_db, int num, char *pattern, char **matching);
int raa_savelist(raa_db_access  *raa_current_db, int lrank, FILE *out, int use_acc, char *prefix);
int raa_modifylist(raa_db_access  *raa_current_db, int lrank, char *type /* "length" or "date" */, char *operation /* ">2000" */, 
	int *pnewlrank, int (*check_interrupt)(void) , int *p_processed);
int raa_knowndbs(raa_db_access  *raa_current_db, char ***pnames, char ***pdescriptions);
char *raa_short_descr(raa_db_access  *raa_current_db, int seqnum, char *text, int maxlen, raa_long pinf, int div, char *name);
void *raa_prep_extract(raa_db_access *raa_current_db, char *format, FILE *outstream, char *choix,
	 char *feature_name, char *bornes, char *min_bornes, char **message, int lrank);
int raa_extract_1_seq(void *opaque);
int raa_extract_interrupt(raa_db_access *raa_current_db, void *opaque);
void *raa_prep_coordinates(raa_db_access  *raa_current_db, int lrank, int seqnum, 
	char *operation, /* "simple","fragment","feature","region" */
	char *feature_name, char *bounds, char *min_bounds);
int *raa_1_coordinate_set(void *);
int raa_loadtaxonomy(raa_db_access *raa_current_db, char *rootname, 
	int (*progress_function)(int, void *), void *progress_arg, 
	int (*need_interrupt_f)(void *), void *interrupt_arg);
char *raa_get_taxon_info(raa_db_access *raa_current_db, char *name, int rank, int tid, int *p_rank, 
	int *p_tid, int *p_parent, struct raa_pair **p_desc_list);
char *raa_getattributes(raa_db_access *raa_current_db, const char *id,
	int *prank, int *plength, int *pframe, int *pgc, char **pacc, char **pdesc, char **pspecies, char **pseq);
char *raa_seqrank_attributes(raa_db_access *raa_current_db, int rank,
	int *plength, int *pframe, int *pgc, char **pacc, char **pdesc, char **pspecies, char **pseq);

int sock_fputs(raa_db_access  *raa_current_db, const char *line);
int sock_flush(raa_db_access  *raa_current_db);
char *read_sock(raa_db_access  *raa_current_db);


int trim_key(char *name); /* remove trailing spaces */
void majuscules(char *name);
int atoi_u(const char *p);
void compact(char *chaine);
int strcmptrail(char *s1, int l1, char *s2, int l2);


#endif  /* RAA_ACNUC_H  */
