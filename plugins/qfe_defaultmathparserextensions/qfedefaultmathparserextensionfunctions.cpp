#include "qfedefaultmathparserextensionfunctions.h"
#include "statistics_tools.h"

qfmpResult fRegression(const qfmpResult* params, unsigned int  n, QFMathParser* p){
    qfmpResult r;
    r.type=qfmpDoubleVector;
    if (n!=2 && n!=4) {
        p->qfmpError("regression(X,Y) needs 2 or 4 arguments");
        return p->getInvalidResult();
    }
    if (!(params[0].convertsToVector() && params[1].convertsToVector())) {
        p->qfmpError("regression(X,Y) arguments X and Y have to be of type number vector");
        return p->getInvalidResult();
    }
    if (params[0].length()!= params[1].length()) {
        p->qfmpError("regression(X,Y) arguments X and Y have to have the same length");
        return p->getInvalidResult();
    }

    double a, b;
    bool fixA=false;
    bool fixB=false;
    if (n==4) {
        if (!(params[2].type==qfmpString && params[3].type==qfmpDouble)) {
            p->qfmpError(QObject::tr("regression(X,Y,'fixParameter',fixValue) argument fixParameter has to be a string and fixValue a number, but parameters are %1 and %2").arg(params[2].toTypeString()).arg(params[3].toTypeString()));
            return p->getInvalidResult();
        }
        if (params[2].str.toLower()=="a") {
            fixA=true;
            a=params[3].num;
        } else if (params[2].str.toLower()=="b") {
            fixB=true;
            b=params[3].num;
        } else {
            p->qfmpError(QObject::tr("regression(X,Y,'fixParameter',fixValue) argument fixParameter has to be a string and fixValue a number 'a' or 'b', but was %1").arg(params[2].toTypeString()));
            return p->getInvalidResult();
        }
    }
    QVector<double> X=params[0].asVector();
    QVector<double> Y=params[1].asVector();
    statisticsLinearRegression(X.data(), Y.data(), qMin(X.size(), Y.size()), a, b, fixA, fixB);

    r.numVec.clear();
    r.numVec<<a;
    r.numVec<<b;
    return r;
}

qfmpResult fWeightedRegression(const qfmpResult* params, unsigned int  n, QFMathParser* p){
    qfmpResult r;
    r.type=qfmpDoubleVector;
    if (n!=3 && n!=5) {
        p->qfmpError("weighted_regression(X,Y,W) needs 3 or 5 arguments");
        return p->getInvalidResult();
    }
    if (!(params[0].convertsToVector() && params[1].convertsToVector() && params[2].convertsToVector())) {
        p->qfmpError("weighted_regression(X,Y,W) arguments X, Y and W have to be of type number vector");
        return p->getInvalidResult();
    }
    if (params[0].length()!= params[1].length() || params[0].length()!= params[2].length() || params[2].length()!= params[2].length()) {
        p->qfmpError("weighted_regression(X,Y,W) arguments X, Y and W have to have the same length");
        return p->getInvalidResult();
    }

    double a, b;
    bool fixA=false;
    bool fixB=false;
    if (n==5) {
        if (!(params[3].type==qfmpString && params[4].type==qfmpDouble)) {
            p->qfmpError("weighted_regression(X,Y,W,'fixParameter',fixValue) argument fixParameter has to be a string and fixValue a number");
            return p->getInvalidResult();
        }
        if (params[3].str.toLower()=="a") {
            fixA=true;
            a=params[4].num;
        } else if (params[3].str.toLower()=="b") {
            fixB=true;
            b=params[4].num;
        } else {
            p->qfmpError(QObject::tr("weighted_regression(X,Y,W,'fixParameter',fixValue) argument fixParameter has to be a string and fixValue a number 'a' or 'b', but was %1").arg(params[3].toTypeString()));
            return p->getInvalidResult();
        }
    }
    QVector<double> X=params[0].asVector();
    QVector<double> Y=params[1].asVector();
    QVector<double> W=params[2].asVector();
    statisticsLinearWeightedRegression(X.data(), Y.data(), W.data(), qMin(X.size(), Y.size()), a, b, fixA, fixB);

    r.numVec.clear();
    r.numVec<<a;
    r.numVec<<b;
    return r;
}

qfmpResult fIRLS(const qfmpResult* params, unsigned int  n, QFMathParser* p){
    qfmpResult r;
    r.type=qfmpDoubleVector;
    if (n<2 && n>6) {
        p->qfmpError("irls(X,Y) needs between 2 and 6 arguments");
        return p->getInvalidResult();
    }
    if (!(params[0].convertsToVector() && params[1].convertsToVector())) {
        p->qfmpError("irls(X,Y) arguments X and Y have to be of type number vector");
        return p->getInvalidResult();
    }
    if (params[0].length()!= params[1].length()) {
        p->qfmpError("irls(X,Y) arguments X and Y have to have the same length");
        return p->getInvalidResult();
    }

    double a, b;
    bool fixA=false;
    bool fixB=false;
    double Lp=1.1;
    int iterations=100;
    int fixOffset=2;
    if (n>=3) {
        if (params[2].type==qfmpDouble) {
            fixOffset=3;
            Lp=params[2].asNumber();
            if (n>=4) {
                if (params[3].type==qfmpDouble) {
                    fixOffset=4;
                    iterations=params[3].asNumber();
                }
            }
        }
    }

    if (n==fixOffset+2) {
        if (!(params[fixOffset+0].type==qfmpString && params[fixOffset+1].type==qfmpDouble)) {
            p->qfmpError("irls(X,Y,...,'fixParameter',fixValue) argument fixParameter has to be a string and fixValue a number");
            return p->getInvalidResult();
        }
        if (params[fixOffset+0].str.toLower()=="a") {
            fixA=true;
            a=params[fixOffset+1].num;
        } else if (params[fixOffset+0].str.toLower()=="b") {
            fixB=true;
            b=params[fixOffset+1].num;
        } else {
            p->qfmpError(QObject::tr("irls(X,Y,...,'fixParameter',fixValue) argument fixParameter has to be a string and fixValue a number 'a' or 'b', but was %1").arg(params[fixOffset+0].toTypeString()));
            return p->getInvalidResult();
        }
    }
    QVector<double> X=params[0].asVector();
    QVector<double> Y=params[1].asVector();
    statisticsIterativelyReweightedLeastSquaresRegression(X.data(), Y.data(), qMin(X.size(), Y.size()), a, b, Lp, iterations, fixA, fixB);

    r.numVec.clear();
    r.numVec<<a;
    r.numVec<<b;
    return r;
}
