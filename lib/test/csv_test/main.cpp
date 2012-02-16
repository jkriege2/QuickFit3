#include "../../csvtools.h"
#include <cmath>
#include <QTextStream>
#include <QElapsedTimer>
#include <iostream>
#include <QVector>


int main(int argc, char *argv[])
{
    std::cout<<"creating test data ...\n";
    QString data="# comment\n# comment\n";
    for (int i=0; i<100000; i++) {
        data.append(QString::number(i)+", ");
        data.append(QString::number(i*i));
        data.append(", "+QString::number(M_PI, 'g', 6));
        data.append(", "+QString::number(double(1)+M_PI, 'f', 12));
        data.append(", "+QString::number(double(2)+M_PI, 'g', 12));
        if (i%100==0)data.append("# comment, comment\n");
        else data.append("\n");
    }
    std::cout<<"test data size: "<<double(data.size())*2.0/1024.0/1024.0<<" MBytes\n";

    QTextStream str(&data);
    std::cout<<"reading test data ...\n";
    QElapsedTimer time;
    time.start();
    int i=0;
    while (!str.atEnd()) {
        QVector<double> d=csvReadline(str,',', '#', 0);
        if (d.size()>0) {
            if (i!=d[0]) std::cout<<"ERROR in line "<<i<<":   expected "<<i<<", but found "<<d[0]<<std::endl;
            if (d.size()!=5) std::cout<<"ERROR in line "<<i<<":   too many columns "<<d.size()<<std::endl;
        }
        i++;
    }
    std::cout<<"read "<<i<<" lines in "<<time.elapsed()<<" ms\n";

    return 0;
}
