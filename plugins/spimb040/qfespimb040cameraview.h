/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#ifndef QFESPIMB040CAMERAVIEW_H
#define QFESPIMB040CAMERAVIEW_H

#include <QWidget>
#include "qvisiblehandlesplitter.h"
#include <QSplitter>
#include <QWidget>
#include <QComboBox>
#include <QSettings>
#include <QMenu>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <cmath>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>
#include <QTime>
#include <QTabWidget>
#include <QFormLayout>
#include <QGroupBox>
#include <QElapsedTimer>
#include "programoptions.h"
#include "jkimage.h"
#include "jkqtfastplotter.h"
#include "jkqttools.h"
#include "colorcombobox.h"
#include "cpptools.h"
#include "qfextensionmanager.h"
#include "qfcameracombobox.h"
#include "qfcameraconfigcombobox.h"
#include "qfespimb00histogrammaskdialog.h"
#include "qfasttablelabel.h"
#include "qfrecentfilesmenu.h"
#include "qftools.h"
#include "qfmanyfilessettings.h"
#include "qfespimb040acquisitiontools.h"
#include "libtiff_tools.h"
class QFESPIMB040OpticsSetupBase; // forward


/** \brief defines the type of the internal image representation of QFESPIMB040CameraView::image, may be uint32_t OR double OR float (nothing else!!!)  */
#define QFESPIMB040CameraView_internalImageType int64_t
//uint32_t

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg) camera view widget
    \ingroup qf3ext_spimb040

    This widget displays a single image + some image statistics (histograms ...). To display a new image, call
    displayImage(). This triggers the display of a new image.

    This class may perform fits to the marginal curves. These functions are availabl:
      - gaussian: \f[ f(x)=F_0+A\cdot\exp\left(-\frac{(x-x_0)^2}{\sigma^2}\right) \f]
      - slit: \f[ f(x)=F0+A\cdot\left[\frac{\sin(\pi\cdot(x-x_0)/d)}{\pi\cdot(x-x_0)/d}\right]^2 \f]
      - sigmoidal: \f[ f(x)=F0+A\cdot\left[\frac{1}{1+\exp\left(-\frac{x-x_0}{d}\right)}\right] \f]
    .
 */
class QFESPIMB040CameraView : public QWidget {
        Q_OBJECT
    public:
        /*! \brief class contructor

            \param cameraID ID of the camera to use
            \param stopresume (if supplied) allows to stop/resume a preview in order to save an image
            \param parent the parent widget
         */
        QFESPIMB040CameraView(QWidget* parent=NULL, int cameraID=0, QFCameraConfigComboBoxStartResume* stopresume=NULL, QFESPIMB040OpticsSetupBase* opticsSetup=NULL);
        virtual ~QFESPIMB040CameraView();

        void init(int cameraID, QFCameraConfigComboBoxStartResume* stopresume, QFESPIMB040OpticsSetupBase* opticsSetup);
        /** \brief load settings */
        void loadSettings(QSettings&, QString prefix);
        void loadSettings(QFManyFilesSettings& settings, QString prefix);
        /** \brief save settings */
        void storeSettings(QSettings&, QString prefix);
        void storeSettings(QFManyFilesSettings& settings, QString prefix);

        /** \brief set size of pixels */
        void setPixelSize(double pixelWidth, double pixelHeight);

    protected:
        void saveMultiSeries(int frames, const QString filename);

