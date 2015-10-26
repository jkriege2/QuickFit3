#include "qfrdrcurveparserfunctions.h"




// res is the function result, params is a vector of the function parameters and n is its count.
// The final parameter p is a pointer to the QFMathParser object that can e.g. be used for error reporting.
void fCurve_getCount(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    // default return value is invalid
    res.setInvalid();
    // get a pointer to the current QFProject
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        // the first parameter has to be a number (qfmpDouble) and specifies the RDR ID
        int evalID=-1;
        if (n==1 && params[0].isUInt()) {
            evalID=params[0].toInteger();
            // get the specified RDR from the project and check whether it is a curve RDR (i.e. implements the interface QFRDRCurvesInterface)
            QFRDRCurvesInterface* rdr=dynamic_cast<QFRDRCurvesInterface*>(p->getRawDataByID(evalID));
            if (rdr ) {
                // if all is OK, return the nuber of curves in the RDR
                res.setDouble(rdr->curvesGetCount());
            } else {
                // report an error
                parser->qfmpError(QObject::tr("curve_getcount(rdrid): the specified RDR is no curve record"));
                res.setInvalid();
                return;
            }
        } else {
            parser->qfmpError(QObject::tr("curve_getcount(rdrid) needs one unsigned integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}


void fCurve_getDataX(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int RDRID=-1;
        if (n>=1 && n<=2 && params[0].type==qfmpDouble) {
            RDRID=params[0].toInteger();
            QFRDRCurvesInterface* rdr=dynamic_cast<QFRDRCurvesInterface*>(p->getRawDataByID(RDRID));
            int curve=0;
            if (n>=2 && params[1].type==qfmpDouble ) {
                curve=params[1].toInteger();
            } else  if (n<=0 || n>2){
                parser->qfmpError(QObject::tr("curve_getx(rdrid, curve=0) requires 1-2  integer arguments (%1 given)").arg(n));
                res.setInvalid();
                return;
            }
            if (rdr && curve>=0 && curve<rdr->curvesGetCount()) {
                res.setDoubleVec(rdr->curvesGetX(curve));
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("curve_getx(rdrid, curve=0): the specified RDR is no curve record"));
                    res.setInvalid();
                    return;
                } else if (!(curve>=0 && curve<rdr->curvesGetCount())) {
                    parser->qfmpError(QObject::tr("curve_getx(rdrid, curve=0): the specified curve (%1) is not available in the RDR (only %2 curves are available)").arg(curve).arg(rdr->curvesGetCount()));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("curve_getx(rdrid, curve=0) needs 1-2 integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}

void fCurve_getDataY(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int RDRID=-1;
        if (n>=1 && n<=2 && params[0].type==qfmpDouble) {
            RDRID=params[0].toInteger();
            QFRDRCurvesInterface* rdr=dynamic_cast<QFRDRCurvesInterface*>(p->getRawDataByID(RDRID));
            int curve=0;
            if (n>=2 && params[1].type==qfmpDouble ) {
                curve=params[1].toInteger();
            } else  if (n<=0 || n>2){
                parser->qfmpError(QObject::tr("curve_gety(rdrid, curve=0) requires 1-2  integer arguments (%1 given)").arg(n));
                res.setInvalid();
                return;
            }
            if (rdr && curve>=0 && curve<rdr->curvesGetCount()) {
                res.setDoubleVec(rdr->curvesGetY(curve));
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("curve_gety(rdrid, curve=0): the specified RDR is no curve record"));
                    res.setInvalid();
                    return;
                } else if (!(curve>=0 && curve<rdr->curvesGetCount())) {
                    parser->qfmpError(QObject::tr("curve_gety(rdrid, curve=0): the specified curve (%1) is not available in the RDR (only %2 curves are available)").arg(curve).arg(rdr->curvesGetCount()));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("curve_gety(rdrid, curve=0) needs 1-2 integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}


void fCurve_getDataXError(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int RDRID=-1;
        if (n>=1 && n<=2 && params[0].type==qfmpDouble) {
            RDRID=params[0].toInteger();
            QFRDRCurvesInterface* rdr=dynamic_cast<QFRDRCurvesInterface*>(p->getRawDataByID(RDRID));
            int curve=0;
            if (n>=2 && params[1].type==qfmpDouble ) {
                curve=params[1].toInteger();
            } else  if (n<=0 || n>2){
                parser->qfmpError(QObject::tr("curve_getxerror(rdrid, curve=0) requires 1-2  integer arguments (%1 given)").arg(n));
                res.setInvalid();
                return;
            }
            if (rdr && curve>=0 && curve<rdr->curvesGetCount()) {
                res.setDoubleVec(rdr->curvesGetXError(curve));
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("curve_getxerror(rdrid, curve=0): the specified RDR is no curve record"));
                    res.setInvalid();
                    return;
                } else if (!(curve>=0 && curve<rdr->curvesGetCount())) {
                    parser->qfmpError(QObject::tr("curve_getxerror(rdrid, curve=0): the specified curve (%1) is not available in the RDR (only %2 curves are available)").arg(curve).arg(rdr->curvesGetCount()));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("curve_getxerror(rdrid, curve=0) needs 1-2 integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}

void fCurve_getDataYError(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int RDRID=-1;
        if (n>=1 && n<=2 && params[0].type==qfmpDouble) {
            RDRID=params[0].toInteger();
            QFRDRCurvesInterface* rdr=dynamic_cast<QFRDRCurvesInterface*>(p->getRawDataByID(RDRID));
            int curve=0;
            if (n>=2 && params[1].type==qfmpDouble ) {
                curve=params[1].toInteger();
            } else  if (n<=0 || n>2){
                parser->qfmpError(QObject::tr("curve_getyerror(rdrid, curve=0) requires 1-2  integer arguments (%1 given)").arg(n));
                res.setInvalid();
                return;
            }
            if (rdr && curve>=0 && curve<rdr->curvesGetCount()) {
                res.setDoubleVec(rdr->curvesGetYError(curve));
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("curve_getyerror(rdrid, curve=0): the specified RDR is no curve record"));
                    res.setInvalid();
                    return;
                } else if (!(curve>=0 && curve<rdr->curvesGetCount())) {
                    parser->qfmpError(QObject::tr("curve_getyerror(rdrid, curve=0): the specified curve (%1) is not available in the RDR (only %2 curves are available)").arg(curve).arg(rdr->curvesGetCount()));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("curve_getyerror(rdrid, curve=0) needs 1-2 integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}
