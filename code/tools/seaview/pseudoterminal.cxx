#ifdef WIN32
#include <windows.h>

#elif defined(__APPLE__) 
#include <unistd.h>
#include <util.h>

#else //X11
#include <unistd.h>
#ifdef sun
#include <stropts.h>
#endif

#endif
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <FL/x.H>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/fl_ask.H>


enum {running, completed, aborted, closed};

struct listener_data {
	int status;
	Fl_Multiline_Output *o;
#ifdef WIN32
	HANDLE pid;
#else
	pid_t pid;
#endif
	int fd;
	Fl_Button *ok, *interrupt;
	Fl_Scrollbar *bar;
	int nl; // current number of complete lines in buffer
	int view_nl; // max no of visible lines in Fl_Multiline_Output
	char *from; // start in buffer of displayed text during running
	char *next; // just after last displayed buffer byte
	char *eol; // points to where new bytes will be put in buffer
	bool modified;
	char buffer[10000000]; // buffer to hold text. Start from beginning again when more text than buffer size
};

typedef void (*thread_function_t) (void *);
extern int fl_create_thread(thread_function_t f, void* p, unsigned stack_size_KB);


static void inter_callback(Fl_Widget *o, struct listener_data *data)
{
	data->status = aborted;
#ifdef WIN32
	BOOL ok = TerminateProcess(data->pid, 0);
	if (ok) {
	  while ( WaitForSingleObject(data->pid, 0) != WAIT_OBJECT_0) Fl::wait(0.1);
	  }
#else
	kill(data->pid, SIGKILL);
#endif
}

static void ok_callback(Fl_Widget *o, struct listener_data *data)
{
	data->status = closed;
}

static void bar_callback(Fl_Scrollbar *wgt, Fl_Multiline_Output *o)
{
	int first_line = wgt->value();
	const char *p = o->value();
	do {
		if(*p == '\n') first_line--;
		p++;
		}
	while(*p != 0 && first_line > 0);
	o->position(p - o->value());
	o->redraw();
}


static void copy_callback(Fl_Widget *wgt, Fl_Multiline_Output *o)
{
	o->position(o->size(), 0);
	o->copy(1);
}


void ext_prog_listener(struct listener_data *data)
{//transmits data from fd to FLTK viewer with CR emulation
  char *p;
  static char fd_buff[2000];
  int l;
  while (data->status == running) {
    l = read(data->fd, fd_buff, sizeof(fd_buff));
    if (l <= 0) break;
    Fl::lock();
    if (data->next + l >= data->buffer + sizeof(data->buffer)) {
      data->next = data->from = data->eol = data->buffer;
      data->nl = 0;
    }
    for (p = fd_buff; p < fd_buff + l; p++) {
      if (*p == '\r') {
	if (p+1 >= fd_buff + l || *(p+1) != '\n') {
	  while(data->eol > data->buffer && *(data->eol - 1) != '\n') 
			      (data->eol)--;
	  }
	}
      else {
	if (*p == '\n') {
	  data->eol = data->next;
	  data->nl++;
	  if(data->nl >= data->view_nl) data->from = strchr(data->from, '\n') + 1;
	  }
	*(data->eol++) = *p;
	if(data->eol > data->next) data->next = data->eol;
	}
      }
    data->modified = true;
    Fl::unlock();
    }
  Fl::lock();
  if (data->status == running) {
    data->o->static_value(data->buffer, data->next - data->buffer);
    data->o->position(data->next - data->buffer, data->next - data->buffer);
    data->o->redraw();
    data->bar->value(data->nl, data->view_nl, 0, data->nl);
    data->bar->show();
    if (!data->ok->active()) {
      data->ok->activate();
      data->interrupt->label("Cancel");
      data->status = completed;
      }
    }
  Fl::unlock();
}


