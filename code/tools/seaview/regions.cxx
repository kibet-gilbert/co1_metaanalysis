#include "seaview.h"
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Check_Button.H>
#include <ctype.h>
#ifdef WIN32
#include <wchar.h>
extern "C" {
  int _wsystem(const wchar_t *command );
}
#endif


enum position_mask {custom_mask = -1, gblocks_sites, position_1, position_2, position_3, position_12,
  variable_sites  };
const char *position_mask_names[] = {"Gblocks", "First codon pos.", "Second codon pos.",
"Third codon pos.", "1st+2nd codon pos.", "Variable sites"};
int position_mask_count = sizeof(position_mask_names)/sizeof(char*);

/* prototypes of included functions */
region *duplicate_region(region *source, const char *name);
void free_region(region *reg);
region *copy_region(region *from, region *to);
int create_full_region(SEA_VIEW *view, const char *name);
void save_active_region(SEA_VIEW *view );
void hide_region_line(SEA_VIEW *view);
int create_codon_position_mask(SEA_VIEW *view, position_mask mask, const char *regname);
int rename_current_region(SEA_VIEW *view, const char *name);
void delete_region(SEA_VIEW *view, int rang);
void activate_region_line(SEA_VIEW *view, int rang);
void ajout_segment(region *maregion, int debut, int fin);
int suppr_segment(region *maregion, int site, char *line);
list_segments *get_segment(region *maregion, int site);
void draw_region_line(Fl_Widget *ob, SEA_VIEW *view);
const char *site_mask_creation_dialog(SEA_VIEW *view, position_mask *outmask);
void regions_menu_callback(Fl_Widget *ob, void *extra);
int begin_change_segment(SEA_VIEW *view, int new_site);
int continue_change_segment(SEA_VIEW *view, int new_site);
void end_change_segment(SEA_VIEW *view);
int extend_segment_at_left(SEA_VIEW *view, int new_site);
static void delete_in_region_line(SEA_VIEW *view, int numsite, int total);
void delete_region_part(SEA_VIEW *view, int numsite, int total);
static void insert_in_active_region(SEA_VIEW *view, int numsite, int total);
void insert_region_part(SEA_VIEW *view, int numsite, int total);
list_regions *parse_regions_from_header(char *header);
void draw_region_background(SEA_VIEW *view, int f_seq0, int l_seq0);
const char *create_species_set(SEA_VIEW *view, const char *set_name);
void species_menu_callback(Fl_Widget *ob, void *extra);
int parse_species_sets_from_header(char *header, int **list_species_sets,
	char **name_species_sets, int totseqs);


extern void adjust_menu_edit_modes(SEA_VIEW *view);
int compute_size_params(SEA_VIEW *view, int force_recompute);
extern char *create_tmp_filename(void);
extern void delete_tmp_filename(const char *base_fname);


region *duplicate_region(region *source, const char *name)
{
region *destin;
list_segments *pseg, *dest_seg, *prev;

destin = (region *)calloc(1, sizeof(region));
if(destin == NULL) return NULL;
if(name != NULL) {
	size_t l;
	l = strlen(name);
	destin->name = (char *)calloc(l+1, 1);
	if(destin->name == NULL) return NULL;
	memcpy(destin->name, name, l+1);
	}
if(source == NULL) return destin;
pseg = source->list;
if(pseg == NULL) return destin;
destin->list = (list_segments *)calloc(1, sizeof(list_segments));
if(destin->list == NULL) return NULL;
*(destin->list) = *pseg;
destin->list->next = NULL;
pseg = pseg->next;
prev = destin->list;
while(pseg != NULL) {
	dest_seg = (list_segments *)calloc(1, sizeof(list_segments));
	if(dest_seg == NULL) return NULL;
	*dest_seg = *pseg;
	dest_seg->next = NULL;
	prev->next = dest_seg;
	prev = dest_seg;
	pseg = pseg->next;
	}
destin->hide_when_viewasprots = source->hide_when_viewasprots;
return destin;
}


void free_region(region *reg)
{
list_segments *pseg, *suiv;
if(reg == NULL) return;
pseg = reg->list;
while(pseg != NULL) {
	suiv = pseg->next;
	free(pseg);
	pseg = suiv;
	}
if(reg->name != NULL) free(reg->name);
free(reg);
}


region *copy_region(region *from, region *to)
{
region *retval;
char *name;
name = to->name;
to->name = NULL;
free_region(to);
retval = duplicate_region(from, NULL);
if(retval ==  NULL) out_of_memory();
retval->name = name;
return retval;
}


int create_full_region(SEA_VIEW *view, const char *name)
{
list_regions *old, *new_list;
region *maregion;
/* le nom existe-t-il deja? */
old = view->regions;
while(old != NULL) {
	if(strcmp(name, old->element->name) == 0) return FALSE;
	old = old->next;
	}
maregion = (region *)calloc(1,sizeof(region));
if(maregion ==  NULL) out_of_memory();
maregion->name = (char *)malloc(strlen(name)+1);
if(maregion->name ==  NULL) out_of_memory();
strcpy(maregion->name, name);
maregion->list = (list_segments *)calloc(1, sizeof(list_segments));
if(maregion->list ==  NULL) out_of_memory();
maregion->list->debut = 1;
maregion->list->fin = view->seq_length;
new_list = (list_regions *)calloc(1, sizeof(list_regions));
if(new_list ==  NULL) out_of_memory();
new_list->element = maregion;
/* placer nouvelle region en fin de liste des regions connues */
if(view->regions == NULL)
	view->regions = new_list;
else	{
	old = view->regions;
	while(old->next != NULL) old = old->next;
	old->next = new_list;
	}
view->active_region = duplicate_region(maregion, name);
if(view->active_region == NULL) return TRUE; /*mess no mem serait mieux */
Fl_Menu_Item *items = (Fl_Menu_Item *)view->menu_file;
items[SAVE_REGIONS].activate();
view->region_length = view->seq_length;
memset(view->region_line, 'X', view->region_length);
view->region_line[view->region_length] = 0;
compute_size_params(view, TRUE);
return TRUE;
}


