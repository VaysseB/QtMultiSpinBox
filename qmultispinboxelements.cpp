#include "qmultispinboxelements.h"


bool QMultiSpinBoxBinaryElement::extractValue(const QString& text, QVariant& value) const
{
    bool ok = true;
    value = text.toInt(&ok, 2);
    return ok;
}

bool QMultiSpinBoxBinaryElement::displayValue(const QVariant& value, QString& text) const
{
    bool ok = true;
    int iv = value.toInt(&ok);
    if (ok)
        text = QString::number(iv, 2);
    return ok;
}
