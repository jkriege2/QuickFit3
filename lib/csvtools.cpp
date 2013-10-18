#include "csvtools.h"
#include <iostream>
#include <math.h>
#include <QDebug>
#include <QChar>
#include "qftools.h"
#include <QApplication>
#include <QMimeData>
#include <QClipboard>

QVector<double> csvReadline(QTextStream& f, QChar separator_char, QChar comment_char, double non_value, const QString& eolChars, const QString& ignoreChars) {
    //const QString line=f.readLine();
    QString line="";
    QString ch=f.read(1);
    while (!f.atEnd() && !eolChars.contains(ch)) {
        line.append(ch);
        ch=f.read(1);
    }
    if (!eolChars.contains(ch)) line.append(ch);
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
                /*case '\r': break;
                case '\n':
                    done=true;
                    if (num.size()>0) {
                        result.append(loc.toDouble(num, &ok));
                    }
                    num="";
                    break;*/
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
                    if (ignoreChars.contains(ch)) {
                         // ignore this character ;-)
                    } else if (eolChars.contains(ch)) {
                        // found an end of line character
                        done=true;
                        if (num.size()>0) {
                            result.append(loc.toDouble(num, &ok));
                        }
                        num="";

                    } else if (ch=='.' && separator_char!='.') {
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
            if (eolChars.contains(ch)) {
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




void csvCopy(const QList<QList<double> >& data, const QStringList& columnsNames, const QStringList& rowNames) {

    QString csv, csvLocale, excel;
    QLocale loc;
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    int datas=0;
    for (int r=0; r<data.size(); r++) {
        if (data[r].size()>datas) datas=data[r].size();
    }

    if (columnsNames.size()>0) {
        csv+="#! ";
        if (rowNames.size()>0) {
            csv+=csv+QObject::tr("\"row name\", ");
            csvLocale+=csvLocale+"; ";
            excel+=excel+"\t";
        }
        for (int i=0; i<columnsNames.size(); i++) {
            if (i>0) {
                csv+=", ";
                csvLocale+="; ";
                excel+="\t";
            }
            if (!columnsNames.value(i, "").isEmpty()) {

                QString strans=columnsNames.value(i, "");
                strans=strans.replace("\"", "").replace("'", "").replace("\n", " ").replace("\r", " ");
                strans=QString("\"%1\"").arg(strans);
                csv+=strans;
                csvLocale+=strans;
                excel+=strans;
            }
        }
        csv+="\n";
        csvLocale+="\n";
        excel+="\n";
    }
    for (int r=0; r<datas; r++) {
        if (rowNames.size()>0) {
            QString strans=rowNames.value(r, "");
            strans=strans.replace("\"", "\\\"").replace("'", "\\\'").replace("\n", " ").replace("\r", " ").replace("\t", " ");
            strans=QString("\"%1\"").arg(strans);
            QString strans_nc=strans;
            strans_nc=strans_nc.replace(',', ';');
            QString strans_ns=strans;
            strans_ns=strans_ns.replace(';', ',');
            csv+=strans_nc+", ";
            csvLocale+=strans_ns+"; ";
            excel+=strans+"\t";
        }
        for (int c=0; c<data.size(); c++) {
            if (c>0) {
                csv+=", ";
                csvLocale+="; ";
                excel+="\t";
            }
            if (r<data[c].size()) {
                double d=data[c].at(r);

                excel+=loc.toString(d);
                csvLocale+=loc.toString(d);
                csv+=CDoubleToQString(d);
            }
        }
        csv+="\n";
        csvLocale+="\n";
        excel+="\n";
    }



    QClipboard *clipboard = QApplication::clipboard();
    QMimeData* mime=new QMimeData();
    mime->setText(excel);
    mime->setData("jkqtplotter/csv", csv.toUtf8());
    mime->setData("application/vnd.ms-excel", excel.toLocal8Bit());
    mime->setData("quickfit/csv", csv.toUtf8());
    mime->setData("text/csv", csv.toLocal8Bit());
    mime->setData("text/comma-separated-values", csv.toLocal8Bit());
    clipboard->setMimeData(mime);
}

void csvCopy(const QList<QVector<double> >& data, const QStringList& columnsNames, const QStringList& rowNames) {
    QList<QList<double> > d;
    for (int i=0; i<data.size(); i++) {
        d.append(data[i].toList());
    }
    csvCopy(d, columnsNames, rowNames);
}

QList<QVector<double> > csvDataRotate(const QList<QVector<double> >& data) {
    QList<QVector<double> > result;
    int cols=0;
    for (int i=0; i<data.size(); i++) {
        if (cols<data[i].size()) cols=data[i].size();
    }
    QVector<double> dEmpty(data.size(), NAN);
    for (int i=0; i<cols; i++) {
        result<<dEmpty;
    }
    for (int i=0; i<data.size(); i++) {
        for (int r=0; r<data[i].size(); r++) {
            result[r].operator[](i)=data[i].at(r);
        }
    }
    return result;
}

QList<QList<double> > csvDataRotate(const QList<QList<double> >& data) {
    QList<QList<double> > result;
    int cols=0;
    for (int i=0; i<data.size(); i++) {
        if (cols<data[i].size()) cols=data[i].size();
    }
    QList<double> dEmpty;
    for (int i=0; i<data.size(); i++) dEmpty<<NAN;
    for (int i=0; i<cols; i++) {
        result<<dEmpty;
    }
    for (int i=0; i<data.size(); i++) {
        for (int r=0; r<data[i].size(); r++) {
            result[r].operator[](i)=data[i].at(r);
        }
    }
    return result;
}
