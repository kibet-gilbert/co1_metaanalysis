#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <FL/fl_ask.H>


typedef struct noeud2{
	struct noeud2 *v1, *v2, *v3;	/* neighbors */
	double l1, l2, l3;		/* actual branch lengths */
	double b1, b2, b3;		/* bootstrap values */
	char* nom;			/* name */
} *noeud;

typedef struct sbranche{
	noeud bout1;
	noeud bout2;
	double length;
	double bootstrap;
} *branche;


char *least_squares_brl(const char *input_tree, int notu, double **dist, char **names);
double* lslgbr(int** btree, int n, double** dist, double* sce, double* totallength);
int tree_ctob(char *carbre, char *nom[], int notu, int *barbre[]);
int tree_btos(int** arbre_b, int notu, double* lgbi, double* lgbp, double* bootval, char** nom, noeud* arbre_s);
noeud create_node(noeud v1, noeud v2, noeud v3, double l1, double l2, double l3, double b1, double b2, double b3, char* nom);
int invmat(double** mat, int n, double** invmat);
int matmat(double** m1, int nbl1, int nbc1, double** m2, int nbl2, int nbc2, double** prod);
int matvect(double** m1, int nbl1, int nbc1, double* v2, int nbl2, double* vprod);
double* vectmat(double* v1, int nbc1, double** m2, int nbl2, int nbc2);
int retder(int *liste);
void aj(int *liste, int nb);
noeud bottomnode(noeud nd);
void lubksb(double **a, int n, int *indx, double b[]);
int rooted(char* carbre);
int unroot_c(char* carbre);
char *write_unrooted(noeud centre, char *tree);
char *write_rooted(noeud centre, noeud from, char *tree);


extern int testbit(int *plist, int num);
extern void bit1(int *plist, int num);
extern void bit0(int *plist, int num);
extern int ludcmp(double **a, int n, int *indx, double *d);

#define lmot (8*sizeof(int))


char *least_squares_brl(const char *input_tree, int notu, double **dist, char **names)
//returns a tree or NULL if error
{
	int **barbre, i, lliste, err;
	double sce, tot, *newbl, *lgbp, *lgbi;
	noeud *sarbre;
	char *tree;
	
	tree = strdup(input_tree);
    lliste=(notu+lmot-1)/lmot;
    barbre=(int**)calloc(notu, sizeof(int*));
    for(i=0;i<notu;i++) barbre[i]=(int*)calloc(lliste, sizeof(int));
	if (rooted(tree)) unroot_c(tree);
	err = tree_ctob(tree, names, notu, barbre);
	free(tree);
	if(err <= 0) {
		fl_alert("Names of processed sequences and names found in tree do not match.");
		tree = NULL;
		}
	else {
		newbl = lslgbr(barbre, notu, dist, &sce, &tot);
		lgbp=newbl; lgbi=newbl+notu;    
		sarbre=(noeud*)calloc(2*notu, sizeof(noeud));
		tree_btos(barbre, notu, lgbi, lgbp, NULL, names, sarbre);
		free(newbl);
		tree = (char *)malloc(strlen(input_tree) + 20*notu);
		write_unrooted(sarbre[0], tree);
		for(i = 0; i < 2 * notu; i++) if(sarbre[i] != NULL) free(sarbre[i]);
		free(sarbre);
		}
    for(i=0;i<notu;i++) free(barbre[i]);
    free(barbre);
	return tree;
}

/* lslgbr */
/* Compute the least-squares estimates of branch lengths of tree btree */
/* according to effective distances dist. Branch length are returned */
/* via a double table, ordered as in btree. sce returned value is the sum */
/* of squares of residuals between patristic and effective distances. */
/* totallength returned value is the sum of all branch lengths. */
/* Negative branch lengths are forced to 0 in sce and tot computing. */
/* Method is from Rzhetsky & Nei (1992) Mol. Biol.Evol 9(5):945-967.  */

