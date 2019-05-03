/* extracted from
 
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
#include "phyml_util.h"

extern int tree_build_interrupted;

/*********************************************************/

void Warn_And_Exit(char *s)
{
	fprintf(stdout,"%s",s);
	fflush(NULL);
#ifndef BATCH
	char c;
	fprintf(stdout,"\n. Type any key to exit.\n");
	fscanf(stdin,"%c",&c);
#endif
	exit(1);
}

/*********************************************************/

void *mCalloc(int nb, size_t size)
{
	void *allocated;
	
	if((allocated = calloc((size_t)nb,(size_t)size)) != NULL)
    {
		return allocated;
    }
	else
		Warn_And_Exit("\n. Err: low memory\n");
	
	return NULL;
}

/*********************************************************/

matrix *Make_Mat(int n_otu)
{
	matrix *mat;
	int i;
	
	mat = (matrix *)mCalloc(1,sizeof(matrix));
	
	mat->n_otu = n_otu;
	
	mat->P        = (phydbl **)mCalloc(n_otu,sizeof(phydbl *));
	mat->Q        = (phydbl **)mCalloc(n_otu,sizeof(phydbl *));
	mat->dist     = (phydbl **)mCalloc(n_otu,sizeof(phydbl *));
	mat->on_off   = (int *)mCalloc(n_otu,sizeof(int));
	mat->name     = (char **)mCalloc(n_otu,sizeof(char *));
	mat->tip_node = (node **)mCalloc(n_otu,sizeof(node *));
	
	
	For(i,n_otu)
    {
		mat->P[i]    = (phydbl *)mCalloc(n_otu,sizeof(phydbl));
		mat->Q[i]    = (phydbl *)mCalloc(n_otu,sizeof(phydbl));
		mat->dist[i] = (phydbl *)mCalloc(n_otu,sizeof(phydbl));
    }
	
	return mat;
}


/*********************************************************/

void Init_Mat(matrix *mat, allseq *data)
{
	int i;
	
	mat->n_otu = data->n_otu;
	mat->r = mat->n_otu;
	mat->curr_int = mat->n_otu;
	mat->method = 1;
	
	For(i,data->n_otu)
    {
		mat->name[i] = strdup(data->c_seq[i]->name);
		mat->on_off[i] = 1;
    }
}

/*********************************************************/

matrix *JC69_Dist(allseq *data, model *mod)
{
	int site,i,j,k;
	phydbl x;
	matrix *mat;
	phydbl **len;
	
	
	len = (phydbl **)mCalloc(data->n_otu,sizeof(phydbl *));
	For(i,data->n_otu)
    len[i] = (phydbl *)mCalloc(data->n_otu,sizeof(phydbl));
	
	
	mat = Make_Mat(data->n_otu);
	Init_Mat(mat,data);
	
	Fors(site,data->c_seq[0]->len,mod->stepsize)
    {
      if (tree_build_interrupted) {
	break;
	}
		For(j,data->n_otu-1)
		{
			for(k=j+1;k<data->n_otu;k++)
			{
				if((!Is_Ambigu(data->c_seq[j]->state+site,mod->datatype,mod->stepsize)) &&
				   (!Is_Ambigu(data->c_seq[k]->state+site,mod->datatype,mod->stepsize)))
				{
					len[j][k]+=data->wght[site];
					len[k][j]=len[j][k];
					if(strncmp(data->c_seq[j]->state+site,
							   data->c_seq[k]->state+site,
							   mod->stepsize))
						mat->P[j][k]+=data->wght[site];
				}
			}
		}
    }
	
	
	For(i,data->n_otu-1)
    for(j=i+1;j<data->n_otu;j++)
	{
		if(len[i][j])
		{
			mat->P[i][j] /= len[i][j];
		}
		else
		{
			mat->P[i][j] = 1.;
		}
		
		mat->P[j][i] = mat->P[i][j];
		
		x = 1.-(mod->ns/(mod->ns-1.))*mat->P[i][j];
		if(x <= .0)
		{
		  Free_Mat(mat);
		  mat = NULL;
		  goto out;
		}
		else
			mat->dist[i][j] = -((mod->ns-1.)/mod->ns)*(phydbl)log(x);
		
		mat->dist[j][i] = mat->dist[i][j];
	}
	
out:For(i,data->n_otu) free(len[i]);
	free(len);
	
	return mat;
}

