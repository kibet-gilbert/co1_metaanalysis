#include "seaview.h"
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Choice.H>

void concatenate_dialog(SEA_VIEW *view);
static void cancel_callback(Fl_Widget *, void *);
void rank_name_callback( Fl_Widget *o, void *data);
void do_concatenate_callback( Fl_Widget *ob, void *data);

extern char *f_format_names[];
extern char *f_format_exts[];
extern int nbr_formats;
extern SEA_VIEW *cmdline_read_input_alignment(int argc, char **argv);
extern known_format what_format(const char *filename);
extern char *argname(int argc, char *argv[], const char *arg);
extern int isarg(int argc, char *argv[], const char *arg);
extern char *create_tmp_filename(void);
extern void delete_tmp_filename(const char *base_fname);
extern char *process_output_options(int argc, char **argv, known_format& out_format, bool& std_output);

static Fl_Round_Button *by_name;
static Fl_Choice *choice;
static int 	interrupted;
static Fl_Check_Button *addgaps;

void concatenate_dialog(SEA_VIEW *view)
{
	Fl_Window *w;
	w = new Fl_Window(305, 160);
	choice = new Fl_Choice(130,10,170, 20, "Source alignment");
	choice->align(FL_ALIGN_LEFT);
	Fl_Window *w2 = Fl::first_window();
	int count = 0;
	while(w2 != NULL) {
		const char *wclass = w2->xclass();
		if(w2 != view->dnawin && wclass != NULL && strcmp(wclass, SEAVIEW_WINDOW) == 0) {
			choice->add(w2->label());
			Fl_Menu_Item *menu = (Fl_Menu_Item *)choice->menu();
			(menu + count)->user_data(w2->user_data());
			count++;
			}
		w2 = Fl::next_window(w2);
	   }
	if(choice->size() <= 1) {
		w->end();
		delete w;
		fl_alert("Concatenation requires more than one alignment window");
		return;
		}
	choice->value(0);
	static char title[100];
	sprintf(title, "Target alignment: %s", view->dnawin->label());
	Fl_Box *target = new Fl_Box(10, 40, w->w() - 20, 20, title);
	target->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
	Fl_Group *g = new Fl_Group(0, 70, w->w(), 40);
	by_name = new Fl_Round_Button(10, 75, 80, 20, "by name");
	by_name->value(1);
	by_name->type(FL_RADIO_BUTTON);
	by_name->callback(rank_name_callback, NULL);
	Fl_Round_Button *rank = new Fl_Round_Button(100, 75, 80, 20, "by rank");
	rank->type(FL_RADIO_BUTTON);
	rank->callback(rank_name_callback, NULL);
	g->end();
	addgaps = new Fl_Check_Button(10, 100, 100, 20, "add gaps");
	Fl_Button *cancel = new Fl_Button(10, 125, 50, 20, "Cancel");
	cancel->callback(cancel_callback, NULL);
	Fl_Return_Button *ok = new Fl_Return_Button(w->w() - 60, 125, 50, 20, "OK");
	ok->callback(do_concatenate_callback, view);
	w->end();
	w->callback(cancel_callback, NULL);
	w->show();
#ifndef MICRO
	w->hotspot(w);
#endif
	interrupted = FALSE;
	while(!interrupted) Fl::wait();
	w->hide();
	Fl::delete_widget(w);
}

static void cancel_callback( Fl_Widget *ob, void *data)
{
	interrupted = TRUE;
}

void rank_name_callback( Fl_Widget *o, void *data)
{
	if(o == by_name) addgaps->activate(); 
	else {
		addgaps->value(0);
		addgaps->deactivate();
	}
}

