#include "pdf_or_ps.h"
#include <time.h>
#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Paged_Device.H>
#include <FL/fl_ask.H>
#include <ctype.h>

extern char *run_and_close_native_file_chooser(Fl_Native_File_Chooser *chooser, int keepalive=0);
extern char *prepare_ps_or_pdf_font(int font_num);
extern const char *extract_filename(const char *fname);

#ifdef NO_PDF

int Fl_PDF_or_PS_File_Device::begin_document(const char* fixedfilename, enum Fl_Paged_Device::Page_Format format,
					     enum Fl_Paged_Device::Page_Layout layout)
{
  file = fopen(fixedfilename, "w");
  if (!file) return 1;
  filename = strdup(fixedfilename);
  return Fl_PostScript_File_Device::start_job(file, 0, format, layout);
}

int Fl_PDF_or_PS_File_Device::start_job(const char *defaultname, enum Fl_Paged_Device::Page_Format format, enum Fl_Paged_Device::Page_Layout layout)
{
  Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
  chooser->type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  chooser->title("Set PostScript filename");       
  chooser->filter("PS Files\t*.ps");
  int l = defaultname ? strlen(defaultname) : 0;
  char *preset = new char[l + 5];
  strcpy(preset, defaultname ? defaultname : "" );
  char *p = strchr(preset, '.');
  if (p) *p = 0;
  strcat(preset, ".ps");
  chooser->preset_file(extract_filename(preset));
  delete[] preset;
  chooser->options(Fl_Native_File_Chooser::SAVEAS_CONFIRM | chooser->options());
  char *plotfilename = run_and_close_native_file_chooser(chooser);
  if(plotfilename == NULL) return 1;
  return begin_document(plotfilename, format, layout);
}

Fl_PDF_or_PS_File_Device::~Fl_PDF_or_PS_File_Device() 
{ 
  if (file) { 
    fclose(file); free(filename); 
  } 
}

#else // NO_PDF

#include <pdflib.h>
#include <setjmp.h>
#ifdef WIN32
extern PDF *win32_PDF_new();
extern int win32_PDF_begin_document(PDF *p, const char *filename, int len, const char *optlist);
extern void win32_PDF_end_document(PDF *p, const char *optlist);
extern void win32_PDF_delete(PDF *p);
#define PDF_begin_document win32_PDF_begin_document
#define PDF_end_document win32_PDF_end_document
#define PDF_new win32_PDF_new
#define PDF_delete win32_PDF_delete
#endif // WIN32

Fl_PDF_File_Device::Fl_PDF_File_Device()
{
  driver(new Fl_PDF_Graphics_Driver);
  filename = NULL;
}

Fl_PDF_File_Device::~Fl_PDF_File_Device()
{
  PDF *p = ((Fl_PDF_Graphics_Driver*)driver())->pdf;
  if (p) PDF_delete(p);
  delete driver();
  if (filename) free(filename);
}

int Fl_PDF_File_Device::start_job(const char *defaultname, enum Fl_Paged_Device::Page_Format format,
				  enum Fl_Paged_Device::Page_Layout layout)
{
  Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
  chooser->type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  chooser->title("Set PDF filename");       
  chooser->filter("PDF Files\t*.pdf");
  int l = defaultname ? strlen(defaultname) : 0;
  char *preset = new char[l + 5];
  strcpy(preset, defaultname ? defaultname : "" );
  char *p = strchr(preset, '.');
  if (p) *p = 0;
  strcat(preset, ".pdf");
  chooser->preset_file(preset);
  delete[] preset;
  chooser->options(Fl_Native_File_Chooser::SAVEAS_CONFIRM | chooser->options());
  char *plotfilename = run_and_close_native_file_chooser(chooser);
  if(plotfilename == NULL) return 1;
  return begin_document(plotfilename, format, layout);
  }
  
int Fl_PDF_File_Device::begin_document(const char* fixedfilename, enum Fl_Paged_Device::Page_Format format,
				       enum Fl_Paged_Device::Page_Layout layout)
{
  if (layout == LANDSCAPE) {
    return begin_custom(fixedfilename, page_formats[format].height, page_formats[format].width);
    }
  return begin_custom(fixedfilename, page_formats[format].width, page_formats[format].height);
}

