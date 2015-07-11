/**
 * \ingroup seqplot
 */
/*@{*/

/** \file cairosequenceplotter.h
 */

#ifndef CAIROSEQUENCEPLOTTER_H
#define CAIROSEQUENCEPLOTTER_H

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cstdio>
#include <stdint.h>

#include "jkiniparser.h"
#include "plotter.h"
#include "tools.h"
#include "sequencererror.h"
#include "channelmanager.h"
#include "sequencegenerator.h"
#include "basesequenceplotter.h"
#include "nisequencegenerator.h"


namespace sequencer {

/**
 * \brief virtual base class for all sequence plotter objects that use the cairo graphics library
 * \ingroup seqplot
 *
 */
class cairoSequencePlotter: public baseSequencePlotter
{
  protected:
    /** \brief internal: plotter object for drawing */
    plotter* p;

    /** \brief width of the drawing surface in surface coordinates (set by plot_*() ) */
    double swidth;
    /** \brief height of the drawing surface in surface coordinates (set by plot_*() ) */
    double sheight;

    /** \brief name of the font used for the plot */
    std::string fontname;

    /** \brief width of coordinate axis lines */
    double coord_line_width;
    /** \brief x position of start of the x axis */
    double coord_0line_x;
    /** \brief x position of the y axis */
    double coord_y0line_x;
    /** \brief analog channels: half height of the plot; digital channels: height of the plot in paper coordinates */
    double coord_hheight;
    /** \brief height of an analog plot in paper coordinates */
    double plot_aheight;
    /** \brief height of a digital plot in paper coordinates */
    double plot_dheight;
    /** \brief line width of plotted graph */
    double graph_line_width;
    /** \brief font size for graph titles (channel names) */
    double title_fontsize;
    /** \brief x-position of the graph titles */
    double title_x;
    /** \brief font size for axis labels */
    double label_fontsize;
    /** \brief t=0 x-position for the graphs */
    double graph_x0;
    /** \brief same as graph_x0, but given in pixel => used to get scaling for text and distances ... */
    long graph_x0_pix;
    /** \brief line width of coordinate axis ticks */
    double coord_tick_line_width;
    /** \brief height of an axis tick */
    double tick_height;
    /** \brief height of an axis tick wuíth associated label */
    double bigtick_height;
    /** \brief distance between two timesteps on x axis */
    double coord_x_1range;
    /** \brief line width of a marker line */
    double marker_line_width;
    /** \brief fontsize of a marker text */
    double marker_fontsize;
    /** \brief space below graphs for markers */
    double marker_space;
    /** \brief additional length of x axis */
    double axis_xadd;
    /** \brief additional length of y axis above and below max/min marker */
    double axis_yadd;

    /** \brief internal: maximum number of ticks in a graph */
    int max_ticks;
    /** \brief internal: maximum width of the generated pixel graphic in pixels */
    int pix_maxwidth;
    /** \brief internal: minimum width of the generated pixel graphic in pixels */
    int pix_minwidth;
    /** \brief internal: heght of one sub-graph in pixels*/
    int graphheight_pix;
    /** \brief number of grid lines in y-direction for analog channels */
    int grid_analogline_number;
    /** \brief width of grid lines */
    double grid_linewidth;
    /** \brief font size for scaling marker on the grid */
    double grid_fontsize;

    /** \brief if this is set >0, then the plots will have this width and not an automatically defined width in pixels*/
    int fixed_width;
    /** \brief scaling factor for the paranmeters, like font sizes, graph_x0 ... */
    double surrounding_scale;

    /** \brief color of graph lines */
    p_color color_graph;
    /** \brief color of warnings */
    p_color color_warning;
    /** \brief color of errors */
    p_color color_error;
    /** \brief color of infos */
    p_color color_info;
    /** \brief color of the grid */
    p_color color_grid;
    /** \brief color of coordinate system */
    p_color color_coordsys;
    /** \brief color of the background */
    p_color color_background;

    /** \brief plot coordinate systems for all channels in the object */
    virtual void plot_coord_systems();
    /** \brief plot the actual data (graphs) */
    virtual void plot_graphs()=0;
    /** \brief plot the markers that are defined in the vector markers */
    virtual void plot_markers();
    /** \brief plot a grid */
    virtual void plot_grid();
    /** \brief plot the background (basically a uniformly colored surface! */
    virtual void plot_background();


