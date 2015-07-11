#include "../../simanneal.h"
#include "../../tools.h"

#include <iostream>

using namespace std;

const unsigned int datacount=8;
const unsigned int paramcount=2;

double data[datacount]={1.1, 1.4, 2.2, 3.05, 4.8, 6, 10.5, 50};
double xval[datacount]={  1, 1.5,   2,    3,   5, 6, 10,   48};
double p[paramcount]={0, 1};
double ubound[paramcount]={100, 100};
double lbound[paramcount]={-100, -100};


double model(double x, double*p, unsigned int paramCount) {
    return p[0]+p[1]*x;
}


double chi2(double* p, unsigned int paramCount) {
    double v=0;
    for (unsigned int i=0; i<datacount; i++) {
        double d=data[i]-model(xval[i], p, paramCount);
        v=v+d*d;
    }
    //std::cout<<"chi2("<<doublevectortostr(p, paramCount)<<") = "<<v<<std::endl;
    return v;
}


int main() {
    fitSimAnneal sa;
    sa.init(chi2, p, paramcount, lbound, ubound);
    sa.fit();

    std::cout<<"BEST FIT: "<<doublevectortostr(sa.get_param(), paramcount)<<".\n";
    std::cout<<"  AFTER "<< sa.get_iterations()<<"ITERATIONS\n";
    std::cout<<"  AND "<< sa.get_chi2EvalCount()<<" chi2() evaluations.\n";
    system("PAUSE");
}
