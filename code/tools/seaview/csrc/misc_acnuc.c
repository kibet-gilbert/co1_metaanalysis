
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#if defined(WIN32)
#include <Windows.h>
#elif defined(unix) || defined(__APPLE__)
#define unixlike
#include <termios.h>
#endif



/* included functions */
int trim_key(char *name); /* remove trailing spaces */
void padtosize(char *pname, char *name, int length);
char *get_code_descr(int code);
int calc_codon_number(char *codon);
char codaa(char *codon, int code);
int get_ncbi_gc_number(int gc);
int get_acnuc_gc_number(int ncbi_gc);
int strcmptrail(char *s1, int l1, char *s2, int l2);
void majuscules(char *name);
void compact(char *chaine);
void *mycalloc(int nbr, size_t taille);
char complementer_base(char nucl);
void complementer_seq(char *deb_ch, int l);
char init_codon_to_aa(char *codon, int gc);
char stop_codon_to_aa(char *codon, int gc);
int notrail2(char *chaine, int len);
int prepch(char *chaine, char **posmot);
int compch(char *cible, int lcible, char **posmot, int nbrmots);
int chg_acnuc(char *acnucvar, char *gcgacnucvar);
void gets_no_echo(char *password, size_t lpw);


int trim_key(char *name) /* remove trailing spaces */
{
char *p;
int l = (int)strlen(name);
p = name + l - 1;
while( p >= name && *p == ' ' ) *(p--) = 0;
return (int)((p + 1) - name);
}


void padtosize(char *pname, char *name, int length)
{
int i;
strncpy(pname,name,length);
pname[length]=0;
for(i=(int)strlen(pname);i<length;i++) pname[i]=' ';
}


#define TOTCODES 25  /* nbre total de codes definis, 0 inclus */
int totcodes=TOTCODES;

char aminoacids[]="RLSTPAGVKNQHEDYCFIMW*X";

struct genetic_code_libel { /* definition d'un code genetique */
	char libel[61]; /* nom du code decrivant ses variants % code standard */
	char* target;
	int code[65]; /* tableau codon->acide amine */
	int ncbi_gc; /* numero NCBI du meme code */
	int codon_init[64]; /* tableau codon initiateur ou stop -> acide amine */
	};

/* 
les codons sont numerotes de 1 a 64 selon ordre alphabetique;
le numero 65 est attribue a tout codon avec base hors AcCcGgTtUu
les acides amines sont numerotes selon l'ordre de la variable aminoacids
de un a 20 + * pour stop et X pour inconnu.
 Table codon_init gives 19 (=M) for initiation codons and 21 (=*) for stop codons.
 In some genetic codes, the same codon corresponds to an aa in table code
 and to a stop in table codon_init.
*/

