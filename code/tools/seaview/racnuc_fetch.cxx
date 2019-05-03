#include "seaview.h"
#include <ctype.h>
#include <FL/Fl_Check_Button.H>

extern "C" { 
#include "raa_acnuc.h"
}

struct runningw_struct {
	raa_db_access *raa; 
	SEA_VIEW *view;
	FILE *infile; 
	int listrank;
	int elt;
	int count, interrupted, use_spec_name, truncated;
	Fl_Box *runb; 
};

/* local functions */
int racnuc_fetch(const char *db_name, const char *name, FILE *list, int use_spec_name, SEA_VIEW *view,
				 Fl_Window *, Fl_Menu_Item *feature_item, const char *scan_word, 
		 Fl_Widget *, Fl_Button*);
int racnuc_fetch_file_or_list(FILE *list, int listrank, 
							   int use_spec_name, raa_db_access *raa, SEA_VIEW *view);
static void idle_callback(void *data);
void racnuc_dialog(SEA_VIEW *view);
void use_key_callback(Fl_Widget *ob, void *data);
void dbchoice_callback(Fl_Widget *ob, void *data);
void acnuc_url_callback(Fl_Widget *ob, void *data);
void ok_callback(Fl_Widget *ob, void *data);
void fsel_callback(Fl_Widget *ob, void *data);
void more_callback(Fl_Widget *ob, void *data);
void close_cb(Fl_Widget *ob, void *data);
raa_db_access *open_acnuc_socket(Fl_Widget *, Fl_Button*);
char *import_one_seq(raa_db_access *raa, const char *id, int use_spec_name, int *plength, 
	int *ptruncated, SEA_VIEW *view, int try_as_keyword, Fl_Menu_Item *feature_item, const char *scan_word);
int import_one_keyword(raa_db_access *raa, const char *keyword, int use_spec_name,  
				int *ptruncated, SEA_VIEW *view, Fl_Menu_Item *feature_item, const char *scan_word);
static int scan_list(raa_db_access *raa, int mylist, const char *scan_word, const char *feature_name);


extern void interrupt_callback(Fl_Widget *ob, void *data);
extern int compute_size_params(SEA_VIEW *view, int force_recompute);
extern void update_comment_gc(int seqnum, int ncbigc, SEA_VIEW *view);
extern char *get_res_value(const char *name, const char *def_value);
extern int set_res_value(const char *name, const char *value);
extern int compute_wid_names(SEA_VIEW *view, int totseqs);
extern int fl_create_thread(void (*f) (void *), void* p, unsigned stack_size_KB);
extern "C" int get_ncbi_gc_number(int gc);

/* globals */
static const char def_acnuc_server[] = "pbil.univ-lyon1.fr:5558";

int racnuc_fetch(const char *db_name, const char *name, FILE *listfile, int use_spec_name, 
				 SEA_VIEW *view, Fl_Window *w, Fl_Menu_Item *feature_item, const char *scan_word,
		 Fl_Widget *ok, Fl_Button *close_b)
/*
return value: 0 iff OK
*/
{
raa_db_access *raa; 
int err, length, truncated = FALSE;

raa = open_acnuc_socket(ok, close_b);
if(raa == NULL) return 1;
err = raa_opendb(raa, (char *)db_name); 
if(err) {
	if(listfile != NULL) fclose(listfile);
	raa_acnucclose(raa);
	fl_message("DataBase %s is offline. Please try again later.", db_name);
	return 1;
	}

if(view->masename == NULL) {
	const char *p = fl_input("Sequence alignment name ?", name);
	if(p == NULL) p = "newseqs";
	view->masename = strdup(p);
	}
err = 0;
my_watch_cursor(w);

if(listfile == NULL) {
	if(import_one_seq(raa, name, use_spec_name, &length, &truncated, view, TRUE, 
					  feature_item, scan_word) == NULL) {
		fl_reset_cursor(w);
		if(feature_item == NULL) fl_message("Sequence/Keyword %s not found in %s", name, db_name);
		else if (scan_word != NULL) fl_message("No feature key matches %s", scan_word);
		else fl_message("Sequence %s or feature key not found.", name);
		}
	}
else {
	truncated = racnuc_fetch_file_or_list(listfile, 0, use_spec_name, raa, view);
	}
raa_acnucclose(raa);
view->wid_names = compute_wid_names(view, view->tot_seqs);
compute_size_params(view, TRUE);
view->DNA_obj->damage(FL_DAMAGE_ALL);
view->horsli->damage(FL_DAMAGE_ALL);
view->vertsli->damage(FL_DAMAGE_ALL);
fl_reset_cursor(w);
if(truncated)
	fl_message("Some seqs have been truncated to the length of %d", view->max_seq_length);
return err;
}


