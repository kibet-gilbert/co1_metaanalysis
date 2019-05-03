/* extracted from:
 
 PHYML :  a program that  computes maximum likelihood  phylogenies from
 DNA or AA homologous sequences
 
 Copyright (C) Stephane Guindon. Oct 2003 onward
 
 This program is free software; you can redistribute it and/or modify it under the terms of the 
 GNU General Public License as published by the Free Software Foundation; either version 2 of 
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without 
 even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with this program; if not, 
 see <http://www.gnu.org/licenses>.
 
 Linking PHYML statically or dynamically with other modules is making a combined work based on PHYML. 
 Thus, the terms and conditions of the GNU General Public License cover the whole combination.
 
 In addition, as a special exception, the copyright holders of PHYML give you permission to combine 
 PHYML with free software programs or libraries that are released under the GNU LGPL and with code 
 included in the standard release of PHYLIP under the University of Washington license (or modified 
 versions of such code, with unchanged license). You may copy and distribute such a system following 
 the terms of the GNU GPL for PHYML and the licenses of the other code concerned.
 
 Note that people who make modified versions of PHYML are not obligated to grant this special exception 
 for their modified versions; it is their choice whether to do so. The GNU General Public License gives 
 permission to release a modified version without this exception; this exception also makes it possible 
 to release a modified version which carries forward this exception.
 
 */

#ifndef PHYML_UTIL_H
#define PHYML_UTIL_H


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define VERSION "v3.0"

#define For(i,n)                     for(i=0; i<n; i++)
#define Fors(i,n,s)                  for(i=0; i<n; i+=s)
#define PointGamma(prob,alpha,beta)  PointChi2(prob,2.0*(alpha))/(2.0*(beta))
#define SHFT2(a,b,c)                 (a)=(b);(b)=(c);
#define SHFT3(a,b,c,d)               (a)=(b);(b)=(c);(c)=(d);
#define MAX(a,b)                     ((a)>(b)?(a):(b))
#define MIN(a,b)                     ((a)<(b)?(a):(b))
#define SIGN(a,b)                    ((b) > 0.0 ? fabs(a) : -fabs(a))
#define SHFT(a,b,c,d)                (a)=(b);(b)=(c);(c)=(d);

#define  NNI_MOVE 0
#define  SPR_MOVE 1


#define  NT 0 /* nucleotides */
#define  AA 1 /* amino acids */

#define  ACGT 0 /* A,G,G,T encoding */
#define  RY   1 /* R,Y     encoding */

#define INTERFACE_DATA_TYPE      0
#define INTERFACE_MULTIGENE      1
#define INTERFACE_MODEL          2
#define INTERFACE_TOPO_SEARCH    3
#define INTERFACE_BRANCH_SUPPORT 4


#define  N_MAX_OPTIONS        100


#define  T_MAX_FILE           100
#define  T_MAX_LINE       2000000
#define  T_MAX_SEQ        2000000
#define  T_MAX_OPTION         100

#define  NODE_DEG_MAX          50
#define  BRENT_ITMAX        10000
#define  BRENT_CGOLD    0.3819660
#define  BRENT_ZEPS        1.e-10
#define  MNBRAK_GOLD     1.618034
#define  MNBRAK_GLIMIT      100.0
#define  MNBRAK_TINY       1.e-20
#define  ALPHA_MIN           0.04
#define  ALPHA_MAX            100
#define  BL_MIN            1.e-06
#define  BL_START          1.e-04
#define  BL_MAX             100.0
#define  GOLDEN_R      0.61803399
#define  GOLDEN_C  (1.0-GOLDEN_R)
#define  N_MAX_INSERT          20
#define  N_MAX_OTU           4000
#define  UNLIKELY          -1.e10
#define  NJ_SEUIL             0.1
#define  ROUND_MAX            100
#define  AROUND_LK           50.0
#define  PROP_STEP            1.0
#define  T_MAX_ALPHABET       100
#define  MDBL_MIN   2.225074E-308
#define  MDBL_MAX   1.797693E+308
#define  POWELL_ITMAX         200
#define  LINMIN_TOL       2.0E-04
#define  LIM_SCALE              3
#define  LIM_SCALE_VAL     1.E-50
/* #define  LIM_SCALE           3000 */
/* #define  LIM_SCALE_VAL    1.E-500 */
#define  DEFAULT_SIZE_SPR_LIST  20
#define  OUTPUT_TREE_FORMAT  0 /* 0-->Newick; 1-->Nexus */
#define  MAX_PARS        1000000000

