#ifndef TBE_H
#define TBE_H

#include "treedraw.h"
/* Main function for computing TBE support
   - reftree: 1 tree in newick string
   - boottrees: several trees in a single newick string
   - returns: newick string of the tree with supports
*/
char* TBE_Compute(char* reftree, char* boottrees);

typedef struct { /* une branche definie par ses deux extremites */
	struct noeud *bouta;
	struct noeud *boutb;
	char *br_label;
	int id;
	double score; // For TBE bootstrap support
	int p; // Number of taxa on the light side og the bipartition (only defined after TBE_Bootstrap)
} TBE_Branch;


typedef struct FD_nj_plot TBE_Tree;
typedef struct noeud TBE_Node;

/* Matrices used in transfer bootstrap computation (tbe.c) */
void Alloc_TBE_Matrices(int n_otu, short unsigned*** i_matrix,
			short unsigned*** c_matrix,short unsigned*** hamming,
			short unsigned** min_dist, short unsigned**  min_dist_edge,
			int** cluster_sizes);

void Free_TBE_Matrices(int n_otu,  short unsigned*** i_matrix, short unsigned*** c_matrix,
		       short unsigned*** hamming, short unsigned** min_dist,
		       short unsigned**  min_dist_edge, int** cluster_sizes);


/* Transfer distance computation */
void Update_All_IC_Ref_Tree(TBE_Tree* ref_tree, TBE_Tree* boot_tree,
			    short unsigned** i_matrix, short unsigned** c_matrix, int* cluster_sizes,
			     TBE_Branch *ref_branches,
			     TBE_Branch *boot_branches);
void Update_IC_Ref_Tree(TBE_Tree *ref_tree, TBE_Node * orig, TBE_Node* target, TBE_Branch *my_br, TBE_Tree *boot_tree,
			short unsigned** i_matrix, short unsigned** c_matrix, int* cluster_sizes,
			     TBE_Branch *ref_branches,
			     TBE_Branch *boot_branches);
void Update_IC_Boot_Tree(TBE_Tree* ref_tree, TBE_Tree* boot_tree, TBE_Node* orig, TBE_Node* target,
			 TBE_Branch *my_br, short unsigned** i_matrix, short unsigned** c_matrix,
			 short unsigned** hamming, short unsigned* min_dist,
			 short unsigned* min_dist_edge, int* cluster_sizes,
			     TBE_Branch *ref_branches,
			     TBE_Branch *boot_branches);
void Update_All_IC_Boot_Tree(TBE_Tree* ref_tree, TBE_Tree* boot_tree,
			     short unsigned** i_matrix, short unsigned** c_matrix,
			     short unsigned** hamming, short unsigned* min_dist,
			     short unsigned* min_dist_edge, int* cluster_sizes,
			     TBE_Branch *ref_branches,
			     TBE_Branch *boot_branches);


// Assigns identifiers to all TBE_branchs of the tree
// And sets branch ids to each nodes br_id1 br_id2 and br_id3
TBE_Branch* TBE_Assign_Branch_Ids(TBE_Tree *tree);
void TBE_Init_Branches(TBE_Node *cur, TBE_Node *prev, TBE_Branch *branches, int *id);
int TBE_Is_Taxon(TBE_Node *n);
void TBE_PrintTree(char *nwstring, TBE_Tree *t, TBE_Branch *branches, int nboot);
void TBE_PrintTree_Recur(int *pos, char *nwstring, TBE_Tree *t, TBE_Node *cur,TBE_Node *orig, int cur_edge, double length, TBE_Branch *branches, int nboot);
#endif