    public slots:
        /** \brief display a new image in the widget, this does not gurantee an update of the statistics

            \param imageInput points to the data of the image to be displayed This points to a 1D array with the size \c width*height*sizeof(unit32_t)
            \param width width of \a image
            \param height height of \a image
            \param timeindex time index of image \a image in seconds
            \param exposuretime exposure time of the image in seconds (used to calculate count rates in units of kHz).

            \note This does NOT guarantee that the statistics are calculated for every frame (to increase frame rate!). If you need
                  the statistics, call displayImageComplete() instead.
        */
        void displayImage(JKImage<uint32_t>& imageInput, double timeindex, double exposuretime, QMap<QString, QVariant> parameters);
        /** \brief display a new image in the widget, in comparison to displayImage() this guarantees an update of the statistics!

            \param imageInput points to the data of the image to be displayed This points to a 1D array with the size \c width*height*sizeof(unit32_t)
            \param width width of \a image
            \param height height of \a image
            \param timeindex time index of image \a image in seconds
            \param exposuretime exposure time of the image in seconds (used to calculate count rates in units of kHz).
        */
        void displayImageComplete(JKImage<uint32_t>& imageInput, double timeindex, double exposuretime, QMap<QString, QVariant> parameters);
        /** \brief clear the display */
        void clearImage();
        /*! \brief display the current camera data/config
            \param camera name of the current camera
            \param framerate current framerate in frames per second
         */
        void displayCameraConfig(QString camera, double framerate);

        /** \brief add a QAction to the main toolbar */
        void addUserAction(QAction* action);
        /** \brief delete a QAction from the main toolbar */
        void deleteUserAction(QAction* action);
    protected slots:
        /** \brief save a report of the evaluation results */
        void saveReport();
        /** \brief save the marginals and histogram data */
        void saveData();
        /** \brief print a report of the evaluation results */
        void printReport();

        void setTableFontsize(int size);
    protected:

        /*! \brief create an evaluation report for the current record */
        void createReportDoc(QTextDocument* document);
        /** \brief create main widgets */
        void createMainWidgets();
        /** \brief create actions and register them to toolbar */
        void createActions();

        void hideEvent(QHideEvent * event);

        void storeCameraConfig(QSettings& setting);


        QVisibleHandleSplitter* splitHor;
        QVisibleHandleSplitter* splitVert;
        QToolBar* toolbar;
        QTabWidget* tabSettings;
        QTabWidget* tabResults;

        QFESPIMB040OpticsSetupBase* opticsSetup;
        QFCameraConfigComboBoxStartResume* m_stopresume;
        int cameraID;
        QMap<QString, QVariant> imageParameters;




        /** \brief main image plotter widget */
        JKQTFastPlotter* pltMain;
        /** \brief label for current position and value below it on the image plot */
        QLabel* labCurrentPos;
        /** \brief plot used to display the current frame */
        JKQTFPimagePlot* plteFrame;
        /** \brief plot used to display an overlay with the broken pixels saved in mask */
        JKQTFPimageOverlayPlot* plteMask;
        /** \brief overlay grid for image plot (image is drawn over the grid of the plotter class) */
        JKQTFPQOverlayLinearGridPlot* plteGrid;
        /** \brief plot used to display the position where the marginal plots are taken */
        JKQTFPVCrossPlot* plteMarginalPos;
        /** \brief plot used to display the range in the histogram */
        JKQTFPXRangePlot* plteHistogramRange;
        /** \brief plot used to display the histogram */
        JKQTFPVBarPlot* plteHistogram;

        /** \brief distance measure plot */
        JKQTFPLinePlot* plteMainDistance;
        /** \brief bottom marginal plotter widget */
        JKQTFastPlotter* pltMarginalBottom;
        /** \brief bottom marginal plot */
        JKQTFPLinePlot* plteMarginalBottom;
        /** \brief bottom marginal plot x data */
        double* pltDataMarginalBottomX;
        /** \brief position where to take marginal plot (if single line is selected) */
        uint32_t pltDataMarginalXPixel;
        double pltDataMarginalXPixelF;
        /** \brief bottom marginal plot y data */
        double* pltDataMarginalBottomY;
        /** \brief bottom marginal plot number of points */
        uint32_t pltDataMarginalBottomN;
        /** \brief left marginal plotter widget */
        JKQTFastPlotter* pltMarginalLeft;
        /** \brief left marginal plot */
        JKQTFPLinePlot* plteMarginalLeft;
        /** \brief bottom marginal plot x data */
        double* pltDataMarginalLeftX;
        /** \brief bottom marginal plot y data */
        double* pltDataMarginalLeftY;
        /** \brief bottom marginal plot number of points */
        uint32_t pltDataMarginalLeftN;
        /** \brief position where to take marginal plot (if single line is selected) */
        uint32_t pltDataMarginalYPixel;
        double pltDataMarginalYPixelF;
        /** \brief combobox to select the type of marginal plot */
        QComboBox* cmbMarginalPlots;
        /** \brief bottom marginal fit number of points */
        uint32_t pltDataMarginalBottomFitN;
        /** \brief bottom marginal fit number of points */
        uint32_t pltDataMarginalLeftFitN;

