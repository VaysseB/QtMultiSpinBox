#ifndef QMULTISPINBOX_H
#define QMULTISPINBOX_H

#include <QList>
#include <QWidget>
#include <QMargins>

// /usr/include/qt5/QtWidgets/5.2.1/QtWidgets/private/qwidget_p.h
#include <private/qwidget_p.h>
#include <private/qwidget_p.h>


QT_BEGIN_NAMESPACE


class QMultiSpinBoxElement
{
public:
    explicit QMultiSpinBoxElement();
    virtual ~QMultiSpinBoxElement();

    virtual int minimumInputLength() const = 0; // exclude prefix and suffix
    virtual int maximumInputLength() const = 0;

    virtual QString defaultText() const = 0; // return string (min <= len <= max)
    virtual bool acceptableChar(const QChar& ch, int pos = -1) const = 0; // exclude prefix and suffix (-1 for general)

    virtual bool extractValue(const QString& text, QVariant& value) const = 0; // true if value is extractable
    virtual bool displayValue(const QVariant& value, QString& text) const = 0; // true if value is displayable
};




class QMultiSpinBoxData
{
public:
    QMultiSpinBoxData(QMultiSpinBoxElement* element, const QString &suffix);

    void shiftLeft(int offset) { shiftLeft(-1 * offset); }
    void shiftRight(int offset);

    int fullLength() const;
    QString fullText() const;

    QMultiSpinBoxElement* element;
    QString suffix;

    int startIndex; // without prefix nor suffix

    QString text;
};



class QMultiSpinBoxPrivate;

class QMultiSpinBox : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Qt::Alignment textAlignement READ textAlignement WRITE setTextAlignement NOTIFY textAlignementChanged)
    Q_PROPERTY(int elementCount READ elementCount NOTIFY elementCountChanged)
    Q_PROPERTY(int currentSectionIndex READ currentSectionIndex WRITE setCurrentSectionIndex NOTIFY currentSectionIndexChanged)
    Q_PROPERTY(QString prefix READ prefix WRITE setPrefix)


public:
    explicit QMultiSpinBox(QWidget *parent = 0);
    virtual ~QMultiSpinBox();



    void reset(); // reset to default state
    void clear(); // reset and remove all elements



    QString text() const;
    int elementCount() const;
    Qt::Alignment textAlignement() const;
    int currentSectionIndex() const;
    QString prefix() const;
    QString suffixOf(int index) const; // between (index, index+1)


    // drawing stuff
    void paintEvent(QPaintEvent *paintEvent);



    // size stuff
    QSize sizeHint() const;
    QSize minimumSizeHint() const;



public slots:
    void appendSpinElement(QMultiSpinBoxElement* element, const QString &suffix = QString(" "));
    void insertSpinElement(int index, QMultiSpinBoxElement* element, const QString &suffix = QString(" "));
    QMultiSpinBoxElement* getSpinElement(int index);
    QMultiSpinBoxElement* takeSpinElement(int index);
    void removeSpinElement(int index);


    void setTextAlignement(Qt::Alignment align);
    void setCurrentSectionIndex(int index); // if not valid, set -1
    void setPrefix(const QString& prefix);
    void setSuffixOf(int index, const QString& suffix);



signals:
    void textAlignementChanged(Qt::Alignment align);
    void elementCountChanged(int count);
    void currentSectionIndexChanged(int index);


protected:
    QMultiSpinBox(QMultiSpinBoxPrivate &dd, QWidget* parent = 0);


private:
    Q_DISABLE_COPY(QMultiSpinBox)
    Q_DECLARE_PRIVATE(QMultiSpinBox)
};





class QMultiSpinBoxPrivate : public QWidgetPrivate
{
public:
    QMultiSpinBoxPrivate();
    virtual ~QMultiSpinBoxPrivate();


    void clear();
    void reset();


    void insert(int index, QMultiSpinBoxElement* element, const QString &suffix);
    QMultiSpinBoxData* take(int index);
    QMultiSpinBoxData* get(int index);


    int textLength() const { return cachedText.length(); }
    QString text() const { return cachedText; }
    void invalidateText();

    int textLength(int count) const;

public:
    QString cachedText;
    Qt::Alignment textAlign;
    int currentSectionIndex;

    QString prefix;
    QList<QMultiSpinBoxData*> elementDatas;
};


//==============================================================================


inline void QMultiSpinBox::appendSpinElement(QMultiSpinBoxElement* element, const QString &suffix)
{
    insertSpinElement(elementCount(), element, suffix);
}

QT_END_NAMESPACE

#endif // QMULTISPINBOX_H