void create_variable_mask(SEA_VIEW *view, region *maregion)
{
  int i, j, in_variable = FALSE;
  list_segments *segment, *previous;
  previous = NULL;
 
  for(i = 1; i <= view->seq_length; i++) {
    for (j = 1; j < view->tot_seqs; j++) {
      if(toupper(view->sequence[j][i-1]) != toupper(view->sequence[0][i-1])) break;
      }
    if(j < view->tot_seqs) {
      if(in_variable) continue;
      in_variable = TRUE;
      segment = (list_segments *)calloc(1, sizeof(list_segments));
      segment->debut = i;
      if(previous != NULL) previous->next = segment;
      else maregion->list = segment;
      previous = segment;
    }
    else {
      if( ! in_variable) continue;
      in_variable = FALSE;
      segment->fin = i - 1;
      for(j = segment->debut; j <= segment->fin; j++) view->region_line[j-1] = 'X';
      }
    }
  if(in_variable) {
    segment->fin = view->seq_length;
    for(j = segment->debut; j <= segment->fin; j++) view->region_line[j-1] = 'X';
    }
}

int run_gblocks_dialog(int *pb5_val, int *pb4_val, int *pb3_val, int *pb2_val)
{
  Fl_Widget *o;
  Fl_Window *w = new Fl_Window(370,100);
  w->label("Gblocks options dialog");
  Fl_Box *b = new Fl_Box(5, 5, w->w(), 20, "Options for a less stringent selection:");
  b->align(FL_ALIGN_LEFT + FL_ALIGN_INSIDE);
  b->labelfont(FL_TIMES_BOLD);
  Fl_Check_Button *b4 = new Fl_Check_Button(10,30, 350, 20, "Allow smaller final blocks");
  Fl_Check_Button *b5 = new Fl_Check_Button(10,50, 350, 20, "Allow gap positions within the final blocks");
  Fl_Check_Button *b2 = new Fl_Check_Button(10,70, 350, 20, "Allow less strict flanking positions");
  Fl_Box *box2 = new Fl_Box(5, b2->y() + 20, w->w(), 20, "Option for a more stringent selection:");
  box2->align(FL_ALIGN_LEFT + FL_ALIGN_INSIDE);
  box2->labelfont(FL_TIMES_BOLD);
  Fl_Check_Button *b3 = new Fl_Check_Button(10,box2->y()+20, 350, 20, "Do not allow many contiguous nonconserved positions");
  Fl_Button *cancel = new Fl_Button(10, b3->y()+30, 50, 20, "Cancel");
  Fl_Return_Button *ret = new Fl_Return_Button(w->w()-60, cancel->y(), 50, 20, "OK");
  w->size(w->w(), ret->y()+22);
  w->end();
  w->set_non_modal();
  w->show();
  while(TRUE) {
    o = Fl::readqueue();
    if (!o) Fl::wait();
    else if(o == cancel || o == w) {
      return 1;
    }
    else if(o == ret)
      break;
  }
  *pb5_val = b5->value(); // -b5=h
  *pb4_val = b4->value(); // -b4=5
  *pb3_val = b3->value(); // -b3=4
  *pb2_val = b2->value(); // -b2=(50% of species)
  Fl::delete_widget(w);
  if (o == cancel || o == w) return 1;
  return 0;
}

int create_gblocks_mask(SEA_VIEW *view, region *maregion, int no_gui=false, int b5_val=0, int b4_val=0, int b3_val=0, int b2_val=0)
/* return 0 iff OK */
{
  char cmd[500];
  char base_fname[PATH_MAX], fname[PATH_MAX], *p, *q, *last;
  int status, segment_count = -1, num, l;
  FILE *in;
  list_segments *segment, *previous;
  void (*alert)(const char*,...) = (no_gui ? Fl::fatal : fl_alert);
  
  q = get_full_path(
		    "Gblocks"
#ifdef WIN32
		    ".exe"
#endif
		    );
  if (!q) {
    alert("Program Gblocks not found."
#if !(defined(WIN32) || defined(__APPLE__))
    "\nTo use Gblocks, download it from http://molevol.cmima.csic.es/castresana/Gblocks.html\n"
    "and put it in a directory of your PATH"
#endif
    );
    return 1;
  }
    
  if (!no_gui && run_gblocks_dialog(&b5_val, &b4_val, &b3_val, &b2_val)) return 1;
  p = create_tmp_filename();
  if(p == NULL) return 1;
  strcpy(base_fname, p);
  sprintf(fname, "%s.fst", base_fname);
  char **newnames = new char*[view->tot_seqs];
  for (int i = 0; i < view->tot_seqs; i++) {
    newnames[i] = new char[15];
    sprintf(newnames[i], "Seq_%d", i);
    }
  status = save_fasta_file(fname, view->sequence, NULL,
		      newnames, view->tot_seqs, view->each_length, NULL,
			   NULL, 0, FALSE, TRUE);
  for (int i = 0; i < view->tot_seqs; i++) delete newnames[i];
  delete[] newnames;
  if (status) {
    alert("Cannot write to temporary file %s", fname);
    delete_tmp_filename(base_fname);
    return 1;
    }
  sprintf(cmd, "\"%s\" \"%s\" -p=s -t=%c", q, fname, view->protein?'p':'d');
  if (b5_val) strcat(cmd, " -b5=h");
  if (b4_val) strcat(cmd, " -b4=5");
  if (b3_val) strcat(cmd, " -b3=4");
  if (b2_val) sprintf(cmd + strlen(cmd), " -b2=%d", view->tot_seqs/2 + 1);
#ifdef WIN32
  //conversion of utf-8 cmd into a WCHAR string
  int wlen = MultiByteToWideChar(CP_UTF8, 0, cmd, -1, NULL, 0);
  WCHAR *wcmd = new WCHAR[wlen];
  MultiByteToWideChar(CP_UTF8, 0, cmd, -1, wcmd, wlen);  
  //_wsystem(wcmd);
  PROCESS_INFORMATION pi; 
  STARTUPINFOW si;
  ZeroMemory( &pi, sizeof(PROCESS_INFORMATION) );
  ZeroMemory( &si, sizeof(STARTUPINFOW) );
  l = CreateProcessW(NULL, wcmd, 0,0,FALSE,CREATE_NO_WINDOW,0,0, &si, &pi);
  if (l) {
    while ( WaitForSingleObject(pi.hProcess, 0) != WAIT_OBJECT_0) Fl::wait(0.1);
  }
  delete[] wcmd;
#else
  strcat(cmd, " > /dev/null");
  system(cmd);
#endif
  sprintf(fname, "%s.fst-gb.txts", base_fname);
  in = fopen(fname, "r");
  if (in == NULL) status = 1;
  if (!status) {
    do {
      p = fgets(cmd, sizeof(cmd), in); 
      if (p == NULL) { status = 1; break; }
      }
    while(strncmp(cmd, "Flank positions of the", 22) != 0);
    if (p) {
      sscanf(cmd + 22, "%d", &segment_count);
      p = fgets(cmd, sizeof(cmd), in);
      if (p == NULL || segment_count < 0 || strncmp(cmd, "Flanks:", 7) != 0) status = 1;
      }
    }
  if (status == 0 && segment_count == 0) {
    alert("Gblocks didn't find any block of conserved sites");
    status = 1;
    }
  if (!status) {
    p = cmd;
    previous = NULL;
    for (num = 0; num < segment_count; num++) {
      q = NULL;
      p = strchr(p, '[');
      if (p != NULL) q = strchr(p, ']');
      if (q == NULL) {
	l = strlen(cmd);
	memmove(cmd, last, l + 1 - (last - cmd));
	l -= (last - cmd);
	p = cmd;
	fgets(cmd + l, sizeof(cmd) - l, in);
	p = strchr(p, '[');
	if (p != NULL) q = strchr(p, ']');
	if (p == NULL || q == NULL) break;
      }
      p++;
      segment = (list_segments*)calloc(sizeof(list_segments), 1);
      sscanf(p, "%d", &segment->debut);
      p = strchr(p, ' ');
      sscanf(p, "%d", &segment->fin);
      p = strchr(p, ']');
      last = p;
      if (previous) previous->next = segment;
      else maregion->list = segment;
      previous = segment;
      for(int j = segment->debut; j <= segment->fin; j++) view->region_line[j-1] = 'X';
      }
    if (num < segment_count) status = 1;
    maregion->hide_when_viewasprots = FALSE;
    }
  if (in) fclose(in);
  delete_tmp_filename(base_fname);
  return status;
}