/* initialisation de tous les codes genetiques */
struct genetic_code_libel genetic_code[TOTCODES] = 
{

{ /* 0: standard */
	{"Standard genetic code"},
	"Standard",
/*ANN*/	{9,10,9,10,4,4,4,4,1,3,1,3,18,18,19,18,
/*CNN*/	11,12,11,12,5,5,5,5,1,1,1,1,2,2,2,2,
/*GNN*/	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
/*TNN*/	21,15,21,15,3,3,3,3,21,16,20,16,2,17,2,17,22},
/*ncbi*/1,
/*init*/{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* CUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	21,0,21,0,0,0,0,0,21,0,0,0,0,0,19,0} /* UUG */
}
,
{ /* 1: yeast mt */
	{"CUN=T  AUA=M  UGA=W"},
	"Yeast Mitochondrial",
	{9,10,9,10,4,4,4,4,1,3,1,3,19,18,19,18,
	11,12,11,12,5,5,5,5,1,1,1,1,4,4,4,4,
	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
	21,15,21,15,3,3,3,3,20,16,20,16,2,17,2,17,22},
	3,
	{0,0,0,0,0,0,0,0,0,0,0,0,19,0,19,0, /* AUA, AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	21,0,21,0,0,0,0,0,0,0,0,0,0,0,0,0} 
}
,
{ /* 2: :    MITOCHONDRIAL CODE OF VERTEBRATES */
	{"AGR=*  AUA=M  UGA=W"},
	"Vertebrate Mitochondrial",
     {9,10,9,10,4,4,4,4,21,3,21,3,19,18,19,18,11,12,11,12,
     5,5,5,5,1,1,1,1,2,2,2,2,13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,21,15,
     21,15,3,3,3,3,20,16,20,16,2,17,2,17,22},
	2,
	{0,0,0,0,0,0,0,0,21,0,21,0,19,19,19,19, /* AUN */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* GUG */
	21,0,21,0,0,0,0,0,0,0,0,0,0,0,0,0}
}
,
{ /* 3:   MITOCHONDRIAL CODE OF FILAMENTOUS FUNGI */
	{"UGA=W"},
	"Mold Mitochondrial; Protozoan Mitochondrial; Coelenterate Mitochondrial; Mycoplasma; Spiroplasma",
     {9,10,9,10,4,4,4,4,1,3,1,3,18,18,19,18,11,12,11,12,5,5,5,
     5,1,1,1,1,2,2,2,2,13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,21,15,21,
     15,3,3,3,3,20,16,20,16,2,17,2,17,22},
	4,
	{0,0,0,0,0,0,0,0,0,0,0,0,19,19,19,19, /* AUN */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* CUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* GUG */
	21,0,21,0,0,0,0,0,0,0,0,0,19,0,19,0} /* UUR */
}
,
{ /* 4:    MITOCHONDRIAL CODE OF INSECT AND PLATYHELMINTHES  */
	{"AUA=M  UGA=W  AGR=S"},
	"Invertebrate Mitochondrial",
     {9,10,9,10,4,4,4,4,3,3,3,3,19,18,19,18,11,12,11,12,5,5,5,
     5,1,1,1,1,2,2,2,2,13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,21,15,21,
     15,3,3,3,3,20,16,20,16,2,17,2,17,22},
	5,
	{0,0,0,0,0,0,0,0,0,0,0,0,19,19,19,19, /* AUN */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* GUG */
	21,0,21,0,0,0,0,0,0,0,0,0,0,0,19,0} /* UUG */
}
,
{ /* 5:    Nuclear code of Candida cylindracea (see nature 341:164) */
	{"CUG=S"},
	"Alternative Yeast Nuclear",
     	{9,10,9,10,4,4,4,4,1,3,1,3,18,18,19,18,
	11,12,11,12,5,5,5,5,1,1,1,1,2,2,3,2,
	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
	21,15,21,15,3,3,3,3,21,16,20,16,2,17,2,17,22},
	12,
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* CUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	21,0,21,0,0,0,0,0,21,0,0,0,0,0,0,0}
}
,
{ /* 6:   NUCLEAR CODE OF CILIATA: UAR = Gln = Q */
	{"UAR=Q"},
	"Ciliate Nuclear; Dasycladacean Nuclear; Hexamita Nuclear",
     {9,10,9,10,4,4,4,4,1,3,1,3,18,18,19,18,11,12,11,12,5,5,5,
     5,1,1,1,1,2,2,2,2,13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,11,15,11,
     15,3,3,3,3,21,16,20,16,2,17,2,17,22},
	6,
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,21,0,0,0,0,0,0,0}
}
,
{ /* 7:   NUCLEAR CODE OF EUPLOTES */
	{"UGA=C"},
	"Euplotid Nuclear",
     {9,10,9,10,4,4,4,4,1,3,1,3,18,18,19,18,11,12,11,12,5,5,5,
     5,1,1,1,1,2,2,2,2,13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,21,15,21,
     15,3,3,3,3,16,16,20,16,2,17,2,17,22},
	10,
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	21,0,21,0,0,0,0,0,0,0,0,0,0,0,0,0}
}
,
{ /* 8:   MITOCHONDRIAL CODE OF ECHINODERMS */
	{"UGA=W  AGR=S  AAA=N"},
	"Echinoderm Mitochondrial; Flatworm Mitochondrial",
     	{10,10,9,10,4,4,4,4,3,3,3,3,18,18,19,18,
	11,12,11,12,5,5,5,5,1,1,1,1,2,2,2,2,
	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
	21,15,21,15,3,3,3,3,20,16,20,16,2,17,2,17,22},
	9,
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* GUG */
	21,0,21,0,0,0,0,0,0,0,0,0,0,0,0,0}
}
,
{ /* 9:   MITOCHONDRIAL CODE OF ASCIDIACEA */
	{"UGA=W  AGR=G  AUA=M"},
	"Ascidian Mitochondrial",
     	{9,10,9,10,4,4,4,4,7,3,7,3,19,18,19,18,
	11,12,11,12,5,5,5,5,1,1,1,1,2,2,2,2,
	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
	21,15,21,15,3,3,3,3,20,16,20,16,2,17,2,17,22},
	13,
	{0,0,0,0,0,0,0,0,0,0,0,0,19,0,19,0, /* AUA,AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* GUG */
	21,0,21,0,0,0,0,0,0,0,0,0,0,0,19,0} /* UUG */
}
,
{ /* 10:   MITOCHONDRIAL CODE OF PLATYHELMINTHES */
	{"UGA=W  AGR=S  UAA=Y AAA=N"},
	"Alternative Flatworm Mitochondrial",
	{10,10,9,10,4,4,4,4,3,3,3,3,18,18,19,18,
	11,12,11,12,5,5,5,5,1,1,1,1,2,2,2,2,
	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
	15,15,21,15,3,3,3,3,20,16,20,16,2,17,2,17,22},
	14,
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,0}
}
,
{ /* 11:   NUCLEAR CODE OF BLEPHARISMA */
	{"UAG=Q"},
	"Blepharisma Macronuclear",
/*ANN*/	{9,10,9,10,4,4,4,4,1,3,1,3,18,18,19,18,
/*CNN*/	11,12,11,12,5,5,5,5,1,1,1,1,2,2,2,2,
/*GNN*/	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
/*TNN*/	21,15,11,15,3,3,3,3,21,16,20,16,2,17,2,17,22},
	15,
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	21,0,0,0,0,0,0,0,21,0,0,0,0,0,0,0}
}
,
{ /* 12:   NUCLEAR CODE OF BACTERIA: differs only for initiation codons */
	{"NUG=AUN=M when initiation codon"},
	"Bacterial, Archaeal and Plant Plastid",
/*ANN*/	{9,10,9,10,4,4,4,4,1,3,1,3,18,18,19,18,
/*CNN*/	11,12,11,12,5,5,5,5,1,1,1,1,2,2,2,2,
/*GNN*/	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
/*TNN*/	21,15,21,15,3,3,3,3,21,16,20,16,2,17,2,17,22},
	11,
	{0,0,0,0,0,0,0,0,0,0,0,0,19,19,19,19, /* AUN */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* CUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* GUG */
	21,0,21,0,0,0,0,0,21,0,0,0,0,0,19,0} /* UUG */
}
,
{ /* 13: Chlorophycean Mitochondrial */
	{"UAG=Leu"},
	"Chlorophycean Mitochondrial",
/*ANN*/	{9,10,9,10,4,4,4,4,1,3,1,3,18,18,19,18,
/*CNN*/	11,12,11,12,5,5,5,5,1,1,1,1,2,2,2,2,
/*GNN*/	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
/*TNN*/	21,15,2,15,3,3,3,3,21,16,20,16,2,17,2,17,22},
/*ncbi*/16,
/*init*/{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	21,0,0,0,0,0,0,0,21,0,0,0,0,0,0,0}
}
,
{ /* 14:    MITOCHONDRIAL CODE OF TREMATODE  */
	{"AUA=M  UGA=W  AGR=S AAA=N"},
	"Trematode Mitochondrial",
     {10,10,9,10,4,4,4,4,3,3,3,3,19,18,19,18,11,12,11,12,5,5,5,
     5,1,1,1,1,2,2,2,2,13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,21,15,21,
     15,3,3,3,3,20,16,20,16,2,17,2,17,22},
	21,
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* GUG */
	21,0,21,0,0,0,0,0,0,0,0,0,0,0,0,0} 
}
,
{ /* 15: TAG-Leu,TCA-stop */
	{"UAG=L UCA=*"},
	"Scenedesmus obliquus mitochondrial",
/*ANN*/	{9,10,9,10,4,4,4,4,1,3,1,3,18,18,19,18,
/*CNN*/	11,12,11,12,5,5,5,5,1,1,1,1,2,2,2,2,
/*GNN*/	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
/*TNN*/	21,15,2,15,21,3,3,3,21,16,20,16,2,17,2,17,22},
/*ncbi*/22,
/*init*/{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	21,0,0,0,21,0,0,0,21,0,0,0,0,0,0,0} 
}
,
{ /* 16: Thraustochytrium-mt */
	{"UUA=*"},
	"Thraustochytrium mitochondrial",
/*ANN*/	{9,10,9,10,4,4,4,4,1,3,1,3,18,18,19,18,
/*CNN*/	11,12,11,12,5,5,5,5,1,1,1,1,2,2,2,2,
/*GNN*/	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
/*TNN*/	21,15,21,15,3,3,3,3,21,16,20,16,21,17,2,17,22},
/*ncbi*/23,
/*init*/{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,19, /* AUG AUU */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* GUG */
	21,0,21,0,0,0,0,0,21,0,0,0,21,0,0,0} 
}
  ,
  { /* 17:   MITOCHONDRIAL CODE OF Pterobranchia */
    {"UGA=W  AGA=S  AGG=K"},
    "Pterobranchia Mitochondrial",
/*ANN*/	{9,10,9,10,4,4,4,4,3,3,9,3,18,18,19,18,
/*CNN*/	11,12,11,12,5,5,5,5,1,1,1,1,2,2,2,2,
/*GNN*/	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
/*TNN*/	21,15,21,15,3,3,3,3,20,16,20,16,2,17,2,17,22},
/*ncbi*/24,
/*init*/{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* CUG */
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* GUG */
         21,0,21,0,0,0,0,0,0,0,0,0,0,0,19,0} /* UUG */
  }
  ,
  { /* 18: Candidate Division SR1 and Gracilibacteria */
    {"UGA=G"},
    "Candidate Division SR1 and Gracilibacteria",
    /*ANN*/	{9,10,9,10,4,4,4,4,1,3,1,3,18,18,19,18,
    /*CNN*/	11,12,11,12,5,5,5,5,1,1,1,1,2,2,2,2,
    /*GNN*/	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
    /*TNN*/	21,15,21,15,3,3,3,3,7,16,20,16,2,17,2,17,22},
    /*ncbi*/25,
    /*init*/{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* GUG */
      21,0,21,0,0,0,0,0,0,0,0,0,0,0,19,0} /* UUG */
  }
  ,
  { /* 19: Pachysolen tannophilus */
    {"CUG=A"},
    "Pachysolen tannophilus Nuclear",
/*ANN*/	{9,10,9,10,4,4,4,4,1,3,1,3,18,18,19,18,
/*CNN*/	11,12,11,12,5,5,5,5,1,1,1,1,2,2,6,2,
/*GNN*/	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
/*TNN*/	21,15,21,15,3,3,3,3,21,16,20,16,2,17,2,17,22},
/*ncbi*/26,
/*init*/{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* CUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	21,0,21,0,0,0,0,0,21,0,0,0,0,0,0,0}
  }
  ,
    { /* 20: Karyorelict Nuclear */
    {"UAR=Q, UGA=W, CUG=A"},
    "Karyorelict Nuclear",
/*ANN*/	{9,10,9,10,4,4,4,4,1,3,1,3,18,18,19,18,
/*CNN*/	11,12,11,12,5,5,5,5,1,1,1,1,2,2,6,2,
/*GNN*/	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
/*TNN*/	11,15,11,15,3,3,3,3,20,16,20,16,2,17,2,17,22},
/*ncbi*/27,
/*init*/{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,21,0,0,0,0,0,0,0}
  }
  ,
    { /* 21: Condylostoma Nuclear */
    {"UAR=Q, UGA=W, CUG=A"},
    "Condylostoma Nuclear",
/*ANN*/	{9,10,9,10,4,4,4,4,1,3,1,3,18,18,19,18,
/*CNN*/	11,12,11,12,5,5,5,5,1,1,1,1,2,2,6,2,
/*GNN*/	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
/*TNN*/	11,15,11,15,3,3,3,3,20,16,20,16,2,17,2,17,22},
/*ncbi*/28,
/*init*/{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	21,0,21,0,0,0,0,0,21,0,0,0,0,0,0,0}
  }
  ,
    { /* 22: Mesodinium Nuclear */
    {"UAR=Y, CUG=A"},
    "Mesodinium Nuclear",
/*ANN*/	{9,10,9,10,4,4,4,4,1,3,1,3,18,18,19,18,
/*CNN*/	11,12,11,12,5,5,5,5,1,1,1,1,2,2,6,2,
/*GNN*/	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
/*TNN*/	15,15,15,15,3,3,3,3,21,16,20,16,2,17,2,17,22},
/*ncbi*/29,
/*init*/{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,21,0,0,0,0,0,0,0}
  }
  ,
    { /* 23: Peritrich Nuclear */
    {"UAR=E, CUG=A"},
    "Peritrich Nuclear",
/*ANN*/	{9,10,9,10,4,4,4,4,1,3,1,3,18,18,19,18,
/*CNN*/	11,12,11,12,5,5,5,5,1,1,1,1,2,2,6,2,
/*GNN*/	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
/*TNN*/	13,15,13,15,3,3,3,3,21,16,20,16,2,17,2,17,22},
/*ncbi*/30,
/*init*/{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,21,0,0,0,0,0,0,0}
  }
  ,
    { /* 24: Blastocrithidia Nuclear */
    {"UAR=E, UGA=W"},
    "Blastocrithidia Nuclear",
/*ANN*/	{9,10,9,10,4,4,4,4,1,3,1,3,18,18,19,18,
/*CNN*/	11,12,11,12,5,5,5,5,1,1,1,1,2,2,2,2,
/*GNN*/	13,14,13,14,6,6,6,6,7,7,7,7,8,8,8,8,
/*TNN*/	13,15,13,15,3,3,3,3,20,16,20,16,2,17,2,17,22},
/*ncbi*/31,
/*init*/{0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0, /* AUG */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	21,0,21,0,0,0,0,0,0,0,0,0,0,0,0,0}
  }


