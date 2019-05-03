#include "seaview.h"
#include <ctype.h>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Paged_Device.H>

void custom_callback(Fl_Widget *obj, void *data);
void my_colorchooser(Fl_Widget *obj, void *data);
void my_colorchooser_callback(Fl_Widget *obj, void *data);
void slider_cb(Fl_Widget *obj, void *data);
void ignore_cb(Fl_Widget *obj, void *data);
void accept_cb(Fl_Widget *obj, void *data);
void permanent_cb(void *data);
void reset_color_callback(Fl_Widget *obj, void *data);
void accept_color_callback(Fl_Widget *obj, void *data);
void reset_callback(Fl_Widget *obj, void *data);
void codon_color_chooser_cb(Fl_Widget *obj, void *data);
int is_view_valid(SEA_VIEW *view);


/* private global variables */
static Fl_Button *A, *C, *G, *T;
static Fl_Slider *bgrnd, *s_bgrnd;
static Fl_Input *protgroups, *s_protgroups, *movekeys;
static Fl_Button **protcol;
static Fl_Check_Button *lowercase, *inverted, *fastaspaces;
static Fl_Int_Input *maxnamewidth;
static Fl_Choice *pdfformat, *saveformat, *alignment_algo, *fontsize;
static Fl_Value_Input *pdffontsize;
static Fl_Value_Input *phylipwidnames;

/* extern variables & functions */
extern int *def_protcolors, max_protcolors;
extern char def_stdcolorgroups[];
extern int numb_stdprotcolors, numb_altprotcolors;
extern color_choice curr_color_choice;
extern char *f_format_names[];
extern int nbr_formats;
extern Fl_Paged_Device::Page_Format printout_pageformat;
extern int printout_fontsize;
extern void mod_multipl(SEA_VIEW *view, int newval);
extern void set_aa_color_mode(color_choice choice);
extern color_choice prep_aa_color_code(char *list_std, char *list_alt, 
	int maxprotcolors, int *numb_stdprotcolors, int *numb_altprotcolors);
extern int set_res_value(const char *name, const char *value);
extern int save_resources(void);
extern void minuscules(char *);
extern char *get_res_value(const char *name, const char *def_value);
extern int int_res_value(const char *name, int def_value);
extern char aminoacids[]; // from misc_acnuc.c


class coloraas : public Fl_Widget {
	void draw(void);
public:
	int codoncolors[22];
	coloraas(int x,int y,int w,int h, int *in_codoncolors) : Fl_Widget(x,y,w,h) 
	{
		memcpy(codoncolors, in_codoncolors, sizeof(codoncolors));
	}
};


