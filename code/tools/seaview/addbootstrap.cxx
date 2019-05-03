#include "treedraw.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#ifndef FALSE
#define FALSE 0
#define TRUE (!FALSE)
#endif
#define lmot (8*sizeof(int))


/* structures */

typedef struct branche { /* une branche definie par ses deux extremites */
	struct noeud *bouta;
	struct noeud *boutb;
	char *br_label;
	int *bits;
	float count;
	struct branche *next_h;
	} branche;
#define s_noeud sizeof(struct noeud)


/* included functions */
int init_add_bootstrap(char *full_tree);
int process_one_replicate(char *one_tree, float w);
char *finish_add_bootstrap(int replicates);
static void free_arbre(struct noeud *racine, struct noeud *);
static void free_branches(branche *branch_tab);
static char *preptree(char *, int *, struct noeud **pracine, struct noeud ***ptabtax, 
	branche **);
static int loadphylip(char *arbre, int *prooted, struct noeud **, branche *);
struct noeud *unrootedset(char *deb, char *fin, branche **p_int_br, struct noeud **tabtax,
	branche *);
static char *recur_write_tree(struct noeud *centre, struct noeud *pere, char *arbre, char *finarbre,
	branche *);
static int get_br_from_bouts(struct noeud *a, struct noeud *b, branche *);
static int for_all_branches(struct noeud *centre, struct noeud *origine, branche *branches);
static int process_branch(struct noeud *cote1, struct noeud *cote2, branche *);
char *get_br_label(struct noeud *a, struct noeud *b, branche *);
inline int is_leave(struct noeud *n);
void set_leaves(struct noeud *centre, struct noeud *pere, int *bits);
static int hashvalue(int *bits, int modulo, int totbits);
void parcourir_branches_each(struct noeud *centre, struct noeud *origine, 
	float w);
void process_branch_each(struct noeud *cote1, struct noeud *cote2, float w);
branche *find_branch(int *bits);
void add_hash(branche *br);
int recodeleaves(struct noeud *node, struct noeud *parent, struct noeud **tabtax);
static void use_count(branche *branches, int count);	
void bit0(int *plist, int num);
void bit1(int *plist, int num);
int testbit(int *plist, int num);
void non(int *listnon, int *list, int len);
char *compute_consensus_tree(char **seqnames, int **rows, int branch_count);
void init_consensus_calculation(int);
static int recodeleaves_consensus(struct noeud *node, struct noeud *parent, char **seqnames);
static branche *find_branch_consensus(int *bits);
static void process_branch_consensus(struct noeud *cote1, struct noeud *cote2);
static void parcourir_branches_consensus(struct noeud *centre, struct noeud *origine);
int process_one_consensus_item(char *one_tree, char **seqnames);
int after_consensus_items(float threshold, int item_count, int ***prows);

/*  extern functions */
extern char *nextpar(char *pospar);
extern double place_midpoint_root(struct noeud *from, struct noeud *racine, int notu, int fixed_outgroup);
extern const char *make_binary_or_unrooted(char *arbre);


/* globals  */
static int w_bits, modulo;
static int has_br_length = 0, notu /* counted from 1 */, totbranches /* counted from 1 */, 
	num_noeud, nextotu;
static branche **hashtable;
static struct noeud **maintabtax;
static branche *mainbranches;
static struct noeud *racine;
static int taille;
static int *candidate_bits;

int init_add_bootstrap(char *full_tree)
// returns != 0 iff error
{
	char *message;
	int err;
	message = preptree(full_tree, &taille, &racine, &maintabtax, &mainbranches);
	if(message != NULL || racine == NULL) return 1;
	taille = (int)(1.5 * taille + 100);
	w_bits = (notu-1) / lmot + 1;
	modulo = 997;
	hashtable = (branche **)calloc(modulo, sizeof(branche *));
	candidate_bits = (int *)calloc(w_bits, sizeof(int));
	//to have same tree display as without bootstrap, must root it at midpoint and go over branches
	//from there
	struct noeud *newroot = (struct noeud *)malloc(sizeof(struct noeud));//deallocated later by free_arbre
	place_midpoint_root(racine, newroot, notu, FALSE); // root at midpoint
	newroot->nom = NULL;
	struct noeud *v1 = newroot->v1; struct noeud *v2 = newroot->v2;
	//remove root
	if(v1->v1 == newroot) v1->v1 = v2; 
	else if(v1->v2 == newroot) v1->v2 = v2; 
	else v1->v3 = v2;
	if(v2->v1 == newroot) v2->v1 = v1; 
	else if(v2->v2 == newroot) v2->v2 = v1; 
	else v2->v3 = v1;
	/* parcourir branches internes de l'arbre et les coder */
	err = for_all_branches(v1, v2, mainbranches);
	if(err) return 1;
	err = for_all_branches(v2, v1, mainbranches);
	if(err) return 1;
	if(has_br_length || (newroot->l1 + newroot->l2 != 0) )err = process_branch(v1, v2, mainbranches);
	if(err) return 1;
	//put back root
	if(v1->v1 == v2) v1->v1 = newroot; 
	else if(v1->v2 == v2) v1->v2 = newroot; 
	else v1->v3 = newroot;
	if(v2->v1 == v1) v2->v1 = newroot; 
	else if(v2->v2 == v1) v2->v2 = newroot; 
	else v2->v3 = newroot;
	racine = newroot;
	return 0;
}


