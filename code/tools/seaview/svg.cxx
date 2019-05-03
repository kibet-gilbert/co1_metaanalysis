#include "treedraw.h"
#include "unrooted.h"
#include "svg.h"
#ifndef NO_PDF
#include "pdf_or_ps.h"
#endif

extern void do_plot(FD_nj_plot *fd_nj_plot, int doing_print);
extern void frame_and_draw_unrooted(FD_unrooted *fd_unrooted);
extern "C" int trim_key(char *name); /* remove trailing spaces */

const char *Fl_SVG_File_Surface::class_id = "Fl_SVG_File_Device";


Fl_SVG_File_Surface::~Fl_SVG_File_Surface() {
  delete driver();
}

void Fl_SVG_File_Surface::end() {
  Fl_SVG_Graphics_Driver *driver = (Fl_SVG_Graphics_Driver*)this->driver();
  fputs("</svg>\n", driver->file());
  fflush(driver->file());
}

Fl_SVG_Graphics_Driver::Fl_SVG_Graphics_Driver(FILE *f) {
  out_ = f;
  width_ = 1;
  linecap_ = "butt";
  family_ = "";
  bold_ = "";
  style_ = "";
  red_ = green_ = blue_ = 0;
}

Fl_SVG_Graphics_Driver::~Fl_SVG_Graphics_Driver()
{
}

void Fl_SVG_Graphics_Driver::rect(int x, int y, int w, int h) {
  fprintf(out_, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" "
	  "fill=\"none\" stroke=\"rgb(%u,%u,%u)\" stroke-width=\"%d\"/>\n", x, y, w, h, red_, green_, blue_, width_);
}

void Fl_SVG_Graphics_Driver::rectf(int x, int y, int w, int h) {
  fprintf(out_, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" "
	  "fill=\"rgb(%u,%u,%u)\" />\n", x, y, w, h, red_, green_, blue_);
}

void Fl_SVG_Graphics_Driver::line(int x1, int y1, int x2, int y2) {
  fprintf(out_, 
	  "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" "
	  "style=\"stroke:rgb(%u,%u,%u);stroke-width:%d;stroke-linecap:%s\" />\n", 
	  x1,y1,x2,y2, red_, green_, blue_, width_, linecap_);
}

void Fl_SVG_Graphics_Driver::font_(int ft, int s) {
  Fl_Graphics_Driver::font(ft, s);
  int famnum = ft/4;
  if (famnum == 0) family_ = "Helvetica";
  else if (famnum == 1) family_ = "Courier";
  else family_ = "Times";
  int modulo = ft % 4;
  int use_bold = modulo == 1 || modulo == 3;
  int use_italic = modulo >= 2;  
  bold_ =  ( use_bold ? " font-weight=\"bold\"" : "" );
  style_ =  ( use_italic ? " font-style=\"italic\"" : "" );
  if (use_italic && famnum != 2) style_ = " font-style=\"oblique\"";
}

void Fl_SVG_Graphics_Driver::font(int ft, int s) {
  Fl_Display_Device::display_device()->driver()->font(ft, s);
  font_(ft, s);
}

void Fl_SVG_Graphics_Driver::line_style(int style, int width, char *dashes) {
  if (width == 0) width = 1;
  width_ = width;
  if (style & FL_CAP_SQUARE) linecap_ = "square";
  if (style & FL_CAP_ROUND) linecap_ = "round";
  else linecap_ = "butt";
}

void Fl_SVG_Graphics_Driver::draw(const char *str, int n, int x, int y) {
  // Caution: Internet Explorer ignores the xml:space="preserve" attribute
  // work-around: replace all spaces by no-break space = U+00A0 = 0xC2-0xA0 (UTF-8) before sending to IE
  fprintf(out_, "<text x=\"%d\" y=\"%d\" font-family=\"%s\"%s%s font-size=\"%d\" "
          "xml:space=\"preserve\" "
          " fill=\"rgb(%u,%u,%u)\" textLength=\"%d\">%.*s</text>\n",x, y, family_, bold_, style_, size(), red_, green_, blue_, (int)width(str, n), n, str);

}

void Fl_SVG_Graphics_Driver::draw(const char* str, int n, float fx, float fy) {
  return draw(str, n, (int)fx, (int)fy);
}

void Fl_SVG_Graphics_Driver::draw(int angle, const char* str, int n, int x, int y) {
  fprintf(out_, "<g transform=\"translate(%d,%d) rotate(%d)\">", x, y, -angle);
  draw(str, n, 0, 0);
  fputs("</g>\n", out_);
}
 
void Fl_SVG_Graphics_Driver::rtl_draw(const char*, int, int, int) {}

void Fl_SVG_Graphics_Driver::color(Fl_Color c) {
  Fl_Graphics_Driver::color(c);
  Fl::get_color(c, red_, green_, blue_);
}

void Fl_SVG_Graphics_Driver::color(uchar r, uchar g, uchar b) {
  red_ = r;
  green_ = g;
  blue_ = b;
}

void Fl_SVG_Graphics_Driver::draw_image(const uchar*, int, int, int, int, int, int) {}
void Fl_SVG_Graphics_Driver::draw_image_mono(const uchar*, int, int, int, int, int, int) {}
void Fl_SVG_Graphics_Driver::draw_image(void (*)(void*, int, int, int, uchar*), void*, int, int, int, int, int) {}
void Fl_SVG_Graphics_Driver::draw_image_mono(void (*)(void*, int, int, int, uchar*), void*, int, int, int, int, int) {}
void Fl_SVG_Graphics_Driver::draw(Fl_RGB_Image*, int, int, int, int, int, int) {}
void Fl_SVG_Graphics_Driver::draw(Fl_Pixmap*, int, int, int, int, int, int) {}
void Fl_SVG_Graphics_Driver::draw(Fl_Bitmap*, int, int, int, int, int, int) {}
double Fl_SVG_Graphics_Driver::width(const char* str, int l) {
 return Fl_Display_Device::display_device()->driver()->width(str, l);
}
int Fl_SVG_Graphics_Driver::height() {
  return Fl_Display_Device::display_device()->driver()->height();
}
int Fl_SVG_Graphics_Driver::descent() {
  return Fl_Display_Device::display_device()->driver()->descent();
}

