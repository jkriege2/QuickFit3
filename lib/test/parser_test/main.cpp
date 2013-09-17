#include <QtCore/QCoreApplication>
#include "../../qfmathparser.h"
#include <QDebug>
#include <stdint.h>
#include <QElapsedTimer>
#define TEST(expr) {\
    parser.resetErrors(); \
    QFMathParser::qfmpNode* n=parser.parse(expr); \
    qfmpResult r=n->evaluate(); \
    qDebug()<<expr<<"       =  "<<r.toTypeString()<<"\n"; \
    if (parser.hasErrorOccured()) { \
        qDebug()<<"   ERROR "<<parser.getLastErrors().join("\n    ")<<"\n" ;\
    }\
    if (doByteCode) { \
        QFMathParser::ByteCodeProgram bprog;\
        QFMathParser::ByteCodeEnvironment bcenv(&parser);\
        bcenv.init(&parser); \
        if (n->createByteCode(bprog, &bcenv)) { \
            double rr=NAN;\
            qDebug()<<expr<<"  =[BC]=  "<<(rr=parser.evaluateBytecode(bprog))<<"\n"; \
            if (r.getType()==qfmpDouble && rr!=r.asNumber()) { \
                qDebug()<<"   BCNOTEQUAL:   eval="<<r.toTypeString()<<"   bc="<<rr<<"   rel_error="<<(rr-r.asNumber())/r.asNumber();\
            } else if (r.getType()==qfmpBool && (rr!=0.0)!=r.asBool()) { \
                qDebug()<<"   BCNOTEQUAL:   eval="<<r.toTypeString()<<"   bc="<<(rr!=0.0);\
            }\
            if (showBytecode) qDebug()<<"\n-----------------------------------------------------------\n"<<QFMathParser::printBytecode(bprog)<<"\n-----------------------------------------------------------\n"; \
        }\
        if (parser.hasErrorOccured()) { \
            qDebug()<<"   BCERROR "<<parser.getLastErrors().join("\n    ")<<"\n" ;\
        }\
    }\
  }
#define TESTCMP(expr,expectedresult) {\
    parser.resetErrors(); \
    QFMathParser::qfmpNode* n=parser.parse(expr); \
    qfmpResult r=n->evaluate(); \
    qDebug()<<expr<<"       =  "<<r.toTypeString()<<"\n"; \
    if (parser.hasErrorOccured()) { \
        qDebug()<<"   ERROR "<<parser.getLastErrors().join("\n    ")<<"\n" ;\
    } else {\
        if (r.operator!=(qfmpResult(expectedresult))) { \
            qDebug()<<"   ERROR: RESULT NOT EQUAL TO EXPECTED RESULT (exprected: "<<qfmpResult(expectedresult).toTypeString()<<")!!!\n" ;\
        }\
    } \
    if (doByteCode) { \
        QFMathParser::ByteCodeProgram bprog;\
        QFMathParser::ByteCodeEnvironment bcenv(&parser);\
        bcenv.init(&parser); \
        if (n->createByteCode(bprog, &bcenv)) { \
            double rr=NAN;\
            qDebug()<<expr<<"  =[BC]=  "<<(rr=parser.evaluateBytecode(bprog))<<"\n"; \
            if (r.getType()==qfmpDouble && rr!=r.asNumber()) { \
                qDebug()<<"   BCNOTEQUAL:   eval="<<r.toTypeString()<<"   bc="<<rr<<"   rel_error="<<(rr-r.asNumber())/r.asNumber();\
            } else if (r.getType()==qfmpBool && (rr!=0.0)!=r.asBool()) { \
                qDebug()<<"   BCNOTEQUAL:   eval="<<r.toTypeString()<<"   bc="<<(rr!=0.0);\
            }\
            if (showBytecode) qDebug()<<"\n-----------------------------------------------------------\n"<<QFMathParser::printBytecode(bprog)<<"\n-----------------------------------------------------------\n"; \
        }\
        if (parser.hasErrorOccured()) { \
            qDebug()<<"   BCERROR "<<parser.getLastErrors().join("\n    ")<<"\n" ;\
        }\
    }\
  }
#define TESTCMP1(expectedresult) TESTCMP(#expectedresult, expectedresult)

double dbgif(bool test, double trueV, double falseV) {
    if (test) return trueV; else return falseV;
}
double dbgcases(bool test1, double trueV1, double elseV) {
    if (test1) return trueV1; else return elseV;
}
double dbgcases(bool test1, double trueV1, bool test2, double trueV2, double elseV) {
    if (test1) return trueV1;
    else if (test2) return trueV2;
    else return elseV;
}

