#ifndef QFSTAGECOMBOBOX_H
#define QFSTAGECOMBOBOX_H

#include <QComboBox>
#include "qfextensionmanager.h"
#include "../interfaces/qfextensionlinearstage.h"
#include "qfextension.h"

/*! \brief QComboBox which is filled with all available stages
    \ingroup qf3ext_spimb040


 */
class QFStageComboBox : public QComboBox {
        Q_OBJECT
    public:
        /** \briefclass cosntructor which does NOT initialize the contents (call init() separately!!!) */
        QFStageComboBox(QWidget* parent=NULL);
        /** \briefclass cosntructor which initializes the contents from the given extManager */
        QFStageComboBox(QFExtensionManager* extManager, QWidget* parent);
        virtual ~QFStageComboBox();

        /** \brief initialize the available stages from the given QFExtensionManager */
        void init(QFExtensionManager* extManager);
        /** \brief currently selected stage as QFExtension */
        QFExtension* currentExtension() const;
        /** \brief currently selected stage as QFExtensionLinearStage */
        QFExtensionLinearStage* currentExtensionLinearStage() const;
        /** \brief currently selected stage axis ID */
        int currentAxisID() const;
    public slots:
        /** \brief update the filled combobox */
        void findExtensions();
    protected:
        /** \brief list of all available QFExtensionLinearStage plugins, initialized by findExtensions() */
        QList<QObject*> stages;

        /** \brief ID of the current item in stages */
        int currentExtensionID() const;

        QFExtensionManager* m_extManager;
    private:
};

#endif // QFSTAGECOMBOBOX_H