/*       1         2
1234567890123456789012
RLSTPAGVKNQHEDYCFIMW*X
*/

};


char *get_code_descr(int code)
/* 
get a 60-letter (or less) description of a variant genetic code
return value	pointer to the description, not to be altered!
*/
{
if(code>=0 && code<totcodes)
	return genetic_code[code].libel ;
else	
	return "Unknown genetic code. Standard code is used.";
}


char *get_code_target(int code)
/* 
get a description of target organisms of a variant genetic code
return value	pointer to the description, not to be altered!
*/
{
if(code >= 0 && code < totcodes)
	return genetic_code[code].target ;
else	
	return "Unknown genetic code. Standard code is used.";
}


int calc_codon_number(char *codon)
{
static char nucleotides[] = "AaCcGgTtUu";
static int nucnum[5] = {0,1,2,3,3};
int num, i, base;
char *p;

num = 0;
for(i = 1; i <= 3; i++) {
	p = strchr(nucleotides, *codon);
	if(p == NULL) {
		num = 64;
		break;
		}
	else
		base = (int)(p-nucleotides)/2;
	num = num * 4 + nucnum[base];
	codon++;
	}
return num;
}


char codaa(char *codon, int code)
/*
amino acid translation:
codon	a 3-base string
code	the genetic code to be used
return value	the amino acid as 1 character
*/
{
struct genetic_code_libel *pdata;
int num;

num = calc_codon_number(codon);
if(code < 0 || code >= totcodes)code = 0;/*use regular code if unknown number */
pdata = &genetic_code[code]; /* ici ecriture plus compacte mal compilee sur PC*/
return aminoacids[ pdata->code[num] - 1 ];
}


