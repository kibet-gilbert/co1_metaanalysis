#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define check_alloc(a,b) calloc(a,b)

extern int tree_build_interrupted;

/* globals */
static double *rl[21];
static double *tl0[64], *tl1[64], *tl2[64], *tti0[64], *tti1[64],*tti2[64], 
	*ttv0[64], *ttv1[64], *ttv2[64];

static int code_mt = 0; // TMP

int 
num(char *cod)
{
  int  n1, n2, n3;
//MG
	static const char bases[] = "ACGT";
	if(strchr(bases, cod[0]) == NULL ||
	   strchr(bases, cod[1]) == NULL ||
	   strchr(bases, cod[2]) == NULL) return 64;
//MG
  n1 = n2 = n3 = 0;
  if (cod[0] == 'C') n1 = 1;
  if (cod[1] == 'C') n2 = 1;
  if (cod[2] == 'C') n3 = 1;
  if (cod[0] == 'G') n1 = 2;
  if (cod[1] == 'G') n2 = 2;
  if (cod[2] == 'G') n3 = 2;
  if (cod[0] == 'T') n1 = 3;
  if (cod[1] == 'T') n2 = 3;
  if (cod[2] == 'T') n3 = 3;

  return 16 * n1 + 4 * n2 + n3;
}



int catsite (char c1, char c2, char c3, int i) {

	/* renvoie 0 si le site i du codon c1c2c3 est non degenere */
	/* 1                                  2-fold degenerate */
	/* 2                                  4-fold degenerate */

	if (i == 3) {
		if ((c1 == 'A') && (c2 == 'T') && (c3 == 'G'))
			return 0;
		if ((c1 == 'T') && (c2 == 'G') && (c3 == 'A'))
			return 0;
		if ((c1 == 'T') && (c2 == 'G') && (c3 == 'G'))
			return 0;
		if (c2 == 'C')
			return 2;
		if ((c1 == 'C') && (c2 == 'T'))
			return 2;
		if ((c1 == 'G') && (c2 == 'T'))
			return 2;
		if ((c1 == 'G') && (c2 == 'G'))
			return 2;
		if ((c1 == 'C') && (c2 == 'G'))
			return 2;
		return 1;
	}
	if (i == 1) {
		if ((c1 == 'C') && (c2 == 'T') && (c3 == 'A'))
			return 1;
		if ((c1 == 'C') && (c2 == 'T') && (c3 == 'G'))
			return 1;
		if ((c1 == 'T') && (c2 == 'T') && (c3 == 'A'))
			return 1;
		if ((c1 == 'T') && (c2 == 'T') && (c3 == 'G'))
			return 1;
		if ((c1 == 'A') && (c2 == 'G') && (c3 == 'A'))
			return 1;
		if ((c1 == 'A') && (c2 == 'G') && (c3 == 'G'))
			return 1;
		if ((c1 == 'C') && (c2 == 'G') && (c3 == 'A'))
			return 1;
		if ((c1 == 'C') && (c2 == 'G') && (c3 == 'G'))
			return 1;
		return 0;
	}
	return 0;
}


char
transf(char nt1, char nt2)
{
	if (nt1 == nt2) {
		return 'S';
	}
	if ((nt1 == 'A') && (nt2 == 'C'))
		return 'v';
	if ((nt1 == 'A') && (nt2 == 'G'))
		return 'i';
	if ((nt1 == 'A') && (nt2 == 'T'))
		return 'v';
	if ((nt1 == 'G') && (nt2 == 'C'))
		return 'v';
	if ((nt1 == 'G') && (nt2 == 'T'))
		return 'v';
	if ((nt1 == 'C') && (nt2 == 'T'))
		return 'i';
	if ((nt1 == 'C') && (nt2 == 'A'))
		return 'v';
	if ((nt1 == 'G') && (nt2 == 'A'))
		return 'i';
	if ((nt1 == 'T') && (nt2 == 'A'))
		return 'v';
	if ((nt1 == 'C') && (nt2 == 'G'))
		return 'v';
	if ((nt1 == 'T') && (nt2 == 'G'))
		return 'v';
	if ((nt1 == 'T') && (nt2 == 'C'))
		return 'i';

	printf("Erreur\n%c, %c\n", nt1, nt2);
        exit(0);

}



