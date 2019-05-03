#include "tbe.h"
#include <assert.h>

extern const char *preptree(TBE_Tree *fd_nj_plot);
extern char *ecrit_arbre_parenth_unrooted(TBE_Tree *fd_nj_plot, TBE_Node *root);

char* TBE_Compute(char *reftree,char* boottrees){
  TBE_Branch *boot_branches;
  TBE_Branch *ref_branches;

  // Parsing reference tree
  TBE_Tree *fd_reftree = (TBE_Tree *)calloc(1, sizeof(TBE_Tree));
  fd_reftree->trees = reftree;
  fd_reftree->current_tree = reftree;
  const char *mess = preptree(fd_reftree);
  //TBE_PrintTree(fd_reftree);

  // Parsing bootstrap trees
  TBE_Tree *fd_boot = (TBE_Tree *)calloc(1, sizeof(TBE_Tree));
  fd_boot->trees = boottrees;
  fd_boot->current_tree = boottrees;
  fd_boot->rank = 0;

  ref_branches = TBE_Assign_Branch_Ids(fd_reftree);
  
  // For each bootstrap tree
  char *p = fd_boot->trees;
  int nboot = 0;
  int i;
  TBE_Branch *cur_edge;
  short unsigned** i_matrix;
  short unsigned** c_matrix;
  short unsigned** hamming;
  short unsigned* min_dist;
  short unsigned* min_dist_edge;
  int* cluster_sizes;
  char*  newtree;

  while (p) {
    // We compare the reference tree with the current bootstrap tree
    char *oldtrees = fd_boot->trees;
    fd_boot->trees = NULL;
    char *oldname = fd_boot->tree_name;
    fd_boot->tree_name = NULL;
    free_tree(fd_boot);
    fd_boot->trees = oldtrees;
    fd_boot->tree_name = oldname;
    mess = preptree(fd_boot);
    //TBE_PrintTree(fd_boot);

    if (mess) {
      fl_alert("%s", mess);
      return NULL;
    }
    
    boot_branches = TBE_Assign_Branch_Ids(fd_boot);
    
    Alloc_TBE_Matrices(fd_reftree->notu, &i_matrix, &c_matrix, &hamming, &min_dist, &min_dist_edge, &cluster_sizes);
    
    Update_All_IC_Ref_Tree(fd_reftree, fd_boot,
			   i_matrix, c_matrix,
			   cluster_sizes,
			   ref_branches, boot_branches);
    Update_All_IC_Boot_Tree(fd_reftree, fd_boot,
			    i_matrix, c_matrix,
			    hamming, min_dist, min_dist_edge,
			    cluster_sizes,
			    ref_branches, boot_branches);
    
    for(i=0; i<2*fd_reftree->notu-2; i++){
      cur_edge = &(ref_branches[i]);
      cur_edge->score+=min_dist[cur_edge->id];
      cur_edge->p=cluster_sizes[cur_edge->id];
      if(cur_edge->p>(fd_reftree->notu/2)){
	cur_edge->p = fd_reftree->notu - cur_edge->p;
      }
    }
    Free_TBE_Matrices(fd_reftree->notu, &i_matrix, &c_matrix, &hamming, &min_dist, &min_dist_edge, &cluster_sizes);
    free(boot_branches);
    nboot++;
    p = get_next_tree(fd_boot);
  }
  free(ref_branches);
  newtree = (char *)malloc(500000);
  TBE_PrintTree(newtree,fd_reftree,ref_branches, nboot);
  free(reftree);
  return newtree;
}


//////

