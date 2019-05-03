#include "seaview.h"
#include <ctype.h>
#include <FL/Fl_Multiline_Output.H>

int nbaagroup=5;
int aagroup[30];
char aalist[30] = "ACDEFGHIKLMNPQRSTVWY";
int TRANSVONLY=0;
char infotext[10000];

extern void *check_alloc(int nbrelt, int sizelt);

void bydefault_aagroups(){
  int i;
  
  sprintf(aalist, "EDQNHRKILMVAPSGTFYWC");
  for(i=0;i<7;i++) aagroup[i]=0;
  for(;i<11;i++) aagroup[i]=1;
  for(;i<16;i++) aagroup[i]=2;
  for(;i<18;i++) aagroup[i]=3;
  for(;i<20;i++) aagroup[i]=4;
}  



/* basecomp */
/* Write in comp the composition (percent of 4 bases) of sequence seq */

int basecomp(char* seq, double* comp){
  int i, lg=0;
  
  for(i=0;i<4;i++) comp[i]=0.;
  for(i=0;seq[i];i++){
    if(seq[i]=='A') comp[0]++;
    else if(seq[i]=='C') comp[1]++;
    else if(seq[i]=='G') comp[2]++;
    else if(seq[i]=='T') comp[3]++;
    else lg--;
    lg++;
  }
  if(lg<=0) return 0;
  for(i=0;i<4;i++) comp[i]=comp[i]*100/lg;
  return lg;
}


/* aaclass */
/* Return the class of aminoacid aa */

int aaclass(char aa){
  
  int i=0;
  
  while(aalist[i]!=aa && aagroup[i]!=-1) i++;
  if(aagroup[i]==-1) return 5;
  return(aagroup[i]);
}


/* aacomp */
/* Compute aminoacid composition of proteic sequence seq. */
/* The whole composition (percent of 20 aminoacid) is written in comp, */
/* ordered as in aalist. */
/* Percent of each aminoacid group are written in compgroup. */

int aacomp(char* seq, double* comp, double* compgroup){
  int i, j, lg=0, nbaa;
  int c;
  
  nbaa=(int)strlen(aalist);
  for(i=0;i<nbaa;i++) comp[i]=0.;
  for(i=0;i<nbaagroup;i++) compgroup[i]=0.;
  for(i=0;seq[i];i++){
    for(j=0;j<nbaa;j++){
      if(seq[i]==aalist[j]) { comp[j]++; break; }
    }
    if(j==nbaa) {
      lg--;
      c = -1;
      }
    else {
      c=aaclass(seq[i]);
      }
    if(c!=-1) compgroup[c]++;
    lg++;
  } 
  if(lg<=0) return 0;
  for(i=0;i<nbaa;i++) comp[i]=comp[i]*100/lg; 
  for(i=0;i<nbaagroup;i++) compgroup[i]=compgroup[i]*100/lg;
  return lg;
}



/* variable */
/* Returns the number of variable sites in sequences sequ */

int variable(char** sequ, int nb){
  int i, j, var;
  
  var=strlen(sequ[0]);
  for(i=0;sequ[0][i];i++){
    for(j=1;j<nb;j++){
      if(sequ[j][i]!=sequ[0][i]) break;
    }
    if(j==nb) var--;
  }
  return var;
}



/* informative */
/* Returns the number of informative sites in sequences sequ */

static int informative(char** sequ, int nb){
  
  int i, j, k, info=0, siteok=0;
  char firstpair;
  
  
  for(i=0;sequ[0][i];i++){
    firstpair=0;
    siteok=0;
    for(j=0;j<nb-1;j++){
      for(k=j+1;k<nb;k++){
	if(sequ[j][i]==sequ[k][i]){
 	  if(firstpair){
	    if(sequ[j][i]!=firstpair) { info++; siteok=1; break; }
	  }
	  else
	    firstpair=sequ[j][i];
        }
      }
      if (siteok) break;
    }
  }
  
  return info;
}


/* refresh */
/* Remove all sites with gaps only from sequence alignment seq. */
/* If option==0, all sites with at least one gap are removed. */