void
titv1(char *cod1, char *cod2, double poids, double *ti, double *tv, double* l)
{
	int             i;
	char            a, b, ci1, ci2, ci3, cj1, cj2, cj3;
	char            transf(char, char);


	ci1 = cod1[0];
	ci2 = cod1[1];
	ci3 = cod1[2];
	cj1 = cod2[0];
	cj2 = cod2[1];
	cj3 = cod2[2];




	
	for (i = 0; i <= 2; i++)
		if (cod1[i] != cod2[i]) {

			l[catsite(ci1, ci2, ci3, i + 1)]+=0.5 * poids;
			l[catsite(cj1, cj2, cj3, i + 1)]+=0.5 * poids;

			a = cod1[i];
			b = cod2[i];
			if (transf(a, b) == 'i') {
				ti[catsite(ci1, ci2, ci3, i + 1)] += 0.5 * poids;
				ti[catsite(cj1, cj2, cj3, i + 1)] += 0.5 * poids;
			} else {
				tv[catsite(ci1, ci2, ci3, i + 1)] += 0.5 * poids;
				tv[catsite(cj1, cj2, cj3, i + 1)] += 0.5 * poids;
			}

			if( code_mt ) continue;  /* il n'y a plus les pb de TI non-syno et de TV syno avec code_mt ! */
		
	if (((ci2 == 'T') && (cj2 == 'T')) || ((ci2 == 'G') && (cj2 == 'G'))) { /* T ou G ensemble en pos 2 des 2 codons */


if (i==0){ /* pos 1 */	
		/* tous ces cas sont des transitions en un site 2-fold non-syno pour le code universel:
il faut les enlever du comptage des TI 2-fold (ti[1]) et les ajouter au comptage des TV 2-fold (tv[1])
pour le code_mt ce sont des sites non dege qui ont ete traites simplement comme il faut */
		if ((ci1 == 'C') && (ci2 == 'G') && (ci3 == 'A') && (cj1 == 'T') && (cj2 == 'G') && (cj3 == 'A')) {
			ti[1] -= 0.5 * poids; /* CGA / TGA */
			tv[1] += 0.5 * poids;  
		}
		if ((ci1 == 'C') && (ci2 == 'G') && (ci3 == 'G') && (cj1 == 'T') && (cj2 == 'G') && (cj3 == 'G')) {
			ti[1] -= 0.5 * poids; /* CGG / TGG */
			tv[1] += 0.5 * poids;  
		}
		if ((ci1 == 'A') && (ci2 == 'G') && (ci3 == 'G') && (cj1 == 'G') && (cj2 == 'G') && (cj3 == 'G')) {
			ti[1] -= 0.5 * poids; /* AGG / GGG */
			tv[1] += 0.5 * poids;  
		}
		if ((ci1 == 'A') && (ci2 == 'G') && (ci3 == 'A') && (cj1 == 'G') && (cj2 == 'G') && (cj3 == 'A')) {
			ti[1] -= 0.5 * poids; /* AGA / GGA */
			tv[1] += 0.5 * poids;  
		}
		if ((ci1 == 'T') && (ci2 == 'G') && (ci3 == 'A') && (cj1 == 'C') && (cj2 == 'G') && (cj3 == 'A')) {
			ti[1] -= 0.5 * poids; /* TGA / CGA */
			tv[1] += 0.5 * poids;  
		}
		if ((ci1 == 'T') && (ci2 == 'G') && (ci3 == 'G') && (cj1 == 'C') && (cj2 == 'G') && (cj3 == 'G')) {
			ti[1] -= 0.5 * poids; /* TGG / CGG */
			tv[1] += 0.5 * poids;  
		}
		if ((ci1 == 'G') && (ci2 == 'G') && (ci3 == 'G') && (cj1 == 'A') && (cj2 == 'G') && (cj3 == 'G')) {
			ti[1] -= 0.5 * poids; /* GGG / AGG */
			tv[1] += 0.5 * poids;
		}
		if ((ci1 == 'G') && (ci2 == 'G') && (ci3 == 'A') && (cj1 == 'A') && (cj2 == 'G') && (cj3 == 'A')) {
			ti[1] -= 0.5 * poids; /* GGA / AGA */
			tv[1] += 0.5 * poids;  
		}


/* tous ces cas sont 
code universel: TV syno en sites 2-fold il faut les enlever du comptage des TV 2-fold (tv[1]) et les ajouter au comptage des TI 2-fold (ti[1])
code_mt: TV non syno en site non dege qui ont ete correctement comptes
*/
		if ((ci1 == 'C') && (ci2 == 'G') && (ci3 == 'A') && (cj1 == 'A') && (cj2 == 'G') && (cj3 == 'A')) {
			tv[1] -= poids; /* CGA / AGA : TV syno code univ, non code mt */
			ti[1] += poids;  
		}
		if ((ci1 == 'A') && (ci2 == 'G') && (ci3 == 'A') && (cj1 == 'C') && (cj2 == 'G') && (cj3 == 'A')) {
			tv[1] -= poids; /* AGA / CGA : TV syno code univ, non code mt */
			ti[1] += poids;  
		}
		if ((ci1 == 'C') && (ci2 == 'G') && (ci3 == 'G') && (cj1 == 'A') && (cj2 == 'G') && (cj3 == 'G')) {
			tv[1] -= poids; /* CGG / AGG : TV syno code univ, non code mt */
			ti[1] += poids;  
		}
		if ((ci1 == 'A') && (ci2 == 'G') && (ci3 == 'G') && (cj1 == 'C') && (cj2 == 'G') && (cj3 == 'G')) {
			tv[1] -= poids; /* AGG / CGG : TV syno code univ, non code mt */
			ti[1] += poids;  
		}
}

if (i==2){ /* pos 3 */	
/* tous ces cas sont
code universel: des TV syno en site 2-fold il faut les enlever des TV 2-fold (iv[1]) et ajouter aux TI 2-fold (ti[1])
code_mt: ce sont des TV non syno en site 2-fold qui int ete comptees normalement
*/
		if ((ci1 == 'A') && (ci2 == 'T') && (ci3 == 'A') && (cj1 == 'A') && (cj2 == 'T') && (cj3 == 'T')) {
			tv[1] -= poids; /* TV ATA / ATT : syno code univ, non code mt */
			ti[1] += poids;  
		}
		if ((ci1 == 'A') && (ci2 == 'T') && (ci3 == 'T') && (cj1 == 'A') && (cj2 == 'T') && (cj3 == 'A')) {
			tv[1] -= poids; /* TV ATT / ATA : syno code univ, non code mt */
			ti[1] += poids;  
		}
		if ((ci1 == 'A') && (ci2 == 'T') && (ci3 == 'A') && (cj1 == 'A') && (cj2 == 'T') && (cj3 == 'C')) {
			tv[1] -= poids; /* TV ATA / ATC : syno code univ, non code mt */
			ti[1] += poids;  
		}
		if ((ci1 == 'A') && (ci2 == 'T') && (ci3 == 'C') && (cj1 == 'A') && (cj2 == 'T') && (cj3 == 'A')) {
			tv[1] -= poids; /* TV ATC / ATA : syno code univ, non code mt */
			ti[1] += poids;  
		}



/* ces 2 cas sont
code universel: des TI non syno en site 2-fold il faut les enlever des TI 2-fold (ti[1]) et les ajouter aux TV 2-fold (tv[1])
code_mt: des TI syno en site 2-fold qui ont ete comptees normalement
*/
		if ((ci1 == 'A') && (ci2 == 'T') && (ci3 == 'A') && (cj1 == 'A') && (cj2 == 'T') && (cj3 == 'G')) {
			ti[1] -= 0.5 * poids; /* TI ATA / ATG : non syno code univ, syno code mt */
			tv[1] += 0.5 * poids;  
		}
		if ((ci1 == 'A') && (ci2 == 'T') && (ci3 == 'G') && (cj1 == 'A') && (cj2 == 'T') && (cj3 == 'A')) {
			ti[1] -= 0.5 * poids; /* TI ATG / ATA : non syno code univ, syno code mt */
			tv[1] += 0.5 * poids;  
		}
}
		}

	}
}




