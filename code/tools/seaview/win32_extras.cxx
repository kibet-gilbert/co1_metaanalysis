#ifdef WIN32

#include <FL/Enumerations.H>
#include "pdf_or_ps.h"
#include <FL/fl_utf8.h>
#include <stdlib.h>
#include <stdio.h>
#  ifndef NO_PDF
#    include <pdflib.h>
#  endif

/* included functions */
void my_bell(void);
char *get_prog_dir(void);
char **getutf8args(int *pargc);
int mysystem(const char *cmd);

#if 100*FL_MAJOR_VERSION + 10*FL_MINOR_VERSION + FL_PATCH_VERSION < 134 // FLTK before 1.3.4

Copy_Surface::Copy_Surface(int w, int h) : Fl_Surface_Device(new Fl_GDI_Graphics_Driver())
{
  HDC oldflgc = fl_gc;
  //int factor = 32; // empirically set
  // exact computation of factor from screen units to EnhMetaFile units (0.01 mm)
  HDC hdc = GetDC(NULL);
  int hmm = GetDeviceCaps(hdc, HORZSIZE);
  int hdots = GetDeviceCaps(hdc, HORZRES);
  int vmm = GetDeviceCaps(hdc, VERTSIZE);
  int vdots = GetDeviceCaps(hdc, VERTRES);
  ReleaseDC(NULL, hdc);
  float factorw =  (100. * hmm) / hdots;
  float factorh =  (100. * vmm) / vdots + 0.5;
  
  RECT rect; rect.left = 0; rect.top = 0; rect.right = w * factorw; rect.bottom = h * factorh;
  fl_gc = CreateEnhMetaFile (NULL, NULL, &rect, NULL);
  if (fl_gc != NULL) {
    SetTextAlign(fl_gc, TA_BASELINE|TA_LEFT);
    SetBkMode(fl_gc, TRANSPARENT);
  } 
  else {
    fl_gc = oldflgc;
    }
}

Copy_Surface::~Copy_Surface()
{
  if(oldflgc == fl_gc) oldflgc = NULL;
  HENHMETAFILE hmf = CloseEnhMetaFile (fl_gc);
  if ( hmf != NULL ) {
    if ( OpenClipboard (NULL) ){
      EmptyClipboard ();
      SetClipboardData (CF_ENHMETAFILE, hmf);
      CloseClipboard ();
    }
    DeleteEnhMetaFile(hmf);
  }
  DeleteDC(fl_gc);
  fl_gc = oldflgc;
  delete driver();
}

#endif // FLTK before 1.3.4


char *get_prog_dir(void)
/* returns path of dir where calling program was launched */
{
	static char *dir = NULL;
	char *line, *p;
	int l;
	
	if(dir == NULL) {
		WCHAR *wp = GetCommandLineW();
		l = 4*wcslen(wp)+1;
		line = new char[l];
    char *keep = line;
		fl_utf8fromwc(line, l, wp, wcslen(wp));
		//fprintf(OUT,"retour GetCommandLine line=%s\n",line);fflush(OUT);
		/* extract 1st word of line or first group delimited by " " */
		if(*line == '"') { line++; p = strchr(line, '"'); }
		else p = strchr(line, ' '); 
		if(p == NULL) p = line + strlen(line);
		l = p - line;
		while( l > 0 && line[l - 1] != '\\' ) l--;
	  if (l == 0) {strcpy(line, ".\\"); l = 2;}
		if(l > 0) {
			dir = (char *)malloc(l + 1);
			memcpy(dir, line, l); dir[l] = 0;
		}
		delete[] keep;
	}
	return dir;
}


char **getutf8args(int *pargc)
{
	int argc, l;
	wchar_t **wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
	char **argv = (char **)malloc(argc * sizeof(char *));
	for (int count = 0; count < argc; count++) {
	    l = 4*wcslen(wargv[count])+1;
	    argv[count] = (char *)malloc(l);
	    fl_utf8fromwc(argv[count], l, wargv[count], wcslen(wargv[count]));
	}
	*pargc = argc;
	return argv;
}


int mysystem(const char *cmd)
{
  DWORD retval;
  BOOL status;
  //conversion of utf-8 cmd into a WCHAR string
  int wlen = MultiByteToWideChar(CP_UTF8, 0, cmd, -1, NULL, 0);
  WCHAR *wcmd = new WCHAR[wlen];
  MultiByteToWideChar(CP_UTF8, 0, cmd, -1, wcmd, wlen);  
  PROCESS_INFORMATION pi; 
  STARTUPINFOW si;
  ZeroMemory( &pi, sizeof(PROCESS_INFORMATION) );
  ZeroMemory( &si, sizeof(STARTUPINFOW) );
  si.cb = sizeof(STARTUPINFOW);
  si.dwFlags = STARTF_USESTDHANDLES;
  si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
  si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
  status = CreateProcessW(NULL, wcmd, 0,0,TRUE,CREATE_NO_WINDOW,0,0, &si, &pi);
  if (!status) return 1;
  do retval = WaitForSingleObject(pi.hProcess, 500);
  while (retval == WAIT_TIMEOUT);
  delete[] wcmd;
  return 0;
}


