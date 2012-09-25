#ifndef QFRDRNUMBERANDBRIGHTNESS_H
#define QFRDRNUMBERANDBRIGHTNESS_H

#include "qfpluginrawdata.h"
#include "qmodernprogresswidget.h"
#include "qfrdrimagingfcstools.h"
#include "qftools.h"

/*!
    \defgroup qf3rdrdp_number_and_brightness Raw Data Record Plugin
    \ingroup qf3rawdataplugins
*/

/*! \brief raw data record plugin class
    \ingroup qf3rdrdp_number_and_brightness
    */

class QFRDRNumberAndBrightnessPlugin : public QObject, public QFPluginRawDataRecordBase {
        Q_OBJECT
        Q_INTERFACES(QFPluginRawDataRecord)
    public:
        /** Default constructor */
        QFRDRNumberAndBrightnessPlugin(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFRDRNumberAndBrightnessPlugin();

        /** \brief Create a new QFRawDataRecord in the given project \a parent. Also adds the record to the project. */
        virtual QFRawDataRecord* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        /** \brief short ID for the plugin */
        virtual QString getID() const  { return tr("number_and_brightness"); };

        /** \brief name for the plugin */
        virtual QString getName() const  { return tr("Number & Brightness"); };

        /** \brief short description for the plugin */
        virtual QString getDescription() const  { return tr("number and brightness data evaluation/record"); };

        /** \brief author the plugin */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const  { return tr("(c) 2012 by Jan W. Krieger"); };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); };
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };

        /** \brief icon for the plugin */
        virtual QString getIconFilename() const  { return QString(":/number_and_brightness/qfrdrnumberandbrightness.png"); };
    protected slots:
        /** \brief insertdata from file*/
        void insertFromImFCSRecord();
        void insertImFCSFile(const QString &filename);

        /** \brief insert record, if it is not yet contained in the project! */
        void insertProjectRecord(const QString& type, const QString& name, const QString& filename, const QString& description=QString(""), const QString& directory=QString(""), const QMap<QString,QVariant>& init_params=QFStringVariantMap(), const QStringList& init_params_readonly=QStringList());

        bool parseSPIMSettings(const QString& filename_settings, QString& description, QMap<QString,QVariant>& initParams, QStringList& paramsReadonly, QStringList& files, QStringList& files_types, QStringList& files_descriptions);
};

#endif // QFRDRNUMBERANDBRIGHTNESS_H
