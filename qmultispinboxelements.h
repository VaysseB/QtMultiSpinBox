#ifndef QMULTISPINBOXELEMENTS_H
#define QMULTISPINBOXELEMENTS_H

#include <QMultiSpinBox>



class QMultiSpinBoxBinaryElement : public QMultiSpinBoxElement
{
public:
    int minimumInputLength() const { return 8; }
    int maximumInputLength() const { return 8; }
    QString prefix() const { return QString("0b"); }

    QString defaultText() const { return QString("00000000"); }
    bool acceptableChar(const QChar& ch) const { return ch.isDigit(); }

    bool extractValue(const QString& text, QVariant& value) const;
    bool displayValue(const QVariant& value, QString& text) const;
};

#endif // QMULTISPINBOXELEMENTS_H
