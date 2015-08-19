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

    bool changeCSI = (d->currentSectionIndex == index);
    if (changeCSI)
        d->currentSectionIndex = -1;

    updateGeometry();

    Q_EMIT elementCountChanged(elementCount());
    if (changeCSI)
        Q_EMIT currentSectionIndexChanged(d->currentSectionIndex);

    update();

    return eData.data()->element;
}


QMultiSpinBoxElement* QMultiSpinBox::getSpinElement(int index)
{
    Q_ASSERT(index >= 0 && index < elementCount());

    Q_D(QMultiSpinBox);
    QScopedPointer<QMultiSpinBoxData> eData(d->get(index));

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

    QRect emptySpace = style()->subControlRect(QStyle::CC_SpinBox,
                                               &option,
                                               QStyle::SC_SpinBoxEditField,
                                               this);

    // *** Draw ***
    // draw the spinbox frame and buttons
    style()->drawComplexControl(QStyle::CC_SpinBox, &option, &painter, this);

    // if nothing is selected AND cursor is away
    if (d->currentSectionIndex < 0) {
//            && d->cursoPos < 0) {
        QSize textSize = fm.size(Qt::TextSingleLine, d->text());
        QRect textRect = QStyle::alignedRect(Qt::LeftToRight, d->textAlign, textSize, emptySpace);
        style()->drawItemText(&painter, textRect,
                              0, palette(), true,
                              d->text(), QPalette::Text);
    }
    else {
        QSize textSize = fm.size(Qt::TextSingleLine, d->text());
        QRect textRect = QStyle::alignedRect(Qt::LeftToRight, d->textAlign, textSize, emptySpace);

        // split into 3 part (by elements)
        QMultiSpinBoxData* dataSelected = d->get(d->currentSectionIndex);
        int startIndexSplit = d->textLength(d->currentSectionIndex);
        QString prePart = d->cachedText.mid(0, startIndexSplit);
        QString activePart = d->cachedText.mid(startIndexSplit, dataSelected->text.length());
        QString postPart = d->cachedText.mid(startIndexSplit + dataSelected->text.length());

        // if cursor is in selection
        if (d->cursoPos >= 0) {

        }
        // if the all section is selected
        else {
            // pre-part
            textSize = fm.size(Qt::TextSingleLine, prePart);
            style()->drawItemText(&painter, textRect,
                                  0, palette(), true,
                                  prePart,
                                  QPalette::Text);
            textRect.setLeft(textRect.left() + textSize.width());

            // active part
            // draw background of hilghlight text
            textSize = fm.size(Qt::TextSingleLine, activePart);
            painter.fillRect(QRect(textRect.topLeft(), textSize),
                             QBrush(palette().color(QPalette::Highlight)));
            // draw hilghlight text
            style()->drawItemText(&painter, textRect,
                                  0, palette(), true,
                                  activePart,
                                  QPalette::HighlightedText);
            textRect.setLeft(textRect.left() + textSize.width());

            // post-part
            textSize = fm.size(Qt::TextSingleLine, postPart);
            style()->drawItemText(&painter, textRect,
                                  0, palette(), true,
                                  postPart,
                                  QPalette::Text);
        }
    }
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
    cursoPos = -1;
    currentSectionIndex = -1;
    cachedText.resize(0);
    prefix.resize(0);
    qDeleteAll(elementDatas);
    elementDatas.clear();
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


QMultiSpinBoxData* QMultiSpinBoxPrivate::get(int index)
{
    // index is valid, element exist
    return elementDatas.value(index);
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
    if (count >= 0)
        length += prefix.length();
    foreach(QMultiSpinBoxData* eData, elementDatas) {
        if (count-- <= 0)
            break;
        length += eData->fullLength();
    }
    return length;
}
