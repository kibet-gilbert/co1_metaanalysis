#ifndef FL_SVG_GRAPHICS_DRIVER_H
#define FL_SVG_GRAPHICS_DRIVER_H

#include <stdio.h>
#include <FL/Fl_Device.H>
#ifndef NO_PDF
#include "pdf_or_ps.h"
#endif

class Fl_SVG_Graphics_Driver : public Fl_Graphics_Driver {
  FILE *out_;
  int width_;
  const char *linecap_;
  uchar red_, green_, blue_;
public:
  Fl_SVG_Graphics_Driver(FILE*);
  ~Fl_SVG_Graphics_Driver();
  FILE* file() {return out_;}
protected:
  const char *family_;
  const char *bold_;
  const char *style_;
  void rect(int x, int y, int w, int h);
  void rectf(int x, int y, int w, int h);
  void line_style(int style, int width, char *dashes=0);
  void line(int x1, int y1, int x2, int y2);
  void font_(int f, int s);
  void font(int f, int s);
  void draw(const char *str, int n, int x, int y);
  void draw(const char*, int, float, float) ;
  void draw(int, const char*, int, int, int) ;
  void rtl_draw(const char*, int, int, int) ;
  void color(uchar r, uchar g, uchar b);
  void color(Fl_Color c);
  void draw_image(const uchar*, int, int, int, int, int, int) ;
  void draw_image_mono(const uchar*, int, int, int, int, int, int) ;
  void draw_image(void (*)(void*, int, int, int, uchar*), void*, int, int, int, int, int) ;
  void draw_image_mono(void (*)(void*, int, int, int, uchar*), void*, int, int, int, int, int) ;
  void draw(Fl_RGB_Image*, int, int, int, int, int, int) ;
  void draw(Fl_Pixmap*, int, int, int, int, int, int) ;
  void draw(Fl_Bitmap*, int, int, int, int, int, int) ;
  double width(const char*, int) ;
  int height() ;
  int descent() ;
  
#if SEAVIEW_FLTK_VERSION >= 140
  void push_clip(int x, int y, int w, int h) {}
  void pop_clip(){}
  void xyline(int x, int y, int x1){}
  void xyline(int x, int y, int x1, int y2){}
  void yxline(int x, int y, int y1){}
  void yxline(int x, int y, int y1, int x2){}
  virtual void point(int x, int y) {}
  virtual void line(int x, int y, int x1, int y1, int x2, int y2) {}
  virtual void xyline(int x, int y, int x1, int y2, int x3) {}
  virtual void yxline(int x, int y, int y1, int x2, int y3) {}
  virtual void loop(int x0, int y0, int x1, int y1, int x2, int y2) {}
  virtual void loop(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3) {}
  virtual void polygon(int x0, int y0, int x1, int y1, int x2, int y2) {}
  virtual void polygon(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3) {}
  virtual int clip_box(int x, int y, int w, int h, int &X, int &Y, int &W, int &H) {return 0;}
  virtual int not_clipped(int x, int y, int w, int h) {return 0;}
  virtual void push_no_clip() {}
  virtual void begin_complex_polygon() {}
  virtual void transformed_vertex(double xf, double yf) {}
  virtual void vertex(double x, double y) {}
  virtual void end_points() {}
  virtual void end_line() {}
  virtual void end_loop() {}
  virtual void end_polygon() {}
  virtual void end_complex_polygon() {}
  virtual void gap() {}
  virtual void circle(double x, double y, double r) {}
  virtual void arc(int x, int y, int w, int h, double a1, double a2) {}
  virtual void pie(int x, int y, int w, int h, double a1, double a2) {}
  virtual Fl_Bitmask create_bitmask(int w, int h, const uchar *array) {return 0;}
  virtual void delete_bitmask(Fl_Bitmask bm) {}
#endif
};

class Fl_SVG_File_Surface : public Fl_Surface_Device {
  int width_, height_;
public:
  static const char *class_id;
  const char *class_name() {return class_id;};
  Fl_SVG_File_Surface(int, int, FILE*, bool use_pdf_width = false);
  void end();
  int width() { return width_; }
  int height() { return height_; }
  ~Fl_SVG_File_Surface();
};

#ifndef NO_PDF
/* Fl_SVG_PDF_width_Graphics_Driver uses a PDF graphics driver only to compute string widths and heights,
 to allow creation of SVG data without calling fl_open_display().
 */
class Fl_SVG_PDF_width_Graphics_Driver : public Fl_SVG_Graphics_Driver {
  Fl_PDF_Graphics_Driver *pdf_driver;
public:
  Fl_SVG_PDF_width_Graphics_Driver(FILE *f);
  ~Fl_SVG_PDF_width_Graphics_Driver();
  int descent();
  int height();
  double width(const char* str, int l);
  void font(int ft, int s);
};
#endif  // ! NO_PDF

#endif // FL_SVG_GRAPHICS_DRIVER_H