#ifndef NO_PDF
Fl_SVG_PDF_width_Graphics_Driver::Fl_SVG_PDF_width_Graphics_Driver(FILE *f) : Fl_SVG_Graphics_Driver(f), pdf_driver(new Fl_PDF_Graphics_Driver) {
    pdf_driver->pdf = PDF_new();
    PDF_begin_document(pdf_driver->pdf, "", 0, "");
    PDF_begin_page_ext(pdf_driver->pdf, 100, 100, "");
}

Fl_SVG_PDF_width_Graphics_Driver::~Fl_SVG_PDF_width_Graphics_Driver() {
    PDF_end_page_ext(pdf_driver->pdf, "");
    PDF_end_document(pdf_driver->pdf, "");
    PDF_delete(pdf_driver->pdf);
    delete pdf_driver;
}

int Fl_SVG_PDF_width_Graphics_Driver::descent() {
    return pdf_driver->descent();
}

int Fl_SVG_PDF_width_Graphics_Driver::height() {
    return pdf_driver->height();
}

double Fl_SVG_PDF_width_Graphics_Driver::width(const char* str, int l) {
    return pdf_driver->width(str, l);
}

void Fl_SVG_PDF_width_Graphics_Driver::font(int ft, int s) {
    font_(ft, s);
    pdf_driver->font(ft, s);
}
#endif // NO_PDF

Fl_SVG_File_Surface::Fl_SVG_File_Surface(int w, int h, FILE *f, bool use_pdf_width) : Fl_Surface_Device(NULL) {
  fprintf(f,
	  "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>\n"
	  "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \n"
	  "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n"
	  "<svg width=\"%dpx\" height=\"%dpx\" viewBox=\"0 0 %d %d\"\n"
	  "xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n", w, h, w, h);
  width_ = w; height_ = h;
#ifndef NO_PDF
  if (use_pdf_width) {
    driver(new Fl_SVG_PDF_width_Graphics_Driver(f));
    }
  else
#endif
    driver(new Fl_SVG_Graphics_Driver(f));
}

void svg_tree_save(FD_nj_plot *fd_nj_plot, Fl_SVG_File_Surface *svg)
{
  svg->set_current();
  fl_font(fd_nj_plot->font_family, fd_nj_plot->font_size);
  fd_nj_plot->char_height = fl_height();
  fd_nj_plot->ascent = fl_height() - fl_descent();
  fl_color(FL_GRAY);
  fl_rect(0, 0, svg->width(), svg->height());
  fl_color(FL_BLACK);
  fl_line_style(FL_CAP_SQUARE, 1);
  if(fd_nj_plot->fd_unrooted == NULL) {
    extern double physy;
    double old_physy = physy;
    physy = svg->height();
    do_plot(fd_nj_plot, TRUE);
    physy = old_physy;
    }
  else {
    FD_unrooted *fd_unrooted = (FD_unrooted *)fd_nj_plot->fd_unrooted;
    fd_unrooted->previous_w = 0; 
    fd_unrooted->previous_h = 0;
    fd_unrooted->comp_phys_bounds = TRUE;
    int old_x = fd_unrooted->x_offset;
    int old_y = fd_unrooted->y_offset;
    fd_unrooted->x = fd_unrooted->x_offset = 0;
    fd_unrooted->y = fd_unrooted->y_offset = 0;
    fd_unrooted->w = svg->width();
    fd_unrooted->h = svg->height();
    frame_and_draw_unrooted(fd_unrooted);
    fd_unrooted->x_offset = old_x;
    fd_unrooted->y_offset = old_y;
    fd_unrooted->comp_phys_bounds = FALSE;
  }
  Fl_Display_Device::display_device()->set_current();
  svg->end();
  fclose(((Fl_SVG_Graphics_Driver*)svg->driver())->file());
  delete svg;
}

void svg_tree_dialog_and_save(FD_nj_plot *fd_nj_plot)
{
  Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  chooser->title("Enter SVG filename");       
  chooser->filter("SVG Files\t*.{svg}");
  char tmp[100];
  strcpy(tmp, extract_filename(fd_nj_plot->tree_name));
  char *p = strrchr(tmp, '.'); if(p != NULL) *p = 0;
  strcat(tmp, ".svg");
  chooser->preset_file(tmp);
  chooser->options(Fl_Native_File_Chooser::SAVEAS_CONFIRM | chooser->options());
  char *plotfilename = run_and_close_native_file_chooser(chooser);
  if (plotfilename == NULL) return;
  FILE *out;
  if (!(out = fopen(plotfilename, "w"))) return;
  int width, height;
  if (fd_nj_plot->fd_unrooted) {
    FD_unrooted *unrooted = (FD_unrooted*)fd_nj_plot->fd_unrooted;
    width = unrooted->unrooted_plot->w() * unrooted->zoomvalue;
    height = unrooted->unrooted_plot->h() * unrooted->zoomvalue;
  } else {
    width = fd_nj_plot->panel->w();
    height = fd_nj_plot->panel->h() * fd_nj_plot->zoomvalue;
  }
  svg_tree_save(fd_nj_plot, new Fl_SVG_File_Surface(width, height, out));
}