void
titv2(char *cod1, char *cod2, double *ti, double *tv, double* l, int *aa, double **rl, int* pos)
{

	char            codint1[4], codint2[4];
	int             i, j, n, aa1, aa2, aaint1, aaint2;
	double          l1, l2, p1, p2;
	void            titv1(char *, char *, double, double *, double *,double*);


        memcpy(codint1, cod1, 3);
        memcpy(codint2, cod1, 3); /* codint_2_ <-- cod_1_ : no problem */
	for (i = 0; i < 2; i++) {
		if (cod1[i] != cod2[i]){
			codint1[i] = cod2[i];
			break;
		}
	}
	for (j = i + 1; j <= 2; j++) {
		if (cod1[j] != cod2[j]){
			codint2[j] = cod2[j];
			break;
		}
	}


	aa1=aa[num(cod1)]; aa2=aa[num(cod2)];
	aaint1=aa[num(codint1)]; aaint2=aa[num(codint2)];
	
	l1 = *(rl[aa1] + aaint1) * *(rl[aaint1] + aa2);
	l2 = *(rl[aa1] + aaint2) * *(rl[aaint2] + aa2);
	p1 = (l1+l2)? l1 / (l1 + l2) : 0.;
	p2 = (l1+l2)? 1.-p1 : 0.;
	for (i=0;i<3;i++) if (pos[i]==0) n=i+1;
	l[catsite(cod1[0], cod1[1] ,cod1[2], n)]+=0.333333;
	l[catsite(cod2[0], cod2[1] ,cod2[2], n)]+=0.333333;
	l[catsite(codint1[0], codint1[1] ,codint1[2], n)]+=0.333333*p1;
	l[catsite(codint2[0], codint2[1] ,codint2[2], n)]+=0.333333*p2;



	titv1(cod1, codint1, p1, ti, tv,l);
	titv1(cod2, codint1, p1, ti, tv,l);
	titv1(cod1, codint2, p2, ti, tv,l);
	titv1(cod2, codint2, p2, ti, tv,l);

}