/* Matrices used in transfer bootstrap computation (tbe.c) */
void Alloc_TBE_Matrices(int n_otu, short unsigned*** i_matrix,
			short unsigned*** c_matrix, short unsigned*** hamming,
			short unsigned** min_dist, short unsigned**  min_dist_edge,
			int** cluster_sizes){
  int i;
  int nb_edges = 2*n_otu-2;
  (*min_dist) = (short unsigned*) malloc(nb_edges*sizeof(short unsigned)); /* array of min Hamming distances */
  (*min_dist_edge) = (short unsigned*) malloc(nb_edges*sizeof(short unsigned)); /* array of edge ids corresponding to min Hamming distances */
  (*cluster_sizes) = (int*) malloc(nb_edges*sizeof(int)); /* array of sizes of clusters associated to each branch (in the post order traversal) */
  (*c_matrix) = (short unsigned**) malloc(nb_edges*sizeof(short unsigned*)); /* matrix of cardinals of complements */
  (*i_matrix) = (short unsigned**) malloc(nb_edges*sizeof(short unsigned*)); /* matrix of cardinals of intersections */
  (*hamming) = (short unsigned**) malloc(nb_edges*sizeof(short unsigned*)); /* matrix of Hamming distances */
  for (i=0; i<nb_edges; i++){
    (*c_matrix)[i] = (short unsigned*) malloc(nb_edges*sizeof(short unsigned));
    (*i_matrix)[i] = (short unsigned*) malloc(nb_edges*sizeof(short unsigned));
    (*hamming)[i] = (short unsigned*) malloc(nb_edges*sizeof(short unsigned));
    (*min_dist)[i] = n_otu; /* initialization to the nb of taxa */
    (*cluster_sizes)[i] = 0;
  }
}

void Free_TBE_Matrices(int n_otu,  short unsigned*** i_matrix, short unsigned*** c_matrix,
		       short unsigned*** hamming, short unsigned** min_dist,
		       short unsigned**  min_dist_edge, int** cluster_sizes){
  int i;
  int nb_edges = 2*n_otu-2;
  for (i=0; i<nb_edges; i++) {
    free((*c_matrix)[i]);
    free((*i_matrix)[i]);
    free((*hamming)[i]);
  }
  free((*c_matrix));
  free((*i_matrix));
  free((*hamming));
  free((*min_dist));
  free((*min_dist_edge));
  free((*cluster_sizes));
}

///// Distance computation