void custom_callback(Fl_Widget *obj, void *data)
{
static Fl_Window *win = NULL;
static Fl_Button *permanent, *accept, *ignore;
static coloraas *mycoloraas;
int x, y, w, h, c;
SEA_VIEW *view = (SEA_VIEW *)obj->user_data();
Fl_Button *reset;
  const Fl_Menu_Item *item;
  
if(win == NULL) {
	win = new Fl_Window(310, 640, "Customization");
	
	x = 9; y = 20; w = 160; h = 20;
	bgrnd = new Fl_Slider(x,y,w,h, "background grey");
	bgrnd->type(FL_HOR_NICE_SLIDER);
	bgrnd->bounds(32., 55.);
	bgrnd->color2( FL_BLACK );
	bgrnd->precision(0);
	bgrnd->align(FL_ALIGN_TOP);
	bgrnd->callback(slider_cb);
	y += bgrnd->h() + 25;

	s_bgrnd = new Fl_Slider(x,y,w,h, "sites background grey");
	s_bgrnd->type(FL_HOR_NICE_SLIDER);
	s_bgrnd->bounds(32., 55.);
	s_bgrnd->color2( FL_BLACK );
	s_bgrnd->precision(0);
	s_bgrnd->align(FL_ALIGN_TOP);
	s_bgrnd->callback(slider_cb);
	
	reset = new Fl_Button(bgrnd->x() + bgrnd->w() + 40, s_bgrnd->y() - 20, 50, 20, "reset");
	reset->color(FL_LIGHT2);
	reset->callback(reset_callback, bgrnd);
	new Fl_Box(FL_DOWN_FRAME, 5, 3,
		win->w() - 8, 90 , "");
	
	y += s_bgrnd->h() + 20;
	w=40; h=w;
	A = new Fl_Button(x, y, w, h, "A");
	A->callback(my_colorchooser );
	
	x += A->w() + 10;
	C = new Fl_Button(x, y, w, h, "C");
	C->callback(my_colorchooser );

	x += A->w() + 10;
	G = new Fl_Button(x, y, w, h, "G");
	G->callback(my_colorchooser );

	x += A->w() + 10;
	T = new Fl_Button(x, y, w, h, "T/U");
	T->callback(my_colorchooser );
	
	x += A->w() + 30;
	reset = new Fl_Button(x, y + 10, 50, 20, "reset");
	reset->color(FL_LIGHT2);
	reset->callback(reset_callback, A);
	new Fl_Box(FL_DOWN_FRAME, 5, A->y() - 5,
		win->w() - 8, A->h() + 10 , "");
	
	x = A->x();
	y += A->h() + 35;
	int topbox = y - 25;
	protgroups = new Fl_Input(x,y,280, 20, "aa coloring scheme");
	protgroups->align(FL_ALIGN_TOP);

	y += protgroups->h() + 20;
	s_protgroups = new Fl_Input(x,y,280, 20, "alternate aa coloring scheme");
	s_protgroups->align(FL_ALIGN_TOP);
	
	y += s_protgroups->h() + 10;
	new Fl_Box(x, y, win->w(), 20, "catalog of amino acid colors");
	y += 20;
	w = h = 25;
	protcol = (Fl_Button **)malloc(max_protcolors * sizeof(Fl_Button *));
	for(c=1; c < max_protcolors; c++) {
		protcol[c] = new Fl_Button(x, y, w, h, "");
		protcol[c]->callback(my_colorchooser);
		x += w + 5;
		}
	y += h + 10;
	reset = new Fl_Button(win->w() / 2 - 25, y, 50, 20, "reset");
	reset->color(FL_LIGHT2);
	reset->callback(reset_callback, protgroups);
	new Fl_Box(FL_DOWN_FRAME, 5, topbox,
		win->w() - 8, y + reset->h() + 5 - topbox, "");
	
	topbox = reset->y() + reset->h() + 10;
	Fl_Box *frame = new Fl_Box(FL_DOWN_FRAME, 5, topbox,
			   win->w() - 8, 50, "");
	Fl_Box *box = new Fl_Box(FL_NO_BOX, 0, topbox, win->w(), 20, "Synonymous codon colors");
	box->align(FL_ALIGN_CENTER);
	mycoloraas = new coloraas(10, topbox + 20, win->w() - 10, 20, view->codoncolors);
	mycoloraas->labelfont(FL_COURIER);
	mycoloraas->labelsize(18);
	Fl_Button *codons = new Fl_Button(win->w() - 65, topbox + 20, 60, 20, "Change");
	codons->callback(codon_color_chooser_cb, mycoloraas);
	codons->clear_visible_focus();
	
	x = A->x();
	y = frame->y() + frame->h() + 25;
	movekeys = new Fl_Input(x,y, 50, 20, "right-left move keys");
	movekeys->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
	reset = new Fl_Button(x + movekeys->w() + 5, y, 50, 20, "reset");
	reset->color(FL_LIGHT2);
	reset->callback(reset_callback, movekeys);

	w = 130;
	x = win->w() - w - 5;
	saveformat = new Fl_Choice(x,y,w,h, "default save format");
	saveformat->align(FL_ALIGN_TOP);
	for(c=0; c < nbr_formats; c++) saveformat->add(f_format_names[c]);
	saveformat->selection_color(saveformat->color());
	
	x = A->x();
	y += movekeys->h() + 10; 
	lowercase = new Fl_Check_Button(x,y,140,20, "display lowercase");

	y += lowercase->h(); 
	inverted = new Fl_Check_Button(x,y,140,20, "inverted");
  
	y += inverted->h();
	fastaspaces = new Fl_Check_Button(x,y,170,20, "spaces in FASTA names");
	fastaspaces->labelsize(13);

	x = win->w() - w - 5;
	y = movekeys->y() + movekeys->h() + 20;
	
	pdfformat = new Fl_Choice(x,y,w/2,h, PDF_OR_PS_EXT" page");
	pdfformat->align(FL_ALIGN_TOP_LEFT);
	pdfformat->add("A4|LETTER");
	pdfformat->selection_color(pdfformat->color());
	pdfformat->labelsize(10);
	pdfformat->textsize(10);
	
	pdffontsize = new Fl_Value_Input(x+w/2,y,w/2,h, PDF_OR_PS_EXT" fontsize");
	pdffontsize->align(FL_ALIGN_TOP_RIGHT);
	pdffontsize->labelsize(10);
  
	y += pdffontsize->h() + 15; 
	maxnamewidth = new Fl_Int_Input(x, y, w, 20, "max name width");
	maxnamewidth->labelsize(13);
	maxnamewidth->align(FL_ALIGN_TOP);

	x = A->x();
	y = fastaspaces->y() + fastaspaces->h() + 25; 
	alignment_algo = new Fl_Choice(x,y,w,h, "alignment algorithm");
	alignment_algo->align(FL_ALIGN_TOP);
  
	y = alignment_algo->y();
	x = pdfformat->x();
	phylipwidnames = new Fl_Value_Input(x,y,w,h, "phylip names width");
	phylipwidnames->align(FL_ALIGN_TOP);
	phylipwidnames->labelsize(13);
  
  x = phylipwidnames->x();
  y = phylipwidnames->y() + phylipwidnames->h() + 15;
  fontsize = new Fl_Choice(x,y,w,h, "sequence font size");
  fontsize->align(FL_ALIGN_TOP_LEFT);
  item = view->menu_props;
  while ((++item)->label()) {
    fontsize->add(item->label());
    }

	x = A->x();
	y += fontsize->h() + 10;
	h = 40;
	w = 95;
	accept = new Fl_Button(x,y,w,h, "Apply");
	accept->tooltip("changes apply only to current seaview run");
	accept->color(FL_LIGHT2);
	x += w + 5;
	permanent = new Fl_Button(x,y,w,h, "Set changes\npermanent");
	permanent->tooltip("changes apply to current & future seaview runs");
	permanent->color(FL_LIGHT2);
	w = 45;
	y += 10;
	ignore = new Fl_Button(win->w() - w - 5,y,w,20, "Close");
	ignore->shortcut(FL_COMMAND | 'w');
	ignore->color(FL_LIGHT2);
	win->end();
	win->resizable(NULL);
	}

mycoloraas->user_data(view);
accept->callback(accept_cb, mycoloraas);
bgrnd->value( view->DNA_obj->color() );
bgrnd->color( view->DNA_obj->color() );
s_bgrnd->value( view->DNA_obj->color2() );
s_bgrnd->color( view->DNA_obj->color2() );
A->color(view->dnacolors[1], view->dnacolors[1]);
C->color(view->dnacolors[2], view->dnacolors[2]);
G->color(view->dnacolors[3], view->dnacolors[3]);
T->color(view->dnacolors[4], view->dnacolors[4]);
char *p, aagroups[30];
p = aagroups;
set_aa_color_mode(USING_STANDARD_COLORS);
for(c=1; c < view->numb_stdprotcolors; c++) {
	for(char aa='A'; aa<='Z';aa++) {
		if(get_color_for_aa(aa) == c) *(p++) = aa;
		}
	if(c+1< view->numb_stdprotcolors) strcpy(p++, ",");
	}
*p = 0;
protgroups->value(aagroups);
if(view->numb_altprotcolors <= 1) s_protgroups->value("");
else {
	p = aagroups;
	set_aa_color_mode(USING_ALT_COLORS);
	for(c=1; c < view->numb_altprotcolors; c++) {
		for(char aa='A'; aa<='Z';aa++) {
			if(get_color_for_aa(aa) == c) *(p++) = aa;
			}
		if(c+1< view->numb_altprotcolors) strcpy(p++, ",");
		}
	*p = 0;
	s_protgroups->value(aagroups);
	}
if(view->curr_colors == view->stdprotcolors) set_aa_color_mode(USING_STANDARD_COLORS);
else set_aa_color_mode(USING_ALT_COLORS);

for(c=1; c < max_protcolors; c++) {
	protcol[c]->color(view->stdprotcolors[c], view->stdprotcolors[c]);
	}
movekeys->value( view->movekeys );
lowercase->value(view->allow_lower);
inverted->value(view->inverted_colors);
fastaspaces->value(view->spaces_in_fasta_names);
maxnamewidth->value(get_res_value("maxnamewidth", "20"));
pdfformat->value(printout_pageformat == Fl_Paged_Device::A4 ? 0 : 1);
pdffontsize->value(printout_fontsize);
phylipwidnames->value(view->phylipwidnames);
saveformat->value(view->format_for_save);
Fl_Menu_Item *menu_align = (Fl_Menu_Item *)view->menu_align;
int clustalopt = ALIGN_OPTIONS + 1;
alignment_algo->clear();
for(x = 0; x < view->count_msa_algos; x++) {
	item = (menu_align + clustalopt + x);
	alignment_algo->add( item->label() );
	}	
alignment_algo->value(view->alignment_algorithm);
  fontsize->value(3);
  item = view->menu_props;
  while((++item)->label()) {
    int fsize;
    sscanf(item->label(), "%d", &fsize);
    if (fsize == view->DNA_obj->labelsize()) {
      fontsize->value(item - view->menu_props - 1);
      break;
      }
  }
  
win->show();
#ifndef MICRO
win->hotspot(win);
#endif
  while (TRUE) {
    Fl_Widget* obj = Fl::readqueue();
    if (!obj) Fl::wait();
    else if (obj == ignore || obj == permanent || obj == win) {
      if (obj == permanent) permanent_cb(mycoloraas);
      win->hide();
      break;
    }
  }
}


