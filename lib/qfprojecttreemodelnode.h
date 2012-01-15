#ifndef QFPROJECTTREEMODELNODE_H
#define QFPROJECTTREEMODELNODE_H

#include <QObject>
#include <QList>
#include <QVariant>
#include <QIcon>
#include "lib_imexport.h"


// forward declaration
class QFProject;
class QFRawDataRecord;
class QFEvaluationItem;


/*! \brief helper class for QFProjectTreeModel which is used to build a virtual project tree
    \ingroup qf3lib_project
*/
class QFLIB_EXPORT QFProjectTreeModelNode : public QObject {
        Q_OBJECT        
    public:
        enum nodeType {
            qfpntUnknown=0,
            qfpntRoot=1,
            qfpntProject=2,
            qfpntDirectory=3,
            qfpntRawDataRecord=4,
            qfpntEvaluationRecord=5
        };
        static QString nodeType2String( const nodeType nt)  {
            switch(nt) {
                case qfpntUnknown: return tr("unknown");
                case qfpntRoot: return tr("root");
                case qfpntProject: return tr("project");
                case qfpntDirectory: return tr("directory");
                case qfpntRawDataRecord: return tr("rawDataRecord");
                case qfpntEvaluationRecord: return tr("evaluationRecord");
            };
            return tr("invalid");
        }

        QFProjectTreeModelNode(nodeType type, QFProjectTreeModelNode *parent=NULL);
        ~QFProjectTreeModelNode();

        void appendChild(QFProjectTreeModelNode *child);

        QFProjectTreeModelNode *child(int row);
        int childCount() const;
        int columnCount() const;
        int row() const;
        QFProjectTreeModelNode *parent();
        nodeType type() const;
        QIcon icon() const;
        void setIcon(QIcon icon);
        QString title() const;
        void setTitle(QString title);
        QString toolHelp() const;
        void setToolHelp(QString title);
        QFEvaluationItem* evaluationItem() const;
        void setEvaluationItem(QFEvaluationItem* item);
        QFRawDataRecord* rawDataRecord() const;
        void setRawDataRecord(QFRawDataRecord* rdr);
        QFProject* project() const;
        void setProject(QFProject* project);

        QFProjectTreeModelNode* addChild(nodeType type, const QString& title=QString(""));
        QFProjectTreeModelNode* addChild(QFRawDataRecord* rdr);
        QFProjectTreeModelNode* addChild(QFEvaluationItem* item);
        QFProjectTreeModelNode* addChildFolder(const QString& title);
    signals:

    public slots:

    private:
        QList<QFProjectTreeModelNode*> childItems;
        QFProjectTreeModelNode *parentItem;
        nodeType m_type;
        QFEvaluationItem* evalItem;
        QFRawDataRecord* rdr;
        QString m_title;
        QString m_toolHelp;
        QFProject* m_project;
};

#endif // QFPROJECTTREEMODELNODE_H
