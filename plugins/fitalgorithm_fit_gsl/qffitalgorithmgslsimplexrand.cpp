#include "qffitalgorithmgslsimplexrand.h"
#include "qffitalgorithmgslsimplexconfigdialog.h"
#include "qffitalgorithmgsltools.h"

#include <cmath>


#include "gsl/gsl_vector.h"
#include "gsl/gsl_multimin.h"
#include "gsl/gsl_blas.h"






QFFitAlgorithmGSLSimplexRand::QFFitAlgorithmGSLSimplexRand():
    QFFitAlgorithmGSLSimplex()
{
    T = gsl_multimin_fminimizer_nmsimplex2rand;
    setParameter("stepsize", 1);
    setParameter("max_iterations", 200);
}


bool QFFitAlgorithmGSLSimplexRand::displayConfig() {
    QFFitAlgorithmGSLSimplexConfigDialog* dlg=new QFFitAlgorithmGSLSimplexConfigDialog(0);

    dlg->setWindowTitle(QObject::tr("GSL Randomized Simplex Parameters ..."));
	// init widget here:
    dlg->init(this);

    if (dlg->exec()==QDialog::Accepted) {
		// read back entered values here
        dlg->storeParams(this);
        delete dlg;
        return true;
    } else {
        delete dlg;
        return false;
    }
}

