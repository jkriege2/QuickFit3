#ifndef QFESPIMB040CONFIGCOMBOBOX_H
#define QFESPIMB040CONFIGCOMBOBOX_H

#include <QComboBox>
#include <QIcon>
#include <QString>
#include "qenhancedcombobox.h"

/*! \brief a combobox which displays a set of configuration files from a given directory
    \ingroup qf3ext_spimb040

    the displayed name is read from the property \c name from the config file,
    when opened using QSettings. if also the property \c icon is set, the file
    referenced therein will be used as the combobox item icon. Otherwise no icon, or
    the QIcon, specified in \a icon are used

  */
class QFESPIMB040ConfigComboBox : public QEnhancedComboBox {
        Q_OBJECT
    public:
        explicit QFESPIMB040ConfigComboBox(QWidget *parent = 0);
        virtual ~QFESPIMB040ConfigComboBox();

        QString currentConfigFilename() const;
        QString currentConfig() const;
        void setIcon(QIcon icon);
        void init(QString baseDir, QString extension=QString("ini"));
    signals:
        /** \brief called when the user selects a new configuration.
         *
         *  \param filename contains the full filename including the absolute path.
         */
        void currentConfigChanged(QString filename);

        /** \brief emitted when the list of lightpath configs changes */
        void configsChanged(QList<QPair<QIcon, QString> > configs);
    public slots:
        /** \brief select the given configuration from a given filename
         *
         *  \param filename the config file to load: either a filename relative to the given main path
         *                  or an absolute filename (then it is not guaranteed that the file can be selected!!!)
         */
        void setCurrentConfigFilename(QString filename);
        /** \brief select the given configuration from a given name
         *
         *  \param config the name of the configuration to select
         */
        void setCurrentConfig(QString config);
    protected:
        QIcon icon;
        QString baseDir;
        QString m_ext;
    protected slots:
        void cmbCurrentIndexChanged(int index);
        QString getConfigName(QString filename) const;
        QIcon getConfigIcon(QString filename) const;

};

#endif // QFESPIMB040CONFIGCOMBOBOX_H