/* UNION AND INTERSECT CALCULATIONS (FOR THE TRANSFER METHOD) */
void Update_IC_Ref_Tree(TBE_Tree *ref_tree, TBE_Node * orig, TBE_Node* target, TBE_Branch *my_br, TBE_Tree *boot_tree,
			short unsigned** i_matrix, short unsigned** c_matrix, int* cluster_sizes,
			TBE_Branch *ref_branches,
			TBE_Branch *boot_branches){
  /* this function does the post-order traversal (recursive from the pseudoroot to the leaves, updating knowledge for the subtrees)
     of the reference tree, examining only leaves (terminal edges) of the bootstrap tree.
     It sends a probe from the orig node to the target node (nodes in ref_tree), calculating I_ij and C_ij
     (see Brehelin, Gascuel, Martin 2008). */
  int j;
  int edge_id; /* its id */
  int next_edge_id;
  TBE_Node *tip;
  int boot_edge_id;
  edge_id = my_br->id; /* all this is in ref_tree */

  // A tip
  if(TBE_Is_Taxon(target)) {
    cluster_sizes[edge_id] = 1;
    for (j=0; j < 2*boot_tree->notu-2; j++) { /* for all the terminal edges of boot_tree */
      boot_edge_id = boot_branches[j].id;
      // If not tip, continue
      tip = NULL;
      if(TBE_Is_Taxon(boot_branches[j].bouta))
	tip = boot_branches[j].bouta;
      if(TBE_Is_Taxon(boot_branches[j].boutb))
	tip = boot_branches[j].boutb;
      if(tip == NULL) continue;
      /* we only want to scan terminal edges of boot_tree, where the right son is a leaf */
      /* else we update all the I_ij and C_ij with i = edge_id */
      if (strcmp(ref_tree->labels[target->rank],boot_tree->labels[tip->rank])) {
	/* here the taxa are different */
	i_matrix[edge_id][boot_edge_id] = 0;
	c_matrix[edge_id][boot_edge_id] = 1;
      } else {
	/* same taxa here in T_ref and T_boot */
	i_matrix[edge_id][boot_edge_id] = 1;
	c_matrix[edge_id][boot_edge_id] = 0;
      }
    } /* end for on all edges of T_boot, for my_br being terminal */
  } else {
    cluster_sizes[edge_id] = 0;
    /* now the case where my_br is not a terminal edge */
    /* first initialise (zero) the cells we are going to update */
    for (j=0; j < 2*boot_tree->notu-2; j++){
      /* We initialize the i and c matrices for the edge edge_id with :
       * 0 for i : because afterwards we do i[edge_id] = i[edge_id] || i[next_edge_id]
       * 1 for c : because afterwards we do c[edge_id] = c[edge_id] && c[next_edge_id]
       */
      if(TBE_Is_Taxon(boot_branches[j].bouta) || TBE_Is_Taxon(boot_branches[j].boutb)){
	boot_edge_id = boot_branches[j].id;
 	i_matrix[edge_id][boot_edge_id] = 0;
	c_matrix[edge_id][boot_edge_id] = 1;
      }
    }
    if(target->v1!=orig){
      Update_IC_Ref_Tree(ref_tree, target, target->v1, &(ref_branches[target->br_id1]), boot_tree, i_matrix, c_matrix, cluster_sizes, ref_branches, boot_branches);
      next_edge_id = target->br_id1;
      cluster_sizes[edge_id] += cluster_sizes[next_edge_id];
      for (j=0; j < 2*boot_tree->notu-2; j++) { /* for all the terminal edges of boot_tree */
	boot_edge_id = boot_branches[j].id;
	// If not a tip, continue
	if(!TBE_Is_Taxon(boot_branches[j].bouta) && !TBE_Is_Taxon(boot_branches[j].boutb)) continue;
	i_matrix[edge_id][boot_edge_id] = i_matrix[edge_id][boot_edge_id] || i_matrix[next_edge_id][boot_edge_id];
	/* above is an OR between two integers, result is 0 or 1 */
	c_matrix[edge_id][boot_edge_id] = c_matrix[edge_id][boot_edge_id] && c_matrix[next_edge_id][boot_edge_id];
	/* above is an AND between two integers, result is 0 or 1 */
      } /* end for j */
    }
    if(target->v2!=orig){
      Update_IC_Ref_Tree(ref_tree, target, target->v2, &(ref_branches[target->br_id2]), boot_tree, i_matrix, c_matrix, cluster_sizes, ref_branches, boot_branches);
      next_edge_id = target->br_id2;
      cluster_sizes[edge_id] += cluster_sizes[next_edge_id];
      for (j=0; j < 2*boot_tree->notu-2; j++) { /* for all the terminal edges of boot_tree */
	boot_edge_id = boot_branches[j].id;
	// If not a tip, continue
	if(!TBE_Is_Taxon(boot_branches[j].bouta) && !TBE_Is_Taxon(boot_branches[j].boutb)) continue;
	i_matrix[edge_id][boot_edge_id] = i_matrix[edge_id][boot_edge_id] || i_matrix[next_edge_id][boot_edge_id];
	/* above is an OR between two integers, result is 0 or 1 */
	c_matrix[edge_id][boot_edge_id] = c_matrix[edge_id][boot_edge_id] && c_matrix[next_edge_id][boot_edge_id];
	/* above is an AND between two integers, result is 0 or 1 */
      } /* end for j */
    }
    
    if(target->v3!=orig){
      Update_IC_Ref_Tree(ref_tree, target, target->v3, &(ref_branches[target->br_id3]), boot_tree, i_matrix, c_matrix, cluster_sizes, ref_branches, boot_branches);
      next_edge_id = target->br_id3;
      cluster_sizes[edge_id] += cluster_sizes[next_edge_id];
      for (j=0; j < 2*boot_tree->notu-2; j++) { /* for all the terminal edges of boot_tree */
	boot_edge_id = boot_branches[j].id;
	// If not a tip, continue
	if(!TBE_Is_Taxon(boot_branches[j].bouta) && !TBE_Is_Taxon(boot_branches[j].boutb)) continue;
	i_matrix[edge_id][boot_edge_id] = i_matrix[edge_id][boot_edge_id] || i_matrix[next_edge_id][boot_edge_id];
	/* above is an OR between two integers, result is 0 or 1 */
	c_matrix[edge_id][boot_edge_id] = c_matrix[edge_id][boot_edge_id] && c_matrix[next_edge_id][boot_edge_id];
	/* above is an AND between two integers, result is 0 or 1 */
      } /* end for j */
    }
  } /* ending the case where my_br is an internal edge */
} /* end update_i_c_post_order_ref_tree */

