#include <stdlib.h>
#include <string.h>

typedef struct ThisReponse {
  
  char **arg; /* je stocke les arguments */
  int nbarguments; /* je stocke le nombre d'arguments */
  
} Reponse ;

extern Reponse * initreponse(void) ;
extern void clear_reponse(Reponse * rep) ;
void ajout_reponse(Reponse *rep, char *pile, int len) ;
extern void parse(char *chaine, Reponse *rep) ;
extern char *val(Reponse *Mono, char *argument) ;