int get_ncbi_gc_number(int gc)
{ /* from acnuc to ncbi genetic code number */
return genetic_code[gc].ncbi_gc;
}


int get_acnuc_gc_number(int ncbi_gc)
{ /* from ncbi to acnuc genetic code number (returns 0 if not found) */
int num;

for( num = 0; num < totcodes; num++ ) 
	if(genetic_code[num].ncbi_gc == ncbi_gc) return num;
return 0;
}


int strcmptrail(char *s1, int l1, char *s2, int l2)
/*
compare strings s1 and s2 of length l1 and l2 as done by strcmp
but ignores all trailing spaces
*/
{
char *fin;
int l, flag=1;

if(l1 > 0) {
	if( (fin = (char *)memchr(s1, 0, l1) ) != NULL) l1 = (int)(fin - s1);
	}
if(l2 > 0) {
	if( (fin = (char *)memchr(s2, 0, l2) ) != NULL) l2 = (int)(fin - s2);
	}

if(l2 > l1) {
	flag = -1;
	fin=s1; s1=s2; s2=fin;
	l=l1; l1=l2; l2=l;
	}
l = l2;
fin = s2 + l;
while(s2 < fin) {
	if( *s1 != *s2 ) return (*s1 - *s2)*flag;
	s1++; s2++;
	}
fin= s1+l1-l2;
while(s1 < fin)	{
	if( *s1 != ' ') return flag;
	s1++;
	}
return 0;
}