int create_codon_position_mask(SEA_VIEW *view, position_mask mask, const char *regname)
/* return values:
 0  no region was created
 1  OK
 2  name is already used
 */
{
  list_regions *old, *new_list;
  region *maregion;
  list_segments *segment, *previous;
  int i;
  /* le nom existe-t-il deja? */
  old = view->regions;
  while (old != NULL) {
    if(strcmp(regname, old->element->name) == 0) break;
    old = old->next;
  }
  if (old != NULL) {
    if (mask != gblocks_sites) return 2;
    int taille, valeur;
    taille = view->menu_sites->vlength();
    valeur = taille;
    while(old->next) {
      old = old->next;
      valeur--;
      }
    delete_region(view, valeur);
    }
  maregion = (region *)calloc(1,sizeof(region));
  if(maregion ==  NULL) out_of_memory();
  maregion->name = (char *)malloc(strlen(regname) + 1);
  if(maregion->name ==  NULL) out_of_memory();
  strcpy(maregion->name, regname);
  if(!view->protein) maregion->hide_when_viewasprots = TRUE;
  
  memset(view->region_line, '-', view->seq_length);
  if (mask == gblocks_sites) {
    if (create_gblocks_mask(view, maregion) ) {
      free(maregion->name);
      free(maregion);
      return 0;
    }
  }
  else if (mask >= position_1 && mask <= position_12) {
    previous = NULL;
    for(i = 1; i <= view->seq_length; i += 3) {
      segment = (list_segments *)calloc(1, sizeof(list_segments));
      if(mask == position_12) {
	segment->debut = i;
	segment->fin = i + 1;
	view->region_line[i - 1] = 'X';
	view->region_line[i] = 'X';
      }
      else {
	segment->debut = i + mask - position_1;
	segment->fin = segment->debut;
	view->region_line[i - 1 + mask - position_1] = 'X';
      }
      if(previous != NULL) previous->next = segment;
      else maregion->list = segment;
      previous = segment;
      }
    }
  else if (mask == variable_sites) create_variable_mask(view, maregion);
  view->region_length = view->seq_length;
  view->region_line[view->region_length] = 0;

  new_list = (list_regions *)calloc(1, sizeof(list_regions));
  if(new_list ==  NULL) out_of_memory();
  new_list->element = maregion;
  /* placer nouvelle region en fin de liste des regions connues */
  if(view->regions == NULL)
	view->regions = new_list;
  else	{
	old = view->regions;
	while(old->next != NULL) old = old->next;
	old->next = new_list;
  }
  view->active_region = duplicate_region(maregion, maregion->name);
  if(view->active_region == NULL) return TRUE; /*mess no mem serait mieux */
  Fl_Menu_Item *items = (Fl_Menu_Item *)view->menu_file;
  items[SAVE_REGIONS].activate();
  compute_size_params(view, TRUE);
  return 1;  
}


int rename_current_region(SEA_VIEW *view, const char *name)
{
list_regions *old, *new_list;
region *maregion;
size_t l;
char *p;

/* le nom existe-t-il deja? */
old = view->regions;
while(old != NULL) {
	if(strcmp(name, old->element->name) == 0) return FALSE;
	old = old->next;
	}
l = strlen(name);
p = (char *)calloc(l+1, 1);
if(p == NULL) return TRUE; /*mess no mem serait mieux */;
save_active_region(view);
memcpy(p, name, l+1);
free(view->active_region->name);
view->active_region->name = p;
maregion = duplicate_region(view->active_region, name);
if(maregion == NULL) return TRUE; /*mess no mem serait mieux */
new_list = (list_regions *)calloc(1, sizeof(list_regions));
if(new_list ==  NULL) out_of_memory();
new_list->element = maregion;
/* placer nouvelle region en fin de liste des regions connues */
if(view->regions == NULL)
	view->regions = new_list;
else	{
	old = view->regions;
	while(old->next != NULL) old = old->next;
	old->next = new_list;
	}
Fl_Menu_Item *items = (Fl_Menu_Item *)view->menu_file;
items[SAVE_REGIONS].activate();
return TRUE;
}


void save_active_region(SEA_VIEW *view)
{ /* save changes in current region */
list_regions *list;
list_segments *act_seg, *mem_seg;
int tmp, taille;

if(view->menu_sites == NULL) return;
  tmp = view->menu_sites->value();
  if(tmp == 0) return;
  taille = view->menu_sites->vlength();
list = view->regions;
if(tmp > 1) while( --tmp) list= list->next;
/* compare active and region in list */
act_seg = list->element->list;
mem_seg = view->active_region->list;
while ( (act_seg != NULL && mem_seg != NULL) || 
		(act_seg == NULL && mem_seg == NULL) ) {
	if( act_seg == NULL && mem_seg == NULL ) 
		return;
	if(act_seg->debut != mem_seg->debut || act_seg->fin != mem_seg->fin)
		break;
	act_seg = act_seg->next;
	mem_seg = mem_seg->next;
	}
list->element = copy_region(view->active_region, list->element);
if(view->format_for_save == MASE_FORMAT || view->format_for_save == NEXUS_FORMAT) {
  set_seaview_modified(view, TRUE);
  }
}