void ignore_cb(Fl_Widget *obj, void *data)
{
obj->window()->hide();
}


void accept_cb(Fl_Widget *obj, void *data)
{
coloraas *mycoloraas = (coloraas *)data;
SEA_VIEW *view = (SEA_VIEW *)mycoloraas->user_data();
if(!is_view_valid(view)) return;
  Fl_Menu_Item *menu_props = (Fl_Menu_Item *)view->menu_props;
props_menu_parts *props_parts = (props_menu_parts *)menu_props->user_data();
char *p;
Fl_Menu_Item *item;

my_watch_cursor(view->dnawin);
memcpy(view->codoncolors, mycoloraas->codoncolors, sizeof(mycoloraas->codoncolors));
	
view->DNA_obj->color((int)(bgrnd->value() + 0.5));
view->DNA_obj->color2((int)(s_bgrnd->value() + 0.5));
view->DNA_obj->parent()->color( view->DNA_obj->color() );
view->dnacolors[1] = A->color();
view->dnacolors[2] = C->color();
view->dnacolors[3] = G->color();
view->dnacolors[4] = T->color();
for(int c=1; c < max_protcolors; c++) view->stdprotcolors[c] = protcol[c]->color();

p = strdup(protgroups->value());
majuscules(p);
protgroups->value(p);
free(p);
p = strdup(s_protgroups->value());
majuscules(p);
s_protgroups->value(p);
free(p);
curr_color_choice = prep_aa_color_code((char *)protgroups->value(), (char *)s_protgroups->value(), max_protcolors, 
	&numb_stdprotcolors, &numb_altprotcolors);
view->numb_stdprotcolors = numb_stdprotcolors;
view->numb_altprotcolors = numb_altprotcolors;
  if (view->numb_altprotcolors > 1) (menu_props + props_parts->colors + 2)->activate();
  else (menu_props + props_parts->colors + 2)->deactivate();
if(view->protein) {
	view->alt_colors = (color_choice)-1; /* to force redraw of sequences */
	item = menu_props + props_parts->colors + 1;
  item->setonly();
  view->menubar->value(item);
  item->callback()(view->menubar, props_parts);
	if(view->numb_altprotcolors > 1) view->alt_colors = USING_STANDARD_COLORS;
	else view->alt_colors = NO_ALT_COLORS;
	}

if(strlen(movekeys->value()) != 4)  movekeys->value(view->movekeys);
else {
	strcpy(view->movekeys, movekeys->value() ); 
	mod_multipl(view, view->multipl->argument() );
	view->multipl->redraw();
	}

item = menu_props + props_parts->allow_lower;
  view->allow_lower = lowercase->value();
if(view->allow_lower) item->set();
else item->clear();

item = menu_props + props_parts->inverted;
view->inverted_colors =! inverted->value(); /* because callback toggles it */
  view->menubar->value(item);
  Fl_Callback1* cb = (Fl_Callback1*)item->callback();
  cb(view->menubar, props_parts->inverted);
if(view->inverted_colors) item->set();
else item->clear();
  
view->spaces_in_fasta_names = fastaspaces->value();
  
set_res_value("maxnamewidth", ((Fl_Input*)maxnamewidth)->value());

printout_pageformat = pdfformat->value() == 0 ? Fl_Paged_Device::A4 : Fl_Paged_Device::LETTER;
printout_fontsize = (int)pdffontsize->value();
view->phylipwidnames = (int)phylipwidnames->value();
if(view->phylipwidnames < 10) {
	view->phylipwidnames = 10;
	phylipwidnames->value(10);
	}

view->format_for_save = (known_format)saveformat->value();

view->alignment_algorithm = alignment_algo->value();
Fl_Menu_Item *menu_align = (Fl_Menu_Item *)view->menu_align;
int clustalopt = ALIGN_OPTIONS + 1;
item = menu_align + clustalopt;
(item + view->alignment_algorithm)->setonly();
  
  item = (Fl_Menu_Item*)view->menu_props + fontsize->value() + 1;
  item->setonly();
  view->menubar->value(item);
  item->callback()(view->menubar, NULL);

if(!view->protein) view->DNA_obj->redraw();
fl_reset_cursor(view->dnawin);
}