void Update_All_IC_Ref_Tree(TBE_Tree* ref_tree, TBE_Tree* boot_tree,
			    short unsigned** i_matrix, short unsigned** c_matrix, int* cluster_sizes,
			    TBE_Branch *ref_branches,
			    TBE_Branch *boot_branches){
  /* this function is the first step of the union and intersection calculations */
  TBE_Node *root;
  
  root=ref_tree->racine;
  Update_IC_Ref_Tree(ref_tree, root, root->v1, &(ref_branches[root->br_id1]), boot_tree, i_matrix, c_matrix, cluster_sizes, ref_branches, boot_branches);
  Update_IC_Ref_Tree(ref_tree, root, root->v2, &(ref_branches[root->br_id2]), boot_tree, i_matrix, c_matrix, cluster_sizes, ref_branches, boot_branches);
} /* end update_all_i_c_post_order_ref_tree */

void Update_IC_Boot_Tree(TBE_Tree* ref_tree, TBE_Tree* boot_tree, TBE_Node* orig, TBE_Node* target,
			 TBE_Branch *my_br, short unsigned** i_matrix, short unsigned** c_matrix,
			 short unsigned** hamming, short unsigned* min_dist,
			 short unsigned* min_dist_edge, int* cluster_sizes,
			 TBE_Branch *ref_branches,
			 TBE_Branch *boot_branches){
  /* here we implement the second part of the Brehelin/Gascuel/Martin algorithm:
     post-order traversal of the bootstrap tree, and numerical recurrence. 
     in this function, orig and target are nodes of boot_tree (aka T_boot).
     min_dist is an array whose size is equal to the number of edges in T_ref.
     It gives for each edge of T_ref its min distance to a split in T_boot. */
  int i;
  int boot_edge_id /* its id */, next_boot_edge_id /* id of descending branches. */;
  int N = ref_tree->notu;
  /* we first have to determine which is the direction of the edge (orig -> target and target -> orig) */
  boot_edge_id = my_br->id; /* here this is an edge_id corresponding to T_boot */

  // Not a taxon
  if(!TBE_Is_Taxon(target)){
    /* because nothing to do in the case where target is a leaf: intersection and union already ok. */
    /* otherwise, keep on posttraversing in all other directions */
    
    /* first initialise (zero) the cells we are going to update */
    for (i=0; i < 2*ref_tree->notu-2; i++) i_matrix[i][boot_edge_id] = c_matrix[i][boot_edge_id] = 0;
    
    if(target->v1 != orig){
      next_boot_edge_id = target->br_id1;
      Update_IC_Boot_Tree(ref_tree, boot_tree, target, target->v1, &(boot_branches[next_boot_edge_id]),
			  i_matrix, c_matrix, hamming, min_dist, min_dist_edge, cluster_sizes, ref_branches, boot_branches);
      for (i=0; i < 2*ref_tree->notu-2; i++) { /* for all the edges of ref_tree */ 
	i_matrix[i][boot_edge_id] += i_matrix[i][next_boot_edge_id];
	c_matrix[i][boot_edge_id] += c_matrix[i][next_boot_edge_id];
      } /* end for i */
    }
    if(target->v2 != orig){
      next_boot_edge_id = target->br_id2;
      Update_IC_Boot_Tree(ref_tree, boot_tree, target, target->v2, &(boot_branches[next_boot_edge_id]),
			  i_matrix, c_matrix, hamming, min_dist, min_dist_edge, cluster_sizes, ref_branches, boot_branches);
      for (i=0; i < 2*ref_tree->notu-2; i++) { /* for all the edges of ref_tree */ 
	i_matrix[i][boot_edge_id] += i_matrix[i][next_boot_edge_id];
	c_matrix[i][boot_edge_id] += c_matrix[i][next_boot_edge_id];
      } /* end for i */
    }
    if(target->v3 != orig){
      next_boot_edge_id = target->br_id3;
      Update_IC_Boot_Tree(ref_tree, boot_tree, target, target->v3, &(boot_branches[next_boot_edge_id]),
			  i_matrix, c_matrix, hamming, min_dist, min_dist_edge, cluster_sizes, ref_branches, boot_branches);
      for (i=0; i < 2*ref_tree->notu-2; i++) { /* for all the edges of ref_tree */ 
	i_matrix[i][boot_edge_id] += i_matrix[i][next_boot_edge_id];
	c_matrix[i][boot_edge_id] += c_matrix[i][next_boot_edge_id];
      } /* end for i */
    }
  } /* end if target is not a leaf: the following loop is performed in all cases */

  for (i=0; i< 2*ref_tree->notu-2; i++) { /* for all the edges of ref_tree */ 
    /* at this point we can calculate in all cases (internal branch or not) the Hamming distance at [i][boot_edge_id], */
    /* card of union minus card of intersection */
    hamming[i][boot_edge_id] =
      cluster_sizes[i] /* #taxa in the cluster i of T_ref */
      + c_matrix[i][boot_edge_id] /* #taxa in cluster edge_id of T_boot BUT NOT in cluster i of T_ref */
      - i_matrix[i][boot_edge_id]; /* #taxa in the intersection of the two clusters */
    
    /* Let's immediately calculate the right ditance, taking into account the fact that the true disance is min (dist, N-dist) */
    if (hamming[i][boot_edge_id] > N/2 /* floor value */) hamming[i][boot_edge_id] = N - hamming[i][boot_edge_id];
    /*   and update the min of all Hamming (TRANSFER) distances hamming[i][j] over all j */
    if (hamming[i][boot_edge_id] < min_dist[i]){
      min_dist[i] = hamming[i][boot_edge_id];
      min_dist_edge[i] = boot_edge_id;
    }
  } /* end for on all edges of T_ref */
} /* end update_i_c_post_order_boot_tree */


