#include "qfrdrimagestack_parserfunctions.h"
#include "qfrdrimagestack_data.h"
#include "qfmathparser.h"

void fStack_isstack(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==1 && params[0].type==qfmpDouble) {
            evalID=params[0].toInteger();
            QFRDRImageStackInterface* rdr=dynamic_cast<QFRDRImageStackInterface*>(p->getRawDataByID(evalID));
            res.setBoolean(rdr);
        } else {
            parser->qfmpError(QObject::tr("rdr_isimagestack(rdrid) needs one integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}

void fStack_getstacks(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==1 && params[0].type==qfmpDouble ) {
            evalID=params[0].toInteger();
            QFRDRImageStackInterface* rdr=dynamic_cast<QFRDRImageStackInterface*>(p->getRawDataByID(evalID));
            if (rdr) {
                res.setDouble(rdr->getImageStackCount());
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("imagestack_getstacks(rdrid): the specified RDR is no image stack record"));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("imagestack_getstacks(rdrid) needs one integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}
void fStack_getwidth(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==2 && params[0].type==qfmpDouble && params[1].type==qfmpDouble) {
            evalID=params[0].toInteger();
            int stack=params[1].toInteger();
            QFRDRImageStackInterface* rdr=dynamic_cast<QFRDRImageStackInterface*>(p->getRawDataByID(evalID));
            if (rdr && stack>=0 && stack<rdr->getImageStackCount()) {
                res.setDouble(rdr->getImageStackWidth(stack));
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("imagestack_getwidth(rdrid, stack): the specified RDR is no image stack record"));
                    res.setInvalid();
                    return;
                } else {
                    parser->qfmpError(QObject::tr("imagestack_getwidth(rdrid, stack): the specified stack (%1) is not available").arg(stack));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("imagestack_getwidth(rdrid, stack) needs two integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}

void fStack_getheight(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==2 && params[0].type==qfmpDouble && params[1].type==qfmpDouble) {
            evalID=params[0].toInteger();
            int stack=params[1].toInteger();
            QFRDRImageStackInterface* rdr=dynamic_cast<QFRDRImageStackInterface*>(p->getRawDataByID(evalID));
            if (rdr && stack>=0 && stack<rdr->getImageStackCount()) {
                res.setDouble(rdr->getImageStackHeight(stack));
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("imagestack_getheight(rdrid, stack): the specified RDR is no image stack record"));
                    res.setInvalid();
                    return;
                } else {
                    parser->qfmpError(QObject::tr("imagestack_getheight(rdrid, stack): the specified stack (%1) is not available").arg(stack));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("imagestack_getheight(rdrid, stack) needs two integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}
void fStack_getframes(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==2 && params[0].type==qfmpDouble && params[1].type==qfmpDouble) {
            evalID=params[0].toInteger();
            int stack=params[1].toInteger();
            QFRDRImageStackInterface* rdr=dynamic_cast<QFRDRImageStackInterface*>(p->getRawDataByID(evalID));
            if (rdr && stack>=0 && stack<rdr->getImageStackCount()) {
                res.setDouble(rdr->getImageStackFrames(stack));
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("imagestack_getframes(rdrid, stack): the specified RDR is no image stack record"));
                    res.setInvalid();
                    return;
                } else {
                    parser->qfmpError(QObject::tr("imagestack_getframes(rdrid, stack): the specified stack (%1) is not available").arg(stack));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("imagestack_getframes(rdrid, stack) needs two integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}
void fStack_getchannels(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        if (n==2 && params[0].type==qfmpDouble && params[1].type==qfmpDouble) {
            evalID=params[0].toInteger();
            int stack=params[1].toInteger();
            QFRDRImageStackInterface* rdr=dynamic_cast<QFRDRImageStackInterface*>(p->getRawDataByID(evalID));
            if (rdr && stack>=0 && stack<rdr->getImageStackCount()) {
                res.setDouble(rdr->getImageStackChannels(stack));
            } else {
                if (!rdr) {
                    parser->qfmpError(QObject::tr("imagestack_getchannels(rdrid, stack): the specified RDR is no image stack record"));
                    res.setInvalid();
                    return;
                } else {
                    parser->qfmpError(QObject::tr("imagestack_getchannels(rdrid, stack): the specified stack (%1) is not available").arg(stack));
                    res.setInvalid();
                    return;
                }
            }
        } else {
            parser->qfmpError(QObject::tr("imagestack_getchannels(rdrid, stack) needs two integer arguments (%1 given)").arg(n));
            res.setInvalid();
            return;
        }
    }
}


void fStack_getframe(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        int stack=0;
        int channel=0;
        int frame=0;
        QFRDRImageStackInterface* rdr=NULL;
        if (n<1 || n>4) {
            parser->qfmpError(QObject::tr("imagestack_getframe(rdrid,...):requires 1-4 integer parameters, but %1 given").arg(n));
            res.setInvalid();
            return;
        }

        if (n>=1 && params[0].type==qfmpDouble ) {
            evalID=params[0].toInteger();
            rdr=dynamic_cast<QFRDRImageStackInterface*>(p->getRawDataByID(evalID));
            if (!rdr) {
                parser->qfmpError(QObject::tr("imagestack_getframe(rdrid,...): the specified RDR is no image stack record"));
                res.setInvalid();
                return;
            }
        } else {
            parser->qfmpError(QObject::tr("imagestack_getframe(rdrid,...): the specified RDR is no valid integer number"));
            res.setInvalid();
            return;
        }

        if (n>=2 && params[1].type==qfmpDouble) {
            stack=params[1].toInteger();
            if (stack<0 || stack>=rdr->getImageStackCount()) {
                parser->qfmpError(QObject::tr("imagestack_getframe(rdrid,stack,...): the specified stack is not available in this RDR"));
                res.setInvalid();
                return;
            }
        }  else {
            parser->qfmpError(QObject::tr("imagestack_getframe(rdrid,stack,...): the specified stack is no valid integer number"));
            res.setInvalid();
            return;
        }

        if (n>=3 && params[2].type==qfmpDouble) {
            frame=params[2].toInteger();
            if (stack<0 || stack>=rdr->getImageStackFrames(stack)) {
                parser->qfmpError(QObject::tr("imagestack_getframe(rdrid,stack,frame,...): the specified frame is not available in this RDR"));
                res.setInvalid();
                return;
            }
        }  else {
            parser->qfmpError(QObject::tr("imagestack_getframe(rdrid,stack,frame,...): the specified frame is no valid integer number"));
            res.setInvalid();
            return;
        }

        if (n==4 && params[3].type==qfmpDouble) {
            channel=params[3].toInteger();
            if (stack<0 || stack>=rdr->getImageStackChannels(stack)) {
                parser->qfmpError(QObject::tr("imagestack_getframe(rdrid,stack,frame,channel): the specified channel is not available in this RDR"));
                res.setInvalid();
                return;
            }
        }  else {
            if (n==4){
                parser->qfmpError(QObject::tr("imagestack_getframe(rdrid,stack,frame,channel): the specified channel is no valid integer number"));
            } else {
                parser->qfmpError(QObject::tr("imagestack_getframe(rdrid,stack,frame,channel): only accepts upt to 4 parameters"));
            }
            res.setInvalid();
            return;
        }

        res.setDoubleVec(rdr->getImageStack(stack, frame, channel), rdr->getImageStackWidth(stack)*rdr->getImageStackHeight(stack));
    }
}

void fStack_getstack(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    res.setInvalid();
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if (p)  {
        int evalID=-1;
        int stack=0;
        int channel=0;
        QFRDRImageStackInterface* rdr=NULL;
        if (n<1 || n>3) {
            parser->qfmpError(QObject::tr("imagestack_getstack(rdrid,...):requires 1-3 integer parameters, but %1 given").arg(n));
            res.setInvalid();
            return;
        }
        if (n>=1 && params[0].type==qfmpDouble ) {
            evalID=params[0].toInteger();
            rdr=dynamic_cast<QFRDRImageStackInterface*>(p->getRawDataByID(evalID));
            if (!rdr) {
                parser->qfmpError(QObject::tr("imagestack_getstack(rdrid,...): the specified RDR is no image stack record"));
                res.setInvalid();
                return;
            }
        } else {
            parser->qfmpError(QObject::tr("imagestack_getstack(rdrid,...): the specified RDR is no valid integer number"));
            res.setInvalid();
            return;
        }

        if (n>=2 && params[1].type==qfmpDouble) {
            stack=params[1].toInteger();
            if (stack<0 || stack>=rdr->getImageStackCount()) {
                parser->qfmpError(QObject::tr("imagestack_getstack(rdrid,stack,...): the specified stack is not available in this RDR"));
                res.setInvalid();
                return;
            }
        }  else {
            parser->qfmpError(QObject::tr("imagestack_getstack(rdrid,stack,...): the specified stack is no valid integer number"));
            res.setInvalid();
            return;
        }


        if (n==3 && params[3].type==qfmpDouble) {
            channel=params[3].toInteger();
            if (stack<0 || stack>=rdr->getImageStackChannels(stack)) {
                parser->qfmpError(QObject::tr("imagestack_getstack(rdrid,stack,channel): the specified channel is not available in this RDR"));
                res.setInvalid();
                return;
            }
        }  else {
            if (n==3){
                parser->qfmpError(QObject::tr("imagestack_getstack(rdrid,stack,channel): the specified channel is no valid integer number"));
            } else {
                parser->qfmpError(QObject::tr("imagestack_getstack(rdrid,stack,channel): only accepts upt to 4 parameters"));
            }
            res.setInvalid();
            return;
        }

        res.setDoubleVec(rdr->getImageStack(stack, 0, channel), rdr->getImageStackWidth(stack)*rdr->getImageStackHeight(stack)*rdr->getImageStackFrames(stack));
    }
}
