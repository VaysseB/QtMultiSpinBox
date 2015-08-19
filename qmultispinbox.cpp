#include "qmultispinbox.h"

#include <QStyleOptionSpinBox>
#include <QDebug>


QMultiSpinBoxElement::QMultiSpinBoxElement()
{
}


QMultiSpinBoxElement::~QMultiSpinBoxElement()
{
}


//==============================================================================


QMultiSpinBoxData::QMultiSpinBoxData(QMultiSpinBoxElement* element,
                                     const QString& suffix) :
    element(element),
    suffix(suffix),
    startIndex(-1),
    text(element->defaultText())
{
}


void QMultiSpinBoxData::shiftRight(int offset)
{
    if (startIndex >= 0)
        startIndex += offset;
}


QString QMultiSpinBoxData::fullText() const
{
    return text + suffix;
}


int QMultiSpinBoxData::fullLength() const
{
    return text.length() + suffix.length();
}

//==============================================================================


QMultiSpinBox::QMultiSpinBox(QWidget *parent)
    : QWidget(*new QMultiSpinBoxPrivate, parent, 0)
{
    setObjectName(QLatin1String("QMultiSpinBox"));
}


QMultiSpinBox::QMultiSpinBox(QMultiSpinBoxPrivate &dd, QWidget* parent) :
    QWidget(dd, parent, 0)
{
    setObjectName(QLatin1String("QMultiSpinBox"));
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


void QMultiSpinBox::insertSpinElement(int index, QMultiSpinBoxElement* element, const QString &suffix)
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
    d->insert(index, element, suffix);

    updateGeometry();
    update();
    Q_EMIT elementCountChanged(elementCount());
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

    updateGeometry();
    update();
    Q_EMIT elementCountChanged(elementCount());

    return eData.data()->element;
}


QString QMultiSpinBox::text() const
{
    Q_D(const QMultiSpinBox);
    return d->text();
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
    update();
    Q_EMIT textAlignementChanged(align);
}


int QMultiSpinBox::currentSectionIndex() const
{
    Q_D(const QMultiSpinBox);
    return d->currentSectionIndex;
}

void QMultiSpinBox::setCurrentSectionIndex(int index)
{
    Q_D(QMultiSpinBox);
    if (index < 0 || index >= elementCount())
        d->currentSectionIndex = -1;
    else
        d->currentSectionIndex = index;
    update();
    Q_EMIT currentSectionIndexChanged(index);
}

QString QMultiSpinBox::prefix() const
{
    Q_D(const QMultiSpinBox);
    return d->prefix;
}

void QMultiSpinBox::setPrefix(const QString& prefix)
{
    Q_D(QMultiSpinBox);
    d->prefix = prefix;
    d->invalidateText();
    update();
}

QString QMultiSpinBox::suffixOf(int index) const
{
    Q_ASSERT(index >= 0 && index < elementCount());
    Q_D(const QMultiSpinBox);
    return d->elementDatas.value(index)->suffix;
}

void QMultiSpinBox::setSuffixOf(int index, const QString& suffix)
{
    Q_ASSERT(index >= 0 && index < elementCount());
    Q_D(QMultiSpinBox);
    d->elementDatas.value(index)->suffix = suffix;
    d->invalidateText();
    update();
}

//------------------------------------------------------------------------------
// display stuff


void QMultiSpinBox::paintEvent(QPaintEvent *paintEvent)
{
    Q_UNUSED(paintEvent);
    Q_D(QMultiSpinBox);

    // no ellide of text, all is visible

    QPainter painter(this);
    QFontMetrics fm(fontMetrics());

    QStyleOptionSpinBox option;
    option.initFrom(this);
    option.frame = true;
    option.buttonSymbols = QAbstractSpinBox::UpDownArrows;
    option.stepEnabled = QAbstractSpinBox::StepUpEnabled
            | QAbstractSpinBox::StepDownEnabled;

    // draw the spinbox frame and buttons
    style()->drawComplexControl(QStyle::CC_SpinBox, &option, &painter, this);

    // if nothing is selected
    if (d->currentSectionIndex < 0) {
        QSize sztext = fontMetrics().size(Qt::TextSingleLine, d->text());
        style()->drawItemText(&painter, align(sztext),
                              0, palette(), true,
                              d->text(), QPalette::Text);
    }
    else {
        QSize sztext = fontMetrics().size(Qt::TextSingleLine, d->text());
        QRect textRect = align(sztext);
        int index = 0;

        // prefix
        QSize s = fm.size(Qt::TextSingleLine, d->prefix);
        style()->drawItemText(&painter, textRect,
                              0, palette(), true,
                              d->prefix,
                              QPalette::Text);
        textRect.setLeft(textRect.left() + s.width());

        // draw each element
        foreach(QMultiSpinBoxData* eData, d->elementDatas) {
            // element content
            s = fm.size(Qt::TextSingleLine, eData->text);

            // if the element is selected
            if (d->currentSectionIndex == index++) {
                painter.fillRect(QRect(textRect.topLeft(), s),
                                 QBrush(palette().color(QPalette::Highlight)));
                style()->drawItemText(&painter, textRect,
                                      0, palette(), true,
                                      eData->text,
                                      QPalette::HighlightedText);
            }
            // if the element is not selected
            else {
                style()->drawItemText(&painter, textRect,
                                      0, palette(), true,
                                      eData->text,
                                      QPalette::Text);
            }

            textRect.setLeft(textRect.left() + s.width());


            // suffix
            s = fm.size(Qt::TextSingleLine, eData->suffix);

            style()->drawItemText(&painter, textRect,
                                  0, palette(), true,
                                  eData->suffix,
                                  QPalette::Text);

            textRect.setLeft(textRect.left() + s.width());
        }
    }
}


