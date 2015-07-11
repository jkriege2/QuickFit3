/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef QFVisualize3DGLEditor_H
#define QFVisualize3DGLEditor_H

#include <QSettings>
#include <QMainWindow>
#include "jkqtpimagetools.h"
#include "qfvisualize3dglwidget.h"
#include "../lib_imexport.h"

namespace Ui {
    class QFVisualize3DGLEditor;
}
/** \brief wrapper class around QFVisualize3DGLWidget, with additional widgets to set the rendering mode ...
 *  \ingroup QFVisualize3D
 */
class LIB_EXPORT QFVisualize3DGLEditor : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFVisualize3DGLEditor(QWidget *parent = 0);
        ~QFVisualize3DGLEditor();

        struct volumeData {
            float* data;
            int sizeX;
            int sizeY;
            int sizeZ;
            double wx;
            double wy;
            double wz;
            bool internal;
            QString name;

            void free();
            int size() const { return sizeX*sizeY*sizeZ; }
        };

        int getVolumeDataCount() const { return data.size(); }
        volumeData getVolumeData(int idx) const { return data[idx]; }
        void removeVolumeData(int idx);
        void clearVolumeData();
        void addVolumeData(float* data, int sizeX, int sizeY, int sizeZ, double wx, double wy, double wz, const QString& name=QString(""));
        void addCopiedVolumeData(float* data, int sizeX, int sizeY, int sizeZ, double wx, double wy, double wz, const QString& name=QString(""));
        void addCopiedVolumeData(double* data, int sizeX, int sizeY, int sizeZ, double wx, double wy, double wz, const QString& name=QString(""));
        void addCopiedVolumeData(uint8_t* data, int sizeX, int sizeY, int sizeZ, double wx, double wy, double wz, const QString& name=QString(""));
        void addCopiedVolumeData(uint16_t* data, int sizeX, int sizeY, int sizeZ, double wx, double wy, double wz, const QString& name=QString(""));
        void addCopiedVolumeData(uint32_t* data, int sizeX, int sizeY, int sizeZ, double wx, double wy, double wz, const QString& name=QString(""));
        void addCopiedVolumeData(uint64_t* data, int sizeX, int sizeY, int sizeZ, double wx, double wy, double wz, const QString& name=QString(""));

        int getCurrentGeometryID() const;
        int getCurrentGeometryID(int i) const;

        void addDefaultPalette(JKQTPMathImageColorPalette pal);
        void setDefaultPalette(JKQTPMathImageColorPalette pal);
        void setDefaultPalettes(JKQTPMathImageColorPalette pal, JKQTPMathImageColorPalette pal1);
        void setDefaultPalettes(JKQTPMathImageColorPalette pal, JKQTPMathImageColorPalette pal1, JKQTPMathImageColorPalette pal3);
        void setDefaultPalettes(JKQTPMathImageColorPalette pal, JKQTPMathImageColorPalette pal1, JKQTPMathImageColorPalette pal3, JKQTPMathImageColorPalette pal4);
        void clearDefaultPalettes();

        QString getConfig() const;
        void setConfig(const QString& config);
        void writeSettings(QSettings& settings, const QString& prefix=QString("")) const;
        void readSettings(QSettings& settings, const QString& prefix=QString(""));

        QFVisualize3DGLWidget* get3DWidget() const;

        bool get_doUpdate() const;
        void set_doUpdate(bool enabled);

        QImage getImage() const;
    protected slots:
        void update3DDataset();
        void setCurrent(int item);
        void storeSceneSettingsFromWidgets();
        void initInternalState();
        void updateGlobalWidgets();
        void on_cmbBlend_currentIndexChanged(int blend);
    protected:
        static QString CDoubleToQString(double value);
        static double CQStringToDouble(QString value);
        static QString boolToQString(bool data);
        static bool QStringToBool(const QString& data);
        static QColor QStringToQColor(const QString& data);
    private:
        Ui::QFVisualize3DGLEditor *ui;
        QList<JKQTPMathImageColorPalette> defaultPalettes;

        QMap<int, QString> mapLUT, mapLUTAlpha;
        QMap<int, JKQTPMathImageColorPalette> mapColorPal;
        QMap<int, QFVisualize3DGLWidget::VisualizationMode> mapVizMode;
        QList<volumeData> data;

        int dataCounter;

};


#endif // QFVisualize3DGLEditor_H
