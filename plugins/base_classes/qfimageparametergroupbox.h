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



#ifndef QFIMAGEPARAMETERGROUPBOX_H
#define QFIMAGEPARAMETERGROUPBOX_H

#include <QGroupBox>
#include "jkqtptools.h"
#include <QComboBox>
#include "jkqtpimageelements.h"
#include "jkqtpimagetools.h"
#include <QCheckBox>
#include "qfdoubleedit.h"
#include <QList>

class QFRawDataRecord;

class QFImageParameterGroupBox : public QGroupBox
{
        Q_OBJECT
    public:
        explicit QFImageParameterGroupBox(QWidget *parent = 0);
        explicit QFImageParameterGroupBox(const QString& title, QWidget *parent = 0);

        void setSelectedMaskStyle(JKQTPOverlayImageEnhanced* plteImageExcluded) const;
        void setSelectedImageStyle(JKQTPMathImage* plteImage, bool* plteOverviewExcludedData, bool emitSignals=true);
        int getMaskStyle() const;
        void setMaskStyle(int idx);

        bool isColorAutoscale() const;
        void setColorAutoscale(bool enable);
        double getColMin() const;
        double getColMax() const;
        void setColMin(double value);
        void setColMax(double value);
        int getImageStyle() const;
        bool getImageStyleDisplayMask() const;
        void setImageStyle(int i);
        int getOutOfRangeMode() const;
        void setOutOfRangeMode(int i);

        QString getColorbarName() const;
        QString getImageStyleName() const;
        QString getOutOfRangeName() const;

        JKQTPMathImageColorPalette colorPalette() const;
        void setColorPalette(JKQTPMathImageColorPalette palette);

        void saveConfig(QFRawDataRecord* record, const QString& egroup, const QString &param, const QString& imageID);
        void loadConfig(QFRawDataRecord* record, const QString& egroup, const QString &param, const QString& imageID, const QString& prefix=QString(), double mi=0, double ma=1);

        void writeSettings(QSettings& settings, const QString& prefix=QString()) const;
        void readSettings(QSettings& settings, const QString& prefix=QString());
    signals:        
        void settingsChanged();
    public slots:
        void connectWidgets(bool connectTo=true);
    protected slots:
        void emitSettingsChanged();
    protected:
        /** \brief combobox for the color bar of plteImage */
        JKQTPMathImageColorPaletteComboBox* cmbColorbar;
        /** \brief combobox for the display style of plteImage */
        QComboBox* cmbImageStyle;
        /** \brief cobobox to select what should happen to the out-of-range pixels */
        QComboBox* cmbOutOfRangeMode;

        /** \brief checkbox to en-/disable automatic color bar scaling */
        QCheckBox* chkImageAutoScale;

        QFDoubleEdit* edtColMin;
        QFDoubleEdit* edtColMax;

        QList<QColor> maskColors;

        void initGrp();
};


#endif // QFIMAGEPARAMETERGROUPBOX_H