/*********************************************************/

void Clean_Tree_Connections(arbre *tree)
{
	
	int i;
	For(i,2*tree->n_otu-2)
    {
		tree->noeud[i]->v[0] = NULL;
		tree->noeud[i]->v[1] = NULL;
		tree->noeud[i]->v[2] = NULL;
		tree->noeud[i]->b[0] = NULL;
		tree->noeud[i]->b[1] = NULL;
		tree->noeud[i]->b[2] = NULL;
    }
}

/*********************************************************/

void Make_Tree_Path(arbre *tree)
{
	tree->curr_path = (node **)mCalloc(tree->n_otu,sizeof(node *));
}

/*********************************************************/

void Init_Node_Light(node *n, int num)
{
	n->list_of_reachable_tips = NULL;
	n->num                    = num;
	n->tax                    = -1;
}

/*********************************************************/

node *Make_Node_Light(int num)
{
	node *n;
	n        = (node *)mCalloc(1,sizeof(node));
	n->v     = (node **)mCalloc(3,sizeof(node *));
	n->l     = (phydbl *)mCalloc(3,sizeof(phydbl));
	n->b     = (edge **)mCalloc(3,sizeof(edge *));
	n->score = (phydbl *)mCalloc(3,sizeof(phydbl));
	Init_Node_Light(n,num);
	return n;
}

/*********************************************************/

void Make_All_Tree_Nodes(arbre *tree)
{
	int i;
	tree->noeud          = (node **)mCalloc(2*tree->n_otu-2,sizeof(node *));
	tree->t_dead_nodes   = (node **)mCalloc(2*tree->n_otu-2,sizeof(node *));
	
	For(i,2*tree->n_otu-2)
    {
		tree->noeud[i] = (node *)Make_Node_Light(i);
		if(i < tree->n_otu) tree->noeud[i]->tax = 1;
		else                tree->noeud[i]->tax = 0;
    }
}

/*********************************************************/

void Init_Tree(arbre *tree, int n_otu)
{
	tree->n_otu                     = n_otu;
	tree->best_tree                 = NULL;
	tree->old_tree                  = NULL;
	tree->mat                       = NULL;
	
	tree->depth_curr_path           = 0;
	tree->has_bip                   = 0;
	tree->n_moves                   = 0;
	tree->n_improvements            = 0;
	tree->number_of_lk_calls        = 0;
	tree->number_of_branch_lk_calls = 0;
	
	tree->init_lnL                  = UNLIKELY;
	tree->best_lnL                  = UNLIKELY;
	tree->c_lnL                     = UNLIKELY;
	tree->lnL_lower_bound           = UNLIKELY;
	tree->n_swap                    = 0;
	
	tree->n_pattern                 = -1;
	tree->prop_of_sites_to_consider = 1.;
	
	tree->branch_and_bound          = 0;
	tree->print_boot_val            = 0;
	tree->print_alrt_val            = 0;
	tree->num_curr_branch_available = 0;
}

/*********************************************************/

arbre *Make_Tree(int n_otu)
{
	arbre *tree;
	int i;
	tree = (arbre *)mCalloc(1,sizeof(arbre ));
	Init_Tree(tree,n_otu);
	tree->t_dir = (int **)mCalloc(2*n_otu-2,sizeof(int *));
	For(i,2*n_otu-2) tree->t_dir[i] = (int *)mCalloc(2*n_otu-2,sizeof(int));
	return tree;
}

