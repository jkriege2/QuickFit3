#include "qfrdrsettings.h"
#include "qfrdrsettings_data.h"
#include "qfrdrsettings_dataeditor.h"

QFRDRSettingsPlugin::QFRDRSettingsPlugin(QObject* parent):
    QObject(parent)
{
    //constructor
}

QFRDRSettingsPlugin::~QFRDRSettingsPlugin()
{
    //destructor
}

QFRawDataRecord* QFRDRSettingsPlugin::createRecord(QFProject* parent) {
	// factory method: create a QFRawDataRecord objectof the type of this plugin (QFRDRSettingsData)
    return new QFRDRSettingsData(parent);
}


void QFRDRSettingsPlugin::registerToMenu(QMenu* menu) {
	// create menu entries to insert data with this type
    QAction* action=new QAction(QIcon(getIconFilename()), tr("Insert configuration file"), parentWidget);
    action->setStatusTip(tr("Insert a new record"));
    connect(action, SIGNAL(triggered()), this, SLOT(insertRecord()));
    menu->addAction(action);
}


void QFRDRSettingsPlugin::insertRecord() {
    if (project) {
        // file format to import
        QString format_name=tr("Configuration/INI File (*.ini *.cfg)");
        // look into INI which was the last used format 
		QString current_format_name=settings->getQSettings()->value("rdr_settings/current_format_filter", format_name).toString();
        // let the user select some files to import
		QStringList files = QFileDialog::getOpenFileNames(parentWidget,
                              tr("Select Configuration/INI File(s) to Import ..."),
                              settings->getCurrentRawDataDir(),
                              format_name, &current_format_name);
        // store the format we just used
		settings->getQSettings()->setValue("rdr_settings/current_format_filter", current_format_name); 
		// here we store some initial parameters
        QMap<QString, QVariant> initParams;

		// add all properties in initParams that will be readonly
        QStringList paramsReadonly;
		
		// now we iterate over all files and use QuickFit's progress bar interface (see plugin services)
        QStringList list = files;
        QStringList::Iterator it = list.begin();
        services->setProgressRange(0, list.size());
        services->setProgress(0);
        int i=0;
        while(it != list.end()) {
            i++;
			QString filename=*it;
            if (QFile::exists(filename)) {
                services->log_text(tr("loading [%2] '%1' ...\n").arg(filename).arg(current_format_name));
                if (current_format_name==format_name) {
					// insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
					QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName(), QStringList(filename), initParams, paramsReadonly);
					if (e->error()) { // when an error occured: remove record and output an error message
						project->deleteRawData(e->getID());
						QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
						services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
					}                
				}
                settings->setCurrentRawDataDir(QFileInfo(*it).dir().absolutePath());
                ++it;
            }
            services->setProgress(i);
            QApplication::processEvents();
        }
        services->setProgress(0);
    }
}



Q_EXPORT_PLUGIN2(rdr_settings, QFRDRSettingsPlugin)
