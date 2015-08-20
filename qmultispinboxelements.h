#ifndef QMULTISPINBOXELEMENTS_H
#define QMULTISPINBOXELEMENTS_H

#include <QString>
#include <QVariant>
#include <QValidator>


class QMultiSpinBoxElement :
        public QValidator
{
    Q_OBJECT
public:
    explicit QMultiSpinBoxElement(QObject *parent = 0);
    virtual ~QMultiSpinBoxElement();

    virtual QVariant defaultValue() const = 0;
    virtual bool extractValue(const QString& text, QVariant& value) const = 0; // true if value is extractable
    virtual bool convertString(const QVariant& value, QString& text) const = 0; // true if value is displayable
};


//==============================================================================


class QMultiSpinBoxBinaryElement : public QMultiSpinBoxElement
{
public:
    QVariant defaultValue() const { return QVariant(10); }
    bool extractValue(const QString& text, QVariant& value) const;
    bool convertString(const QVariant& value, QString& text) const;
    QValidator::State validate(QString &text, int &pos) const;

private:
    QString defText;
};

#endif // QMULTISPINBOXELEMENTS_H