/*********************************************************/

void Copy_Tax_Names_To_Tip_Labels(arbre *tree, allseq *data)
{
	int i;
	
	For(i,tree->n_otu)
    {
		tree->noeud[i]->name = strdup(data->c_seq[i]->name);
		tree->noeud[i]->tax = 1;
		tree->noeud[i]->num = i;
    }
}

/*********************************************************/

void Make_List_Of_Reachable_Tips(arbre *tree)
{
	int i,j;
	
	For(i,2*tree->n_otu-2)
    {
		tree->noeud[i]->list_of_reachable_tips = (node ***)mCalloc(3,sizeof(node **));
		tree->noeud[i]->n_of_reachable_tips    = (int *)mCalloc(3,sizeof(int));
		For(j,3)
		tree->noeud[i]->list_of_reachable_tips[j] = (node **)mCalloc(tree->n_otu,sizeof(node *));
    }
}

/*********************************************************/

void Init_Edge_Light(edge *b, int num)
{
	b->num                  = num;
	b->bip_score            = 0;
	b->dist_btw_edges       = .0;
	b->topo_dist_btw_edges  = 0;
	b->has_zero_br_len      = 0;
	b->is_p_lk_l_u2d        = 0;
	b->is_p_lk_r_u2d        = 0;
	
	b->p_lk_left            = NULL;
	b->p_lk_rght            = NULL;
	b->Pij_rr               = NULL;
}

/*********************************************************/

void Make_Edge_Dirs(edge *b, node *a, node *d)
{
	int i;
	
	if(a == b->rght)
    {
		printf("\n. a->num = %3d ; d->num = %3d",a->num,d->num);
		printf("\n. Err in file %s at line %d\n",__FILE__,__LINE__);
		Warn_And_Exit("");
    }
	if(d == b->left)
    {
		printf("\n. a->num = %3d ; d->num = %3d",a->num,d->num);
		printf("\n. Err in file %s at line %d\n",__FILE__,__LINE__);
		Warn_And_Exit("");
    }
	
	b->l_r = b->r_l = -1;
	For(i,3)
    {
		if((a->v[i]) && (a->v[i] == d))
		{
			b->l_r  = i; /* we consider here that 'a' is on the left handside of 'b'*/
			a->b[i] = b;
		}
		if((d->v[i]) && (d->v[i] == a))
		{
			b->r_l  = i; /* we consider here that 'd' is on the right handside of 'b'*/
			d->b[i] = b;
		}
    }
	
	if(a->tax) {b->r_l = 0; For(i,3) if(d->v[i]==a) {b->l_r = i; break;}}
	
	
	b->l_v1 = b->l_v2 = b->r_v1 = b->r_v2 = -1;
	For(i,3)
    {
		if(b->left->v[i] != b->rght)
		{
			if(b->l_v1 < 0) b->l_v1 = i;
			else            b->l_v2 = i;
		}
		
		if(b->rght->v[i] != b->left)
		{
			if(b->r_v1 < 0) b->r_v1 = i;
			else            b->r_v2 = i;
		}
    }
}

/*********************************************************/

edge *Make_Edge_Light(node *a, node *d, int num)
{
	edge *b;
	
	b = (edge *)mCalloc(1,sizeof(edge));
	
	Init_Edge_Light(b,num);
	
	if(a && b)
    {
		b->left = a;  b->rght = d;
		if(a->tax) {b->rght = a; b->left = d;} /* root */
		/* a tip is necessary on the right side of the edge */
		
		(b->left == a)?
		(Make_Edge_Dirs(b,a,d)):
		(Make_Edge_Dirs(b,d,a));
		
		b->l                    = a->l[b->l_r];
		if(a->tax) b->l         = a->l[b->r_l];
		if(b->l < BL_MIN)  b->l = BL_MIN;
		else if(b->l > BL_MAX) b->l = BL_MAX;
		b->l_old                = b->l;
    }
	else
    {
		b->left = NULL;
		b->rght = NULL;
    }
	
	return b;
	
}