#define JC69       1
#define K80        2
#define F81        3
#define HKY85      4
#define F84        5
#define TN93       6
#define GTR        7
#define CUSTOM     8

#define WAG       11
#define DAYHOFF   12
#define JTT       13
#define BLOSUM62  14
#define MTREV     15
#define RTREV     16
#define CPREV     17
#define DCMUT     18
#define VT        19
#define MTMAM     20
#define MTART     21
#define HIVW      22
#define HIVB      23
#define CUSTOMAA  24

typedef	double phydbl;

/*********************************************************/

typedef struct __Node {
	struct __Node                       **v; /* table of pointers to neighbor nodes. Dimension = 2 x n_otu - 3 */
	struct __Node               ***bip_node; /* three lists of pointer to tip nodes. One list for each direction */
	struct __Edge                       **b; /* table of pointers to neighbor branches */
	struct __Node ***list_of_reachable_tips; /* list of tip nodes that can be reached in each direction from that node */
	
	int                *n_of_reachable_tips; /* sizes of the list_of_reachable_tips (in each direction) */
	int                           *bip_size; /* Size of each of the three lists from bip_node */
	int                                 num; /* node number */
	int                                 tax; /* tax = 1 -> external node, else -> internal node */
	int                        check_branch; /* check_branch=1 is the corresponding branch is labelled with '*' */
	char                        ***bip_name; /* three lists of tip node names. One list for each direction */
	char                              *name; /* taxon name (if exists) */
	
	phydbl                           *score; /* score used in BIONJ to determine the best pair of nodes to agglomerate */
	phydbl                               *l; /* lengths of the (three or one) branche(s) connected this node */
	phydbl                     dist_to_root;
}node;


/*********************************************************/

typedef struct __Edge {
	/*
	 syntax :  (node) [edge]
	 (left_1) .                   .(right_1)
	 \ (left)  (right) /
	 \._____________./
	 /    [b_fcus]   \
	 /                 \
	 (left_2) .                   .(right_2)
	 
	 */
	
	struct __Node               *left,*rght; /* node on the left/right side of the edge */
	int         l_r,r_l,l_v1,l_v2,r_v1,r_v2;
	/* these are directions (i.e., 0, 1 or 2): */
	/* l_r (left to right) -> left[b_fcus->l_r] = right */
	/* r_l (right to left) -> right[b_fcus->r_l] = left */
	/* l_v1 (left node to first node != from right) -> left[b_fcus->l_v1] = left_1 */
	/* l_v2 (left node to secnd node != from right) -> left[b_fcus->l_v2] = left_2 */
	/* r_v1 (right node to first node != from left) -> right[b_fcus->r_v1] = right_1 */
	/* r_v2 (right node to secnd node != from left) -> right[b_fcus->r_v2] = right_2 */
	
	struct __NNI                       *nni;
	
	
	int                                 num; /* branch number */
	phydbl                                l; /* branch length */
	phydbl                           best_l; /* best branch length found so far */
	phydbl                            l_old; /* old branch length */
	
	int                           bip_score; /* score of the bipartition generated by the corresponding edge
	 bip_score = 1 iif the branch is fond in both trees to be compared,
	 bip_score = 0 otherwise. */
	
	phydbl        ***p_lk_left,***p_lk_rght; /* likelihoods of the subtree on the left and
	 right side (for each site and each relative rate category) */
	short int     **p_lk_tip_r,**p_lk_tip_l; 
	
	double                        ***Pij_rr; /* matrix of change probabilities and its first and secnd derivates */
	int                     *pars_l,*pars_r; /* parsimony of the subtree on the left and right sides (for each site) */
	unsigned int               *ui_l, *ui_r; /* union - intersection vectors used in Fitch's parsimony algorithm */
	int              **p_pars_l, **p_pars_r; /* conditional parsimony vectors */
	
	int                         num_st_left; /* number of the subtree on the left side */
	int                         num_st_rght; /* number of the subtree on the right side */
	
	
	/* Below are the likelihood scaling factors (used in functions
     `Get_All_Partial_Lk_Scale' in lk.c */
	int                          scale_left;
	int                          scale_rght;
	phydbl                *sum_scale_f_left;
	phydbl                *sum_scale_f_rght;
	
	phydbl                          bootval; /* bootstrap value (if exists) */
	
	short int                      is_alive; /* is_alive = 1 if this edge is used in a tree */
	
	phydbl                   dist_btw_edges;
	int                 topo_dist_btw_edges;
	
	int                     has_zero_br_len;
	
	phydbl                       ratio_test; /* approximate likelihood ratio test */
	phydbl                   alrt_statistic; /* aLRT statistic */
	int          num_tax_left, num_tax_rght; /* number of taxa in subtrees          */
	phydbl     avg_dist_left, avg_dist_rght; /* average taxon distance in subtrees  */
	
	int                       is_p_lk_l_u2d; /* is the conditional likelihood vector on the left up
	 to data ? */
	int                       is_p_lk_r_u2d; /* is the conditional likelihood vector on the right up
	 to data ? */
	
}edge;