double dbgcases(bool test1, double trueV1, bool test2, double trueV2, bool test3, double trueV3, double elseV) {
    if (test1) return trueV1;
    else if (test2) return trueV2;
    else if (test3) return trueV3;
    else return elseV;
}

#define SPEEDTEST(expr)     { \
    double a=1.23456; \
    double b=12.3456; \
    double c=-123.456; \
    parser.addVariableDouble("a", a); \
    parser.addVariableDouble("b", b); \
    parser.addVariableDouble("c", c); \
    int cnt=10000; \
    double pi=M_PI; \
    QElapsedTimer timer; \
    double el=0; \
    timer.start(); \
    volatile double val=1; \
    for (int i=0; i<cnt; i++) { \
        val=expr; \
    } \
    double native_val=val; \
    el=double(timer.nsecsElapsed())/1e6; \
    qDebug()<<"evaluations: "<<cnt; \
    qDebug()<<"expression: "<<#expr; \
    qDebug()<<"native:                                   "<<el<<" ms\t= "<<double(cnt)*1000.0/el<<" eval/sec\t     (result="<<val<<")"; \
    double nat=el; \
    parser.resetErrors(); \
    timer.start(); \
    QFMathParser::qfmpNode* n=parser.parse(#expr); \
    el=double(timer.nsecsElapsed())/1e6; \
    qDebug()<<"parsing: "<<el<<" ms"; \
    parser.resetErrors(); \
    QFMathParser::ByteCodeProgram bprog;\
    QFMathParser::ByteCodeEnvironment bcenv(&parser);\
    timer.start(); \
    bcenv.init(&parser); \
    bool bytecodeOK=false; \
    if (n->createByteCode(bprog, &bcenv)) { \
        el=double(timer.nsecsElapsed())/1e6; \
        qDebug()<<"generating bytecode: "<<el<<" ms (incl. environment init)"; \
        if (showBytecode) qDebug()<<"\n-----------------------------------------------------------\n"<<QFMathParser::printBytecode(bprog)<<"\n-----------------------------------------------------------\n"; \
        bytecodeOK=true; \
    } else {\
        qDebug()<<"generating bytecode: impossible\n"; \
        qDebug()<<"   ERROR "<<parser.getLastErrors().join("\n    ")<<"\n" ; \
    } \
     \
    timer.start(); \
    qfmpResult rtst; \
    for (int i=0; i<cnt; i++) { \
        rtst=n->evaluate(); \
    } \
    el=double(timer.nsecsElapsed())/1e6; \
    qDebug()<<"interpreted (evaluate with return value): "<<el<<" ms\t= "<<double(cnt)*1000.0/el<<" eval/sec\t     (result="<<rtst.toTypeString()<<")"; \
    qDebug()<<"interpreted/native : "<<el/nat; \
    if (parser.hasErrorOccured()) { \
        qDebug()<<"   ERROR "<<parser.getLastErrors().join("\n    ")<<"\n" ; \
    } \
    if (native_val!=rtst.asNumber()) { \
        qDebug()<<"   ERROR native and expression value differ rel_error="<<(native_val-rtst.asNumber())/fabs(native_val)<<":    native="<<native_val<<"    evaluated="<<rtst.asNumber(); \
    }\
    \
    timer.start(); \
    qfmpResult rr; \
    for (int i=0; i<cnt; i++) { \
        n->evaluate(rr); \
    } \
    el=double(timer.nsecsElapsed())/1e6; \
    qDebug()<<"interpreted (evaluate call-by-value):     "<<el<<" ms\t= "<<double(cnt)*1000.0/el<<" eval/sec\t     (result="<<rr.toTypeString()<<")"; \
    qDebug()<<"interpreted/native : "<<el/nat; \
    if (parser.hasErrorOccured()) { \
        qDebug()<<"   ERROR "<<parser.getLastErrors().join("\n    ")<<"\n" ; \
    } \
    if (native_val!=rr.asNumber()) { \
        qDebug()<<"   ERROR native and expression value differ rel_error="<<(native_val-rr.asNumber())/fabs(native_val)<<":    native="<<native_val<<"    evaluated="<<rr.asNumber(); \
    }\
    \
    if (doBytecode && bytecodeOK) { \
        timer.start(); \
        double rrb; \
        for (int i=0; i<cnt; i++) { \
            rrb=parser.evaluateBytecode(bprog); \
        } \
        el=double(timer.nsecsElapsed())/1e6; \
        qDebug()<<"interpreted (bytecode):                   "<<el<<" ms\t= "<<double(cnt)*1000.0/el<<" eval/sec\t     (result="<<rrb<<")"; \
        qDebug()<<"interpreted/native : "<<el/nat; \
        if (parser.hasErrorOccured()) { \
            qDebug()<<"   PARSER_ERROR "<<parser.getLastErrors().join("\n    ")<<"\n" ; \
        } \
        if (native_val!=rrb) { \
            qDebug()<<"   ERROR native and expression value differ rel_error="<<(native_val-rrb)/fabs(native_val)<<":    native="<<native_val<<"    evaluated="<<rrb; \
        }\
    } \
    qDebug()<<"\n"; \
}