QRect QMultiSpinBox::align(const QSize& textSize) const
{
    Q_D(const QMultiSpinBox);

    // get available space (remove borders and buttons)
    QStyleOptionSpinBox option;
    option.initFrom(this);
    option.frame = true;
    option.buttonSymbols = QAbstractSpinBox::UpDownArrows;
    option.stepEnabled = QAbstractSpinBox::StepUpEnabled
            | QAbstractSpinBox::StepDownEnabled;

    QRect rSpace = style()->subControlRect(QStyle::CC_SpinBox,
                                           &option,
                                           QStyle::SC_SpinBoxEditField,
                                           this);

    // align text
    QPoint p(0, 0);
    QSize s(textSize);

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

    s.rwidth() += 1; // for loss of precision

    return QRect(p, s);
}


//------------------------------------------------------------------------------
// size stuff


QSize QMultiSpinBox::sizeHint() const
{
    QSize size = minimumSizeHint();
    size.rwidth() += 10;
    size.rheight() += 1;
    return size;
}


QSize QMultiSpinBox::minimumSizeHint() const
{
    Q_D(const QMultiSpinBox);

    QSize s = fontMetrics().size(Qt::TextSingleLine, d->text());

    QStyleOptionSpinBox option;
    option.initFrom(this);
    option.frame = true;
    option.buttonSymbols = QAbstractSpinBox::UpDownArrows;
    option.stepEnabled = QAbstractSpinBox::StepUpEnabled
            | QAbstractSpinBox::StepDownEnabled;
    QSize buttonSize = style()->subControlRect(QStyle::CC_SpinBox,
                                               &option,
                                               QStyle::SC_SpinBoxDown,
                                               this).size();

    s.rwidth() += buttonSize.width();
    s.rheight() = qMax(s.height(), buttonSize.height() * 2);

    s.rwidth() += 4; // abstract margins
    s.rheight() += 2; // abstract margins

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
    currentSectionIndex = -1;
    cachedText.resize(0);
    prefix.resize(0);
    qDeleteAll(elementDatas);
    elementDatas.clear();
}


void QMultiSpinBoxPrivate::reset()
{
}


void QMultiSpinBoxPrivate::insert(int index, QMultiSpinBoxElement* element, const QString &suffix)
{
    QMultiSpinBoxData* newElement = new QMultiSpinBoxData(element, suffix);
    // index is valid, element not null

    elementDatas.insert(index, newElement);

    newElement->startIndex = textLength(index);

    for (int i=index+1; i<elementDatas.count(); i++)
        elementDatas.value(i)->shiftRight(newElement->fullLength());

    invalidateText();
}


QMultiSpinBoxData* QMultiSpinBoxPrivate::take(int index)
{
    // index is valid, element exist
    QMultiSpinBoxData* takenElementData = elementDatas.takeAt(index);

    for (int i=index; i<elementDatas.count(); i++)
        elementDatas.value(i)->shiftLeft(takenElementData->fullLength());

    invalidateText();

    return takenElementData;
}

void QMultiSpinBoxPrivate::invalidateText()
{
    cachedText.resize(0);
    cachedText.append(prefix);
    foreach(QMultiSpinBoxData* eData, elementDatas)
        cachedText.append(eData->fullText());
}

int QMultiSpinBoxPrivate::textLength(int count) const
{
    int length = 0;
    foreach(QMultiSpinBoxData* eData, elementDatas) {
        if (count-- <= 0)
            break;
        length += eData->fullLength();
    }
    return length;
}