int racnuc_fetch_file_or_list(FILE *listfile, int listrank, int use_spec_name, raa_db_access *raa, 
							   SEA_VIEW *view)
{
	runningw_struct runningw_data;
	Fl_Window *runw = new Fl_Window(330,60);
	runw->label("importing sequences");
	runningw_data.runb = new Fl_Box(FL_UP_FRAME, 5, 5, 320, 20, "");
	Fl_Button *runi = new Fl_Button(130,30, 60, 20, "Interrupt");
	runi->callback(interrupt_callback, &(runningw_data.interrupted));
	runw->end();
	runw->position((Fl::w() - runw->w())/2, (Fl::h() - runw->h())/2);
	runw->set_modal();
	runw->callback(interrupt_callback, &(runningw_data.interrupted));
	runw->show();
#ifndef MICRO
	runw->hotspot(runw);
#endif
	runningw_data.raa=raa;
	if(listfile != NULL) runningw_data.infile=listfile;
	else  {
		runningw_data.listrank = listrank;
		runningw_data.infile = NULL;
		runningw_data.elt = 1;
		}
	runningw_data.interrupted = FALSE;
	runningw_data.truncated = FALSE;
	runningw_data.use_spec_name = use_spec_name;
	runningw_data.view = view;
	runningw_data.count = 0;
	Fl::add_idle(idle_callback, &runningw_data);
	if(listfile != NULL) {
		while(runningw_data.infile != NULL) Fl::wait();
		}
	else  {
		while(runningw_data.listrank != 0) Fl::wait();
		}
	delete runw;
	return runningw_data.truncated;
}


static void idle_callback(void *data)
{
static char aux[150];
char line[100], *p, *mnemo;
int length, doit, next;
runningw_struct *runningw_data = (runningw_struct *)data;

doit = FALSE;
if(!runningw_data->interrupted) {
	if(runningw_data->infile != NULL) {
		if( fgets(line, sizeof(line), runningw_data->infile) != NULL ) doit = TRUE;
		}
	else if((next = raa_nexteltinlist(runningw_data->raa, runningw_data->elt, runningw_data->listrank, 
								 &mnemo, NULL)) != 0) {
		strcpy(line, mnemo);
		runningw_data->elt = next;
		doit = TRUE;
		}
	}
if(doit) {
	p = line + strlen(line) - 1;
	if(p > line && *p == '\n') *p = 0;
	mnemo = import_one_seq(runningw_data->raa, line, runningw_data->use_spec_name, &length, 
		&runningw_data->truncated, runningw_data->view, FALSE, 0, NULL);
	if(mnemo != NULL) 
		sprintf(aux, "Name: %s Count: %d", mnemo, ++runningw_data->count);
	else
		sprintf(aux, "Name: %s not found", line);
	runningw_data->runb->label(aux);
	runningw_data->runb->window()->redraw();
	}
else {
	Fl::remove_idle(idle_callback, data);
	if(runningw_data->infile != NULL){
		fclose(runningw_data->infile);
		runningw_data->infile = NULL;
		}
	runningw_data->listrank = 0;
	}
}

void raa_win_cb(Fl_Widget *ob, void *data)
{
  Fl_Button *b = (Fl_Button*)data;
  b->do_callback();
}

struct raa_dialog_struct {
	SEA_VIEW *view;
	Fl_Input *seq_name_field;
	Fl_Choice *dbchoice;
	Fl_Choice *namingchoice;
	Fl_Check_Button *use_key;
	Fl_Choice *key_choice;
	Fl_Input *scan_field;
	Fl_Return_Button *ok;
	Fl_Button *close_b;
};