#pragma GCC push_options
#pragma GCC optimize ("O0")
void speed_test(bool doBytecode, bool showBytecode) {
    QFMathParser parser;
    qDebug()<<"\n\n=========================================================";
    qDebug()<<"== SPEED TEST\n=========================================================";
    ;
    {
        SPEEDTEST(pi*b);
        SPEEDTEST(pi/b);
        SPEEDTEST(pi+b);
        SPEEDTEST(pi-b);
        SPEEDTEST(-pi);
        SPEEDTEST(pi*a+45.3e-4/1e-8-1);
        SPEEDTEST(sqrt(sin(a*b)));
        SPEEDTEST(sqrt(45.6*sin(pi*37.467)+45.3e-4/1e-8)-1);
        SPEEDTEST(1+1);
        SPEEDTEST(1.54-1.35);
        SPEEDTEST(1.54-5%2);
        SPEEDTEST(1.54-5.0/2.0);
        SPEEDTEST(1+2+3+4+5+6+7+8+9+10+11+12+13+14+15+16+17+18+19+20);
        SPEEDTEST(1-2-3-4-5-6-7-8-9-10-11-12-13-14-15-16-17-18-19-20);
        SPEEDTEST(sqrt(1+2)+sin(3+4)+sqrt(5+6)+sin(7+8)+cos(9+10)+sqrt(11+12)+cos(13+14)+sqrt(15+16)+sin(17+18)+cos(19+20));
        SPEEDTEST(dbgcases(a<b, 1, a>b, 2, a>=b, 3, -1));
        SPEEDTEST(a-b-c-a-b-c-c-b-a-c-b-a-c-a-c-b-a-18.0-c-a);
        SPEEDTEST(sqrt(a+b)+sin(b+c)+sqrt(a-c)+sin(b+a)+cos(a+c)+sqrt(b+b)+cos(a+a)+sqrt(-c)+sin(a*c)+cos(b+5.0));
        SPEEDTEST(cos(sqrt(a+b)+c*a));
    }
}
#pragma GCC pop_options