int Fl_PDF_File_Device::begin_custom(const char* plotfilename, int pwidth, int pheight)
  {
  width = pwidth;
  height = pheight;
  PDF *pdf = PDF_new();
  if (pdf == NULL) return 1;
  Fl_PDF_Graphics_Driver *d = (Fl_PDF_Graphics_Driver*)driver();
  d->pdf = pdf;
  if (PDF_begin_document(pdf, plotfilename, 0, "compatibility=1.3") == -1) {
    fl_alert("Error opening %s for writing\n", plotfilename);
    return 1;
  }
  PDF_set_info(pdf, "Title", plotfilename );
  PDF_set_info(pdf, "Creator", "seaview");
  previous_surface = Fl_Surface_Device::surface();
  PDF_set_parameter(pdf, "topdown", "true");
  if (width == page_formats[Fl_Paged_Device::A4].width) {
    left_margin = 18;
    top_margin = 18;
  }
  else {
    left_margin = 12;
    top_margin = 12;
  }
  filename = strdup(plotfilename);
  set_current();
  return 0;
}

int Fl_PDF_File_Device::start_page()
{
  Fl_PDF_Graphics_Driver *d = (Fl_PDF_Graphics_Driver*)driver();
  PDF *p = d->pdf;
  PDF_begin_page_ext(p, width, height, "");
  PDF_translate(p, left_margin, top_margin);
  PDF_setlinecap(p, 1);
  PDF_setlinewidth(p, 1);
  d->in_page = true;
  if (d->pdf_font != -1) {
    PDF_setfont(p, d->pdf_font, d->size());
    }
  PDF_save(p);
  d->tr_x = d->tr_y = 0;
  return 0;
}

int Fl_PDF_File_Device::printable_rect(int *w, int *h)
//returns 0 iff OK
{
  if(w) *w = (int)((width - 2 * left_margin) + .5);
  if(h) *h = (int)((height - 2 * top_margin) + .5);
  return 0;
}

void Fl_PDF_File_Device::margins(int *left, int *top, int *right, int *bottom)
{
  *left = left_margin;
  *top = top_margin;
  *right = left_margin;
  *bottom = top_margin;
}

void Fl_PDF_File_Device::origin(int x, int y)
{
  Fl_PDF_Graphics_Driver *d = (Fl_PDF_Graphics_Driver*)driver();
  d->tr_x = x;
  d->tr_y = y;
  PDF *p = d->pdf;
  PDF_restore(p);
  PDF_save(p);
  PDF_translate(p, x, y);
}

void Fl_PDF_File_Device::origin(int *px, int *py)
{
  Fl_PDF_Graphics_Driver *d = (Fl_PDF_Graphics_Driver*)driver();
  if (px) *px = d->tr_x;
  if (py) *py = d->tr_y;
}

void Fl_PDF_File_Device::translate(int x, int y)
{
  Fl_PDF_Graphics_Driver *d = (Fl_PDF_Graphics_Driver*)driver();
  PDF *p = d->pdf;
  PDF_save(p);
  PDF_translate(p, x, y);
}

void Fl_PDF_File_Device::untranslate()
{
  Fl_PDF_Graphics_Driver *d = (Fl_PDF_Graphics_Driver*)driver();
  PDF *p = d->pdf;
  PDF_restore(p);
}

int Fl_PDF_File_Device::end_page()
{
  Fl_PDF_Graphics_Driver *d = (Fl_PDF_Graphics_Driver*)driver();
  PDF_restore(d->pdf);
  PDF_end_page_ext(d->pdf, "");
  d->in_page = false;
  return 0;
}

void Fl_PDF_File_Device::end_job()
{
  PDF_end_document(((Fl_PDF_Graphics_Driver*)driver())->pdf, "");
  previous_surface->set_current();
}

void *Fl_PDF_File_Device::pdf()
{
  return ((Fl_PDF_Graphics_Driver*)driver())->pdf;
}

void Fl_PDF_File_Device::error_catch()
{
  Fl_Display_Device::display_device()->set_current();
  PDF *pdf = (PDF*)this->pdf();
  fl_alert("Error while writing pdf file:\n%s\n"
	   "[%d] %s: %s\n", filename,
	   PDF_get_errnum(pdf), PDF_get_apiname(pdf), PDF_get_errmsg(pdf) );
}

int Fl_PDF_File_Device::surface_catch()
{
  return pdf_catch(((Fl_PDF_Graphics_Driver*)driver())->pdf);
}

void Fl_PDF_File_Device::surface_try(jmp_buf **jbuf)
{
  PDF *p = ((Fl_PDF_Graphics_Driver*)driver())->pdf;
  *jbuf = &( pdf_jbuf(p)->jbuf );
}


Fl_PDF_Graphics_Driver::Fl_PDF_Graphics_Driver()
{
#ifdef __APPLE__
  encoding	= "macroman";
#else
  encoding	= "iso8859-1";
#endif
  pdf = NULL;
  pdf_font = -1;
  in_page = false;
}