int process_one_replicate(char *one_tree, float w)
// returns 1 iff one_tree was a correct tree
{
	struct noeud *eachroot;
	char *message;
	struct noeud  **eachtabtax;
	branche  *eachbranches;
	
	message = preptree(one_tree, NULL, &eachroot, &eachtabtax, &eachbranches);
	if(message != NULL) return 0;
	if( recodeleaves(eachroot, NULL, maintabtax) ) return 0;
	parcourir_branches_each(eachroot, NULL, w);
	free_arbre(eachroot, NULL);
	free(eachtabtax); 
	free_branches(eachbranches);
	return 1;
}

char *finish_add_bootstrap(int replicates)
//returns NULL iff error
{
	char *arbre1, *arbre2, *arbre3, *arbre, *f1, *f2, *f3, *bootstrap = NULL;
	struct noeud *r1, *r2, *r3, *r, *r1p, *r2p, *r3p;
	double l1, l2, l3;
	use_count(mainbranches, replicates);	  
/*  calcul de son format phylip unrooted */
	r = racine->v1;
	if(r->nom != NULL) {//racine->v1 is a leaf
		r1 = r;
		r = racine->v2;
		if(r->v1 == racine) { r2 = r->v2; r3 = r->v3; l2 = r->l2; l3 = r->l3; }
		else if(r->v2 == racine) { r2 = r->v1; r3 = r->v3; l2 = r->l1; l3 = r->l3; }
		else  { r2 = r->v1; r3 = r->v2; l2 = r->l1; l3 = r->l2; }
		r1p = racine;
		r2p = r3p = racine->v2;
		l1 = racine->l1 + racine->l2;
		}
	else {//racine->v1 is not a leaf
		r3 = racine->v2;
		if(r->v1 == racine) { r1 = r->v2; r2 = r->v3; l1 = r->l2; l2 = r->l3; }
		else if(r->v2 == racine) { r1 = r->v1; r2 = r->v3; l1 = r->l1; l2 = r->l3; }
		else  { r1 = r->v1; r2 = r->v2; l1 = r->l1; l2 = r->l2; }
		r1p = r2p = racine->v1;
		r3p = racine;
		l3 = racine->l1 + racine->l2;
		if(has_br_length || (l3 != 0) ) bootstrap = get_br_label(racine->v1, racine->v2, mainbranches);
		}
	arbre1 = (char *)malloc( taille);
	arbre2 = (char *)malloc( taille);
	arbre3 = (char *)malloc( taille);
	f1 = recur_write_tree(r1, r1p, arbre1, arbre1 + taille, mainbranches);
	if(f1 == NULL) return NULL;
	*(f1 + 1) = 0;
	f2 = recur_write_tree(r2, r2p, arbre2, arbre2 + taille, mainbranches);
	if(f2 == NULL) return NULL;
	*(f2 + 1) = 0;
	f3 = recur_write_tree(r3, r3p, arbre3, arbre3 + taille, mainbranches);
	if(f3 == NULL) return NULL;
	*(f3 + 1) = 0;
	arbre = (char *)malloc( f1 - arbre1 + f2 - arbre2 + f3 - arbre3 + 100);
	if(has_br_length) sprintf(arbre, "(%s:%.5f,%s:%.5f,%s%s:%.5f);", arbre1, l1, arbre2, l2, arbre3, 
			bootstrap != NULL ? bootstrap : "", l3);
	else 	sprintf(arbre, "(%s,%s,%s%s);", arbre1, arbre2, arbre3, bootstrap != NULL ? bootstrap : "");

	free(arbre1); free(arbre2); free(arbre3);
	free_arbre(racine, NULL);
	free(maintabtax); 
	free_branches(mainbranches);
	free(hashtable);
	free(candidate_bits);
return arbre;
}


static void free_arbre(struct noeud *racine, struct noeud *pere)
{
if(racine == NULL) return;
if(racine->v1 != pere) free_arbre(racine->v1, racine);
if(racine->v2 != pere) free_arbre(racine->v2, racine);
if(racine->v3 != pere) free_arbre(racine->v3, racine);
if(racine->nom != NULL) free(racine->nom);
free(racine);
}


static void free_branches(branche *branch_tab)
{
	for(int i = 0; i < notu - 3; i++) {
		if(branch_tab[i].br_label != NULL) free(branch_tab[i].br_label);
		if(branch_tab[i].bits != NULL) free(branch_tab[i].bits);
	}
	if(notu > 3) free(branch_tab);
}


static char *preptree(char *tree, int *ptaille, 
	struct noeud **pracine, struct noeud ***ptabtax, branche **pbranches)
{
int i, c, rooted, v, error;
char *arbre, *p;
double root_br_l;
struct noeud *racine, *fils, **tabtax;
branche *branches = NULL;

