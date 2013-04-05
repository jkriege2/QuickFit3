#ifndef QFESPIMB040FILENAMETOOL_H
#define QFESPIMB040FILENAMETOOL_H

#include <QMap>
#include <QString>
#include "qenhancedlineedit.h"
#include <QWidget>
#include <QDir>
#include <QFileInfo>

class QFESPIMB040OpticsSetup; //forward
class QFESPIMB040ExperimentDescription; //forward
class QFESPIMB040AcquisitionDescription; //forward

class QFESPIMB040FilenameTool {
    public:
        QFESPIMB040FilenameTool();

        QString transformFilename(const QString& filename) const;
        void setReplaceValues(const QMap<QString, QString>& replaceValues);
        void setReplaceValue(const QString& key, const QString& value);
        void bindLineEdit(QEnhancedLineEdit* edit);
        void setGlobalReplaces(QFESPIMB040OpticsSetup* setup, QFESPIMB040ExperimentDescription* exp, QFESPIMB040AcquisitionDescription* acq);

    protected:
        QMap<QString, QString> replaceValues;
        QList<QEnhancedLineEdit*> edits;
        void fillLineEdits();
};

#endif // QFESPIMB040FILENAMETOOL_H
