/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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



#ifndef QFCORRELATIONMASKTOOLS_H
#define QFCORRELATIONMASKTOOLS_H

#include <QObject>
#include <QMenu>
#include <QAction>
#include <QSet>
#include <QList>
#include "qfrawdatarecord.h"
#include "qfrdrrunselection.h"
#include "qfrdrfcsdatainterface.h"
#include "qfrdrimagemask.h"
#include "qffinderroneouscorrelationdlg.h"
#include "qfrdrimagemaskedittools.h"

class QFCorrelationMaskTools : public QFRDRImageMaskEditTools
{
        Q_OBJECT
    public:
        explicit QFCorrelationMaskTools(QWidget *parentWidget = 0, const QString& settingsPrefix="");
        
        void registerCorrelationToolsToMenu(QMenu* menu) const;
        QAction* get_actFindErroneousPixels() const { return actFindErroneousPixels; }

        virtual void setRDR(QFRawDataRecord* rdr);

    public slots:
        /** \brief mask ACF runs according to statistics (find erroneous pixels) */
        void findErroneousPixels();
        void maskAllZeroCorrelations();
    protected:
        QAction* actFindErroneousPixels;
        QAction* actMaskAllZero;
        QFRDRFCSDataInterface* fcs;

        
};

#endif // QFCORRELATIONMASKTOOLS_H
