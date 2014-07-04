#include "../../qftools.h"
#include <cmath>
#include <QTextStream>
#include <QElapsedTimer>
#include <iostream>
#include <QStringList>
#include "../../qfpreprocessormagic.h"
#include <QDebug>


int main(int argc, char *argv[])
{


    QStringList test1, test1res;
    test1<<"123"<<"abc"<<"xyz"<<"XyZ";
    qDebug()<<"escapify: "<<escapify(test1)<<"\n";
    test1res=deescapifyList(escapify(test1));
    qDebug()<<"deescapifyList(escapify): "<<((test1==test1res)?"OK":"COMPARE ERROR")<<" : "<<test1res.join(";;")<<"\n";



    test1.clear();
    test1<<"1\n2\n3"<<"a\rb\tc"<<"x\"yz"<<"X\'yZ"<<"X\'y\'Z"<<"X\"yZ\"";
    qDebug()<<"escapify: "<<escapify(test1)<<"\n";
    test1res=deescapifyList(escapify(test1));
    qDebug()<<"deescapifyList(escapify): "<<((test1==test1res)?"OK":"COMPARE ERROR")<<" : "<<test1res.join(";;")<<"\n";



    qDebug()<<"PREPROCESSOR MAGIC:\n";
    #define PARAMCNT 0
    qDebug()<<PARAMCNT;
    #define PARAMCNT1 QFPPM_INC(PARAMCNT)
    qDebug()<<PARAMCNT1;
    #define PARAMCNT2 QFPPM_INC(PARAMCNT1)
    qDebug()<<PARAMCNT2;


    QDir d("../../");
    qDebug()<<qfDirListFilesRecursive(d).join("\n");

    return 0;
}