void racnuc_dialog(SEA_VIEW *view)
{
static Fl_Window *load_form = NULL;
static raa_dialog_struct raa_fields;

if(load_form == NULL) {
	int newx, x, w;
	load_form = new Fl_Window(450,190);
	load_form->box(FL_FLAT_BOX);
	load_form->label("Database Sequence Import");
	
	raa_fields.dbchoice = new Fl_Choice(80, 5, 105, 20, "Database:");
	raa_fields.dbchoice->add("embl|genbank|swissprot");
	raa_fields.dbchoice->value(0);
	raa_fields.dbchoice->callback(dbchoice_callback, &raa_fields);
	fl_font(raa_fields.dbchoice->labelfont(), raa_fields.dbchoice->labelsize());
	
	raa_fields.namingchoice = new Fl_Choice(300, 5, 145, 20, "Name using:");
	raa_fields.namingchoice->add("ID-LOCUS name|Species name");
	raa_fields.namingchoice->value(0);
	
	new Fl_Box(FL_UP_FRAME, 5, 30, 440, 60, "");

	x = 205; w = 175;
	raa_fields.seq_name_field = new Fl_Input(0, 35, w, 20, "Name/Accession no./Keyword:");
	newx = (int)fl_width(raa_fields.seq_name_field->label()) + 10 + 3;
	raa_fields.seq_name_field->resize(newx, raa_fields.seq_name_field->y(),
		x - newx + w, raa_fields.seq_name_field->h() );
	raa_fields.ok = new Fl_Return_Button(390, 35, 50, 20 , "OK");
	raa_fields.ok->callback(ok_callback, &raa_fields);
	
	raa_fields.use_key = new Fl_Check_Button(10, 65, 20, 20, "");
	raa_fields.use_key->callback(use_key_callback, &raa_fields);
	raa_fields.key_choice = new Fl_Choice(0, raa_fields.use_key->y(), 100, 20, "Feature key:");
	x = raa_fields.use_key->x() + raa_fields.use_key->w() + 
		(int)fl_width(raa_fields.key_choice->label()) + 3 + 3;
	raa_fields.key_choice->position(x, raa_fields.key_choice->y());
	raa_fields.key_choice->deactivate();
	
	raa_fields.scan_field = new Fl_Input(0, raa_fields.key_choice->y(), 1, 20, "Matching string:");
	newx = raa_fields.key_choice->x() + raa_fields.key_choice->w() + 
		(int)fl_width(raa_fields.scan_field->label()) + 6;
	raa_fields.scan_field->resize(newx, raa_fields.scan_field->y(),
									  440 - newx, raa_fields.scan_field->h() );
	raa_fields.scan_field->deactivate();
	
	new Fl_Box(FL_DOWN_FRAME, 220, 95, 20, 20, "or");
	
	new Fl_Box(FL_UP_FRAME, 5, 120, 440, 30, "");
	
	Fl_Box *b = new Fl_Box(FL_NO_BOX, 35, 125, 150, 20, "File of names/acc. nos.:");
	newx = b->x() + (int)fl_width(b->label()) + 5;
	Fl_Button *fsel = new Fl_Button(0, 125, 1, 20 , "Browse for file");
	fsel->resize(newx, fsel->y(), (int)fl_width(fsel->label()) + 8, fsel->h() );
	fsel->callback(fsel_callback, &raa_fields);

	raa_fields.close_b = new Fl_Button(380, 160, 65, 20 , "Close");
	raa_fields.close_b->callback(close_cb, NULL);
	raa_fields.close_b->shortcut(FL_COMMAND | 'w');
	
	Fl_Check_Button *more = new Fl_Check_Button(5, 160, 135, 20 , "More Databases");
	more->callback(more_callback, &raa_fields);
	
	Fl_Button *acnuc_url_button = new Fl_Button(more->x() + more->w() + 5, more->y(), 200, more->h(), 
									 "Set DB server address");
	acnuc_url_button->callback(acnuc_url_callback, NULL);
	
	load_form->end();
	load_form->position((Fl::w() - load_form->w())/2, 
		(Fl::h() - load_form->h())/2);
	load_form->set_modal();
	load_form->hotspot(load_form);
	load_form->callback(raa_win_cb, raa_fields.close_b);
	}
raa_fields.seq_name_field->value("");
raa_fields.seq_name_field->take_focus();
raa_fields.view = view;
load_form->show();
}


