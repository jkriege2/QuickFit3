// returns the name of the model
void DLL_EXPORT getModelName(char* buffer, int type) {
    switch(type) {
        case 1: { // returns the model ID (short name, no whitespaces ... usable as part of filename)
            char* m=MODEL_SHORTNAME;
            strcpy(buffer, m);
            return;
        }

        case 2: { // long model description
            char* m=MODEL_HELP;//3D free and normal diffusion model with up to three independent diffusing species, one non-diffusing species and a triplett fraction.\0";
            strcpy(buffer, m);
            return;
        }

        case 3: { // image describing the fit model
            char* m=MODEL_IMAGE;
            strcpy(buffer, m);
            return;
        }

        default:
        case 0: { // returns the model name
            char* m=MODEL_NAME;
            strcpy(buffer, m);
            return;
        }
    }
}


quickfit2ModelParameters DLL_EXPORT getParameterProperties() {
    quickfit2ModelParameters d;

    // initialize the boolean (non-zero/zero int) fields
    d.userInitialized=(int*)malloc(PARAMCOUNT*sizeof(int));
    for (int i=0; i<PARAMCOUNT; i++) { d.userInitialized[i]=TRUE; }
    d.userEditable=(int*)malloc(PARAMCOUNT*sizeof(int));
    for (int i=0; i<PARAMCOUNT; i++) { d.userEditable[i]=TRUE; }
    d.fit=(int*)malloc(PARAMCOUNT*sizeof(int));
    for (int i=0; i<PARAMCOUNT; i++) { d.fit[i]=TRUE; }
    d.loadFromINI=(int*)malloc(PARAMCOUNT*sizeof(int));
    for (int i=0; i<PARAMCOUNT; i++) { d.loadFromINI[i]=TRUE; }
    d.min=(double*)malloc(PARAMCOUNT*sizeof(double));
    for (int i=0; i<PARAMCOUNT; i++) { d.min[i]=0; }
    d.max=(double*)malloc(PARAMCOUNT*sizeof(double));
    for (int i=0; i<PARAMCOUNT; i++) { d.max[i]=0; }
    d.widgetType=(int*)malloc(PARAMCOUNT*sizeof(int));
    for (int i=0; i<PARAMCOUNT; i++) { d.widgetType[i]=QF2MP_WIDGET_DEFAULT; }
    d.widgetIncrement=(double*)malloc(PARAMCOUNT*sizeof(double));
    for (int i=0; i<PARAMCOUNT; i++) { d.widgetIncrement[i]=1.0; }
    d.parameterClass=(uint32_t*)malloc(PARAMCOUNT*sizeof(uint32_t));
    for (int i=0; i<PARAMCOUNT; i++) { d.parameterClass[i]=0; }

    // initialize the string fields
    d.name=(char**)malloc(PARAMCOUNT*sizeof(char*));
    d.label=(char**)malloc(PARAMCOUNT*sizeof(char*));
    d.shortName=(char**)malloc(PARAMCOUNT*sizeof(char*));
    d.unit=(char**)malloc(PARAMCOUNT*sizeof(char*));

    // fill the string fields with the parameter names, descriptions ...
    for (unsigned int i=0; i<PARAMCOUNT; i++) {
        unsigned int ID=MODEL_PARAM[i].ID;
        d.name[ID]=qf2m_strcpy(MODEL_PARAM[i].name);
        d.shortName[ID]=qf2m_strcpy(MODEL_PARAM[i].shortName);
        d.label[ID]=qf2m_strcpy(MODEL_PARAM[i].label);
        d.unit[ID]=qf2m_strcpy(MODEL_PARAM[i].unit);
        d.min[ID]=MODEL_PARAM[i].min;
        d.max[ID]=MODEL_PARAM[i].max;
        d.fit[ID]=MODEL_PARAM[i].fit;
        d.userEditable[ID]=MODEL_PARAM[i].userEditable;
        d.userInitialized[ID]=MODEL_PARAM[i].userInitialized;
        d.loadFromINI[ID]=MODEL_PARAM[i].loadFromINI;
        d.widgetType[ID]=MODEL_PARAM[i].widgetType;
        d.widgetIncrement[ID]=MODEL_PARAM[i].widgetIncrement;
        d.parameterClass[ID]=MODEL_PARAM[i].parameterClass;

    }


    return d;
}



