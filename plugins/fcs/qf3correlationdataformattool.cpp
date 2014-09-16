#include "qf3correlationdataformattool.h"

QF3CorrelationDataFormatTool::QF3CorrelationDataFormatTool()
{
}

bool QF3CorrelationDataFormatTool::loadFile(const QString &filename, bool propertiesOnly)
{
    clear();
    this->filename=filename;
    ok=true;

    QFile f(filename);
    if (f.open(QFile::ReadOnly|QFile::Text)) {
        int section=-1;
        QRegExp rxHeader("\\s*\\[\\(w+)\\]\\s*");
        QRegExp rxRole("role(\\d+)");
        QRegExp rxCHName("(channel_name|channelname)(\\d+)");
        rxCHName.setCaseSensitivity(Qt::CaseInsensitive);
        QTextCodec* codec=QTextCodec::codecForCStrings();
        while (!f.atEnd()) {
            QString line;
            if (codec) {
                line=codec->toUnicode(f.readLine()).trimmed();
            } else {
                QByteArray l=f.readLine();
                line=QString::fromLatin1(l.data(), l.size()).trimmed();
            }

            // first we search for a comment character, which has not been escaped with \#
            // if we are in section==0 (properties), the # is also ignored, if it is found within " ... " (which may also be escaped by \")
            int comment=-1;
            isQuot=false;
            for (int i=0; i<line.size(); i++) {
                if (section==0) {
                    if (line[i]=='"' && (i<=0 || !isQuot || line[i-1]!='\\')) {
                        isQuot=!isQuot;
                    }
                } else {
                    isQuot=false;
                }
                if (line[i]=='#' && !isQuot && (i<=0 || line[i-1]!='\\')) {
                    comment=i;
                    break;
                }
            }
            // only use the part in front of the comment character
            if (comment>=0) {
                line=line.left(comment).trimmed();
            }
            if (line.size()>0) { // we only have to process non-empty lines
                if (rxHeader.exactMatch(line)) { // first we check, whether the line starts a new section ... and recognize the section
                    QString sec=rxHeader.cap(1).trimmed().toLower();
                    if (sec=="properties") {
                        section=0;
                    } else if (sec=="correlationdata" || sec=="correlation") {
                        section=1;
                    } else if (sec=="ratedata" || sec=="countratedata" || sec=="rates" || sec=="countrates") {
                        section=2;
                    }
                } else {
                    if (section==0) { // properties section
                        QString name, value;
                        bool isName=true;
                        for (in i=0; i<line.size(); i++) {
                            if (isName) {
                                if (line[i]!='=') {
                                    name+=line[i];
                                } else {
                                    isName=false;
                                }
                            } else {
                                value+=line[i];
                            }
                        }
                        name=name.trimmed();
                        value=value.trimmed();
                        if (value.startsWith('"') && value.endsWith('"')) {
                            value=value.mid(1,value.simplified()-2);
                            QString val;
                            bool skip=false;
                            for (int i=0; i<value.size(); i++) {
                                if (!skip) {
                                    if (value[i]=='\\' && i+1<value.size()) {
                                        if (value[i+1]=='"') { val+='"'; skip=true; }
                                        else if (value[i+1]=='\\') { val+='\\'; skip=true; }
                                        else if (value[i+1]=='n') { val+='\n'; skip=true; }
                                        else if (value[i+1]=='r') { val+='\r'; skip=true; }
                                        else if (value[i+1]=='t') { val+='\t'; skip=true; }
                                        else if (value[i+1]=='\'') { val+='\''; skip=true; }
                                        else {
                                            val+=value[i];
                                        }
                                    } else {
                                        val+=value[i];
                                    }
                                } else {
                                    skip=false;
                                }
                            }
                            value=val;
                        }
                        value=value.trimmed();
                        const QString nameL=name.toLower();
                        if (nameL=="codec") {
                            QTextCodec cc=QTextCodec::codecForName(value.toLatin1());
                            if (cc) {
                                codec=cc;
                            }
                        } else if (nameL=="group") {
                            group=value;
                        } else if (nameL=="folder") {
                            folder=value;
                        } else if (nameL=="role") {
                            role=value;
                        } else if (nameL=="channel_name" || nameL=="channelname") {
                            channel_name=value;
                        } else if (nameL=="input_file" || nameL=="inputfile") {
                            input_file=value;
                        } else if (nameL=="runs") {
                            runs=value.toInt();
                        } else if (nameL=="channels") {
                            channels=value.toInt();
                        } else if (nameL=="correlations") {
                            correlations=value.toInt();
                        } else if (rxRole.exactMatch(nameL)) {
                            int idx=rxRole.cap(1).toInt();
                            while (idx>=0 && roles.size()<=idx){
                                roles.append("");
                            }
                            roles[idx]=value;
                        } else if (rxCHName.exactMatch(nameL)) {
                            int idx=rxCHName.cap(1).toInt();
                            while (idx>=0 && channel_names.size()<=idx){
                                channel_names.append("");
                            }
                            channel_names[idx]=value;
                        } else {
                            properties[name]=value;
                        }
                    } else if (!propertiesOnly && section==1) { // correlation data section
                        if (correlations.size()<=0) reserveCorrelations(); // if data arrays have to yet been reserved, do so now
                    } else if (!propertiesOnly && section==2) { // count-rates section
                        if (countrates.size()<=0) reserveRate(); // if data arrays have to yet been reserved, do so now
                    }
                }
            }
        }
        if (!propertiesOnly) {

        }
        f.close();
    } else {
        setError(QObject::tr("Could not open file '%1', reason: %2").arg(filename).arg(f.errorString()));
        ok=false;
    }
    return ok;
}

void QF3CorrelationDataFormatTool::reserveCorrelations()
{
    correlations.clear();
    QList<QVector<double> > runlst;
    QVector<double> d;
    for (int r=0; r<runs; r++) {
        runlst.append(d);
    }
    for (int c=0; c<correlationTypes; c++) {
        correlations.append(runlst);
    }
}

void QF3CorrelationDataFormatTool::reserveRate()
{
    countrates.clear();
    QList<QVector<double> > runlst;
    QVector<double> d;
    for (int r=0; r<runs; r++) {
        runlst.append(d);
    }
    for (int c=0; c<channels; c++) {
        countrates.append(runlst);
    }
}

void QF3CorrelationDataFormatTool::clear()
{
    filename="";
    ok=false;
    correlationTypes=1;
    channels=1;
    runs=0;
    roles.clear();
    group="";
    folder="";
    input_file="";
    correlations.clear();
    taus.clear();
    times.clear();
    countrates.clear();
    role="";
    channel_name="";
    channel_names.clear();
    properties.clear();
}