void use_key_callback(Fl_Widget *ob, void *data)
{
	raa_db_access *raa;
	char *name;
	int num, total;
	raa_dialog_struct *raa_fields = (raa_dialog_struct *)data;
	raa_fields->key_choice->clear();
	raa_fields->scan_field->value(NULL);
	if(((Fl_Button *)ob)->value() ) {
		raa = open_acnuc_socket(raa_fields->ok, raa_fields->close_b);
		if(raa == NULL || raa_opendb(raa, (char *)raa_fields->dbchoice->text()) || raa->swissprot) {
			((Fl_Button *)ob)->value(0);
			if(raa) raa_acnucclose(raa);
			return;
			}
		my_watch_cursor(ob->window());
		total = raa_read_first_rec(raa, raa_smj);
		for(num = 2; num <= total; num++) {
			name = raa_readsmj(raa, num, NULL, NULL);
			if(strncmp(name, "04", 2) != 0) continue;
			if(strcmp(name, "04ID") == 0) continue;
			if(strcmp(name, "04LOCUS") == 0) continue;
			int item_rank = raa_fields->key_choice->add(name + 2, 0, NULL, NULL);
      ((Fl_Menu_Item*)(raa_fields->key_choice->menu() + item_rank))->argument(num);
			}
		raa_fields->key_choice->value(0);
		raa_fields->key_choice->activate();
		raa_fields->scan_field->activate();
		raa_acnucclose(raa);
		fl_reset_cursor(ob->window());
		}
	else {
		raa_fields->key_choice->deactivate();
		raa_fields->scan_field->deactivate();
		}
}


void dbchoice_callback(Fl_Widget *ob, void *data)
{
	raa_dialog_struct *raa_fields = (raa_dialog_struct *)data;
	raa_fields->use_key->value(0);
	raa_fields->key_choice->clear();
	raa_fields->key_choice->deactivate();
	raa_fields->scan_field->deactivate();
	if( strcmp( ((Fl_Choice *)ob)->text(), "swissprot") == 0) raa_fields->use_key->deactivate();
	else raa_fields->use_key->activate();
}


void acnuc_url_callback(Fl_Widget *ob, void *data)
{
	const char *url2 = fl_input("Enter address of remote acnuc server\n"
								"(currently %s)", def_acnuc_server, get_res_value("racnuc", def_acnuc_server) );
	if(url2 != NULL) set_res_value("racnuc", url2);
}


void ok_callback(Fl_Widget *ob, void *data)
{
	raa_dialog_struct *raa_fields = (raa_dialog_struct *)data;
	Fl_Menu_Item *feature_item;
	const char *scan_word = NULL;
	if(strlen(raa_fields->seq_name_field->value()) == 0) return;
	int use_spec_name = raa_fields->namingchoice->value() == 1;
	if(raa_fields->use_key->value() ) {
		feature_item = (Fl_Menu_Item *)raa_fields->key_choice->mvalue();
		scan_word = raa_fields->scan_field->value();
		if(scan_word != NULL) {
			const char *p = scan_word;
			while(*p == ' ') p++;
			if(*p == 0) scan_word = NULL;
			else scan_word = p;
			}
	}
	else feature_item = NULL;
	racnuc_fetch(raa_fields->dbchoice->text(), raa_fields->seq_name_field->value(), NULL, 
		use_spec_name, raa_fields->view, raa_fields->dbchoice->window(), feature_item, scan_word, 
		     raa_fields->ok, raa_fields->close_b);
}


void fsel_callback(Fl_Widget *ob, void *data)
{
#define MESSAGE "Select file of list of seq. names"
	raa_dialog_struct *raa_fields = (raa_dialog_struct *)data;

	Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
	chooser->type(Fl_Native_File_Chooser::BROWSE_FILE);   // let user browse a single file
	chooser->title(MESSAGE);                        
	char *filename = run_and_close_native_file_chooser(chooser);
	if(filename == NULL) return;
	FILE *list = fopen(filename, "r");
	int use_spec_name = raa_fields->namingchoice->value() == 1;
	if(list != NULL) {
		racnuc_fetch(raa_fields->dbchoice->text(), NULL, list, use_spec_name, raa_fields->view,
					 raa_fields->dbchoice->window(), 0, NULL, raa_fields->ok, raa_fields->close_b);
		}
}


