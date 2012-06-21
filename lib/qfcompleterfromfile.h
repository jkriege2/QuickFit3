#ifndef QFCOMPLETERFROMFILE_H
#define QFCOMPLETERFROMFILE_H

#include <QCompleter>
#include <QStringListModel>
#include <QString>
#include <QStringList>
#include <QFile>
#include "lib_imexport.h"

/** \brief completer class that reads and writes the completer list to and from a file
 *  \ingroup qf3lib_widgets
 */
class QFLIB_EXPORT QFCompleterFromFile : public QCompleter {
    Q_OBJECT
public:
    /** Default constructor */
    QFCompleterFromFile(QObject * parent = 0);
    /** Default destructor */
    virtual ~QFCompleterFromFile();
    /** Access m_filename
     * \return The current value of m_filename
     */
    QString filename() { return m_filename; }
    /** Set m_filename
     * \param val New value to set
     */
    void setFilename(QString val);
    /** \brief return the QStringListModel that holds the completer list */
    QStringListModel* stringlistModel() const { return m_stringlist; }
protected:
private:
    /** \brief filename of the file that is used to read and store the internal model
     *
     * Storing is disabled when this is set to an empty string (default).
     */
    QString m_filename;
    QStringListModel* m_stringlist;
protected slots:
    /** \brief used to store the internal model contents into the file in m_filename */
    void modelDataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight );
    /** \brief used to store the internal model contents into the file in m_filename */
    void modelRowChanged ( const QModelIndex & parent, int start, int end );
    /** \brief used to store the internal model contents into the file in m_filename */
    void modelReset ();
};

#endif // QFCOMPLETERFROMFILE_H