void permanent_cb(void *data)
{
char txt[150], *p;
int v, i;
uchar r, g, b;
coloraas *mycoloraas = (coloraas *)data;

accept_cb(NULL, data);

v = (int)(bgrnd->value() + 0.5);
sprintf(txt, "%d", v );
set_res_value("background", txt );
v = (int)(s_bgrnd->value() + 0.5);
sprintf(txt, "%d", v );
set_res_value("sites_background", txt );

Fl::get_color( A->color(), r, g, b);
sprintf(txt, "%d %d %d,", r, g, b);
Fl::get_color( C->color(), r, g, b);
sprintf(txt + strlen(txt), "%d %d %d,", r, g, b);
Fl::get_color( G->color(), r, g, b);
sprintf(txt + strlen(txt), "%d %d %d,", r, g, b);
Fl::get_color( T->color(), r, g, b);
sprintf(txt + strlen(txt), "%d %d %d", r, g, b);
set_res_value("dnacolors", txt);

set_res_value("stdcolorgroups", (char *)protgroups->value());
set_res_value("altcolorgroups", (char *)s_protgroups->value());

txt[0] = 0;
for(int c=1; c < max_protcolors; c++) {
	Fl::get_color( protcol[c]->color(), r, g, b);
	sprintf(txt + strlen(txt), "%d %d %d,", r, g, b);
	}
txt[strlen(txt) - 1] = 0;
set_res_value("protcolors", txt);

set_res_value("movekeys", movekeys->value() );
set_res_value("lowercase", lowercase->value() ? "true" : "false");
set_res_value("inverted", inverted->value() ? "true" : "false");
set_res_value("spacesinfastanames", fastaspaces->value() ? "true" : "false");
set_res_value("printoutpageformat", printout_pageformat == Fl_Paged_Device::A4 ? "A4" : "LETTER" );
sprintf(txt, "%d", (int)pdffontsize->value() );
set_res_value("printoutfontsize",  txt);

sprintf(txt, "%d", (int)phylipwidnames->value() );
set_res_value("phylipwidnames",  txt);

strcpy(txt, f_format_names[saveformat->value()] );
minuscules(txt);
set_res_value("save", txt );

v = alignment_algo->value();
sprintf(txt, "%d", v );
set_res_value("alignment", txt);
	
p = txt;
for(i = 0; i < 21; i++) {
	sprintf(p, "%c:%d,", aminoacids[i], mycoloraas->codoncolors[i+1]);
	p += strlen(p);
	}
*(p-1) = 0;
set_res_value("codoncolors", txt);
  
set_res_value("sequencefontsize", fontsize->text());

save_resources();
}