        double pltDataMarginalLeftYMin;
        double pltDataMarginalLeftYMax;
        double pltDataMarginalBottomYMin;
        double pltDataMarginalBottomYMax;

        double MarginalLeftWidth;
        double MarginalTopWidth;
        double MarginalLeftPosition;
        double MarginalTopPosition;



        /** \brief main image plotter widget */
        JKQTFastPlotter* pltCountsHistogram;
        /** \brief spin edit for lower count range bound */
        QDoubleSpinBox* spinCountsLower;
        /** \brief spin edit for upper count range bound */
        QDoubleSpinBox* spinCountsUpper;
        /** \brief check box to use automatic count range determination */
        QCheckBox* chkCountsRangeAutoLow;
        /** \brief check box to use automatic count range determination */
        QCheckBox* chkCountsRangeAutoHigh;
        /** \brief label for the image statistics output */
        QFastTableLabel* labImageStatistics;
        //QLabel* labImageStatistics;
        QString labImageStatisticsText;
        /** \brief spin edit for number of histogram bins */
        QSpinBox* spinHistogramBins;
        /** \brief check box to use automatic histogram bin determination */
        QCheckBox* chkHistogramBinsAuto;
        /** \brief check box to use logarithmic histogram y-scale */
        QCheckBox* chkHistogramLog;
        /** \brief check box to autimatically find defective pixels */
        QCheckBox* chkFindDefectivePixels;

        /** \brief checkbox that alows to switch a grid over the image on and off */
        QCheckBox* chkGrid;
        /** \brief spinbox that allows to set the spacing of the grid */
        QSpinBox* spinGridWidth;
        /** \brief color combobox for the overlay color */
        ColorComboBox* cmbGridColor;
        /** \brief combobox for special image transfromation modes */
        QComboBox* cmbImageTransformMode;

        /** \brief combobox to select fit function */
        QComboBox* cmbMarginalFitFunction;
        /** \brief label for marginal fit results */
        QFastTableLabel* labMarginalFitResults;
        /** \brief left marginal fit plot */
        JKQTFPLinePlot* plteMarginalFitLeft;
        /** \brief bottom marginal fit plot */
        JKQTFPLinePlot* plteMarginalFitBottom;
        /** \brief bottom marginal fit plot y data */
        double* pltDataMarginalFitBottomY;
        /** \brief left marginal fit plot y data */
        double* pltDataMarginalFitLeftY;
        /** \brief bottom marginal fit plot x data */
        double* pltDataMarginalFitBottomX;
        /** \brief left marginal fit plot x data */
        double* pltDataMarginalFitLeftX;
        /** \brief marginal fit results as string */
        QString marginalResults;
        /** \brief marginal fit results as string for QFastTableLabel */
        QString marginalResultsSimple;


        /** \brief combobox to select color bar */
        QComboBox* cmbColorscale;
        /** \brief color combobox for the overlay color */
        ColorComboBox* cmbMaskColor;
        /** \brief combobox for rotation of the image */
        QComboBox* cmbRotation;






        /** \brief combobox for special image transfromation modes */
        QComboBox* cmbAlign1ImageTransformMode;
        /** \brief combobox to select color bar */
        QComboBox* cmbAlign1Colorscale;
        /** \brief combobox to select fit function */
        QComboBox* cmbAlign1MarginalFitFunction;


        /** \brief combobox for special image transfromation modes */
        QComboBox* cmbAlign2ImageTransformMode;
        /** \brief combobox to select color bar */
        QComboBox* cmbAlign2Colorscale;
        /** \brief combobox to select fit function */
        QComboBox* cmbAlign2MarginalFitFunction;








