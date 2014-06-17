#include "flex_sin_tools.h"
#include <QLocale>
#include <errno.h>
#include "qftools.h"
#include <QDebug>

/**************************************************************************************************************************
 * methods and functions, that are not part of a class
 **************************************************************************************************************************/
void FLEX_analyze(QString filename, QString& mode, unsigned int& channelCount, unsigned int& runCount, bool& crossCorrelation, bool& autoCorrelation) {
    FILE* FLEX_file=fopen(filename.toAscii().data(), "r");
    if (ferror(FLEX_file)) throw FLEX_exception(format("error while opening file '%s':\n  %s", filename.toAscii().data(), strerror(errno)));
    bool readingHeader=true;
    runCount=1;
    crossCorrelation=false;
    autoCorrelation=false;
    channelCount=1;
    bool findIdentifier=true;
    FLEX_TOKEN token=FLEX_getToken(FLEX_file);
    // first we parse the header (until we find FLXA)
    while (token.type!=FLEX_EOF && readingHeader) {
        if (findIdentifier) {
            if (token.type==FLEX_NAME) {
                if (token.value.contains("FLXA", Qt::CaseInsensitive)) {
                    findIdentifier=false;
                } else if (token.type!=FLEX_LINEBREAK && token.type!=FLEX_EOF) {
                    throw FLEX_exception("did not find file header 'FLXA'");
                }
            }
        } else {
            if (token.type==FLEX_NAME) { // here we parse "<name> = <value>"
                QString name=token.value; // store <name>
                QString value="";
                // get next token which has to be a '='
                token=FLEX_getToken(FLEX_file);
                if (token.type==FLEX_EQUALS) {
                    // get next token which has to be a value or a (quoted) string or a linebreak
                    token=FLEX_getToken(FLEX_file);
                    if (token.type==FLEX_QUOTED) {
                        value=token.value;
                    } else if (token.type==FLEX_NAME) {
                        value=token.value;
                    } else if (token.type==FLEX_VALUE) {
                        value=token.value;
                    } else if (token.type!=FLEX_LINEBREAK) {
                        throw FLEX_exception(format("linebreak, number or (quoted) string expected"));
                    }
                    // here we check whether this is an interpreted data field (i.e. it is stored in a separate field of item1
                    if (name.compare("Mode",  Qt::CaseInsensitive)==0) {
                        mode=token.value;
                        if (token.value.contains("Single Auto", Qt::CaseInsensitive)) {
                            crossCorrelation=false;
                            autoCorrelation=true;
                            channelCount=1;
                        } else if (token.value.contains("Single Cross", Qt::CaseInsensitive)) {
                            crossCorrelation=true;
                            autoCorrelation=false;
                            channelCount=2;
                        } else if (token.value.contains("Dual Auto", Qt::CaseInsensitive)) {
                            crossCorrelation=false;
                            autoCorrelation=true;
                            channelCount=2;
                        } else if (token.value.contains("Dual Cross", Qt::CaseInsensitive)) {
                            crossCorrelation=true;
                            autoCorrelation=false;
                            channelCount=2;
                        } else if (token.value.contains("Quad", Qt::CaseInsensitive)) {
                            crossCorrelation=true;
                            autoCorrelation=true;
                            channelCount=2;
                        }
                        readingHeader=false;

                    }
                }
            } else if (token.type==FLEX_QUOTED) {
                // we stop reading the header when we meet the first quoted string token
                // this is possible as every line of the header begins with an unquoted name
                // token followed by a colon and number/quoted or alineabreak (identifier line!)
                readingHeader=false;
            }
        }
        if (readingHeader) token=FLEX_getToken(FLEX_file);
    }
}


