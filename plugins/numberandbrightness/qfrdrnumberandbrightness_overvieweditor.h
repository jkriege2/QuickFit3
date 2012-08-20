#ifndef QFRDRNUMBERANDBRIGHTNESSOVERVIEWEDITOR_H
#define QFRDRNUMBERANDBRIGHTNESSOVERVIEWEDITOR_H

#include "qfrawdataeditor.h"
#include <QLabel>
#include "qt/jkqtplotter.h"
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>
#include "qvisiblehandlesplitter.h"
#include <QSplitter>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QAbstractTableModel>
#include "datacutslider.h"
#include "qt/jkqtfastplotter.h"
#include "jkqtptools.h"
#include "qfrdroverviewimagedisplay.h"


/*! \brief editor for QFRawDataRecord
    \ingroup qf3rdrdp_number_and_brightness
*/
class QFRDRNumberAndBrightnessOverviewEditor : public QFRawDataEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRNumberAndBrightnessOverviewEditor(QFPluginServices* services,  QFRawDataPropertyEditor* propEditor, QWidget* parent);
        /** Default destructor */
        virtual ~QFRDRNumberAndBrightnessOverviewEditor();
    protected slots:
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged();
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old);

    protected:
        /** \brief create widgets on object creation */
        void createWidgets();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        QFRDROverviewImageDisplay* display;
};

#endif // QFRDRNUMBERANDBRIGHTNESSOVERVIEWEDITOR_H