void refresh(char** seq, int nbseq, int option, int prot)
{
  
  int lgseq, l=-1, drapeau, i, j, k;
  char **seqref ;
  
  lgseq=(int)strlen(seq[0]);
  seqref = (char**)check_alloc(nbseq, sizeof(char*));
  for(i=0;i<nbseq;i++)
    seqref[i]=(char*)check_alloc(lgseq+1, sizeof(char));
  
  if (option==0){ /* no site with at least one gap */
    for(i=0;i<lgseq;i++){
      drapeau=0;
      for(j=0;j<nbseq;j++){
	if (!prot  && seq[j][i]!='A' && seq[j][i]!='C' && seq[j][i]!='G' && seq[j][i]!='T' && (!TRANSVONLY || (seq[j][i]!='R' && seq[j][i]!='Y'))) drapeau=1;
	if (prot && (seq[j][i]=='X' || seq[j][i]=='-')) drapeau=1;
      }
      if (drapeau==0){
	l++;
	for(k=0;k<nbseq;k++) *(seqref[k]+l)=*(seq[k]+i);
      }	
    }
    for(i=0;i<nbseq;i++)
      for (j=l+1;j<lgseq;j++) 
        *(seqref[i]+j)='\0';
  }
  else{ /* no site with only gaps */
    for(i=0;i<lgseq;i++){
      drapeau=0;
      for(j=0;j<nbseq;j++){
	if (!prot && ((*(seq[j]+i)=='A' || *(seq[j]+i)=='C' || *(seq[j]+i)=='T' || *(seq[j]+i)=='G') || (TRANSVONLY && (*(seq[j]+i)=='R' || *(seq[j]+i)=='Y')))) { 
	  drapeau=1;
	  break;
	}
	if (prot && (*(seq[j]+i)!='X' || *(seq[j]+i)!='-')) { 
	  drapeau=1;
	  break;
	}
      }
      if (drapeau==1){
	l++;
	for(k=0;k<nbseq;k++){
	  *(seqref[k]+l)=*(seq[k]+i);
	  *(seqref[k]+l+1)=0;
        }
      }		
    }
  }
  for (i=0;i<nbseq;i++) 
    for (j=0;j<lgseq;j++)
      *(seq[i]+j)=*(seqref[i]+j);
  
  for(i=0;i<nbseq;i++) free(seqref[i]);
  free(seqref);
}


void reg_and_pos(SEA_VIEW *view, int *selected, list_segments* active)
{ 
  
  int sel1=0, sel2=0, sel3=0, i /*, cur_reg_deb=0*/;
  int lgseq = view->seq_length;
  list_segments start_end = {1, lgseq, NULL};
  if (active == NULL) active = &start_end;
  
  /*if(view->protein){
    sel1=sel2=sel3=1;
  }
  else{
    if(GetStatus(po1i)) sel1=1;
    if(GetStatus(po2i)) sel2=1;
    if(GetStatus(po3i)) sel3=1;
  }*/
  sel1=sel2=sel3=1;

  if(sel1 && sel2 && sel3){
    while(active) {
      for(i = active->debut - 1; i < active->fin; i++) selected[i]=1;
      active = active->next;
      }
  }
  else{
    /*if(sel1 && sel2 && !sel3){
      for(i=0;i<lgseq;i++){
        if(regions[i] && !regions[i-1]) cur_reg_deb=i;
	j=i-cur_reg_deb;
        if(regions[i] && (j%3==0 || j%3==1)) selected[i]=1; else selected[i]=0;
      }
    }
    if(sel1 && !sel2 && sel3){
      for(i=0;i<lgseq;i++){
        if(regions[i] && !regions[i-1]) cur_reg_deb=i;
	j=i-cur_reg_deb;
	if(regions[i] && (j%3==0 || j%3==2)) selected[i]=1; else selected[i]=0;
      }
    }
    if(!sel1 && sel2 && sel3){
      for(i=0;i<lgseq;i++){
        if(regions[i] && !regions[i-1]) cur_reg_deb=i;
	j=i-cur_reg_deb;
	if(regions[i] && (j%3==1 || j%3==2)) selected[i]=1; else selected[i]=0;
      }
    }
    if(sel1 && !sel2 && !sel3){
      for(i=0;i<lgseq;i++){
        if(regions[i] && !regions[i-1]) cur_reg_deb=i;
	j=i-cur_reg_deb;
	if(regions[i] && (j%3==0)) selected[i]=1; else selected[i]=0;
      }
    }
    if(!sel1 && sel2 && !sel3){
      for(i=0;i<lgseq;i++){
        if(regions[i] && !regions[i-1]) cur_reg_deb=i;
	j=i-cur_reg_deb;
	if(regions[i] && (j%3==1)) selected[i]=1; else selected[i]=0;
      }
    }
    if(!sel1 && !sel2 && sel3){
      for(i=0;i<lgseq;i++){
        if(regions[i] && !regions[i-1]) cur_reg_deb=i;
	j=i-cur_reg_deb;
 	if(regions[i] && (j%3==2)) selected[i]=1; else selected[i]=0;
      }
    }*/
    if(!sel1 && !sel2 && !sel3){
      for(i=0;i<lgseq;i++) selected[i]=0;
    }
  }
  return;
}