FLEX_TOKEN FLEX_readNumberMatrix(FILE* FLEX_file, QList<QVector<double> > *datm, bool getNextToken, FLEX_TOKEN lastToken) {
    //std::cout<<"<<<entered FLEX_readNumberMatrix>>>\n";

    QVector<double> data; // a single row may contain 1024 columns at the most
                       // this field is used to temporarily save the read data
    QList<QVector<double> > dat;
    // we do not already know how many rows make up the correlation data, so we first read all rows into
    // a buffer (vector dat). Then we can determine the number of data rows and get an according amount
    // of memory in item1.

    // now we read until the end of the file or the first non-number token
    // (name or quoted, whitespace/linebreaks will be swallowed)
    FLEX_TOKEN token;
    if (getNextToken) token=FLEX_getToken(FLEX_file);
    else token=lastToken;
    bool first=true; // if true: all linebreaks and other tokens will be ignored (= search for first FLEX_VALUE)
    //unsigned int count=0;
    bool end=false;
    while (!end && token.type!=FLEX_EOF && (first || (!first && token.type!=FLEX_QUOTED))) {
        if (token.type==FLEX_VALUE) {
            // just the next number
            data.push_back(token.doubleValue);
            first=false;
        } else if (token.type==FLEX_LINEBREAK && !first) {
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
        } else if (token.type==FLEX_NAME && !first) {
            if (data.size()>0) dat.push_back(data);
            data.clear();
            end=true;
        } else if (token.type==FLEX_GROUP && !first) {
            if (data.size()>0) dat.push_back(data);
            data.clear();
            end=true;
        }
        if (!end) token=FLEX_getToken(FLEX_file); // get next token from file
        //std::cout<<booltostr(first)<<"    "<<token.value.toStdString()<<std::endl;
    }
    *datm=dat;
    //std::cout<<"<<<leaving FLEX_readNumberMatrix>>>\n";
    return token;
}



QString FLEX_ReadQuoted(FILE* FLEX_file, char quotationMark) {
    unsigned char ch;
    ch=fgetc(FLEX_file);
    if (ferror(FLEX_file)) throw FLEX_exception(format("error during file access operation:\n  %s", strerror(errno)));
    while((!feof(FLEX_file)) && (ch<=32) && (ch!='\n')) {
        ch=fgetc(FLEX_file);
        if (ferror(FLEX_file)) throw FLEX_exception(format("error during file access operation:\n  %s", strerror(errno)));
		//std::cout<<"   quoted_before: '"<<ch<<"'"<<std::endl;
	}
    if (ch!=quotationMark) throw FLEX_exception(format("unexcpected character: found '%c', but expected '%c'", ch, quotationMark));
    QString data="";
    ch=0;
    while((!feof(FLEX_file)) && (ch!='\n') && (ch!=quotationMark)) {
		//std::cout<<"   quoted: '"<<ch<<"'"<<std::endl;
        ch=fgetc(FLEX_file);
        if (ferror(FLEX_file)) throw FLEX_exception(format("error during file access operation:\n  %s", strerror(errno)));
		if ((ch!='\n') && (ch!=quotationMark)) data=data+ch;
	}
	return data;
}

QDate FLEX_toDate(QString d) {
    return QDate::fromString(d, "dd-MMM-yy");
}

QTime FLEX_toTime(QString d){
    return QTime::fromString(d, "hh:mm:ss");
}

