#ifndef QtMultiSpinBox_H
#define QtMultiSpinBox_H

#include <QList>
#include <QWidget>
#include <QAbstractSpinBox>

// /usr/include/qt5/QtWidgets/5.2.1/QtWidgets/private/qwidget_p.h
#include <private/qabstractspinbox_p.h>

#ifdef QT_NO_VALIDATOR
#error QtMultiSpinBox needs validator
#endif



QT_BEGIN_NAMESPACE

class QtMultiSpinBoxElement;




class QtMultiSpinBoxData
{
public:
    QtMultiSpinBoxData(QtMultiSpinBoxElement* element, const QString &suffix);

    QtMultiSpinBoxElement* element;
    QString suffix;
};



class QtMultiSpinBoxPrivate;

class QtMultiSpinBox : public QAbstractSpinBox
{
    Q_OBJECT
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(int currentSectionIndex READ currentSectionIndex WRITE setCurrentSectionIndex NOTIFY currentSectionIndexChanged)
    Q_PROPERTY(QString prefix READ prefix WRITE setPrefix)


public:
    explicit QtMultiSpinBox(QWidget *parent = 0);
    virtual ~QtMultiSpinBox();



    void clear(); // reset and remove all elements
    QString text() const;



    bool isEmpty() const;
    int count() const;
    int currentSectionIndex() const;
    QString prefix() const;
    QString suffix(int index) const; // between (index, index+1)
    QVariant value(int index) const;
    QString text(int index) const;


    StepEnabled stepEnabled() const;
    void stepBy(int steps);



public Q_SLOTS:
    void appendSpinElement(QtMultiSpinBoxElement* element, const QString &suffix = QString());
    void insertSpinElement(int index, QtMultiSpinBoxElement* element, const QString &suffix = QString());
    QtMultiSpinBoxElement* getSpinElement(int index);
    QtMultiSpinBoxElement* takeSpinElement(int index);
    void removeSpinElement(int index);


    void setCurrentSectionIndex(int index); // if not valid, set -1
    void setPrefix(const QString& prefix);
    void setSuffix(int index, const QString& suffix);
    void setValue(int index, const QVariant& sectionValue);
    void setText(int index, const QString& sectionText);




Q_SIGNALS:
    void currentSectionIndexChanged(int index);
    void editingFinished(int index);


protected:
    QtMultiSpinBox(QAbstractSpinBoxPrivate &dd, QWidget *parent = 0);

    void focusInEvent(QFocusEvent* event);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_sectionEditingFinished())
    Q_PRIVATE_SLOT(d_func(), void _q_cursorPositionChanged(int,int))


private:
    Q_DISABLE_COPY(QtMultiSpinBox)
    Q_DECLARE_PRIVATE(QtMultiSpinBox)

    QScopedPointer<QtMultiSpinBoxPrivate> d_ptr;
};





class QtMultiSpinBoxPrivate : public QValidator
{
    Q_DECLARE_PUBLIC(QtMultiSpinBox)

public:
    QtMultiSpinBoxPrivate(QtMultiSpinBox* s);
    virtual ~QtMultiSpinBoxPrivate();


    void clear();


    // simplify whitespace but do not trimmed
    QString simplify(const QString& text) const;


    void insert(int index, QtMultiSpinBoxElement* element, const QString &suffix);
    QtMultiSpinBoxData* take(int index);
    QtMultiSpinBoxData* get(int index) const;


    // slots
    void _q_sectionEditingFinished();
    void _q_cursorPositionChanged(int old,int new_);


    int textIndex(const QString &text, int indexElement) const;
    bool checkAndSplit(const QString &input, QList<QStringRef> &result) const; // chunk of text (no prefix no suffix)
    QValidator::State validate(QString &text, int &pos) const;
    void fixup(QString &) const;

    QString textAt(const QString& input, int index) const;
    QString setTextAt(const QString& input, int index, const QString &text) const;


    void changeText(QLineEdit* edit, const QString& text) const;



public:
    int currentSectionIndex;
    QString prefix;
    QList<QtMultiSpinBoxData*> elementDatas;

    QtMultiSpinBox* q_ptr;
};


//==============================================================================


inline void QtMultiSpinBox::appendSpinElement(QtMultiSpinBoxElement* element, const QString &suffix)
{
    insertSpinElement(count(), element, suffix);
}

inline bool QtMultiSpinBox::isEmpty() const
{
    return count() <= 0;
}

inline QString QtMultiSpinBox::text() const
{
    return QAbstractSpinBox::text();
}

QT_END_NAMESPACE

#endif // QtMultiSpinBox_H
