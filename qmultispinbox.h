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
    virtual QString prefix() const { return QString(); }
    virtual QString suffix() const { return QString(); }

    virtual QString defaultText() const = 0; // return string (min <= len <= max)
    virtual bool acceptableChar(const QChar& ch) const = 0; // exclude prefix and suffix

    virtual bool extractValue(const QString& text, QVariant& value) const = 0; // true if value is extractable
    virtual bool displayValue(const QVariant& value, QString& text) const = 0; // true if value is displayable
};




class QMultiSpinBoxData
{
public:
    QMultiSpinBoxData(QMultiSpinBoxElement* element);

    int size() const { return lastIndex - firstIndex + prefixLength + suffixLength; } // with prefix + suffix
    void shiftLeft(int offset) { shiftLeft(-1 * offset); }
    void shiftRight(int offset);

    QString fullText() const;

    QMultiSpinBoxElement* element;
    int prefixLength, suffixLength;
    QString prefix, suffix;

    int firstIndex, lastIndex; // without prefix nor suffix

    QString text;
};



class QMultiSpinBoxPrivate;

class QMultiSpinBox : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Qt::Alignment textAlignement READ textAlignement WRITE setTextAlignement NOTIFY textAlignementChanged)


public:
    explicit QMultiSpinBox(QWidget *parent = 0);
    virtual ~QMultiSpinBox();



    void reset(); // reset to default state
    void clear(); // reset and remove all elements



    QString text() const;
    int elementCount() const;
    Qt::Alignment textAlignement() const;


    // drawing stuff
    void paintEvent(QPaintEvent *paintEvent);



    // size stuff
    QSize sizeHint() const;
    QSize minimumSizeHint() const;



public slots:
    void insertSpinElement(int index, QMultiSpinBoxElement* element);
    QMultiSpinBoxElement* takeSpinElement(int index);
    void removeSpinElement(int index);


    void setTextAlignement(Qt::Alignment align);


signals:
    void textAlignementChanged(Qt::Alignment align);


protected:
    QMultiSpinBox(QMultiSpinBoxPrivate &dd, QWidget* parent = 0);


private:
    QRect internalFrameRect() const; // get available space (remove borders)
    QRect internalTextRect() const; // exact rectangle of the text content
    int buttonColumnWidth() const;

    static QMargins margins();


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


    void insert(int index, QMultiSpinBoxElement* element);
    QMultiSpinBoxData* take(int index);


    int textLength() const { return cachedText.length(); }
    QString text() const { return cachedText; }
    void invalidateText();

public:
    QString cachedText;

    QList<QMultiSpinBoxData*> elementDatas;
    Qt::Alignment textAlign;
};

QT_END_NAMESPACE

#endif // QMULTISPINBOX_H