void majuscules(char *name)
{
name--;
while(*(++name) != 0) *name = toupper(*name);
}


void compact(char *chaine)
{
int l;
char *p, *q;

l= (int)strlen(chaine); p=chaine+l;
while( *(--p) == ' ' && p>=chaine) *p=0;
while((p=strchr(chaine,' '))!=NULL) {
	q=p+1;
	while(*q==' ') q++;
	l= (int)(q-p);
	while(*q!=0) {*(q-l) = *q; q++; }
	*(q-l)=0;
	}
}



void *mycalloc(int nbre, size_t size)
{
void *point;

point = calloc(nbre,size);
if(point == NULL) {
	fprintf(stderr,"Error: problem allocating memory.\n");
	exit(1);
	}
return point;
}


char complementer_base(char nucl)
{
    switch (nucl) {
        case 'a':
        case 'A': return('t');

        case 'c':
        case 'C': return('g');

        case 'g':
        case 'G': return('c');
  
        case 'u':
        case 'U':
        case 't':
        case 'T': return('a');

        case 'r':
        case 'R': return('y');

        case 'y':
        case 'Y': return('r');

	default : return('n');

	}
}
   

/*   ~~~~~~~~~~~~ retourne le complementaire d'une sequence ~~~~~~~~~~~ 
 * recoit l'adresse du debut d'un tableau de caractere et sa longueur
 * inverse et complemente cette sequence
 * prend en compte si c'est un adn ou un arn
 * -------------------------------------------------------------------- */

