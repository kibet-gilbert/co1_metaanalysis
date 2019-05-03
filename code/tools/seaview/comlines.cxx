#include "seaview.h"
#include <string.h>
#include <stdlib.h>

void draw_comment_lines(Fl_Widget *ob, SEA_VIEW *view);
int parse_comment_lines_from_header(char *header, char ***plines, 
	char ***pnames, int **plengths, int *pmax_seq_length);
int insert_char_in_comment(int key, int num, SEA_VIEW *view);
int insert_gap_all_comments(int numgaps, int pos,  SEA_VIEW *view);
int delete_char_in_comment(SEA_VIEW *view, int count, int comnum, int depart, 
	int protect);
int delete_in_all_comments(int numdels, int pos,  SEA_VIEW *view);
void update_menu_footers(SEA_VIEW *view);
void footers_menu_callback(Fl_Widget *ob, void* data);

/* used prototypes */
void set_and_show_new_cursor_site(SEA_VIEW *view, int new_pos, int center,
	int force_redraw);
int compute_size_params(SEA_VIEW *view, int force_recompute);


void draw_comment_lines(Fl_Widget *ob, SEA_VIEW *view)
{
int offset, x, y, l_line, nline;
int couleur, background;

if( !view->show_comment_lines ) return;
x = view->x_seq; 
y = view->y_seq + (view->pos_first_comment_line - 1) * view->line_height;
/* write lines */
x = view->x_seq;
offset= view->first_site - 1;
for(nline = 0; 
	nline < view->tot_comment_lines ; nline++, y += view->line_height ) {
	if(view->mod_comment_line == 0 || view->mod_comment_line == nline+1 ) {
		if(view->active_comment_line == nline + 1) {
			couleur = FL_WHITE;
			background = FL_BLACK;
			}
		else	{
			couleur = FL_BLACK;
			background = ob->selection_color();
			}
		fl_font(FL_COURIER_ITALIC, ob->labelsize() );
		fl_color(background); 
		fl_rectf( view->x_name, 
			y - view->line_height + fl_descent(), 
			(view->wid_names+1) * view->char_width, 
			view->line_height);
		fl_color(couleur); 
		fl_draw(view->comment_name[nline], 
			FL_min((unsigned)view->wid_names, 
			strlen(view->comment_name[nline]) ), 
			view->x_name, y);
		l_line = ( 
		offset + view->tot_sites < view->comment_length[nline] ? 
		view->tot_sites : view->comment_length[nline] - offset);
		if(l_line <= 0) continue;
		fl_font(ob->labelfont(), ob->labelsize() );
		fl_color(ob->color()); 
		fl_rectf(x, y - view->line_height + fl_descent(), 
			(view->tot_sites + 1) * view->char_width, 
			view->line_height);
		fl_color(FL_BLACK); 
		fl_draw(view->comment_line[nline] + offset,  l_line, x, y);
		}
	}
}  


int parse_comment_lines_from_header(char *header, char ***plines, 
	char ***pnames, int **plengths, int *pmax_seq_length)
{
char *new_header, *fin_new_header, *old_header, *j, *p, *q;
int l_header, num, total, max_seq_length;
char **lines, **names;
int *lengths;

if(header == NULL) return 0;
old_header = header;
l_header = strlen(header);
total = 0;
while (*header!= 0) {
	if(strncmp(header,";;|",3) == 0) {
		total++;
		do	header = strchr(header,'\n') + 1;
		while(strncmp(header, ";;||", 4) != 0);	
		}
	header = strchr(header,'\n') + 1;
	}
if(total == 0) return 0;
names = (char **)malloc(total * sizeof(char *));
lines = (char **)malloc(total * sizeof(char *));
lengths = (int *)malloc(total * sizeof(int));
if( names == NULL || lines == NULL || lengths == NULL) out_of_memory();
header = old_header;
if( (new_header = (char *)malloc(l_header+1)) == NULL) out_of_memory();
fin_new_header = new_header;
*new_header = 0;
num = -1;
max_seq_length = *pmax_seq_length;
while (*header!= 0) {
	if(strncmp(header,";;|",3) == 0) {
		num++;
		p = header + 3; while(*p == ' ') p++;
		q = strchr(p, '\n');
		names[num] = (char *)malloc(q - p + 1);
		if( names[num] == NULL ) out_of_memory();
		memcpy( names[num], p, q - p );
		names[num][q - p] = 0;
		header = q + 1;
		p = header;
		do	p = strchr(p,'\n') + 1;
		while(strncmp(p, ";;||", 4) != 0);
		if(p - header > max_seq_length) max_seq_length = p - header;
		lines[num] = (char *)malloc(p - header + 1);
		if( lines[num] == NULL ) out_of_memory();
		q = lines[num];
		do	{
			p = strchr(header,'\n') + 1;
			memcpy(q, header + 2, p - header - 3);
			q += p - header - 3;
			header = p;
			}
		while(strncmp(p, ";;||", 4) != 0);
		*q = 0;
		lengths[num] = strlen(lines[num]);
		}
	else	{
		j=(char *)memccpy(fin_new_header, header, '\n', l_header);
		fin_new_header += (j - fin_new_header);
		}
	header = strchr(header,'\n') + 1;
	}
*fin_new_header = 0;
strcpy(old_header, new_header);
free(new_header);
*pnames = names; *plines = lines; *plengths = lengths;
*pmax_seq_length = max_seq_length;
return num + 1;
}


