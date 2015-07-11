
#include "cairosequenceplotter.h" // class's header file


namespace sequencer {

// class constructor
cairoSequencePlotter::cairoSequencePlotter():
    baseSequencePlotter()
{
  // all values of the next variables (parameters) are given in plotter canvas coordinates. For each
  // single coordinate system there is a space o height 4. On the time axis a timestep of sample_timestep
  // is represented by a distance of 1.
  //
  // changes these variables to change the look of the graphs:

  coord_line_width=0.03;       // line width of coordinate system
  coord_tick_line_width=0.025; // line width of coordinate axis ticks
  coord_0line_x=2.4;           // x position of start of the x axis
  coord_y0line_x=2.5;          // x position of the y axis
  coord_hheight=1.5;           // analog channels: half height of the plot;
                               // digital channels: height of the plot in paper coordinates
  plot_aheight=4;              // height of an analog plot in paper coordinates
  plot_dheight=2.5;            // height of a digital plot in paper coordinates
  graph_line_width=0.04;       // line width of plotted graph
  title_fontsize=0.5;          // font size for graph titles (channel names)
  label_fontsize=0.25;         // font size for axis labels
  graph_x0=3;                  // t=0 x-position for the graphs
  title_x=0.2;                 // x-position of the graph titles
  tick_height=0.2;             // height of an axis tick
  bigtick_height=0.3;          // height of an axis tick with label
  coord_x_1range=0.05;         // distance between two timesteps on x axis
  max_ticks=200;               // maximum number of ticks in a graph
  marker_line_width=0.03;      // line width of a marker line
  marker_fontsize=0.35;        // fontsize of a marker text
  marker_space=6;              // space below plots for markers
  axis_xadd=4;                 // additional length of x axis
  axis_yadd=0.15;              // additional length of y axis above and below max/min marker
  pix_maxwidth=3000;           // maximum width of the generated pixel graphic in pixels
  pix_minwidth=600;            // minimum width of the generated pixel graphic in pixels
  graphheight_pix=200;         // height of one graph in pixel.
  graph_x0_pix=150;            // same as graph_x0, but given in pixel => used to get scaling for text and distances ...
  grid_analogline_number=11;   // number of grid lines for ananlog channels
  grid_linewidth=0.02;         // width of grid lines
  grid_fontsize=0.225;         // fontsize for the scaling markers of the grid
  fixed_width=-1;
  surrounding_scale=1;
  plot_scaled=false;
  fontname="arial";

  color_graph.r=0; color_graph.g=0; color_graph.b=0.9; color_graph.a=1;
  color_warning.r=0; color_warning.g=0.4; color_warning.b=0.2; color_warning.a=0.5;
  color_error.r=0.8; color_error.g=0; color_error.b=0; color_error.a=0.5;
  color_info.r=0; color_info.g=0.56; color_info.b=0.7; color_info.a=0.5;
  color_grid.r=0.4; color_grid.g=0.4; color_grid.b=0.4; color_grid.a=1;
  color_coordsys.r=0; color_coordsys.g=0; color_coordsys.b=0; color_coordsys.a=1;
  color_background.r=0.95; color_background.g=0.95; color_background.b=0.95; color_background.a=1;

}

// class destructor
cairoSequencePlotter::~cairoSequencePlotter()
{
}

double cairoSequencePlotter::get_coord_center(unsigned long n){
  double ret=1;
  if (n>0)
    for (unsigned long i=0; i<n; i++) {
      if ((*cmanager)[channames[i]].type==cmAnalogOutput) { // ananlog channel
        ret+=plot_aheight;
      } else ret+=plot_dheight;
    }
  ret+=2;
  return ret;//1+2*(2*n+1);
}

double cairoSequencePlotter::get_plot_y0(unsigned long n){
  double ret=get_plot_ty0(n);
  if ((*cmanager)[channames[n]].type==cmAnalogOutput) {
    ret+=plot_aheight-(plot_aheight-2*coord_hheight)/2.0;
  } else {
    ret+=plot_dheight-(plot_dheight-coord_hheight)/2.0;
  }

  return ret;//1+2*(2*n+1);
}

double cairoSequencePlotter::get_plot_ty0(unsigned long n){
  double ret=1;
  if (n>0)
    for (unsigned long i=0; i<n; i++) {
      if ((*cmanager)[channames[i]].type==cmAnalogOutput) { // ananlog channel
        ret+=plot_aheight;
      } else ret+=plot_dheight;
    }
  return ret;//1+2*(2*n+1);
}

double cairoSequencePlotter::calc_height(){
  double ret=1;
  if (chnum>0)
    for (unsigned long i=0; i<chnum; i++) {
      if ((*cmanager)[channames[i]].type==cmAnalogOutput) { // ananlog channel
        ret+=plot_aheight;
      } else ret+=plot_dheight;
    }
  ret+=marker_space;
  return ret;//1+2*(2*n+1);
}

double cairoSequencePlotter::get_plot_scale(unsigned long n){
    cm_channel c=(*cmanager)[channames[n]];
    /* the standard range for an analog port is -10..10, so this has to be scaled to
       min .. max or valmin .. valmax */
    if (c.type==cmDigitalOutput) return coord_hheight;
    if (plot_scaled) {
      //std::cout<<"scaled: "<<c.valmin<<" ... "<<c.valmax<<std::endl;
      return 2*coord_hheight/fabs(c.valmax-c.valmin);
    } else {
      //std::cout<<"non-scaled: "<<c.min<<" ... "<<c.max<<std::endl;
      return 2*coord_hheight/fabs(c.max-c.min);
    }
};

double cairoSequencePlotter::get_plot_shift(unsigned long n){
  cm_channel c=(*cmanager)[channames[n]];
  if (c.type==cmDigitalOutput) { // digital channel
    return 0;
  } else {
    /* the standard range for an analog port is -10..10 with 0-line in the middle,
       so this has to be scaled to min .. max or valmin .. valmax.

       shift has to be subtracted from the value befor scaling and so
       shifts the points to the right position.
    */
    if (plot_scaled) {
      return c.valmin;
    } else {
      return c.min;
    }
  }
};


void cairoSequencePlotter::plot_background(){
  p->set_stroke(color_background); p->set_source(color_background); p->set_fill(color_background);
  p->filled_rectangle(0,0,swidth*2,swidth*2);
  p->set_stroke(color_coordsys); p->set_source(color_coordsys); p->set_fill(color_coordsys);
}

void cairoSequencePlotter::plot_coord_systems(){
  if (chnum>0) {
    for (unsigned long i=0; i<chnum; i++) {
      cm_channel c=(*cmanager)[channames[i]];
      std::string lab;
      double t;
      double y0=get_plot_ty0(i);
      double y0yaxis=get_plot_y0(i);
      double shifty=get_plot_shift(i);
      double scaley=get_plot_scale(i);
      double cmin=c.min;
      double ynn;
      if (plot_scaled) cmin=c.valmin;
      double cmax=c.max;
      if (plot_scaled) cmax=c.valmax;
      double y0xaxis=y0yaxis-scaley*(0-shifty); // y-coordinate of x axis
      if (0<cmin) y0xaxis=y0yaxis-scaley*(cmin-shifty); // if min..max range starts above 0 plot x-axis at min-y-position
      if (0>cmax) y0xaxis=y0yaxis-scaley*(cmax-shifty); // if min..max range end below 0 plot x-axis at max-y-position

      // output channel name
      p->set_fontsize(title_fontsize*surrounding_scale);
      p->set_bold(true); p->set_italic(false);
      p->set_fontname(fontname);
      p->set_stroke(color_coordsys);
      p->set_source(color_coordsys);
      p->set_fill(color_coordsys);
      p->text(title_x*surrounding_scale, y0+0.8*title_fontsize, channames[i]);

      // draw x-axis
      p->set_line_width(coord_line_width*surrounding_scale);
      p->set_dash(0);
      p->set_stroke(color_coordsys);
      p->line(coord_0line_x*surrounding_scale, y0xaxis, t_duration/get_sample_timestep()*coord_x_1range+axis_xadd*surrounding_scale, y0xaxis);
      p->set_line_width(coord_tick_line_width*surrounding_scale);
      p->set_bold(false);
      p->set_fontsize(label_fontsize*surrounding_scale);
      double tstep=1;

      // plot axis labels
      while(t_duration/get_sample_timestep()/tstep>max_ticks/10) {
        tstep*=10;
      }
      for (double tx=0; tx<t_duration/get_sample_timestep(); tx+=tstep) {
        t=t_start+get_sample_timestep()*tx;
        lab=floattounitstr(t/1e6, "s");
        p->centered_text(graph_x0*surrounding_scale+tx*coord_x_1range, y0xaxis+1.5*tick_height, lab);
        p->line(graph_x0*surrounding_scale+tx*coord_x_1range, y0xaxis-bigtick_height/2.0, graph_x0*surrounding_scale+tx*coord_x_1range, y0xaxis+bigtick_height/2.0);
      }
      // be sure to plot last label:
      t=t_start+t_duration;
      lab=floattounitstr(t/1e6, "s");
      p->centered_text(graph_x0*surrounding_scale+t_duration/get_sample_timestep()*coord_x_1range, y0xaxis+1.5*tick_height, lab);
      p->line(graph_x0*surrounding_scale+t_duration/get_sample_timestep()*coord_x_1range, y0xaxis-bigtick_height/2.0, graph_x0*surrounding_scale+t_duration/get_sample_timestep()*coord_x_1range, y0xaxis+bigtick_height/2.0);
      /*// plot axis ticks
      tstep/=10;
      t=t_start+tstep*get_sample_timestep();
      lab=floattounitstr(t/1e6, "s");
      p->centered_text(graph_x0*surrounding_scale+tstep*coord_x_1range, y0xaxis+1.5*tick_height, lab);
      p->line(graph_x0*surrounding_scale+tstep*coord_x_1range, y0xaxis-bigtick_height/2.0, graph_x0*surrounding_scale+tstep*coord_x_1range, y0xaxis+bigtick_height/2.0);
      t=t_start+tstep*2.0*get_sample_timestep();
      lab=floattounitstr(t/1e6, "s");
      p->centered_text(graph_x0*surrounding_scale+2*tstep*coord_x_1range, y0xaxis+1.5*tick_height, lab);*/
      p->line(graph_x0*surrounding_scale+2*tstep*coord_x_1range, y0xaxis-bigtick_height/2.0, graph_x0*surrounding_scale+2*tstep*coord_x_1range, y0xaxis+bigtick_height/2.0);
      for (double tx=0; tx<=t_duration/get_sample_timestep(); tx+=tstep) {
        p->line(graph_x0*surrounding_scale+tx*coord_x_1range, y0xaxis-tick_height/2.0, graph_x0*surrounding_scale+tx*coord_x_1range, y0xaxis+tick_height/2.0);
      }
      p->set_bold(true);
      p->centered_text(t_duration/get_sample_timestep()*coord_x_1range+axis_xadd*surrounding_scale, y0xaxis-1.2*tick_height, "time");

      // draw y-axis
      if ((*cmanager)[channames[i]].type==cmAnalogOutput) { // ananlog channel
          p->set_line_width(grid_linewidth*surrounding_scale);
          p->line(coord_y0line_x*surrounding_scale, y0yaxis+axis_yadd, coord_y0line_x*surrounding_scale, y0yaxis-2*coord_hheight-axis_yadd); // y axis
          p->set_bold(false);
          p->set_line_width(coord_tick_line_width*surrounding_scale);
          // max label
          ynn=y0yaxis-scaley*(cmax-shifty);
          p->line(coord_y0line_x*surrounding_scale-tick_height*surrounding_scale/2.0, ynn, coord_y0line_x*surrounding_scale+tick_height/2.0, ynn); // tick
          p->rightaligned_text(coord_y0line_x*surrounding_scale-tick_height*surrounding_scale, ynn, floattostr(cmax)); // label
          // min label
          ynn=y0yaxis-scaley*(cmin-shifty);
          p->line(coord_y0line_x*surrounding_scale-tick_height*surrounding_scale/2.0, ynn, coord_y0line_x*surrounding_scale+tick_height/2.0, ynn); // tick
          p->rightaligned_text(coord_y0line_x*surrounding_scale-tick_height*surrounding_scale, ynn, floattostr(cmin)); // label
          // 0 label if in min..max range
          if ((0>cmin)&&(0<cmax)) {
            p->set_stroke(color_coordsys);
            ynn=y0yaxis-scaley*(0-shifty);
            p->rightaligned_text(coord_y0line_x*surrounding_scale-tick_height*surrounding_scale, ynn, floattostr(0)); // label
          }
          p->set_line_width(coord_line_width*surrounding_scale);
      } else { // digital channel
          p->set_line_width(grid_linewidth*surrounding_scale);
          p->line(coord_y0line_x*surrounding_scale, y0yaxis+axis_yadd, coord_y0line_x*surrounding_scale, y0yaxis-coord_hheight-axis_yadd); // y axis
          p->set_bold(false);
          p->set_line_width(coord_tick_line_width*surrounding_scale);
          // max label
          ynn=y0yaxis-scaley*(1-shifty);
          p->line(coord_y0line_x*surrounding_scale-tick_height*surrounding_scale/2.0, ynn, coord_y0line_x*surrounding_scale+tick_height/2.0, ynn); // tick
          p->rightaligned_text(coord_y0line_x*surrounding_scale-tick_height*surrounding_scale, ynn, "1"); // label
          // min label
          ynn=y0yaxis-scaley*(0-shifty);
          p->line(coord_y0line_x*surrounding_scale-tick_height*surrounding_scale/2.0, ynn, coord_y0line_x*surrounding_scale+tick_height/2.0, ynn); // tick
          p->rightaligned_text(coord_y0line_x*surrounding_scale-tick_height*surrounding_scale, ynn, "0"); // label
          p->set_stroke(color_coordsys); p->set_dash(0);
          p->set_line_width(coord_line_width*surrounding_scale);
      }
    }
  }
}


void cairoSequencePlotter::plot_grid(){
  if (chnum>0) {
    for (unsigned long i=0; i<chnum; i++) {
      cm_channel c=(*cmanager)[channames[i]];
      std::string lab;
      double t;
      double y0=get_plot_ty0(i);
      double y0yaxis=get_plot_y0(i);
      double shifty=get_plot_shift(i);
      double scaley=get_plot_scale(i);
      double cmin=c.min;
      double ynn, ymin, ymax;
      if (plot_scaled) cmin=c.valmin;
      double cmax=c.max;
      if (plot_scaled) cmax=c.valmax;
      double y0xaxis=y0yaxis-scaley*(0-shifty); // y-coordinate of x axis
      if (0<cmin) y0xaxis=y0yaxis-scaley*(cmin-shifty); // if min..max range starts above 0 plot x-axis at min-y-position
      if (0>cmax) y0xaxis=y0yaxis-scaley*(cmax-shifty); // if min..max range end below 0 plot x-axis at max-y-position

      // determine tstep
      double tstep=1;
      while(t_duration/get_sample_timestep()/tstep>max_ticks/10) {
        tstep*=10;
      }
      double grid_distance=(cmax-cmin)/(grid_analogline_number-1);


      // draw y-grid ... only on analog channels ... on digital channels there is only a max-line at 1
      if ((*cmanager)[channames[i]].type==cmAnalogOutput) { // ananlog channel
          p->set_line_width(grid_linewidth*surrounding_scale);
          p->set_stroke(color_grid);
          p->set_dash(1);
          for (double yg=cmin; yg<=cmax; yg+=grid_distance) {
            ynn=y0yaxis-scaley*(yg-shifty);
            p->line(coord_y0line_x*surrounding_scale, ynn, graph_x0*surrounding_scale+t_duration/get_sample_timestep()*coord_x_1range, ynn);
          }
          p->set_stroke(color_coordsys);
          p->set_dash(0);
          ymin=y0yaxis-scaley*(cmin-shifty);
          ymax=y0yaxis-scaley*(cmax-shifty);
      } else if ((*cmanager)[channames[i]].type==cmDigitalOutput) { // ananlog channel
          p->set_line_width(grid_linewidth*surrounding_scale);
          p->set_stroke(color_grid);
          p->set_dash(1);
          ynn=y0yaxis-scaley*(1-shifty);
          p->line(coord_y0line_x*surrounding_scale, ynn, graph_x0*surrounding_scale+t_duration/get_sample_timestep()*coord_x_1range, ynn);
          p->set_stroke(color_coordsys);
          p->set_dash(0);
          ymin=y0yaxis-scaley*(cmin-shifty);
          ymax=y0yaxis-scaley*(cmax-shifty);
      }

      // draw grid for time axis ... on every big tick!
      p->set_line_width(grid_linewidth*surrounding_scale);
      p->set_dash(1);
      p->set_fontsize(grid_fontsize*surrounding_scale);
      p->set_bold(false); p->set_italic(false);
      p->set_fontname(fontname);
      p->set_stroke(color_grid);
      p->set_source(color_grid);
      p->set_fill(color_grid);
      for (double tx=0; tx<=t_duration/get_sample_timestep(); tx+=tstep) {
        t=t_start+get_sample_timestep()*tx;
        p->line(graph_x0*surrounding_scale+tx*coord_x_1range, ymin, graph_x0*surrounding_scale+tx*coord_x_1range, ymax);

        std::string lab;
        // now draw two markers that show the scaling of the grid
        if (tx==0) {
          lab=floattounitstr(tstep*get_sample_timestep()/1e6, "s");
          double tw=1.1*p->textwidth(lab);
          double th=1.1*p->textheight(lab);
          double xxx=graph_x0*surrounding_scale+(tstep/2.0)*coord_x_1range;
          double yyy=y0yaxis-scaley*(cmax-shifty);
          p->set_line_width(2.0*grid_linewidth*surrounding_scale);
          p->set_dash(0);
          p->line(graph_x0*surrounding_scale, yyy-0.5*th, graph_x0*surrounding_scale+tstep*coord_x_1range, yyy-0.5*th);
          p->line(graph_x0*surrounding_scale, yyy-0.75*th, graph_x0*surrounding_scale+tstep/10.0*coord_x_1range, yyy-0.75*th);
          p->set_dash(1);
          p->set_line_width(grid_linewidth*surrounding_scale);
          p->centered_text(xxx, yyy-1.5*th, lab);
          lab=floattounitstr(tstep/10.0*get_sample_timestep()/1e6, "s");
          xxx=graph_x0*surrounding_scale+(tstep/10.0/2.0)*coord_x_1range;
          p->centered_text(xxx, yyy-2.0*th, lab);

          if ((*cmanager)[channames[i]].type==cmAnalogOutput) {
            lab=floattostr(grid_distance);
            tw=1.1*p->textwidth(lab);
            th=1.1*p->textheight(lab);
            xxx=graph_x0*surrounding_scale+(tx)*coord_x_1range;
            yyy=y0yaxis-scaley*(cmax-grid_distance/2.0-shifty);
            p->set_dash(0);
            p->set_line_width(2.0*grid_linewidth*surrounding_scale);
            p->line(xxx, y0yaxis-scaley*(cmax-shifty), xxx, y0yaxis-scaley*(cmax-grid_distance-shifty));
            p->set_dash(1);
            p->set_line_width(grid_linewidth*surrounding_scale);
            p->centered_text(xxx-tw*1.2, yyy, lab);
          }
        }
      }
      p->set_dash(0);
      p->set_stroke(color_coordsys);
      p->set_source(color_coordsys);
      p->set_fill(color_coordsys);
    }
  }
}


void cairoSequencePlotter::plot_markers(){
  std::map<double, double> offsets;
  std::map<double, double> widths;
  /* take care that markers at the same time are beeing positione below each other

     offsets is initially 0 for all times where a marker is positioned. The entry
     for the time of a marker is increase whenever a marker is drawn. So markers that
     will be drawn at the same position may be below the already existing marker.

     widths saves the max. width of the markers at each time, so a collision detection
     is possible and markers that intersect may be plottet a bit below.
  */
  if ((markers.size()>0)&&(chnum>0)) {
    for (unsigned long i=0; i<markers.size(); i++) {
      offsets[markers[i].time]=0;
      widths[markers[i].time]=0;
    }
  }
  p->set_dash(0);
  p->set_line_width(marker_line_width*surrounding_scale);
  p->set_bold(false);
  p->set_italic(true);
  p->set_fontsize(marker_fontsize*surrounding_scale);
  if ((markers.size()>0)&&(chnum>0)) {
    for (unsigned long i=0; i<markers.size(); i++) {
      widths[markers[i].time]=mmax(widths[markers[i].time],p->textwidth(markers[i].text));
    }
  }
  if ((markers.size()>0)&&(chnum>0)) {
    for (unsigned long i=0; i<markers.size(); i++) {
      p->set_dash(0);
      p->set_line_width(marker_line_width*surrounding_scale);
      p->set_bold(false);
      p->set_italic(true);
      p->set_fontsize(marker_fontsize*surrounding_scale);
      switch(markers[i].type) {
        case spInfo:
          p->set_stroke(color_info);
          p->set_source(color_info);
          p->set_fill(color_info);
          break;
        case spError:
          p->set_stroke(color_error);
          p->set_source(color_error);
          p->set_fill(color_error);
          break;
        case spWarning:
          p->set_stroke(color_warning);
          p->set_source(color_warning);
          p->set_fill(color_warning);
          break;
      }
      if ((markers[i].time>=t_start)&&(markers[i].time<=t_stop)) {
        double x=graph_x0*surrounding_scale+(markers[i].time-t_start)/t_duration*t_duration/get_sample_timestep()*coord_x_1range;
        double y0=get_plot_ty0(0);
        double y1=get_plot_y0(0);
        double w=p->textwidth(markers[i].text);
        if (chnum>=2) y1=get_plot_y0(chnum-1);
        // colision detection
        for(std::map<double, double>::const_iterator k=widths.begin(); k!=widths.end(); ++k) {
          if (k->first!=markers[i].time) {
            double xx=graph_x0*surrounding_scale+(k->first-t_start)/t_duration*t_duration/get_sample_timestep()*coord_x_1range;
            double xxb=xx-k->second/2.0;
            double xxe=xx+k->second/2.0;
            if (!((x-w/2.0>xxe)||(x+w/2.0<xxb))) { // collision
              offsets[markers[i].time]=mmax(offsets[k->first], offsets[markers[i].time]);
            }
          }
        }

        p->line(x, y0, x, y1+1+offsets[markers[i].time]);
        p->textblock(x, y1+offsets[markers[i].time]+1+tick_height*0.2, markers[i].text, pCentered, phCenter, pvTop);
        offsets[markers[i].time]+=p->textheight(markers[i].text)*1.1;
      }
      p->set_bold(false);
      p->set_italic(false);
      p->set_stroke(color_coordsys);
      p->set_source(color_coordsys);
      p->set_fill(color_coordsys);
    }
  }
}


void cairoSequencePlotter::plot_png(std::string filename, long height){
  plot_png(filename, 0, get_end_time(), height);
}

void cairoSequencePlotter::plot_png(std::string filename, double tstart, double tstop, long hheight){
  long height=hheight;
  p=new plotter;
  t_start=tstart;
  if (t_start<0) t_start=0;
  t_stop=tstop;
  if (t_stop<0) t_stop=get_end_time();
  t_duration=t_stop-t_start;
  chnum=get_channel_count();
  chlist=false;
  if (plotchannels.size()>0) {
    chnum=plotchannels.size();
    chlist=true;
  }
  channames.clear();
  sort_channels();
  if (height<=0) height=graphheight_pix*chnum+50;
  swidth=t_duration/get_sample_timestep()*coord_x_1range+graph_x0*1.5;
  sheight=calc_height();

  if (fixed_width>0) {
    if (fixed_width<pix_minwidth) fixed_width=pix_minwidth;
    if (fixed_width>pix_maxwidth) fixed_width=pix_maxwidth;
  };

  // PNG specific code
  long pwidth=(fixed_width>0)?fixed_width:(long)(height*swidth/sheight);
  surrounding_scale=1.0*(graph_x0_pix/graph_x0*swidth/pwidth);
  swidth=t_duration/get_sample_timestep()*coord_x_1range+(graph_x0*1.2+axis_xadd*1.5)*surrounding_scale;
  double x1r=coord_x_1range; // save this to restore it later
  //std::cout<<"swidth="<<swidth<<",  sheight="<<sheight<<std::endl;
  //std::cout<<"pwidth="<<pwidth<<",  height="<<height<<std::endl;
  //std::cout<<"surrounding_scale="<<surrounding_scale<<std::endl;
  if (t_duration>0) while (swidth>pwidth/2.0 || pwidth>pix_maxwidth) { // make sure that the pixel width is not too big
    coord_x_1range/=1.2;
    surrounding_scale=1.0*(graph_x0_pix/graph_x0*swidth/pwidth)/1.2;
    swidth=t_duration/get_sample_timestep()*coord_x_1range+(graph_x0*1.2+axis_xadd*1.5)*surrounding_scale;
    pwidth=(fixed_width>0)?fixed_width:(long)(height*swidth/sheight);
  }
  //std::cout<<"swidth="<<swidth<<",  sheight="<<sheight<<std::endl;
  //std::cout<<"pwidth="<<pwidth<<",  height="<<height<<std::endl;
  //std::cout<<"surrounding_scale="<<surrounding_scale<<std::endl;
  if (t_duration>0 && pwidth<pix_minwidth) {
    while (pwidth<pix_minwidth && fixed_width<=0) { // make sure that there is not much more than one pixel per timestep
      coord_x_1range*=2;
      surrounding_scale=1.0*(graph_x0_pix/graph_x0*swidth/pwidth)*2.0;
      swidth=t_duration/get_sample_timestep()*coord_x_1range+(graph_x0*1.2+axis_xadd*1.5)*surrounding_scale;
      pwidth=(fixed_width>0)?fixed_width:(long)(height*swidth/sheight);
    }
  }
  surrounding_scale=1.0*(graph_x0_pix/graph_x0*swidth/pwidth);
  //std::cout<<"swidth="<<swidth<<",  sheight="<<sheight<<std::endl;
  //std::cout<<"pwidth="<<pwidth<<",  height="<<height<<std::endl;
  //std::cout<<"surrounding_scale="<<surrounding_scale<<std::endl;
  p->init(0, swidth, 0, sheight);
  p->file_open_png(filename, pwidth, height);

  int maxt=max_ticks;
  while (max_ticks*8>pwidth) { max_ticks/=2; }
  // -----------------

  plot_background();
  plot_markers();
  plot_grid();
  plot_coord_systems();
  plot_graphs();

  p->file_close();
  max_ticks=maxt;
  coord_x_1range=x1r; // restore old settings
  delete p;
  p=NULL;
}


void cairoSequencePlotter::plot_pdf(std::string filename, double height){
  plot_pdff(filename, 0, get_end_time(), height);
}

void cairoSequencePlotter::plot_pdff(std::string filename, double tstart, double tstop, double hheight){
  long height=trunc(hheight);
  p=new plotter;
  t_start=tstart;
  if (t_start<0) t_start=0;
  t_stop=tstop;
  if (t_stop<0) t_stop=get_end_time();
  t_duration=t_stop-t_start;
  chnum=get_channel_count();
  chlist=false;
  if (plotchannels.size()>0) {
    chnum=plotchannels.size();
    chlist=true;
  }
  channames.clear();
  sort_channels();
  if (height<=0) height=graphheight_pix*chnum+50;
  swidth=t_duration/get_sample_timestep()*coord_x_1range+graph_x0*1.5;
  sheight=calc_height();

  if (fixed_width>0) {
    if (fixed_width<pix_minwidth) fixed_width=pix_minwidth;
    if (fixed_width>pix_maxwidth) fixed_width=pix_maxwidth;
  }

  // PNG specific code
  long pwidth=(fixed_width>0)?fixed_width:(long)(height*swidth/sheight);
  surrounding_scale=1.0*(graph_x0_pix/graph_x0*swidth/pwidth);
  double x1r=coord_x_1range; // save this to restore it later
  if (t_duration>0) while (swidth>pwidth/2.0 || pwidth>pix_maxwidth) { // make sure that the pixel width is not too big
    coord_x_1range/=1.2;
    surrounding_scale=1.0*(graph_x0_pix/graph_x0*swidth/pwidth)/1.2;
    swidth=t_duration/get_sample_timestep()*coord_x_1range+(graph_x0*1.5)*surrounding_scale;
    pwidth=(fixed_width>0)?fixed_width:(long)(height*swidth/sheight);
  }
  if (t_duration>0 && pwidth<pix_minwidth) {
    while (pwidth<pix_minwidth && fixed_width<=0) { // make sure that there is not much more than one pixel per timestep
      coord_x_1range*=2;
      surrounding_scale=1.0*(graph_x0_pix/graph_x0*swidth/pwidth)*2.0;
      swidth=t_duration/get_sample_timestep()*coord_x_1range+(graph_x0*1.5)*surrounding_scale;
      pwidth=(fixed_width>0)?fixed_width:(long)(height*swidth/sheight);
    }
  }
  surrounding_scale=1.0*(graph_x0_pix/graph_x0*swidth/pwidth);
  //std::cout<<"swidth="<<swidth<<",  sheight="<<sheight<<std::endl;
  //std::cout<<"pwidth="<<pwidth<<",  height="<<height<<std::endl;
  //std::cout<<"surrounding_scale="<<surrounding_scale<<std::endl;
  p->init(0, swidth, 0, sheight);
  p->file_open_pdf(filename, pwidth, height);

  int maxt=max_ticks;
  while (max_ticks*8>pwidth) { max_ticks/=2; }
  // -----------------

  plot_background();
  plot_markers();
  plot_grid();
  plot_coord_systems();
  plot_graphs();

  p->file_close();
  max_ticks=maxt;
  coord_x_1range=x1r; // restore old settings
  delete p;
  p=NULL;

}

void cairoSequencePlotter::plot_ps(std::string filename, double height){
  plot_ps(filename, 0, get_end_time(), height);
}

void cairoSequencePlotter::plot_ps(std::string filename, double tstart, double tstop, double height){
  p=new plotter;
  t_start=tstart;
  t_stop=tstop;
  if (t_stop==-1) t_stop=get_end_time();
  t_duration=t_stop-t_start;
  chnum=get_channel_count();
  chlist=false;
  if (plotchannels.size()>0) {
    chnum=plotchannels.size();
    chlist=true;
  }
  channames.clear();
  sort_channels();
  double swidth=t_duration/get_sample_timestep()*coord_x_1range+7;
  double sheight=calc_height();
  p->init(0, swidth, 0, sheight);


  // PS specific code
  double pwidth=(height*swidth/sheight);
  p->file_open_ps(filename, pwidth);
  int maxt=max_ticks;
  while (max_ticks*tick_height*5>swidth) { max_ticks/=2; }
  // ----------------

  plot_markers();
  plot_coord_systems();
  plot_graphs();

  p->file_close();
  max_ticks=maxt;
  delete p;
  p=NULL;
}


}

