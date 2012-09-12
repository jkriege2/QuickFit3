#include "csvtools.h"
#include <iostream>
#include <math.h>
#include <QDebug>
#include <QChar>

QVector<double> csvReadline(QTextStream& f, QChar separator_char, QChar comment_char, double non_value) {
    const QString line=f.readLine();
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    //qDebug()<<"line='"<<line;
    QVector<double> result;
    bool done=false;
    QString num="";
    bool isComment=false;
    bool ok=false;
    double d=0;
    //for (int i=0; i<line.size(); i++) {
    for (QString::const_iterator i=line.begin(); i!=line.end(); ++i) {
        const QChar ch=*i;//line[i];
        const char cc=ch.toAscii();
        if (!isComment) {
            switch(cc) {

                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case 'e':
                case '-':
                case '+':
                case 'n':
                case 'a':
                case 'i':
                case 'f':
                    num=num+ch;
                    break;
                case '\r': break;
                case '\n':
                    done=true;
                    if (num.size()>0) {
                        result.append(loc.toDouble(num, &ok));
                    }
                    num="";
                    break;
                case 'N':
                    num=num+'n';
                    break;
                case 'A':
                    num=num+'a';
                    break;
                case 'I':
                    num=num+'i';
                    break;
                case 'F':
                    num=num+'f';
                    break;
                case 'E':
                    num=num+'e';
                    break;
                default:
                    if (ch=='.' && separator_char!='.') {
                        num=num+'.';
                    } else if (ch==',' && separator_char!=',') {
                        num=num+'.';
                    } else if (ch==separator_char) {
                        if (num.size()>0) {
                            ok=true;
                            if (num=="nan") {
                                result.append(NAN);
                            } else if (num=="inf") {
                                result.append(INFINITY);
                            } else{
                                d=loc.toDouble(num, &ok);
                                if (ok) result.append(d);
                                else result.append(non_value);
                            }
                        }
                        num="";
                    } else if (ch==comment_char) {
                        isComment=true;
                        if (num.size()>0) {
                            ok=true;
                            if (num=="nan") {
                                result.append(NAN);
                            } else{
                                d=num.toDouble(&ok);
                                if (ok) result.append(d);
                                else result.append(non_value);
                            }
                        }
                        num="";
                        done=true;
                    }
                    break;
            }
        } else {
            if (ch=='\n') {
                done=true;
            }
            break;
        }

        if (done) break;
    }
    if ((!done) && num.size()>0) {
        result.append(loc.toDouble(num, &ok));
    }
    if (result.size()<=0 && isComment) return csvReadline(f, separator_char, comment_char, non_value);
    return result;
}