/*********************************************************/

typedef struct __Arbre {
	struct __Node                         *root; /* root node */
	struct __Node                       **noeud; /* array of nodes that defines the tree topology */
	struct __Edge                     **t_edges; /* array of edges */
	struct __Arbre                    *old_tree; /* old copy of the tree */
	struct __Arbre                   *best_tree; /* best tree found so far */
	struct __Model                         *mod; /* substitution model */
	struct __AllSeq                       *data; /* sequences */
	struct __Option                      *input; /* input parameters */
	struct __Matrix                        *mat; /* pairwise distance matrix */
	struct __Edge                **t_dead_edges;
	struct __Node                **t_dead_nodes;
	struct __Node                   **curr_path; /* list of nodes that form a path in the tree */
	
	
	int                         depth_curr_path; /* depth of the node path defined by curr_path */
	int                                 has_bip; /*if has_bip=1, then the structure to compare
	 tree topologies is allocated, has_bip=0 otherwise */
	int                                   n_otu; /* number of taxa */
	int                               curr_site; /* current site of the alignment to be processed */
	int                               curr_catg; /* current class of the discrete gamma rate distribution */
	int                                  n_swap; /* number of NNIs performed */
	int                               n_pattern; /* number of distinct site patterns */
	int                      has_branch_lengths; /* =1 iff input tree displays branch lengths */
	int                          print_boot_val; /* if print_boot_val=1, the bootstrap values are printed */
	int                          print_alrt_val; /* if print_boot_val=1, the bootstrap values are printed */
	int                              both_sides; /* both_sides=1 -> a pre-order and a post-order tree
	 traversals are required to compute the likelihood
	 of every subtree in the phylogeny*/
	int               num_curr_branch_available; /*gives the number of the next cell in t_edges that is free to receive a pointer to a branch */
	int                            n_dead_edges;
	int                            n_dead_nodes;
	int                                 **t_dir;
	int                          n_improvements;
	int                                 n_moves;
	
	int                                      dp; /* Data partition */
	int                               s_mod_num; /* Substitution model number */
	int                      number_of_lk_calls;
	int               number_of_branch_lk_calls;
	
	phydbl                              init_lnL;
	phydbl                              best_lnL; /* highest value of the loglikelihood found so far */
	phydbl                                 c_lnL; /* loglikelihood */
	phydbl                         *c_lnL_sorted; /* used to compute c_lnL by adding sorted terms to minimize CPU errors */
	phydbl                              *site_lk; /* vector of likelihoods at individual sites */
	phydbl                     **log_site_lk_cat; /* loglikelihood at individual sites and for each class of rate*/
	phydbl                       unconstraint_lk; /* unconstrained (or multinomial) likelihood  */
	phydbl             prop_of_sites_to_consider;
	phydbl                       lnL_lower_bound;
	phydbl                        **log_lks_aLRT; /* used to compute several branch supports */
	
	int                              *site_pars;
	int                                  c_pars;
	int                               *step_mat;
	
	struct __SPR                     **spr_list;
	struct __SPR                      *best_spr;
	int                           size_spr_list;
	int                  perform_spr_right_away;
	
	phydbl                         dnorm_thresh;
	
	
	time_t                                t_beg;
	time_t                            t_current;
	
	struct __Triplet            *triplet_struct;
	
	int                        branch_and_bound;
}arbre;

/*********************************************************/

