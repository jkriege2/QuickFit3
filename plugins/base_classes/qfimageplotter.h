/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

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



#ifndef QFIMAGEPLOTTER_H
#define QFIMAGEPLOTTER_H

#include <stdint.h>
#include <QWidget>

#include "qfplotter.h"

class JKQTPMathImage; //forward
class JKQTPOverlayImageEnhanced; //forward
class QFImageParameterGroupBox; //forward
class QFOverlayStyleCombobox; //forward
class QFRawDataRecord; //forward

class QFImagePlotter : public QFPlotter
{
        Q_OBJECT
    public:
        explicit QFImagePlotter(bool datastore_internal, QWidget* parent=NULL, JKQTPdatastore* datast=NULL);
        explicit QFImagePlotter(QWidget *parent = 0);

        ~QFImagePlotter();

        JKQTPMathImage* get_imagePlot() const { return plteImage; }
        JKQTPOverlayImageEnhanced* get_selectionPlot() const { return plteImageSelected; }
        JKQTPOverlayImageEnhanced* get_maskPlot() const { return plteImageExcluded; }

        double* getData() const;
        void getDataAtBR(int idx, double& value);
        double getDataAt(int idx, double defaultValue=0);
        int32_t getDataSize() const;
        bool* getExcluded() const;
        bool* getSelected() const;

        void connectTo(QFImageParameterGroupBox* paramGrp, QFOverlayStyleCombobox* overlayCmb);
        void setSelectedData(bool* plteOverviewSelectedData,  bool* plteOverviewExcludedData);
    signals:
        void saveImageSettings();
    public slots:
        void updatePlot();
        void updateImage(double* data, bool *plteOverviewSelectedData, bool *plteOverviewExcludedData, int width, int height, const QString& label, bool deleteData=false, bool clearDatastore=false);
        void updateOverlays(double* avgOut=NULL, double* sdOut=NULL);
        void updateOverlays(bool *plteOverviewSelectedData, bool *plteOverviewExcludedData, double* avgOut=NULL, double* sdOut=NULL);
        void setDisplayOverlay(bool displayOverlay, double* avgOut=NULL, double* sdOut=NULL);
        void setCurrent(QFRawDataRecord* current);
        void clearImage();
        void setAspectRatio(bool keepAspect);
    protected slots:
        void updateImage();
        void moveColorbarsAuto();
        void overlayStyleChanged();
        void setCopyableData();
    protected:
        void initImFCSPlotter();

        /** \brief plot for the parameter image in pltImage */
        JKQTPMathImage* plteImage;
        double* plteImageData;
        int32_t plteImageSize;
        int plteImageWidth;
        int plteImageHeight;
        QString plteImageLabel;
        /** \brief data in plteOverviewSelected */
        bool* plteOverviewSelectedData;
        /** \brief data in plteOverviewExcluded */
        bool* plteOverviewExcludedData;

        /** \brief plot for the selected runs in pltImage, plot plteImageSelectedData */
        JKQTPOverlayImageEnhanced* plteImageSelected;
        /** \brief plot for the excluded runs in pltImage, plot plteImageSelectedData */
        JKQTPOverlayImageEnhanced* plteImageExcluded;

        bool displayOverlay;
        bool displayMask;
        bool keepAspect;

        QFImageParameterGroupBox* paramGrp;
        QFOverlayStyleCombobox* overlayCmb;

        QFRawDataRecord* current;
};


#endif // QFIMAGEPLOTTER_H