void Fl_PDF_Graphics_Driver::line(int x1, int y1, int x2, int y2)
{
  PDF_moveto(pdf, x1, y1);
  PDF_lineto(pdf, x2, y2);
  PDF_stroke(pdf);
}
void Fl_PDF_Graphics_Driver::xyline(int x, int y, int x1)
{
  line(x, y, x1, y);
}
void Fl_PDF_Graphics_Driver::xyline(int x, int y, int x1, int y2)
{
  line(x, y, x1, y);
  line(x1,y,x1,y2);
}
void Fl_PDF_Graphics_Driver::yxline(int x, int y, int y1)
{
  line(x, y, x, y1);
}
void Fl_PDF_Graphics_Driver::yxline(int x, int y, int y1, int x2)
{
  line(x, y, x, y1);
  line(x,y1,x2,y1);
}

void Fl_PDF_Graphics_Driver::rect(int x, int y, int w, int h)
{
  PDF_rect(pdf, x, y+h, w, h);
  PDF_stroke(pdf);
}

void Fl_PDF_Graphics_Driver::rectf(int x, int y, int w, int h)
{
  PDF_rect(pdf, x, y+h, w, h);
  PDF_fill(pdf);
}

void Fl_PDF_Graphics_Driver::draw(const char *str, int n, int x, int y)
{
  PDF_show_xy2(pdf, str, n, x, y);
}

void Fl_PDF_Graphics_Driver::draw(int angle, const char* str, int n, int x, int y)
{
  PDF_save(pdf);
  PDF_translate(pdf, x, y);
  PDF_rotate(pdf, angle);
  PDF_show_xy2(pdf, str, n, 0, 0);
  PDF_restore(pdf);
}

void Fl_PDF_Graphics_Driver::font(int f, int s)
{
  char *current_ps_font = prepare_ps_or_pdf_font(f);
  pdf_font = PDF_load_font(pdf, current_ps_font, 0, encoding, "");
  Fl_Graphics_Driver::font(f, s);
  if (in_page) PDF_setfont(pdf, pdf_font, s);
}

void Fl_PDF_Graphics_Driver::color(Fl_Color c)
{
  uchar red, green, blue;
  Fl::get_color(c, red, green, blue);
  color(red, green, blue);
}

void Fl_PDF_Graphics_Driver::color(uchar red, uchar green, uchar blue)
{
  float r, g, b;
  r = red/255.; g = green/255.; b = blue/255.;
  PDF_setcolor(pdf, "fillstroke", "rgb", r, g, b, 0);
}

double Fl_PDF_Graphics_Driver::width(const char* str, int l)
{
  return PDF_stringwidth2(pdf, str, l, pdf_font, size());
}

int Fl_PDF_Graphics_Driver::height() {
  return size();
}

int Fl_PDF_Graphics_Driver::descent() {
  return (int)(-PDF_get_value(pdf, "descender", pdf_font) * size() + 0.5) + 1;
}

void Fl_PDF_Graphics_Driver::draw(const char* str, int n, float fx, float fy) {
  draw(str, n, (int)fx, (int)fy);
}

void Fl_PDF_Graphics_Driver::push_clip(int x, int y, int w, int h) 
{
  PDF_save(pdf);
  PDF_moveto(pdf, x, y); PDF_lineto(pdf, x + w, y); PDF_lineto(pdf, x + w, y + h);
  PDF_lineto(pdf, x, y + h); 
  PDF_closepath(pdf); 
  PDF_clip(pdf);
}

void Fl_PDF_Graphics_Driver::pop_clip()
{
  PDF_restore(pdf);
}

void Fl_PDF_Graphics_Driver::draw_image(const uchar*, int, int, int, int, int, int) {}
void Fl_PDF_Graphics_Driver::draw_image_mono(const uchar*, int, int, int, int, int, int) {}
void Fl_PDF_Graphics_Driver::draw_image(void (*)(void*, int, int, int, uchar*), void*, int, int, int, int, int) {}
void Fl_PDF_Graphics_Driver::draw_image_mono(void (*)(void*, int, int, int, uchar*), void*, int, int, int, int, int) {}
void Fl_PDF_Graphics_Driver::draw(Fl_RGB_Image*, int, int, int, int, int, int) {}
void Fl_PDF_Graphics_Driver::draw(Fl_Pixmap*, int, int, int, int, int, int) {}
void Fl_PDF_Graphics_Driver::draw(Fl_Bitmap*, int, int, int, int, int, int) {}
void Fl_PDF_Graphics_Driver::line_style(int style, int width, char *dashes) {}
void Fl_PDF_Graphics_Driver::rtl_draw(const char*, int, int, int) {}

#endif // NO_PDF
