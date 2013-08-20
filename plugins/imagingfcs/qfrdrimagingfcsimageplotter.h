#ifndef QFRDRIMAGINGFCSIMAGEPLOTTER_H
#define QFRDRIMAGINGFCSIMAGEPLOTTER_H

#include <stdint.h>
#include <QWidget>

#include "qfplotter.h"

class JKQTPMathImage; //forward
class JKQTPOverlayImageEnhanced; //forward
class QFRDRImagingFCSImageParameterGroupBox; //forward
class QFRDRImagingFCSOverlayStyleCombobox; //forward
class QFRawDataRecord; //forward

class QFRDRImagingFCSImagePlotter : public QFPlotter
{
        Q_OBJECT
    public:
        explicit QFRDRImagingFCSImagePlotter(bool datastore_internal, QWidget* parent=NULL, JKQTPdatastore* datast=NULL);
        explicit QFRDRImagingFCSImagePlotter(QWidget *parent = 0);

        JKQTPMathImage* get_imagePlot() const { return plteImage; }
        JKQTPOverlayImageEnhanced* get_selectionPlot() const { return plteImageSelected; }
        JKQTPOverlayImageEnhanced* get_maskPlot() const { return plteImageExcluded; }

        void connectTo(QFRDRImagingFCSImageParameterGroupBox* paramGrp, QFRDRImagingFCSOverlayStyleCombobox* overlayCmb);
        void setSelectedData(bool* plteOverviewSelectedData,  bool* plteOverviewExcludedData);
    signals:
        void saveImageSettings();
    public slots:
        void updateImage();
        void updateImage(double* data, bool *plteOverviewSelectedData, bool *plteOverviewExcludedData, int width, int height, const QString& label, bool deleteData=false);
        void updateOverlays();
        void setDisplayOverlay(bool displayOverlay, bool displayMask);
        void setCurrent(QFRawDataRecord* current);
    protected slots:
        void moveColorbarsAuto();
        void overlayStyleChanged();
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

        QFRDRImagingFCSImageParameterGroupBox* paramGrp;
        QFRDRImagingFCSOverlayStyleCombobox* overlayCmb;

        QFRawDataRecord* current;
};

#endif // QFRDRIMAGINGFCSIMAGEPLOTTER_H
