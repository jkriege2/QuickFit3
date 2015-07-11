/*
/*
  Name: plotter.h
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/

  the class plotter is dedicated to Julia!
*/
/**
 * \defgroup plotter graphical output class (plotter)
 * \ingroup tool_classes
 *
 * This group contains classes and methods that can be used to plot graphs using the
 * <a href="http://cairographics.org/">Cairo Graphics library</a>. Here are some more usefull links:
 *  - <a href="http://cairographics.org/manual/">official cairo manual</a>
 *  - <a href="http://cairographics.org/samples/">official cairo samples</a>
 *
 * .
 */
/*@{*/

/** \file jkiniparser.h
 *
 */


#include <iostream>
#include <cairo.h>
#include <cairo-pdf.h>
#include <cairo-ps.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include "tools.h"


#ifndef plotter_h
#define plotter_h


/** \brief text alignment
  * \ingroup plotter
  */
enum p_text_alignment {
	pCentered,         /*!< \brief cenetered text */
	pLeftAlign,        /*!< \brief left aligned text */
	pRightAlign        /*!< \brief right aligned text  */
};

/** \brief horizontal block alignment
  * \ingroup plotter
  */
enum p_halignment {
	phCenter,         /*!< \brief cenetered block */
	phLeft,           /*!< \brief left aligned block */
	phRight           /*!< \brief right aligned block  */
};

/** \brief vertical block alignment
  * \ingroup plotter
  */
enum p_valignment {
	pvTop,         /*!< \brief top aligned block */
	pvCenter,      /*!< \brief centered block */
	pvBottom       /*!< \brief bottom block  */
};

/** \brief defines a color as RGBA
  * \ingroup plotter
  */
typedef struct {
  double r;
  double g;
  double b;
  double a;
} p_color;


