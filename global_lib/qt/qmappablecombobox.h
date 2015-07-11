#ifndef QMAPPABLECOMBOBOX_H
#define QMAPPABLECOMBOBOX_H

#include <QComboBox>
#include <QFile>

/*! \brief this combobox may be mapped by a QDataWidgetMapper to a model

    it also adds new features to QComboBox:
      - store to/load a file from the combobox list automatically (when setFilename() is set)
      - translations of the text and the list items with a given translations file with the translatedData property
      - load images associated with the current text, by searching in "<images_dir>/<text.toLower()>.png"
    .
*/
class QMappableComboBox : public QComboBox {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText USER true)
    Q_PROPERTY(QVariant data READ data WRITE setData)
    Q_PROPERTY(QString translatedText READ translatedText WRITE setTranslatedText)
public:
    /** Default constructor */
    QMappableComboBox(QWidget* parent = NULL);

    /** Default destructor */
    virtual ~QMappableComboBox();

    /** \brief set the current text
     *
     * if the \a text is found in the combobox items, the according item is selected.
     * Otherwise if the combobox is editable the edit is set.
     */
    void setText(QString text);

    /** \brief set the current item data
     *
     * if the \a data is found in the combobox items, the according item is selected.
     * Otherwise nothing happens
     */
    void setData(QVariant data);
    /** \brief returns the currently visible text */
    QString text() const;
    /** \brief returns the data of the current object */
    QVariant data() const;
    /** \brief set filename property \see m_filename
     *
     * When the filename is set the file contents is loaded into the internal model.
     */
    void setFilename(QString fn);
    /** \brief return the current filename \see m_filename */
    inline QString filename() const { return m_filename; };
    /** \brief set translation filename
     *
     * When the filename is set and the translatedData property is used, the data pushed into/out of the combobox is translated
     * Translation files should have this format:
     * \verbatim foreign_language;internal_language \endverbatim
     * An empty argument switches this function off.
     * If you also want the selection list to be translated, make sure this is set BEFORE setFilename()!
     */
    void setTranslationFilename(QString fn);
    /** \brief set the current item text
     *
     * If the \a text is found in the combobox items, the according item is selected.
     *
     * This function potentially translates external to internal.
     */
    void setTranslatedText(QString data);
    /** \brief returns the data of the current object
     *
     *  This function potentially translates internal to external.
     */
    QString translatedText() const;

    /** \brief set the directory in which to search for images. an empty argument switches this functio  off. */
    void setImagesDir(QString d);

protected:
    /** \brief filename of the file that is used to read and store the internal model
     *
     * Storing is disabled when this is set to an empty string (default).
     */
    QString m_filename;

    QString m_imagesDir;

    /** \brief translation map internal->external */
    QMap<QString, QString> translationInternalExternal;
    /** \brief translation map internal->external */
    QMap<QString, QString> translationExternalInternal;

    //virtual void focusOutEvent ( QFocusEvent * event ) ;
protected slots:
    /** \brief used to store the internal model contents into the file in m_filename */
    void modelDataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight );
    /** \brief used to store the internal model contents into the file in m_filename */
    void modelRowChanged ( const QModelIndex & parent, int start, int end );
private:
};

#endif // QMAPPABLECOMBOBOX_H
