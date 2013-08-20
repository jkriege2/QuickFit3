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
        void setSelectedImageStyle(JKQTPMathImage* plteImage, bool* plteOverviewExcludedData);
        int getMaskStyle() const;
        void setMaskStyle(int idx);

        JKQTPMathImage::ColorPalette colorPalette() const;
        void setColorPalette(JKQTPMathImage::ColorPalette palette);

        void saveConfig(QFRawDataRecord* record, const QString& egroup, const QString &param, const QString& imageID);
        void loadConfig(QFRawDataRecord* record, const QString& egroup, const QString &param, const QString& imageID, const QString& prefix=QString());

        void writeSettings(QSettings& settings, const QString& prefix=QString()) const;
        void readSettings(QSettings& settings, const QString& prefix=QString());
    signals:
        void settingsChanged();
    public slots:
        
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
