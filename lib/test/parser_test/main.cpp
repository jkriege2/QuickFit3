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
    TEST("0xFF&0x0F&0x02");
    TEST("0x01&0x0F&0x02");
    TEST("0x01|0x0F&0x02");
    TEST("(0x01|0x0F)&0x02");
    TEST("0x01|(0x0F&0x02)");

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
    TEST("s(x)=if(x>0, x+s(x-1), 0)");
    TEST("fib(x)=if(x<=1, 1, fib(x-1)+fib(x-2))");
    qDebug()<<"\n\n"<<parser.printFunctions()<<"\n\n";
    TEST("ff(2)+f(2)");

    TEST("s(0)");
    TEST("s(1)");
    TEST("s(2)");
    TEST("s(5)");
    TEST("fib(0)");
    TEST("fib(1)");
    TEST("fib(2)");
    TEST("fib(3)");
    TEST("fib(4)");
    TEST("fib(5)");

    TEST("if(pi==3, \"yes\", \"no\")");
    TEST("if(pi>3, \"is pi\", false)");
    TEST("\"1\"+cases(1>1, \" is >1\", 1>0.5, \" is >0.5\", \"else\")");

    TEST("[]");
    TEST("[1]");
    TEST("[1,2]");
    TEST("[f(2),ff(1),pi^2]");
    TEST("[1,\"bla\"]");
    TEST("1:1");
    TEST("vec=1:5");
    TEST("1:0.5:5");
    TEST("sum(vec)");
    TEST("for(vec)");

    TEST("for(i,0,5,i)");
    TEST("for(i,0:5,i)");
    TEST("sum(i,0,5,i)");
    TEST("sum(i,0:5,i)");
    TEST("for(i,0,2,5,i)");
    TEST("for(i,0:2:5,i)");
    TEST("sum(i,0,2,5,i)");
    TEST("sum(i,0:2:5,i)");

    TEST("for(i,0,5,i^2)");
    TEST("for(i,0:5,i^2)");
    TEST("sum(i,0,5,i^2)");
    TEST("sum(i,0:5,i^2)");
    TEST("for(i,0,2,5,i^2)");
    TEST("for(i,0:2:5,i^2)");
    TEST("sum(i,0,2,5,i^2)");
    TEST("sum(i,0:2:5,i^2)");

    TEST("prod(i,1,5,i)");
    TEST("prod(i,1,5,i^2)");
    TEST("prod(i,1,2,5,i)");
    TEST("prod(i,1,2,5,i^2)");
    TEST("for(i,0,5,i^2)");
    TEST("x=0:(pi/5):(2*pi)");
    TEST("y=x");
    TEST("sin(1)");
    TEST("sin(1:5)");
    TEST("sin(x)");
    TEST("cos(x)");
    TEST("atan2(x,y)");
    TEST("x=1:10");
    TEST("sum(x)");
    TEST("count(x)");
    TEST("mean(x)");
    TEST("var(x)");
    TEST("sqrt(var(x))");
    TEST("std(x)");
    TEST("median(x)");
    TEST("min(x)");
    TEST("max(x)");
    TEST("quantile(x,0.25)");
    TEST("quantile(x,0.75)");

    TEST("x[5]=-1");
    TEST("x");
    TEST("x[1:3]=nan");
    TEST("x");
    TEST("x[[7,9]]=inf");
    TEST("x");
    TEST("[-7,-9]");
    TEST("x[[7,9]]=[-7,-9]");
    TEST("x");
    TEST("x[[1,3,5,7,9]]");
    TEST("x[0:2:9]");
    TEST("removeall(x, -1)");
    TEST("remove(x, 1:3)");
    TEST("x=1:20");
    TEST("reverse(x)");
    TEST("shuffle(x)");
    TEST("dsort(x)");
    TEST("sort(x)");
    TEST("concat(1:2:4, [18,19,20], 3, [1, pi])");
    TEST("[1:2:4, [18,19,20], 3, 1, pi, 3:-1:1]");
    TEST("for(i,1,5,if(i<=3,-i,i^3))");
    TEST("x=1:5;y=5:-1:1");
    TEST("x+y");
    TEST("x*y");
    TEST("x/y");
    TEST("x&y");
    TEST("x|y");
    TEST("x%y");
    TEST("-x");
    TEST("x^y");
    TEST("x^2");
    TEST("xx=~(1:20)");
    TEST("sum(i,0,length(xx)-1,int2bin(xx[i])+\"   \")");
    TEST("sum(i,~xx,int2hex(i)+\"   \")");
    TEST("sum(i,~xx,int2oct(i)+\"   \")");



    return 0;
}