        /** \brief label for image sie and framerate */
        QLabel* labVideoSettings;

        /** \brief checkbox to switch image statistics with histogram on/off */
        QCheckBox* chkImageStatisticsHistogram;

        /** \brief splitter on graph pane */
        QVisibleHandleSplitter* gsplitter;
        QLabel* labGraphMean;
        /** \brief plotter for the graph display */
        JKQTFastPlotter* pltGraph;
        /** \brief line plot for the graph display */
        JKQTFPLinePlot* plteGraph;
        JKQTFPYRangePlot* plteGraphRange;
        /** \brief datavectors for the graph display */
        QVector<double> plteGraphDataX;
        QVector<double> plteGraphDataY;
        /** \brief checkbox to switch graph display on/off */
        QCheckBox* chkGraph;
        /** \brief combobox that allows to select which parameter should be graphed */
        QComboBox* cmbGraphParameter;
        QTime graphTime;
        QPushButton* btnClearGraph;
        QDoubleSpinBox* spinGraphWindow;

        QSpinBox* spinTableFontsize;




        /** \brief action to save the current raw image */
        QAction* actSaveRaw;
        /** \brief action to save the current raw image */
        QAction* actSaveMulti;
        /** \brief action to save a series of image */
        QAction* actSaveMultiSeries;
        QSpinBox* spinSaveSeriesFrames;
        /** \brief action to save the current transformed image */
        QAction* actSaveTransformed;
        /** \brief action to activate/disactivate mask editing mode */
        QAction* actMaskEdit;
        /** \brief recent mask files menu */
        QFRecentFilesMenu* recentMaskFiles;
        /** \brief create a mask based on the current histogram */
        QAction* actMaskHisto;
        /** \brief action to clear mask */
        QAction* actMaskClear;
        /** \brief action to save mask */
        QAction* actMaskSave;
        /** \brief action to load mask */
        QAction* actMaskLoad;

        /** \brief action to activate/disactivate measuring mode */
        QAction* actMeasure;
        /** \brief action to activate/disactivate normal cursor mode mode */
        QAction* actCursor;

        /** \brief action to print a report */
        QAction* actPrintReport;

        QActionGroup* actgMouse;

        /** \brief action to save a report */
        QAction* actSaveReport;
        /** \brief action to save marginal+histogram data */
        QAction* actSaveData;

        QAction* actNormalMode;
        QAction* actAlign1Mode;
        QAction* actAlign2Mode;
        QActionGroup* actgAlignMode;

        //bool measureFirst;


        /*! \brief image data

            The image in this data member is drawn by redrawFrame(). The function prepareImage() copies the contents of
            \a rawImage (which represents the raw data read from the camera) into \a image and may possibly apply some
            filtering or data transformations!
         */
        JKImage<QFESPIMB040CameraView_internalImageType> image;
        /*! \brief the raw image data

            The image in this data member is NOT directly drawn by redrawFrame(). The function prepareImage() copies the contents of
            \a rawImage (which represents the raw data read from the camera) into \a image and may possibly apply some
            filtering or data transformations!
        */
        JKImage<uint32_t> rawImage;
        /*! \brief the mask image

            The pixels set to \c true in this image (which has to have the same size, as image and rawImage are used to mask
            broken pixels etc. when calculating image statistics, color scaling ... Note that when a call to prepareImage()
            implies a different image size, this image will be set to all \c false!
        */
        JKImage<bool> mask;

        bool maskEmpty;

        /** \brief indicates whether the statistics of the raw image have already been calculated */
        bool imageStatisticsCalculated;
        /** \brief image statistics: number of broken pixels */
        uint32_t imageBrokenPixels;
        /** \brief image statistics: sum of pixel values */
        double imageSum;
        /** \brief image statistics: average pixel value */
        double imageMean;
        /** \brief image statistics: standard deviation of pixel values */
        double imageStddev;
        /** \brief image statistics: minimum pixel value */
        QFESPIMB040CameraView_internalImageType imageImin;
        /** \brief image statistics: maximum pixel value */
        QFESPIMB040CameraView_internalImageType imageImax;
        /** \brief time index of the last frame in seconds */
        double imageTimeindex;
        /** \brief exposure time of the last frame in seconds */
        double imageExposureTime;


