#include "fitalgorithm_inst_a1.h"
#include "fitalgorithm_inst_a1config.h"
#include <cmath>


QFFitAlgorithmInst_A1::QFFitAlgorithmInst_A1() {
    // set default parameter values
	//setParameter("param_name", 0.0);
}

QFFitAlgorithm::FitResult QFFitAlgorithmInst_A1::intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, QFFitAlgorithm::Functor* model, const double* paramsMin, const double* paramsMax) {
    QFFitAlgorithm::FitResult result;

    int paramCount=model->get_paramcount();

    double param=getParameter("param_name").toDouble();

	//////////////////////////////////////////////////////////////////////
	// DO YOU FIT HERE
	//////////////////////////////////////////////////////////////////////

    result.params["param_name"]=param;

    result.message=QObject::tr("<b>fit was successfull</b>");
    result.messageSimple=QObject::tr("fit was successfull");;

    return result;
}

bool QFFitAlgorithmInst_A1::displayConfig() {
    QFFitAlgorithmInst_A1ConfigDialog* dlg=new QFFitAlgorithmInst_A1ConfigDialog(0);

	// init widget here:
    //dlg->setParam(getParameter("param_name").toDouble());

    if (dlg->exec()==QDialog::Accepted) {
		// read back entered values here
        //setParameter("param_name", dlg->getParamName());
        delete dlg;
        return true;
    } else {
        delete dlg;
        return false;
    }
}

