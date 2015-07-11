#include "plotter.h"

plotter::~plotter() {
  if (pdashes!=NULL) free(pdashes);   
}

plotter::plotter() {
  xoffset=40;
  yoffset=40;
  dash_style=0;
  pdashes=NULL;
  fontsize=0.05;
  fontname="sans";
  bold=false;
  italic=false;
  isPNG=false;
  init(-1,-1,1,1);
}

void plotter::init(double xxmin, double xmax, double yymin, double ymax){
  xmin=xxmin;
  ymin=yymin;
  xwidth=xmax-xxmin;
  ywidth=ymax-yymin;
};

double plotter::mtpx(double m){
  return (m-xmin)/(xwidth)*xpwidth;
};

double plotter::mtpy(double m){
  return (m-ymin)/(ywidth)*ypwidth;
};

double plotter::lxtp(double l) {
  return l/xwidth*xpwidth;
}

double plotter::lytp(double l) {
  return l/ywidth*ypwidth;
}

double plotter::ptlx(double l) {
  return l*xwidth/xpwidth;
}

double plotter::ptly(double l) {
  return l*ywidth/ypwidth;
}

double plotter::ptmx(double m) {
  return xmin+m*xwidth/xpwidth;
}

double plotter::ptmy(double m) {
  return ymin+m*ywidth/ypwidth;
}

int plotter::file_open_pdf(std::string filename, double width, double height){
  xpwidth=cmtp(xwidth/100.0)+2.0*xoffset;
  ypwidth=cmtp(ywidth/100.0)+2.0*yoffset;

  if (width==-1) {
    if (height>0) {
      ypwidth=cmtp(height/100.0);
    }
  } else {
    double ratio=ypwidth/xpwidth;
    xpwidth=cmtp(width/100.0);
    ypwidth=cmtp(width/100.0)*ratio;
    if (height>0) {
      ypwidth=cmtp(height/100.0);
    }
  }

  cairofile = fopen (filename.c_str(), "w");
  isPNG=false;
  cairofilename=filename;
  if (cairofile == NULL) {
  	fprintf (stderr, "Failed to open file %s for writing.\n", filename.c_str());
  	return 1;
  }

  //std::cout<<"pdf  "<<xpwidth<<"  --  "<<ypwidth<<std::endl;
  surface = cairo_pdf_surface_create (filename.c_str(), xpwidth, ypwidth);

  cr = cairo_create (surface);
  return 0;
}

int plotter::file_open_ps(std::string filename, double width, double height){
  xpwidth=cmtp(xwidth)+2.0*xoffset;
  ypwidth=cmtp(ywidth)+2.0*yoffset;

  if (width==-1) {
    if (height>0) {
      ypwidth=cmtp(height/100.0);
    }
  } else {
    double ratio=ypwidth/xpwidth;
    xpwidth=cmtp(width/100.0);
    ypwidth=cmtp(width/100.0)*ratio;
    if (height>0) {
      ypwidth=cmtp(height/100.0);
    }
  }
  //std::cout<<"ps   "<<xpwidth<<"  --  "<<ypwidth<<std::endl;

  cairofile = fopen (filename.c_str(), "w");
  isPNG=false;
  cairofilename=filename;
  if (cairofile == NULL) {
  	fprintf (stderr, "Failed to open file %s for writing.\n", filename.c_str());
  	return 1;
  }

  surface = cairo_ps_surface_create (filename.c_str(), xpwidth, ypwidth);

  cr = cairo_create (surface);
  return 0;
}

int plotter::file_open_png(std::string filename, long width, long height){
  isPNG=true;
  cairofilename=filename;
  xpwidth=500*ywidth/xwidth; 

  if (width==-1) {
    xpwidth=500;
    ypwidth=xpwidth/xwidth*ywidth; 
    if (height>0) {
      ypwidth=height;
    }
  } else {
    double ratio=ypwidth/xpwidth;
    xpwidth=width;
    ypwidth=width*ywidth/xwidth;
    if (height>0) {
      ypwidth=height;
    }
  }
  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, (long)xpwidth, (long)ypwidth);

  cr = cairo_create (surface);
  set_source_rgba(1,1,1,1);
  cairo_rectangle(cr, -10, -10, 2*xpwidth, 2*ypwidth);
  cairo_fill_preserve(cr);
  set_source_rgba(0,0,0,1);
  cairo_stroke (cr);
  return 0;
}