int insert_char_in_comment(int key, int num, SEA_VIEW *view)
{
char *pos;
int l;

if(num + view->comment_length[view->active_comment_line - 1] >
	view->max_seq_length)
	num = view->max_seq_length - 
		view->comment_length[view->active_comment_line - 1];
pos = view->comment_line[view->active_comment_line - 1] + view->cursor_site - 1;
l = view->comment_length[view->active_comment_line - 1] - view->cursor_site + 1;
memmove(pos + num, pos, l + 1);
memset(pos, key, num);
view->mod_comment_line = view->cursor_seq;
view->comment_length[view->active_comment_line - 1] += num;
set_and_show_new_cursor_site(view, view->cursor_site + num, FALSE, TRUE);
set_seaview_modified(view, TRUE);
return num;
}


int insert_gap_all_comments(int numgaps, int pos,  SEA_VIEW *view)
{
int num, l, total = 0;
char *debut;
for(num = 0; num < view->tot_comment_lines; num++) {
	if(pos > view->comment_length[num] + 1) continue; /* beyond end */
	total = numgaps;
	if(total + view->comment_length[num] > view->max_seq_length) {
		total = view->max_seq_length - view->comment_length[num];
		fl_beep(FL_BEEP_DEFAULT);
		}
	debut = view->comment_line[num] + pos - 1;
	l = view->comment_length[num] - pos + 1;
	memmove(debut + total, debut, l + 1);
	memset(debut, '-', total);
	view->comment_length[num] += total;
	}
return total;
}


int delete_char_in_comment(SEA_VIEW *view, int count, int comnum, int depart, 
	int protect)
{
char *pos;
int l, total;

if(count >= depart) count = depart - 1;
pos = view->comment_line[comnum - 1] + depart - 1;
l = view->comment_length[comnum - 1] - depart + 1;
if(protect) {
	for(total = 1; total <= count ; total++)
		if( *(pos - total) != '-' ) break;
	count = total - 1;
	if(count == 0) return 0;
	}
memmove(pos - count, pos, l + 1);
view->mod_comment_line = comnum;
view->comment_length[comnum - 1] -= count;
set_and_show_new_cursor_site(view, depart - count, FALSE, TRUE);
set_seaview_modified(view, TRUE);
return count;
}


int delete_in_all_comments(int numdels, int pos,  SEA_VIEW *view)
{
int num, l;
char *debut;
numdels = FL_min(numdels, pos);
for(num = 0; num < view->tot_comment_lines; num++) {
	if(pos > view->comment_length[num] + 1) continue; /* beyond end */
	debut = view->comment_line[num] + pos - 1;
	l = view->comment_length[num] - pos + 1;
	memmove(debut - numdels, debut, l + 1);
	view->comment_length[num] -= numdels;
	}
return numdels;
}