void complementer_seq(char *deb_ch, int l)
{
    int ii = 0;
    char compl1,compl2;

    for(ii = 0; ii <= (l-1)/2; ii++)
    {
	compl1 = complementer_base(*(deb_ch+ii));

	compl2 = complementer_base(*(deb_ch+l-ii-1));

	*(deb_ch+ii)     = compl2;
	*(deb_ch+l-ii-1) = compl1;
    }	

 
}


char init_codon_to_aa(char *codon, int gc)
{
int num, aa;
struct genetic_code_libel *pdata;

num = calc_codon_number(codon);
if(num >= 64) return 'X';
/* use regular code if unknown number */
if(gc < 0 || gc >= totcodes) gc = 0; 
pdata = &genetic_code[gc];
aa = pdata->codon_init[num];
/* if not listed in expected init codons */
if(aa == 0 || aa == 21) aa = pdata->code[num];
return aminoacids[aa - 1];
}


char stop_codon_to_aa(char *codon, int gc)
{
int num, aa;
struct genetic_code_libel *pdata;

num = calc_codon_number(codon);
if(num >= 64) return 'X';
/* use regular code if unknown number */
if(gc < 0 || gc >= totcodes) gc = 0; 
pdata = &genetic_code[gc];
aa = pdata->code[num];
if (aa != 21) {
	aa = pdata->codon_init[num];
	/* if not listed in expected stop codons */
	if (aa != 21) aa = pdata->code[num];
}
return aminoacids[aa - 1];
}


int notrail2(char *chaine, int len)
{
len--;
while(len>=0 && chaine[len]==' ') len--;
return len+1;
}


int prepch(char *chaine, char **posmot)
{
/*
chaine: template a rechercher qui contient des wildcard @
posmot: tableau de pointeurs vers char au retour rempli avec des pointeurs adequats qui pointent dans chaine qui ne doit plus etre modifiee
valeur rendue: nbre de pointeurs dans tableau posmot
*/
char *pos;
int nbrmots;
static char wildcard='@';

if(strchr(chaine,'@')==NULL) return 0;
nbrmots= -1;
pos=chaine+strlen(chaine)-1;
while( pos>=chaine && *pos==' ' ) pos--;
*(pos+1)=0;

pos=chaine;
while(*pos!=0) {
	if(*pos==wildcard) {
		posmot[++nbrmots]=NULL;
		*pos=0;
		while(*(pos+1)==wildcard) pos++;
		}
	else	{
		posmot[++nbrmots]=pos;
		while( *(pos+1)!=wildcard && *(pos+1) !=0 ) pos++;
		}
	pos++;
	}
return nbrmots+1;
}