void hide_region_line(SEA_VIEW *view)
{
free_region(view->active_region);
view->active_region = NULL;
Fl_Menu_Item *items = (Fl_Menu_Item *)view->menu_file;
if (view->tot_sel_seqs == 0) items[SAVE_REGIONS].deactivate();
compute_size_params(view, TRUE);
}


void delete_region(SEA_VIEW *view, int rang)
{
list_regions *list, *previous;
region *maregion;
list_segments *segment, *next;

  view->menu_sites->remove(rang-1);
list = view->regions;
if(rang == 1) 
	view->regions = list->next;
else	{
	while( --rang) {
		previous = list;
		list= list->next;
		}
	previous->next = list->next;
	}
maregion = list->element;
segment = maregion->list;
while(segment != NULL) {
	next = segment->next;
	free(segment);
	segment = next;
	}
free(maregion->name);
free(maregion);
free(list);
hide_region_line(view);
}


void activate_region_line(SEA_VIEW *view, int rang)
{
list_regions *list;
list_segments *segment;
list = view->regions;
while ( --rang ) 
	list = list->next;
if(view->viewasprots && list->element->hide_when_viewasprots) return;
if(view->active_region == NULL) {
	Fl_Menu_Item *items = (Fl_Menu_Item *)view->menu_file;
	items[SAVE_REGIONS].activate();
	}
view->active_region = duplicate_region(list->element, list->element->name);
view->region_length = view->seq_length;
memset(view->region_line, '-', view->region_length);
view->region_line[view->region_length] = 0;
segment = view->active_region->list;
while(segment != NULL) {
	memset(view->region_line + segment->debut - 1, 'X', 
		segment->fin - segment->debut + 1);
	segment = segment->next;
	}
}


void ajout_segment(region *maregion, int debut, int fin)
/* toutes inferferences entre nouveau segment et anciens sont acceptees
*/
{
list_segments *psegment, *next, *prev;
prev = NULL; psegment = maregion->list;
while(psegment != NULL) { /* recherche premier segment apres le nouveau */
	if(debut <= psegment->fin + 1) break;
	prev = psegment;
	psegment = psegment->next;
	}
if(psegment == NULL || fin < psegment->debut - 1) { /* nouveau sans interference*/
	next = (list_segments *)calloc(1, sizeof(list_segments));
	if(next ==  NULL) out_of_memory();
	next->debut = debut; next->fin = fin; next->next = psegment;
	if(prev != NULL) prev->next = next;
	else 	maregion->list = next;
	return;
	}
/* nouveau segment interfere avec un(des) autre(s) */
psegment->debut = FL_min(debut, psegment->debut);
psegment->fin = FL_max(fin, psegment->fin);
while(psegment->next != NULL && psegment->fin >= psegment->next->debut - 1) {
	psegment->fin = FL_max(psegment->fin, psegment->next->fin);
	next = psegment->next->next;
	free(psegment->next);
	psegment->next = next;
	}
return;
}


int suppr_segment(region *maregion, int site, char *line)
/* suppression du segment entier exact contenant le site site
et la ligne-regions ligne est mise a jour avec des - (sauf si line==NULL)
valeur rendue TRUE si pas de segment supprime, FALSE si ok.
*/
{
list_segments *psegment, *prev, *next;
int debut, fin;

psegment = maregion->list; prev = NULL;
while(psegment != NULL && psegment->fin < site) {
	prev = psegment;
	psegment = psegment->next;
	}
if(psegment == NULL || psegment->debut > site) return TRUE;
next = psegment->next; debut = psegment->debut; fin = psegment->fin;
free(psegment);
if(prev != NULL) prev->next = next;
else	maregion->list = next;
if(line != NULL) memset(line+debut-1, '-', fin-debut+1);
return FALSE;
}


list_segments *get_segment(region *maregion, int site)
/* rend le segment contenant site ou NULL si site n'est pas dans un segment */
{
list_segments *psegment;
psegment = maregion->list;
while(psegment != NULL && psegment->fin < site) {
	psegment = psegment->next;
	}
if(psegment == NULL || psegment->debut > site) return NULL;
return psegment;
}


void draw_region_line(Fl_Widget *ob, SEA_VIEW *view)
{
int offset, x, y, l_line;

x = view->x_seq; 
y = view->y_seq + FL_min(view->tot_lines, view->tot_seqs) * view->line_height;
fl_font(ob->labelfont(),ob->labelsize());
/* clear the background of the region line */
fl_color(ob->color());
fl_rectf(x - view->char_width, y - view->line_height + fl_descent(), 
	(view->tot_sites + 2) * view->char_width, view->line_height);
/* write region line */
offset= view->first_site - 1;
l_line = ( offset + view->tot_sites < view->region_length ? 
view->tot_sites : view->region_length - offset);
if(l_line <= 0) return;
fl_color(view->region_color);
fl_draw(view->region_line + offset, l_line, x, y);
}  


const char *site_mask_creation_dialog(SEA_VIEW *view, position_mask *outmask)
{
  const char *retval;
  static char custom_name[50];
  *outmask = custom_mask;
  Fl_Window *w = new Fl_Window(300,220);
  w->label("Site set creation dialog");
  Fl_Input *input = new Fl_Input(180, 20, 100, 20, "Name of custom site set?");
  input->value("all sites");
  Fl_Group  *g = new Fl_Group(0,40,w->w(),130);
  Fl_Round_Button *mask[6];
  mask[0] = new Fl_Round_Button(30, input->y() + 30, 150, 20, position_mask_names[0]);
  mask[0]->type(FL_RADIO_BUTTON);
  mask[1] = new Fl_Round_Button(mask[0]->x(), mask[0]->y() + 20, 150, 20, position_mask_names[1]);
  mask[1]->type(FL_RADIO_BUTTON);
  mask[2] = new Fl_Round_Button(mask[0]->x(), mask[1]->y() + 20, 150, 20, position_mask_names[2]);
  mask[2]->type(FL_RADIO_BUTTON);
  mask[3] = new Fl_Round_Button(mask[0]->x(), mask[2]->y() + 20, 180, 20, position_mask_names[3]);
  mask[3]->type(FL_RADIO_BUTTON);
  mask[4] = new Fl_Round_Button(mask[0]->x(), mask[3]->y() + 20, 120, 20, position_mask_names[4]);
  mask[4]->type(FL_RADIO_BUTTON);
  mask[5] = new Fl_Round_Button(mask[0]->x(), mask[4]->y() + 20, 120, 20, position_mask_names[5]);
  mask[5]->type(FL_RADIO_BUTTON);
  g->end();
  if(view->protein) {
    for(int i = 1; i <= 4; i++) mask[i]->deactivate();
    }
  Fl_Return_Button *ok = new Fl_Return_Button(170, mask[5]->y() + mask[5]->h() + 10, 50, 30, "Ok");
  Fl_Button *cancel = new Fl_Button(ok->x() + ok->w() + 5, ok->y(), 60, 30, "Cancel");
  w->end();
  w->set_non_modal();
  w->show();
  while(TRUE) {
	Fl_Widget *o = Fl::readqueue();
	if (!o) Fl::wait();
	else if(o == mask[0] || o == mask[1] || o == mask[2] || o == mask[3] || o == mask[4] || o == mask[5]) {
	  ((Fl_Round_Button*)o)->setonly();
	  ok->take_focus();
	  input->deactivate();
	  if(o == mask[0]) {*outmask = gblocks_sites;}
	  else if(o == mask[1]) {*outmask = position_1;}
	  else if(o == mask[2]) {*outmask = position_2;}
	  else if(o == mask[3]) {*outmask = position_3;}
	  else if(o == mask[4]) {*outmask = position_12;}
	  else if(o == mask[5]) {*outmask = variable_sites;}
	  retval = position_mask_names[*outmask];
	}
	else if(o == cancel || o == w) {
	  retval = NULL;
	  break;
	  }
	else if(o == ok) {
	  if(input->active()) {
		strncpy(custom_name, input->value(), sizeof(custom_name) - 1);
		retval = custom_name;
		}
	  break;
	  }
	}
  delete w;
  return retval;
}