double* lslgbr(int** btree, int n, double** dist, double* sce, double* totallength)
{
	int i, j, k, ii, nbdist, nbbr, *ind1, *ind2, b1, b2;
	double **ta, **B, **invB, **prod1, *lgbr, *cdist, *pdist;
	
	nbdist=n*(n-1)/2; nbbr=2*n-3;
	ta=(double**)calloc(nbbr, sizeof(double*));
	B=(double**)calloc(nbbr, sizeof(double*));
	invB=(double**)calloc(nbbr, sizeof(double*));
	prod1=(double**)calloc(nbbr, sizeof(double*));
	
	/* move double** dist to double* cdist */
	
	cdist=(double*)calloc(nbdist, sizeof(double));
	ind1=(int*)calloc(nbdist, sizeof(int));
	ind2=(int*)calloc(nbdist, sizeof(int));
	
	ii=0;
	for(i=0;i<n-1;i++){
		for(j=i+1;j<n;j++){
			cdist[ii]=dist[i][j];
			ind1[ii]=i; ind2[ii]=j;
			ii++;
		}
	}
	
	/* build ta */
	
	for(i=0;i<nbbr;i++) ta[i]=(double*)calloc(nbdist, sizeof(double));
	
	for(i=0;i<n;i++) {
		for(j=0;j<nbdist;j++){
			if(ind1[j]==i || ind2[j]==i) ta[i][j]=1.; else ta[i][j]=0.;
		}
	}
	for(i=n;i<nbbr;i++){
		for(j=0;j<nbdist;j++){
			b1=testbit(btree[i-n],ind1[j]+1); b2=testbit(btree[i-n],ind2[j]+1);
			if((b1 && !b2) || (b2 && !b1)) ta[i][j]=1.; else ta[i][j]=0.;
		}
	}
	
	/* compute B */
	
	for(i=0;i<nbbr;i++) B[i]=(double*)calloc(nbbr, sizeof(double));
	for(i=0;i<nbbr;i++) invB[i]=(double*)calloc(nbbr, sizeof(double));
	
	for(i=0;i<nbbr;i++) {
		for(j=0;j<nbbr;j++){
			B[i][j]=0.;
			for(k=0;k<nbdist;k++) B[i][j]+=ta[i][k]*ta[j][k];
		}
	}
	
	/* compute lgbr and pdist */
	
	for(i=0;i<nbbr;i++) prod1[i]=(double*)calloc(nbdist, sizeof(double));
	lgbr=(double*)calloc(nbbr, sizeof(double));
	
	if(!invmat(B, nbbr, invB)) return NULL;
	matmat(invB, nbbr, nbbr, ta, nbbr, nbdist, prod1);
	matvect(prod1, nbbr, nbdist, cdist, nbdist, lgbr);
	pdist=vectmat(lgbr, nbbr, ta, nbbr, nbdist);
	
	/* compute sce and total length */
	
	if(sce){
		*sce=0.;
		for(i=0;i<nbdist;i++) 
			if(pdist[i]>0.) *sce+=(pdist[i]-cdist[i])*(pdist[i]-cdist[i]);
			else *sce+=cdist[i]*cdist[i];
	}
	
	if(totallength){
		*totallength=0.;
		for(i=0;i<nbbr;i++) if(lgbr[i]>0.) *totallength+=lgbr[i];
	}
	
	free(cdist); free(ind1); free(ind2);
	for(i=0;i<nbbr;i++){
		free(ta[i]);
		free(B[i]);
		free(invB[i]);
		free(prod1[i]);
	}
	free(ta); free(B); free(invB); free(prod1);
	free(pdist);
	return lgbr;
}


