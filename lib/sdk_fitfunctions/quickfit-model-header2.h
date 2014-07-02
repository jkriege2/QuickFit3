#ifdef __GNUC__
  inline double MODEL_EVAL(double t, const double* parameters) __attribute__((always_inline));
  inline void MODEL_ALLOCATE_PLOTS(quickfit2ModelPlot* plotdata) __attribute__((always_inline));
#else
  inline double MODEL_EVAL(double t, const double* parameters);
  inline void MODEL_ALLOCATE_PLOTS(quickfit2ModelPlot* plotdata);
#endif



inline void MODEL_ALLOCATE_PLOTS(quickfit2ModelPlot* plotdata) {
    plotdata->name=(char**)calloc(plotdata->N_plots, sizeof(char*));
    plotdata->data=(double*)calloc(plotdata->N_plots*plotdata->N_time, sizeof(double));
}


#define MODEL_COPY_PARAM(param_vec1, param_vec) \
    double* param_vec1=(double*)malloc(PARAMCOUNT*sizeof(double)); \
    memcpy(param_vec1, param_vec, PARAMCOUNT*sizeof(double));