	arbre = strdup(tree);
	/* lecture de l'arbre */
	p = arbre; while(isspace(*p)) p++;
	if(*p == '[') { 
		do p++; while(*p != ']'); p++; 
		while(isspace(*p)) p++;
	}
	if(*p != '(') goto erreur;
	if(p > arbre) memmove(arbre, p, strlen(p) + 1);
	p = arbre + 1;
	notu=2; i=3; v = 0;
	while( (c=*(p++)) != 0 && c != ';') {
		if(c == ')') notu++;
		if(c == '(') i++;
		if(c == ',') v++;
	}
	if(i != notu)goto erreur;

totbranches = 0;
arbre = (char *)realloc(arbre, strlen(arbre) + 4 * v + 5 ); /* worst case add 4 chars for each , */
p = (char *)make_binary_or_unrooted(arbre);
if(p != NULL) return p;
notu = v + 1 ; /* after this notu = number of OTUs  */

/* allocate all memory */
tabtax = (struct noeud **)calloc(2*notu-1, sizeof(struct noeud *));
  for (i = 0; i < 2*notu - 1; i++) {
    tabtax[i] = (struct noeud *)calloc(1, s_noeud);
    tabtax[i]->rank = i;
  }
  if(notu > 3) branches = (branche *)calloc(notu-3, sizeof(branche));
if(ptaille != NULL) *ptaille = strlen(arbre);
error = loadphylip(arbre, &rooted, tabtax, branches);
free(arbre);
if(error) goto erreur;
	
if(! rooted) {
	racine = *tabtax;
	if(num_noeud >= 2*notu - 1) goto erreur;
	}
else	{
	racine = *(tabtax+num_noeud);
	root_br_l= racine->l1 + racine->l2;
/* y a-t-il un bootstrap sur l'une des branches racine ? */
	i = get_br_from_bouts(racine, racine->v1, branches); 
	if(i == -1) i = get_br_from_bouts(racine, racine->v2, branches);
	if(i != -1) {
		branches[i].bouta = racine->v1;
		branches[i].boutb = racine->v2;
		}
/* remove the root */
	fils = racine->v1;
	if(fils->v1 == racine) { fils->v1 = racine->v2; fils->l1 = root_br_l; }
	else if(fils->v2 == racine) { fils->v2 = racine->v2; fils->l2 = root_br_l; }
	else { fils->v3 = racine->v2; fils->l3 = root_br_l; }
	fils = racine->v2;
	if(fils->v1 == racine) { fils->v1 = racine->v1; fils->l1 = root_br_l; }
	else if(fils->v2 == racine) { fils->v2 = racine->v1; fils->l2 = root_br_l; }
	else { fils->v3 = racine->v1; fils->l3 = root_br_l; }
	free(racine);
	racine = *tabtax;
	rooted = 0;
	}
if(notu < 2) return (char *)"Tree should contain at least 2 elements.";
if(totbranches != notu - 3) return (char *)"Trouble totbranches";
*pracine = racine;
*ptabtax = tabtax;
*pbranches = branches;
return NULL;

erreur:
return (char *)"File does not contain correct tree data.";
} /* end of preptree */



static int loadphylip(char *arbre, int *prooted, struct noeud **tabtax, branche *branches)
// returns != 0 iff error
{
char *deba,*debb,*debc, *finarbre;
struct noeud *p1, *p2, *p3, *p;
branche *int_br_g, *int_br_d;

has_br_length = 2;
/* ignore all stuff after last closing parenthesis 
(needed for fastDNAml output)
*/
finarbre= nextpar(arbre);
*prooted = 0;
deba=arbre+1;
debb=deba;
while(*debb != ',') {
	if(*debb == 0) return 1;
	if(*debb == '(')debb=nextpar(debb);
	debb++;
	}
debb++;
debc=debb;
while(*debc != ',' && debc<finarbre) {
	if(*debc == '(')debc=nextpar(debc);
	debc++;
	}
if(*debc==',') {
/* the tree is unrooted <==> it has 3 subtrees at its bottommost level */
	debc++;
	}
else	{
/* the tree is rooted */
	debc=finarbre+1;
	*prooted = 1;
	}
num_noeud = notu-1;
nextotu = -1;
p1 = unrootedset(deba, debb-2 ,&int_br_g, tabtax, branches);
if(p1 == NULL) return 1;
p2 = unrootedset(debb, debc-2, &int_br_d, tabtax, branches);
if(p2 == NULL) return 1;
p = *(tabtax+(++num_noeud));
if(num_noeud >= 2*notu - 1) return 1;
if(!has_br_length) {
	p1->l3 = 0.5*p1->l3;
	p2->l3 = 0.5*p2->l3;
	}
p->v1=p1; p1->v3=p; p->l1=p1->l3;
if(int_br_g!=NULL) { int_br_g->bouta=p; int_br_g->boutb=p1; }
p->v2=p2; p2->v3=p; p->l2=p2->l3;
if(int_br_d!=NULL) { int_br_d->bouta=p; int_br_d->boutb=p2; }
if(! *prooted) {
	p3 = unrootedset(debc, finarbre-1, &int_br_g, tabtax, branches);
	if(p3 == NULL) return 1;
	if(int_br_g!=NULL) { int_br_g->bouta=p; int_br_g->boutb=p3; }
	p->v3=p3; p3->v3=p; p->l3=p3->l3;
	}
else	{
	p->v3=NULL;
/* recherche d'un dernier label interne */
	if(totbranches < notu - 3) {
		branches[totbranches].bouta = p1;
		branches[totbranches].boutb = p2;
		totbranches++;
		}
	}
return 0;
}