/* ChaineTOBits. Read c_tree carbre and write b_tree barbre. */
int tree_ctob(char *carbre, char *nom[], int nbotu, int *barbre[])
//returns nbotu if OK, -2 if found name not in array nom, -1 if other error
{
	
	
	int   i=0, j, k, fin=0, nbpo=0,
	nbpf = 0, cptv1 = 0, br_ouverte, *listecour, 
	otu = -1, pomoinspf, cpttree=0, t=1;
	char  c, cc, dejalu = '\0';
	static char readname[300];
	
	
	sscanf(carbre+(cpttree++), "%c", &c);
	if (c == '[') {
		while ((c != ']') && c) sscanf(carbre+(cpttree++), "%c", &c);
		if (c != ']'){
//			printf("Unmatched '[' ']'\n");
			return -1;
		}
	} 
	else
		if (c == '(') cpttree=0;
		else{
//			printf("Tree file 1st character must be '(' or '['\n");
			return -1;
		}
	while ((c != ';') && c ) {
		sscanf(carbre+(cpttree++), "%c", &c);
		if (c == '(') nbpo++;
		if (c == ')') nbpf++;
		if ((nbpo == nbpf + 1) && (c == ',')) cptv1++;
	}
	
	if (c != ';'){
//		printf("';' missing at end of tree\n");
		return -1;
	}
	
	if (nbpo != nbpf){
//		printf("Unmatched parenthesis\n");
		return -1;
	}
	
	if (cptv1 == 2) /* unrooted : ok */;
	else 
		if (cptv1 == 1){ /* rooted : problem */ 
//			printf("Unexpected rooted tree.\n"); 
			return -1; 
		}
		else{ /* bad tree string */
//			printf("Bad number of ',' in tree\n");
			return -1;
		}
	if(nbotu != nbpo + 2) return -2;
	
	
	listecour=(int*) calloc(nbotu, sizeof(int));
	
	cpttree=0;
	sscanf(carbre+(cpttree++), "%c", &c);
	if (c == '['){
		while (c != ']') sscanf(carbre+(cpttree++), "%c", &c);
		while((c==']') || (c==' ') || (c=='\n') || (c=='\t')) sscanf(carbre+(cpttree++), "%c", &c);
		if (c!='(') return -1;
	}
	else
		while(c!='(') sscanf(carbre+(cpttree++), "%c", &c);
	
	pomoinspf=1;
	for (i = 0; i < nbotu; i++) listecour[i] = 0;
	br_ouverte = 0;
	
	
	for (k = 0; t==1; k++) {
		if (dejalu == '\0') sscanf(carbre+(cpttree++), "%c", &c);
		else {
			c = dejalu;
			dejalu = '\0';
		}
		switch (c) {
			case ';': fin = 1; break;
			case ',': case '\n': case '\t': case '\'': case ' ': break;
			case '(':
				pomoinspf ++;
				br_ouverte++;
				aj(listecour, br_ouverte); break;
			case ')':
				pomoinspf--;
				if (*(carbre+cpttree+1) == ';' || pomoinspf==0) {
					fin = 1; break;
				}
				j = retder(listecour);
				while(carbre[cpttree]!=',' && carbre[cpttree]!=')') cpttree++;
				break;
			default:
				otu=-1; cc = c; i = 0;
				while ((cc != ':') && (cc != ',') && (cc != ')') && (cc != '\n') && (cc != ' ')) {
					if (cc != '\'') { readname[i++] = cc; readname[i]='\0'; }
					sscanf(carbre+(cpttree++), "%c", &cc);
				}
				for(j=0;j<nbotu;j++) if(strcmp(readname, nom[j]) == 0) { otu=j; break; }
				if (otu==-1) {/*printf("Unknown name : %s\n", readname);*/ return -2; }
				for(i=0;i<nbotu-3;i++) bit0(barbre[i], otu+1);
				for (i = 0; i < nbotu-3; i++)
					if (listecour[i] != 0)
						bit1(barbre[listecour[i]-1], otu+1);
				cpttree--;
				while(carbre[cpttree]!=',' && carbre[cpttree]!=')') cpttree++; 
		}
		if (fin == 1) break;
	}
	
	free(listecour);
	return nbotu; 
}


