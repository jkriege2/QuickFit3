#include "fitalgorithm_inst.h"
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "fitalgorithm_instconfig.h"

QFFitAlgorithmInst::QFFitAlgorithmInst() {
    // set default parameter values
	//setParameter("param_name", 0.0);
}


QFFitAlgorithm::FitResult QFFitAlgorithmInst::intFit(double* paramsOut, double* paramErrorsOut, double* initialParams, QFFitAlgorithm::Functor* model, double* paramsMin, double* paramsMax) {
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

bool QFFitAlgorithmInst::displayConfig() {
    LevmarConfigDialog* dlg=new LevmarConfigDialog(0);
	
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


Q_EXPORT_PLUGIN2(target_id, QFFitAlgorithmInstPlugin)