struct noeud *unrootedset(char *deb, char *fin, branche **p_int_br, struct noeud **tabtax,
	branche *branches)
//returns NULL iff error
{
struct noeud *p, *pp;
char *virg, *ferme;
branche *int_br;
static int l;
static double brlength;

*p_int_br=NULL;
while(*deb==' ')deb++;
while(*fin==' ')fin--;
if(*deb != '(') { /* une feuille */
	virg = strchr(deb, ':');
	if(virg != NULL && virg < fin) {
//		if(has_br_length == 0) return NULL;
		sscanf(virg+1, "%le", &brlength);
		has_br_length=1;
		}
	else	{
//		if(has_br_length == 1) return NULL;
		brlength = 1;
		has_br_length=0;
		virg = fin + 1;
		}
	virg--;
	while(*deb==' ' || *deb=='\'')deb++;
	if( virg-1 >= deb && *virg == '\'' ) virg--;
	l = virg-deb+1;
	p = *(tabtax + ++nextotu);
	p->nom = (char *)calloc(l + 1, 1);
	memcpy(p->nom, deb, l);
	p->nom[l] = 0;
	p->l3 = brlength;
	p->v1 = p->v2 = p->v3 = NULL;
	return p;
	}
/* un noeud */
num_noeud++;
if(num_noeud >= 2*notu - 1) return NULL;
p = *(tabtax + num_noeud);
ferme =  nextpar(deb);
virg=deb + 1;
while(*virg != ',' && virg < fin) {
	if(*virg == '(') virg=nextpar(virg);
	virg++;
	}
if(virg>=ferme) return NULL;
pp = unrootedset(deb + 1, virg - 1, &int_br, tabtax, branches);
if(pp == NULL) return NULL;
p->v1 = pp; pp->v3 = p; p->l1 = pp->l3;
if(int_br != NULL) { int_br->bouta = p; int_br->boutb = pp; }
pp = unrootedset(virg + 1, ferme - 1, &int_br, tabtax, branches);
if(pp == NULL) return NULL;
p->v2 = pp; pp->v3 = p; p->l2 = pp->l3;
if(int_br != NULL) { int_br->bouta = p; int_br->boutb = pp; }
virg = strchr(ferme, ':');
if(virg != NULL && virg < fin) { /* traitement longueur */
	sscanf(virg+1, "%le", &brlength);
	has_br_length=1;
	}
else	{
	brlength = 1;
	has_br_length=0;
	//virg = fin + 1;
	}
/* recherche bootstrap (internal label) */
if(totbranches < notu-3) {
	*p_int_br= &branches[totbranches];
	totbranches++;
	}
p->l3 = brlength;
return p;
}


/* calcul d'un arbre racine au format phylip */
static char *recur_write_tree(struct noeud *centre, struct noeud *pere, char *arbre, char *finarbre,
	branche *branches)
//returns pter to last used byte of tree or NULL iff error
//caution: no null byte after last used byte !
{
int l;
char *p, *q;
struct noeud *gauche, *droit;
double lg, ld, lup;

if(is_leave(centre)) {
	l=strlen(centre->nom);
	if(arbre+l>=finarbre) return NULL;	
	memcpy(arbre,centre->nom,l);
	arbre += l-1;
	}
else	{
	if(centre->v1 == pere) {
		gauche = centre->v2; droit = centre->v3; lg = centre->l2; ld = centre->l3; lup = centre->l1;
		}
	else if(centre->v2 == pere) {
		gauche = centre->v1; droit = centre->v3;  lg = centre->l1; ld = centre->l3; lup = centre->l2;
		}
	else {
		gauche = centre->v1; droit = centre->v2;  lg = centre->l1; ld = centre->l2; lup = centre->l3;
		}
	*arbre='(';
	p = arbre;
	arbre=recur_write_tree(gauche, centre, arbre+1, finarbre, branches);
	if(arbre==NULL) return NULL;
	if(has_br_length) {
		if(arbre+10>=finarbre) return NULL;
		sprintf(++arbre,":%.5f",lg);
		while(*arbre!=0) arbre++;
		}
	else arbre++;
	*arbre=',';
	arbre = recur_write_tree(droit, centre, arbre+1, finarbre, branches);
	if(arbre==NULL) return NULL;
	if(has_br_length) {
		if(arbre+10>=finarbre) return NULL;
		sprintf(++arbre,":%.5f",ld);
		while(*arbre!=0) arbre++;
		}
	else arbre++;
	*arbre=')';
	/* ecriture des labels internes */
	if( (q = get_br_label(centre, pere, branches)) != NULL && (has_br_length || (lup != 0) ) ) {
		l=strlen(q);
		if(arbre+l>=finarbre) return NULL;
		memcpy(arbre+1,q,l);
		arbre+=l;
		}
	else if( pere != NULL && (!has_br_length) && lup == 0 ) {//multibranches processed here
		memmove(p, p + 1, arbre - p);
		arbre -= 2;
		}
	}
return arbre;
}