void regions_menu_callback(Fl_Widget *ob, void *extra)
{
  SEA_VIEW *view;
  const char *reg_name;
  position_mask mask;
  Fl_Menu_ *menu = (Fl_Menu_*)ob;
  view = (SEA_VIEW *) ob->user_data();
  int reponse = menu->mvalue() - view->menu_sites->get_menu();
  int ok;
  Fl_Menu_Item *items = view->menu_sites->get_menu();

  if(reponse == 0) { /* create new region */
	reg_name = site_mask_creation_dialog(view, &mask);
	if(reg_name == NULL) return;
	if(mask == custom_mask) ok = create_full_region(view, reg_name);
	else {
	  int retval = create_codon_position_mask(view, mask, reg_name);
	  if (retval == 0) return;
	  if (retval == 2) ok = 0; else ok = 1;
	  }
	if(!ok) {
	  fl_alert("This name is already used: %s", reg_name);
	  return;
	  }
	view->DNA_obj->redraw();
	items[0].deactivate();
	items[1].activate();
	items[2].activate();
	items[3].activate();
    view->menu_sites->add(reg_name, regions_menu_callback, NULL, FL_MENU_RADIO);
    view->menu_sites->value(view->menu_sites->vlength());
	}
  else if(reponse == 1) { /* duplicate current region */
	  reg_name = fl_input("New set name?", view->active_region->name);
	  if(reg_name == NULL) return;
	  if( !rename_current_region(view, reg_name) ) {
		  fl_alert("This name is already used: %s",reg_name);
		  return;
		  }
	  view->DNA_obj->redraw();
    view->menu_sites->add(reg_name, regions_menu_callback, NULL, FL_MENU_RADIO);
    view->menu_sites->value(view->menu_sites->vlength());
	  }
  else if(reponse == 2) { /* hide current region */
	  save_active_region(view);
	  hide_region_line(view);
	  view->DNA_obj->redraw();
	  items[0].activate();
	  items[1].deactivate();
	  items[2].deactivate();
	  items[3].deactivate();
    view->menu_sites->value(0);
	  }
  else if(reponse == 3) { /* delete current region */
	  list_regions *list;
	  int valeur, tmp;
    valeur = view->menu_sites->value();
    if (!valeur) return;
	  tmp = valeur;
	  list = view->regions;
	  if(tmp > 1) while( --tmp) list= list->next;
	  if( fl_choice("Confirm deletion of set:\n%s", "Cancel", "Delete", NULL, list->element->name) ) {
		  delete_region(view, valeur);
	    view->menu_sites->value(0);
		  view->DNA_obj->redraw();
		  items[0].activate();
		  items[1].deactivate();
		  items[2].deactivate();
		  items[3].deactivate();
		  }	
	  }
  else	{ /* choix d'une region courante */
	  save_active_region(view);
	  int valeur = menu->mvalue() - view->menu_sites->vitem(0) + 1;
	  activate_region_line(view, valeur);
	  compute_size_params(view, TRUE);
	  view->DNA_obj->redraw();
    view->menu_sites->value(valeur);
	  items[0].deactivate();
	  items[1].activate();
	  items[2].activate();
	  items[3].activate();
	  }
}


static int new_debut, new_fin, prev_site;
static list_segments *modified_segment;

int begin_change_segment(SEA_VIEW *view, int new_site)
/*
returns TRUE si click sur extremite d'un segment ou entre 2 segments, FALSE sinon et enleve le site clique de la region
*/
{
int tmp;
modified_segment = get_segment(view->active_region, new_site);
if ( modified_segment != NULL && new_site != modified_segment->debut &&
	new_site != modified_segment->fin) {
	/* enlever un site au milieu d'un segment */
	tmp = modified_segment->fin;
	modified_segment->fin = new_site - 1;
	ajout_segment(view->active_region, new_site+1, tmp);
	view->region_line[new_site - 1] ='-';
	return FALSE;
	}
if(modified_segment != NULL) {
	new_debut = modified_segment->debut;
	new_fin = modified_segment->fin;
	}
else	{
	new_debut = new_fin = new_site;
	view->region_line[new_site - 1] ='X';
	}
prev_site = new_site;
return TRUE;
}

int continue_change_segment(SEA_VIEW *view, int new_site)
{
int min;
char *pos, new_char;
list_segments *current_segment;

if(new_site == prev_site) return FALSE;
current_segment = get_segment(view->active_region, new_site);
if( current_segment != NULL && current_segment != modified_segment)return FALSE;
pos = view->region_line + new_site - 1;
if( *pos == '-' ) new_char = 'X';
else	new_char = '-';
min = FL_min(prev_site, new_site);
memset( view->region_line + min - 1, new_char, abs(prev_site - new_site) + 1 );
view->region_line[new_site - 1] = 'X';
if(new_char == 'X') {
	new_debut = FL_min(new_debut, new_site);
	new_fin = FL_max(new_fin, new_site);
	}
else	{
	if(new_site > prev_site) 
		new_debut = new_site;
	else
		new_fin = new_site;
	}
prev_site = new_site;
return TRUE;
}


