

// returns the name of the model
const char* DLL_EXPORT getModelName(int16_t type) {
    switch(type) {
        case 1:  // returns the model ID (short name, no whitespaces ... usable as part of filename)
            return QF3SFF_ID;
        case 2:  // long model description
            return QF3SFF_SHORTNAME;//3D free and normal diffusion model with up to three independent diffusing species, one non-diffusing species and a triplett fraction.\0";
        case 3:  // long model description
            return QF3SFF_HELP;//3D free and normal diffusion model with up to three independent diffusing species, one non-diffusing species and a triplett fraction.\0";
        case 0:  // returns the model name
            return QF3SFF_NAME;
        default:
            return NULL;
    }
    return NULL;
}


const QF3SimpleFFParameter* DLL_EXPORT getParameterProperties() {
    return QF3SFF_PARAMETERS;
}



uint16_t DLL_EXPORT getParameterCount() {
    return QF3SFF_PARAMETER_COUNT;
}


double DLL_EXPORT evaluate(double t, const double* parameters) {
    return QF3SFF_EVALUATE(t, parameters);
}

void DLL_EXPORT multiEvaluate(double* c, const double* t, uint64_t NN, const double* parameters) {
    for (register unsigned int i=0; i<NN; i++) {
        c[i]=QF3SFF_EVALUATE(t[i], parameters);
    }
}

