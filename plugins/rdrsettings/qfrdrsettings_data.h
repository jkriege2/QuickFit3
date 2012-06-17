#ifndef QFRDRSETTINGSDATA_H
#define QFRDRSETTINGSDATA_H

#include <QList>
#include <QObject>
#include <QMap>
#include <QProgressBar>
#include <QStatusBar>
#include <QWidget>
#include <QMenu>
#include <QPair>
#include <QList>
#include <QAction>
#include "qfrawdatarecord.h"
#include "qfrdrsettings_dataeditor.h"
#include "qfrawdatarecordfactory.h"


/*! \brief QFRawDataRecord implementation
    \ingroup qf3rdrdp_rdr_settings

*/
class QFRDRSettingsData : public QFRawDataRecord {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRSettingsData(QFProject* parent);
        /** Default destructor */
        virtual ~QFRDRSettingsData();

        /** \brief return type (short type string) */
        virtual QString getType() const { return "rdr_settings"; }
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("configuration file"); }
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/rdr_settings/qfrdrsettings_small.png"); }
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("configuration/ini file"); }
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/rdr_settings/qfrdrsettings.png"); }
        /** \brief returns the number of additional editor panes for this record */
        virtual int getEditorCount() { return 1; }
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName(int i) {
            if (i==0) return tr("configuration file editor");
            return QString("");
        }
        /** \brief create an object for the i-th editor pane */
        virtual QFRawDataEditor* createEditor(QFPluginServices* services,  QFRawDataPropertyEditor *propEditor, int i=0, QWidget* parent=NULL) {
            if (i==0) return new QFRDRSettingsDataEditor(services, propEditor, parent);
            return NULL;
        }
        /** \brief export the raw data into the specified format */
        virtual void exportData(const QString& format, const QString& filename)const ;
        /** \brief returns a list of filetypes which correspond to the filetypes returned by getExportFiletypes() */
        QStringList getExportFiletypes() {
            QStringList sl;
            return sl;
        }
        /** \brief returns the title of the Export file dialog */
        QString getExportDialogTitle() { return tr(""); }
        /** \brief returns the filetype of the Export file dialog */
        QString getExportDialogFiletypes() { return tr(""); }


		/** \brief some example method that returns data */
        QSettings* getData() const;
    protected:
        /** \brief write the contents of the object to a XML file */
        virtual void intWriteData(QXmlStreamWriter& w);
        /** \brief read in external data files <b>and</b> data stored in the project file <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * If \a e is \c NULL then this method should only read the datafiles already saved in the files property.
         */
        virtual void intReadData(QDomElement* e=NULL);

		/** \brief load data file */
        bool loadDataFile(QString filename);

    private:
		/** \brief example: stores some data */
        QSettings* data;
};


#endif // QFRDRSETTINGSDATA_H
