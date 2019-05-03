#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// included functions
void load_resources(const char *progname);
char *get_res_value(const char *name, const char *def_value);
int int_res_value(const char *name, int def_value);
int bool_res_value(const char *name, int def_value);
int set_res_value(const char *name, const char *value);
int save_resources(void);
void delete_res_value(const char *name);


#if defined(__APPLE__)
#include <ApplicationServices/ApplicationServices.h>

/* for Mac OS X */
char *get_res_value(const char *name, const char *def_value)
{
  CFStringRef cfname;
  CFPropertyListRef cfvalue;
  static char value[400];

  cfname = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingUTF8);
  if(cfname == NULL) return (char *)def_value;
  cfvalue = CFPreferencesCopyAppValue(cfname, kCFPreferencesCurrentApplication);
  CFRelease(cfname); 
  if(cfvalue == NULL) return (char *)def_value;
  Boolean ok = CFStringGetCString( (CFStringRef)cfvalue, value, sizeof(value), kCFStringEncodingUTF8);
  CFRelease(cfvalue);
  return (ok ? value : (char *)def_value);
}


int set_res_value(const char *name, const char *value)
{
  CFStringRef cfname, cfvalue;

  cfname = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingUTF8);
  if(cfname == NULL) return 1;
  cfvalue = CFStringCreateWithCString(kCFAllocatorDefault, value, kCFStringEncodingUTF8);
  if(cfvalue == NULL) return 1;
  CFPreferencesSetAppValue(cfname, cfvalue, kCFPreferencesCurrentApplication);
  CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);
  CFRelease(cfname); 
  CFRelease(cfvalue);
  return 0;
}


void delete_res_value(const char *name)
{
  CFStringRef cfname;
  cfname = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingUTF8);
  if(cfname == NULL) return;
  CFPreferencesSetAppValue(cfname, NULL, kCFPreferencesCurrentApplication);
  CFRelease(cfname); 
}


int save_resources(void)
{ // nothing to do, taken care of by the system
return 0;
}

void load_resources(const char *progname)
{ // nothing to do, taken care of by the system
return;	
}


#else /* for X11 and win32 */

typedef struct _item {
	struct _item *before;
	char *name;
	char *value;
	struct _item *next;
	} item;

static item *firstitem = NULL;

extern char *get_prog_dir(void); 


void delete_res_value(const char *name)
{
item *elt = firstitem;

while(elt != NULL) {
	if(strcmp(elt->name, name) == 0) {
		if(elt->before == NULL) {
			firstitem = elt->next;
			}
		else {
			elt->before->next = elt->next;
			}
		if(elt->next != NULL) elt->next->before = elt->before;
		free(elt->name); free(elt->value); free(elt);
		break;
		}
	elt = elt->next;
	}
return;
}


char *res_filename(void)
{
static char fname[FILENAME_MAX];
char *p;

#ifdef WIN32
p = get_prog_dir();
if(p == NULL) return NULL;
strcpy(fname, p);
strcat(fname, "\\seaview.ini");
#else
p = getenv("HOME");
if(p == NULL) return NULL;
strcpy(fname, p);
strcat(fname, "/.seaviewrc");
#endif
return fname;
}


void load_resources(const char *progname)
{
FILE *in;
char *fname, *p, line[500];
item *elt, *last = NULL;

fname = res_filename();
if(fname == NULL) return;
in = fopen(fname, "r");
if(in == NULL) return;
while((p = fgets(line, sizeof(line), in)) != NULL) {
	p = strchr(line, '\n'); if(p != NULL) *p = 0;
	p = strchr(line, '=');
	if(p == NULL) continue;
	*(p++) = 0; while(isspace(*p))  p++;
	if(*p == 0) continue;
	elt = (item *)malloc(sizeof(item));
	if(elt == NULL) break;
	elt->name = strdup(line);
	elt->value = strdup(p);
	if(elt->name == NULL || elt->value == NULL) break;
	if(last != NULL) last->next = elt;
	else firstitem = elt;
	elt->before = last;
	last = elt;
	elt->next = NULL;
	}
fclose(in);
}


char *get_res_value(const char *name, const char *def_value)
{
item *elt = firstitem;

while(elt != NULL) {
	if(strcmp(name, elt->name) == 0) return elt->value;
	elt = elt->next;
	}
return (char *)def_value;
}


int set_res_value(const char *name, const char *value)
{
item *elt, *last = NULL;

elt = firstitem;
while(elt != NULL) {
	if(strcmp(elt->name, name) == 0) {
		free(elt->value);
		elt->value = strdup(value);
		return 0;
		}
	last = elt;
	elt = elt->next;
	}
elt = (item *)malloc(sizeof(item));
if(elt == NULL) return 1;
elt->name = strdup(name);
elt->value = strdup(value);
if(elt->name == NULL || elt->value == NULL) return 1;
if(last != NULL) last->next = elt;
else firstitem = elt;
elt->before = last;
elt->next = NULL;
return 0;
}


int sort_items(const void *i1, const void *i2)
{
return strcmp( (*(item **)i1)->name, (*(item **)i2)->name);
}


int save_resources(void)
{ 
char *fname;
item *elt;
FILE *out;
int count, i;
item **table;

fname = res_filename();
if(fname == NULL) return 1;
out = fopen(fname, "w");
if(out == NULL) return 1;

elt = firstitem; count = 0;
while(elt != NULL) {
	count++;
	elt = elt->next;
	}
table = (item **)malloc(count * sizeof(item *));
if(table != NULL) {
	elt = firstitem; i = 0;
	while(elt != NULL) {
		table[i++] = elt;
		elt = elt->next;
		}
	qsort(table, count, sizeof(item *), sort_items);
	for(i = 0; i < count; i++) fprintf(out, "%s=%s\n", table[i]->name, table[i]->value);
	free(table);
	}
else	{
	elt = firstitem;
	while(elt != NULL) {
		fprintf(out, "%s=%s\n", elt->name, elt->value);
		elt = elt->next;
		}
	}
fclose(out);
return 0;
}

#endif


int int_res_value(const char *name, int def_value)
{
int val = def_value;
char *p = get_res_value(name, NULL);
if( p != NULL ) sscanf(p, "%d", &val);
return val;
}


int bool_res_value(const char *name, int def_value)
{
char *p = get_res_value(name, NULL);
if( p == NULL ) return def_value;
return (*p == 'T' || *p == 't');
}