/* tree_btos */
/* BitTOStruct. Create s_tree arbre_s from : b_tree arbre_b, leaves number notu, */
/* branch lengths lgbi (internal branches) , lgbp (terminal branches). */
/* If tree has no branch length, arguments 3 and 4 must be NULL. */
int tree_btos(int** arbre_b, int notu, double* lgbi, double* lgbp, double* bootval, char** nom, noeud* arbre_s)
{
	
	noeud   	p1, p2;
	int           notuv, i, j, k, sommebi, tax1, tax2, n1=0, n2=0, n3=0, cpt1=-1;
	int           *kill_tax, *kill_bi;
	double	arg6, arg9;
	
	kill_tax = (int *)calloc(notu, sizeof(int));
	kill_bi = (int *)calloc(notu, sizeof(int));
	
	notuv = notu;
	for (i = 0; i <notu; i++){
		kill_tax[i] = 0;
		kill_bi[i] = 0;
	}
	
	
	/* terminal nodes */
	
	for (i = 0; i < notu; i++){
		if(lgbp==NULL) arg6=-1.0; else arg6=lgbp[i];
		arbre_s[i] = create_node(NULL, NULL, NULL, 0., 0., arg6, -1., -1., -1., nom[i]);
	}
	
	
	/* internal nodes */
	
	for (i = 0; i < notu - 3; i++) {
		/* determination de la bi a creer */
		for (j = 0; j < notu - 3 ; j++) {
			if (kill_bi[j] == 0) {
				sommebi = 0;
				for (k = 0; k < notu; k++)
					if (kill_tax[k] == 0) {
						if(testbit(arbre_b[j], k+1)) sommebi++;
					}
				if (sommebi == 2 || notuv - sommebi == 2)
					break;
			}
		}
		
		/* determination des 2 otus/noeuds fils */
		if (sommebi == 2) {
			for (k = 0; k < notu; k++)
				if (testbit(arbre_b[j], k+1) && kill_tax[k] == 0) {
					tax1 = k;
					break;
				}
			for (k = tax1 + 1; k < notu; k++)
				if (testbit(arbre_b[j], k+1) && kill_tax[k] == 0) {
					tax2 = k;
					break;
				}
		} else {
			for (k = 0; k < notu; k++)
				if (!testbit(arbre_b[j], k+1) && kill_tax[k] == 0) {
					tax1 = k;
					break;
				}
			for (k = tax1 + 1; k < notu; k++)
				if (!testbit(arbre_b[j], k+1) && kill_tax[k] == 0) {
					tax2 = k;
					break;
				}
		}
		
		p1 = bottomnode(arbre_s[tax1]);
		p2 = bottomnode(arbre_s[tax2]);
		
		if (lgbp) arg6=lgbi[j]; else arg6=0.;
		if (bootval) arg9=bootval[j]; else arg9=-1;
		arbre_s[notu + (++cpt1)] = create_node(p1, p2, NULL, p1->l3, p2->l3, arg6, p1->b3, p2->b3, arg9, NULL);
		
		p1->v3 = arbre_s[notu + cpt1];
		p2->v3 = arbre_s[notu + cpt1];
		
		kill_tax[tax1] = kill_bi[j] = 1;
		notuv--;
	}
	
	
	/* last node */
	
	
	for (i = 0; i < 2 * notu - 3; i++) 
		if (arbre_s[i]->v3 == NULL) {
			n1 = i;
			break;
		}
	
	for (i = n1 + 1; i < 2 * notu - 3; i++) 
		if (arbre_s[i]->v3 == NULL) {
			n2 = i;
			break;
		}
	
	for (i = n2 + 1; i < 2 * notu - 3; i++) 
		if (arbre_s[i]->v3 == NULL) {
			n3 = i;
			break;
		}
	
	
	arbre_s[2 * notu - 3] = create_node(arbre_s[n1], arbre_s[n2], arbre_s[n3], arbre_s[n1]->l3, arbre_s[n2]->l3, arbre_s[n3]->l3, arbre_s[n1]->b3, arbre_s[n2]->b3, arbre_s[n3]->b3, NULL);
	arbre_s[n1]->v3 = arbre_s[2 * notu - 3];
	arbre_s[n2]->v3 = arbre_s[2 * notu - 3];
	arbre_s[n3]->v3 = arbre_s[2 * notu - 3];
	
	free(kill_tax); free(kill_bi);
	return 0;
}

/* create_node */
/* Return a node (part of a s_tree) whose "parent", "childs" and values are */
/* v1, v2, v3, l1, l2, l3, nom, order. */

noeud create_node(noeud v1, noeud v2, noeud v3, double l1, double l2, double l3, double b1, double b2, double b3, char* nom) 
{
	noeud nd;
	
	nd = (struct noeud2*) calloc(1, sizeof(struct noeud2));
	if (nd==NULL) {
		printf("Not enough memory.\n");
		return NULL;
	}
	nd->v1 = v1; nd->v2 = v2; nd->v3 = v3;
	nd->l1 = l1; nd->l2 = l2; nd->l3 = l3;
	nd->b1 = b1; nd->b2 = b2; nd->b3 = b3;
	if (nom!=NULL) nd->nom = strdup(nom);
	return nd;
}