void more_callback(Fl_Widget *ob, void *data)
{
raa_dialog_struct *raa_fields = (raa_dialog_struct *)data;
raa_db_access *raa;
char **names, **descriptions;
int num, tot;
Fl_Check_Button *b = (Fl_Check_Button *)ob;

if(b->value()) {
	raa = open_acnuc_socket(raa_fields->ok, raa_fields->close_b);
	if(raa == NULL) {
	  b->value(0);
	  return;
	  }
	b->deactivate();
	tot = raa_knowndbs(raa, &names, &descriptions); 
	for(num = 0; num < tot; num++) {
		raa_fields->dbchoice->add(names[num], 0, NULL, 0, 0);
		}
	raa_acnucclose(raa);
	}
}

struct intr_struct {
  char *server_ip;
  int port;
  raa_db_access *raa;
  int err;
  bool killed;
  bool done;
};

void close_cb(Fl_Widget *ob, void *data)
{
  if (data == NULL) {
    ob->window()->hide();
  }
  else {
    intr_struct *ss = (intr_struct*)data;
    ss->killed = true;
    ss->done = true;
    ob->user_data(NULL);
  }
}

static void do_nothing_error_mess_proc(raa_db_access* raa, char* message)
{
}

static void open_socket_thread(intr_struct *ss)
//runs in a background thread
{
  raa_error_mess_proc = do_nothing_error_mess_proc;
  ss->err = raa_open_socket(ss->server_ip, ss->port, (char *)"seaview", &(ss->raa));
  if (ss->killed) {
    delete ss;
    return;
  }
  Fl::lock(); 
  ss->done = true;
  Fl::awake();
  Fl::unlock();
}
			
raa_db_access *open_acnuc_socket(Fl_Widget *ok, Fl_Button *close_b)
// attempt to open remote acnuc socket using interruptible thread
// returns NULL iff error or interrupted
{
  raa_db_access *raa = NULL;
  char *server_ip;
  int port, err;
  char *url = get_res_value("racnuc", def_acnuc_server);
  err = raa_decode_address(url, &server_ip, &port, NULL);
  if (!err) {
    intr_struct *ss = new intr_struct;
    ss->server_ip = server_ip;
    ss->port = port;
    ss->raa = NULL;
    ss->done = false;
    ss->killed = false;
    ss->err = 0;
    my_watch_cursor(ok->window());
    close_b->user_data(ss);
    close_b->label("Interrupt");
    ok->deactivate(); 
    if (fl_create_thread((void(*)(void*))open_socket_thread, (void*)ss, 0) ) {
      open_socket_thread(ss); // run in main thread if child thread creation failed
      }
    else {
      while ( !ss->done ) Fl::wait(); // wait for end/kill of child thread
      }
    fl_reset_cursor(ok->window());
    ok->activate(); 
    close_b->user_data(NULL);
    close_b->label("Close");
    if (ss->killed) {
      fl_alert("%s", "Connection attempt was cancelled");
      delete ss;
      return NULL;
      }
    err = ss->err;
    raa = ss->raa;
    delete ss;
  }
  if (err) {
    fl_alert("Can't connect to remote acnuc server:\n%s", url);
    raa = NULL;
    }
  return raa;
}


/*static void interrupt_callback(Fl_Widget *ob, void *data)
{
*(int *)data = TRUE;
}*/


