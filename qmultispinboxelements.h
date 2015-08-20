#ifndef QMULTISPINBOXELEMENTS_H
#define QMULTISPINBOXELEMENTS_H

#include <QString>
#include <QVariant>


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


//==============================================================================


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