void
titv3(char *cod1, char *cod2, double *ti, double *tv, double* l, int *aa, double **rl)
{

	char           *codint1[6], *codint2[6];
	int             i, j, ii,a,b,c,d,aaa,aab,aac,aad;
	double           like[6], p[6], somli, rlab, rlbc, rlcd;
	void            titv1(char *, char *, double, double *, double *, double*);
	int             num(char *);

	for (i = 0; i < 6; i++) {
		if ((codint1[i] = (char *) malloc(4 * sizeof(char))) == NULL) {
			printf("Erreur d'allocation\n");
			exit(1);
		}
		if ((codint2[i] = (char *) malloc(4 * sizeof(char))) == NULL) {
			printf("Erreur d'allocation\n");
			exit(1);
		}
	}
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3 ; j++)
			if (j != i) {
				if ((i == 0) || ((i == 1) && (j == 0))) {
					ii = 3 * i + j - 1;
				} else {
					ii = 3 * i + j - 2;
				}
				/*sprintf(codint1[ii], "%s", cod1);*/
				memcpy(codint1[ii], cod1, 3);
				*(codint1[ii] + i) = cod2[i];
				/*sprintf(codint2[ii], "%s", codint1[ii]);*/
				memcpy(codint2[ii], codint1[ii], 3);
				*(codint2[ii] + j) = cod2[j];
				a=num(cod1);
				b=num(codint1[ii]);
				c=num(codint2[ii]);
				d=num(cod2);
				aaa=aa[a];
				aab=aa[b];
				aac=aa[c];
				aad=aa[d];
				rlab=*(rl[aaa]+aab);
				rlbc=*(rl[aab]+aac);
				rlcd=*(rl[aac]+aad);
				like[ii] = rlab*rlbc*rlcd;
			}
	}

	somli = 0;
	for (i = 0; i < 6; i++)
		somli += like[i];
	for (i = 0; i < 6; i++) {
		p[i] = like[i] / somli;
		titv1(cod1, codint1[i], p[i], ti, tv,l);
		titv1(codint1[i], codint2[i], p[i], ti, tv,l);
		titv1(codint2[i], cod2, p[i], ti, tv,l);
	}


}


/*
void prefastlwl(double **rl, double **tl0, double **tl1, double **tl2, double **tti0, double **tti1,
                double **tti2, double **ttv0, double **ttv1, double **ttv2) */
