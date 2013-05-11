#include <QtCore/QCoreApplication>
#include "../../qfmathparser.h"
#include <QDebug>
#include <stdint.h>
#include <QElapsedTimer>
#define TEST(expr) {\
    parser.resetErrors(); \
    QFMathParser::qfmpResult r=parser.evaluate(expr); \
    qDebug()<<expr<<"  =  "<<r.toTypeString()<<"\n"; \
    if (parser.hasErrorOccured()) { \
        qDebug()<<"   ERROR "<<parser.getLastErrors().join("\n    ")<<"\n" ;\
    }\
  }


#define SPEEDTEST(expr)     { \
    int cnt=1000; \
    double pi=M_PI; \
    QElapsedTimer timer; \
    double el=0; \
    timer.start(); \
    double val=1; \
    for (int i=0; i<cnt; i++) { \
        val=expr; \
    } \
    el=double(timer.nsecsElapsed())/1e6; \
    qDebug()<<"evaluations: "<<cnt; \
    qDebug()<<"expression: "<<#expr; \
    qDebug()<<"native: "<<el<<" ms     (result="<<val<<")"; \
    double nat=el; \
    parser.resetErrors(); \
    timer.start(); \
    QFMathParser::qfmpNode* n=parser.parse(#expr); \
    el=double(timer.nsecsElapsed())/1e6; \
    qDebug()<<"parsing: "<<el<<" ms"; \
     \
    timer.start(); \
    for (int i=0; i<cnt; i++) { \
        QFMathParser::qfmpResult r=n->evaluate(); \
        val=r.num; \
    } \
    el=double(timer.nsecsElapsed())/1e6; \
    qDebug()<<"interpreted: "<<el<<" ms     (result="<<val<<")"; \
    qDebug()<<"interpreted/native : "<<el/nat; \
    if (parser.hasErrorOccured()) { \
        qDebug()<<"   ERROR "<<parser.getLastErrors().join("\n    ")<<"\n" ; \
    } \
    qDebug()<<"\n"; \
}


#pragma GCC push_options
#pragma GCC optimize ("O0")
void speed_test() {
    QFMathParser parser;
    qDebug()<<"\n\n=========================================================";
    qDebug()<<"== SPEED TEST\n=========================================================";

    {
        SPEEDTEST(pi*37.467);
        SPEEDTEST(pi*37.467+45.3e-4/1e-8-1);
        SPEEDTEST(sqrt(sin(2.5*58)));
        SPEEDTEST(sqrt(45.6*sin(pi*37.467)+45.3e-4/1e-8)-1);
    }
}
#pragma GCC pop_options

int main(int argc, char *argv[])
{
    
    QFMathParser parser;

    /*for (uint32_t i=0; i<0xFFFFFFFF; i++) {
        double di=i;
        uint32_t ii=di;
        if (ii!=i) qDebug()<<"   "<<i<<"!="<<ii<<" (double="<<di<<")";
        if (i%10000000==0) qDebug()<<i;
    }*/

    speed_test();

    TEST("1+2+pi");
    TEST("1+2*pi");
    TEST("1*2+pi");
    TEST("1*(2+pi)");
    TEST("sin(2.5*pi)");
    TEST("sqrt(sin(2.5*pi))");
    TEST("-2.5e-45");
    TEST("2.5e-45");
    TEST("000.001");
    TEST("2.8e45");
    TEST("0b11010");
    TEST("0b112010");
    TEST("0xFF");
    TEST("0xFF&0x0F");

    qDebug()<<"\n\n"<<parser.printVariables()<<"\n\n";
    TEST("a=5*5+0.1");
    TEST("a=5*5+0.2");
    TEST("b=true&&false");
    TEST("s1=\"blablaBLA\"");
    TEST("s2=\"_BLUB12\"");
    TEST("s=tolower(\"blablaBLA\"+s2)");
    qDebug()<<"\n\n"<<parser.printVariables()<<"\n\n";

    qDebug()<<"\n\n"<<parser.printFunctions()<<"\n\n";
    TEST("ff(x)=x^2");
    TEST("f(x)=ff(x)^2");
    qDebug()<<"\n\n"<<parser.printFunctions()<<"\n\n";
    TEST("ff(2)+f(2)");

    return 0;
}