Fl_Window *create_and_run_pseudoterminal(const char *label, struct listener_data *data)
#define FL_min(a,b)      ( (a) < (b) ? (a):(b) )
{
  static char message[100];
  Fl_Double_Window *w = new Fl_Double_Window( FL_min(700, Fl::w()), FL_min(600, Fl::h()-22 ) );
  w->xclass("Terminal");
  w->label(label);
  w->callback((Fl_Callback*)inter_callback, data);
  Fl_Multiline_Output *o = new Fl_Multiline_Output(0, 3, w->w() - 15, w->h() - 40);
  o->textfont(FL_COURIER);
  o->textsize(12);
  o->maximum_size(5000000);
  data->bar = new Fl_Scrollbar(o->x() + o->w(), o->y(), 15, o->h());
  data->bar->callback((Fl_Callback*)bar_callback, o);
  data->view_nl = o->h() / o->textsize();
#ifdef WIN32
  data->view_nl = (int)(data->view_nl * 0.80);
#endif
  data->bar->value(0, data->view_nl, 0, data->view_nl);
  data->bar->hide();
  sprintf(message, "Wait for %s completion", label);
  Fl_Box *w_message = new Fl_Box(0, o->y() + o->h() + 3, w->w(), 20, message);
  Fl_Button *w_copy = new Fl_Button(w->w()/2 - 30, w_message->y(), 60, 20, "Copy all");
  w_copy->hide();
  w_copy->callback((Fl_Callback*)copy_callback, o);
  data->status = running;
  data->o = o;
  data->nl = 0;
  data->from = data->next = data->eol = data->buffer;
  Fl_Button *interrupt = new Fl_Button(5, w->h() - 35, 70, 30, "Interrupt");
  interrupt->callback((Fl_Callback*)inter_callback, data);
  interrupt->labelfont(FL_HELVETICA_BOLD);
  Fl_Return_Button *ok = new Fl_Return_Button(w->w() - 60, interrupt->y(), 50, 30, "OK");
  ok->callback((Fl_Callback*)ok_callback, data);
  ok->labelfont(FL_HELVETICA_BOLD);
  ok->deactivate();
  data->ok = ok;
  data->interrupt = interrupt;
  data->modified = false;
  w->end();
  w->position( (Fl::w() - w->w())/2, (Fl::h() - w->h())/2 );
  Fl_Box *r = new Fl_Box(interrupt->x() + interrupt->w(), o->y(), ok->x() - interrupt->x() - interrupt->w(), o->h());
  w->resizable(r);
  w->show();
#ifndef MICRO
  w->hotspot(w);
#endif
  fl_create_thread( (thread_function_t)ext_prog_listener, data, 0);
  while (data->status == running) {
    if (data->modified) {
      data->modified = false;
      data->o->static_value(data->from, data->next - data->from);
      data->o->redraw();
      }
    Fl::wait(0.1);
    }
  w_message->hide();
  w_copy->show();
#ifdef WIN32
  _close(data->fd);
#else
  close(data->fd);
#endif
  return w;
}