/** \brief plotter class to output graphics using <a href="http://cairographics.org/">Cairo Graphics library</a>.
  * \ingroup plotter
  *
  * This class provides a free coordinate system which is determined by the plotter::init() function.
  * You can then draw in this coordinate system and the class plotter will do all the transformations
  * into the system of the output file. plotter supports a subset of the cairo instruction set which
  * is listed here:
  *  - set fill and stroke colour as gray value, gray value with transparency, RGB or RGBA value.
  *    Here A gives the transparency of the object where A=1: non-transparent and A=0: fully transparent
  *  - set the line width in units of the user defined coordinate system!
  *  - set the line style (dashes)
  *  - plot rectangles
  *  - plot circles
  *  - plot lines
  *  - output text in different fonts
  * .
  *
  * The class supports some of the cairo output devices:
  *  - PDF files (using plotter::file_open_pdf() )
  *  - PostScript files (using plotter::file_open_ps() )
  *  - PNG files (using plotter::file_open_png() )
  * .
  *
  * Here are some more usefull links:
  *  - <a href="http://cairographics.org/manual/">official cairo manual</a>
  *  - <a href="http://cairographics.org/samples/">official cairo samples</a>
  * .
  *
  * You need to link these libraries to your EXE file:
  *  \code -lm -lcairo -lpixman -lpng12 -lz \endcode
  *
  * You will have to call init() before any file_open*() command. With init() you can generate a
  * user coordinate system that will be used by all subsequent drawing operations. The next picture
  * defines this coordinate system:
  *  \image html plotter_coordsys.png
  *
  * From this one can deduce the transformation rules for points:
  *  \f[ x_{\mbox{user}}=\frac{x-\mbox{xmin}}{\mbox{xmax}-\mbox{xmin}}\cdot\mbox{pwidth} \f]
  * and a rule for lengths:
  *  \f[ l_{\mbox{user}}=\frac{l}{\mbox{xmax}-\mbox{xmin}}\cdot\mbox{pwidth} \f]
  * There are two routines to convert positions ( mtpx() and mtpy() ) and two routines to convert
  * lengths (lxtp() and lytp() ). To scale fontsizes and linewidths the lxtp() method is beeing used.
  * The methods ptlx(), ptly(), ptmx(), ptmy() just calculate the reversed transformations.
  *
  * Here is a simple example code:
  * \code
// init plotter object with a canvas with coordinates (0,0) to (20,10)
// all lengthes are given in meters.
plotter p;
p.init(0, 1, 0, 1);

// output to a pdf file
p.file_open_png("test.png", 500);

// draw a gray filledand an unfilled rectangle starting at (1,1) and with the width 5 and height 3
p.set_fill_gray(0.5);
p.set_stroke_gray(0.5);
p.filled_rectangle(0.10,0.10,0.10,0.10);
p.set_stroke_gray(0.2);
p.rectangle(0.15,0.15,0.20,0.20);

// plot a red, transparent filled circle (blue semi-transparent border) at (5,5) with radius 2
// and an unfilled circle
p.set_fill_rgba(1.0,0,0,1);
p.set_stroke_rgba(0,0,1.0,0.5);
p.set_line_width(0.007);
p.filled_circle(0.50,0.50,0.20);
p.circle(0.75,0.75,0.1);

// plot some green lines (transparent and non-transparent)
p.set_stroke_rgb(0,1,0);
p.set_line_width(0.01);
p.line(1,0,0,1);
p.set_stroke_rgba(0,1,0,0.35);
p.line(0,0,1,1);
p.set_stroke_rgba(0,1,0,0.8);
p.line(0.25,0.5,0.75,0.6);

// plot some dashed lines in variuos widths
p.set_stroke_rgb(0,0,0);
p.set_dash(0);
p.set_line_width(0.002); p.line(0.7,0.08,0.9,0.08);
p.set_line_width(0.005); p.line(0.7,0.1,0.9,0.1);
p.set_line_width(0.01);  p.line(0.7,0.12,0.9,0.12);
p.set_dash(1);
p.set_line_width(0.002); p.line(0.7,0.14,0.9,0.14);
p.set_line_width(0.005); p.line(0.7,0.16,0.9,0.16);
p.set_line_width(0.01);  p.line(0.7,0.18,0.9,0.18);
p.set_dash(2);
p.set_line_width(0.002); p.line(0.7,0.20,0.9,0.20);
p.set_line_width(0.005); p.line(0.7,0.22,0.9,0.22);
p.set_line_width(0.01);  p.line(0.7,0.24,0.9,0.24);
p.set_dash(3);
p.set_line_width(0.002); p.line(0.7,0.26,0.9,0.26);
p.set_line_width(0.005); p.line(0.7,0.28,0.9,0.28);
p.set_line_width(0.01);  p.line(0.7,0.30,0.9,0.30);
p.set_dash(4);
p.set_line_width(0.002); p.line(0.7,0.32,0.9,0.32);
p.set_line_width(0.005); p.line(0.7,0.34,0.9,0.34);
p.set_line_width(0.01);  p.line(0.7,0.36,0.9,0.36);
p.set_dash(5);
p.set_line_width(0.002); p.line(0.7,0.38,0.9,0.38);
p.set_line_width(0.005); p.line(0.7,0.40,0.9,0.40);
p.set_line_width(0.01);  p.line(0.7,0.42,0.9,0.42);

// plot some text
p.set_dash(0);
p.set_stroke_gray(0.5);
p.set_line_width(0.001);
p.set_fontsize(0.02);
p.text(0.15, 0.7, "Text TEXT");
p.set_bold(true); p.text(0.25, 0.7, "Text TEXT");
p.set_fontsize(0.01); p.set_bold(false);
p.text(0.15, 0.725, "Text TEXT");
p.set_bold(true); p.text(0.25, 0.725, "Text TEXT");
p.set_fontsize(0.02); p.set_bold(false);
p.text(0.15, 0.75, "Text TEXT");
p.set_italic(true); p.text(0.25, 0.75, "Text TEXT");
p.set_bold(true); p.text(0.15, 0.775, "Text TEXT");
p.centered_text(0.15, 0.8, "TEXTTEXT");
p.set_fontname("times"); p.set_bold(false);
p.centered_text(0.15, 0.825, "TEXTTEXT");
p.set_fontname("symbol"); p.set_italic(false);
p.centered_text(0.25, 0.85, "abcdefghijklmnopqrstuvwxyz");
p.set_fontname("wingdings"); p.set_italic(false);
p.centered_text(0.25, 0.875, "abcdefghijklmnopqrstuvwxyz");
p.line(0.15,0.7, 0.15,0.825);
p.line(0.25,0.7, 0.25,0.775);

// close the file again
p.file_close();
    \endcode
  * The output of the code looks like this:
  * \image html plotter1.png
  *
  * \test a test application for the plotter can be found in the file test_plotter.cpp.
  */