void end_change_segment(SEA_VIEW *view)
{
if(modified_segment == NULL ) /* nouveau segment */
	ajout_segment(view->active_region, new_debut, new_fin);
else 	{
	if (new_fin > modified_segment->fin)
	/* segment rallonge par sa fin */
		ajout_segment(view->active_region, modified_segment->debut, 
 			new_fin);
	else if( new_fin < modified_segment->fin ) 
		/*segment raccourci par sa fin */
		modified_segment->fin = new_fin;
	if (new_debut < modified_segment->debut)
		/* segment rallonge par son debut */
		ajout_segment(view->active_region, new_debut, 
 			modified_segment->fin);
	else if( new_debut > modified_segment->debut ) 
		/* segment raccourci par son debut */
		modified_segment->debut = new_debut;
	}
}


int extend_segment_at_left(SEA_VIEW *view, int new_site)
{
list_segments *segment;

segment = view->active_region->list;
if(segment == NULL || segment->debut > new_site) return TRUE;
if(get_segment(view->active_region, new_site) != NULL) return TRUE;
while(segment->next != NULL) {
	if(segment->next->debut > new_site) break;
	segment = segment->next;
	}
memset(view->region_line + segment->fin, 'X', new_site - segment->fin);
ajout_segment(view->active_region, segment->fin, new_site);
return FALSE;
}


static void delete_in_region_line(SEA_VIEW *view, int numsite, int total)
/* delete total sites in pos numsite of view->region_line */
{
char *site;
int l;
l = view->region_length;
site = view->region_line + numsite - 1;
memmove(site - total, site, l - numsite + 2);
view->region_length -= total;
}


void delete_region_part(SEA_VIEW *view, int numsite, int total)
{
int debut, fin, vnumsite, vtotal;
list_segments *segment, *previous;
list_regions *next_region;
list_regions active;

if(total >= numsite) total = numsite - 1;
if(total == 0) return;
if(view->active_region != NULL) delete_in_region_line(view, numsite, total);
/* pour toutes les regions en commencant par active_region */
if(view->active_region != NULL) {
	active.element = view->active_region;
	active.next = view->regions;
	next_region = &active;
	}
else
	next_region = view->regions;
while(next_region != NULL) {
	if(view->viewasprots && next_region->element->hide_when_viewasprots) {
	  vtotal = total * 3;
	  vnumsite = numsite * 3;
	}
	else {
	  vtotal = total;
	  vnumsite = numsite;
	}
	segment = next_region->element->list;
	while(segment != NULL) {
		debut = segment->debut; fin = segment->fin;
		if(debut >= vnumsite) debut -= vtotal;
		else if(debut >= vnumsite - vtotal) debut = vnumsite - vtotal;
		if(fin >= vnumsite - vtotal) {
			fin -= vtotal;
			if(fin < debut) {
				debut = segment->debut;
				segment = segment->next;
				suppr_segment(next_region->element, debut, 
						NULL);
				}
			else	{
				segment->debut = debut;
				segment->fin = fin;
				segment = segment->next;
				}
			}
		else
			segment = segment->next;
		}
/* il peut etre necessaire de fusionner 2 segments consecutifs */
	segment = next_region->element->list;
	previous = NULL;
	while(segment != NULL) {
		if(previous != NULL && segment->debut == previous->fin + 1) {
			fin = segment->fin;
			suppr_segment(next_region->element, segment->debut, 
					NULL);
			ajout_segment(next_region->element, previous->debut, 
					fin);
			segment = previous;
			}
		previous = segment;
		segment = segment->next;
		}
	next_region = next_region->next;
	}
}


static void insert_in_active_region(SEA_VIEW *view, int numsite, int total)
/* 
insert total sites in pos numsite of view->region_line and of ->active_region 
*/
{
char *site, new_char;
list_segments *segment;

site = view->region_line + numsite - 1;
memmove(site + total, site, view->region_length - numsite + 2);
view->region_length += total;
segment = view->active_region->list;
new_char = '-';
while(segment != NULL) {
	if(segment->debut <= numsite && segment->fin >= numsite) {
		new_char = 'X';
		}
	if(segment->debut > numsite) segment->debut += total;
	if(segment->fin >= numsite) segment->fin += total;
	segment = segment->next;
	}
memset(site, new_char, total);
}


void insert_region_part(SEA_VIEW *view, int numsite, int total)
/* insert total sites in pos numsite of all known regions */
{
list_segments *segment;
list_regions *next_region;
int vtotal, vnumsite;

if(total + view->region_length > view->max_seq_length)
	total = view->max_seq_length - view->region_length;
if(total == 0) return;
if(view->active_region != NULL) insert_in_active_region(view, numsite, total);
next_region = view->regions;
while(next_region != NULL) {
	segment = next_region->element->list;
	if(view->viewasprots && next_region->element->hide_when_viewasprots) {
	  vtotal = total * 3;
	  vnumsite = numsite * 3;
	  }
	else {
	  vtotal = total;
	  vnumsite = numsite;
	  }
	while(segment != NULL) {
		if(segment->debut > vnumsite) segment->debut += vtotal;
		if(segment->fin >= vnumsite) segment->fin += vtotal;
		segment = segment->next;
		}
	next_region = next_region->next;
	}
}


