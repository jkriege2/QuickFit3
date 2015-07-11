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


#ifndef QFLUTEDITOR_H
#define QFLUTEDITOR_H

#include <QGraphicsView>
#include <QGraphicsItem>
#include <QList>
#include <QFrame>
#include <QTimer>
#include <QAction>
#include "jkqtpimagetools.h"
#include "../lib_imexport.h"

class QFLUTEditorNode; // forwrd

// see: http://qt-project.org/doc/qt-4.8/graphicsview-elasticnodes.html
/** \brief this widget allows to display and edit LUTs, based on line segments
 *  \ingroup QFVisualize3D
 *
 */
class LIB_EXPORT QFLUTEditor : public QFrame {
        Q_OBJECT
    public:
        QFLUTEditor( QWidget * parent = 0 );

        void getRangeForNode(QFLUTEditorNode *node, double& min, double& max) const;
        QPointF getPrevNodePos(QFLUTEditorNode *node) const;

        double getLutValueMin() const {return lut_value_min; }
        double getLutValueMax() const {return lut_value_max; }
        double getLutMin() const {return lut_min; }
        double getLutMax() const {return lut_max; }

        void setLutValueMin(double value)  { lut_value_min=value; ensureLutNodePositions(); }
        void setLutValueMax(double value)  { lut_value_max=value; ensureLutNodePositions(); }
        void setLutMin(double value)  { lut_min=value; ensureLutNodePositions(); }
        void setLutMax(double value)  { lut_max=value; ensureLutNodePositions(); }

        QFLUTEditorNode* addNode(QPointF pos, int lut=0);
        void deleteNode(int idx);

        /** \brief map from LUT to widget coordinate */
        QRectF map(const QRectF& r) const;
        /** \brief map from LUT to widget coordinate */
        QPointF map(const QPointF& r) const;
        /** \brief map from widget to LUT coordinate */
        QRectF invMap(const QRectF& r) const;
        /** \brief map from widget to LUT coordinate */
        QPointF invMap(const QPointF& r) const;

        QVector<int> getCurrentLUT(int items=4096) const;
        QVector<int> getCurrentAlphaLUT(int items=4096) const;

        QVector<int> getLUT(const QString& config, int items=4096, JKQTPMathImageColorPalette palette=JKQTPMathImageGRAY);
        QVector<int> getAlphaLUT(const QString& config, int items=4096, JKQTPMathImageColorPalette palette=JKQTPMathImageGRAY);

        void setHistogram(const QVector<double>& histogramY);
        void setHistogram(const double *data, int dataSize, int bins=256);
        void setHistogram(const float* data, int dataSize, int bins=256);
        void clearHistogram();

        QString getConfig() const;
        QString getDefaultConfig() const;
        void setConfig(const QString& config, bool update=true);

        bool getAlphaMode() const { return alphaMode; }
        void setAlphaMode(bool a);

        JKQTPMathImageColorPalette getColorPalette() const { return LUTPalette; }

        QSize sizeHint() const;
        QSize minimumSizeHint() const;

    signals:
        void lutChanged(QVector<int> lut);        

    public slots:
        void setColorPalette(JKQTPMathImageColorPalette LUTPalette);

        void setLogHistogram(bool lh);
        void resetLUT();
        void showLUTInfo();

    protected slots:
        void emitLUTChanged();

    protected:
        QVector<double> histogramX, histogramY;
        QList<QFLUTEditorNode*> lut_nodes;
        double lut_min;
        double lut_max;
        double lut_value_min;
        double lut_value_max;

        bool logHistogram;

        int currentNode;
        int gradientWidth;

        bool alphaMode;

        JKQTPMathImageColorPalette LUTPalette;

        QTimer* lutTimer;

        QAction* actResetLUT;
        QAction* actGetLUTInfo;

        void ensureLutNodePositions();

        friend class QFLUTEditorNode;


        void normalizeHistogram();

        virtual void paintEvent(QPaintEvent* event);

        virtual void mouseDoubleClickEvent ( QMouseEvent * event );
        virtual void mouseMoveEvent ( QMouseEvent * event );
        virtual void mousePressEvent ( QMouseEvent * event );
        virtual void mouseReleaseEvent ( QMouseEvent * event );

        int borderWidth() const;
        int borderWidthWithGradient() const;
        int borderWidthBoth() const;

        void updateLUT();

        QString doubleToQString(double value, int prec=15, char f='g', QChar decimalSeparator='.') const;
        QString doubleVecToQString(const QVector<double> &value, int prec=15, char f='g', QChar decimalSeparator='.', const QString itemSeparator=";") const;
        double QStringToDouble(QString value) const;
        QVector<double> QStringToDoubleVec(QString value, const QString itemSeparator=";") const;
};



class LIB_EXPORT QFLUTEditorNode {
     public:
         QFLUTEditorNode(int lut, QColor color, QFLUTEditor *graphWidget);


         int getLut() const { return lut; }

         QPointF pos() const { return m_pos; }
         void setPos(const QPointF& pos) { m_pos=pos; }
         void setPos(double x, double y) { m_pos=QPointF(x,y); }
         void setPosCorrected(const QPointF& pos);

         void paint(QPainter *painter, bool activated=false);

         QColor getColor() const { return color; }
         double getNodeSize() const { return nodeSize; }
     protected:


     private:
         QColor color;
         int lut;
         qreal nodeSize;
         QPointF m_pos;
         QFLUTEditor *graph;

 };

#endif // QFLUTEDITOR_H