int plotter::file_close(){
  cairo_stroke (cr);
  if (!isPNG) {
    cairo_show_page (cr);
    cairo_destroy (cr);
    cairo_surface_destroy(surface);
    fclose (cairofile);
  } else {
    ////std::cout<<"write png"<<std::endl;    
    cairo_destroy (cr);
    cairo_surface_write_to_png (surface, cairofilename.c_str());
    cairo_surface_destroy(surface);
  }
  return 0;
}

void plotter::set_line_width(double width) {
  cairo_set_line_width(cr, lxtp(width));
}

void plotter::set_source_rgb(double r, double g, double b) {
  cairo_set_source_rgb(cr, r,g,b);
}

void plotter::set_source_gray(double g) {
  cairo_set_source_rgb(cr, g,g,g);
}

void plotter::set_source_graya(double g, double a) {
  cairo_set_source_rgba(cr, g,g,g,a);
}

void plotter::set_source_rgba(double r, double g, double b, double a) {
  cairo_set_source_rgba(cr, r,g,b,a);
}

void plotter::set_source(p_color color) {
  cairo_set_source_rgba(cr, color.r,color.g,color.b,color.a);
}

void plotter::set_fill_rgb(double r, double g, double b){ fr=r; fg=g; fb=b; fa=1;};
void plotter::set_fill_gray(double g){ fr=g; fg=g; fb=g; fa=1;};
void plotter::set_fill_graya(double g, double a){ fr=g; fg=g; fb=g; fa=a;};
void plotter::set_fill_rgba(double r, double g, double b, double a){ fr=r; fg=g; fb=b; fa=a;};
void plotter::set_fill(p_color color) { fr=color.r; fg=color.g; fb=color.b; fa=color.a; };

void plotter::set_stroke_rgb(double r, double g, double b){ sr=r; sg=g; sb=b; sa=1;};
void plotter::set_stroke_gray(double g){ sr=g; sg=g; sb=g; sa=1;};
void plotter::set_stroke_graya(double g, double a){ sr=g; sg=g; sb=g; sa=a;};
void plotter::set_stroke_rgba(double r, double g, double b, double a){ sr=r; sg=g; sb=b; sa=a;};
void plotter::set_stroke(p_color color) { sr=color.r; sg=color.g; sb=color.b; sa=color.a; };

void plotter::filled_rectangle(double x, double y, double xw, double yw){
  set_source_rgba(fr,fg,fb,fa);
  restore_dashes();
  //std::cout<<"frectangle  "<<mtpx(x)<<"   "<<mtpx(y)<<"   "<<lxtp(xw)<<"   "<<lytp(yw)<<std::endl;
  cairo_rectangle(cr, mtpx(x), mtpy(y), lxtp(xw), lytp(yw));
  cairo_fill_preserve(cr);
  set_source_rgba(sr,sg,sb,sa);
  cairo_stroke (cr);
}

void plotter::rectangle(double x, double y, double xw, double yw){
  set_source_rgba(fr,fg,fb,fa);
  restore_dashes();
  //std::cout<<"rectangle  "<<mtpx(x)<<"   "<<mtpx(y)<<"   "<<lxtp(xw)<<"   "<<lytp(yw)<<std::endl;
  cairo_rectangle(cr, mtpx(x), mtpy(y), lxtp(xw), lytp(yw));
  set_source_rgba(sr,sg,sb,sa);
  cairo_stroke (cr);
}

void plotter::line(double x, double y, double x1, double y1){
  set_source_rgba(fr,fg,fb,fa);
  restore_dashes();
  cairo_move_to(cr, mtpx(x), mtpy(y));
  cairo_line_to(cr, mtpx(x1), mtpy(y1));
  set_source_rgba(sr,sg,sb,sa);
  cairo_stroke (cr);
  //std::cout<<"line  "<<mtpx(x)<<"   "<<mtpx(y)<<"   "<<mtpx(x1)<<"   "<<mtpx(y1)<<std::endl;
}

void plotter::circle(double x, double y, double r){
  set_source_rgba(fr,fg,fb,fa);
  restore_dashes();
  cairo_arc(cr,mtpx(x),mtpy(y),lxtp(r), 0, 2*M_PI);
  //std::cout<<"circle  "<<mtpx(x)<<"   "<<mtpy(y)<<"   "<<lxtp(r)<<std::endl;
  set_source_rgba(sr,sg,sb,sa);
  cairo_stroke (cr);
}