        /** \brief histogram x-values */
        double* histogram_x;
        /** \brief histogram y-values */
        double* histogram_y;
        /** \brief number of entries in the histogram */
        uint32_t histogram_n;
        /** \brief when was the count rate histogram updated the last time */
        QElapsedTimer histogramUpdateTime;
        QElapsedTimer labelUpdateTime;

        /** \brief path last used to save/load masks (*.msk) */
        QString lastMaskpath;
        /** \brief path last used to save images */
        QString lastImagepath;
        /** \brief filter last used to save images */
        QString lastImagefilter;

        int lastMaskHistogramMode;
        int lastMaskHistogramPixels;

        /** \brief flag to indicate that we are currently redrawing */
        bool currentlyRedrawing;

        /** \brief width of a pixel in microns */
        double pixelWidth;
        /** \brief height of a pixel in microns */
        double pixelHeight;


        double measureX[2];
        double measureY[2];

        /** \brief image correlation coefficient */
        double correlationCoefficient;
        double correlationCoefficientCenter;

        int cmbImageTransformModeCurrentIndex() const;
        int cmbColorscaleCurrentIndex() const;
        int cmbMarginalFitFunctionCrrentIndex() const;

    protected slots:
        void set_palette(int dummy);
        /** \brief called when the mouse moves over the image plot window */
        void imageMouseMoved(double x, double y);
        /** \brief called when the mouse is clicked on the image plot window. This is used together with actMaskEdit to control editing of image masks */
        void imageMouseClicked(double x, double y);

        void imageMouseDragged(double x1, double y1, double x2, double y2, Qt::KeyboardModifiers modifiers);
        /** \brief en/disables autoscaling of coloring
         *
         * This method en/disables the input widgets for the upper/lower counts. It does not trigger a redraw, as
         * the input fields are only en-/disabled and the values are not changed, so the display should be up to date!
         * \note parameter is just dummy
         */
        void setCountsAutoscale(bool autoscale=true);
        /** \brief en/disables autoscaling of histogram bins
         *
         * This method en/disables the input widget for the histogram bins. It does not trigger a redraw, as
         * the input fields are only en-/disabled and the values are not changed, so the display should be up to date!
         */
        void setHistogramBinsAutoscale(bool autoscale);
        /** \brief redraws the frame with the current settings for color scale ... This reads data from the data member \a image
         *  \note this does NOT recalculate the statistics. For a full recalc/redraw, call displayImageStatistics() first, or
         *        call redrawFrameRecalc().
         */
        void redrawFrame();
        /** \brief this function prepares the \a image data memeber, i.e. it copies the contents of rawImage
         *         and possibly applies some math to it (background removal ...)
         */
        void prepareImage();
        /** \brief redraws the frame with the current settings for color scale ... and recalculate statistics and transforms */
        void redrawFrameRecalc(bool forceHisto=false);
        /** \brief calculate image statistics */
        void displayImageStatistics(bool withHistogram=true, bool forceHistogram=false);
        /** \brief clear mask */
        void clearMask();
        /** \brief save mask */
        void saveMask();
        /** \brief load mask */
        void loadMask();
        void loadMask(const QString &filename);
        /** \brief save the current raw image (rawImage) */
        void saveRaw();
        void saveMulti();
        void saveMultiSeries();
        /** \brief save the current transformed image (image) */
        void saveTransformedImage();
        void histogramMask();
        void histogramChecked(bool checked);

        void updateGrid();

        void enableGraph(bool enabled);
        void graphParameterChanged();
        void updateGraph();
        void clearGraph();