class plotter {
  private:
    /** \brief cairo surface to draw on */
    cairo_surface_t *surface;
    /** \brief cairo drawing context */
    cairo_t *cr;
    /** \brief FILE structure for output in files */
    FILE *cairofile;

    /** \brief width of the graphic in pixel */
    double xpwidth;
    /** \brief height of the graphic in pixel */
    double ypwidth;
    /** \brief x-offset of the graphic in pixel */
    double xoffset;
    /** \brief y-offset of the graphic in pixel */
    double yoffset;
    /** \brief width of the graphic in user coordinates (set by init() ) */
    double xwidth;
    /** \brief height of the graphic in user coordinates (set by init() ) */
    double ywidth;
    /** \brief minimum x user coordinate (set by init() ) */
    double xmin;
    /** \brief minimum x user coordinate (set by init() ) */
    double ymin;

    /** \brief fill color, red component */
    double fr;
    /** \brief fill color, green component */
    double fg;
    /** \brief fill color, blue component */
    double fb;
    /** \brief fill color, alpha component */
    double fa;
    /** \brief stroke color, red component */
    double sr;
    /** \brief stroke color, green component */
    double sg;
    /** \brief stroke color, blue component */
    double sb;
    /** \brief stroke color, alpha component */
    double sa;

    /** \brief current dashing style */
    int dash_style;
    /** \brief current dashing style */
    double* pdashes;
    /** \brief current dashing style */
    int pnum_dashes;
    /** \brief current dashing style */
    double poffset;

    /** \brief convert a length in meters into a value in DTP points, as expected by cairo */
    inline double cmtp(double cm) { return cm*100.0* 72.0/2.54; };//scalefactor* cm*100.0* 72.0/2.54; };
    /** \brief convert a length in DTP points into a value in meters*/
    inline double ptcm(double dtp) { return dtp/100.0/72.0*2.54; };//dtp/scalefactor/100.0/72.0*2.54; };
    /** \brief convert a position in the user system into a value in DTP points, as expected by cairo  */
    double mtpx(double m);
    /** \brief convert a position in th user system into a value in DTP points, as expected by cairo */
    double mtpy(double m);

    /** \brief convert a length (parallel to x axis) in the user coordinate system into DTP points*/
    double lxtp(double l);
    /** \brief convert a length (parallel to y axis) in the user coordinate system into DTP points*/
    double lytp(double l);
    /** \brief convert a length (parallel to x axis) in DTP points system into the user coordinate*/
    double ptlx(double l);
    /** \brief convert a length (parallel to y axis) in DTP points system into the user coordinate*/
    double ptly(double l);
    /** \brief convert a x-position in DTP points system into the user coordinate*/
    double ptmx(double m);
    /** \brief convert a y-position in DTP points system into the user coordinate*/
    double ptmy(double m);

    void restore_dashes();