typedef struct __Super_Arbre {
	struct __Arbre                           *tree;
	struct __List_Arbre                  *treelist; /* list of trees. One tree for each data set to be processed */
	struct __AllSeq              *data_of_interest;
	struct __Option                   **optionlist; /* list of pointers to input structures (used in supertrees) */
	
	struct __Node           ***match_st_node_in_gt;
	/*  match_st_in_gt_node[subdataset number][supertree node number]
	 *  gives the node in tree estimated from 'subdataset number' that corresponds
	 *  to 'supertree node number' in the supertree
	 */
	
	struct __Node           *****map_st_node_in_gt;
	/*  mat_st_gt_node[gt_num][st_node_num][direction] gives the
	 *  node in gt gt_num that maps node st_node_num in st.
	 */
	
	struct __Edge             ***map_st_edge_in_gt;
	/*  map_st_gt_br[gt_num][st_branch_num] gives the
	 *  branch in gt gt_num that maps branch st_branch_num
	 *  in st.
	 */
	
	struct __Edge            ****map_gt_edge_in_st;
	/*  mat_gt_st_br[gt_num][gt_branch_num][] is the list of
	 *  branches in st that map branch gt_branch_num
	 *  in gt gt_num.
	 */
	
	int                   **size_map_gt_edge_in_st;
	/*  size_map_gt_st_br[gt_num][gt_branch_num] gives the
	 *  size of the list map_gt_st_br[gt_num][gt_branch_num][]
	 */
	
	
	struct __Edge             ***match_st_edge_in_gt;
	/* match_st_edge_in_gt[gt_num][st_branch_num] gives the
	 * branch in gt gt_num that matches branch st_branch_num
	 */
	
	struct __Edge             ***match_gt_edge_in_st;
	/* match_gt_edge_in_st[gt_num][gt_branch_num] gives the
	 * branch in st that matches branch gt_branch_num
	 */
	
	struct __Node                  ****closest_match;
	/* closest_match[gt_num][st_node_num][dir] gives the 
	 * closest node in st that matches a node in gt gt_num
	 */
	
	int                              ***closest_dist;
	/* closest_dist[gt_num][st_node_num][dir] gives the
	 * number of edges to traverse to get to node
	 * closest_match[gt_num][st_node_num][dir]
	 */
	
	int                                         n_gt;
	/* number of trees */
	
	phydbl                                      **bl;
	/* bl[gt_num][gt_branch_num] gives the length of
	 * branch gt_branch_num
	 */
	
	phydbl                                  **bl_cpy;
	/* copy of bl */
	
	phydbl                                     **bl0;
	/* bl estimated during NNI (original topo) 
	 * See Mg_NNI.
	 */
	
	phydbl                                     **bl1;
	/* bl estimated during NNI (topo conf 1) 
	 * See Mg_NNI.
	 */
	
	phydbl                                     **bl2;
	/* bl estimated during NNI (topo conf 2) 
	 * See Mg_NNI.
	 */
	
	int                                *bl_partition;
	/* partition[gt_num] gives the edge partition number 
	 * gt_num belongs to.
	 */
	int                               n_bl_partition;
	
	
	
	struct __Model                          **s_mod; /* substitution model */
	
	int                                     n_s_mod;
	int                                 lock_br_len;
	
}superarbre;

/*********************************************************/

typedef struct __List_Arbre { /* a list of trees */
	struct __Arbre   **tree;
	int           list_size;                /* number of trees in the list */
}arbrelist;

/*********************************************************/

typedef struct __Seq {
	char           *name; /* sequence name */
	int              len; /* sequence length */
	char          *state; /* sequence itself */
	short int *is_ambigu; /* is_ambigu[site] = 1 if state[site] is an ambiguous character. 0 otherwise */
}seq;

/*********************************************************/


typedef struct __AllSeq {
	struct __Seq **c_seq;             /* compressed sequences      */
	phydbl        *b_frq;             /* observed state frequencies */
	short int     *invar;             /* 1 -> states are identical, 0 states vary */
	int            *wght;             /* # of each site in c_seq */
	short int    *ambigu;             /* ambigu[i]=1 is one or more of the sequences at site
	 i display an ambiguous character */
	phydbl    obs_pinvar;
	int            n_otu;             /* number of taxa */
	int        clean_len;             /* uncrunched sequences lenghts without gaps */
	int       crunch_len;             /* crunched sequences lengths */
	int         init_len;             /* length of the uncompressed sequences */
	int        *sitepatt;             /* this array maps the position of the patterns in the
	 compressed alignment to the positions in the uncompressed
	 one */
}allseq;

/*********************************************************/

