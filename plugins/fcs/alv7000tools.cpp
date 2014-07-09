#include "alv7000tools.h"
#include <QLocale>
#include <errno.h>
#include <QDebug>


/**************************************************************************************************************************
 * methods and functions, that are not part of a class
 **************************************************************************************************************************/
void ALV7_analyze(QString filename, QString& mode, unsigned int& channelCount, unsigned int& runCount, bool& crossCorrelation, bool& autoCorrelation, int& inputchannels, int& firstchannel) {
    FILE* alv_file=fopen(filename.toAscii().data(), "r");
    if (ferror(alv_file)) throw ALV7_exception(format("error while opening file '%s':\n  %s", filename.toAscii().data(), strerror(errno)));
    bool readingHeader=true;
    //bool isCorrelation=false;
    //bool isCounRate=false;
    bool findIdentifier=true;
    ALV7_TOKEN token=ALV7_getToken(alv_file, readingHeader);
    // first we parse the header (until we find the first quoted string)
    // the data from the header is interpeted and the resulting quickfitDataStoreItem are
    // beeing created.
    while (token.type!=ALV7_EOF && readingHeader) {
        if (findIdentifier) {
            if (token.type==ALV7_NAME) {
                if (token.value.contains("ALV-7002", Qt::CaseInsensitive)) {
                    findIdentifier=false;
                    inputchannels=2;
                } else if (token.value.contains("ALV-7004", Qt::CaseInsensitive)) {
                    findIdentifier=false;
                    inputchannels=4;
                } else if (token.type!=ALV7_LINEBREAK && token.type!=ALV7_EOF) {
                    throw ALV7_exception("did not find file header 'ALV-700X'");
                }
            }
        } else {
            if (token.type==ALV7_NAME) { // here we parse "<name> : <value>"
                QString name=token.value; // store <name>
                QString value="";
                // get next token which has to be a colon':'
                token=ALV7_getToken(alv_file, readingHeader);
                //if (token.type!=ALV_COLON) throw ALV7_exception(format("colon ':' expected, but found '%s'", token.value.toAscii().data()));
                if (token.type==ALV7_COLON) {
                    // get next token which has to be a value or a quoted string or a linebreak
                    token=ALV7_getToken(alv_file, readingHeader);
                    if (token.type==ALV7_QUOTED) {
                        value=token.value;
                    } else if (token.type==ALV7_VALUE) {
                        value=token.value;
                    } else if (token.type!=ALV7_LINEBREAK) {
                        throw ALV7_exception(format("linebreak, number or quoted string expected"));
                    }
                    // here we check whether this is an interpreted data field (i.e. it is stored in a separate field of item1
                    if (name.compare("Runs",  Qt::CaseInsensitive)==0) {
                        runCount=(int)round(token.doubleValue);
                        //std::cout<<"runs found "<<item1->runs<<std::endl;
                    } else if (name.compare("Mode",  Qt::CaseInsensitive)==0) {
                        mode=token.value;
                        ALV7_analyzeMode(mode, channelCount, crossCorrelation, autoCorrelation, inputchannels, firstchannel);
                    }
                }
            } else if (token.type==ALV7_QUOTED) {
                // we stop reading the header when we meet the first quoted string token
                // this is possible as every line of the header begins with an unquoted name
                // token followed by a colon and number/quoted or alineabreak (identifier line!)
                readingHeader=false;
            }
        }
        if (readingHeader) token=ALV7_getToken(alv_file, readingHeader);
    }
}