static int get_br_from_bouts(struct noeud *a, struct noeud *b, branche *branches)
{
int i;
for(i=0; i < totbranches; i++) {
	if(branches[i].bouta==a && branches[i].boutb==b) 
		return i;
	if(branches[i].boutb==a && branches[i].bouta==b) 
		return i;
	}
return -1;
}


static int for_all_branches(struct noeud *centre, struct noeud *origine, branche *branches)
/* parcourir recursivement toutes les branches de l'arbre sans racine
a partir de centre et dans la direction opposee a son voisin origine
returns 1 iff error
*/
{
if(centre==NULL) return 0;
if(centre->v1!=origine) {
	if( process_branch(centre, centre->v1, branches) ) return 1;
	if( for_all_branches(centre->v1, centre, branches) ) return 1;
	}
if(centre->v2!=origine) {
	if( process_branch(centre, centre->v2, branches) ) return 1;
	if( for_all_branches(centre->v2, centre, branches) ) return 1;
	}
if(centre->v3!=origine) {
	if( process_branch(centre, centre->v3, branches) ) return 1;
	if( for_all_branches(centre->v3, centre, branches) ) return 1;
	}
return 0;
}


static int process_branch(struct noeud *cote1, struct noeud *cote2, branche *branches)
//returns 1 iff error
{ 
int num;
branche *br;

/* eliminer branches terminales */
if(is_leave(cote1) || is_leave(cote2) ) return 0;
if(!has_br_length) {//if no branch length ignore multifurcations
	if(cote1->v1 == cote2 && cote1->l1 == 0) return 0;
	if(cote1->v2 == cote2 && cote1->l2 == 0) return 0;
	if(cote1->v3 == cote2 && cote1->l3 == 0) return 0;
}

num = get_br_from_bouts(cote1, cote2, branches);
if(num == -1) return 1;
br = &branches[num];
br->bits = (int *)calloc(w_bits, sizeof(int));
br->count = 0;
br->next_h = NULL;
set_leaves(cote1, cote2, br->bits);
if(! testbit(br->bits, 1) ) {
	int width, word = 1;
	non(br->bits, br->bits, w_bits);
/*	eliminer bits a 1 terminaux */
	width = notu % lmot;
	if(width > 1) word = (1 << width) - 1;
	if(width > 0) br->bits[w_bits - 1] &= word;
	}
add_hash(br);
return 0;
}


void set_leaves(struct noeud *centre, struct noeud *pere, int *bits)
{
if(is_leave(centre)) {
	bit1(bits, centre->rank +  1);
	return;
	}
if(centre->v1 != pere)set_leaves(centre->v1, centre, bits);
if(centre->v2 != pere)set_leaves(centre->v2, centre, bits);
if(centre->v3 != pere)set_leaves(centre->v3, centre, bits);
}


void parcourir_branches_each(struct noeud *centre, struct noeud *origine, 
	float w)
/* parcourir recursivement toutes les branches de l'arbre sans racine
a partir de centre et dans la direction opposee a son voisin origine
*/
{
if(centre==NULL) return;
if(centre->v1!=origine) {
	if (centre->l1) process_branch_each(centre, centre->v1, w);
	parcourir_branches_each(centre->v1, centre, w);
	}
if(centre->v2!=origine) {
	if (centre->l2) process_branch_each(centre, centre->v2, w);
	parcourir_branches_each(centre->v2, centre, w);
	}
if(centre->v3!=origine) {
	if (centre->l3) process_branch_each(centre, centre->v3, w);
	parcourir_branches_each(centre->v3, centre, w);
	}
}


void process_branch_each(struct noeud *cote1, struct noeud *cote2, float w)
{ 
branche *br;

/* eliminer branches terminales */
if(is_leave(cote1) || is_leave(cote2) ) return;

memset(candidate_bits, 0, w_bits * sizeof(int));

set_leaves(cote1, cote2, candidate_bits);
if(! testbit(candidate_bits, 1) ) {
	int width, word = 1;
	non(candidate_bits, candidate_bits, w_bits);
/* eliminer bits a 1 terminaux */
	width = notu % lmot;
	if(width > 1) word = (1 << width) - 1;
	if(width > 0) candidate_bits[w_bits - 1] &= word;
	}
br = find_branch(candidate_bits);
if(br != NULL) br->count += w;
}


branche *find_branch(int *bits)
{
int h;
branche *br;

h = hashvalue(bits, modulo, notu);
br = hashtable[h];
while(br != NULL) {
	if(memcmp(br->bits, bits, w_bits * sizeof(int) ) == 0) return br;
	br = br->next_h;
	}
return NULL;
}


void add_hash(branche *br)
{
int h;
branche *old;

h = hashvalue(br->bits, modulo, notu);
old = hashtable[h];
hashtable[h] = br;
br->next_h = old;
}


