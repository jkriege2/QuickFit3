#ifndef QFESPIMB040EXPERIMENTDESCRIPTION_H
#define QFESPIMB040EXPERIMENTDESCRIPTION_H

#include <QWidget>
#include <QSettings>

namespace Ui {
    class QFESPIMB040ExperimentDescription;
}
/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg): experiment description widget
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040ExperimentDescription : public QWidget {
        Q_OBJECT
    public:
        explicit QFESPIMB040ExperimentDescription(QWidget *parent = 0);
        ~QFESPIMB040ExperimentDescription();
        /** \brief load settings */
        void loadSettings(QSettings& settings, QString prefix);
        /** \brief save settings */
        void storeSettings(QSettings& settings, QString prefix) const;

        QString getID() const;

        /*! \brief return a map containing the experiment description
         */
        QMap<QString, QVariant> getDescription() const;
        QString getTitle() const;
        QString getExperimenter() const;
protected slots:
        void updateTime();
        void on_btnClearAll_clicked();
        void on_btnSaveTemplate_clicked();
        void on_btnLoadTemplate_clicked();
    private:
        Ui::QFESPIMB040ExperimentDescription *ui;
};

#endif // QFESPIMB040EXPERIMENTDESCRIPTION_H