list_regions *parse_regions_from_header(char *header)
/* parse the region information from header lines
and remove it from them
*/
{
char *p, reg_name[200], aux[200], separ[5], *i, *j, *new_header, 
	*fin_new_header, *old_header;
int nreg, tot_reg, l, *endpoints, l_header;
region *maregion;
list_segments *segment;
list_regions *rlist, *deb_rlist = NULL, *cur_list;
if(header == NULL) return NULL;
old_header = header;
l_header=strlen(header);
if( (new_header = (char *)malloc(l_header+1)) == NULL) out_of_memory();
fin_new_header = new_header;
*new_header = 0;
while (*header!= 0) {
	if(strncmp(header,";;#",3) == 0) {
		p=strchr(header,'=') + 1;
		while(*p == ' ') p++;
		sscanf(p,"%d",&tot_reg);
		endpoints = (int *)malloc(2*tot_reg*sizeof(int));
		if(endpoints==NULL) out_of_memory();
		while(*p != ' ' && *p != '\n') p++;
		while(*p == ' ') p++;
		if( *p == '\n') { /* no region name in file */
			strcpy(reg_name, "regions");
			header = p;
			}
		else	{
			header = strchr(p,'\n');
			memcpy(reg_name, p, header - p);
			l = header - p;
			reg_name[l] = 0; 
			while(reg_name[--l]==' ') reg_name[l] = 0;
			}
		header++;
		nreg=0;
		while(nreg <= 2*tot_reg-1) {
			j = (char *)memccpy(aux, header, '\n', sizeof(aux));
			if(j == NULL) goto next_line;  
			*(j - 1) = 0;
			strcpy(separ,";, "); j=aux;
			while( (i=strtok(j,separ)) != NULL) {
				sscanf(i,"%d",&l);
				if( nreg > 0 &&  
					l <= *(endpoints+nreg-1) - nreg%2 ) {
					fprintf(stderr,
			"Region endpoints are not in increasing order: %d\n",l);
					goto next_line;
					}
				*(endpoints + nreg++) = l;
				j = NULL;
				}
			header = strchr(header,'\n') + 1;
			}
		maregion = (region *)calloc(1,sizeof(region));
		if(maregion ==  NULL) out_of_memory();
		l=strlen(reg_name);
		maregion->name = (char *)malloc(l+1);
		if(maregion->name ==  NULL) out_of_memory();
		strcpy(maregion->name, reg_name);
		for(nreg = 2*tot_reg -1; nreg >= 0; nreg -= 2) {
			segment = (list_segments *)
				malloc(sizeof(list_segments));
			if(segment ==  NULL) out_of_memory();
			segment->fin = endpoints[nreg];
			segment->debut = endpoints[nreg - 1];
			segment->next = maregion->list;
			maregion->list = segment;
			}
		free(endpoints);
		rlist = (list_regions *)malloc(sizeof(list_regions));
		if(rlist ==  NULL) out_of_memory();
		rlist->element = maregion;
		rlist->next = NULL;
		if(deb_rlist == NULL)
			deb_rlist = rlist;
		else	{
			cur_list =  deb_rlist;
			while(cur_list->next != NULL)
				cur_list = cur_list->next;
			cur_list->next = rlist;
			}
		continue;
		}
	else	{
		j=(char *)memccpy(fin_new_header, header, '\n', l_header);
		fin_new_header += (j - fin_new_header);
		}
next_line:
	header = strchr(header,'\n') + 1;
	}
*fin_new_header = 0;
strcpy(old_header, new_header);
free(new_header);
return deb_rlist;
}


void draw_region_background(SEA_VIEW *view, int f_seq0, int l_seq0)
{
list_segments *segment;
int nb_sites, debut, der_site, h, x, y, w, seqnum, y0;
Fl_Widget *ob = view->DNA_obj;

if(view->active_region == NULL || view->numb_gc == 1) return;
segment = view->active_region->list;
if(segment == NULL) return;
der_site = view->first_site + view->tot_sites - 1;
if(view->mod_seq == 0) { /* toutes les seqs */
	h = FL_min(view->tot_lines, view->tot_seqs - view->first_seq + 1) *
		view->line_height;
	y0 = view->y_seq - view->line_height + fl_descent();
	f_seq0 = l_seq0 = 0;
	}
else if(view->mod_seq == -1) { /* les seqs selectionnees seulement */
	h = view->line_height;
	y0 = view->y_seq - view->line_height + fl_descent() +
		(f_seq0 + 1 - view->first_seq) * view->line_height;
	}
else	{ /* la seq mod_seq seulement */
	h = view->line_height;
	y0 = view->y_seq - view->line_height + fl_descent() +
		(view->mod_seq - view->first_seq) * view->line_height;
	f_seq0 = l_seq0 = 0;
	}
fl_color(ob->selection_color());
do	{
	if(segment->debut > der_site) break;
	if(segment->fin < view->first_site) continue;
	debut = FL_max(segment->debut, view->first_site);
	nb_sites = FL_min(segment->fin, der_site) - debut + 1;
	x = view->x_seq + (debut - view->first_site) * view->char_width;
	y = y0;
	w = nb_sites * view->char_width;
	for(seqnum = f_seq0; seqnum <= l_seq0; seqnum++) {
		if(view->mod_seq != -1 || view->sel_seqs[seqnum])
			fl_rectf( x, y, w, h);
		y += view->line_height;
		}
	}
while( (segment = segment->next) != NULL );
}


const char *create_species_set(SEA_VIEW *view, const char *set_name)
{
int newnumb, i;
if(view->numb_species_sets >= MAX_SPECIES_SETS)
	return "Cannot create more sets";
/* le nom existe-t-il deja? */
for(i=0; i< view->numb_species_sets; i++) {
	if(strcmp(set_name, view->name_species_sets[i]) == 0) 
		return "Name already used";
	}
newnumb = view->numb_species_sets + 1;
view->list_species_sets[newnumb-1] = (int *)calloc(view->tot_seqs, sizeof(int));
if(view->list_species_sets[newnumb-1] == NULL)
	return "Not enough memory";
view->name_species_sets[newnumb-1] = 
	(char *)calloc(strlen(set_name)+1, sizeof(char));
if(view->name_species_sets[newnumb-1] == NULL)
	return "Not enough memory";
memcpy(view->list_species_sets[newnumb-1], view->sel_seqs, 
	view->tot_seqs * sizeof(int) );
strcpy(view->name_species_sets[newnumb-1], set_name);
view->numb_species_sets = newnumb;
return NULL;
}