void plotter::filled_circle(double x, double y, double r){
  set_source_rgba(fr,fg,fb,fa);
  restore_dashes();
  cairo_arc(cr,mtpx(x),mtpy(y),lxtp(r), 0, 2*M_PI);
  //std::cout<<"fcircle  "<<mtpx(x)<<"   "<<mtpy(y)<<"   "<<lxtp(r)<<std::endl;
  cairo_fill_preserve(cr);
  set_source_rgba(sr,sg,sb,sa);
  cairo_stroke (cr);
}

void plotter::plot_matrix(double x0, double y0, double xwidth, double ywidth, double* data, long countx, long county) {
  double Bmin=data[0];
  double Bmax=data[0];
  restore_dashes();
  for (long x=0; x<countx; x++) {
    for (long y=0; y<county; y++) {
        if (data[x*county + y]>Bmax) Bmax=data[x*county + y];
        if (data[x*county + y]<Bmin) Bmin=data[x*county + y];
    }
  }
  double xpos=x0;
  for (long x=0; x<countx; x++) {
    double ypos=y0;
    for (long y=0; y<county; y++) {
      double gray=(data[x*county + y]-Bmin)/(Bmax-Bmin);
      set_line_width(0);
      set_fill_gray(gray);
      set_stroke_gray(gray);
      rectangle(xpos, ypos, xwidth/countx*1.02, ywidth/county*1.02);  
      ypos+=ywidth/county;
    }
    xpos+=xwidth/countx;
  }
}

void plotter::restore_dashes(){
  if (dash_style==-1) {
    cairo_set_dash (cr, pdashes, pnum_dashes, poffset);       
  } else set_dash(dash_style);
}

void plotter::set_dash_ll(double *dashes, int num_dashes, double offset){
  dash_style=-1;
  if (dashes!=NULL) {
    if (pdashes!=NULL) free(pdashes);    
    pdashes=(double*)malloc(sizeof(double)*num_dashes);
    for (int i=0; i<num_dashes; i++) {
      pdashes[i]=dashes[i];  
    }
  } else pdashes=NULL;
  pnum_dashes=num_dashes;
  poffset=offset;  
  cairo_set_dash (cr, dashes, num_dashes, offset);   
}

void plotter::set_dash(int style){
  double lw=cairo_get_line_width(cr);
  dash_style=style;
  if (style==0) {
      cairo_set_dash (cr, NULL, 0, 0);
  }
  if (style==1) {
        double dashes[] = {lw, lw};
        int    ndash  = 2;
        double offset = 0;
        cairo_set_dash (cr, dashes, ndash, offset);
  }     
  if (style==2) {
        double dashes[] = {5.0*lw, 2.0*lw};
        int    ndash  = 2;
        double offset = 0;
        cairo_set_dash (cr, dashes, ndash, offset);
  }
  if (style==3) {
        double dashes[] = {5*lw, 2*lw, lw, 2*lw};
        int    ndash  = 4;
        double offset = 0;
        cairo_set_dash (cr, dashes, ndash, offset);
  }
  if (style==4) {
        double dashes[] = {10.0*lw, 5.0*lw};
        int    ndash  = 2;
        double offset = 0;
        cairo_set_dash (cr, dashes, ndash, offset);
  }
  if (style==5) {
        double dashes[] = {5*lw, 2*lw, lw, 2*lw, lw, 2*lw};
        int    ndash  = 6;
        double offset = 0;
        cairo_set_dash (cr, dashes, ndash, offset);
  }
}

