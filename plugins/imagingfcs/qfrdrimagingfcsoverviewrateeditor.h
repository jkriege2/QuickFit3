#ifndef QFRDRIMAGINGFCSOVERVIEWRATEEDITOR_H
#define QFRDRIMAGINGFCSOVERVIEWRATEEDITOR_H

#include <QWidget>
#include "qfrawdatarecord.h"
#include "qfrawdataeditor.h"
#include "qfrdrimagingfcs_data.h"
#include "qfrdrimagingfcscountratedisplay.h"
#include "qfrdroverviewimagedisplay.h"
#include "qvisiblehandlesplitter.h"

class QFRDRImagingFCSOverviewRateEditor : public QFRawDataEditor
{
        Q_OBJECT
    public:
        explicit QFRDRImagingFCSOverviewRateEditor(QFPluginServices* services, QFRawDataPropertyEditor* propEditor, QWidget *parent = 0);
        virtual ~QFRDRImagingFCSOverviewRateEditor();

    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) ;
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged() ;

    public slots:
        /** \brief read the settings */
        virtual void readSettings() ;
        /** \brief write the settings */
        virtual void writeSettings() ;
    protected:
        void createWidgets();

        QFRDRImagingFCSCountrateDisplay* crEditor;
        QFRDROverviewImageDisplay* imEditor1;

        QVisibleHandleSplitter* mainSplitter;

};

#endif // QFRDRIMAGINGFCSOVERVIEWRATEEDITOR_H