inline int is_leave(struct noeud *n)
{
return n->nom != NULL;
}


char *get_br_label(struct noeud *a, struct noeud *b, branche *branches)
{
int i;
i = get_br_from_bouts(a, b, branches);
return (i == -1 ? NULL : branches[i].br_label);
}


static int hashvalue(int *bits, int modulo, int totbits)
{
/* inspire de celui de java */
const unsigned maxi = (UINT_MAX - 256) / 37;
unsigned c, h = 0;
unsigned char *debut, *fin;

debut = (unsigned char *)bits;
fin = debut + (totbits-1)/8;
debut--;
while(++debut <= fin ) {
    	c = *debut;
	h = h * 37 + c;
	if(h >= maxi) h %= modulo;
	}
return h % modulo;
}


int recodeleaves(struct noeud *node, struct noeud *parent, struct noeud **tabtax)
//returns != 0 iff error
{
int i;

if(node == NULL) return 0;
if(is_leave(node)) {
	for(i=0; i < notu; i++) {
		if(strcmp(node->nom, tabtax[i]->nom)==0) {
			node->rank = i;
			break;
			}
		}
	if(i>= notu) return 1;
	}
if(node->v1 != parent) recodeleaves(node->v1, node, tabtax); 
if(node->v2 != parent) recodeleaves(node->v2, node, tabtax); 
if(node->v3 != parent) recodeleaves(node->v3, node, tabtax); 
return 0;
}


static void use_count(branche *branches, int count)	
{
int num;
char text[20], *p;
branche *br;
char nan[] = "NaN";

if (count) p = text;
else p = nan;
for(num = 0; num < totbranches; num++) {
	br = branches + num;
	if(br->br_label != NULL) free(br->br_label);
	if (count) sprintf(text, "%d", (int)(br->count * 100. / count + 0.5) );
	br->br_label = strdup(p);
	}
}

void bit0(int *plist, int num)
{
	num--;
	plist+=(num/lmot);
	*plist &= ~(1<<(num%lmot));
}


void bit1(int *plist, int num)
{
	num--;
	plist+=(num/lmot);
	*plist |= (1<<(num%lmot));
}

int testbit(int *plist, int num)
{
	num--;
	plist += (num/lmot);
	return (*plist) & (1<<(num%lmot));
}

void non(int *listnon, int *list, int len)
{
	int i;
	for(i=0; i< len; i++)
		listnon[i]= ~list[i];
}


void et(int *listet, int *list1, int *list2, int len)
{
  int i;
  for (i = 0; i < len; i++) listet[i] = list1[i] & list2[i];
}


int irbit(int *pdeblist, int deb, int fin)
/* 
 finds next set bit strictly after bit of rank deb and before that of rank fin
 returns rank of that bit or 0 if none present
 */
{
  unsigned int mot;
  int *plist, *debw, *finw, retval, d;
  
  if(deb >= fin) return 0;
  finw = pdeblist+(fin-1)/lmot;
  debw = pdeblist+deb/lmot;
  
  plist = debw;
  mot = *plist;
  if( (d = deb%lmot) != 0) mot = ( mot>>d ) << d;
  if(mot == 0 && plist < finw) {
    do	{
      if ( *(++plist) != 0) break;
    }
    while ( plist < finw );
    mot = *plist;
  }
  if( plist == finw && (d = fin%lmot) != 0 ) { d = lmot-d; mot <<= d; mot >>= d; }
  
  if(mot == 0)return 0;
  retval = (plist-pdeblist)*lmot + 1;
  while( (mot&1) == 0 ) {
    mot >>= 1;
    retval++;
  }
  return retval;
  /*
   the instruction
   mot = mot ^ (~(-mot));
   replaces
   x...x10...0
   by
   0...011...1
   so that the bit count operation can be used to find first set bit
   */
}


/* Returns the Newick string of the consensus tree expressed as a list of internal branches,
 each branch i is represented by a bitlist rows[i] (one bit for each leave, 1s on one side, 0s on the other;
 rows are standardized with their first bit at 1).
 The resulting tree is multibranched, unrooted, without length nor branch support.
 This function could be expanded to output also branch support.
 Initialization:
 Build the array of columns: bitlists obtained by reading perpendicular to the rows, one column for each leave.
 Prepare a string pter for each leave initialized to its name.
 Prepare row-mask a bitlist with 1 for each column.
 Prepare column-mask a bitlist with 1 for each row (=branch).
 Definition: a multibranched cherry is a tree node with only terminal branches on one side;
 a multibranched cherry is detectable relatively to an internal branch: all columns indexed by
 the leaves at one or the other side of the branch are identical.
 Algorithm:
 For each row (=branch) k, check whether all columns for unmasked leaves i1,i2,... at its 1-side are identical
 (mask with column-mask when comparing columns);
 if not, check whether all columns for unmasked leaves i1,i2,... at its 0-side are identical;
 if not, continue with next branch;
 if yes, build the Newick form of this cherry: "(i1,i2,...)" and attach it to i1's pter and put 0 to other leave pters.
 Remove i2,... from row-mask.
 Remove k from column-mask.
 Continue until all rows (=branches) have been removed.
 Finally, 3 or more leaves remain, and the full Newick string concatenates the strings attached to them.
 */