    protected:
        /*! \brief save a given JKImage as a file (different formats)

            This method displays a filename selection dialog and then saves the given JKImage in the user-selected
            file format. THe parameter \A dialogTitle is used as the title line for the filnemae selection dialog
         */
        template<class T>
        void saveJKImage(const JKImage<T>& img, const QString& dialogTitle) {
            QStringList imageFilters;
            QString filtTIFF16, filtTIFFFloat, filtPNG, filtBMP, filtTIFF, filtCSV;
            imageFilters<<(filtTIFF16=tr("16-Bit Grayscal TIFF (*.tif *.tiff16)"));
            imageFilters<<(filtTIFFFloat=tr("Float Grayscal TIFF (*.tif *.tiff)"));
            imageFilters<<(filtPNG=tr("Color Coded PNG (*.png)"));
            imageFilters<<(filtBMP=tr("Color Coded BMP (*.bmp)"));
            imageFilters<<(filtTIFF=tr("Color Coded TIFF (*.tif)"));
            imageFilters<<(filtCSV=tr("Comma Separated Values (*.dat)"));

            QString imFilter=lastImagefilter;


            QString fileName = qfGetSaveFileName(this, dialogTitle,
                                    lastImagepath,
                                    imageFilters.join(";;"),&imFilter);

            /*QFileDialog dialog(this, tr("Save Raw Image as ..."), lastImagepath);
            dialog.setAcceptMode(QFileDialog::AcceptSave);
            dialog.setNameFilters(imageFilters);
            dialog.setNameFilter(imFilter);
            if (dialog.exec() != QDialog::Accepted) return ;
            imFilter = dialog.selectedNameFilter();
            QString fileName = dialog.selectedFiles()[0];*/

            if (fileName.isEmpty()) return;
            QFile file(fileName);
            if (!file.open(QFile::WriteOnly | QFile::Text)) {
                QMessageBox::warning(this, tr("QuickFit SPIM Control: %1").arg(dialogTitle),
                                  tr("Cannot write file '%1':\nreason: %2.")
                                  .arg(fileName)
                                  .arg(file.errorString()));
                if (m_stopresume) m_stopresume->resume();
                return;
            }
            file.close();
            file.remove();
            lastImagepath=QFileInfo(fileName).absolutePath();
            lastImagefilter=imFilter;
            QApplication::setOverrideCursor(Qt::WaitCursor);


            if (lastImagefilter==filtTIFF16) {
                img.save_tiffuint16(fileName.toStdString());
            } else if (lastImagefilter==filtTIFFFloat) {
                img.save_tifffloat(fileName.toStdString());
            } else if (lastImagefilter==filtCSV) {
                img.save_csv(fileName.toStdString());
            } else if (lastImagefilter==filtPNG) {
                QImage imgo;
                JKQTFPimagePlot_array2image<T>(img.data(), img.width(), img.height(), imgo, (JKQTFPColorPalette)cmbColorscaleCurrentIndex(), spinCountsLower->value(), spinCountsUpper->value());
                imgo.save(fileName, "PNG");
            } else if (lastImagefilter==filtBMP) {
                QImage imgo;
                JKQTFPimagePlot_array2image<T>(img.data(), img.width(), img.height(), imgo, (JKQTFPColorPalette)cmbColorscaleCurrentIndex(), spinCountsLower->value(), spinCountsUpper->value());
                imgo.save(fileName, "BMP");
            } else if (lastImagefilter==filtTIFF) {
                QImage imgo;
                JKQTFPimagePlot_array2image<T>(img.data(), img.width(), img.height(), imgo, (JKQTFPColorPalette)cmbColorscaleCurrentIndex(), spinCountsLower->value(), spinCountsUpper->value());
                imgo.save(fileName, "TIFF");
            } else {
                img.save_tiffuint16(fileName.toStdString());
            }
            QFileInfo fi(fileName);
            QString fn=fi.absolutePath()+"/"+fi.completeBaseName();
            QSettings setting(QString(fn+".configuration.ini"), QSettings::IniFormat);
            storeCameraConfig(setting);

            QApplication::restoreOverrideCursor();
        }




        void transformImage(JKImage<QFESPIMB040CameraView_internalImageType>& out, const JKImage<uint32_t>& raw);


};

#endif // QFESPIMB040CAMERAVIEW_H