void Update_All_IC_Boot_Tree(TBE_Tree* ref_tree, TBE_Tree* boot_tree,
			     short unsigned** i_matrix, short unsigned** c_matrix,
			     short unsigned** hamming, short unsigned* min_dist,
			     short unsigned* min_dist_edge, int* cluster_sizes,
			     TBE_Branch *ref_branches,
			     TBE_Branch *boot_branches){
  /* this function is the second step of the union and intersection calculations */
  int i;
  TBE_Node *root;
  root=boot_tree->racine;
  Update_IC_Boot_Tree(ref_tree, boot_tree, root, root->v1, &(boot_branches[root->br_id1]), i_matrix, c_matrix, hamming, min_dist, min_dist_edge, cluster_sizes, ref_branches, boot_branches);
  Update_IC_Boot_Tree(ref_tree, boot_tree, root, root->v2, &(boot_branches[root->br_id2]), i_matrix, c_matrix, hamming, min_dist, min_dist_edge, cluster_sizes, ref_branches, boot_branches);
  
  /* and then some checks to make sure everything went ok */
  for(i=0; i<2*ref_tree->notu-2; i++) {
    assert(min_dist[ref_branches[i].id] >= 0);
    if(TBE_Is_Taxon(ref_branches[i].bouta) || TBE_Is_Taxon(ref_branches[i].boutb)){
      assert(min_dist[ref_branches[i].id] == 0); /* any terminal edge should have an exact match in any bootstrap tree */
    }
  }
} /* end update_all_i_c_post_order_boot_tree */