/* freq_obs */
/* Write at address freq observed frequencies of 16 di-nucleotides XY */
/* (X= A,C,G,T  ,  Y=A,C,G,T) X and Y being homologous nucleotides of sequences */
/* seq1 and seq2. Alphabetic order is used : freq[0]=AA frequency, freq[1]=AC, */
/* ..., freq[15]=TT. */
int freq_obs(char* seq1, char* seq2, double* freq){
  
  int i, lgseq, lgseqvrai;
  
  if ((int)strlen(seq1)!=(int)strlen(seq2)){
    printf ("Longueurs inegales.\n");
    return -1;
  }
  for(i=0;i<16;i++) freq[i]=0;
  lgseq=(int)strlen(seq1);
  lgseqvrai=lgseq;
  for(i=0;i<lgseq;i++){
    switch(seq1[i]){
      case 'A':
	switch(seq2[i]){
	  case 'A' : freq[0]++; break;
	  case 'C' : freq[1]++; break;
	  case 'G' : freq[2]++; break;
	  case 'T' : freq[3]++; break;
	  default : lgseqvrai --; break;
	}
	break;
      case 'C':
	switch(seq2[i]){
	  case 'A' : freq[4]++; break;
	  case 'C' : freq[5]++; break;
	  case 'G' : freq[6]++; break;
	  case 'T' : freq[7]++; break;
	  default : lgseqvrai --; break;
	}
	break;
      case 'G':
	switch(seq2[i]){
	  case 'A' : freq[8]++; break;
	  case 'C' : freq[9]++; break;
	  case 'G' : freq[10]++; break;
	  case 'T' : freq[11]++; break;
	  default : lgseqvrai --; break;
	}
	break;
      case 'T':
	switch(seq2[i]){
	  case 'A' : freq[12]++; break;
	  case 'C' : freq[13]++; break;
	  case 'G' : freq[14]++; break;
	  case 'T' : freq[15]++; break;
	  default : lgseqvrai --; break;
	}
	break;
      default :
	lgseqvrai --;
    }
  }
  if(lgseqvrai!=0){
    for(i=0;i<16;i++) freq[i]/=lgseqvrai;
    return 1;
  }
  else return 0;
}


static void save_stat_cb(Fl_Widget *ob, void *data)
{
  Fl_Multiline_Output *ml = (Fl_Multiline_Output*)data;
  Fl_Native_File_Chooser fnfc;
  fnfc.title("Pick a text file");
  fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  fnfc.filter("Text\t*.txt\n");
  fnfc.preset_file("statistics.txt");
  fnfc.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM | fnfc.options());
  if ( fnfc.show() != 0 ) return;
  FILE *out = fopen(fnfc.filename(), "w");
  fprintf(out, "Alignment: %s\n\n", (char*)ml->user_data());
  fputs(ml->value(), out);
  fclose(out);
}