/*********************************************************/

void Make_All_Tree_Edges(arbre *tree)
{
	int i;
	
	tree->t_edges      = (edge **)mCalloc(2*tree->n_otu-3,sizeof(edge *));
	tree->t_dead_edges = (edge **)mCalloc(2*tree->n_otu-3,sizeof(edge *));
	
	For(i,2*tree->n_otu-3) tree->t_edges[i] = (edge *)Make_Edge_Light(NULL,NULL,i);
}

/*********************************************************/

arbre *Make_Tree_From_Scratch(int n_otu, allseq *data)
{
	arbre *tree;
	
	tree = Make_Tree(n_otu);
	Make_All_Tree_Nodes(tree);
	Make_All_Tree_Edges(tree);
	Make_Tree_Path(tree);
	Make_List_Of_Reachable_Tips(tree);
	if(data)
    {
		Copy_Tax_Names_To_Tip_Labels(tree,data);
		tree->data = data;
    }
	return tree;
}

/*********************************************************/

int Is_Ambigu(char *state, int datatype, int stepsize)
{
	int i;
	
	if(datatype == NT)
    {
		For(i,stepsize)
		{
			if(strchr("MRWSYKBDHVNXO?-.",state[i]))
				return 1;
		}
    }
	else
    {
		if(strchr("X?-.",state[0])) return 1;
    }
	
	return 0;
}

/*********************************************************/

void R_wtree(node *pere, node *fils, char *s_tree, arbre *tree)
{
	int i,p;
	
	p = -1;
	if(fils->tax)
    {
		if(OUTPUT_TREE_FORMAT == 0)
			strcat(s_tree,fils->name);
		else
			sprintf(s_tree+(int)strlen(s_tree),"%d",fils->num+1);
		
		if((fils->b[0]) && (fils->b[0]->l != -1))
		{
			strcat(s_tree,":");
			/* 	  sprintf(s_tree+(int)strlen(s_tree),"%.10f",fils->b[0]->l); */
			sprintf(s_tree+(int)strlen(s_tree),"%f",fils->b[0]->l);
		}
		sprintf(s_tree+(int)strlen(s_tree),",");
	}
	else
    {
		s_tree[(int)strlen(s_tree)]='(';
		For(i,3)
		{
			if(fils->v[i] != pere)
				R_wtree(fils,fils->v[i],s_tree,tree);
			else p=i;
		}
		s_tree[(int)strlen(s_tree)-1]=')';
		if(fils->b[0]->l != -1)
		{
			if(tree->print_boot_val)
				sprintf(s_tree+(int)strlen(s_tree),"%d",fils->b[p]->bip_score);
			else if(tree->print_alrt_val)
				sprintf(s_tree+(int)strlen(s_tree),"%f",fils->b[p]->ratio_test);
			
			strcat(s_tree,":");
			/* 	  sprintf(s_tree+(int)strlen(s_tree),"%.10f",fils->b[p]->l); */
			sprintf(s_tree+(int)strlen(s_tree),"%f",fils->b[p]->l);
			
			strcat(s_tree,",");
		}
    }
}

/*********************************************************/

char *Write_Tree(arbre *tree)
{
	
	char *s;
	int i;
	
	s=(char *)mCalloc(T_MAX_LINE,sizeof(char));
	
	s[0]='(';
	
	i = 0;
	while((!tree->noeud[tree->n_otu+i]->v[0]) ||
		  (!tree->noeud[tree->n_otu+i]->v[1]) ||
		  (!tree->noeud[tree->n_otu+i]->v[2])) i++;
	
	R_wtree(tree->noeud[tree->n_otu+i],tree->noeud[tree->n_otu+i]->v[0],s,tree);
	R_wtree(tree->noeud[tree->n_otu+i],tree->noeud[tree->n_otu+i]->v[1],s,tree);
	R_wtree(tree->noeud[tree->n_otu+i],tree->noeud[tree->n_otu+i]->v[2],s,tree);
	
	
	s[(int)strlen(s)-1]=')';
	s[(int)strlen(s)]=';';
	
	
	return s;
}