char *compute_consensus_tree(char **seqnames, int **rows, int branch_count)
{
  // each row represents a tree branch as the bitlist of leaves on one side of the branch
  int i, j, k, l, w, word, restbranches, j1;
  char *newtree, *p;
  char **treeparts = (char**)malloc(notu*sizeof(char*)); 
  // treepart[i] will be the growing string of subtree containing leave i, or NULL
  for (i = 0; i < notu; i++) treeparts[i] = strdup(seqnames[i]);
  int height = (branch_count-1)/lmot + 1; // length of column bitlists
  int width = (notu-1)/lmot + 1; // length of row bitlists
  int *column_tmp = (int*)malloc(height*sizeof(int));
  int *column_mask = (int*)malloc(height*sizeof(int)); // bitlist of remaining rows (= branches)
  memset(column_mask, -1, height*sizeof(int));
  int *row_tmp = (int*)malloc(width*sizeof(int));
  int *row_mask = (int*)malloc(width*sizeof(int)); // bitlist of remaining columns
  memset(row_mask, -1, width*sizeof(int));
  /* clear bits after end of bitlist */
  word = 1;
  w = notu % lmot;
  if (w > 1) word = (1 << w) - 1;
  if (w > 0) row_mask[width - 1] &= word;
  // build the column table: one column for each tree leave, perpendicular to bit array of rows
  int **columns = (int**)malloc(notu*sizeof(int*));
  for (i = 0; i < notu; i++) {
    columns[i] = (int*)calloc(height, sizeof(int));
  }
  for (i = 0; i < branch_count; i++) {
    for (j = 0; j < notu; j++) {
      if (testbit(rows[i], j+1)) bit1(columns[j], i+1);
    }
  }
  restbranches = branch_count; // number of remaining branches to process
  while (restbranches > 0) {
    for (i = 0; i < branch_count; i++) { // loop for all rows i
      if (!testbit(column_mask, i+1)) continue; // skip removed rows(=branches)
      et(row_tmp, rows[i], row_mask, width); // mask out removed leaves
      j1 = irbit(row_tmp, 0, notu); // find first leave on 1-side of branch i
      j = j1-1;
      et(column_tmp, columns[j], column_mask, height); // mask out removed rows
      while ( (j1 = irbit(row_tmp, j1, notu)) != 0) { // for all other leaves on 1-side of branch i
	for (k = 0; k < height; k++) { // check that column k is identical-with-mask to column j1-1
	  if (column_tmp[k] != (columns[j1-1][k] & column_mask[k])) break;
	}
	if (k < height) break;
      }
      if (k < height) { // columns were not all identical, try columns on 0-side of branch i
    	non(row_tmp, rows[i], width); // list of leaves on 0-side of branch
	et(row_tmp, row_tmp, row_mask, width); // mask out removed leaves
	j1 = irbit(row_tmp, 0, notu); // find first leave on 0-side of branch i
	j = j1-1;
	et(column_tmp, columns[j], column_mask, height); // mask out removed rows
	while ( (j1 = irbit(row_tmp, j1, notu)) != 0) {// for all other leaves on 0-side of branch i
	  for (k = 0; k < height; k++) { // check that column k is identical-with-mask to column j1-1
	    if (column_tmp[k] != (columns[j1-1][k] & column_mask[k])) break;
	  }
	  if (k < height) break;
	}
      }
      if (k < height) continue; // not all colummns on one side of the branch were identical-with-mask
      // we have found a cherry corresponding to row i
      j = irbit(row_tmp, 0, notu) - 1; // first of the cherry leaves
      bit0(column_mask, i+1); // remove row (=tree internal branch) i from list of remaining rows
      restbranches--; // one branch less to process
      // build growing tree string containing leave j
      newtree = (char*)malloc(strlen(treeparts[j]) + 2);
      sprintf(newtree, "(%s", treeparts[j]);
      j1 = j+1;
      while ( (j1 = irbit(row_tmp, j1, notu)) != 0) { // add names of all found leaves
	k = j1-1;
	l = strlen(newtree) + strlen(treeparts[k]) + 2;
	newtree = (char*)realloc(newtree, l+1);
	strcat(newtree, ",");
	strcat(newtree, treeparts[k]);
	free(treeparts[k]);
	treeparts[k] = NULL;
      }
      strcat(newtree, ")");
      free(treeparts[j]);
      treeparts[j] = newtree;
      for (k = 0; k < width; k++) { // remove found leaves from row_mask 
	row_mask[k] &= (~row_tmp[k]);
      }
      bit1(row_mask, j+1); // except leave j
    }
  } // end of loop while branches remain
  l = 0; // build complete Newick string of tree after all branches have been processed
  for (i = 0; i < notu; i++) { // 3 or more non-NULL treeparts[i] remain
    if (treeparts[i]) l += strlen(treeparts[i]) + 1;
  }
  newtree = (char*)malloc(l + 3);
  p = newtree;
  for (i = 0; i < notu; i++) {
    if (!treeparts[i]) continue;
    sprintf(p, ",%s", treeparts[i]);
    p += strlen(p);
    free(treeparts[i]);
    treeparts[i] = NULL;
    }
  *newtree = '(';
  strcpy(p, ");"); // final Newick string is complete
  free(column_mask); // free memory
  free(column_tmp);
  free(row_mask);
  free(row_tmp);
  for (i = 0; i < notu; i++) free(columns[i]);
  free(columns);
  free(treeparts);
  return newtree;
}


