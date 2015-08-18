#include "qmultispinbox.h"

#include <QStyleOptionSpinBox>


QMultiSpinBoxElement::QMultiSpinBoxElement()
{
}


QMultiSpinBoxElement::~QMultiSpinBoxElement()
{
}


//==============================================================================


QMultiSpinBoxData::QMultiSpinBoxData(QMultiSpinBoxElement* element) :
    element(element),
    firstIndex(-1),
    lastIndex(-1)
{
}


void QMultiSpinBoxData::shiftRight(int offset)
{
    if (firstIndex >= 0) {
        firstIndex += offset;
        lastIndex += offset;
    }
}


//==============================================================================


QMultiSpinBox::QMultiSpinBox(QWidget *parent)
    : QWidget(*new QMultiSpinBoxPrivate, parent, 0)
{
}


QMultiSpinBox::QMultiSpinBox(QMultiSpinBoxPrivate &dd, QWidget* parent) :
    QWidget(dd, parent, 0)
{
}


QMultiSpinBox::~QMultiSpinBox()
{
}


void QMultiSpinBox::clear()
{
    Q_D(QMultiSpinBox);
    d->clear();
}


void QMultiSpinBox::reset()
{
    Q_D(QMultiSpinBox);
    d->reset();
}


void QMultiSpinBox::insertSpinElement(int index, QMultiSpinBoxElement* element)
{
    Q_ASSERT(index >= 0 && index <= elementCount());
    Q_ASSERT(element != NULL);

    Q_ASSERT(element->minimumInputLength() <= element->maximumInputLength());

    const QString defaultText = element->defaultText();
    Q_ASSERT(defaultText.length() <= element->maximumInputLength());
    Q_ASSERT(defaultText.length() >= element->minimumInputLength());

    Q_ASSERT_X(!element->acceptableChar(QChar(QChar::Null)),
               "QMultiSpinBox::insertSpinElement",
               "null character is not acceptable");
    Q_ASSERT_X(!element->acceptableChar(QChar(QChar::Tabulation)),
               "QMultiSpinBox::insertSpinElement",
               "tabulation character is not acceptable");
    Q_ASSERT_X(!element->acceptableChar(QChar(QChar::LineFeed)),
               "QMultiSpinBox::insertSpinElement",
               "line feed character is not acceptable");
    Q_ASSERT_X(!element->acceptableChar(QChar(QChar::CarriageReturn)),
               "QMultiSpinBox::insertSpinElement",
               "carriage return character is not acceptable");
    Q_ASSERT_X(!element->acceptableChar(QChar(QChar::Space)),
               "QMultiSpinBox::insertSpinElement",
               "space character is not acceptable");
    Q_ASSERT_X(!element->acceptableChar(QChar(QChar::Nbsp)),
               "QMultiSpinBox::insertSpinElement",
               "non-breaking space character is not acceptable");

    Q_D(QMultiSpinBox);
    d->insert(index, element);
}


void QMultiSpinBox::removeSpinElement(int index)
{
    delete takeSpinElement(index);
}


QMultiSpinBoxElement* QMultiSpinBox::takeSpinElement(int index)
{
    Q_ASSERT(index >= 0 && index < elementCount());

    Q_D(QMultiSpinBox);
    QScopedPointer<QMultiSpinBoxData> eData(d->take(index));
    return eData.data()->element;
}


QString QMultiSpinBox::text() const
{
    Q_D(const QMultiSpinBox);
    return d->text;
}


int QMultiSpinBox::elementCount() const
{
    Q_D(const QMultiSpinBox);
    return d->elementDatas.count();
}


//-----------------------------------------------------------------------------
// property stuff


Qt::Alignment QMultiSpinBox::textAlignement() const
{
    Q_D(const QMultiSpinBox);
    return d->textAlign;
}

void QMultiSpinBox::setTextAlignement(Qt::Alignment align)
{
    Q_D(QMultiSpinBox);
    d->textAlign = align;
    Q_EMIT textAlignementChanged(align);
}

//------------------------------------------------------------------------------
// display stuff


void QMultiSpinBox::paintEvent(QPaintEvent *paintEvent)
{
    Q_UNUSED(paintEvent);
    Q_D(QMultiSpinBox);

    // no ellide of text, all is visible

    QPainter painter(this);

    QStyleOptionSpinBox option;
    option.initFrom(this);
    option.frame = true;
    option.buttonSymbols = QAbstractSpinBox::UpDownArrows;
    option.stepEnabled = QAbstractSpinBox::StepUpEnabled
            | QAbstractSpinBox::StepDownEnabled;

    style()->drawComplexControl(QStyle::CC_SpinBox, &option, &painter, this);

    style()->drawItemText(&painter, internalTextRect(),
                          0, palette(), true,
                          d->text, QPalette::Text);
}