    /** \brief returns the y coordinate of the 0-line of  <i>n</i>-th coordinate system  */
    virtual double get_coord_center(unsigned long n);
    /** \brief returns the y coordinate of the left-bottom corner of  <i>n</i>-th coordinate system  */
    virtual double get_plot_y0(unsigned long n);
    /** \brief returns the y coordinate of the left-top corner of  <i>n</i>-th coordinate system  */
    virtual double get_plot_ty0(unsigned long n);
    /** \brief returns the estimated height of the complete graphic */
    virtual double calc_height();

    /** \brief returns the scale factor for plot of channel n */
    virtual double get_plot_scale(unsigned long n);
    /** \brief returns the shift for plot of channel n */
    virtual double get_plot_shift(unsigned long n);


    /** \brief if \c plotScaled is true then the plot axes are the values put into the low-level sequence generator if it is false the plot reflects the actual output of the DACs */
    bool plot_scaled;


  public:

    /** \brief control whether the real DAC output (false) or the scaled (true) values shold be plottet */
    SetMacro(bool, plot_scaled);
    /** \brief return whether the real DAC output (false) or the scaled (true) values shold be plottet */
    GetMacro(bool, plot_scaled);
    /** \brief set the height of one subgraph in pixels */
    SetMacro(int, graphheight_pix);
    /** \brief return the height of one subgraph in pixels */
    GetMacro(int, graphheight_pix);
    /** \brief set the width of the plot. If the width shall be determined automatically, give -1 (default) */
    SetMacro(int, fixed_width);
    /** \brief set the \a fixed_width of the plot.*/
    GetMacro(int, fixed_width);
    /** \brief class constructor */
    cairoSequencePlotter();
  	/** \brief class destructor */
  	~cairoSequencePlotter();


  	/** \brief plot the sequence currently represented by the class into a PNG file \c filename that is \c height pixels high. The width is beeing determined automatically. If \c height=-1 then \c height=plotheight_pix*num_plots+50. This is done in order to allow fixed font size independent of the length of a sequence. */
  	virtual void plot_png(std::string filename, long height=-1);
  	/** \brief plot a part [tstart .. tstop] of the sequence currently represented by the class into a PNG file \c filename that is \c height pixels high. The width is beeing determined automatically. If \c height=-1 then \c height=plotheight_pix*num_plots+50.  This is done in order to allow fixed font size independent of the length of a sequence. If \c tstop=-1 then the plot ends at the end of the sequence.*/
  	virtual void plot_png(std::string filename, double tstart, double tstop=-1, long height=-1);


    /** \todo reimplement baseSequenceParser::plot_pdf() */
  	/** \brief plot the sequence currently represented by the class into a DPF file \c filename that is \c height centimeters high. The width is beeing determined automatically. This is done in order to allow fixed font size independent of the length of a sequence. */
  	virtual void plot_pdf(std::string filename, double height=-1);
  	/** \brief plot a part [tstart .. tstop] of the sequence currently represented by the class into a PDF file \c filename that is \c height centimeters high. The width is beeing determined automatically. This is done in order to allow fixed font size independent of the length of a sequence.  If \c tstop=-1 then the plot ends at the end of the sequence. */
  	virtual void plot_pdff(std::string filename, double tstart, double tstop=-1, double height=-1);

    /** \todo reimplement baseSequenceParser::plot_ps() */
  	/** \brief plot the sequence currently represented by the class into a PostScript file \c filename that is \c height centimeters high. The width is beeing determined automatically. This is done in order to allow fixed font size independent of the length of a sequence. */
  	virtual void plot_ps(std::string filename, double height=-1);
  	/** \brief plot a part [tstart .. tstop] of the sequence currently represented by the class into a PostScript file \c filename that is \c height centimeters high. The width is beeing determined automatically. This is done in order to allow fixed font size independent of the length of a sequence.  If \c tstop=-1 then the plot ends at the end of the sequence. */
  	virtual void plot_ps(std::string filename, double tstart, double tstop=-1, double height=-1);

};

}
#endif // CAIROSEQUENCEPLOTTER_H


/*@}*/