char *import_one_seq(raa_db_access *raa, const char *id, int use_spec_name, int *plength, 
	int *ptruncated, SEA_VIEW *view, int try_as_keyword, Fl_Menu_Item *feature_item, const char *scan_word)
{
char *mnemo, *acc, *specname, *descr, *p, *seq;
int length, frame, gencode, rank, type, pext, num, next, list1, matchinglist, count, err;
static char line[500];

mnemo = raa_getattributes(raa, id, &rank, &length, &frame, &gencode, &acc, &descr, &specname, 
						  feature_item == NULL ? &seq : NULL);
if(mnemo == NULL) {
	if(try_as_keyword) {
		return (import_one_keyword(raa, id, use_spec_name, ptruncated, view, 
								   feature_item, scan_word) ? NULL : (char *)id);
		}
	else return NULL;
	}
if(feature_item != NULL) {
	if(scan_word != NULL) {
		sprintf(line, "n=%s and t=%s", mnemo, feature_item->label());
		err = raa_proc_query(raa, line, NULL, (char *)"mylist", &list1, &count, NULL, NULL);
		if(err || count == 0) return NULL;
		matchinglist = scan_list(raa, list1, scan_word, feature_item->label() );
		if(matchinglist == 0) return NULL; 
		racnuc_fetch_file_or_list(NULL, matchinglist, use_spec_name, raa, view);
		return (char *)"OK";
		}
	else {
		mnemo = raa_readsub(raa, rank, NULL, &type, &pext, NULL, NULL, NULL, NULL);
		int desired_type = (int)feature_item->argument();
		if(type != desired_type ) {
			next = -pext;
			while(next > 0) {
				next = raa_readlng(raa, next);
				for (num = 0; num < raa->SUBINLNG; num++) {
					rank = raa->rlng_buffer->sub[num];
					if(rank == 0) break;
					mnemo = raa_readsub(raa, rank, NULL, &type, NULL, NULL, NULL, NULL, NULL);
					if(type == desired_type) break; 
					}
				if(type == desired_type) break; 
				}
			if(type != desired_type)  return NULL;
			}
		}
	mnemo = raa_getattributes(raa, mnemo, NULL, &length, &frame, &gencode, &acc, &descr, &specname, &seq);
	}
gencode = get_ncbi_gc_number(gencode);
if(frame == 1) {
	seq -= 2; /* this terrible thing is possible after raa_getattributes */
	length += 2;
	*seq = '-'; *(seq + 1) = '-';
	}
else if(frame == 2) {
	seq -= 1; length += 1;
	*seq = '-'; 
	}
add_seq_to_align(view, use_spec_name ? specname : mnemo, seq, length);
*plength = length;
if(view->comments == NULL) view->comments = (char **)calloc(view->tot_seqs, sizeof(char *));
p = view->comments[view->tot_seqs-1];
*line = 0;
if(p != NULL && strcmp(p, ";\n") != 0) strcpy(line, p);
if(p != NULL) free(p);
p = strchr(descr, ' '); /* skip 1st word (name) from descr */
if(p != NULL) p++; else p = descr;
sprintf(line + strlen(line), ";%s\n", p);
p = line + strlen(line);
if(use_spec_name) 
	sprintf(p, ";Name=%s\n;Acc=%s\n", mnemo, acc);
else 
	sprintf(p, ";/organism=\"%s\"\n;Acc=%s\n", specname, acc);
if(frame != 0 && strstr(line, "/codon_start=") == NULL) sprintf(line + strlen(line), 
	";/codon_start=%d\n", frame + 1);
int l = strlen(line);
p = (char *)malloc(l + 1);
if(p != NULL) strcpy(p, line);
view->comments[view->tot_seqs-1] = p;
if(view->comments != NULL && gencode != 1) update_comment_gc(view->tot_seqs - 1, gencode, view);
return mnemo;
}


int import_one_keyword(raa_db_access *raa, const char *keyword, int use_spec_name, 
				int *ptruncated, SEA_VIEW *view, Fl_Menu_Item *feature_item, const char *scan_word)
/* returns 0 iff keyword exists */
{
	int numlist, err;
	char query[WIDTH_MAX + 50];
	
	sprintf(query, "k=%s", keyword);
	if(feature_item != NULL) {
		sprintf(query + strlen(query), " and t=%s", feature_item->label());
		}
	err = raa_proc_query(raa, query, NULL, (char *)"bykeyword", &numlist, NULL, NULL, NULL);
	if(err) return 1;
	if(scan_word != NULL) {
		numlist = scan_list(raa, numlist, scan_word, feature_item->label() );
		if(numlist == 0) return 1;
		}
	*ptruncated = racnuc_fetch_file_or_list(NULL, numlist, use_spec_name, raa, view);
	return 0;
}


static int scan_list(raa_db_access *raa, int mylist, const char *scan_word, const char *feature_name)
// returns rank of modified list or 0 if error
{
	int err, matchinglist;
	char line[50], *p;
	sprintf(line, "prep_getannots&nl=1\n%s|%s\n", raa->embl ? "FT" : "FEATURES", feature_name);
	sock_fputs(raa, line);
	p = read_sock(raa);
	if(strncmp(p, "code=0", 6) != 0) return 0;
	err = raa_modifylist(raa, mylist, (char *)"scan", (char *)scan_word, &matchinglist, NULL, NULL);
	return (err != 0 || raa_bcount(raa, matchinglist) == 0) ? 0 : matchinglist; 
}
