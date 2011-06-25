#ifndef QFSTAGECOMBOBOX_H
#define QFSTAGECOMBOBOX_H

#include <QComboBox>
#include "qfextensionmanager.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensionlinearstage.h"
#include "qfextension.h"

/*! \brief QComboBox which is filled with all available stages
    \ingroup qf3ext_spimb040


 */
class QFStageComboBox : public QComboBox {
        Q_OBJECT
    public:
        QFStageComboBox(QFExtensionManager* extManager, QWidget* parent);
        virtual ~QFStageComboBox();

        QFExtension* currentExtension() const;
        QFExtensionLinearStage* currentExtensionLinearStage() const;
        int currentAxisID() const;
        int currentExtensionID() const;
    public slots:
        /** \brief update the filled combobox */
        void findExtensions();
    protected:
        /** \brief list of all available QFExtensionLinearStage plugins, initialized by findExtensions() */
        QList<QObject*> stages;

        QFExtensionManager* m_extManager;
    private:
};

#endif // QFSTAGECOMBOBOX_H
