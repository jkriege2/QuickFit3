/*
    Copyright (c) 2016 Jan Buchholz (<jan.buchholz@dkfz.de>), German Cancer Research Center (DKFZ)



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

#ifndef QFIMAGEREADERCSV_H
#define QFIMAGEREADERCSV_H

#include <stdint.h>
#include <QString>
#include <QMessageBox>
#include "qfpluginimporters.h"
#include "qfimporter.h"
#include "qfimporterimageseries.h"
#include "qftools.h"
#include "csvtools.h"
#include "qfdlgcsvparameters.h"
#include "programoptions.h"

/*! \brief QFImporter class for CSV files.
    \ingroup qf3importerplugins_importers_basicimages

*/

class QFImageReaderCSV : public QFImporterImageSeries
{
public:
    inline QFImageReaderCSV() {
        this->width=0;
        this->channels=0;
        this->height=0;
        this->rawFile=NULL;
        this->frame=-1;
        this->frames=0;
        QString configprefix="QFImFCSSetParamFromFileDialog/csv/";

        QString s;
        s=ProgramOptions::getConfigValue(configprefix+"column_separator_save", ",").toString();
        this->columnSeparator=(s.size()>0)?s[0].toLatin1():',';
        s=ProgramOptions::getConfigValue(configprefix+"decimal_separator_save", ",").toString();
        this->decimalSeperator=(s.size()>0)?s[0].toLatin1():'.';
        s=ProgramOptions::getConfigValue(configprefix+"comment_start_save", ",").toString();
        this->commentStart=(s.size()>0)?s[0].toLatin1():'#';
        s=ProgramOptions::getConfigValue(configprefix+"header_start_save", ",").toString();
        this->headerStart=s;
    }

    inline virtual ~QFImageReaderCSV() {

    }

    /*! \copydoc QFFitFunction::filter()   */
    inline virtual QString filter() const  {
        return formatName()+QObject::tr(" (*.csv *.txt *.dat)");
    }
    /*! \copydoc QFFitFunction::formatName()   */
    inline virtual QString formatName() const  {
        return QObject::tr("CSV files");
    }

    bool readSettings(QString filename){
        QString configprefix="QFImFCSSetParamFromFileDialog/csv/";
        bool ok=false;
        if (QFile::exists(filename) && QFile::exists(filename+".description.txt")) {
            QSettings set(filename+".description.txt", QSettings::IniFormat);
            QString s;
            s=set.value("columnSeparator", ProgramOptions::getConfigValue(configprefix+"column_separator_save", ",").toString()).toString();
            this->columnSeparator=(s.size()>0)?s[0].toLatin1():',';
            s=set.value("decimalSeparator", ProgramOptions::getConfigValue(configprefix+"decimal_separator_save", ",").toString()).toString();
            this->decimalSeperator=(s.size()>0)?s[0].toLatin1():'.';
            s=set.value("commentStart", ProgramOptions::getConfigValue(configprefix+"comment_start_save", ",").toString()).toString();
            this->commentStart=(s.size()>0)?s[0].toLatin1():'#';
            this->headerStart=set.value("headerStart", ProgramOptions::getConfigValue(configprefix+"header_start_save", ",").toString()).toString();
            ok=true;
        }
        return ok;
    }

    void writeSettings(QString filename){
        QSettings set(filename+".description.txt", QSettings::IniFormat);
        set.setValue("columnSeparator",QString(this->columnSeparator));
        set.setValue("decimalSeparator",QString(this->decimalSeperator));
        set.setValue("commentStart",QString(this->commentStart));
        set.setValue("headerStart",QString(this->headerStart));
        set.sync();
    }