// Assigns identifiers to all branches of the tree
// And sets branch ids to each nodes br_id1 br_id2 and br_id3
TBE_Branch* TBE_Assign_Branch_Ids(TBE_Tree *tree){
  int id = -1;
  TBE_Branch * branches = (TBE_Branch*) malloc((2*tree->notu-2)*sizeof(TBE_Branch));
  for(int i=0;i<2*tree->notu-2;i++){
    branches[i].score = 0;
    branches[i].p = 0;
  }
  TBE_Init_Branches(tree->racine->v1, tree->racine, branches, &id);
  TBE_Init_Branches(tree->racine->v2, tree->racine, branches, &id);
  return branches;
}


void TBE_Init_Branches(TBE_Node *cur, TBE_Node *prev, TBE_Branch *branches, int *id){
  (*id)++;
  branches[*id].id = *id;
  branches[*id].bouta = prev;
  branches[*id].boutb = cur;

  if(cur ==  prev->v1){
    prev->br_id1 = *id;
  }else if(cur == prev->v2){
    prev->br_id2 = *id;
  }else if(cur == prev->v3){
    prev->br_id3 = *id;
  }

  if(prev ==  cur->v1){
    cur->br_id1 = *id;
  }else if(prev == cur->v2){
    cur->br_id2 = *id;
  }else if(prev == cur->v3){
    cur->br_id3 = *id;
  }
  
  if(cur->v1!=NULL && cur->v1!=prev){
    TBE_Init_Branches(cur->v1, cur, branches,id);
  }
  if(cur->v2!=NULL && cur->v2!=prev){
    TBE_Init_Branches(cur->v2, cur, branches,id);
  }
  if(cur->v3!=NULL && cur->v3!=prev){
    TBE_Init_Branches(cur->v3, cur, branches,id);
  }
}

int TBE_Is_Taxon(TBE_Node *n){
  int nneigh = 0;
  nneigh+=(n->v1!=NULL);
  nneigh+=(n->v2!=NULL);
  nneigh+=(n->v3!=NULL);
  return nneigh==1;
}

void TBE_PrintTree(char* nwstring, TBE_Tree *t, TBE_Branch *branches, int nboot){
  int pos=0;
  nwstring[pos++]='(';
  TBE_PrintTree_Recur(&pos,nwstring,t, t->racine->v1,t->racine, t->racine->br_id1, t->racine->l1, branches, nboot);
  nwstring[pos++]=',';
  TBE_PrintTree_Recur(&pos,nwstring,t, t->racine->v2,t->racine, t->racine->br_id2,t->racine->l2, branches, nboot);
  nwstring[pos++]=')';
  nwstring[pos++]=';';
  nwstring[pos++]='\0';
}

void TBE_PrintTree_Recur(int *pos, char *nwstring, TBE_Tree *t, TBE_Node *cur,TBE_Node *orig, int cur_edge, double length, TBE_Branch *branches, int nboot){
  if(TBE_Is_Taxon(cur)){
    (*pos)+=snprintf(nwstring+(*pos),500000-(*pos),"%s",t->labels[cur->rank]);
  }else{
    nwstring[(*pos)++]='(';
    int nb = 0;
    if(cur->v1!=orig){
      TBE_PrintTree_Recur(pos, nwstring, t, cur->v1, cur, cur->br_id1, cur->l1, branches, nboot);
      nb++;
    }
    if(cur->v2!=orig){
      if(nb>0)
	nwstring[(*pos)++]=',';
      TBE_PrintTree_Recur(pos, nwstring, t, cur->v2, cur, cur->br_id2, cur->l2, branches, nboot);
      nb++;
    }
    if(cur->v3!=orig){
      nwstring[(*pos)++]=',';
      TBE_PrintTree_Recur(pos,nwstring, t, cur->v3, cur, cur->br_id2, cur->l3, branches, nboot);
    }
    nwstring[(*pos)++]=')';

    // Compute and write support
    if(branches[cur_edge].p>1){
      double support = 100.0*(1.0 - (branches[cur_edge].score * 1.0 / nboot) / (branches[cur_edge].p-1));
      (*pos)+=snprintf(nwstring+(*pos),500000-(*pos),"%d",int(support+0.5));
    }
  }
  // Write length
  (*pos)+=snprintf(nwstring+(*pos),500000-(*pos),":%f",length);
}