void reset_callback(Fl_Widget *obj, void *data)
{
if(data == A) {
	A->color(FL_RED, FL_RED);
	C->color(FL_GREEN, FL_GREEN);
	G->color(FL_YELLOW, FL_YELLOW);
	T->color(FL_BLUE, FL_BLUE);
	}
else if(data == bgrnd) {
	int back_color, region_back_color;
#ifdef WIN32
	back_color = FL_DARK1; region_back_color = 43;
#else
	back_color = FL_GRAY; region_back_color = FL_DARK2;
#endif
	bgrnd->value( back_color );
	bgrnd->color( back_color );
	s_bgrnd->value( region_back_color );
	s_bgrnd->color( region_back_color );
	}
else if(data == movekeys) {
	movekeys->value(MOVEKEYS_DEFAULT);
	}
else if(data == protgroups) {
	protgroups->value(def_stdcolorgroups);
	s_protgroups->value("");
	for(int c=1; c < max_protcolors; c++) {
		protcol[c]->color(def_protcolors[c], def_protcolors[c]);
		}
	}
obj->window()->redraw();
}


void my_colorchooser(Fl_Widget *obj, void *data)
{
static Fl_Window *win = NULL;
static Fl_Color_Chooser *ch;
uchar r, g, b;
static Fl_Color initial;

if(win == NULL) {
	win = new Fl_Window(205, 125, "Color chooser");
	ch = new Fl_Color_Chooser(1, 1, 200, 95);
	ch->callback(my_colorchooser_callback);
	Fl_Button *reset = new Fl_Button(5, 100, 40, 20, "reset");
	reset->callback(reset_color_callback, ch);
	Fl_Button *ok = new Fl_Button(160, 100, 40, 20, "OK");
	ok->callback(accept_color_callback, ch);
	win->end();
	win->resizable(NULL);
	win->set_modal();
	win->callback(reset_color_callback, ch);
}
if(win->shown()) return;
Fl::get_color( obj->color(), r, g, b);
ch->rgb(r/255., g/255., b/255.);
ch->user_data(obj);
obj->user_data(&initial);
initial = obj->color();
win->show();
return;
}