void prefastlwl(void)
{
	double           l[3], ti[3], tv[3];
	char             cod1[3], cod2[3];
	int             i, j, ii, jj, nbdiff, pos[3], aa[64], n1, n2, n3;
	void            titv2(char *, char *, double *, double *, double *, int *, double **, int *pos);
	void            titv3(char *, char *, double *, double *, double *, int *, double **);
	void            titv1(char *, char *, double, double *, double *, double *);
	double		 minrl;

/* code des acides amines:
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20    0
F W Y H M L I V P  C  A  G  T  S  Q  N  K  R  E  Q  stop
*/
	
	aa[0] = 17;/* aaa K */
	aa[1] = 16;/* aac N */
	aa[2] = 17;/* aag K */
	aa[3] = 16;/* aat N */
	aa[4] = 13;/* aca T */
	aa[5] = 13;/* acc T */
	aa[6] = 13;/* acg T */
	aa[7] = 13;/* act T */
if(code_mt)
	aa[8] = 0;/* aga * */
else
	aa[8] = 18;/* aga R */
	aa[9] = 14;/* agc S */
if(code_mt)
	aa[10] = 0;/* agg * */
else
	aa[10] = 18;/* agg R */
	aa[11] = 14;/* agt S */
if(code_mt)
	aa[12] = 5;/* ata M */
else
	aa[12] = 7;/* ata I */
	aa[13] = 7;/* atc I */
	aa[14] = 5;/* atg M */
	aa[15] = 7;/* att I */
	aa[16] = 15;
	aa[17] = 4;
	aa[18] = 15;
	aa[19] = 4;
	aa[20] = 9;
	aa[21] = 9;
	aa[22] = 9;
	aa[23] = 9;
	aa[24] = 18;
	aa[25] = 18;
	aa[26] = 18;
	aa[27] = 18;
	aa[28] = 6;
	aa[29] = 6;
	aa[30] = 6;
	aa[31] = 6;
	aa[32] = 19;
	aa[33] = 20;
	aa[34] = 19;
	aa[35] = 20;
	aa[36] = 11;
	aa[37] = 11;
	aa[38] = 11;
	aa[39] = 11;
	aa[40] = 12;
	aa[41] = 12;
	aa[42] = 12;
	aa[43] = 12;
	aa[44] = 8;
	aa[45] = 8;
	aa[46] = 8;
	aa[47] = 8;
	aa[48] = 0;/* taa * */
	aa[49] = 3;/* tac Y */
	aa[50] = 0;/* tag * */
	aa[51] = 3;/* tat Y */
	aa[52] = 14;/* tca S */
	aa[53] = 14;/* tcc S */
	aa[54] = 14;/* tcg S */
	aa[55] = 14;/* tct S */
if(code_mt)
	aa[56] = 2;/* tga W */
else
	aa[56] = 0;/* tga * */
	aa[57] = 10;/* tgc */
	aa[58] = 2;/* tgg W */
	aa[59] = 10;/* tgt */
	aa[60] = 6;/* tta */
	aa[61] = 1;/* ttc */
	aa[62] = 6;/* ttg */
	aa[63] = 1;/* ttt */


/* ajoute par M. Gouy */
/* calcul minrl = val minimale du tableau rl */
minrl=rl[1][1];
for(i=1; i<=20; i++)
	for(j=i+1; j<=20; j++)
		if(rl[i][j] < minrl ) minrl=rl[i][j];
/* chargement rl[0][i] et rl[i][0] avec minrl correspond a aa = stop */
	for(i= 0; i<=20; i++) rl[0][i] = rl[i][0] = minrl;

//MG repair	for (i = 0; i < 63; i++) {
		for (i = 0; i < 64; i++) {

		for (j = i; j < 64; j++) {

			for(ii=0;ii<3;ii++){
				l[ii]=ti[ii]=tv[ii]=0;
			}


			n1 = i / 16;
			n2 = (i - 16 * n1) / 4;
			n3 = i - 16 * n1 - 4 * n2;
			cod1[0] = 'A';
			if (n1 == 1)
				cod1[0] = 'C';
			if (n1 == 2)
				cod1[0] = 'G';
			if (n1 == 3)
				cod1[0] = 'T';
			cod1[1] = 'A';
			if (n2 == 1)
				cod1[1] = 'C';
			if (n2 == 2)
				cod1[1] = 'G';
			if (n2 == 3)
				cod1[1] = 'T';
			cod1[2] = 'A';
			if (n3 == 1)
				cod1[2] = 'C';
			if (n3 == 2)
				cod1[2] = 'G';
			if (n3 == 3)
				cod1[2] = 'T';

			n1 = j / 16;
			n2 = (j - 16 * n1) / 4;
			n3 = j - 16 * n1 - 4 * n2;
			cod2[0] = 'A';
			if (n1 == 1)
				cod2[0] = 'C';
			if (n1 == 2)
				cod2[0] = 'G';
			if (n1 == 3)
				cod2[0] = 'T';
			cod2[1] = 'A';
			if (n2 == 1)
				cod2[1] = 'C';
			if (n2 == 2)
				cod2[1] = 'G';
			if (n2 == 3)
				cod2[1] = 'T';
			cod2[2] = 'A';
			if (n3 == 1)
				cod2[2] = 'C';
			if (n3 == 2)
				cod2[2] = 'G';
			if (n3 == 3)
				cod2[2] = 'T';


			nbdiff = 0;
			pos[0] = pos[1] = pos[2] = 0;
			if (cod1[0] != cod2[0]) {
				nbdiff++;
				pos[0] = 1;
			}
			if (cod1[1] != cod2[1]) {
				nbdiff++;
				pos[1] = 1;
			}
			if (cod1[2] != cod2[2]) {
				nbdiff++;
				pos[2] = 1;
			}

			if (nbdiff != 2)
				for (jj = 0; jj < 3; jj++)
					if (pos[jj] == 0) {
						l[catsite(cod1[0], cod1[1], cod1[2], jj + 1)] += 0.5;
						l[catsite(cod2[0], cod2[1], cod2[2], jj + 1)] += 0.5;
					}
			if (nbdiff == 1)
				titv1(cod1, cod2, 1.0, ti, tv, l);
			if (nbdiff == 2)
				titv2(cod1, cod2, ti, tv, l, aa, rl, pos);
			if (nbdiff == 3)
				titv3(cod1, cod2, ti, tv, l, aa, rl);

			
			*(tl0[i]+j)=*(tl0[j]+i)=l[0];
			*(tl1[i]+j)=*(tl1[j]+i)=l[1];
			*(tl2[i]+j)=*(tl2[j]+i)=l[2];
			*(tti0[i]+j)=*(tti0[j]+i)=ti[0];
			*(tti1[i]+j)=*(tti1[j]+i)=ti[1];
			*(tti2[i]+j)=*(tti2[j]+i)=ti[2];
			*(ttv0[i]+j)=*(ttv0[j]+i)=tv[0];
			*(ttv1[i]+j)=*(ttv1[j]+i)=tv[1];
			*(ttv2[i]+j)=*(ttv2[j]+i)=tv[2];

		}
	}
return;	
}