QMargins QMultiSpinBox::margins()
{
    return QMargins(2, 2, 2, 2);
}

int QMultiSpinBox::buttonColumnWidth() const
{
    return 12;
}

QRect QMultiSpinBox::internalFrameRect() const
{
    // get available space (remove borders)
    QRect r = QRect(QPoint(0, 0), size());

    QStyleOptionSpinBox option;
    option.initFrom(this);
    option.frame = true;
    option.buttonSymbols = QAbstractSpinBox::UpDownArrows;
    option.stepEnabled = QAbstractSpinBox::StepUpEnabled
            | QAbstractSpinBox::StepDownEnabled;

    r.setWidth(r.width()
               - style()->pixelMetric(QStyle::PM_SpinBoxFrameWidth, &option, this) * 2
               - buttonColumnWidth());

    return r.marginsRemoved(margins());
}


QRect QMultiSpinBox::internalTextRect() const
{
    Q_D(const QMultiSpinBox);
    QRect rSpace = internalFrameRect();
    QPoint p(0, 0);
    QSize s(fontMetrics().size(Qt::TextSingleLine, d->text));

    // horizontal: left
    s.rwidth() = qMin(s.width(), rSpace.width());
    if (d->textAlign.testFlag(Qt::AlignLeft))
        p.rx() = rSpace.x();
    // horizontal: right
    else if (d->textAlign.testFlag(Qt::AlignRight))
        p.rx() = rSpace.x() + (rSpace.width() - s.width());
    // horizontal: center
    else if (d->textAlign.testFlag(Qt::AlignHCenter))
        p.rx() = rSpace.x() + (rSpace.width() - s.width()) * 0.5;

    // vertical: up
    s.rheight() = qMin(s.height(), rSpace.height());
    if (d->textAlign.testFlag(Qt::AlignTop))
        p.ry() = rSpace.y();
    // vertical: down
    else if (d->textAlign.testFlag(Qt::AlignBottom))
        p.ry() = rSpace.y() + (rSpace.height() - s.height());
    // vertical: center
    else if (d->textAlign.testFlag(Qt::AlignVCenter))
        p.ry() = rSpace.y() + (rSpace.height() - s.height()) * 0.5;

    return QRect(p, s);
}

//------------------------------------------------------------------------------
// size stuff


QSize QMultiSpinBox::sizeHint() const
{
    QSize size = minimumSizeHint();
    size.rwidth() += 20;
    size.rheight() += 6;
    return size;
}


QSize QMultiSpinBox::minimumSizeHint() const
{
    Q_D(const QMultiSpinBox);

    QSize s = fontMetrics().size(Qt::TextSingleLine, d->text);
    s.rheight() += margins().top() + margins().bottom();
    s.rwidth() += margins().left() + margins().right();

    QStyleOptionSpinBox option;
    option.initFrom(this);
    option.frame = true;
    option.buttonSymbols = QAbstractSpinBox::UpDownArrows;
    option.stepEnabled = QAbstractSpinBox::StepUpEnabled
            | QAbstractSpinBox::StepDownEnabled;

    s.rwidth() += style()->pixelMetric(QStyle::PM_SpinBoxFrameWidth, &option, this) * 2;
    s.rwidth() += buttonColumnWidth(); // left buttons

    return s;
}


//==============================================================================


QMultiSpinBoxPrivate::QMultiSpinBoxPrivate() :
    textAlign(Qt::AlignCenter)
{
    clear();
}


QMultiSpinBoxPrivate::~QMultiSpinBoxPrivate()
{
    clear();
}


void QMultiSpinBoxPrivate::clear()
{
    qDeleteAll(elementDatas);
    text.resize(0);
}


void QMultiSpinBoxPrivate::reset()
{
    text.resize(0);
}


void QMultiSpinBoxPrivate::insert(int index, QMultiSpinBoxElement* element)
{
    QMultiSpinBoxData* newElement = new QMultiSpinBoxData(element);
    // index is valid, element not null

    elementDatas.insert(index, newElement);

    QString finalText = element->prefix()
            + element->defaultText()
            + element->suffix();
    text.insert(index, finalText);
    newElement->firstIndex = index;
    newElement->lastIndex = index + finalText.count();

    for (int i=index+1; i<elementDatas.count(); i++)
        elementDatas.value(i)->shiftRight(newElement->size());
}


QMultiSpinBoxData* QMultiSpinBoxPrivate::take(int index)
{
    // index is valid, element exist
    QMultiSpinBoxData* takenElementData = elementDatas.takeAt(index);

    for (int i=index; i<elementDatas.count(); i++)
        elementDatas.value(i)->shiftLeft(takenElementData->size());

    text.remove(index, takenElementData->size());

    return takenElementData;
}
