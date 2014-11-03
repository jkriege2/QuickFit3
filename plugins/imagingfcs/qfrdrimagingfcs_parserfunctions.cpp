#include "qfrdrimagingfcs_parserfunctions.h"
#include "qfrdrimagingfcs_data.h"


void fRDR_isimfcs(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==1 && params[0].type==qfmpDouble) {
            evalID=params[0].toInteger();
            QFRDRImagingFCSData* rdr=dynamic_cast<QFRDRImagingFCSData*>(p->getRawDataByID(evalID));
            res.setBoolean(rdr);
        } else {
            parser->qfmpError(QObject::tr("rdr_isimfcs(rdrid) needs one integer arguments"));
            res.setInvalid();
            return;
        }
    }
}