    /** \brief current font name */
    std::string fontname;
    /** \brief currently draw italic text? */
    bool italic;
    /** \brief currently draw bold text? */
    bool bold;
    /** \brief currentfont size */
    double fontsize;

    /** \brief do we output as PNG file? */
    bool isPNG;
    /** \brief filename of the output file */
    std::string cairofilename;
  public:
    /** \brief set an offset in x direction on the paper (in paper coordiunates/DTP points!) */
    SetMacro(double, xoffset);
    /** \brief get the offset in x direction on the paper (in paper coordiunates/DTP points!) */
    GetMacro(double, xoffset);
    /** \brief set an offset in y direction on the paper (in paper coordiunates/DTP points!) */
    SetMacro(double, yoffset);
    /** \brief get the offset in y direction on the paper (in paper coordiunates/DTP points!) */
    GetMacro(double, yoffset);
    /** \brief return the cairo surface */
    GetMacro(cairo_surface_t*, surface);
    /** \brief return the cairo context */
    GetMacro(cairo_t*, cr);

    /** \brief get the font name */
    GetMacro(std::string, fontname);
    /** \brief set the font name */
    SetMacro(std::string, fontname);
    /** \brief set the font flag italic */
    SetMacro(bool, italic);
    /** \brief get the font flag italic */
    GetMacro(bool, italic);
    /** \brief set the font flag bold */
    SetMacro(bool, bold);
    /** \brief get the font flag bold */
    GetMacro(bool, bold);
    /** \brief set the font size */
    SetMacro(double, fontsize);
    /** \brief get the font size */
    GetMacro(double, fontsize);

    /** \brief class constructor calls plotter::init(-1,1,-1,1) */
    plotter();
    /** \brief class destructor */
    ~plotter();
    /** \brief open a PDF file and start drawing. \c width gives the width of the drawing paper in centimeter. \c width=-1 means that paperwidth=coordsystem width. The hieght is beeing determined using the aspect ration 1 (Call init() before this method) */
    int file_open_pdf(std::string filename, double width=-1, double height=-1);
    /** \brief open a PostScript file. \c width gives the width of the drawing paper in centimeter. \c width=-1 means that paperwidth=coordsystem width. The hieght is beeing determined using the aspect ration 1  (Call init() before this method) */
    int file_open_ps(std::string filename, double width=-1, double height=-1);
    /** \brief open a PNG file. \a width gives the width of the image in pixels. \c width=-1 means that
     *         the width will be determined automatically (based on a width of 500 pixels and the aspect ration
     *         of the user coordinate system (Call init() before this method). \a height gives the height
     *         of the image in pixels. If \c height=-1 the height will be determined automatically.
     */
    int file_open_png(std::string filename, long width=-1, long height=-1);
    /** \brief close an opened file and stop drawing */
    int file_close();

    /** \brief set the linewidth of the next objects in units of the custom coordinate system*/
    void set_line_width(double width);
    /** \brief set the source color as a RGB color (A=1)*/
    void set_source_rgb(double r, double g, double b);
    /** \brief set the source color as gray value (R=G=B=g, A=1)*/
    void set_source_gray(double g);
    /** \brief set the source color as gray value (R=G=B=g) with transparency information*/
    void set_source_graya(double g, double a);
    /** \brief set the source color as a RGBA color*/
    void set_source_rgba(double r, double g, double b, double a);
    /** \brief set the source color*/
    void set_source(p_color color);


    /** \brief set the fill color as a RGB color (A=1)*/
    void set_fill_rgb(double r, double g, double b);
    /** \brief set the fill color as gray value (R=G=B=g, A=1)*/
    void set_fill_gray(double g);
    /** \brief set the fill color as gray value (R=G=B=g) with transparency information*/
    void set_fill_graya(double g, double a);
    /** \brief set the fill color as a RGBA color*/
    void set_fill_rgba(double r, double g, double b, double a);
    /** \brief set the fill color*/
    void set_fill(p_color color);

