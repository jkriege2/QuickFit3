#ifndef QFETCSPCIMPORTER_H
#define QFETCSPCIMPORTER_H

#include <time.h>
#include <QObject>
#include "qfextension.h"

class QFETCSPCImporterDialog; // forward

/*!
    \defgroup qf3ext_tcspcimporter QFExtension implementation
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtension implementation
    \ingroup qf3ext_tcspcimporter
 */
class QFETCSPCImporter : public QObject, public QFExtensionBase {
        Q_OBJECT
        Q_INTERFACES(QFExtension)
    public:
        /** Default constructor */
        QFETCSPCImporter(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFETCSPCImporter();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("tcspcimporter"); }
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("TCSPC importer"); }
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("TCSPC importer (FCS, count rates, ...)"); }
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); }
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2012 by Jan W. krieger"); }
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); }
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/tcspcimporter/tcspcimporter.png"); }
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        }
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();
    protected slots:
        void startPlugin();
    protected:
        /** \copydoc QFExtensionBase::projectChanged() */
        virtual void projectChanged(QFProject* oldProject, QFProject* project);
        /** \copydoc QFExtensionBase::initExtension() */
        virtual void initExtension();
        /** \copydoc QFExtensionBase::loadSettings() */
        virtual void loadSettings(ProgramOptions* settings);
        /** \copydoc QFExtensionBase::storeSettings() */
        virtual void storeSettings(ProgramOptions* settings);



        /** \brief log project text message
         *  \param message the message to log
         */
        virtual void log_text(QString message);
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message);
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message);

        void insertFCSCSVFile(const QString &filenameFCS, const QString& filenameCR, const QMap<QString, QVariant> &paramValues, const QStringList &paramReadonly, const QString& group=QString(""), const QString& role=QString(""));
        void insertFCCSCSVFile(const QString& filenameFCS, const QString &filenameCR1, const QString &filenameCR2, const QMap<QString, QVariant> &paramValues, const QStringList &paramReadonly, const QString& group=QString(""), const QString& role=QString(""));
        void insertCountRate(const QString &filename, const QMap<QString, QVariant> &paramValues, const QStringList &paramReadonly, const QString& group=QString(""), const QString& role=QString(""));

        QFETCSPCImporterDialog* dlgCorrelate;
	protected:
        QFPluginLogService* logService;
		
	protected slots:
        void correlationDialogClosed();

};

#endif // QFETCSPCIMPORTER_H