int 
/*
fastlwl(char **seq, int nbseq, int lgseq, double **ka, double **ks, double **tti0, double **tti1, double **tti2,
        double **ttv0, double **ttv1, double **ttv2, double **tl0, double **tl1, double **tl2, double **vka, 
        double **vks, int* sat1, int* sat2) */
fastlwl(char **seq, int nbseq, int lgseq, double **ka, double **ks, double **vka, double **vks, int* sat1, int* sat2,
	int *wght)
{
	double           l[3], a[3], b[3], p[3], q[3], ti[3], tv[3],
	                aaa[3], bb[3];
	char            cod1[3], cod2[3];
	int             i, j, ii, num1, num2, weight;

	*sat1=-1; *sat2=-1;

	if (lgseq % 3 != 0) {
		return 1;
	}

	for (i = 0; i < nbseq - 1; i++) {
		for (j = i + 1; j < nbseq; j++) {
		  if(tree_build_interrupted) return 1;
			l[0] = l[1] = l[2] = 0.;
			ti[0] = ti[1] = ti[2] = tv[0] = tv[1] = tv[2] = 0.;

			for (ii = 0; ii < lgseq / 3; ii++) {
				weight = wght[3 * ii];//MG: use bootstrap codon weights
				if(weight == 0) continue;
				cod1[0] = *(seq[i] + 3 * ii);
				cod1[1] = *(seq[i] + 3 * ii + 1);
				cod1[2] = *(seq[i] + 3 * ii + 2);
				cod2[0] = *(seq[j] + 3 * ii);
				cod2[1] = *(seq[j] + 3 * ii + 1);
				cod2[2] = *(seq[j] + 3 * ii + 2);
				num1 = num(cod1);
				num2 = num(cod2);
				if(num1 == 64 || num2 == 64) continue;//MG ignore - or N-containing codons
				l[0] += weight * tl0[num1][num2];
				l[1] += weight * tl1[num1][num2];
				l[2] += weight * tl2[num1][num2];
				ti[0] += weight * tti0[num1][num2];
				ti[1] += weight * tti1[num1][num2];
				ti[2] += weight * tti2[num1][num2];
				tv[0] += weight * ttv0[num1][num2];
				tv[1] += weight * ttv1[num1][num2];
				tv[2] += weight * ttv2[num1][num2];
			}
			for (ii = 0; ii < 3; ii++) {
				if (l[ii]==0.) { a[ii]=0.; b[ii]=0.; } 
				else{
					p[ii] = ti[ii] / l[ii];
					q[ii] = tv[ii] / l[ii];
					
					aaa[ii] = 1 - 2 * p[ii] - q[ii];
					bb[ii] = 1 - 2 * q[ii];
				/*	cc[ii] = (aaa[ii] + bb[ii]) / 2;   */

					if (bb[ii] <= 0.) {
						b[ii] = -10001.;
					} else
						b[ii] = - 0.5 * (double) log(bb[ii]);
//MG simpler			b[ii] = 0.5 * (double) log(1./bb[ii]);
	
					if ((aaa[ii] <= 0.) || (bb[ii] <= 0.)) {
						a[ii] = -10001.;
					} else
						a[ii] = - 0.5 * (double) log(aaa[ii]) + 0.25 * log(bb[ii]);
//MG simpler			a[ii] = 0.5 * (double) log(1./aaa[ii]) - 0.25 * log(1./bb[ii]);
				}
			}

			if ((a[1] > -10000.) && (a[2] > -10000.) && (b[2] > -10000.) && ks){
				if(l[1]+l[2]==0.) ks[i][j]=0.;
				else ks[i][j] = (l[1] * a[1] + l[2] * a[2]) / (l[2] + l[1]) + b[2];
			}
			else {
				if (ks){
				  *sat1=i; *sat2=j; return 1;
				}
			}

			if ((a[0] > -10000.) && (b[0]> -10000.) && (b[1]> -10000.) && ka){
				if(l[0]+l[1]==0.) ka[i][j]=0.;
				else ka[i][j] = a[0] + (l[0] * b[0] + l[1] * b[1]) / (l[0] + l[1]);
			}
			else {
				if (ka) {
				  *sat1=i;
				  *sat2=j;
				  return 1;
				}
			}
		}
	}
	return 0;
}