ALV7_TOKEN ALV7_readNumberMatrix(FILE* alv_file, QVector<QVector<double> >* datm) {
    //std::cout<<"<<<entered ALV7_readNumberMatrix>>>\n";

    QVector<double> data; // a single row may contain 1024 columns at the most
                       // this field is used to temporarily save the read data
    QVector<QVector<double> > dat;
    // we do not already know how many rows make up the correlation data, so we first read all rows into
    // a buffer (vector dat). Then we can determine the number of data rows and get an according amount
    // of memory in item1.

    // now we read until the end of the file or the first non-number token
    // (name or quoted, whitespace/linebreaks will be swallowed)
    ALV7_TOKEN token=ALV7_getToken(alv_file, false);
    bool first=true; // if true: all linebreaks and other tokens will be ignored (= search for first ALV_VALUE)
    //unsigned int count=0;
    bool end=false;
    while (!end && token.type!=ALV7_EOF && (first || (!first && token.type!=ALV7_QUOTED))) {
        if (token.type==ALV7_VALUE) {
            // just the next number
            data.push_back(token.doubleValue);
            first=false;
        } else if (token.type==ALV7_LINEBREAK && !first) {
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
        if (!end) token=ALV7_getToken(alv_file, false); // get next token from file
        //std::cout<<booltostr(first)<<"    "<<token.value.toStdString()<<std::endl;
    }
    *datm=dat;
    //std::cout<<"<<<leaving ALV7_readNumberMatrix>>>\n";
    return token;
}



QString ALV7_ReadQuoted(FILE* alv_file, char quotationMark) {
    unsigned char ch;
    ch=fgetc(alv_file);
    if (ferror(alv_file)) throw ALV7_exception(format("error during file access operation:\n  %s", strerror(errno)));
    while((!feof(alv_file)) && (ch<=32) && (ch!='\n')) {
        ch=fgetc(alv_file);
        if (ferror(alv_file)) throw ALV7_exception(format("error during file access operation:\n  %s", strerror(errno)));
        //std::cout<<"   quoted_before: '"<<ch<<"'"<<std::endl;
    }
    if (ch!=quotationMark) throw ALV7_exception(format("unexcpected character: found '%c', but expected '%c'", ch, quotationMark));
    QString data="";
    ch=0;
    while((!feof(alv_file)) && (ch!='\n') && (ch!=quotationMark)) {
        //std::cout<<"   quoted: '"<<ch<<"'"<<std::endl;
        ch=fgetc(alv_file);
        if (ferror(alv_file)) throw ALV7_exception(format("error during file access operation:\n  %s", strerror(errno)));
        if ((ch!='\n') && (ch!=quotationMark)) data=data+ch;
    }
    return data;
}

QDate ALV7_toDate(QString d) {
    return QDate::fromString(d, "dd-MMM-yy");
}

QTime ALV7_toTime(QString d){
    return QTime::fromString(d, "hh:mm:ss");
}

ALV7_TOKEN ALV7_getToken(FILE* alv_file, bool readingHeader) {
    ALV7_TOKEN t;
    t.value="";
    t.doubleValue=0;
    unsigned char ch;
    ch=fgetc(alv_file);
    if (ferror(alv_file)) throw ALV7_exception(format("error during file access operation:\n  %s", strerror(errno)));
    while((!feof(alv_file)) && ((ch<=32) && (ch!='\n'))) {
        ch=fgetc(alv_file);
        if (ferror(alv_file)) throw ALV7_exception(format("error during file access operation:\n  %s", strerror(errno)));
    }
    if (feof(alv_file)||(ch==255)) {
        t.type=ALV7_EOF;
        return t;
    }
    if (ch=='\n') {
        t.type=ALV7_LINEBREAK;
        return t;
    }
    if (ch==':') {
        t.type=ALV7_COLON;
        return t;
    }
    if (ch=='"' || ch=='\''){
        t.type=ALV7_QUOTED;
        ungetc(ch, alv_file);
        if (ferror(alv_file)) throw ALV7_exception(format("error during file access operation:\n  %s", strerror(errno)));
        t.value=ALV7_ReadQuoted(alv_file, ch);
        return t;
    }
    if (isdigit(ch)||(ch=='-')||(ch=='+')||(ch=='.')||(ch=='e')||(ch=='E')) {
        t.type=ALV7_VALUE;
        // read in the complete number in scientific notation
        while((!feof(alv_file)) && (isdigit(ch)||(ch=='-')||(ch=='+')||(ch=='.')||(ch=='e')||(ch=='E'))) {
            t.value+=ch;
            ch=fgetc(alv_file);
            if (ferror(alv_file)) throw ALV7_exception(format("error during file access operation:\n  %s", strerror(errno)));
        }
        ungetc(ch, alv_file);
        if (ferror(alv_file)) throw ALV7_exception(format("error during file access operation:\n  %s", strerror(errno)));
        t.value=t.value.trimmed();
        QLocale c(QLocale::C);
        c.setNumberOptions(QLocale::OmitGroupSeparator);
        t.doubleValue=c.toDouble(t.value);
        return t;
    }
    if (isalpha(ch)) {
        t.type=ALV7_NAME;
        while((!feof(alv_file)) && ((!readingHeader && !isspace(ch))||(readingHeader && (ch!=':') && (ch!='\n') && (ch!=255)))) {
            t.value+=ch;
            ch=fgetc(alv_file);
            if (ferror(alv_file)) throw ALV7_exception(format("error during file access operation:\n  %s", strerror(errno)));
        }
        ungetc(ch, alv_file);
        if (ferror(alv_file)) throw ALV7_exception(format("error during file access operation:\n  %s", strerror(errno)));
        t.value=t.value.trimmed();
        //qDebug()<<"AVL7_NAME: "<<t.value;
        return t;
    }
    // the parser has found an unknown token. an exception will be thrown
    throw ALV7_exception(format("unexcpected character: found '%c' (0x%X)", ch, (int)ch));
}




void ALV7_analyzeMode(const QString &mode, unsigned int &channelCount, bool &crossCorrelation, bool& autoCorrelation, int &inputchannels, int& firstchannel)
{
    QString m=mode.toUpper().trimmed().simplified();
    if (m=="C-CH0/1+1/0" || m=="C-CH0/1a1/0") {
        autoCorrelation=false;
        crossCorrelation=true;
        channelCount=2;
        inputchannels=2;
        firstchannel=0;
    } else if (m=="C-CH0/1") {
        autoCorrelation=false;
        crossCorrelation=true;
        channelCount=1;
        inputchannels=2;
        firstchannel=0;
    } else if (m=="C-CH1/0") {
        autoCorrelation=false;
        crossCorrelation=true;
        channelCount=1;
        inputchannels=2;
        firstchannel=1;
    } else if (m=="C-CH2/3") {
        autoCorrelation=false;
        crossCorrelation=true;
        channelCount=1;
        inputchannels=2;
        firstchannel=2;
    } else if (m=="C-CH3/2") {
        autoCorrelation=false;
        crossCorrelation=true;
        channelCount=1;
        inputchannels=2;
        firstchannel=3;
    } else if (m=="A-CH0" || m=="A-(CH0+1)") {
        autoCorrelation=true;
        crossCorrelation=false;
        channelCount=1;
        inputchannels=1;
        firstchannel=0;
    } else if (m=="A-CH1") {
        autoCorrelation=true;
        crossCorrelation=false;
        channelCount=1;
        inputchannels=1;
        firstchannel=1;
    } else if (m=="A-CH2") {
        autoCorrelation=true;
        crossCorrelation=false;
        channelCount=1;
        inputchannels=1;
        firstchannel=2;
    } else if (m=="A-CH3") {
        autoCorrelation=true;
        crossCorrelation=false;
        channelCount=1;
        inputchannels=1;
        firstchannel=3;
    } else if (m=="A-CH0+1") {
        autoCorrelation=true;
        crossCorrelation=false;
        channelCount=2;
        inputchannels=2;
        firstchannel=0;
    } else if (m=="A-CH2+3") {
        autoCorrelation=true;
        crossCorrelation=false;
        channelCount=2;
        inputchannels=2;
        firstchannel=2;
    } else if (m=="A-CH0+1 A-CH2+3") {
        autoCorrelation=true;
        crossCorrelation=false;
        channelCount=4;
        inputchannels=4;
        firstchannel=0;
    } else if (m=="A-CH0+1 C-CH0/1+1/0" || m=="A-CH0+1 C-CH0/1a1/0") {
        crossCorrelation=true;
        autoCorrelation=true;
        channelCount=2;
        inputchannels=2;
        firstchannel=0;
    } else if (m=="A-CH0+1 C-CH2/3+3/2") {
        crossCorrelation=true;
        autoCorrelation=true;
        channelCount=2;
        inputchannels=4;
        firstchannel=0;
    } else if (m=="A-CH2+3 C-CH2/3+3/2") {
        crossCorrelation=true;
        autoCorrelation=true;
        channelCount=2;
        inputchannels=4;
        firstchannel=2;
    } else  {
        autoCorrelation=mode.toUpper().contains("A-", Qt::CaseInsensitive);
        crossCorrelation=mode.toUpper().contains("C-", Qt::CaseInsensitive);
        channelCount=1;
        inputchannels=1;
    }
}
