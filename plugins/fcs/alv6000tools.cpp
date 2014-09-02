/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "alv6000tools.h"
#include <QLocale>
#include <errno.h>



/**************************************************************************************************************************
 * methods and functions, that are not part of a class
 **************************************************************************************************************************/
void ALV6_analyze(QString filename, QString& mode, unsigned int& channelCount, unsigned int& runCount, bool& crossCorrelation) {
    FILE* alv_file=fopen(filename.toAscii().data(), "r");
    if (ferror(alv_file)) throw alv6_exception(format("error while opening file '%s':\n  %s", filename.toAscii().data(), strerror(errno)));
    bool readingHeader=true;
    //bool isCorrelation=false;
    //bool isCounRate=false;
    bool findIdentifier=true;
    ALV6_TOKEN token=ALV6_getToken(alv_file, readingHeader);
    // first we parse the header (until we find the first quoted string)
    // the data from the header is interpeted and the resulting quickfitDataStoreItem are
    // beeing created.
    while (token.type!=ALV6_EOF && readingHeader) {
        if (findIdentifier) {
            if (token.type==ALV6_NAME) {
                if (token.value.contains("ALV-6000", Qt::CaseInsensitive)) {
                    findIdentifier=false;
                } else if (token.type!=ALV6_LINEBREAK && token.type!=ALV6_EOF) {
                    throw alv6_exception("did not find file header 'ALV-6000'");
                }
            }
        } else {
            if (token.type==ALV6_NAME) { // here we parse "<name> : <value>"
                QString name=token.value; // store <name>
                QString value="";
                // get next token which has to be a colon':'
                token=ALV6_getToken(alv_file, readingHeader);
                //if (token.type!=ALV_COLON) throw alv6_exception(format("colon ':' expected, but found '%s'", token.value.toAscii().data()));
                if (token.type==ALV6_COLON) {
                    // get next token which has to be a value or a quoted string or a linebreak
                    token=ALV6_getToken(alv_file, readingHeader);
                    if (token.type==ALV6_QUOTED) {
                        value=token.value;
                    } else if (token.type==ALV6_VALUE) {
                        value=token.value;
                    } else if (token.type!=ALV6_LINEBREAK) {
                        throw alv6_exception(format("linebreak, number or quoted string expected"));
                    }
                    // here we check whether this is an interpreted data field (i.e. it is stored in a separate field of item1
                    if (name.compare("Runs",  Qt::CaseInsensitive)==0) {
                        runCount=(int)round(token.doubleValue);
                        //std::cout<<"runs found "<<item1->runs<<std::endl;
                    } else if (name.compare("Mode",  Qt::CaseInsensitive)==0) {
                        mode=token.value;
                        crossCorrelation=token.value.contains("CROSS", Qt::CaseInsensitive);
                        channelCount=1;
                        if (token.value.contains("DUAL", Qt::CaseInsensitive)) channelCount=2;

                    }
                }
            } else if (token.type==ALV6_QUOTED) {
                // we stop reading the header when we meet the first quoted string token
                // this is possible as every line of the header begins with an unquoted name
                // token followed by a colon and number/quoted or alineabreak (identifier line!)
                readingHeader=false;
            }
        }
        if (readingHeader) token=ALV6_getToken(alv_file, readingHeader);
    }
}


ALV6_TOKEN ALV6_readNumberMatrix(FILE* alv_file, QVector<QVector<double> >* datm) {
    //std::cout<<"<<<entered ALV6_readNumberMatrix>>>\n";

    QVector<double> data; // a single row may contain 1024 columns at the most
                       // this field is used to temporarily save the read data
    QVector<QVector<double> > dat;
    // we do not already know how many rows make up the correlation data, so we first read all rows into
    // a buffer (vector dat). Then we can determine the number of data rows and get an according amount
    // of memory in item1.

    // now we read until the end of the file or the first non-number token
    // (name or quoted, whitespace/linebreaks will be swallowed)
    ALV6_TOKEN token=ALV6_getToken(alv_file, false);
    bool first=true; // if true: all linebreaks and other tokens will be ignored (= search for first ALV_VALUE)
    //unsigned int count=0;
    bool end=false;
    while (!end && token.type!=ALV6_EOF && (first || (!first && token.type!=ALV6_QUOTED))) {
        if (token.type==ALV6_VALUE) {
            // just the next number
            data.push_back(token.doubleValue);
            first=false;
        } else if (token.type==ALV6_LINEBREAK && !first) {
            // end of a line of numbers
            //std::cout<<"linebreak"<<std::endl;
            if (data.size()==0) {
                // a block of numbers (correlation data) is ended by an empty line
                // (= no items in data vector!)
                end=true;
            } else {
                dat.push_back(data);
                data.clear();
            }
        }
        if (!end) token=ALV6_getToken(alv_file, false); // get next token from file
        //std::cout<<booltostr(first)<<"    "<<token.value.toStdString()<<std::endl;
    }
    *datm=dat;
    //std::cout<<"<<<leaving ALV6_readNumberMatrix>>>\n";
    return token;
}



