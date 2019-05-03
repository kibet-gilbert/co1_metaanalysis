#include "parser.h"

char *unprotect_quotes(char *name);



/** initialisation d'une réponse **/
Reponse * initreponse(void) {
  Reponse *rep;
  rep = (Reponse *) malloc (sizeof(Reponse));
  rep->nbarguments = 0;
  rep->arg = NULL;
  return rep;
}


void clear_reponse(Reponse * rep) { 
  int pti;
  
  for(pti=0; pti < rep->nbarguments; pti++) {
    free(rep->arg[pti]);
  }
  if(rep->arg != NULL) free(rep->arg);
  free(rep);
}


void ajout_reponse(Reponse *rep, char *pile, int len) {
  int pti;

    if (rep->nbarguments==0)
      rep->arg = (char **)malloc(1 * sizeof(char *));
    else
      rep->arg= (char**) realloc(rep->arg,(rep->nbarguments+1)* sizeof(char *)) ; /* ici j'ai deja alloue donc realloc (plus de ligne) */
	
    rep->arg[rep->nbarguments]=(char *) malloc(len + 1); /* j'alloue l'espace pour l'argument */
    
    pti=rep->nbarguments;
    memcpy(rep->arg[pti], pile, len); /* je stocke l'argument */
    rep->arg[pti][len] = 0;
    
    rep->nbarguments++;
    
  }




/** le parseur qui rempli la structure Reponse **/
void parse(char *chaine, Reponse *rep) {
char *ori;

ori=chaine;
while(*chaine != 0) {
    if (*chaine == '"' ) {
    	do 	{ /* chercher le prochain " en ignorant les \" sauf si en fin de partie entre " " */
    		chaine++;
    		if(*chaine == 0) break;
    		}
    	while(*chaine != '"' || ( *(chaine-1) == '\\' && *(chaine+1) != '&' && *(chaine+1) != 0) );
    	if(*chaine == 0) break;
    	chaine++;
    	} 
    if(*chaine == '&') { 
      	ajout_reponse(rep, ori, chaine-ori);
      	ori=chaine+1;
    	}
    chaine++;
    }
ajout_reponse(rep, ori, chaine-ori); 
}



/** pour rechercher la valeur d'un argument dans la structure **/
char *val(Reponse *Mono, char *argument) 
{
int count, num, l, larg;
char *retval = NULL, *debut, *p;

count = Mono->nbarguments;
larg = strlen(argument);
for(num=0; num < count; num++) {
	debut = Mono->arg[num];
	p = strchr(debut, '=');
	if(p == NULL || larg != p - debut) continue;
	p++;
	if(strncmp(argument, debut, larg) != 0) continue;
	l = strlen(p);
	retval = (char *)malloc(l+1);
	strcpy(retval, p);
	unprotect_quotes(retval);
	break;
	}
return retval;
}


char *unprotect_quotes(char *name)
/*  enlever les " " encadrantes et decoder les \" internes en " */
{
char *p;
int l;

l = strlen(name);
if(*name == '"') {
	memmove(name, name+1, l--); /* enlever la " initiale */
	if( *(name+l-1) == '"') *(name + --l) = 0; /* enlever la " terminale */
	}

while ((p = strstr(name, "\\\"" /* la chaine \"  */ )) != NULL) { memmove(p, p+1, name + l - p); l--; }
return name;
}

