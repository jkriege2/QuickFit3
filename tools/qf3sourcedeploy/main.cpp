#include <QCoreApplication>
#include <QRegExp>
#include <QDir>
#include <QtCore>
#include <iostream>

static QStringList qfDirListFilesRecursive_private(QDir& base, const QDir& dir, const QStringList& filters) {
    QStringList sl;

    QStringList d=dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
    for (int i=0; i<d.size(); i++) {
        QDir ddir=dir;
        ddir.cd(d[i]);
        sl<<qfDirListFilesRecursive_private(base, ddir, filters);
    }

    QStringList l;
    if (!filters.isEmpty()) l=dir.entryList(filters, QDir::Files);
    else l=dir.entryList(QDir::Files);
    if (l.size()>0) {
        for (int i=0; i<l.size(); i++) {
            sl<<QFileInfo(dir.absoluteFilePath(l[i])).canonicalFilePath();
        }
    }
    return sl;
}

QStringList qfDirListFilesRecursive(QDir& dir, const QStringList& filters) {
    QStringList sl;
    sl=qfDirListFilesRecursive_private(dir, dir, filters);
    return sl;
}

void addFilesToSet(QSet<QString>& LIB_FILES_SET, const QStringList& pro_files, QRegExp rxPat, QDir indir) {
    for (int i=0; i<pro_files.size(); i++) {
        QString profn=indir.absoluteFilePath(pro_files[i]);
        QDir prodir=QFileInfo(profn).absoluteDir();
        if (QFile::exists(profn)) {
            QFile f(profn);
            if (f.open(QFile::ReadOnly|QFile::Text)) {
                //qDebug()<<"  scanning "<<profn<<prodir;
                while (!f.atEnd()) {
                    QString line=f.readLine();
                    if (rxPat.indexIn(line)>=0) {
                        QString newfn=QFileInfo(prodir.absoluteFilePath(rxPat.cap(1))).canonicalFilePath();
                        //qDebug()<<"    found "<<rxPat.cap(1)<<newfn;
                        if (!LIB_FILES_SET.contains(newfn) && QFile::exists(newfn)) LIB_FILES_SET.insert(newfn);
                    }
                }
                f.close();
            }
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDir indir(a.arguments().value(1));
    QString pattern=a.arguments().value(2, "(\\..*\\/LIB\\/\\S+\\.\\S+)");
    QRegExp rxPat(pattern);
    QString patternXML=a.arguments().value(3, "\\<file[^\\>]*\\>(\\..*\\/LIB\\/\\S+\\.\\S+)\\<\\s*\\/file\\S*\\>");
    QRegExp rxPatXML(patternXML);
    QString patternXMLSub=a.arguments().value(4, "\\<file[^\\>]*\\>(\\S+\\.\\S+)\\<\\s*\\/file\\S*\\>");
    QRegExp rxPatXMLSub(patternXMLSub);
    QString patternSub=a.arguments().value(5, "(\\S+\\.\\S+)");
    QRegExp rxPatSub(patternSub);
    //std::cout<<"SCANNING \""<<a.arguments().value(1).toStdString()<<"\" "<<indir.absolutePath().toStdString()<<"\n\n";
    //std::cout<<pattern.toStdString()<<"\n";
    QStringList pro_filters, all_filters, xml_filter;
    pro_filters<<"*.pro"<<"*.inc"<<"*.pri";
    xml_filter<<"*.qrc";
    all_filters<<"*.*";

    //QStringList all_files=qfDirListFilesRecursive(indir, all_filters);
    QStringList pro_files=qfDirListFilesRecursive(indir, pro_filters);
    QStringList xml_files=qfDirListFilesRecursive(indir, xml_filter);



    QSet<QString> LIB_FILES_SET;
    addFilesToSet(LIB_FILES_SET, pro_files, rxPat, indir);
    addFilesToSet(LIB_FILES_SET, xml_files, rxPatXML, indir);
    //qDebug()<<"\n\nPRO:\n"<<pro_files.join("\n")<<"\n\nXML:\n"<<xml_files.join("\n");

    QStringList LIB_FILES=LIB_FILES_SET.toList();
    qSort(LIB_FILES);
    //qDebug()<<LIB_FILES.join("\n");

    for (int i=0; i<LIB_FILES.size(); i++) {
        QDir d=QFileInfo(LIB_FILES[i]).absoluteDir();
        QString fn=QFileInfo(LIB_FILES[i]).fileName();
        if (fn.toLower().endsWith(".qrc")) {
            //qDebug()<<"### SCANNING "<<d<<fn;
            addFilesToSet(LIB_FILES_SET, QStringList(fn), rxPatXMLSub, d);
        } else {
            //qDebug()<<fn;
            for (int j=0; j<pro_filters.size(); j++) {
                QRegExp rxpro(pro_filters[j], Qt::CaseInsensitive, QRegExp::Wildcard);
                if (rxpro.indexIn(fn)>=0) {
                    //qDebug()<<"### SCANNING "<<d<<fn;
                    addFilesToSet(LIB_FILES_SET, QStringList(fn), rxPatSub, d);
                }
            }
        }
    }

    LIB_FILES=LIB_FILES_SET.toList();

    for (int i=0; i<LIB_FILES.size(); i++) {
        if (LIB_FILES[i].toLower().endsWith(".png")) {
            LIB_FILES_SET.insert(LIB_FILES[i].left(LIB_FILES[i].size()-4)+".svg");
            LIB_FILES_SET.insert(LIB_FILES[i].left(LIB_FILES[i].size()-4)+".svgz");
        }
    }
    LIB_FILES=LIB_FILES_SET.toList();
    qSort(LIB_FILES);

    //qDebug()<<"\n\nLIB:\n"<<LIB_FILES;

    for (int i=0; i<LIB_FILES.size(); i++) {
        std::cout<<LIB_FILES[i].toStdString()<<"\n";
    }


    return 0;
}