void plotter::text(double x, double y, std::string text) {
  cairo_select_font_face (cr, fontname.c_str(), italic?CAIRO_FONT_SLANT_ITALIC:CAIRO_FONT_SLANT_NORMAL, bold?CAIRO_FONT_WEIGHT_BOLD:CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size (cr, lxtp(fontsize));
  cairo_move_to (cr, mtpx(x),mtpy(y));
  cairo_show_text (cr, text.c_str());    
  cairo_stroke (cr);
}

void plotter::centered_text(double x, double y, std::string text) {
  cairo_text_extents_t extents;
  double xx, yy;

  cairo_select_font_face (cr, fontname.c_str(), italic?CAIRO_FONT_SLANT_ITALIC:CAIRO_FONT_SLANT_NORMAL, bold?CAIRO_FONT_WEIGHT_BOLD:CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size (cr, lxtp(fontsize));

  cairo_text_extents (cr, text.c_str(), &extents);
  xx = mtpx(x)-(extents.width/2.0 + extents.x_bearing);
  yy = mtpy(y)-(extents.height/2.0 + extents.y_bearing);
  cairo_move_to (cr, xx,yy);
  //std::cout<<"centered_text  "<<xx<<", "<<yy<<":  "<<lxtp(fontsize)<<std::endl;
  cairo_show_text (cr, text.c_str());
  cairo_stroke (cr);
}

void plotter::rightaligned_text(double x, double y, std::string text) {
  cairo_text_extents_t extents;
  double xx, yy;

  cairo_select_font_face (cr, fontname.c_str(), italic?CAIRO_FONT_SLANT_ITALIC:CAIRO_FONT_SLANT_NORMAL, bold?CAIRO_FONT_WEIGHT_BOLD:CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size (cr, lxtp(fontsize));

  cairo_text_extents (cr, text.c_str(), &extents);
  xx = mtpx(x)-(extents.width + extents.x_bearing);
  yy = mtpy(y)-(extents.height/2.0 + extents.y_bearing);
  cairo_move_to (cr, xx,yy);
  cairo_show_text (cr, text.c_str());
  cairo_stroke (cr);
}

void plotter::textblock(double x, double y, std::string text, p_text_alignment talign, p_halignment halign, p_valignment valign){
  double xx=mtpx(x);
  double yy=mtpy(y);
  cairo_text_extents_t stde, stda;
  cairo_text_extents_t extents;

  cairo_select_font_face (cr, fontname.c_str(), italic?CAIRO_FONT_SLANT_ITALIC:CAIRO_FONT_SLANT_NORMAL, bold?CAIRO_FONT_WEIGHT_BOLD:CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size (cr, lxtp(fontsize));

  cairo_text_extents(cr, "Aquator", &stde);
  double lineheight=1.1*(stde.height);
  cairo_text_extents(cr, "ABX()", &stda);
  double aheight=stda.height;
  double xoffset=0;
  double yoffset=0;
  std::vector<std::string> lst;
  toStringVector(lst, text);

  ////std::cout<<"centered_text  "<<xx<<", "<<yy<<":  "<<lxtp(fontsize)<<std::endl;
  ////std::cout<<"lineheight="<<lineheight<<"   aheight="<<aheight<<std::endl;

  if (lst.size()>0) {
    double height=lst.size()*lineheight;
    double width=0;
    for (int i=0; i<lst.size(); i++) {
      cairo_text_extents(cr, toUTF8(lst[i]).c_str(), &extents);
      if (extents.width + extents.x_bearing>width) width=extents.width + extents.x_bearing;
    }
    if (halign==phRight) xoffset=-width;
    if (halign==phCenter) xoffset=-width/2.0;
    if (valign==pvCenter) yoffset=-height/2.0;
    if (valign==pvBottom) yoffset=-height;
    //std::cout<<"height="<<height<<"   width="<<width<<"   aheight="<<aheight<<std::endl;
    if ((width>0)&&(height>0)) {
      for (int i=0; i<lst.size(); i++) {
        cairo_text_extents(cr, toUTF8(lst[i]).c_str(), &extents);
        double linewidth=extents.width + extents.x_bearing;
        xx=mtpx(x);
        if (talign==pRightAlign) xx+=width-linewidth;
        if (talign==pCentered) xx+=width/2.0-linewidth/2.0;
        
        yy=mtpy(y)+aheight+i*lineheight;
        
        if (lst[i].size()>0) {
          cairo_move_to (cr, xx+xoffset,yy+yoffset);
          if (!onlySpace(lst[i])) cairo_show_text (cr, toUTF8(lst[i]).c_str());
          ////std::cout<<toUTF8(lst[i])<<std::endl;
        }
        //cairo_stroke (cr);
        //std::cout<<"textblock  "<<xx<<", "<<yy<<":  '"<<lst[i]<<"'"<<std::endl;
      }
    }
  }
  cairo_stroke (cr);
}

double plotter::textheight(std::string text){
  cairo_text_extents_t extents;
  double xx, yy;

  cairo_select_font_face (cr, fontname.c_str(), italic?CAIRO_FONT_SLANT_ITALIC:CAIRO_FONT_SLANT_NORMAL, bold?CAIRO_FONT_WEIGHT_BOLD:CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size (cr, lxtp(fontsize));

  cairo_text_extents (cr, text.c_str(), &extents);
  return ptly(extents.height);
}

double plotter::textwidth(std::string text){
  cairo_text_extents_t extents;
  double xx, yy;

  cairo_select_font_face (cr, fontname.c_str(), italic?CAIRO_FONT_SLANT_ITALIC:CAIRO_FONT_SLANT_NORMAL, bold?CAIRO_FONT_WEIGHT_BOLD:CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size (cr, lxtp(fontsize));

  cairo_text_extents (cr, text.c_str(), &extents);
  return ptlx(extents.width);
}
