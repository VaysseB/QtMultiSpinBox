#include "qmultispinboxelements.h"



QMultiSpinBoxElement::QMultiSpinBoxElement(QObject* parent) :
    QValidator(parent)
{
}


QMultiSpinBoxElement::~QMultiSpinBoxElement()
{
}


//==============================================================================


bool QMultiSpinBoxBinaryElement::extractValue(const QString& text, QVariant& value) const
{
    bool ok = true;
    value = text.toInt(&ok, 2);
    return ok;
}

bool QMultiSpinBoxBinaryElement::convertString(const QVariant& value, QString& text) const
{
    bool ok = true;
    int iv = value.toInt(&ok);
    if (ok)
        text = QString::number(iv, 2);
    return ok;
}

QValidator::State QMultiSpinBoxBinaryElement::validate(QString &text, int &pos) const
{
    Q_UNUSED(pos)
    bool ok = true;
    text.toInt(&ok);
    if (ok)
        return QValidator::Acceptable;
    return QValidator::Invalid;
}
