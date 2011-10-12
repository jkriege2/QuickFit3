#ifndef QFESPIMB040OPTICSSETUP_H
#define QFESPIMB040OPTICSSETUP_H

#include <QWidget>
#include "programoptions.h"
#include "qfpluginservices.h"

class QFESPIMB040MainWindow; // forward
namespace Ui {
    class QFESPIMB040OpticsSetup; // forward
}
/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg): instrument setup widget
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040OpticsSetup : public QWidget
{
    Q_OBJECT

public:
    explicit QFESPIMB040OpticsSetup(QFESPIMB040MainWindow* parent, QFPluginServices* pluginServices);
    ~QFESPIMB040OpticsSetup();

    /** \brief load settings */
    void loadSettings(QSettings& settings, QString prefix);
    /** \brief save settings */
    void storeSettings(QSettings& settings, QString prefix);

private:
    Ui::QFESPIMB040OpticsSetup *ui;
    QFESPIMB040MainWindow* m_parent;
    QFPluginServices* m_pluginServices;
};

#endif // QFESPIMB040OPTICSSETUP_H
