#include "qtmultispinboxelements.h"

QtIntMultiSpinBoxElement::QtIntMultiSpinBoxElement(QObject * parent) :
    QtMultiSpinBoxValidatorWrapper(parent)
{
}

QtIntMultiSpinBoxElement::QtIntMultiSpinBoxElement(int bottom, int top, QObject *parent) :
    QtMultiSpinBoxValidatorWrapper(parent)
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
        return QVariant(v+steps);
    return QVariant();
}