const char *win32_UTF8_to_CP1252(const char *in)
{
  static char dst[250];
  fl_utf8toa(in, strlen(in), dst, sizeof(dst));
  return (const char *)dst;
}


#ifndef NO_PDF
static size_t pdf_write_proc(PDF *p, void *data, size_t size)
{
  FILE **output = (FILE**)PDF_get_opaque(p);
  return fwrite(data, 1, size, *output);
}

// because PDF_begin_document fails on WIN32 if non ascii filename

PDF *win32_PDF_new()
{
  FILE **output = new FILE*;
  PDF *pdf = PDF_new2(0,0,0,0, output);
  if (!pdf) delete output;
  return pdf;
}

int win32_PDF_begin_document(PDF *p, const char *filename, int len, const char *optlist)
{
  FILE **output = (FILE**)PDF_get_opaque(p);
  *output = fl_fopen(filename, "wb"); // binary mode compulsory
  if (*output == NULL) return -1;
  PDF_begin_document_callback(p, pdf_write_proc, optlist);
  return 1;
}

void win32_PDF_end_document(PDF *p, const char *optlist)
{
  FILE **output = (FILE**)PDF_get_opaque(p);
#ifdef PDF_end_document
#undef PDF_end_document 
#undef PDF_delete 
#endif
  PDF_end_document(p, optlist);
  fclose(*output);
}

void win32_PDF_delete(PDF *p)
{
  FILE **output = (FILE**)PDF_get_opaque(p);
  delete output;
  PDF_delete(p);
}
#endif

/* to delete all "base_fname*" files
 */
void delete_tmp_filename(const char *base_fname)
{
  WIN32_FIND_DATAW data;
  HANDLE h;
  WCHAR fname[MAX_PATH];
  unsigned l = strlen(base_fname);
  unsigned wn = fl_utf8toUtf16(base_fname, l, NULL, 0) + 2; // Query length
  WCHAR *wbuf = new WCHAR[wn];
  wn = fl_utf8toUtf16(base_fname, l, (unsigned short*)wbuf, wn); // Convert string to WCHAR[]
  wbuf[wn] = '*'; // add '*' at end of base name
  wbuf[wn + 1] = 0;
  WCHAR *p = wbuf + wn; // find end of directory part in base_fname
  while (*p != '\\') p--;
  l = p - wbuf + 1;
  memcpy(fname, wbuf, l * sizeof(WCHAR)); // memorize directory part
  h = FindFirstFileW(wbuf, &data);
  delete[] wbuf;
  if (h  != INVALID_HANDLE_VALUE) {
    do {
      memcpy(fname + l, data.cFileName, (wcslen(data.cFileName)+1)*sizeof(WCHAR)); // build full pathname
      DeleteFileW(fname);
      }
    while (FindNextFileW(h, &data) != 0);
    FindClose(h);
    }
}

/*
 void add_to_path(char *dir)
 /* ajoute a la variable PATH la directory dir
 teste si deja dedans ou si vide *
 {
 char *buffer;
 int l;
 
 if(dir == NULL || strlen(dir) == 0) return;
 buffer = (char *)malloc(10000);
 l = GetEnvironmentVariable("PATH", buffer, 10000);
 if(l == 0) *buffer = 0;
 if(strstr(buffer, dir) != NULL) return;
 if(*buffer != 0) strcat(buffer, ";");
 strcat(buffer, dir);
 SetEnvironmentVariable("PATH", buffer);
 free(buffer);
 }
 
 
 char **split_args(char *all, int *pargc)
 {
 char *p, *q, **argv;
 int l, argc = 0;
 
 // compter les arguments
 p = all;
 while(*p != 0) {
 argc++;
 if(*p == '"') p = strchr(p+1, '"');
 else p = strchr(p, ' ');
 if(p == NULL) break;
 p++;
 while(*p == ' ') p++;
 }
 argv = (char **)malloc(argc * sizeof(char *));
 *pargc = argc;
 // separer les arguments, enlever les "" encadrantes si presentes
 p = all; argc = 0;
 while(*p != 0) {
 if(*p == '"') { p++; q = strchr(p, '"'); }
 else q = strchr(p, ' ');
 if(q == NULL) q = p + strlen(p);
 l = q - p; 
 argv[argc] = (char *)malloc(l + 1);
 memcpy(argv[argc], p, l); argv[argc][l] = 0;
 if(*q == 0) break;
 p = q + 1;
 while(*p == ' ') p++;
 argc++;
 }
 return argv;
 }
 
 int mysystem(const char *command)
 /*
 command contains "cmd.exe /c prog.exe args"  or  "prog.exe args"
 the first form is compulsory if args contain "> outfile"
 prog is searched in dir where calling program was launched, current dir,
 windows system dir, windows dir, dirs in PATH.
 returns 0 iff OK
 *\
{
  static char buf[100];
  STARTUPINFO info;
  PROCESS_INFORMATION pi;
  int retval;
  
  GetStartupInfo(&info);
  retval = CreateProcess(NULL, (char *)command,0,0,0,0,0,_getcwd(buf,sizeof(buf)),&info,&pi);
  if(retval == 0) return 1;
  do	{
    Fl::check();
    retval = WaitForSingleObject(pi.hProcess, 500);
  }
  while(retval == WAIT_TIMEOUT);
  return 0;
}

 */

#endif // WIN32