    /** \brief set the stroke color as a RGB color (A=1)*/
    void set_stroke_rgb(double r, double g, double b);
    /** \brief set the stroke color as gray value (R=G=B=g, A=1)*/
    void set_stroke_gray(double g);
    /** \brief set the stroke color as gray value (R=G=B=g) with transparency information*/
    void set_stroke_graya(double g, double a);
    /** \brief set the stroke color as a RGBA color*/
    void set_stroke_rgba(double r, double g, double b, double a);
    /** \brief set the stroke color*/
    void set_stroke(p_color color);

    /** \brief set 'dashing' of lines, low-level interface, see <a href="http://www.cairographics.org/manual/cairo-cairo-t.html#cairo-set-dash">cairo documentation</a> or this <a href="http://cairographics.org/samples/dash/">example</a> for more details*/
    void set_dash_ll(double *dashes, int num_dashes, double offset);

    /** \brief set 'dashing' of lines, from a predefined set of line styles
     *
     * This is a list of allowed line styles:
     *  - 0: solid line
     *  - 1: dotted line
     *  - 2: dashed line
     *  - 3: dash-dot line
     *  - 4: long dashed line
     *  - 5: dash-dot-dot line
     * .
     */
    void set_dash(int style);


    /** \brief plot a filled rectangle from <i>(x,y)</i> with width \c xw and height \c yw */
    void filled_rectangle(double x, double y, double xw, double yw);
    /** \brief plot a filled circle around <i>(x,y)</i> with radius \c r */
    void filled_circle(double x, double y, double r);
    /** \brief plot a rectangle from <i>(x,y)</i> with width \c xw and height \c yw */
    void rectangle(double x, double y, double xw, double yw);
    /** \brief plot a circle around <i>(x,y)</i> with radius \c r */
    void circle(double x, double y, double r);

    void plot_matrix(double x0, double y0, double xwidth, double ywidth, double* data, long countx, long county);

    /** \brief plot a straight line from <i>(x,y)</i> to <i>(x1,y1)</i> */
    void line(double x, double y, double x1, double y1);

    /** \brief output some text at <i>(x,y)</i>. Use set_fontname(), set_fontsize(), set_italic() and set_bold() to determine the font style.*/
    void text(double x, double y, std::string text);
    /** \brief output some text centered around <i>(x,y)</i>.  Use set_fontname(), set_fontsize(), set_italic() and set_bold() to determine the font style.
     *
     * This routine first calculates the text extents and then calculates an offset so that the text
     * is centered around (x,y), so it calculates:
     * \f[ x'=x-\frac{\mathrm{textwidth}}{2} \f]
     * \f[ y'=y-\frac{\mathrm{textheight}}{2} \f]
     */
    void centered_text(double x, double y, std::string text);
    /** \brief output some text right-aligned at <i>(x,y)</i> (sentered in y-direction).  Use set_fontname(), set_fontsize(), set_italic() and set_bold() to determine the font style.
     *
     * This routine first calculates the text extents and then calculates an offset so that the text
     * is centered around (x,y), so it calculates:
     * \f[ x'=x-\mathrm{textwidth} \f]
     * \f[ y'=y-\frac{\mathrm{textheight}}{2} \f]
     */
    void rightaligned_text(double x, double y, std::string text);
    /** \brief output some text at <i>(x,y)</i> The text may also contain special characters like linebreaks!
     */
    void textblock(double x, double y, std::string text, p_text_alignment talign=pLeftAlign, p_halignment halign=phLeft, p_valignment valign=pvTop );
    /** \brief return the height of the given text
     */
    double textheight(std::string text);
    /** \brief return the width of the given text
     */
    double textwidth(std::string text);

    /** \brief initialize the user coordinate system. Call this before ana \c open_file_*() method! */
    void init(double xmin, double xmax, double ymin, double ymax);
};

#endif /* plotter_h */
/*@}*/
