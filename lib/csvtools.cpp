#include "csvtools.h"
#include <iostream>

QVector<double> csvReadline(QTextStream& f, QChar separator_char, QChar comment_char) {
    QString line=f.readLine();
    //std::cout<<"line='"<<line.toStdString()<<"'\n";
    QVector<double> result;
    bool done=false;
    QString num="";
    bool isComment=false;
    for (int i=0; i<line.size(); i++) {
        QChar ch=line[i];
        if (!isComment) {
            switch(ch.toAscii()) {
                case '\r': break;
                case '\n':
                    done=true;
                    if (num.size()>0) {
                        result.append(num.toDouble());
                    }
                    num="";
                    break;
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
                case '.':
                case '-':
                case 'e': case 'E':
                    num=num+ch;
                    break;
                default:
                    if (ch==separator_char) {
                        if (num.size()>0) {
                            result.append(num.toDouble());
                        }
                        num="";
                    } else if (ch==comment_char) {
                        isComment=true;
                        if (num.size()>0) {
                            result.append(num.toDouble());
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
        }

        if (done) break;
    }
    if ((!done) && num.size()>0) {
        result.append(num.toDouble());
    }
    return result;
}
