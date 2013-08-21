#ifndef QFRDRIMAGINGFCSIMAGEPARAMETERGROUPBOX_H
#define QFRDRIMAGINGFCSIMAGEPARAMETERGROUPBOX_H

#include <QGroupBox>
#include "jkqtptools.h"
#include <QComboBox>
#include "jkqtpimageelements.h"
#include "jkqtpimagetools.h"
#include <QCheckBox>
#include "qfdoubleedit.h"
#include <QList>

class QFRawDataRecord;

class QFRDRImagingFCSImageParameterGroupBox : public QGroupBox
{
        Q_OBJECT
    public:
        explicit QFRDRImagingFCSImageParameterGroupBox(QWidget *parent = 0);
        explicit QFRDRImagingFCSImageParameterGroupBox(const QString& title, QWidget *parent = 0);

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

        JKQTPMathImage::ColorPalette colorPalette() const;
        void setColorPalette(JKQTPMathImage::ColorPalette palette);

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
        JKQTPMathImageColorPalette* cmbColorbar;
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


#endif // QFRDRIMAGINGFCSIMAGEPARAMETERGROUPBOX_H