void species_menu_callback(Fl_Widget *ob, void *extra)
{
SEA_VIEW *view;
  Fl_Menu_* menu = (Fl_Menu_*)ob;
const char *set_name;
const char *message;
int i, rang, numset, taille;

view = (SEA_VIEW *)ob->user_data();
int reponse = menu->mvalue() - view->menu_species->get_menu();
  taille = view->menu_species->vlength();
  rang = view->menu_species->value();
if (reponse == 0) { /* create new set of species */
	if(view->tot_sel_seqs == 0) return;
	set_name = (char *)fl_input("New species set name?", "good seqs");
	if(set_name == NULL) return;
	if( (message = create_species_set(view, set_name) ) != NULL ) {
		fl_alert("%s\n%s", message, set_name);
		return;
		}
  view->menu_species->add(set_name, species_menu_callback, NULL, FL_MENU_TOGGLE);
  view->menu_species->value(++taille);
  view->menu_species->get_menu()->deactivate();
  (view->menu_species->get_menu()+1)->activate();
	}
else if(reponse == 1) { /* effacer un set */
  Fl_Menu_Item *items = view->menu_species->get_menu();
	items[1].deactivate();
	if(rang == 0) return; /* par securite */
	numset = rang - 1;
  view->menu_species->remove(numset);
	items[0].deactivate();
	free(view->list_species_sets[numset]);
	free(view->name_species_sets[numset]);
	memcpy(view->list_species_sets + numset,
		view->list_species_sets + numset + 1,
		(view->numb_species_sets - numset - 1)*sizeof(int *) );
	memcpy(view->name_species_sets + numset,
		view->name_species_sets + numset + 1,
		(view->numb_species_sets - numset - 1)*sizeof(char *) );
	(view->numb_species_sets)--;
	view->tot_sel_seqs = 0;
	memset(view->sel_seqs, 0, view->tot_seqs * sizeof(int));
	view->DNA_obj->redraw();
	}
else	{ /* choix d'un set courant */
	int minvis, maxvis, visible = FALSE, premier = 0;
  rang = menu->mvalue() - view->menu_species->vitem(0) + 1;
	memcpy(view->sel_seqs, view->list_species_sets[rang-1], 
		view->tot_seqs * sizeof(int) );
	view->tot_sel_seqs = 0;
	minvis = view->first_seq; maxvis = minvis + view->tot_lines - 1;
	for(i=0; i< view->tot_seqs; i++) 
		if(view->sel_seqs[i]) {
			++(view->tot_sel_seqs);
			if(!visible) visible = (i+1 >= minvis && i+1 <= maxvis);
			if(premier == 0) premier = i + 1;
			}
	if(premier != 0 && !visible ) {
		int maxi;
  		maxi = (int)((Fl_Slider *)view->vertsli)->maximum();
		if(premier > maxi) premier = maxi;
		view->first_seq = premier;
		((Fl_Slider *)view->vertsli)->value(premier);
		}
	view->DNA_obj->redraw();
  view->menu_species->value(rang);
  (view->menu_species->get_menu()+1)->activate();
	adjust_menu_edit_modes(view);
	}
}


vlength_menu::vlength_menu(Fl_Menu_ *m, const char *name, const Fl_Menu_Item *init, int clength)
{
  menu_ = m;
  offset = clength;
  vlen = 0;
  val = 0;
  items = (Fl_Menu_Item*)malloc((offset+1)*sizeof(Fl_Menu_Item));
  memcpy(items, init, offset*sizeof(Fl_Menu_Item));
  memset(items + offset, 0, sizeof(Fl_Menu_Item));
  rank = menu_->add(name, 0, NULL, items, FL_SUBMENU_POINTER);
}

void vlength_menu::add(const char *txt, Fl_Callback *cb, void *data, int flags)
{
  items = (Fl_Menu_Item*)realloc(items, (offset + vlen + 2)*sizeof(Fl_Menu_Item));
  ((Fl_Menu_Item*)(menu_->menu() + rank))->user_data( items );
  memset(items + offset + vlen, 0, 2*sizeof(Fl_Menu_Item));
  (items + offset + vlen)->label( strdup(txt) );
  (items + offset + vlen)->callback(cb, data);
  (items + offset + vlen)->flags = flags;
  vlen++;
}

void vlength_menu::remove(int rank)
{
  if (rank >= vlen) return;
  free( (void*)vitem(rank)->label() );
  memmove(items + offset + rank, items + offset + rank + 1, (vlen - rank)*sizeof(Fl_Menu_Item));
  vlen--;
}


Fl_Menu_Item* vlength_menu::find_item(const char* txt)
{
  for (int i = 0; i < vlen; i++) {
    if (strcmp(vitem(i)->label(), txt) == 0) return vitem(i);
    }
  return NULL;
}

void vlength_menu::value(int v) 
{
  if (v) vitem(v - 1)->setonly();
  else if (val) vitem(val - 1)->clear();
  val = v;
}

void vlength_menu::vclear() 
{
  for (int i = 0; i < vlen; i++) {
    free( (void*)(items+offset+i)->label() );
  }
  vlen = 0;
  val = 0;
  memset(items + offset, 0, sizeof(Fl_Menu_Item));
}

vlength_menu::~vlength_menu()
{
  for (int i = 0; i < vlen; i++) {
    free( (void*)(items+offset+i)->label() );
    }
  free(items);
  ((Fl_Menu_Item*)(menu_->menu() + rank))->user_data( NULL );
  ((Fl_Menu_Item*)(menu_->menu() + rank))->flags = 0;
}

int parse_species_sets_from_header(char *header, int **list_species_sets,
	char **name_species_sets, int totseqs)
/* parse the species sets information from header lines
and remove it from them
*/
{
char *p, set_name[200], aux[200], separ[5], *i, *j, *new_header, 
	*fin_new_header, *old_header;
int numset, tot_spec, l, l_header, num;
if(header == NULL) return 0;
old_header = header;
l_header=strlen(header);
if( (new_header = (char *)malloc(l_header+1)) == NULL) out_of_memory();
fin_new_header = new_header;
*new_header = 0;
numset = -1;
while (*header!= 0) {
	if(strncmp(header,";;@ of species =", 16) == 0) {
		numset++;
		p=strchr(header,'=') + 1;
		while(*p == ' ') p++;
		sscanf(p,"%d",&tot_spec);
		list_species_sets[numset] = (int *)calloc(totseqs, sizeof(int));
		if(list_species_sets[numset]==NULL) out_of_memory();
		while(*p != ' ' && *p != '\n') p++;
		while(*p == ' ') p++;
		if( *p == '\n') { /* no set name in file */
			strcpy(set_name, "species set");
			header = p;
			}
		else	{
			header = strchr(p,'\n');
			memcpy(set_name, p, header - p);
			l = header - p;
			set_name[l] = 0; 
			while(set_name[--l]==' ') set_name[l] = 0;
			}
		l = (int) strlen(set_name);
		name_species_sets[numset] = (char *)malloc(l+1);
		if(name_species_sets[numset] == NULL) out_of_memory();
		memcpy(name_species_sets[numset], set_name, l+1);
		header++;
		num = 0;
		while(num < tot_spec) {
			j = (char *)memccpy(aux, header, '\n', sizeof(aux));
			if(j == NULL) goto next_line;  
			*(j - 1) = 0;
			strcpy(separ,";, "); j=aux;
			while( (i=strtok(j,separ)) != NULL) {
				sscanf(i,"%d",&l);
				if( l>= 1 && l <= totseqs)
					list_species_sets[numset][l-1] = 1;
				num++;
				j = NULL;
				}
			header = strchr(header,'\n') + 1;
			}
		continue;
		}
	else	{
		j=(char *)memccpy(fin_new_header, header, '\n', l_header);
		fin_new_header += (j - fin_new_header);
		}
next_line:
	header = strchr(header,'\n') + 1;
	}
*fin_new_header = 0;
strcpy(old_header, new_header);
free(new_header);
return numset + 1;
}