    QList<QVector<double> > openCSVAskUser(const QString& filename, const QString& configprefix, QStringList* colNames) {
        QFDlgCSVParameters* csvDlg=new QFDlgCSVParameters(NULL, QString(this->columnSeparator), QString(this->decimalSeperator), QString(this->commentStart), QString(this->headerStart));

        csvDlg->setFileContents(filename);
        QList<QVector<double> > res;
        if (csvDlg->exec()==QDialog::Accepted) {
            this->columnSeparator=csvDlg->get_column_separator();
            this->decimalSeperator=csvDlg->get_decimal_separator();
            this->commentStart=csvDlg->get_comment_start();
            this->headerStart=csvDlg->get_header_start();

            QFile file(filename);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                res=readCSV(in, this->columnSeparator, this->decimalSeperator, this->commentStart, this->headerStart, NAN, QString("\n"), QString("\r"), colNames);
                file.close();
            }
        }
        delete csvDlg;
        return res;
    }

    /** \brief open the given image sequence file
     *  \param filename name of the image sequence file
     *  \return \c true on success
     */
    inline virtual bool open(QString filename) {
        close();
        bool ok=false;
        QStringList csvCols;

        if(readSettings(filename)){
            QFile file(filename);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                data=readCSV(in, this->columnSeparator, this->decimalSeperator, this->commentStart, this->headerStart, NAN, QString("\n"), QString("\r"), &csvCols);
                file.close();
            }
        }else{
            data=importCSVAskUser(filename, "QFImFCSSetParamFromFileDialog/csv/", &csvCols);
            writeSettings(filename);
            QMessageBox::warning(NULL, QObject::tr("CSV import"),QObject::tr("Writing file specific preferences to %1.description.txt.\nDelete this file to see import dialog again.").arg(filename),
                                           QMessageBox::Ok,QMessageBox::Ok);
        }
        //QVector<double> newData;
        //newData=csv[col];
        frame=0;
        this->height=1;
        this->width=this->data.size();
        if (this->data.size()>0) {
            ok=true;
            this->frames=this->data[0].size();
        }
        return ok;
    }

    /** \brief close the currently opened image file */
    inline virtual void close() {
        frame=-1;
        frames=0;
        width=0;
        height=0;
        channels=0;
    }

    /** \brief move the reading pointer back to the first frame */
    inline virtual void reset() {
    }

    /** \brief return the number of frames in the files.*/
    inline virtual uint32_t countFrames() {
        return frames;
    }

    /** \brief move on to the next frame in the file. return \c false if no further image exists */
    inline virtual bool nextFrame() {
        frame++;
        if(frame<frames){
            return true;
        }
        frame=frames-1;
        return false;
    }

protected:
    /** \brief return the width of the frames (valid after open() returned \c true */
    inline virtual uint32_t intFrameWidth() {
        return width;
    }

    /** \brief return the height of the frames (valid after open() returned \c true */
    inline virtual uint32_t intFrameHeight() {
        return height;
    }

    inline virtual uint32_t intFrameChannels() {
        return channels;
    }

    /** \brief read a new frame into the given array of floating point numbers */
    inline virtual bool intReadFrameFloat(float* data, int channel=0) {
        if (this->frames==0) return false;
        for (uint64_t i=0; i<width*height; i++) {
            data[i]=this->data[i][frame];
        }
        return true;
    }

    /** \brief read a new frame into the given array of integers */
    inline virtual bool intReadFrameUINT16(uint16_t* data, int channel=0) {
        if (this->frames==0) return false;
        for (uint64_t i=0; i<width*height; i++) {
            data[i]=this->data[i][frame];
        }
        return true;
    }
    inline virtual bool intReadFrameDouble(double* data, int channel=0) {
        if (this->frames==0) return false;
        for (uint64_t i=0; i<width*height; i++) {
            data[i]=this->data[i][frame];
        }
        return true;
    }



    QFile* rawFile;
    QString filename;
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint32_t frames;
    uint32_t frame;
    QString type_name;
    char columnSeparator,decimalSeperator,commentStart;
    QString headerStart;
    QList<QVector<double> > data;


};

#endif // QFIMAGEREADERCSV_H
