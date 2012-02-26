#include "../../qftools.h"
#include <cmath>
#include <QTextStream>
#include <QElapsedTimer>
#include <iostream>
#include <QStringList>


int main(int argc, char *argv[])
{
    QStringList test1, test1res;
    test1<<"123"<<"abc"<<"xyz"<<"XyZ";
    std::cout<<"escapify: "<<escapify(test1).toStdString()<<"\n";
    test1res=deescapifyList(escapify(test1));
    std::cout<<"deescapifyList(escapify): "<<((test1==test1res)?"OK":"COMPARE ERROR")<<" : "<<test1res.join(";;").toStdString()<<"\n";



    test1.clear();
    test1<<"1\n2\n3"<<"a\rb\tc"<<"x\"yz"<<"X\'yZ"<<"X\'y\'Z"<<"X\"yZ\"";
    std::cout<<"escapify: "<<escapify(test1).toStdString()<<"\n";
    test1res=deescapifyList(escapify(test1));
    std::cout<<"deescapifyList(escapify): "<<((test1==test1res)?"OK":"COMPARE ERROR")<<" : "<<test1res.join(";;").toStdString()<<"\n";



    return 0;
}
