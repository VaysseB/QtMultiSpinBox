#include "qmultispinbox.h"

#include <QStyleOptionSpinBox>
#include <QDebug>

#include "qmultispinboxelements.h"



QMultiSpinBoxData::QMultiSpinBoxData(QMultiSpinBoxElement* element,
                                     const QString& suffix) :
    element(element),
    suffix(suffix),
    startIndex(-1),
    currentText()
{
    if (!element->convertString(element->defaultValue(), currentText)) {
        currentText.clear();
        qWarning("QMultiSpinBoxElement:  convert to string the default value return false (not logical)");
    }
    else
        currentText = currentText.simplified();
}


void QMultiSpinBoxData::shiftRight(int offset)
{
    if (startIndex >= 0)
        startIndex += offset;
}


QString QMultiSpinBoxData::fullText() const
{
    return currentText + suffix;
}

int QMultiSpinBoxData::fullLength() const
{
    return currentText.length() + suffix.length();
}

QString QMultiSpinBoxData::text() const
{
    return currentText;
}

int QMultiSpinBoxData::length() const
{
    return currentText.length();
}

//==============================================================================


QMultiSpinBox::QMultiSpinBox(QWidget *parent)
    : QWidget(*new QMultiSpinBoxPrivate, parent, 0)
{
    setObjectName(QLatin1String("QMultiSpinBox"));

    Q_D(QMultiSpinBox);

    connect(d->control, SIGNAL(updateNeeded(QRect)),
            this, SLOT(update()));
    connect(d->control, SIGNAL(updateMicroFocus()),
            this, SLOT(updateMicroFocus()));
    connect(d->control, SIGNAL(editingFinished()),
            this, SLOT(sectionEditingFinished()));
}


QMultiSpinBox::QMultiSpinBox(QMultiSpinBoxPrivate &dd, QWidget* parent) :
    QWidget(dd, parent, 0)
{
    setObjectName(QLatin1String("QMultiSpinBox"));
    qDebug("QMultiSpinBox:private constructor called");
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
    if (elementCount() <= 0)
        d->currentSectionIndex = -1;
    else {
        if (index < 0 || index >= elementCount())
            d->currentSectionIndex = 0;
        else
            d->currentSectionIndex = index;

        // check cursor!
        int lIndex = d->get(d->currentSectionIndex)->startIndex;
        int rIndex = lIndex + d->get(d->currentSectionIndex)->length();

        // cursor out if section -> invalidate
        if (d->control->cursorPosition() < lIndex
                || d->control->cursorPosition() > rIndex)
            d->control->setCursorPosition(-1);
    }

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


    // *** Draw Spinbox ***
    // draw the spinbox frame and buttons
    style()->drawComplexControl(QStyle::CC_SpinBox, &option, &painter, this);

    // *** Draw Text ***
    QRect textRect = fm.boundingRect(emptySpace, (int)d->textAlign | (int)Qt::TextSingleLine, d->text());
    painter.setClipRect(textRect);

    d->initLineEditControl(this);

    int textDrawFlags = QWidgetLineControl::DrawText;

    // if nothing is selected AND cursor is away
    if (d->cursorVisible)
        textDrawFlags |= QWidgetLineControl::DrawCursor;
    else if (d->control->hasSelectedText())
        textDrawFlags |= QWidgetLineControl::DrawSelections;

    d->control->draw(&painter, textRect.topLeft(), textRect, textDrawFlags);
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


//------------------------------------------------------------------------------


void QMultiSpinBox::sectionEditingFinished()
{
    Q_D(QMultiSpinBox);
    Q_ASSERT_X(d->currentSectionIndex >= 0, "editing", "control claims to finished editing but no section is active");
    Q_EMIT editingFinished(d->currentSectionIndex);
}


//==============================================================================


QMultiSpinBoxPrivate::QMultiSpinBoxPrivate() :
    textAlign(Qt::AlignCenter),
    control(new QWidgetLineControl())
{
    control->setEchoMode(QLineEdit::Normal);
    control->setReadOnly(false);
    control->setCursorMoveStyle(Qt::VisualMoveStyle);

    clear();
}


QMultiSpinBoxPrivate::~QMultiSpinBoxPrivate()
{
    qDeleteAll(elementDatas);
    delete control;
}


void QMultiSpinBoxPrivate::clear()
{
    control->setCursorPosition(-1);
    control->setText(QString());

    if (control->hasSelectedText())
        control->removeSelection();

    control->setModified(false);

    cursorVisible = false;
    currentSectionIndex = -1;
    prefix.resize(0);
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
    control->setCursorPosition(-1);
    control->removeSelection();
    QString t = prefix;
    foreach(QMultiSpinBoxData* eData, elementDatas)
        t.append(eData->fullText());
    control->setText(t);
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


void QMultiSpinBoxPrivate::initLineEditControl(QMultiSpinBox* widget)
{
    control->setFont(widget->font());
    control->setPalette(widget->palette());
    control->setCursorWidth(widget->style()->pixelMetric(QStyle::PM_TextCursorWidth));
}
