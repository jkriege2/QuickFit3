#include <QtCore/QCoreApplication>
#include "../../qfmathparser.h"
#include <QDebug>
#include <stdint.h>

#define TEST(expr) {\
    parser.resetErrors(); \
    QFMathParser::qfmpResult r=parser.evaluate(expr); \
    qDebug()<<expr<<"  =  "<<r.toTypeString()<<"\n"; \
    if (parser.hasErrorOccured()) { \
        qDebug()<<"   ERROR "<<parser.getLastErrors().join("\n    ")<<"\n" ;\
    }\
  }

int main(int argc, char *argv[])
{
    
    QFMathParser parser;

    /*for (uint32_t i=0; i<0xFFFFFFFF; i++) {
        double di=i;
        uint32_t ii=di;
        if (ii!=i) qDebug()<<"   "<<i<<"!="<<ii<<" (double="<<di<<")";
        if (i%10000000==0) qDebug()<<i;
    }*/


    TEST("1+2+pi");
    TEST("1+2*pi");
    TEST("1*2+pi");
    TEST("1*(2+pi)");
    TEST("sin(2.5*pi)");
    TEST("sqrt(sin(2.5*pi))");

    qDebug()<<parser.printVariables();
    TEST("a=5*5+0.1");
    TEST("a=5*5+0.2");
    TEST("b=true&&false");
    TEST("s1=\"blablaBLA\"");
    TEST("s2=\"_BLUB12\"");
    TEST("s=tolower(\"blablaBLA\"+s2)");
    qDebug()<<parser.printVariables();
    return 0;
}