typedef struct __Matrix { /* mostly used in BIONJ */
	phydbl    **P,**Q,**dist; /* observed proportions of transition, transverion and  distances
	 between pairs of  sequences */
	
	arbre              *tree; /* tree... */
	int              *on_off; /* on_off[i]=1 if column/line i corresponds to a node that has not
	 been agglomerated yet */
	int                n_otu; /* number of taxa */
	char              **name; /* sequence names */
	int                    r; /* number of nodes that have not been agglomerated yet */
	struct __Node **tip_node; /* array of pointer to the leaves of the tree */
	int             curr_int; /* used in the NJ/BIONJ algorithms */
	int               method; /* if method=1->NJ method is used, BIONJ otherwise */
}matrix;

/*********************************************************/

typedef struct __Model {
	int      whichmodel;
	/*
	 1 => JC69
	 2 => K80
	 3 => F81
	 4 => HKY85
	 5 => F84
	 6 => TN93
	 7 => GTR
	 11 => Dayhoff
	 12 => JTT
	 13 => MtREV
	 */
	char     *modelname;
	int              ns; /* number of states (4 for ADN, 20 for AA) */
	phydbl          *pi; /* states frequencies */
	int        datatype; /* 0->DNA, 1->AA */
	
	/* ADN parameters */
	phydbl        kappa; /* transition/transversion rate */
	phydbl       lambda; /* parameter used to define the ts/tv ratios in the F84 and TN93 models */
	phydbl        alpha; /* gamma shapa parameter */
	phydbl     *r_proba; /* probabilities of the substitution rates defined by the discrete gamma distribution */
	phydbl          *rr; /* substitution rates defined by the discrete gamma distribution */
	int    asked_n_catg; /* number of categories in the discrete gamma distribution */
	int          n_catg; /* number of categories in the discrete gamma distribution */
	phydbl       pinvar; /* proportion of invariable sites */
	int           invar; /* =1 iff the substitution model takes into account invariable sites */
	
	/* Below are 'old' values of some substitution parameters (see the comments above) */
	phydbl    alpha_old;
	phydbl    kappa_old;
	phydbl   lambda_old;
	phydbl   pinvar_old;
	
	char  *custom_mod_string; /* string of characters used to define custom
	 models of substitution */
	phydbl       **rr_param; /* table of pointers to relative rate parameters of the GTR or custom model */
	phydbl *rr_param_values; /* relative rate parameters of the GTR or custom model */
	int       *rr_param_num; 
	int      *n_rr_param_per_cat; /* [3][1][2] for the previous example */
	int          n_diff_rr_param; /* number of different relative substitution rates in the custom model */
	
	int    update_eigen; /* update_eigen=1-> eigen values/vectors need to be updated */
	
	double    ***Pij_rr; /* matrix of change probabilities */
	
	
	int         seq_len; /* sequence length */
	
	/* AA parameters */
	/* see PMat_Empirical in models.c for AA algorithm explanation */
	double    *mat_Q; /* 20x20 amino-acids substitution rates matrix */
	double   *mat_Vr; /* 20x20 right eigenvectors of mat_Q */
	double   *mat_Vi; /* 20x20 inverse matrix of mat_Vr */
	double   *vct_ev; /* eigen values */
	double     *expt;
	double *vct_eDmr; /* diagonal terms of a 20x20 diagonal matrix */
	double    *uexpt;
	
	
	phydbl        mr; /* mean rate = branch length/time interval */
	/* mr = -sum(i)(vct_pi[i].mat_Q[ii]) */
	
	/* term n = exp(nth eigenvalue of mat_Q / mr) */
	int     stepsize; /* stepsize=1 for nucleotide models, 3 for codon models */
	int        n_otu; /* number of taxa */
	struct __Optimiz *s_opt; /* pointer to parameters to optimize */
	int    bootstrap; /* bootstrap values are computed if bootstrap > 0.
	 The value give the number of replicates */
	phydbl      *user_b_freq; /* user-defined nucleotide frequencies */
	
	
}model;

arbre *Make_Tree_From_Scratch(int n_otu, allseq *data);
void   Bionj(matrix *mat);
char *Write_Tree(arbre *tree);
void Free_Tree(arbre *tree);
void Free_Mat(matrix *mat);
matrix *JC69_Dist(allseq *data, model *mod);
matrix *Make_Mat(int n_otu);
void Init_Mat(matrix *mat, allseq *data);
void *mCalloc(int nb, size_t size);
int Is_Ambigu(char *state, int datatype, int stepsize);

#endif
