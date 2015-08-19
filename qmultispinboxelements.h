#ifndef QMULTISPINBOXELEMENTS_H
#define QMULTISPINBOXELEMENTS_H

#include <QMultiSpinBox>



class QMultiSpinBoxBinaryElement : public QMultiSpinBoxElement
{
public:
    QMultiSpinBoxBinaryElement(int length = 4) {
        while (--length >= 0)
            defText.append('0');
    }

    int minimumInputLength() const { return defText.length(); }
    int maximumInputLength() const { return defText.length(); }

    QString defaultText() const { return defText; }
    bool acceptableChar(const QChar& ch, int) const { return ch == QChar('0') || ch == QChar('1'); }

    bool extractValue(const QString& text, QVariant& value) const;
    bool displayValue(const QVariant& value, QString& text) const;

private:
    QString defText;
};

#endif // QMULTISPINBOXELEMENTS_H