void DLL_EXPORT freeParameterProperties(quickfit2ModelParameters params){
    free(params.userInitialized);
    free(params.userEditable);
    free(params.fit);
    free(params.min);
    free(params.max);
    free(params.loadFromINI);
    free(params.parameterClass);

    for (int i=0; i<PARAMCOUNT; i++) {
        free(params.name[i]);
        free(params.shortName[i]);
        free(params.label[i]);
        free(params.unit[i]);
    }

    free(params.name);
    free(params.label);
    free(params.shortName);
}

double DLL_EXPORT getParameterInitialValue(unsigned int parameter) {
    for (unsigned int i=0; i<PARAMCOUNT; i++) {
        if (MODEL_PARAM[i].ID==parameter) return MODEL_PARAM[i].init;
    }
    return 0.0;
}




unsigned int DLL_EXPORT getParameterCount() {
    return PARAMCOUNT;
}


double DLL_EXPORT evaluate(double t, double* parameters) {
    return MODEL_EVAL(t, parameters);
}

void DLL_EXPORT multiEvaluate(double* c, double* t, unsigned int NN, double* parameters) {
    for (register unsigned int i=0; i<NN; i++) {
        c[i]=MODEL_EVAL(t[i], parameters);
    }
}

int DLL_EXPORT isFitParameter(unsigned int parameter, double* param_vec) {
    for (unsigned int i=0; i<PARAMCOUNT; i++) {
        if (parameter==MODEL_PARAM[i].ID) {
            if (! MODEL_PARAM[i].fit) return FALSE;
        }
    }
    return getParameterWidgetVisible(parameter, param_vec);
}

void DLL_EXPORT freePlot(quickfit2ModelPlot* plot) {
    free(plot->t);
    for (size_t i=0; i<plot->N_plots; i++) {
        free(plot->name[i]);
    }
    free(plot->name);
    free(plot->data);
}

#ifdef MODEL_PLOT_SIMPLE
    void DLL_EXPORT plot(quickfit2ModelPlot* plotdata, double* param_vec) {
        plotdata->N_plots=1;
        plotdata->name=(char**)calloc(plotdata->N_plots, sizeof(char*));
        plotdata->data=(double*)calloc(plotdata->N_plots*plotdata->N_time, sizeof(double));
        MODEL_COPY_PARAM(param_vec1, param_vec);
        transformParams(param_vec1);
        for (unsigned int i=0; i<plotdata->N_time; i++) {
            plotdata->data[i]=MODEL_EVAL(plotdata->t[i], param_vec1);
        }
        free(param_vec1);
        plotdata->name[0]=qf2m_strcpy("fit model");
        plotdata->fullPlot=0;
    }
#endif



#ifdef MODEL_PLOT_SEMISIMPLE
void DLL_EXPORT plot(quickfit2ModelPlot* plotdata, double* param_vec) {
    plotdata->N_plots=PLOT_COUNT;
    MODEL_ALLOCATE_PLOTS(plotdata);
    for (unsigned int p=0; p<PLOT_COUNT; p++) {
        MODEL_COPY_PARAM(param_vec1, param_vec);
        MODEL_PLOT_TRANSFORM(p, param_vec1);
        transformParams(param_vec1);
        for (unsigned int i=0; i<plotdata->N_time; i++) {
            plotdata->data[i+p*plotdata->N_time]=MODEL_EVAL(plotdata->t[i], param_vec1);
        }
        free(param_vec1);
        plotdata->name[p]=qf2m_strcpy(MODEL_PLOTS[p].name);
    }
    plotdata->fullPlot=0;
}
#endif