void my_colorchooser_callback(Fl_Widget *obj, void *data)
{
int r, g, b;
Fl_Button *but = (Fl_Button *)data;
Fl_Color_Chooser *ch = (Fl_Color_Chooser *)obj;
r = (int)(ch->r() * 255 + 0.5);
g = (int)(ch->g() * 255 + 0.5);
b = (int)(ch->b() * 255 + 0.5);
Fl_Color c = fl_rgb_color(r, g, b);
but->color(c, c);
but->redraw();
}


void reset_color_callback(Fl_Widget *obj, void *data)
{
Fl_Color_Chooser *ch = (Fl_Color_Chooser *)data;
Fl_Widget *from = (Fl_Widget *)(ch->user_data());
Fl_Color c = *(Fl_Color *)(from->user_data());
uchar r, g, b;

from->color(c, c);
from->redraw();
if(obj->window() == NULL) ((Fl_Window *)obj)->hide();
else {
	Fl::get_color( c, r, g, b);
	ch->rgb(r/255., g/255., b/255.);
	}
}


void accept_color_callback(Fl_Widget *obj, void *data)
{
obj->window()->hide();
}


void slider_cb(Fl_Widget *obj, void *data)
{
Fl_Valuator *v = (Fl_Valuator *)obj;
v->color( (int)(v->value() + 0.5) );
}


class colorcell : public Fl_Widget {
    FL_EXPORT void draw(void);
    FL_EXPORT int handle(int);
public:
    FL_EXPORT colorcell(int x,int y,int w,int h) :
	Fl_Widget(x,y,w,h,NULL) {
		this->copy_label(" ");
	}
};


class codon_color_chooser : public Fl_Window
	{
	public:
		Fl_Group *g;
		colorcell *selected;
		coloraas *mycoloraas;
		codon_color_chooser(coloraas *mycoloraas);
	private:
		static void cancel_cb(Fl_Widget *ob, void *data);
		static void new_colors_cb(Fl_Widget *w, void *data);
		static void reset_cb(Fl_Widget *w, void *data);
		static void defaults_cb(Fl_Widget *w, void *data);
	};


