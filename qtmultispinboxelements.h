#ifndef QTMULTISPINBOXELEMENTS_H
#define QTMULTISPINBOXELEMENTS_H

#include <QString>
#include <QVariant>
#include <QValidator>

#include <QIntValidator>


class QtMultiSpinBoxElement
{
public:
    explicit QtMultiSpinBoxElement() {}
    virtual ~QtMultiSpinBoxElement() {}

    virtual QVariant defaultValue() const = 0;
    virtual QVariant valueFromText(const QString &text) const = 0;
    virtual QString textFromValue(const QVariant &value) const = 0;
    virtual QVariant stepBy(const QVariant &value, int steps) = 0;

    virtual QValidator::State validate(QString &, int &) const = 0;
    virtual void fixup(QString &) const {}
};


//------------------------------------------------------------------------------
template <typename V> // QIntValidator
class QtMultiSpinBoxValidatorWrapper :
        public V,
        public QtMultiSpinBoxElement
{
public:
    explicit QtMultiSpinBoxValidatorWrapper(QObject * parent = 0) : V(parent) {}

    QValidator::State validate(QString &text, int &pos) const { return V::validate(text, pos); }
    void fixup(QString &text) const { V::fixup(text); }
};


//------------------------------------------------------------------------------


class QtIntMultiSpinBoxElement :
        public QtMultiSpinBoxValidatorWrapper<QIntValidator>
{
    Q_OBJECT
    Q_PROPERTY(int stepIncrement READ stepIncrement WRITE setStepIncrement)

public:
    explicit QtIntMultiSpinBoxElement(QObject * parent = 0);
    QtIntMultiSpinBoxElement(int bottom, int top, QObject *parent = 0);

    QVariant defaultValue() const { return QVariant((int)0); }
    QVariant valueFromText(const QString &text) const;
    QString textFromValue(const QVariant &value) const;
    QVariant stepBy(const QVariant &value, int steps);

    void setStepIncrement(int incr) { m_stepIncr = incr; }
    int stepIncrement() const { return m_stepIncr; }

private:
    int m_stepIncr;
};


class QtDoubleMultiSpinBoxElement :
        public QtMultiSpinBoxValidatorWrapper<QDoubleValidator>
{
    Q_OBJECT
    Q_PROPERTY(int stepIncrement READ stepIncrement WRITE setStepIncrement)

public:
    explicit QtDoubleMultiSpinBoxElement(QObject* parent = 0);
    QtDoubleMultiSpinBoxElement(double bottom, double top, int decimals, QObject *parent = 0);

    QVariant defaultValue() const { return QVariant((double)0.0); }
    QVariant valueFromText(const QString &text) const;
    QString textFromValue(const QVariant &value) const;
    QVariant stepBy(const QVariant &value, int steps);

    void setStepIncrement(double incr) { m_stepIncr = incr; }
    double stepIncrement() const { return m_stepIncr; }

private:
    double m_stepIncr;
};


#endif // QTMULTISPINBOXELEMENTS_H
