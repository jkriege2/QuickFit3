#ifndef QFRDRSETTINGSEDITOR_H
#define QFRDRSETTINGSEDITOR_H

#include "qfrawdataeditor.h"
#include <QLabel>
#include <QTreeView>
#include "qsettingsmodel.h"

/*! \brief editor for QFRawDataRecord
    \ingroup qf3rdrdp_rdr_settings
*/
class QFRDRSettingsDataEditor : public QFRawDataEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRSettingsDataEditor(QFPluginServices* services, QFRawDataPropertyEditor* propEditor, QWidget* parent);
        /** Default destructor */
        virtual ~QFRDRSettingsDataEditor();
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

        /** \brief label for test */
        QTreeView* tree;
        QLabel* labFilename;
        QSettingsModel* model;
    private:
};

#endif // QFRDRSETTINGSEDITOR_H
