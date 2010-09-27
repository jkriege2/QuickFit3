#include "alv5000tools.h"
#include <QLocale>



/**************************************************************************************************************************
 * methods and functions, that are not part of a class
 **************************************************************************************************************************/
void ALV_analyze(QString filename, QString& mode, unsigned int& channelCount, unsigned int& runCount, bool& crossCorrelation) {
    FILE* alv_file=fopen(filename.toAscii().data(), "r");
    if (ferror(alv_file)) throw alv_exception(format("error while opening file '%s':\n  %s", filename.toAscii().data(), strerror(errno)));
    bool readingHeader=true;
    bool isCorrelation=false;
    bool isCounRate=false;
    bool findIdentifier=true;
    ALV_TOKEN token=ALV_getToken(alv_file, readingHeader);
    // first we parse the header (until we find the first quoted string)
    // the data from the header is interpeted and the resulting quickfitDataStoreItem are
    // beeing created.
    while (token.type!=ALV_EOF && readingHeader) {
        if (findIdentifier) {
            if (token.type==ALV_NAME) {
                if (token.value.contains("ALV-5000", Qt::CaseInsensitive)) {
                    findIdentifier=false;
                } else if (token.type!=ALV_LINEBREAK && token.type!=ALV_EOF) {
                    throw alv_exception("did not find file header 'ALV-5000'");
                }
            }
        } else {
            if (token.type==ALV_NAME) { // here we parse "<name> : <value>"
                QString name=token.value; // store <name>
                QString value="";
                // get next token which has to be a colon':'
                token=ALV_getToken(alv_file, readingHeader);
                //if (token.type!=ALV_COLON) throw alv_exception(format("colon ':' expected, but found '%s'", token.value.toAscii().data()));
                if (token.type==ALV_COLON) {
                    // get next token which has to be a value or a quoted string or a linebreak
                    token=ALV_getToken(alv_file, readingHeader);
                    if (token.type==ALV_QUOTED) {
                        value=token.value;
                    } else if (token.type==ALV_VALUE) {
                        value=token.value;
                    } else if (token.type!=ALV_LINEBREAK) {
                        throw alv_exception(format("linebreak, number or quoted string expected"));
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
            } else if (token.type==ALV_QUOTED) {
                // we stop reading the header when we meet the first quoted string token
                // this is possible as every line of the header begins with an unquoted name
                // token followed by a colon and number/quoted or alineabreak (identifier line!)
                readingHeader=false;
            }
        }
        if (readingHeader) token=ALV_getToken(alv_file, readingHeader);
    }
}


ALV_TOKEN ALV_readNumberMatrix(FILE* alv_file, std::vector<std::vector<double> >* datm) {
    //std::cout<<"<<<entered ALV_readNumberMatrix>>>\n";

    std::vector<double> data; // a single row may contain 1024 columns at the most
                       // this field is used to temporarily save the read data
    std::vector<std::vector<double> > dat;
    // we do not already know how many rows make up the correlation data, so we first read all rows into
    // a buffer (vector dat). Then we can determine the number of data rows and get an according amount
    // of memory in item1.

    // now we read until the end of the file or the first non-number token
    // (name or quoted, whitespace/linebreaks will be swallowed)
    ALV_TOKEN token=ALV_getToken(alv_file, false);
    bool first=true; // if true: all linebreaks and other tokens will be ignored (= search for first ALV_VALUE)
    unsigned int count=0;
    bool end=false;
    while (!end && token.type!=ALV_EOF && (first || (!first && token.type!=ALV_QUOTED))) {
        if (token.type==ALV_VALUE) {
            // just the next number
            data.push_back(token.doubleValue);
            first=false;
        } else if (token.type==ALV_LINEBREAK && !first) {
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
        if (!end) token=ALV_getToken(alv_file, false); // get next token from file
        //std::cout<<booltostr(first)<<"    "<<token.value.toStdString()<<std::endl;
    }
    *datm=dat;
    //std::cout<<"<<<leaving ALV_readNumberMatrix>>>\n";
    return token;
}



QString ALV_ReadQuoted(FILE* alv_file, char quotationMark) {
    unsigned char ch;
    ch=fgetc(alv_file);
    if (ferror(alv_file)) throw alv_exception(format("error during file access operation:\n  %s", strerror(errno)));
	while((!feof(alv_file)) && (ch<=32) && (ch!='\n')) {
		ch=fgetc(alv_file);
		if (ferror(alv_file)) throw alv_exception(format("error during file access operation:\n  %s", strerror(errno)));
		//std::cout<<"   quoted_before: '"<<ch<<"'"<<std::endl;
	}
	if (ch!=quotationMark) throw alv_exception(format("unexcpected character: found '%c', but expected '%c'", ch, quotationMark));
    QString data="";
    ch=0;
	while((!feof(alv_file)) && (ch!='\n') && (ch!=quotationMark)) {
		//std::cout<<"   quoted: '"<<ch<<"'"<<std::endl;
		ch=fgetc(alv_file);
		if (ferror(alv_file)) throw alv_exception(format("error during file access operation:\n  %s", strerror(errno)));
		if ((ch!='\n') && (ch!=quotationMark)) data=data+ch;
	}
	return data;
}

QDate ALV_toDate(QString d) {
    return QDate::fromString(d, "dd-MMM-yy");
}

QTime ALV_toTime(QString d){
    return QTime::fromString(d, "hh:mm:ss");
}

ALV_TOKEN ALV_getToken(FILE* alv_file, bool readingHeader) {
    ALV_TOKEN t;
    t.value="";
    t.doubleValue=0;
    unsigned char ch;
    ch=fgetc(alv_file);
    if (ferror(alv_file)) throw alv_exception(format("error during file access operation:\n  %s", strerror(errno)));
	while((!feof(alv_file)) && ((ch<=32) && (ch!='\n'))) {
		ch=fgetc(alv_file);
        if (ferror(alv_file)) throw alv_exception(format("error during file access operation:\n  %s", strerror(errno)));
	}
	if (feof(alv_file)||(ch==255)) {
	    t.type=ALV_EOF;
	    return t;
	}
	if (ch=='\n') {
	    t.type=ALV_LINEBREAK;
	    return t;
	}
	if (ch==':') {
	    t.type=ALV_COLON;
	    return t;
	}
	if (ch=='"' || ch=='\''){
	    t.type=ALV_QUOTED;
	    ungetc(ch, alv_file);
        if (ferror(alv_file)) throw alv_exception(format("error during file access operation:\n  %s", strerror(errno)));
	    t.value=ALV_ReadQuoted(alv_file, ch);
	    return t;
    }
	if (isdigit(ch)||(ch=='-')||(ch=='+')||(ch=='.')||(ch=='e')||(ch=='E')) {
        t.type=ALV_VALUE;
        // read in the complete number in scientific notation
        while((!feof(alv_file)) && (isdigit(ch)||(ch=='-')||(ch=='+')||(ch=='.')||(ch=='e')||(ch=='E'))) {
            t.value+=ch;
            ch=fgetc(alv_file);
            if (ferror(alv_file)) throw alv_exception(format("error during file access operation:\n  %s", strerror(errno)));
        }
        ungetc(ch, alv_file);
        if (ferror(alv_file)) throw alv_exception(format("error during file access operation:\n  %s", strerror(errno)));
        t.value=t.value.trimmed();
        QLocale c(QLocale::C);
        t.doubleValue=c.toDouble(t.value);
        return t;
    }
    if (isalpha(ch)) {
        t.type=ALV_NAME;
        while((!feof(alv_file)) && ((!readingHeader && !isspace(ch))||(readingHeader && (ch!=':') && (ch!='\n') && (ch!=255)))) {
            t.value+=ch;
            ch=fgetc(alv_file);
            if (ferror(alv_file)) throw alv_exception(format("error during file access operation:\n  %s", strerror(errno)));
        }
        ungetc(ch, alv_file);
        if (ferror(alv_file)) throw alv_exception(format("error during file access operation:\n  %s", strerror(errno)));
        t.value=t.value.trimmed();
        return t;
    }
	// the parser has found an unknown token. an exception will be thrown
	throw alv_exception(format("unexcpected character: found '%c' (0x%X)", ch, (int)ch));
}



/*void ALV_LoadFile(QString filename, quickfitDataStore* datastore) {
    FILE* alv_file=fopen(filename.toAscii().data(), "r");
    if (ferror(alv_file)) throw alv_exception(format("error while opening file '%s':\n  %s", filename.toAscii().data(), strerror(errno)));
    bool readingHeader=true;
    bool isCorrelation=false;
    bool isCounRate=false;
    bool findIdentifier=true;
    ALV_TOKEN token=ALV_getToken(alv_file, readingHeader);
    // first we parse the header (until we find the first quoted string)
    // the data from the header is interpeted and the resulting quickfitDataStoreItem are
    // beeing created.
    quickfitDataStoreItem* item1=new quickfitDataStoreItem;
    quickfitDataStoreItem* item2=NULL;
    item1->filename=filename;
    while (token.type!=ALV_EOF && readingHeader) {
        if (findIdentifier) {
            if (token.type==ALV_NAME) {
                if (token.value.contains("ALV-5000", Qt::CaseInsensitive)) {
                    findIdentifier=false;
                } else if (token.type!=ALV_LINEBREAK && token.type!=ALV_EOF) {
                    throw alv_exception("did not find file header 'ALV-5000'");
                }
            }
        } else {
            if (token.type==ALV_NAME) { // here we parse "<name> : <value>"
                quickfitDataStoreItemAdd item;
                item.name=token.value; // store <name>
                // get next token which has to be a colon':'
                token=ALV_getToken(alv_file, readingHeader);
                if (token.type!=ALV_COLON) throw alv_exception(format("colon ':' expected, but found '%s'", token.value.toAscii().data()));
                // get next token which has to be a value or a quoted string or a linebreak
                token=ALV_getToken(alv_file, readingHeader);
                if (token.type==ALV_QUOTED) {
                    item.value=token.value;
                } else if (token.type==ALV_VALUE) {
                    item.value=token.value;
                    item.doubleValue=token.doubleValue;
                } else if (token.type!=ALV_LINEBREAK) {
                    throw alv_exception(format("linebreak, number or quoted string expected"));
                }
                // here we check whether this is an interpreted data field (i.e. it is stored in a separate field of item1
                if (item.name.compare("Date",  Qt::CaseInsensitive)==0) {
                    item1->date=ALV_toDate(item.value);
                    if (item1->date.year()<1950) item1->date.setDate(item1->date.year()+100, item1->date.month(), item1->date.day());
                    //std::cout<<"date found '"<<item.value.toStdString()<<"'   "<<item1->date.toString().toStdString()<<std::endl;
                } else if (item.name.compare("Time",  Qt::CaseInsensitive)==0) {
                    item1->time=ALV_toTime(item.value);
                    //std::cout<<"time found '"<<item.value.toStdString()<<"'   "<<item1->time.toString().toStdString()<<std::endl;
                } else if (item.name.contains("Duration",  Qt::CaseInsensitive)) {
                    item1->duration=item.doubleValue;
                    //std::cout<<"duration found "<<item1->duration<<std::endl;
                } else if (item.name.compare("Runs",  Qt::CaseInsensitive)==0) {
                    item1->runs=(int)round(item.doubleValue);
                    //std::cout<<"runs found "<<item1->runs<<std::endl;
                } else if (item.name.compare("Mode",  Qt::CaseInsensitive)==0) {
                    item1->mode=item.value;
                    item1->isCrossCorrelation=item.value.contains("CROSS", Qt::CaseInsensitive);
                    item1->isDualChannel=item.value.contains("DUAL", Qt::CaseInsensitive);
                    if (item.value.contains("CH0", Qt::CaseInsensitive)) item1->channel="CH0";
                    else if (item.value.contains("CH1", Qt::CaseInsensitive)) item1->channel="CH1";
                    else if (item.value.contains("CH2", Qt::CaseInsensitive)) item1->channel="CH2";
                    else if (item.value.contains("CH3", Qt::CaseInsensitive)) item1->channel="CH3";

                } else {
                    item1->additionalData.push_back(item);
                }
            } else if (token.type==ALV_QUOTED) {
                // we stop reading the header when we meet the first quoted string token
                // this is possible as every line of the header begins with an unquoted name
                // token followed by a colon and number/quoted or alineabreak (identifier line!)
                readingHeader=false;
            }
        }
        if (readingHeader) token=ALV_getToken(alv_file, readingHeader);
    }
    if (item1->isDualChannel) {
        item2=new quickfitDataStoreItem(*item1);
        item1->channel="CH0";
        item2->channel="CH1";
    }
    //std::cout<<"item1="<<item1<<"   item2="<<item2<<std::endl;
    while (token.type!=ALV_EOF) {
        //std::cout<<token.value.toStdString()<<std::endl;
        bool getNew=true;
        if (token.type==ALV_QUOTED && token.value.contains("correlation", Qt::CaseInsensitive)) {
            getNew=false;
            if (!item1->isDualChannel) {
                token=ALV_readCorrelation(alv_file, item1, item1->runs);
            } else {
                token=ALV_readCorrelation(alv_file, item1, item2, item1->runs);
            }
        }
        if (token.type==ALV_QUOTED && token.value.contains("count", Qt::CaseInsensitive)) {
            getNew=false;//false;
            if (!item1->isDualChannel) {
                token=ALV_readRate(alv_file, item1);
            } else {
                token=ALV_readRate(alv_file, item1, item2);
            }
        }
        if (getNew) token=ALV_getToken(alv_file, false);
    }
    fclose(alv_file);

    datastore->add(item1);
    if (item1->isDualChannel) {
        datastore->add(item2);
    }
}*/


/*ALV_TOKEN ALV_readRate(FILE* alv_file, quickfitDataStoreItem* item1) {
    //std::cout<<"<<<entered ALV_readRate>>>\n";
    std::vector<std::vector<double> > dat;

    // read the numbers in a vector of vectors
    ALV_TOKEN token=ALV_readNumberMatrix(alv_file, &dat);

    // initialize all min/max values to 0
    item1->rateMax=0;
    item1->rateMin=0;
    item1->rateTMax=0;
    item1->rateTMin=0;

    item1->rateN=dat.size();
    if (item1->rateN>0) {
        item1->allocateRate(item1->rateN);
    }
    double mean1=0;
    // The min/max values are calculated while the data is read. If we process the first data
    // point (i==0) the min/max variables are initialized with this value. Afterwards the are
    // updated whenever the curren value is >max or <min.
    for (size_t i=0; i<item1->rateN; i++) {
        std::vector<double> d=dat[i];
        if (d.size()<2) throw alv_exception("too few columns in rate block.");
        item1->rateT[i]=d[0];
        item1->rate[i]=d[1];
        mean1+=d.at(1);

        if (i==0) {
            item1->rateMax=d[1];
            item1->rateMin=d[1];
            item1->rateTMax=d[0];
            item1->rateTMin=d[0];
        } else {
            if (d[1]>item1->rateMax) item1->rateMax=d[1];
            if (d[1]<item1->rateMin) item1->rateMin=d[1];
            if (d[0]>item1->rateTMax) item1->rateTMax=d[0];
            if (d[0]<item1->rateTMin) item1->rateTMin=d[0];
        }

    }
    item1->mean=mean1/dat.size();
    //std::cout<<"<<<entered ALV_readRate>>>\n";
    return token;
}

ALV_TOKEN ALV_readRate(FILE* alv_file, quickfitDataStoreItem* item1, quickfitDataStoreItem* item2) {
    //std::cout<<"<<<entered ALV_readRate2>>>\n";
    std::vector<std::vector<double> > dat;

    // read the numbers in a vector of vectors
    ALV_TOKEN token=ALV_readNumberMatrix(alv_file, &dat);

    // initialize all min/max values to 0
    item1->rateMax=0;
    item1->rateMin=0;
    item1->rateTMax=0;
    item1->rateTMin=0;
    item2->rateMax=0;
    item2->rateMin=0;
    item2->rateTMax=0;
    item2->rateTMin=0;

    item1->rateN=dat.size();
    item2->rateN=dat.size();
    if (item1->rateN>0) {
        item1->allocateRate(item1->rateN);
        item2->allocateRate(item1->rateN);
    }
    double mean1=0;
    double mean2=0;
    // The min/max values are calculated while the data is read. If we process the first data
    // point (i==0) the min/max variables are initialized with this value. Afterwards the are
    // updated whenever the curren value is >max or <min.
    for (size_t i=0; i<item1->rateN; i++) {
        std::vector<double> d=dat[i];
        if (d.size()<3) throw alv_exception("too few columns in rate block.");
        item1->rateT[i]=d[0];
        item1->rate[i]=d[1];
        item2->rateT[i]=d[0];
        item2->rate[i]=d[2];
        mean1+=d[1];
        mean2+=d[2];

        if (i==0) {
            item1->rateMax=d[1];
            item1->rateMin=d[1];
            item1->rateTMax=d[0];
            item1->rateTMin=d[0];
            item2->rateMax=d[2];
            item2->rateMin=d[2];
            item2->rateTMax=d[0];
            item2->rateTMin=d[0];
        } else {
            if (d[1]>item1->rateMax) item1->rateMax=d[1];
            if (d[1]<item1->rateMin) item1->rateMin=d[1];
            if (d[0]>item1->rateTMax) item1->rateTMax=d[0];
            if (d[0]<item1->rateTMin) item1->rateTMin=d[0];
            if (d[2]>item2->rateMax) item2->rateMax=d[2];
            if (d[2]<item2->rateMin) item2->rateMin=d[2];
            item2->rateTMax=item1->rateTMax;
            item2->rateTMin=item1->rateTMin;
        }

    }
    item1->mean=mean1/dat.size();
    item2->mean=mean2/dat.size();
    //std::cout<<"<<<entered ALV_readRate2>>>\n";
    return token;
}

ALV_TOKEN ALV_readCorrelation(FILE* alv_file, quickfitDataStoreItem* item1, long runs) {
    //std::cout<<"<<<entered ALV_readCorrelation>>>\n";
    std::vector<std::vector<double> > dat;

    // read the numbers in a vector of vectors
    ALV_TOKEN token=ALV_readNumberMatrix(alv_file, &dat);

    // initialize all min/max values to 0
    item1->correlationMin=0;
    item1->correlationMax=0;
    item1->correlationTMin=0;
    item1->correlationTMax=0;
    item1->correlationMeanMin=0;
    item1->correlationMeanMax=0;
    item1->indexMin=0;
    item1->indexMax=dat.size()-1;

    // now dat should contain as many items as data rows in the file, so we can copy the
    // correlation data from dat into item1:
    item1->correlationN=dat.size();
    if (item1->correlationN>0) {
        item1->allocateCorrelation(runs, item1->correlationN);
    }

    // The min/max values are calculated while the data is read. If we process the first data
    // point (i==0) the min/max variables are initialized with this value. Afterwards the are
    // updated whenever the curren value is >max or <min.
    for (size_t i=0; i<dat.size(); i++) {
        std::vector<double> d=dat[i];
        if (d.size()<1) throw alv_exception("too few columns in rate block.");
        item1->correlationT[i]=d[0]/1000.0;
        if (runs==1) {
            // one run => there is no average in the file!
            if (d.size()<2) throw alv_exception("too few columns in rate block.");
            item1->correlation[i]=d[1];
            item1->correlationMean[i]=d[1];
            if (i==0) {
                item1->correlationMin=d[1];
                item1->correlationMax=d[1];
                item1->correlationMeanMin=d[1];
                item1->correlationMeanMax=d[1];
            } else {
                if (d[1]>item1->correlationMax) item1->correlationMax=d[1];
                if (d[1]<item1->correlationMin) item1->correlationMin=d[1];
                if (d[1]>item1->correlationMeanMax) item1->correlationMeanMax=d[1];
                if (d[1]<item1->correlationMeanMin) item1->correlationMeanMin=d[1];
            }
        } else {
            // multiple runs -> average is in file -> ignore average column
            if (d.size()<(int)runs+2) throw alv_exception("too few columns in rate block.");
            item1->correlationMean[i]=d[1];
            if (i==0) {
                item1->correlationMin=d[2];
                item1->correlationMax=d[2];
                item1->correlationMeanMin=d[1];
                item1->correlationMeanMax=d[1];
            } else {
                if (d[1]>item1->correlationMeanMax) item1->correlationMeanMax=d[1];
                if (d[1]<item1->correlationMeanMin) item1->correlationMeanMin=d[1];
            }
            for (int k=0; k<runs; k++) {
                item1->correlation[i*runs+k]=d[k+2];
                if (d[k+2]>item1->correlationMax) item1->correlationMax=d[2];
                if (d[k+2]<item1->correlationMin) item1->correlationMin=d[2];
            }
        }

        if (i==0) {
            item1->correlationTMin=d[0]/1000.0;
            item1->correlationTMax=d[0]/1000.0;
        } else {
            if (d[0]/1000.0>item1->correlationTMax) item1->correlationTMax=d[0]/1000.0;
            if (d[0]/1000.0<item1->correlationTMin) item1->correlationTMin=d[0]/1000.0;
        }
    }

    // calculate variance
    for (size_t i=0; i<dat.size(); i++) {
        if (runs==1) {
            item1->correlationVar[i]=pow(0.05*(item1->correlationMeanMax-item1->correlationMeanMin), 2);
            item1->correlationStdDev[i]=fabs(0.05*(item1->correlationMeanMax-item1->correlationMeanMin));
        } else {
            double v1=0;
            for (int k=0; k<runs; k++) {
                v1+=pow(item1->correlation[i*runs+k]-item1->correlationMean[i], 2);
            }
            item1->correlationVar[i]=v1;
            item1->correlationStdDev[i]=sqrt(v1);
        }
    }



    //std::cout<<"<<<entered ALV_readCorrelation>>>\n";
    return token;
}

ALV_TOKEN ALV_readCorrelation(FILE* alv_file, quickfitDataStoreItem* item1, quickfitDataStoreItem* item2, long runs) {
    //std::cout<<"<<<entered ALV_readCorrelation2>>>\n";

    std::vector<std::vector<double> > dat;

    // read the numbers in a vector of vectors
    ALV_TOKEN token=ALV_readNumberMatrix(alv_file, &dat);

    // initialize all min/max values to 0
    item1->correlationMin=0;
    item1->correlationMax=0;
    item1->correlationMeanMin=0;
    item1->correlationMeanMax=0;
    item1->correlationTMin=0;
    item1->correlationTMax=0;
    item2->correlationMin=0;
    item2->correlationMax=0;
    item2->correlationMeanMin=0;
    item2->correlationMeanMax=0;
    item2->correlationTMin=0;
    item2->correlationTMax=0;

    // now dat should contain as many items as data rows in the file, so we can copy the
    // correlation data from dat into item1:
    item1->correlationN=dat.size();
    item2->correlationN=dat.size();
    item1->indexMin=0;
    item1->indexMax=dat.size()-1;
    item2->indexMin=0;
    item2->indexMax=dat.size()-1;
    if (item1->correlationN>0) {
        item1->allocateCorrelation(runs, item1->correlationN);
        item2->allocateCorrelation(runs, item1->correlationN);
    }
    // The min/max values are calculated while the data is read. If we process the first data
    // point (i==0) the min/max variables are initialized with this value. Afterwards the are
    // updated whenever the curren value is >max or <min.
    for (size_t i=0; i<dat.size(); i++) {
        std::vector<double> d=dat[i];
        item1->correlationT[i]=d[0]/1000.0;
        item2->correlationT[i]=d[0]/1000.0;
        if (runs==1) {
            // one run => there is no average in the file!
            if (d.size()<3) throw alv_exception("too few columns in rate block.");
            item1->correlation[i]=d[1];
            item2->correlation[i]=d[2];
            item1->correlationMean[i]=d[1];
            item2->correlationMean[i]=d[2];
            if (i==0) {
                item1->correlationMin=d[1];
                item1->correlationMax=d[1];
                item2->correlationMin=d[2];
                item2->correlationMax=d[2];
            } else {
                if (d[1]>item1->correlationMax) item1->correlationMax=d[1];
                if (d[1]<item1->correlationMin) item1->correlationMin=d[1];
                if (d[2]>item2->correlationMax) item2->correlationMax=d[2];
                if (d[2]<item2->correlationMin) item2->correlationMin=d[2];
            }
            item1->correlationMeanMin=item1->correlationMin;
            item1->correlationMeanMax=item1->correlationMax;
            item2->correlationMeanMin=item1->correlationMeanMin;
            item2->correlationMeanMax=item1->correlationMeanMax;

        } else {
            // multiple runs -> average is in file
            if (d.size()<2*(int)runs+3) throw alv_exception("too few columns in rate block.");
            item1->correlationMean[i]=d[1];
            item2->correlationMean[i]=d[runs+2];
            if (i==0) {
                item1->correlationMin=d[2];
                item1->correlationMax=d[2];
                item2->correlationMin=d[runs+2];
                item2->correlationMax=d[runs+2];
                item1->correlationMeanMin=d[1];
                item1->correlationMeanMax=d[1];
                item2->correlationMeanMin=d[runs+2];
                item2->correlationMeanMax=d[runs+2];
            } else {
                if (d[1]>item1->correlationMeanMax) item1->correlationMeanMax=d[1];
                if (d[1]<item1->correlationMeanMin) item1->correlationMeanMin=d[1];
                if (d[runs+2]>item2->correlationMeanMax) item2->correlationMeanMax=d[runs+2];
                if (d[runs+2]<item2->correlationMeanMin) item2->correlationMeanMin=d[runs+2];
            }
            for (int k=0; k<runs; k++) {
                item1->correlation[i*runs+k]=d[k+2];
                item2->correlation[i*runs+k]=d[runs+2+k+1];
                if (d[k+2]>item1->correlationMax) item1->correlationMax=d[2];
                if (d[k+2]<item1->correlationMin) item1->correlationMin=d[2];
                if (d[runs+2+k+1]>item2->correlationMax) item2->correlationMax=d[runs+2+k+1];
                if (d[runs+2+k+1]<item2->correlationMin) item2->correlationMin=d[runs+2+k+1];
            }
        }


        if (i==0) {
            item1->correlationTMin=d[0]/1000.0;
            item1->correlationTMax=d[0]/1000.0;
        } else {
            if (d[0]/1000.0>item1->correlationTMax) item1->correlationTMax=d[0]/1000.0;
            if (d[0]/1000.0<item1->correlationTMin) item1->correlationTMin=d[0]/1000.0;
        }
        item2->correlationTMax=item1->correlationTMax;
        item2->correlationTMin=item1->correlationTMin;
    }


    // calculate variance
    for (int i=0; i<(int)dat.size(); i++) {
        if (runs==1) {
            item1->correlationVar[i]=pow(0.05*(item1->correlationMeanMax-item1->correlationMeanMin), 2);
            item1->correlationStdDev[i]=fabs(0.05*(item1->correlationMeanMax-item1->correlationMeanMin));
            item2->correlationVar[i]=pow(0.05*(item2->correlationMeanMax-item2->correlationMeanMin), 2);
            item2->correlationStdDev[i]=fabs(0.05*(item2->correlationMeanMax-item2->correlationMeanMin));
        } else {
            double v1=0;
            double v2=0;
            for (int k=0; k<runs; k++) {
                v1+=pow(item1->correlation[i*runs+k]-item1->correlationMean[i], 2);
                v2+=pow(item2->correlation[i*runs+k]-item2->correlationMean[i], 2);
            }
            item1->correlationVar[i]=v1;
            item1->correlationStdDev[i]=sqrt(v1);
            item2->correlationVar[i]=v2;
            item2->correlationStdDev[i]=sqrt(v2);
        }
    }


    //std::cout<<"<<<entered ALV_readCorrelation2>>>\n";
    return token;
}*/