FLEX_TOKEN FLEX_getToken(FILE* FLEX_file) {
    FLEX_TOKEN t;
    t.value="";
    t.doubleValue=0;
    unsigned char ch;
    ch=fgetc(FLEX_file);
    if (ferror(FLEX_file)) throw FLEX_exception(format("error during file access operation:\n  %s", strerror(errno)));
    while((!feof(FLEX_file)) && ((ch<=32) && (ch!='\n'))) {
        ch=fgetc(FLEX_file);
        if (ferror(FLEX_file)) throw FLEX_exception(format("error during file access operation:\n  %s", strerror(errno)));
	}
    if (feof(FLEX_file)||(ch==255)) {
        t.type=FLEX_EOF;
    } else if (ch=='\n') {
        t.type=FLEX_LINEBREAK;
    } else if (ch=='=') {
        t.type=FLEX_EQUALS;
    } else if (ch=='"' || ch=='\''){
        t.type=FLEX_QUOTED;
        ungetc(ch, FLEX_file);
        if (ferror(FLEX_file)) throw FLEX_exception(format("error during file access operation:\n  %s", strerror(errno)));
        t.value=FLEX_ReadQuoted(FLEX_file, ch);
	    return t;
    } else if (isdigit(ch)||(ch=='-')||(ch=='+')||(ch=='.')||(ch=='e')||(ch=='E')) {
        t.type=FLEX_VALUE;
        // read in the complete number in scientific notation
        while((!feof(FLEX_file)) && (isdigit(ch)||(ch=='-')||(ch=='+')||(ch=='.')||(ch=='e')||(ch=='E'))) {
            t.value+=ch;
            ch=fgetc(FLEX_file);
            if (ferror(FLEX_file)) throw FLEX_exception(format("error during file access operation:\n  %s", strerror(errno)));
        }
        // find special cases -1.#IND, -1.#INF
        if (ch=='#') {
            while((!feof(FLEX_file)) && ((ch=='#')||(ch=='I')||(ch=='i')||(ch=='N')||(ch=='n')||(ch=='F')||(ch=='f')||(ch=='D')||(ch=='d'))) {
                t.value+=ch;
                ch=fgetc(FLEX_file);
                if (ferror(FLEX_file)) throw FLEX_exception(format("error during file access operation:\n  %s", strerror(errno)));
            }
        }
        if (ch=='i'||ch=='I'||ch=='n'||ch=='N') {
            while((!feof(FLEX_file)) &&((ch=='I')||(ch=='i')||(ch=='N')||(ch=='n')||(ch=='F')||(ch=='f')||(ch=='D')||(ch=='d'))) {
                t.value+=ch;
                ch=fgetc(FLEX_file);
                if (ferror(FLEX_file)) throw FLEX_exception(format("error during file access operation:\n  %s", strerror(errno)));
            }
        } else if (isalpha(ch)) {
            while((!feof(FLEX_file)) && isalpha(ch)) {
                t.value+=ch;
                ch=fgetc(FLEX_file);
                if (ferror(FLEX_file)) throw FLEX_exception(format("error during file access operation:\n  %s", strerror(errno)));
            }
        }
        ungetc(ch, FLEX_file);


        if (ferror(FLEX_file)) throw FLEX_exception(format("error during file access operation:\n  %s", strerror(errno)));
        t.value=t.value.trimmed();
        if (t.value.toLower().endsWith("inf")) {
            t.doubleValue=1.0/0.0;
        } else if (t.value.toLower().endsWith("nan")||t.value.toLower().endsWith("ind")) {
            t.doubleValue=NAN;
        } else {
            QLocale c(QLocale::C);
            c.setNumberOptions(QLocale::OmitGroupSeparator);
            bool ok=false;
            t.doubleValue=c.toDouble(t.value, &ok);
            if (!ok) {
                t.type=FLEX_VALUE;
            }
        }
    } else if (ch=='[') {
        t.type=FLEX_GROUP;
        while((!feof(FLEX_file)) && (ch!=']') && (ch!='\n') && (ch!=255)) {
            if (ch!='[' && ch!=']') t.value+=ch;
            ch=fgetc(FLEX_file);
            if (ferror(FLEX_file)) throw FLEX_exception(format("error during file access operation:\n  %s", strerror(errno)));
        }

        if (ferror(FLEX_file)) throw FLEX_exception(format("error during file access operation:\n  %s", strerror(errno)));
        t.value=t.value.trimmed();
    } else if (isalpha(ch)) {
        t.type=FLEX_NAME;
        while((!feof(FLEX_file)) && (ch!='=') && (ch!='\n') && (ch!=255)) {
            t.value+=ch;
            ch=fgetc(FLEX_file);
            if (ferror(FLEX_file)) throw FLEX_exception(format("error during file access operation:\n  %s", strerror(errno)));
        }
        ungetc(ch, FLEX_file);
        if (ferror(FLEX_file)) throw FLEX_exception(format("error during file access operation:\n  %s", strerror(errno)));
        t.value=t.value.trimmed();
    } else {
        // the parser has found an unknown token. an exception will be thrown
        throw FLEX_exception(format("unexcpected character: found '%c' (0x%X)", ch, (int)ch));
    }
    //qDebug()<<"       FLEX_getToken: "<<QChar(t.type)<<t.value<<t.doubleValue;
    return t;
}