/* loadrl */
/* Load rl's values = weights of aminoacids substitutions for Ka/Ks computing. */

void loadrl(void){
	int i, j;
	
	
	for(i=0;i<64;i++){
		tl0[i]=(double*)check_alloc(64, sizeof(double));
		tl1[i]=(double*)check_alloc(64, sizeof(double));
		tl2[i]=(double*)check_alloc(64, sizeof(double));
		tti0[i]=(double*)check_alloc(64, sizeof(double));
		tti1[i]=(double*)check_alloc(64, sizeof(double));
		tti2[i]=(double*)check_alloc(64, sizeof(double));
		ttv0[i]=(double*)check_alloc(64, sizeof(double));
		ttv1[i]=(double*)check_alloc(64, sizeof(double));
		ttv2[i]=(double*)check_alloc(64, sizeof(double));
	}
	
	for(i=0;i<=20;i++){
		rl[i]=(double*)check_alloc(21, sizeof(double));
		rl[0][i]=rl[i][0]=0.;
	}
	
	rl[2][1]=rl[3][1]=rl[3][2]=rl[5][1]=rl[5][3]=rl[6][1]=rl[6][3]=rl[6][5]=0.382;
	rl[7][1]=rl[7][3]=rl[7][5]=rl[7][6]=rl[8][1]=rl[8][5]=rl[8][6]=rl[8][7]=0.382;
	rl[11][9]=rl[12][9]=rl[13][4]=rl[13][9]=rl[15][4]=rl[15][13]=rl[16][14]=rl[16][15]=0.382;
	rl[17][4]=rl[18][4]=rl[18][15]=rl[18][17]=rl[19][4]=rl[19][15]=rl[19][16]=rl[20][16]=0.382;
	rl[20][19]=0.382;
	
	rl[4][1]=rl[4][3]=rl[5][2]=rl[5][4]=rl[6][2]=rl[6][4]=rl[7][2]=rl[7][4]=0.343;
	rl[8][2]=rl[8][3]=rl[8][4]=rl[9][4]=rl[9][5]=rl[9][6]=rl[9][7]=rl[9][8]=0.343;
	rl[11][4]=rl[11][5]=rl[11][6]=rl[11][7]=rl[11][8]=rl[12][4]=rl[12][11]=rl[13][3]=0.343;
	rl[13][5]=rl[13][6]=rl[13][7]=rl[13][8]=rl[13][11]=rl[13][12]=rl[14][4]=rl[14][9]=0.343;
	rl[14][11]=rl[14][12]=rl[14][13]=rl[15][3]=rl[15][8]=rl[15][9]=rl[15][11]=rl[15][12]=0.343;
	rl[15][14]=rl[16][4]=rl[16][9]=rl[16][12]=rl[16][13]=rl[17][3]=rl[17][5]=rl[17][8]=0.343;
	rl[17][13]=rl[17][15]=rl[17][16]=rl[18][1]=rl[18][3]=rl[18][5]=rl[18][7]=rl[18][8]=0.343;
	rl[18][13]=rl[18][16]=rl[19][9]=rl[19][12]=rl[19][13]=rl[19][14]=rl[19][17]=rl[19][18]=0.343;
	rl[20][4]=rl[20][12]=rl[20][13]=rl[20][14]=rl[20][15]=rl[20][18]=0.343;
	
	rl[4][2]=rl[9][1]=rl[9][2]=rl[9][3]=rl[11][1]=rl[11][2]=rl[11][3]=rl[12][5]=0.128;
	rl[12][6]=rl[12][7]=rl[12][8]=rl[13][1]=rl[13][2]=rl[13][10]=rl[14][3]=rl[14][5]=0.128;
	rl[14][6]=rl[14][7]=rl[14][8]=rl[14][10]=rl[15][1]=rl[15][2]=rl[15][5]=rl[15][6]=0.128;
	rl[15][7]=rl[16][3]=rl[16][5]=rl[16][7]=rl[16][8]=rl[16][10]=rl[16][11]=rl[17][1]=0.128;
	rl[17][2]=rl[17][6]=rl[17][7]=rl[17][9]=rl[17][11]=rl[17][12]=rl[17][14]=rl[18][2]=0.128;
	rl[18][6]=rl[18][9]=rl[18][11]=rl[18][12]=rl[18][14]=rl[19][1]=rl[19][3]=rl[19][5]=0.128;
	rl[19][6]=rl[19][7]=rl[19][8]=rl[19][11]=rl[20][9]=rl[20][11]=rl[20][17]=rl[12][3]=0.128;
	
	rl[10][1]=rl[10][2]=rl[10][3]=rl[10][4]=rl[10][5]=rl[10][6]=rl[10][7]=rl[10][8]=0.040;
	rl[10][9]=rl[11][10]=rl[12][1]=rl[12][2]=rl[12][10]=rl[14][1]=rl[14][2]=rl[15][10]=0.040;
	rl[16][1]=rl[16][2]=rl[16][6]=rl[17][10]=rl[18][10]=rl[19][2]=rl[19][10]=rl[20][1]=0.040;
	rl[20][2]=rl[20][3]=rl[20][5]=rl[20][6]=rl[20][7]=rl[20][8]=rl[20][10]=0.040;
	
	
	
	for (i = 1; i <= 20; i++) {
		*(rl[i] + i) = 1.0;
		for (j = i + 1; j <= 20; j++)
			*(rl[i] + j) = *(rl[j] + i);
	}
}