int colorcell::handle(int event) 
{
	char *aa, *p;
	int key, before, i;
	Fl_Group *g;
	colorcell *cell;
	codon_color_chooser *mychooser = (codon_color_chooser *)this->window();

    switch (event)
    {
		case FL_RELEASE:
			if(mychooser->selected) {
				mychooser->selected->redraw();
				}
			mychooser->selected = this;
			this->redraw();
			Fl::focus(this);
			break;
		case FL_KEYBOARD:
			if( Fl::event_state(FL_COMMAND) ) return 0; // can be shortcut of other widgets
			if(Fl::event_length() == 0) break;
			key = Fl::event_text()[0];
			key = toupper(key);
			if((aa = strchr(aminoacids, key)) == NULL) break;
			if(*aa == 'X') break;
			p = (char *)this->label();
			before = *p;
			*p = *aa;
			this->redraw();
			g = parent();
			for(i=0; i < 256; i++) {
				cell = (colorcell *)g->child(i);
				p = (char *)cell->label();
				if(*p == key && cell != this) {
					*p = before;
					cell->redraw();
					break;
					}
				}
			break;
		case FL_FOCUS:
			break;
		default:
			return 0;
	}
	return 1;
}


void colorcell::draw(void) 
{
	codon_color_chooser *mychooser = (codon_color_chooser *)this->window();
	fl_color(color());
	fl_rectf(x(), y(), w(), h());
	fl_color(FL_BLACK);
	if(*label() != ' ') {
		fl_font(labelfont(), labelsize());
		fl_draw(label(), 1, x() + w()/2 - (int)fl_width(label())/2, y() + h()/2 + fl_size()/3 );
		}
	if(mychooser->selected == this) {
		fl_line_style(0, 3);
		fl_rect(x() + 2, y() + 2, w()-4, h()-4);
		fl_line_style(0, 0);
		}
}


void decode_codon_colors(int *colors)
// puts desired colorindex in colors[1..21] in the order of aminoacids table
{
	const char *q;
	char *p, *codestring;
	int i, val, retval = 0;
	
	codestring = get_res_value("codoncolors", NULL);
	if(codestring != NULL) {
//codestring contains R:10,W:155,*:5,....
		p = codestring;
		for(i = 1; i <= 21; i++) {
			q = strchr(aminoacids, *p);
			if(q == NULL || *q == 'X') {
				p = strchr(p, ',');
				if(p == NULL) break;
				p++;
				continue;
			}
			p += 2;
			sscanf(p, "%d", &val);
			colors[q - aminoacids + 1] = val;
			retval++;
			p = strchr(p, ',');
			if(p == NULL) break;
			p++;
			}
		}
	if(retval != 21) {//absence or incomplete codoncolors preference
		for(i = 1; i <= 21; i++) colors[i] = i + 72;
		return;
	}
}


void codon_color_chooser::defaults_cb(Fl_Widget *w, void *data)
{
	int aa, colrank, i;
	char *p;
	codon_color_chooser *mychooser = (codon_color_chooser *)w->window();
	colorcell *cell;
	for(i=0; i < 256; i++) {
		cell = (colorcell *)mychooser->g->child(i);
		p = (char *)cell->label();
		*p = ' ';
		}
	for(aa=1; aa <= 21; aa++) {
		colrank = aa + 72;
		cell = (colorcell *)mychooser->g->child(colrank);
		p = (char *)cell->label();
		*p = aminoacids[aa-1];
	}
	mychooser->selected = NULL;
	mychooser->g->redraw();
}


void codon_color_chooser::cancel_cb(Fl_Widget *ob, void *data)
{
	codon_color_chooser *mychooser = (codon_color_chooser *)(ob->window() ? ob->window() : ob);
	mychooser->hide();
	Fl::delete_widget(mychooser);
}


void codon_color_chooser::new_colors_cb(Fl_Widget *w, void *data)
{
	codon_color_chooser *mychooser = (codon_color_chooser *)w->window();
	char *aa;
	int rank, i;
	colorcell *cell;
	
	for(i=0; i < 256; i++) {
		cell = (colorcell *)mychooser->g->child(i);
		aa = (char *)cell->label();
		if(*aa == ' ') continue;
		rank = strchr(aminoacids, *aa) - aminoacids;
		mychooser->mycoloraas->codoncolors[rank+1] = i;
		}
	mychooser->mycoloraas->redraw();
	mychooser->do_callback();
}