void geni_act(SEA_VIEW *view)
{
  int i, j, k, l, ii, nb, lgtot, lg, lgvar, lginfo, *lgcomp, totlgcomp, nc=0;
  int *selected, nbaa, nbcp;
  double **comp, *compall, compcomp[4], **compgroup, *compgroupall, min[4], max[4];
  double P, Psum=0., Q, Qsum=0., Pmin, Pmax, Qmin, Qmax, titvratio, titvmin, titvmax, freq[16], mat_obs[16], *ti, *tv;
  char **genseq, **genname, *text, *minname[4], *maxname[4], nt[4]={'A', 'C', 'G', 'T'};
  char *titvminname1, *titvminname2, *titvmaxname1, *titvmaxname2;
  int lgseq = view->seq_length;
  int nbseq = view->tot_seqs;
  
  bydefault_aagroups();
  
  text=(char*)check_alloc(10000, sizeof(char));
  
  nb=lgtot=0;
  selected=(int*)check_alloc(lgseq, sizeof(int));
  reg_and_pos(view, selected, view->active_region ? view->active_region->list : NULL);
  nbaa=(int)strlen(aalist);
  
  /* SET UP DATA */
  
  for(i=0;i<lgseq;i++) if (selected[i]) lgtot++;
  if(lgtot==0){
    fl_alert("No selected sites");
    free(selected); free(text);
    return;
  }
  for(i=0;i<nbseq;i++) if (view->tot_sel_seqs == 0 || view->sel_seqs[i]) nb++;
  genseq=(char**)check_alloc(nb, sizeof(char*));
  genname=(char**)check_alloc(nb, sizeof(char*));
  for(i=0;i<nb;i++)
    genseq[i]=(char*)check_alloc(lgtot+1, sizeof(char));
  j=0;
  for(i=0;i<nbseq;i++){
    if(view->tot_sel_seqs == 0 || view->sel_seqs[i]){
      genname[j]=view->seqname[i];
      l=0;
      for(k=0;k<lgseq;k++) {
	if(selected[k]) genseq[j][l++] = toupper(view->sequence[i][k]);
	}
      j++;
    }
  }
  
  
  ti=(double*)check_alloc(nb*(nb-1)/2, sizeof(double));
  tv=(double*)check_alloc(nb*(nb-1)/2, sizeof(double));
  
  
  /* ALL SITES : COMPOSITION */
  
  lgcomp=(int*)check_alloc(nb, sizeof(int));
  comp=(double**)check_alloc(nb, sizeof(double*));
  for(i=0;i<nb;i++) 
    comp[i]=(double*)check_alloc(view->protein?nbaa:4, sizeof(double));
  compall=(double*)check_alloc(view->protein?nbaa:4, sizeof(double));
  
  if(view->protein){
    compgroup=(double**)check_alloc(nb, sizeof(double*));
    for(i=0;i<nb;i++) 
      compgroup[i]=(double*)check_alloc(nbaagroup, sizeof(double));
    compgroupall=(double*)check_alloc(nbaagroup, sizeof(double));
    
    totlgcomp=0;
    for(i=0;i<nb;i++){
      lgcomp[i]=aacomp(genseq[i], comp[i], compgroup[i]);
      totlgcomp+=lgcomp[i];
    }
    for(i=0;i<nbaa;i++){
      compall[i]=0.;
      for(j=0;j<nb;j++)
	compall[i]+=comp[j][i]*lgcomp[j];
      compall[i]/=totlgcomp;
    }
    for(i=0;i<nbaagroup;i++){
      compgroupall[i]=0.;
      for(j=0;j<nb;j++)
	compgroupall[i]+=compgroup[j][i]*lgcomp[j];
      compgroupall[i]/=totlgcomp;
    }
  }
  else{  
    totlgcomp=0;
    for(i=0;i<nb;i++){
      lgcomp[i]=basecomp(genseq[i], comp[i]);
      totlgcomp+=lgcomp[i];
    }
    for(i=0;i<4;i++){
      compall[i]=0.;
      for(j=0;j<nb;j++)
	compall[i]+=comp[j][i]*lgcomp[j];
      compall[i]/=totlgcomp;
      
    }
  }  
  
  
  /* COMPLETE SITES ONLY */
  
  refresh(genseq, nb, 0, view->protein);
  lg=(int)strlen(genseq[0]);
  lgvar=variable(genseq, nb);
  lginfo=informative(genseq, nb);
  
  /* COMPOSITION */
  
  if(!view->protein && lg!=0){
    for(i=0;i<nb;i++)
      lgcomp[i]=basecomp(genseq[i], comp[i]);
    for(i=0;i<4;i++){
      compcomp[i]=0;
      for(j=0;j<nb;j++) compcomp[i]+=comp[j][i];
      compcomp[i]/=nb;
      min[i]=max[i]=comp[0][i]; minname[i]=maxname[i]=genname[0];
      for(j=0;j<nb;j++){
	if(comp[j][i]<min[i]){ min[i]=comp[j][i]; minname[i]=genname[j]; }
	if(comp[j][i]>max[i]){ max[i]=comp[j][i]; maxname[i]=genname[j]; }
      }
    }
  }
  
  /* CHANGES */
  
  if(!view->protein && lg!=0){
        
    for(ii=0;ii<16;ii++) mat_obs[ii]=0.;
    k=0; nbcp=0;
    titvmin=100000.; titvmax=0.;
    for(i=0;i<nb-1;i++){
      for(j=i+1;j<nb;j++){
        freq_obs(genseq[i], genseq[j], freq);
        P=(freq[2]+freq[7]+freq[8]+freq[13])*lg;
        Q=(freq[1]+freq[3]+freq[4]+freq[6]+freq[9]+freq[11]+freq[12]+freq[14])*lg;
        for(ii=0;ii<16;ii++) mat_obs[ii]+=freq[ii];
	
	ti[nc]=P;
	tv[nc]=Q;
	nc++;
	
	
	Psum+=P;
	Qsum+=Q;
        if(Q>0.1){
	  titvratio=P/Q;
          if(titvratio<titvmin){ 
	    titvmin=titvratio; 
	    titvminname1=genname[i];
	    titvminname2=genname[j];
	    Pmin=P; Qmin=Q;
	  }
      	  if(titvratio>titvmax){ 
	    titvmax=titvratio; 
	    titvmaxname1=genname[i];
	    titvmaxname2=genname[j];
	    Pmax=P; Qmax=Q;
	  }
	  nbcp++;
        }
        k++;
      }
    }
    
    for(ii=0;ii<16;ii++) mat_obs[ii]/=(nb*(nb-1)/(2*100.));
    
    /*
     printf("observed matrix :\n");
     printf("AA : %.1f  CC : %.1f  GG : %.1f  TT : %.1f\n", mat_obs[0], mat_obs[5], mat_obs[10], mat_obs[15]);
     printf("AG : %.1f  CT : %.1f\n", mat_obs[2]+mat_obs[8], mat_obs[7]+mat_obs[13]);
     printf("AC : %.1f  AT : %.1f  CG : %.1f  GT : %.1f\n", mat_obs[1]+mat_obs[4], mat_obs[3]+mat_obs[12], mat_obs[6]+mat_obs[9], mat_obs[11]+mat_obs[14]);
     */
    
    /*
     titvf=fopen("TITV", "w");
     fprintf(titvf, "Ti\tTv\n");
     for(i=0;i<nc;i++) fprintf(titvf, "%f\t%f\n", ti[i], tv[i]);
     fclose(titvf);
     */
    
  }
  
  
  /* PAIRWISE SUBSTITUTION MATRIX */
  
  if(!view->protein && nb==2 && lg!=0)
    freq_obs(genseq[0], genseq[1], freq);
  
  
  /* DISPLAY INTO NEW WINDOW */
  char *p;
  Fl_Window *win = new Fl_Window(600, 500, "Statistics");
  Fl_Multiline_Output *ml = new Fl_Multiline_Output(2, 2, win->w()-4, win->h()-24);
  ml->textfont(FL_COURIER);
  ml->user_data(view->masename);
  win->resizable(ml);
  Fl_Button *b = new Fl_Button(win->w()/2 - 25 , win->h() - 23, 50, 20, "Save");
  b->callback(save_stat_cb, ml);
  Fl_Return_Button *ok = new Fl_Return_Button(win->w() - 55 , win->h() - 23, 50, 20, "OK");

  if(view->tot_sel_seqs == 0) sprintf(text, "%d species\n", nb);
  else {
    sprintf(text, "%d selected species\n", nb);
  }
  if(view->tot_sel_seqs > 0 && nb<=20){
    for(i=0;i<view->tot_seqs;i++){
      if(!view->sel_seqs[i]) continue;
      strcat(text, view->seqname[i]); strcat(text, ", ");
      if((i+1)%4==0 || i==nb-1) strcat(text, "\n");
    }
    strcat(text, "\n");
  }
  p = text + strlen(text);
  sprintf(p, "\n%d selected sites\n", lgtot);
  p += strlen(p);
  if(view->protein)
    sprintf(p, "    including    %d complete (no gaps, no X)\n", lg);
  else
    sprintf(p, "    including    %d complete (no gaps, no N)\n", lg);
  p += strlen(p);
  if(nb>=2 && lg!=0){
    sprintf(p, "    including    %d variable (%.1f%% of complete)\n", lgvar, (double)lgvar*100/lg);
    p += strlen(p);
    if(nb>=4){
      sprintf(p, "    including    %d informative (%.1f%% of complete)\n", lginfo, (double)lginfo*100/lg);
      p += strlen(p);
    }
  }
  if(view->protein){
    int k;
    sprintf(p, "\nAMINOACID COMPOSITION (all sites) :\n");
    p += strlen(p);
    j=0; 
    for(i=0;i<nbaagroup;i++){
      sprintf(p, "\n\nGroup %d : %.1f%%", i+1, compgroupall[i]);
      p += strlen(p);
      k = 0;
      while(aagroup[j]==i){
	if(k++ % 3 == 0) {
	  sprintf(p, "\n    "); p += strlen(p);
	  }
	sprintf(p, "%c : %.1f     ", aalist[j], compall[j]);
	p += strlen(p);
	j++;
      }
      if(k % 3 != 0) sprintf(p++, "\n");
    }
    if(j!=nbaa){
      sprintf(p, "\n\nOthers : \n");
      p += strlen(p);
      while(aalist[j]){
	sprintf(p, "%c : %.1f     ", aalist[j], compall[j]);
	p += strlen(p);
	sprintf(p++, "\n");
	j++;
      }
    }
  }
  else{
    sprintf(p, "\nBASE COMPOSITION :\n");
    p += strlen(p);
    sprintf(p, "     All sites  :\t %.1f%% A   %.1f%% C   %.1f%% G   %.1f%% T\n", compall[0], compall[1], compall[2], compall[3]);
    p += strlen(p);
    if(nb>1 && lg!=0){
      sprintf(p, "     Complete sites only :\n");
      p += strlen(p);
      sprintf(p, "                                Minimum                   Maximum:\n");
      p += strlen(p);
      for(i=0;i<4;i++){
        sprintf(p, "     %c : %.1f %%    ", nt[i], compcomp[i]);
	p += strlen(p);
        sprintf(p, "%.1f%% (%s)     ", min[i], minname[i]);
 	p += strlen(p);
        sprintf(p, "%.1f%% (%s)\n", max[i], maxname[i]);
	p += strlen(p);
      }
    }
  }
  
  if(!view->protein && nb>1 && lg!=0){
    sprintf(p, "\nOBSERVED CHANGES (complete sites)\n"); p += strlen(p);
    if(nb>2)
      sprintf(p, "Transition/transversion ratio : %.3f (mean over all sequence pairs)\n", (double)Psum/Qsum);
    else
      sprintf(p, "Transition/transversion ratio : %.3f\n", (double)Psum/Qsum);
    p += strlen(p);
    if(nb>2){
      sprintf(p, "Minimum : %.3f (%s - %s, %d transitions, %d transversions)\n", titvmin, titvminname1, titvminname2, (int)Pmin, (int)Qmin);
      p += strlen(p);
      sprintf(p, "Maximum : %.3f (%s - %s, %d transitions, %d transversions)\n", titvmax, titvmaxname1, titvmaxname2, (int)Pmax, (int)Qmax);
      //p += strlen(p);
    }
    else {
      sprintf(p, "Observed nucleotide pairs :\n"); p += strlen(p);
      sprintf(p, "%35s%s\n"
	         "%36s A     C     G     T\n", "", genname[1], ""); p += strlen(p);
      for(i=0;i<4;i++){
	sprintf(p, "%30s %c", i==1 ? genname[0] : "", nt[i]); p += strlen(p);
   	for(j=0;j<4;j++){
	  sprintf(p, "%6d", (int)(freq[4*i+j]*lgtot)); p += strlen(p);
	}
	*p++ = '\n';
      }
    }
  } 
  
  ml->value(text);
  win->end();
  win->show();
  while (TRUE) {
    Fl_Widget* obj = Fl::readqueue();
    if (!obj) Fl::wait();
    else if (obj == ok || obj == win) {
      delete win;
      break;
    }
  }
  
  free(selected); free(text);
  for(i=0;i<nb;i++) {free(genseq[i]); free(comp[i]);}
  free(genseq); free(genname); free(comp);
  free(lgcomp); free(compall); 
  if(view->protein){
    for(i=0;i<nb;i++) free(compgroup[i]);
    free(compgroup);
    free(compgroupall);
  }
} 

void stats_callback(Fl_Widget *obj, void *data)
{
  geni_act((SEA_VIEW*)obj->user_data());
}