/* invmat */
/* Invert square matrix mat(n,n). Result in invmat.*/
/* If mat is singular, 0 is returned (1 otherwise) */

int invmat(double** mat, int n, double** invmat)
{
	int i, j, *indx;
	double **lu, d, *col;
	
	
	for(i=0;i<n;i++) invmat[i]=(double*)calloc(n, sizeof(double));
	
	lu = (double **)calloc(n + 1, sizeof(double *));
	for(i=1;i<n+1;i++){
		lu[i]=(double*)calloc(n+1, sizeof(double));
		for(j=1;j<n+1;j++) lu[i][j]=mat[i-1][j-1];
	}
	indx=(int*)calloc(n+1, sizeof(int));
	col=(double*)calloc(n+1, sizeof(double));
	
	if(!ludcmp(lu, n, indx, &d)) return 0;
	
	for(j=1;j<=n;j++){
		for(i=1;i<=n;i++) col[i]=0.;
		col[j]=1.;
		lubksb(lu, n, indx, col);
		for(i=1;i<=n;i++) invmat[i-1][j-1]=col[i];
	}
	for(i=1; i<n+1;i++) free(lu[i]);
	free(lu);
	return 1;
}


/* Product of matrices. */
/* Matrices m1(nbl1, nbc1) and m2(nbl2, nbc2) are multiplicated. */
/* Product is written into prod(nbl1, nbc2). */
/* prod allocation is not performed. */

int matmat(double** m1, int nbl1, int nbc1, double** m2, int nbl2, int nbc2, double** prod)
{
	int i, j, k;
	
	if(nbc1!=nbl2)  return 0;
	
	for(i=0;i<nbl1;i++){
		for(j=0;j<nbc2;j++){
			prod[i][j]=0.;
			for(k=0;k<nbc1;k++)
				prod[i][j]+=m1[i][k]*m2[k][j];
		}
	}
	return 1;
}


/* Return the product vector of matrix m1(nbl1, nbc1) by column vector v2. */
int matvect(double** m1, int nbl1, int nbc1, double* v2, int nbl2, double* vprod)
{
	int i, k;
	
	if(nbc1!=nbl2) return 0;  
	
	for(i=0;i<nbl1;i++){
		vprod[i]=0;
		for(k=0;k<nbc1;k++) vprod[i]+=m1[i][k]*v2[k];
	}
	return 1;
}


/* Return the product vector of row vector v1 by matrix m2(nbl1, nbc1). */
double* vectmat(double* v1, int nbc1, double** m2, int nbl2, int nbc2)
{
	int j, k;
	double* vprod;
	
	if(nbc1!=nbl2) return 0;
	vprod=(double*)calloc(nbc2+1, sizeof(double));
	for(j=0;j<nbc2;j++){
		vprod[j]=0;
		for(k=0;k<nbc1;k++) vprod[j]+=v1[k]*m2[k][j];
	}
	return vprod;
}


int retder(int *liste)
{
	int i=0, j;
	while (liste[i] != 0) i++;
	j = *(liste + i - 1);
	*(liste + i - 1) = 0;
	return j;
}


void aj(int *liste, int nb) 
{
	int  i=0;
	while (liste[i] != 0) i++;
	*(liste + i) = nb;
	return;
}


/* bottomnode */
/* Return the root node of rooted s_tree including node nd. */
/*    !!BEWARE!!    */
/* If the s_tree including node nd is unrooted, program will bug */
/* (infinite loop). */
noeud bottomnode(noeud nd)
{
	if(nd->v3 == NULL) return nd;
	return (bottomnode(nd->v3));
}


/* lubksb */
/* from Numerical Recipes in C */
void lubksb(double **a, int n, int *indx, double b[])
{
	int i,ii=0,ip,j;
	double sum;
	
	for (i=1;i<=n;i++) {
		ip=indx[i];
		sum=b[ip];
		b[ip]=b[i];
		if (ii)
			for (j=ii;j<=i-1;j++) sum -= a[i][j]*b[j];
		else if (sum) ii=i;
		b[i]=sum;
	}
	for (i=n;i>=1;i--) {
		sum=b[i];
		for (j=i+1;j<=n;j++) sum -= a[i][j]*b[j];
		b[i]=sum/a[i][i];
	}
}