void init_consensus_calculation(int n)
{
  notu = n;
  w_bits = (notu-1) / lmot + 1;
  modulo = 997;
  hashtable = (branche **)calloc(modulo, sizeof(branche *));
  candidate_bits = (int *)calloc(w_bits, sizeof(int));
}

static int recodeleaves_consensus(struct noeud *node, struct noeud *parent, char **seqnames)
//returns != 0 iff error
{
  int i;
  
  if (node == NULL) return 0;
  if (is_leave(node)) {
    for (i=0; i < notu; i++) {
      if (strcmp(node->nom, seqnames[i])==0) {
	node->rank = i;
	break;
      }
    }
    if (i>= notu) return 1;
  }
  if (node->v1 != parent) recodeleaves_consensus(node->v1, node, seqnames); 
  if (node->v2 != parent) recodeleaves_consensus(node->v2, node, seqnames); 
  if (node->v3 != parent) recodeleaves_consensus(node->v3, node, seqnames); 
  return 0;
}

branche *find_branch_consensus(int *bits)
{
  int h;
  branche *br;
  
  h = hashvalue(bits, modulo, notu);
  br = hashtable[h];
  while (br != NULL) {
    if(memcmp(br->bits, bits, w_bits * sizeof(int) ) == 0) return br;
    br = br->next_h;
  }
  br = (branche*)calloc(1, sizeof(branche));;
  br->bits = (int*)malloc(w_bits*sizeof(int));
  memcpy(br->bits, bits, w_bits*sizeof(int));
  br->count = 0;
  br->next_h = NULL;
  add_hash(br);
  return br;
}

static void process_branch_consensus(struct noeud *cote1, struct noeud *cote2)
{ 
  branche *br;
  
  /* eliminer branches terminales */
  if (is_leave(cote1) || is_leave(cote2) ) return;
  
  memset(candidate_bits, 0, w_bits * sizeof(int));
  
  set_leaves(cote1, cote2, candidate_bits);
  if (! testbit(candidate_bits, 1) ) {
    int width, word = 1;
    non(candidate_bits, candidate_bits, w_bits);
    /* eliminer bits a 1 terminaux */
    width = notu % lmot;
    if (width > 1) word = (1 << width) - 1;
    if (width > 0) candidate_bits[w_bits - 1] &= word;
  }
  br = find_branch_consensus(candidate_bits);
  br->count += 1;
}

static void parcourir_branches_consensus(struct noeud *centre, struct noeud *origine)
/* parcourir recursivement toutes les branches de l'arbre sans racine
 a partir de centre et dans la direction opposee a son voisin origine
 */
{
  if(centre==NULL) return;
  if(centre->v1!=origine) {
    if (centre->l1) process_branch_consensus(centre, centre->v1);
    parcourir_branches_consensus(centre->v1, centre);
  }
  if(centre->v2!=origine) {
    if (centre->l2) process_branch_consensus(centre, centre->v2);
    parcourir_branches_consensus(centre->v2, centre);
  }
  if(centre->v3!=origine) {
    if (centre->l3) process_branch_consensus(centre, centre->v3);
    parcourir_branches_consensus(centre->v3, centre);
  }
}


int process_one_consensus_item(char *one_tree, char **seqnames)
{
  struct noeud *eachroot;
  char *message;
  struct noeud  **eachtabtax;
  branche  *eachbranches;
  
  message = preptree(one_tree, NULL, &eachroot, &eachtabtax, &eachbranches);
  if (message != NULL) return 0;
  if( recodeleaves_consensus(eachroot, NULL, seqnames) ) return 0;
  parcourir_branches_consensus(eachroot, NULL);
  free_arbre(eachroot, NULL);
  free(eachtabtax); 
  free_branches(eachbranches);
  return 1;
}


int after_consensus_items(float threshold, int item_count, int ***prows)
{
  branche *br, *br2;
  int i, j = 0, br_count = 0;
  if (threshold > 0.999) threshold = 0.999;
  else if (threshold < 0.501) threshold = 0.501;
  for (i = 0; i < modulo; i++) {
    br = hashtable[i];
    while (br) {
      if (br->count/item_count >= threshold) br_count++;
      else free(br->bits);
      br = br->next_h;
    }
  }
  int **rows = (int**)malloc(br_count * sizeof(int*));
  for (i = 0; i < modulo; i++) {
    br = hashtable[i];
    while (br) {
      if (br->count/item_count >= threshold) rows[j++] = br->bits;
      br2 = br->next_h;
      free(br);
      br = br2;
    }
  }
  *prows = rows;
  free(hashtable);
  free(candidate_bits);
  return br_count;
}