int compch(char *cible, int lcible, char **posmot, int nbrmots)
{
/*
cible: chaine a tester pour presence du template
lcible: long. de cible qui n'est pas forcement finie par \0
	doit etre <= 150
posmot: tableau fabrique par prepch
nbrmots: valeur rendue par prepch
valeur rendue: 1 ssi template present dans cible, 0 si absent
*/
int num= 0, l, total;
char *pos;
static char vcible[151];

pos=cible+lcible-1;
while( pos>=cible && *pos==' ' ) pos--;
lcible= (int)(pos-cible+1);
memcpy(vcible,cible,lcible);
vcible[lcible]=0;
cible=vcible;
if(posmot[nbrmots-1]==NULL)
	total=nbrmots-1;
else
	total=nbrmots-2;

if(posmot[0]!=NULL) { /* comparaison avec mot initial */
	l= (int)strlen(posmot[0]);
	if(strncmp(cible,posmot[0],l)!=0) return 0;
	cible += l;
	num++;
	}
while(num<total) { /* recherche des mots internes */
	num++;
	pos=strstr(cible,posmot[num]);
	if(pos==NULL) return 0;
	l= (int)strlen(posmot[num]);
	cible = pos+l;
	num++;
	}
if( total==nbrmots-1 ) return 1; /* template se termine par @ */
/* test si cible se termine par dernier mot du template */
l= (int)strlen(posmot[nbrmots-1]);
if( strcmp(vcible+lcible-l,posmot[nbrmots-1]) == 0 ) return 1;
return 0;
}


int chg_acnuc(char *acnucvar, char *gcgacnucvar)
/*
Changing the values of variables  acnuc  and  gcgacnuc :
acnucvar	the new value of acnuc (may be a variable, may be acnuc itself)
gcgacnucvar	the new value of gcgacnuc (may be a variable, 
		may be gcgacnuc itself)
returns TRUE if error, FALSE if ok
*/
{
static char newacnuc[200], newgcgacnuc[200];
char *point;

if( strcmp(acnucvar,"acnuc") != 0 ) {
	point=getenv(acnucvar);
	if(point!=NULL) acnucvar=point;
	strcpy(newacnuc,"acnuc=");
	strcat(newacnuc,acnucvar);
	if( putenv(newacnuc) ) return 1;
	}

if( strcmp(gcgacnucvar,"gcgacnuc") != 0 ) {
	point=getenv(gcgacnucvar);
	if(point!=NULL) gcgacnucvar=point;
	strcpy(newgcgacnuc,"gcgacnuc=");
	strcat(newgcgacnuc,gcgacnucvar);
	if( putenv(newgcgacnuc) ) return 1;
	}
return 0;
}


void gets_no_echo(char *password, size_t lpw)
{
char *p, c;
#ifdef unixlike
struct termios initialrsettings, newrsettings;
int err;
#elif defined(WIN32)
DWORD mode, savemode, lu;
HANDLE hconsole;
int err;
#endif

#ifdef unixlike
err = tcgetattr( fileno(stdin), &initialrsettings );
if(err == 0) {
	newrsettings = initialrsettings;
	newrsettings.c_lflag &= ~ECHO;
	newrsettings.c_lflag &= ~ICANON;
	tcsetattr( fileno(stdin), TCSAFLUSH, &newrsettings );
	}
#elif defined(WIN32)
hconsole = GetStdHandle(STD_INPUT_HANDLE);
err = GetConsoleMode(hconsole, &savemode);
if(err != 0) {
	mode = savemode;
	mode &= ~ENABLE_ECHO_INPUT;
	mode &= ~ENABLE_LINE_INPUT;
	SetConsoleMode(hconsole, mode);
	FlushConsoleInputBuffer(hconsole); /* necessary */
	}
#endif
p = password;
do {
#ifdef WIN32
	ReadConsole(hconsole, &c, 1, &lu, NULL);
#else
	c = getchar();
#endif
	if(c == EOF || c == '\n' || c == '\r') break;
	if(c != '\b' && c != '\x7F' ) {
		*(p++) = c;
		putchar('*');
		}
	else {
		if(p > password) {
			p--;
			putchar('\b');putchar(' ');putchar('\b');
			}
		}
	}
while(p - password < lpw);
*p = 0;
#ifdef unixlike
if(err == 0) {
	tcsetattr( fileno(stdin), TCSANOW, &initialrsettings );
	putchar('\n');
	}
#elif defined(WIN32)
if(err != 0) {
	SetConsoleMode(hconsole, savemode);
	putchar('\n');
	}
#endif
return;
}