/* Return 1 if c_tree carbre is rooted, 0 if unrooted, -1 if problem */
int rooted(char* carbre)
{
	int i=0, cpt=0, cptv=0;
	
	while((carbre[i]==' ' || carbre[i]=='\n' || carbre[i]=='\t') && carbre[i]) i++;
	if(carbre[i]!='[' && carbre[i]!='(') {
		printf("Tree first char must be ( or [\n"); 
		return -1; 
	}
	
	if(carbre[i]=='[') while(carbre[i]!=']' && carbre[i]) i++;
	if(!carbre[i]){ printf("Unmatched '[' ']'\n"); return -1; }
	while(carbre[i]!='(' && carbre[i]) i++;
	if(!carbre[i]) {printf("No initial parenthesis\n"); return -1; }
	
	while(carbre[i]!=';' && carbre[i]){
		if (carbre[i]=='(') cpt++;
		if (carbre[i]==')') cpt--;
		if (carbre[i]==',' && cpt==1) cptv++;
		i++;
	}
	
	if (cptv==2) return 0;
	if (cptv==1) return 1;
	return -1;
}


/* unroot_c */
/* Remove root from rooted c_tree carbre. */
int unroot_c(char* carbre)
{
	int i=0, diff;
	
	while((carbre[i]==' ' || carbre[i]=='\n' || carbre[i]=='\t') && carbre[i]) i++;
	if(carbre[i]!='[' && carbre[i]!='(') {
		printf("Tree first char must be ( or [\n"); 
		return -1; 
	}
	
	if(carbre[i]=='[') while(carbre[i]!=']' && carbre[i]) i++;
	if(!carbre[i]){ printf("Unmatched '[' ']'\n"); return -1; }
	while(carbre[i]!='(' && carbre[i]) i++;
	if(!carbre[i]) {printf("No initial parenthesis\n"); return -1; }
	
	i++;
	while(carbre[i]!='(' && carbre[i] && carbre[i]!=';') i++;
	if(!carbre[i] || carbre[i]==';') return 0;
	carbre[i]=' ';
	diff=0; 
	while(diff!=-1 && carbre[i] && carbre[i]!=';'){
		if(carbre[i]=='(')  diff++; 
		if(carbre[i]==')')  diff--; 
		i++;
	}
	
	if(!carbre[i] || carbre[i]==';') return 0;
	carbre[i-1]=' ';
	while(carbre[i]!=',' && carbre[i]!=')') {
		carbre[i]=' ';
		i++;
	}
	
	return 1;
}  



char *write_unrooted(noeud centre, char *tree_start)
{
	char *tree;
	tree = tree_start;
	*tree++ = '(';
	
	if(centre->v1 == NULL && centre->v2 == NULL) centre = centre->v3;
	else if(centre->v1 == NULL && centre->v3 == NULL) centre = centre->v2;
	else if(centre->v2 == NULL && centre->v3 == NULL) centre = centre->v1;
	
	tree = write_rooted(centre->v1, centre, tree);
	sprintf(tree, ":%.5f,", centre->l1);
	tree += strlen(tree);
	
	tree = write_rooted(centre->v2, centre, tree);
	sprintf(tree, ":%.5f,", centre->l2);
	tree += strlen(tree);

	tree = write_rooted(centre->v3, centre, tree);
	sprintf(tree, ":%.5f);", centre->l3);
	
	return tree_start;
}


char *write_rooted(noeud centre, noeud from, char *tree)
{
	noeud g, d;
	double lg, ld;
	if(centre->v1 == from) {g = centre->v2; d = centre->v3; lg = centre->l2; ld = centre->l3; }
	else if(centre->v2 == from) {g = centre->v1; d = centre->v3; lg = centre->l1; ld = centre->l3; }
	else {g = centre->v1; d = centre->v2; lg = centre->l1; ld = centre->l2; }
	if(g == NULL) {
		strcpy(tree, centre->nom); 
		}
	else {
		*tree++ = '(';
		tree = write_rooted(d, centre, tree);
		sprintf(tree, ":%.5f,", ld);
		tree += strlen(tree);
		tree = write_rooted(g, centre, tree);
		sprintf(tree, ":%.5f)", lg);
		}
	tree += strlen(tree);
	return tree;
}