int run_external_prog_in_pseudoterm(char *cmd, const char *dialogfname, const char *label)
/* 
 cmd can be:
 prog arg1 arg2 ...
 or:
 "prog" arg1 "arg2" ... > outfile
 and:
 prog can also use file named in dialogfname as stdin (NULL if no such file)
 label: short name of operation that is run
 returns 0 iff cmd completed correctly
 */
{
	static struct listener_data data;
	static bool inuse = 0;
	if(inuse) {
	  fl_alert("Operation %s is not possible while another alignment/tree-building is in progress",
		   label);
	  return 1;
	  }
	inuse = 1;
	Fl_Window *w;
#ifdef WIN32
	HANDLE ChildInput, ChildStdoutRd, ChildStdoutWr, stdouthandle = NULL;
	SECURITY_ATTRIBUTES saAttr; 
	PROCESS_INFORMATION pi; 
	STARTUPINFOW si;
	char *p, *q;
	
	if(dialogfname != NULL) {
		FILE *in = fopen(dialogfname, "r");
		if(in == NULL) {
		  inuse = 0;
		  return 1;
		  }
		ChildInput = (HANDLE)_get_osfhandle(fileno(in));
	}
	p = cmd;//is there stdout redirection in cmd ?
	if(*p == '"') p = strchr(p+1, '"') + 1;
	p = strchr(p, '>');
	if(p != NULL) {
		*p = 0;
		p++;
		while(*p == ' ') p++;
		if (*p == '"') {
		  q = strchr(++p, '"');
		  if (q) *q = 0;
		  }
		char *stdoutfname = strdup(p);
		FILE *out = fopen(stdoutfname, "w");
		free(stdoutfname);
		if(out != NULL) stdouthandle = (HANDLE)_get_osfhandle(fileno(out));
		}
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 
	CreatePipe(&ChildStdoutRd, &ChildStdoutWr, &saAttr, 0/*suggested buffer size*/);
	DWORD written;
	WriteFile(ChildStdoutWr, "Running ", 8, &written, NULL);
	WriteFile(ChildStdoutWr, cmd, strlen(cmd), &written, NULL);
	WriteFile(ChildStdoutWr, "\n ", 1, &written, NULL);
	SetHandleInformation(ChildStdoutRd, HANDLE_FLAG_INHERIT, 0);
	ZeroMemory( &pi, sizeof(PROCESS_INFORMATION) );
	ZeroMemory( &si, sizeof(STARTUPINFOW) );
	si.cb = sizeof(STARTUPINFOW);
	si.hStdError = ChildStdoutWr;
	si.hStdOutput = stdouthandle ? stdouthandle : ChildStdoutWr;
	if(dialogfname != NULL) si.hStdInput = ChildInput;
	else si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	si.dwFlags |= STARTF_USESTDHANDLES;
//conversion of utf-8 cmd into a WCHAR string
	int wlen = MultiByteToWideChar(CP_UTF8, 0, cmd, -1, NULL, 0);
	WCHAR *wcmd = new WCHAR[wlen];
	MultiByteToWideChar(CP_UTF8, 0, cmd, -1, wcmd, wlen);
	int retval = CreateProcessW(NULL, wcmd, 0,0,TRUE,CREATE_NO_WINDOW,0,0, &si, &pi);
	delete wcmd;
	CloseHandle(ChildStdoutWr);
	if(stdouthandle != NULL) CloseHandle(stdouthandle);
	if(dialogfname != NULL) CloseHandle(ChildInput);
	if(retval == 0) {
	  CloseHandle(ChildStdoutRd);
	  inuse = 0;
	  return 1;
	}
	CloseHandle(pi.hThread);
	data.pid = pi.hProcess;
	data.fd = _open_osfhandle(
#  ifdef _WIN64
				 (intptr_t)
#  else
				 (long)
#  endif
				 ChildStdoutRd, _O_RDONLY);
	w = create_and_run_pseudoterminal(label, &data);
	CloseHandle(pi.hProcess);
#else
  // creates a pseudoterminal
  pid_t pid;
  int masterfd;
  char *slavename;
#if defined(__APPLE__) 
  char slavename2[50];
  if (fl_mac_os_version < 100500) { //special for Mac OS X 10.4 + i386
    slavename = slavename2;
    pid = forkpty(&masterfd, slavename2, NULL, NULL);
    }
  else
#endif
    {
    masterfd = posix_openpt(O_RDWR|O_NOCTTY);
    grantpt(masterfd);
    unlockpt(masterfd);
    slavename = ptsname(masterfd);
    if(masterfd == -1 || slavename == NULL) {
      inuse = 0;
      return 1;
      }
      pid = fork();
    }
	if(pid == -1) {
	  inuse = 0;
	  return 1;
	  }
	else if(pid > 0) {// the parent
		data.pid = pid;
		data.fd = masterfd;
		w = create_and_run_pseudoterminal(label, &data);
	}
	else {//the child
		if(dialogfname != NULL) {//case of use of dialog file
			freopen(dialogfname, "r", stdin);
		}
		freopen(slavename, "a", stderr);
#ifdef sun
		ioctl(fileno(stderr), I_PUSH, "ptem");
		//		ioctl(fileno(stderr), I_PUSH, "ldterm");
		//		ioctl(fileno(stderr), I_PUSH, "ttcompat");
#endif
		fputs(cmd, stderr); fputc('\n', stderr); fflush(stderr);//show command
		//decode progname
		char *p, *q, *progpath, **argv;
		int argc, newout = -1;
		close(masterfd);
		p = cmd;
		while(*p == ' ') p++;
		if(*p == '"') q = strchr(++p, '"');
		else {
			q = strchr(p, ' ');
			if(q == NULL) q = p + strlen(p);
			}
		progpath = new char[q-p+1];
		memcpy(progpath, p, q-p); progpath[q-p] = 0;
		//goto end of progname
		p = q;
		if(*p == '"') p++;
	  // take care of stdout
	  // bug in Mac OS X 10.4 i386 requires us to close stdout and reopen it on fd = 1
		fclose(stdout);
		if((q = strchr(p, '>')) != NULL) {//open desired stdout file
			char stdoutfile[200];
			FILE *f;
			*q = 0;
			do q++; while(*q == ' ');
			if (*q == '"') q++;
			strcpy(stdoutfile, q);
			q = stdoutfile + strlen(stdoutfile) - 1;
			while (q > stdoutfile && (*q == ' ' || *q == '"') ) q--;
			*(q + 1) = 0;
			f = fopen(stdoutfile, "w");
			if(f) newout = fileno(f);
		}
		else {
		  newout = open(slavename, O_WRONLY|O_APPEND);
		  }
		if(newout != 1 && newout != -1) {
		  dup2(newout, 1);
		  }
		//count args
		argc = 1;
		q = p - 1;
		while (*++q) {
		  if (isspace(*q)) continue;
		  argc++;
		  if (*q == '"') {
		    q = strchr(q + 1, '"');
		    if (!q) break;
		    }
		  else {
		    do q++; while ( *q && !isspace(*q) );
		    q--;
		    }
		  }
		//put args in argv array
		argv = (char **)malloc((argc + 1)*sizeof(char *));
		argv[0] = progpath;
		argv[argc] = NULL;
		q = p - 1;
		int i = 1;
		while (*++q) {
		  if (isspace(*q)) continue;
		  if (*q == '"') {
		    argv[i++] = ++q;
		    q = strchr(q, '"');
		    if (!q) break;
		    *q = 0;
		  }
		  else {
		    argv[i++] = q;
		    do q++; while ( *q && !isspace(*q) );
		    if (*q == 0) {
		      break;
		      }
		    *q = 0;
		  }
		}
		int err = 1; const char *errmess;
		if(access(progpath, X_OK) != 0) errmess = "executable";
		else {
			err = execv(progpath, argv);
			errmess = "a valid program";
			}
		fprintf(stderr, "\n\n\n%s is not %s\n", progpath, errmess);
		exit(err);
	}
#endif
	while(data.status == completed) Fl::wait();
	delete w;
	if(dialogfname != NULL) remove(dialogfname);
	inuse = 0;
	return data.status == aborted;
} 