void update_menu_footers(SEA_VIEW *view)
{
Fl_Menu_Item *items = (Fl_Menu_Item *)view->menu_footers;
if(view->tot_comment_lines > 0) 
	items[SHOW_HIDE_FOOTERS].activate();
else	
	items[SHOW_HIDE_FOOTERS].deactivate();
  items[SHOW_HIDE_FOOTERS].label( view->show_comment_lines ? "Hide footers" : "Show footers" );
if(view->show_comment_lines && view->active_comment_line > 0)
	items[DELETE_FOOTER].activate();
else
	items[DELETE_FOOTER].deactivate();
if(view->tot_seqs > 0 )
	items[CREATE_FOOTER].activate();
else
	items[CREATE_FOOTER].deactivate();
}


void footers_menu_callback(Fl_Widget *ob, void* data)
{
SEA_VIEW *view = (SEA_VIEW *) ob->user_data();
int num, reponse = view->menubar->mvalue() - view->menu_footers;
if(reponse == SHOW_HIDE_FOOTERS && view->tot_comment_lines > 0) {
	view->show_comment_lines = !view->show_comment_lines;
	view->active_comment_line = 0;
	view->cursor_in_comment = FALSE;
	compute_size_params(view, TRUE);
	view->DNA_obj->redraw();
	}
else if(reponse == CREATE_FOOTER) {
	char **pline, **pname, *texte, *name;
	const char *c_name;
	int *plength;
	c_name = fl_input("Name of new footer line?", "Comments" );
	if(c_name == NULL || strlen(c_name) == 0) return;
	num = view->tot_comment_lines;
	pline = (char **)malloc((view->tot_comment_lines + 1) *sizeof(char *));
	if(pline == NULL) return;
	pname = (char **)malloc((view->tot_comment_lines + 1) *sizeof(char *));
	if(pname == NULL) return;
	plength = (int *)malloc((view->tot_comment_lines + 1) *sizeof(int ));
	if(plength == NULL) return;
	texte = (char *)malloc((view->max_seq_length + 1) *sizeof(char));
	if(texte == NULL) return;
	name = (char *)malloc(strlen(c_name) + 1);
	if(name == NULL) return;
	memset(texte, '-', view->seq_length);
	texte[view->seq_length] = 0;
	strcpy(name, c_name);
	if(view->tot_comment_lines > 0) {
		memcpy(pline, view->comment_line, 
			view->tot_comment_lines * sizeof(char *));
		free(view->comment_line);
		}
	pline[view->tot_comment_lines] = texte;
	view->comment_line = pline;
	if(view->tot_comment_lines > 0) {
		memcpy(pname, view->comment_name, 
			view->tot_comment_lines * sizeof(char *));
		free(view->comment_name);
		}
	pname[view->tot_comment_lines] = name;
	view->comment_name = pname;
	if(view->tot_comment_lines > 0) {
		memcpy(plength, view->comment_length, 
			view->tot_comment_lines * sizeof(int));
		free(view->comment_length);
		}
	plength[view->tot_comment_lines] = view->seq_length;
	view->comment_length = plength;
	++(view->tot_comment_lines);
	view->show_comment_lines = TRUE;
	view->active_comment_line = 0;
	view->cursor_in_comment = FALSE;
	set_seaview_modified(view, TRUE);
	compute_size_params(view, TRUE);
	view->DNA_obj->redraw();
	}
else if(reponse == DELETE_FOOTER) {
	if(view->tot_comment_lines == 0 || view->active_comment_line == 0)
		return;
	if(! fl_choice("Confirm deletion?", "Cancel", "Delete", NULL) ) return;
	free(view->comment_line[view->active_comment_line - 1]);
	free(view->comment_name[view->active_comment_line - 1]);
	for(num = view->active_comment_line; num < view->tot_comment_lines;
			num++) {
		view->comment_line[num - 1] = view->comment_line[num];
		view->comment_name[num - 1] = view->comment_name[num];
		view->comment_length[num - 1] = view->comment_length[num];
		}
	(view->tot_comment_lines)--;
	if(view->tot_comment_lines == 0) view->show_comment_lines = FALSE;
	view->active_comment_line = 0;
	view->cursor_in_comment = FALSE;
	set_seaview_modified(view, TRUE);
	compute_size_params(view, TRUE);
	view->DNA_obj->redraw();
	}
update_menu_footers(view);
}
