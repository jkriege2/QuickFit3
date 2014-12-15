/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate: 2014-11-09 19:22:45 +0100 (So, 09 Nov 2014) $  (revision $Rev: 3606 $)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfrdrfcsparserfunctions.h"
#include "statistics_tools.h"
#include "qfrdrsimplecountrateinterface.h"
#include "qfrdrfcsdatainterface.h"
#include "qfrdrcountratesinterface.h"
#include <stdint.h>
#include "qfproject.h"
#include "qfrawdatarecord.h"


void fRDR_isfcs(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==1 && params[0].type==qfmpDouble) {
            evalID=params[0].toInteger();
            QFRDRFCSDataInterface* rdr=dynamic_cast<QFRDRFCSDataInterface*>(p->getRawDataByID(evalID));
            res.setBoolean(rdr);
        } else {
            parser->qfmpError(QObject::tr("rdr_isfcs(rdrid) needs one integer arguments"));
            res.setInvalid();
            return;
        }
    }
}


void fRDR_issimplecountrate(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==1 && params[0].type==qfmpDouble) {
            evalID=params[0].toInteger();
            QFRDRSimpleCountRatesInterface* rdr=dynamic_cast<QFRDRSimpleCountRatesInterface*>(p->getRawDataByID(evalID));
            res.setBoolean(rdr);
        } else {
            parser->qfmpError(QObject::tr("rdr_issimplecountrate(rdrid) needs one integer arguments"));
            res.setInvalid();
            return;
        }
    }
}
void fFCS_correlation(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==2 && params[0].type==qfmpDouble && params[1].type==qfmpDouble) {
            evalID=params[0].toInteger();
            int run=params[1].toInteger()-1;
            QFRDRFCSDataInterface* rdr=dynamic_cast<QFRDRFCSDataInterface*>(p->getRawDataByID(evalID));
            if (rdr && run>=0 && run<rdr->getCorrelationRuns()) {
                res.setDoubleVec(rdr->getCorrelationRun(run), rdr->getCorrelationN());
            }
        } else {
            parser->qfmpError(QObject::tr("fcs_correlation(rdrid, run) needs two integer arguments"));
            res.setInvalid();
            return;
        }
    }
}
void fFCS_correlationerror(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==2 && params[0].type==qfmpDouble && params[1].type==qfmpDouble) {
            evalID=params[0].toInteger();
            int run=params[1].toInteger()-1;
            QFRDRFCSDataInterface* rdr=dynamic_cast<QFRDRFCSDataInterface*>(p->getRawDataByID(evalID));
            if (rdr && run>=0 && run<rdr->getCorrelationRuns()) {
                res.setDoubleVec(rdr->getCorrelationRunError(run), rdr->getCorrelationN());
            }
        } else {
            parser->qfmpError(QObject::tr("fcs_correlationerror(rdrid, run) needs two integer arguments"));
            res.setInvalid();
            return;
        }
    }
}

void fFCS_meancorrelation(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==1 && params[0].type==qfmpDouble ) {
            evalID=params[0].toInteger();
            QFRDRFCSDataInterface* rdr=dynamic_cast<QFRDRFCSDataInterface*>(p->getRawDataByID(evalID));
            if (rdr) {
                res.setDoubleVec(rdr->getCorrelationMean(), rdr->getCorrelationN());
            }
        } else {
            parser->qfmpError(QObject::tr("fcs_meancorrelation(rdrid) needs one integer arguments"));
            res.setInvalid();
            return;
        }
    }
}
void fFCS_meancorrelationerror(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==1 && params[0].type==qfmpDouble ) {
            evalID=params[0].toInteger();
            QFRDRFCSDataInterface* rdr=dynamic_cast<QFRDRFCSDataInterface*>(p->getRawDataByID(evalID));
            if (rdr) {
                res.setDoubleVec(rdr->getCorrelationStdDev(), rdr->getCorrelationN());
            }
        } else {
            parser->qfmpError(QObject::tr("fcs_meancorrelationerror(rdrid) needs one integer arguments"));
            res.setInvalid();
            return;
        }
    }
}

void fFCS_tau(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==1 && params[0].type==qfmpDouble) {
            evalID=params[0].toInteger();
            QFRDRFCSDataInterface* rdr=dynamic_cast<QFRDRFCSDataInterface*>(p->getRawDataByID(evalID));
            if (rdr) {
                res.setDoubleVec(rdr->getCorrelationT(), rdr->getCorrelationN());
            }
        } else {
            parser->qfmpError(QObject::tr("fcs_tau(rdrid) needs one integer arguments"));
            res.setInvalid();
            return;
        }
    }
}

void fFCS_runs(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==1 && params[0].type==qfmpDouble) {
            evalID=params[0].toInteger();
            QFRDRFCSDataInterface* rdr=dynamic_cast<QFRDRFCSDataInterface*>(p->getRawDataByID(evalID));
            if (rdr ) {
                res.setVariant(rdr->getCorrelationRuns());
            }
        } else {
            parser->qfmpError(QObject::tr("fcs_runs(rdrid) needs one integer arguments"));
            res.setInvalid();
            return;
        }
    }
}