/*********************************************************/

void Connect_One_Edge_To_Two_Nodes(node *a, node *d, edge *b, arbre *tree)
{
	int i,dir_a_d;
	
	dir_a_d = -1;
	For(i,3) if(a->v[i] == d) {dir_a_d = i; break;}
	
	
	a->b[dir_a_d] = b;
	b->num        = tree->num_curr_branch_available;
	b->left       = a;
	b->rght       = d;
	if(a->tax) {b->rght = a; b->left = d;} /* root */
	/* a tip is necessary on the right side of the edge */
	
	(b->left == a)?
    (Make_Edge_Dirs(b,a,d)):
    (Make_Edge_Dirs(b,d,a));
	
	b->l                    = a->l[b->l_r];
	if(a->tax) b->l         = a->l[b->r_l];
	if(b->l < BL_MIN)  b->l = BL_MIN;
	else if(b->l > BL_MAX) b->l = BL_MAX;
	b->l_old                = b->l;
}

/*********************************************************/

void Free(void *p)
{
	if(p != NULL) free(p);
}

/*********************************************************/

void Free_Mat(matrix *mat)
{
	int i;
	
	For(i,mat->n_otu)
    {
		Free(mat->P[i]);
		Free(mat->Q[i]);
		Free(mat->dist[i]);
		Free(mat->name[i]);
    }
	
	Free(mat->P);
	Free(mat->Q);
	Free(mat->dist);
	Free(mat->name);
	Free(mat->tip_node);
	
	Free(mat->on_off);
	Free(mat);
}

/*********************************************************/

void Free_Node(node *n)
{
	int i;
	
	Free(n->b);
	Free(n->v);
	Free(n->l);
	Free(n->score);
	Free(n->name);
	
	if(n->list_of_reachable_tips)
    {
		For(i,3) Free(n->list_of_reachable_tips[i]);
		Free(n->list_of_reachable_tips);
		Free(n->n_of_reachable_tips);
    }
	
	Free(n);
}

/*********************************************************/

void Free_Edge(edge *b)
{
	Free(b);
}

/*********************************************************/

void Free_Tree(arbre *tree)
{
	int i,j,k;
	edge *b;
	node *n;
	
	For(i,2*tree->n_otu-2) Free(tree->t_dir[i]);
	Free(tree->t_dir);
	
	if(tree->has_bip)
    {
		For(i,2*tree->n_otu-2)
		{
			Free(tree->noeud[i]->bip_size);
			For(j,3)
			{
				Free(tree->noeud[i]->bip_node[j]);
				For(k,tree->n_otu) Free(tree->noeud[i]->bip_name[j][k]);
				Free(tree->noeud[i]->bip_name[j]);
			}
			Free(tree->noeud[i]->bip_node);
			Free(tree->noeud[i]->bip_name);
		}
    }
	
	Free(tree->curr_path);
	
	For(i,2*tree->n_otu-3)
    {
		b = tree->t_edges[i];
		Free_Edge(b);
    }
	Free(tree->t_edges);
	
	
	For(i,2*tree->n_otu-2)
    {
		n = tree->noeud[i];
		Free_Node(n);
    }
	Free(tree->noeud);
	
	
	For(i,tree->n_dead_edges)
    Free_Edge(tree->t_dead_edges[i]);
	
	For(i,tree->n_dead_nodes)
    Free_Node(tree->t_dead_nodes[i]);
	
	Free(tree->t_dead_edges);
	Free(tree->t_dead_nodes);
	
	
	Free(tree);
}