int main(int argc, char *argv[])
{
    
    QFMathParser parser;
    bool doByteCode=false;
    bool showBytecode=false;

    /*for (uint32_t i=0; i<0xFFFFFFFF; i++) {
        double di=i;
        uint32_t ii=di;
        if (ii!=i) qDebug()<<"   "<<i<<"!="<<ii<<" (double="<<di<<")";
        if (i%10000000==0) qDebug()<<i;
    }*/


    double pi=M_PI;
    TESTCMP("1+2+pi", 1.0+2.0+M_PI);
    TESTCMP("1+2*pi", 1.0+2.0*M_PI);
    TESTCMP("1*2+pi", 1.0*2.0+M_PI);
    TESTCMP1(1*(2+pi));
    TESTCMP1(sin(2.5*pi));
    TESTCMP1(sqrt(sin(2.5*pi)));
    TESTCMP1(-2.5e-45);
    TESTCMP1(2.5e-45);
    TESTCMP1(000.001);
    TESTCMP1(2.8e45);
    TESTCMP1(0b11010);
    TEST("0b112010");
    TESTCMP1(0xFF);
    TESTCMP1(0xFF&0x0F);
    TESTCMP1(0xFF&0x0F&0x02);
    TESTCMP1(0x01&0x0F&0x02);
    TESTCMP1(0x01|0x0F&0x02);
    TESTCMP1((0x01|0x0F)&0x02);
    TESTCMP1(0x01|(0x0F&0x02));

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
    TEST("xx=~(1:20)");
    TEST("sum(i,0,length(xx)-1,int2bin(xx[i])+\"   \")");
    TEST("sum(i,~xx,int2hex(i)+\"   \")");
    TEST("sum(i,~xx,int2oct(i)+\"   \")");
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

    TEST("x=1:5;y=5:-1:1");
    TEST("x=1:5; y=5:-1:1");
    TEST("1:5;5:-1:1");
    TEST("1:1:5;5:-1:1");
    TEST("[1,2,5];[5,4,3]");
    TEST("x=[1,2,5]; y=[5,4,3]");
    TEST("x=[1,2,5]; [5,4,3]");
    TEST("1; 5");
    TEST("x=1; 5");
    TEST("1; y=5");
    TEST("x=1; y=5");
    TEST("5:-1:1;1:5");
    TEST("5:-1:1;1:1:5");
    TEST("1:1:5;5:-1:1");
    TEST("[1,2,5];[5,4,3]");
    TEST("[1,2,5];[5,4,3];[4,3,pi]");
    TEST("[1,2,5];1:5;[4,3,pi]");
    TEST("[1,2,5];[5,4,3];1:5");
    TEST("x=[1,2,5]; y=[5,4,3]");
    TEST("x=[1,2,5]; [5,4,3]");
    TEST("x=[1,2,5]");
    TEST("[1,2,5]");




    TEST("prod(i,1,5,i)");
    TEST("prod(i,1,2,5,i)");
    TEST("sum(i,1,5,i)");
    TEST("sum(i,1,2,5,i)");
    TEST("sum(i,0,1,5,sum(j,0,3,j^2))");
    TEST("sum(i,0,1,5,sum(i,0,3,j^2))");
    TEST("sum(i,0,1,5,sum(j,0,i,j^2))");
    TEST("sum(i,0,1,5,sum(j,0,3,j^2))");
    TEST("sum(i,0,1,5,sum(i,0,3,i^2))");

    TEST("f(x)=x^2;f(23+a)");
    TEST("fib(x)=if(x<=1, 1, fib(x-1)+fib(x-2)); fib(5)");
    TEST("xx=a; j0(xx)-j0(a)");
    TEST("j1(a)");
    TEST("jn(1,2)");

    TEST("x=1:10");
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
    TEST("x=[\"a\", \"b\", \"c\", \"d\", \"e\", \"f\", \"g\", \"h\", \"i\", \"j\", \"k\", \"l\", \"m\", \"n\"]");
    TEST("x[5]=-1");
    TEST("x");
    TEST("x[5]=\"x\"");
    TEST("x");
    TEST("x[1:3]=\"inf\"");
    TEST("x");
    TEST("x[[7,9]]=\"inf\"");
    TEST("x");
    TEST("x[[7,9]]=[\"7\",\"9\"]");
    TEST("x");
    TEST("x[[1,3,5,7,9]]");
    TEST("x[0:2:9]");
    TEST("x=\"abcdefghijklmnop\"");
    TEST("x[5]=-1");
    TEST("x");
    TEST("x[5]=\"x\"");
    TEST("x");
    TEST("x[1:3]=\"z\"");
    TEST("x");
    TEST("x[[7,9]]=\"z\"");
    TEST("x");
    TEST("x[[7,9]]=[\"7\",\"9\"]");
    TEST("x");
    TEST("x[[1,3,5,7,9]]");
    TEST("x[0:2:9]");
    TEST("x=[true,false,true,false,true,false,true,false,true,false,true,false,true,false,true,false]");
    TEST("x[5]=-1");
    TEST("x");
    TEST("x[1:3]=false");
    TEST("x");
    TEST("x[[7,9]]=false");
    TEST("x");
    TEST("x[[7,9]]=[false,true]");
    TEST("x");
    TEST("x[[1,3,5,7,9]]");
    TEST("x[0:2:9]");
    TEST("concat(1,2,3,4)");
    TEST("concat(\"1\",\"1\",\"1\",\"1\")");
    TEST("removeall([\"1\",\"2\",\"3\",\"1\"], \"1\")");
    TEST("removeall([\"1\",\"2\",\"3\",\"1\"], \"0\")");
    TEST("removeall([\"1\",\"2\",\"3\",\"1\"], \"3\")");
    TEST("concat(true, false, true, false)");
    TEST("concat(1, false, \"a\", false)");
    TEST("concat(true, false, \"a\", false)");
    TEST("x=[1,2,3,4,5,6,7,8,9]");
    TEST("y=reverse(x)");
    TEST("idxb=(trunc(x%2)==0)");
    TEST("idx=find(trunc(x%2)==0)");
    TEST("idx=find(trunc(x%2), 0)");
    TEST("x[idx]");
    TEST("y[idx]");
    TEST("select(y, x%2==0)");
    TEST("num2str(x)");
    TEST("tosystempathseparator(\"bla\\bla/bla\")");
    TEST("tosystempathseparator([\"bla\\bla/bla\", \"bla/bla\\bla\"])");
    TEST("shuffle(x)");
    TEST("shuffle([true, true, true, true, false, false, false, false])");
    TEST("shuffle(num2str(x))");
    TEST("x==x");
    TEST("x==y");
    //speed_test(doByteCode, showBytecode);

    return 0;
}