void codon_color_chooser::reset_cb(Fl_Widget *w, void *data)
{
	codon_color_chooser *mychooser = (codon_color_chooser *)w->window();
	char *p;
	int aa, colrank, colors[22];
	colorcell *cell;
	
	for(colrank=0; colrank < 256; colrank++) {
		cell = (colorcell *)mychooser->g->child(colrank);
		p = (char *)cell->label();
		*p = ' ';
	}
	decode_codon_colors(colors);
	//we have now a complete list of colors
	for(aa = 1; aa <= 21; aa++) {
		cell = (colorcell *)mychooser->g->child(colors[aa]);
		p = (char *)cell->label();
		*p = aminoacids[aa-1];
		}
	mychooser->selected = NULL;
	mychooser->g->redraw();
}


void codon_color_chooser_cb(Fl_Widget *obj, void *data)
{
	codon_color_chooser *mychooser = new codon_color_chooser( (coloraas *)data );
	mychooser->show();
}


codon_color_chooser::codon_color_chooser(coloraas *in_coloraas) : Fl_Window(330, 414)
{
	int i, j;
	colorcell *box;
	
	mycoloraas = in_coloraas;
	label("Synonymous codon color chooser");
	set_modal();
	Fl_Box *b = new Fl_Box(FL_NO_BOX, 0, 0, this->w(), 60,
			"Click on color cell, and type desired\none-letter amino acid (or * for stop codons).");
	b->align(FL_ALIGN_CENTER);
	g = new Fl_Group(5, 65, 320, 320);
	for(i=0; i < 16; i++) {
		for(j=0; j < 16; j++) {
			box = new colorcell( 5 + j*20, 65 + i * 20, 20, 20);
			box->box(FL_FLAT_BOX);
			box->color(16*i + j);
			box->align(FL_ALIGN_CENTER);
			box->labelfont(FL_COURIER_BOLD);
			box->labelsize(14);
			}
		}
	g->end();
//fill cells with adequate aas
	for(i = 1; i <= 21; i++) {
		colorcell *cell = (colorcell *)g->child(mycoloraas->codoncolors[i]);
		char *p = (char *)cell->label();
		*p = aminoacids[i-1];
	}
	
	Fl_Button *cancel = new Fl_Button(5, 390, 60, 20, "Cancel");
	cancel->callback(cancel_cb, NULL);
	cancel->tooltip("don't change anything");
	cancel->shortcut(FL_COMMAND | 'w');
	Fl_Button *reset = new Fl_Button(100, 390, 60, 20, "Reset");
	reset->callback(reset_cb, NULL);
	reset->tooltip("back to previous color set");
	Fl_Button *defaults = new Fl_Button(170, 390, 70, 20, "Defaults");
	defaults->callback(defaults_cb, NULL);
	defaults->tooltip("use seaview's default color set");
	Fl_Button *ok = new Fl_Button(275, 390, 50, 20, "OK");
	ok->callback(new_colors_cb, NULL);
	ok->tooltip("accept new color set");
	end();
	callback(cancel_cb, NULL);
	selected = NULL;
}


void coloraas::draw(void)
{
	int i, wl;
	fl_font(this->labelfont(), this->labelsize());
	wl = (int)(fl_width("A") + 0.5);
	for(i=0; i < 21; i++) {
		fl_color(this->codoncolors[i + 1]);
		fl_rectf(x() + i * wl, y(), wl, h());
		}
	fl_color(FL_BLACK);
	fl_draw(aminoacids, 21, x() , y() + h()/2 + fl_size()/3 );
}


int is_view_valid(SEA_VIEW *view)
{
	Fl_Window *w = Fl::first_window();
	while(w != NULL) {
		const char *wclass = w->xclass();
		if(wclass != NULL && strcmp(wclass, SEAVIEW_WINDOW) == 0 && w->user_data() == view) return TRUE;
		w = Fl::next_window(w);
		}
	return FALSE;
}