void FLEX_readFile(QString filename, FLEX_DATA &data)
{
    data.filename=filename;
    FLEX_analyze(filename, data.mode, data.channelCount, data.runCount, data.crossCorrelation, data.autoCorrelation);


    FILE* FLEX_file=fopen(filename.toAscii().data(), "r");
    if (ferror(FLEX_file)) throw FLEX_exception(format("error while opening file '%s':\n  %s", filename.toAscii().data(), strerror(errno)));
    FLEX_TOKEN token=FLEX_getToken(FLEX_file);

    QString currentGroup="";

    while (token.type!=FLEX_EOF) {
        if (token.type==FLEX_NAME) { // here we parse "<name> = <value>"
            QString name=token.value; // store <name>
            QString value="";
            // get next token which has to be a '='
            token=FLEX_getToken(FLEX_file);
            if (token.type==FLEX_EQUALS) {
                // get next token which has to be a value or a (quoted) string or a linebreak
                token=FLEX_getToken(FLEX_file);
                if (token.type==FLEX_QUOTED || token.type==FLEX_NAME) {
                    data.properties[name]=token.value;
                } else if (token.type==FLEX_VALUE) {
                    data.properties[name]=token.doubleValue;
                } else if (token.type!=FLEX_LINEBREAK) {
                    throw FLEX_exception(format("linebreak, number or (quoted) string expected"));
                }
            }
        } else if (token.type==FLEX_GROUP) {
            currentGroup=token.value.toLower();
        } else if (token.type==FLEX_VALUE && !currentGroup.isEmpty()) {
            if (currentGroup=="correlationfunction") {
                QList<QVector<double> > cdata;
                FLEX_readNumberMatrix(FLEX_file, &cdata, false, token);
                cdata=dataRotate(cdata);
                if (cdata.size()>0) {
                    data.corr_tau=cdata.first();
                    for (int i=1; i<cdata.size(); i++) {
                        data.corrs.append(cdata[i]);
                    }
                }
                currentGroup="";
                //qDebug()<<"     correlation columns:"<< cdata.size()<<"X"<<dataGetRows(cdata);
            } else if (currentGroup=="intensityhistory") {
                QList<QVector<double> > cdata;
                FLEX_readNumberMatrix(FLEX_file, &cdata, false, token);
                cdata=dataRotate(cdata);
                if (cdata.size()>0) {
                    data.counts_time=cdata.first();
                    for (int i=1; i<cdata.size(); i++) {
                        data.counts.append(cdata[i]);
                    }
                }
                currentGroup="";
                //qDebug()<<"     intensity columns:"<< cdata.size()<<"X"<<dataGetRows(cdata);
            } else if (currentGroup=="histogram") {
                QList<QVector<double> > cdata;
                FLEX_readNumberMatrix(FLEX_file, &cdata, false, token);
                //cdata=dataRotate(cdata);
                currentGroup="";
                //qDebug()<<"     histogram columns:"<< cdata.size()<<"X"<<dataGetRows(cdata);
            }

        }
        token=FLEX_getToken(FLEX_file);
        //qDebug()<<QChar(token.type);
    }

}