//concatenate target = target + source
static void concatenate(SEA_VIEW *target, SEA_VIEW *source, bool want_addgaps, bool is_by_name, bool no_gui)
{
  int i, j, l, *found, target_length = target->seq_length;
  char *gaps;
  if (is_by_name) {//concatenation by name
    if (want_addgaps) {
      l = FL_max(target_length, source->seq_length);
      gaps = (char *)malloc(l + 1);
      memset(gaps, '-', l);
      found = (int *)calloc(target->tot_seqs + source->tot_seqs, sizeof(int));
    }
    for (i = 0; i < source->tot_seqs; i++) {
      for (j = 0; j < target->tot_seqs; j++) if(strcmp(source->seqname[i], target->seqname[j]) == 0) break;
      if (j >= target->tot_seqs) {//seq in source absent from target
        if ( !want_addgaps ) continue;
          //add a gap-only sequence to target
        add_seq_to_align(target, source->seqname[i], gaps, target_length, no_gui);
        j = target->tot_seqs - 1;
        if (source->comments[i] != NULL) target->comments[j] = strdup(source->comments[i]);
      }
      l = target->each_length[j] + source->each_length[i];
      target->sequence[j] = (char *)realloc(target->sequence[j], l + 1);
      memcpy(target->sequence[j] + target->each_length[j], source->sequence[i], source->each_length[i]);
      target->each_length[j] = l;
      if (l > target->seq_length) target->seq_length = l;
      target->sequence[j][l] = 0;
      if (want_addgaps) found[j] = TRUE;
    }
    if (want_addgaps) {
      for (j = 0; j < target->tot_seqs; j++) {
        if (found[j]) continue;
        //seqs from target only are concatenated with a gap-only sequence
        l = target->each_length[j] + source->seq_length;
        target->sequence[j] = (char *)realloc(target->sequence[j], l + 1);
        memset(target->sequence[j] + target->each_length[j], '-', source->seq_length);
        target->sequence[j][l] = 0;
        target->each_length[j] = l;
      }
      free(gaps); 
      free(found); 
    }
  }
  else {//concatenation by rank
    for (i = 0; i < source->tot_seqs; i++) {
      if (i >= target->tot_seqs) break;
      l = strlen(target->sequence[i]) + strlen(source->sequence[i]);
      target->sequence[i] = (char *)realloc(target->sequence[i], l + 1);
      strcat(target->sequence[i], source->sequence[i]);
      target->each_length[i] = l;
      if (l > target->seq_length) target->seq_length = l;
    }
  }
}


void do_concatenate_callback( Fl_Widget *ob, void *data)
{
  int i;
  SEA_VIEW *target = (SEA_VIEW *)data;

  interrupted = TRUE;
  i = choice->value();
  Fl_Menu_Item *item = (Fl_Menu_Item *)choice->menu() + i;
  SEA_VIEW *source = (SEA_VIEW *)item->user_data();
  char *oldname = strdup(target->masename);
  concatenate(target, source, addgaps->value(), by_name->value(), false);
  free(target->masename);
  free(target->each_length);
  for (i = 0; i < target->tot_seqs; i++) free(target->col_rank[i]);
  free(target->col_rank);
  init_dna_scroller(target, target->tot_seqs, oldname, target->protein, target->header);	
  free(oldname);
  set_seaview_modified(target, TRUE);
}

void store_concat_part_as_region(SEA_VIEW *target, char *name, int from, int to)
{
  list_segments ls = {from, to, NULL};
  list_segments *pls = new list_segments; *pls = ls;
  region *r = new region;
  r->name = strdup(extract_filename(name));
  r->list = pls;
  list_regions *list = target->regions;
  list_regions *list2 = new list_regions;
  list2->element = r;
  list2->next = NULL;
  if (list) {
    while (list->next) list = list->next;
    list->next = list2;
  } else target->regions = list2;
}

void concatenate_no_gui(int argc, char *argv[])
{
  char *err_message;
  bool std_output = false;
  SEA_VIEW *target = cmdline_read_input_alignment(argc, argv);
  bool record_partition = isarg(argc, argv, "-record_partition");

  if (record_partition) store_concat_part_as_region(target, argv[argc-1], 1, target->seq_length);
  char *all_sources = argname(argc, argv, "-concatenate");
  known_format out_format = target->format_for_save;
  
  char *source_name;
  source_name = strtok(all_sources, ",");
  bool is_by_rank = isarg(argc, argv, "-by_rank");
  while (source_name) {
    SEA_VIEW *source = cmdline_read_input_alignment(1, &source_name);
    int old_length = target->seq_length + 1;
    concatenate(target, source, !is_by_rank, !is_by_rank, true);
    if (record_partition) store_concat_part_as_region(target, source_name, old_length, target->seq_length);
    source_name = strtok(NULL, ",");
    }
  
  char *outfile = process_output_options(argc, argv, out_format, std_output);

  err_message = save_alignment_or_region(outfile, target->sequence, target->comments,
					 target->header, target->seqname, target->tot_seqs, target->each_length,
					 target->regions, NULL, out_format,
					 target->numb_species_sets, target->list_species_sets,
					 target->name_species_sets, NULL, 0, target->protein,
					 target->tot_comment_lines, target->comment_name, 
					 target->comment_line, target->phylipwidnames,
					 target->tot_trees, target->trees, 
					 target->menu_trees->vitem(0), target->spaces_in_fasta_names);
  if (err_message) {
    fprintf(stderr, "%s\n", err_message);
    if (std_output) delete_tmp_filename(outfile);
    exit(1);
  }
  if (std_output) {
    char line[200];
    FILE *in = fopen(outfile, "r");
    while (fgets(line, sizeof(line), in) != NULL) {
      fputs(line, stdout);
    }
    fclose(in);
    fflush(stdout);
    delete_tmp_filename(outfile);
  }
  exit(0);
}

