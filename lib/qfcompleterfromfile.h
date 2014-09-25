/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    inline QString filename() { return m_filename; }
    /** Set m_filename
     * \param val New value to set
     */
    void setFilename(QString val);
    /** \brief return the QStringListModel that holds the completer list */
    inline QStringListModel* stringlistModel() const { return m_stringlist; }
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