QString ALV6_ReadQuoted(FILE* alv_file, char quotationMark) {
    unsigned char ch;
    ch=fgetc(alv_file);
    if (ferror(alv_file)) throw alv6_exception(format("error during file access operation:\n  %s", strerror(errno)));
    while((!feof(alv_file)) && (ch<=32) && (ch!='\n')) {
        ch=fgetc(alv_file);
        if (ferror(alv_file)) throw alv6_exception(format("error during file access operation:\n  %s", strerror(errno)));
        //std::cout<<"   quoted_before: '"<<ch<<"'"<<std::endl;
    }
    if (ch!=quotationMark) throw alv6_exception(format("unexcpected character: found '%c', but expected '%c'", ch, quotationMark));
    QString data="";
    ch=0;
    while((!feof(alv_file)) && (ch!='\n') && (ch!=quotationMark)) {
        //std::cout<<"   quoted: '"<<ch<<"'"<<std::endl;
        ch=fgetc(alv_file);
        if (ferror(alv_file)) throw alv6_exception(format("error during file access operation:\n  %s", strerror(errno)));
        if ((ch!='\n') && (ch!=quotationMark)) data=data+ch;
    }
    return data;
}

QDate ALV6_toDate(QString d) {
    return QDate::fromString(d, "dd-MMM-yy");
}

QTime ALV6_toTime(QString d){
    return QTime::fromString(d, "hh:mm:ss");
}

ALV6_TOKEN ALV6_getToken(FILE* alv_file, bool readingHeader) {
    ALV6_TOKEN t;
    t.value="";
    t.doubleValue=0;
    unsigned char ch;
    ch=fgetc(alv_file);
    if (ferror(alv_file)) throw alv6_exception(format("error during file access operation:\n  %s", strerror(errno)));
    while((!feof(alv_file)) && ((ch<=32) && (ch!='\n'))) {
        ch=fgetc(alv_file);
        if (ferror(alv_file)) throw alv6_exception(format("error during file access operation:\n  %s", strerror(errno)));
    }
    if (feof(alv_file)||(ch==255)) {
        t.type=ALV6_EOF;
        return t;
    }
    if (ch=='\n') {
        t.type=ALV6_LINEBREAK;
        return t;
    }
    if (ch==':') {
        t.type=ALV6_COLON;
        return t;
    }
    if (ch=='"' || ch=='\''){
        t.type=ALV6_QUOTED;
        ungetc(ch, alv_file);
        if (ferror(alv_file)) throw alv6_exception(format("error during file access operation:\n  %s", strerror(errno)));
        t.value=ALV6_ReadQuoted(alv_file, ch);
        return t;
    }
    if (isdigit(ch)||(ch=='-')||(ch=='+')||(ch=='.')||(ch=='e')||(ch=='E')) {
        t.type=ALV6_VALUE;
        // read in the complete number in scientific notation
        while((!feof(alv_file)) && (isdigit(ch)||(ch=='-')||(ch=='+')||(ch=='.')||(ch=='e')||(ch=='E'))) {
            t.value+=ch;
            ch=fgetc(alv_file);
            if (ferror(alv_file)) throw alv6_exception(format("error during file access operation:\n  %s", strerror(errno)));
        }
        ungetc(ch, alv_file);
        if (ferror(alv_file)) throw alv6_exception(format("error during file access operation:\n  %s", strerror(errno)));
        t.value=t.value.trimmed();
        QLocale c(QLocale::C);
        c.setNumberOptions(QLocale::OmitGroupSeparator);
        t.doubleValue=c.toDouble(t.value);
        return t;
    }
    if (isalpha(ch)) {
        t.type=ALV6_NAME;
        while((!feof(alv_file)) && ((!readingHeader && !isspace(ch))||(readingHeader && (ch!=':') && (ch!='\n') && (ch!=255)))) {
            t.value+=ch;
            ch=fgetc(alv_file);
            if (ferror(alv_file)) throw alv6_exception(format("error during file access operation:\n  %s", strerror(errno)));
        }
        ungetc(ch, alv_file);
        if (ferror(alv_file)) throw alv6_exception(format("error during file access operation:\n  %s", strerror(errno)));
        t.value=t.value.trimmed();
        return t;
    }
    // the parser has found an unknown token. an exception will be thrown
    throw alv6_exception(format("unexcpected character: found '%c' (0x%X)", ch, (int)ch));
}


