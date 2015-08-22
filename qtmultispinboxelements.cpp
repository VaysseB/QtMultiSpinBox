#include "qtmultispinboxelements.h"

#include <QDebug>


QtIntMultiSpinBoxElement::QtIntMultiSpinBoxElement(QObject * parent) :
    QtMultiSpinBoxValidatorWrapper(parent),
    m_stepIncr(1)
{
}

QtIntMultiSpinBoxElement::QtIntMultiSpinBoxElement(int bottom, int top, QObject *parent) :
    QtMultiSpinBoxValidatorWrapper(parent),
    m_stepIncr(1)
{
    setRange(bottom, top);
}

QVariant QtIntMultiSpinBoxElement::valueFromText(const QString &text) const
{
    bool ok = true;
    int v = text.toInt(&ok);
    if (ok)
        return QVariant(v);
    return QVariant();
}

QString QtIntMultiSpinBoxElement::textFromValue(const QVariant &value) const
{
    bool ok = true;
    int v = value.toInt(&ok);
    if (ok)
        return QString::number(v);
    return QString();
}

QVariant QtIntMultiSpinBoxElement::stepBy(const QVariant &value, int steps)
{
    bool ok = true;
    int v = value.toInt(&ok);
    if (ok)
        return QVariant(v + steps * m_stepIncr);
    return QVariant();
}

//------------------------------------------------------------------------------

QtDoubleMultiSpinBoxElement::QtDoubleMultiSpinBoxElement(QObject * parent) :
    QtMultiSpinBoxValidatorWrapper(parent),
    m_stepIncr(1)
{
}

QtDoubleMultiSpinBoxElement::QtDoubleMultiSpinBoxElement(double bottom, double top, int decimals, QObject *parent) :
    QtMultiSpinBoxValidatorWrapper(parent),
    m_stepIncr(1)
{
    setRange(bottom, top);
    setDecimals(decimals);
}

QVariant QtDoubleMultiSpinBoxElement::valueFromText(const QString &text) const
{
    bool ok = true;
    QString s = QString(text).replace(",", ".");
    double v = s.toDouble(&ok); // require dot instead of comma (don't ask why)
    if (ok)
        return QVariant(v);
    return QVariant();
}

QString QtDoubleMultiSpinBoxElement::textFromValue(const QVariant &value) const
{
    bool ok = true;
    double v = value.toDouble(&ok);
    if (ok) {
        QString s = QString::number(v);
        // remove any doubles zeros at the end, don't remove the last one
        while (s.endsWith("00"))
            s.remove(s.length()-2, 1);
        s.replace(".", ","); // later, require comma instead of point (don't ask why)
        s.remove(",0");
        return s;
    }
    return QString();
}

QVariant QtDoubleMultiSpinBoxElement::stepBy(const QVariant &value, int steps)
{
    bool ok = true;
    double v = value.toDouble(&ok);
    if (ok)
        return QVariant(v + (double)steps * m_stepIncr);
    return QVariant();
}
