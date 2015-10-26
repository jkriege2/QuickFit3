/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
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



//void fRDR_isfcs(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
//{
//    res.setInvalid();
//    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
//    if (p)  {
//        int evalID=-1;
//        if (n==1 && params[0].type==qfmpDouble) {
//            evalID=params[0].toInteger();
//            QFRDRFCSDataInterface* rdr=dynamic_cast<QFRDRFCSDataInterface*>(p->getRawDataByID(evalID));
//            res.setBoolean(rdr);
//        } else {
//            parser->qfmpError(QObject::tr("rdr_isfcs(rdrid) needs one integer arguments (%1 given)").arg(n));
//            res.setInvalid();
//            return;
//        }
//    }
//}

void fFCS_correlation(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==2 && params[0].type==qfmpDouble && params[1].type==qfmpDouble) {
            evalID=params[0].toInteger();
            int run=params[1].toInteger();
            QFRDRFCSDataInterface* rdr=dynamic_cast<QFRDRFCSDataInterface*>(p->getRawDataByID(evalID));
            if (rdr && run==-1) {
                res.setDoubleVec(rdr->getCorrelationMean(), rdr->getCorrelationN());
            } else if (rdr && run>=0 && run<rdr->getCorrelationRuns()) {
                res.setDoubleVec(rdr->getCorrelationRun(run), rdr->getCorrelationN());
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("fcs_correlationerror(rdrid, run): the specified RDR is no FCS record"));
                    res.setInvalid();
                    return;
                } else {
                    parser->qfmpError(QObject::tr("fcs_correlationerror(rdrid, run): the specified run (%1) is not available").arg(run));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("fcs_correlation(rdrid, run) needs two integer arguments (%1 given)").arg(n));
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
            int run=params[1].toInteger();
            QFRDRFCSDataInterface* rdr=dynamic_cast<QFRDRFCSDataInterface*>(p->getRawDataByID(evalID));
            if (rdr && run==-1) {
                res.setDoubleVec(rdr->getCorrelationStdDev(), rdr->getCorrelationN());
            } else if (rdr && run>=0 && run<rdr->getCorrelationRuns()) {
                res.setDoubleVec(rdr->getCorrelationRunError(run), rdr->getCorrelationN());
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("fcs_correlationerror(rdrid, run): the specified RDR is no FCS record"));
                    res.setInvalid();
                    return;
                } else {
                    parser->qfmpError(QObject::tr("fcs_correlationerror(rdrid, run): the specified run (%1) is not available").arg(run));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("fcs_correlationerror(rdrid, run) needs two integer arguments (%1 given)").arg(n));
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
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("fcs_meancorrelation(rdrid): the specified RDR is no FCS record"));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("fcs_meancorrelation(rdrid) needs one integer arguments (%1 given)").arg(n));
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
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("fcs_meancorrelationerror(rdrid): the specified RDR is no FCS record"));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("fcs_meancorrelationerror(rdrid) needs one integer arguments (%1 given)").arg(n));
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
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("fcs_tau(rdrid): the specified RDR is no FCS record"));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("fcs_tau(rdrid) needs one integer arguments (%1 given)").arg(n));
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
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("fcs_runs(rdrid): the specified RDR is no FCS record"));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("fcs_runs(rdrid) needs one integer arguments (%1 given)").arg(n));
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
            parser->qfmpError(QObject::tr("rdr_isscr(rdrid) needs one integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}


void fSCR_channels(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==1 && params[0].type==qfmpDouble) {
            evalID=params[0].toInteger();
            QFRDRSimpleCountRatesInterface* rdr=dynamic_cast<QFRDRSimpleCountRatesInterface*>(p->getRawDataByID(evalID));
            if (rdr ) {
                res.setDouble(rdr->getSimpleCountrateChannels());
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("scr_channels(rdrid): the specified RDR is no simple count rate (SCR) record"));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("scr_channels(rdrid) needs one integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}


void fSCR_runs(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==1 && params[0].type==qfmpDouble) {
            evalID=params[0].toInteger();
            QFRDRSimpleCountRatesInterface* rdr=dynamic_cast<QFRDRSimpleCountRatesInterface*>(p->getRawDataByID(evalID));
            if (rdr ) {
                res.setDouble(rdr->getSimpleCountrateRuns());
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("scr_runs(rdrid): the specified RDR is no simple count rate (SCR) record"));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("scr_runs(rdrid) needs one integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}


void fSCR_mean(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n>=1 && n<=3 && params[0].type==qfmpDouble) {
            evalID=params[0].toInteger();
            QFRDRSimpleCountRatesInterface* rdr=dynamic_cast<QFRDRSimpleCountRatesInterface*>(p->getRawDataByID(evalID));
            int run=-1;
            int channel=0;
            if (n==2 && params[1].type==qfmpDouble ) {
                run=params[1].toInteger();
            } else if (n==3 && params[1].type==qfmpDouble  && params[2].type==qfmpDouble ) {
                run=params[1].toInteger();
                channel=params[2].toInteger();
            } else  if (n!=1){
                parser->qfmpError(QObject::tr("scr_mean(rdrid, run=-1, channel=0) 1-3  integer arguments (%1 given)").arg(n));
                res.setInvalid();
                return;
            }
            if (rdr && channel>=0 && channel<rdr->getSimpleCountrateChannels() && run>=-1 && run<rdr->getSimpleCountrateRuns()) {
                res.setDouble(rdr->getSimpleCountrateAverage(run, channel));
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("scr_mean(rdrid, run=-1, channel=0): the specified RDR is no simple count rate (SCR) record"));
                    res.setInvalid();
                    return;
                } else if (!(channel>=0 && channel<rdr->getSimpleCountrateChannels())) {
                    parser->qfmpError(QObject::tr("scr_mean(rdrid, run=-1, channel=0): the specified channel (%1) is not available in the RDR").arg(channel));
                    res.setInvalid();
                    return;
                } else if (!(run>=0 && run<rdr->getSimpleCountrateRuns())) {
                    parser->qfmpError(QObject::tr("scr_mean(rdrid, run=-1, channel=0): the specified run (%1) is not available in the RDR").arg(run));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("scr_mean(rdrid, run=-1, channel=0) needs 1-3 integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}


void fSCR_std(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n>=1 && n<=3 && params[0].type==qfmpDouble) {
            evalID=params[0].toInteger();
            QFRDRSimpleCountRatesInterface* rdr=dynamic_cast<QFRDRSimpleCountRatesInterface*>(p->getRawDataByID(evalID));
            int run=-1;
            int channel=0;
            if (n==2 && params[1].type==qfmpDouble ) {
                run=params[1].toInteger();
            } else if (n==3 && params[1].type==qfmpDouble  && params[2].type==qfmpDouble ) {
                run=params[1].toInteger();
                channel=params[2].toInteger();
            } else if (n!=1) {
                parser->qfmpError(QObject::tr("scr_std(rdrid, run=-1, channel=0) 1-3  integer arguments (%1 given)").arg(n));
                res.setInvalid();
                return;
            }
            if (rdr && channel>=0 && channel<rdr->getSimpleCountrateChannels() && run>=-1 && run<rdr->getSimpleCountrateRuns()) {
                res.setDouble(rdr->getSimpleCountrateStdDev(run, channel));
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("scr_std(rdrid, run=-1, channel=0): the specified RDR is no simple count rate (SCR) record"));
                    res.setInvalid();
                    return;
                } else if (!(channel>=0 && channel<rdr->getSimpleCountrateChannels())) {
                    parser->qfmpError(QObject::tr("scr_std(rdrid, run=-1, channel=0): the specified channel (%1) is not available in the RDR").arg(channel));
                    res.setInvalid();
                    return;
                } else if (!(run>=0 && run<rdr->getSimpleCountrateRuns())) {
                    parser->qfmpError(QObject::tr("scr_std(rdrid, run=-1, channel=0): the specified run (%1) is not available in the RDR").arg(run));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("scr_std(rdrid, run=-1, channel=0) 1-3 one integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}
















void fRDR_iscountrate(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==1 && params[0].type==qfmpDouble) {
            evalID=params[0].toInteger();
            QFRDRCountRatesInterface* rdr=dynamic_cast<QFRDRCountRatesInterface*>(p->getRawDataByID(evalID));
            res.setBoolean(rdr);
        } else {
            parser->qfmpError(QObject::tr("rdr_iscr(rdrid) needs one integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}


void fCR_rate(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n>=1 && n<=3 && params[0].type==qfmpDouble) {
            evalID=params[0].toInteger();
            QFRDRCountRatesInterface* rdr=dynamic_cast<QFRDRCountRatesInterface*>(p->getRawDataByID(evalID));
            int run=-1;
            int channel=0;
            if (n==2 && params[1].type==qfmpDouble ) {
                run=params[1].toInteger();
            } else if (n==3 && params[1].type==qfmpDouble  && params[2].type==qfmpDouble ) {
                run=params[1].toInteger();
                channel=params[2].toInteger();
            } else  if (n!=1){
                parser->qfmpError(QObject::tr("cr_rate(rdrid, run=-1, channel=0) 1-3  integer arguments (%1 given)").arg(n));
                res.setInvalid();
                return;
            }
            if (rdr && channel>=0 && channel<rdr->getRateChannels() && run>=-1 && run<rdr->getRateRuns()) {
                res.setDoubleVec(rdr->getRateRun(run, channel), rdr->getRateN());
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("cr_rate(rdrid, run=-1, channel=0): the specified RDR is no count rate (CR) record"));
                    res.setInvalid();
                    return;
                } else if (!(channel>=0 && channel<rdr->getRateChannels())) {
                    parser->qfmpError(QObject::tr("cr_rate(rdrid, run=-1, channel=0): the specified channel (%1) is not available in the RDR").arg(channel));
                    res.setInvalid();
                    return;
                } else if (!(run>=0 && run<rdr->getRateRuns())) {
                    parser->qfmpError(QObject::tr("cr_rate(rdrid, run=-1, channel=0): the specified run (%1) is not available in the RDR").arg(run));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("cr_mean(rdrid, run=-1, channel=0) needs 1-3 integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}


void fCR_time(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==1 && params[0].type==qfmpDouble) {
            evalID=params[0].toInteger();
            QFRDRCountRatesInterface* rdr=dynamic_cast<QFRDRCountRatesInterface*>(p->getRawDataByID(evalID));
            if (rdr ) {
                res.setDoubleVec(rdr->getRateT(), rdr->getRateN());
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("cr_time(rdrid): the specified RDR is no count rate (CR) record"));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("cr_time(rdrid) needs one integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}


void fCR_runs(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==1 && params[0].type==qfmpDouble) {
            evalID=params[0].toInteger();
            QFRDRCountRatesInterface* rdr=dynamic_cast<QFRDRCountRatesInterface*>(p->getRawDataByID(evalID));
            if (rdr ) {
                res.setDouble(rdr->getRateRuns());
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("cr_runs(rdrid): the specified RDR is no count rate (CR) record"));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("cr_runs(rdrid) needs one integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}


void fCR_channels(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==1 && params[0].type==qfmpDouble) {
            evalID=params[0].toInteger();
            QFRDRCountRatesInterface* rdr=dynamic_cast<QFRDRCountRatesInterface*>(p->getRawDataByID(evalID));
            if (rdr ) {
                res.setDouble(rdr->getRateChannels());
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("cr_channels(rdrid): the specified RDR is no count rate (CR) record"));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("cr_channels(rdrid) needs one integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}


void fCR_mean(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n>=1 && n<=3 && params[0].type==qfmpDouble) {
            evalID=params[0].toInteger();
            QFRDRCountRatesInterface* rdr=dynamic_cast<QFRDRCountRatesInterface*>(p->getRawDataByID(evalID));
            int run=-1;
            int channel=0;
            if (n==2 && params[1].type==qfmpDouble ) {
                run=params[1].toInteger();
            } else if (n==3 && params[1].type==qfmpDouble  && params[2].type==qfmpDouble ) {
                run=params[1].toInteger();
                channel=params[2].toInteger();
            } else if (n!=1) {
                parser->qfmpError(QObject::tr("cr_mean(rdrid, run=-1, channel=0) 1-3  integer arguments (%1 given)").arg(n));
                res.setInvalid();
                return;
            }
            if (rdr && channel>=0 && channel<rdr->getRateChannels() && run>=-1 && run<rdr->getRateRuns()) {
                res.setDouble(rdr->getRateMean(run, channel));
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("cr_mean(rdrid, run=-1, channel=0): the specified RDR is no count rate (CR) record"));
                    res.setInvalid();
                    return;
                } else if (!(channel>=0 && channel<rdr->getRateChannels())) {
                    parser->qfmpError(QObject::tr("cr_mean(rdrid, run=-1, channel=0): the specified channel (%1) is not available in the RDR").arg(channel));
                    res.setInvalid();
                    return;
                } else if (!(run>=0 && run<rdr->getRateRuns())) {
                    parser->qfmpError(QObject::tr("cr_mean(rdrid, run=-1, channel=0): the specified run (%1) is not available in the RDR").arg(run));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("cr_mean(rdrid, run=-1, channel=0) needs 1-3 integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}


void fCR_std(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n>=1 && n<=3 && params[0].type==qfmpDouble) {
            evalID=params[0].toInteger();
            QFRDRCountRatesInterface* rdr=dynamic_cast<QFRDRCountRatesInterface*>(p->getRawDataByID(evalID));
            int run=-1;
            int channel=0;
            if (n==2 && params[1].type==qfmpDouble ) {
                run=params[1].toInteger();
            } else if (n==3 && params[1].type==qfmpDouble  && params[2].type==qfmpDouble ) {
                run=params[1].toInteger();
                channel=params[2].toInteger();
            } else if (n!=1) {
                parser->qfmpError(QObject::tr("cr_std(rdrid, run=-1, channel=0) 1-3  integer arguments (%1 given)").arg(n));
                res.setInvalid();
                return;
            }
            if (rdr && channel>=0 && channel<rdr->getRateChannels() && run>=-1 && run<rdr->getRateRuns()) {
                res.setDouble(rdr->getRateStdDev(run, channel));
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("cr_std(rdrid, run=-1, channel=0): the specified RDR is no count rate (CR) record"));
                    res.setInvalid();
                    return;
                } else if (!(channel>=0 && channel<rdr->getRateChannels())) {
                    parser->qfmpError(QObject::tr("cr_std(rdrid, run=-1, channel=0): the specified channel (%1) is not available in the RDR").arg(channel));
                    res.setInvalid();
                    return;
                } else if (!(run>=0 && run<rdr->getRateRuns())) {
                    parser->qfmpError(QObject::tr("cr_std(rdrid, run=-1, channel=0): the specified run (%1) is not available in the RDR").arg(run));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("cr_std(rdrid, run=-1, channel=0) 1-3 one integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}
