#include "qtmultispinbox.h"

#include <QDebug>

#include "qtmultispinboxelements.h"

#define DBG_LEVEL_VALIDATE      1
#define DBG_LEVEL_CHECKANDSPLIT 2
#define DBG_LEVEL_INSERT        3
#define DBG_LEVEL_COND(l) (l == 0)

#ifdef QT_DEBUG
#define QMSBDEBUG(LEVEL) if (DBG_LEVEL_COND(LEVEL)) qDebug()
#else
#define QMSBDEBUG(LEVEL) if (false) qDebug()
#endif



QtMultiSpinBoxData::QtMultiSpinBoxData(QtMultiSpinBoxElement* element,
                                       const QString& suffix) :
    element(element),
    suffix(suffix)
{
}

//==============================================================================


QtMultiSpinBox::QtMultiSpinBox(QWidget *parent) :
    QAbstractSpinBox(parent),
    d_ptr(new QtMultiSpinBoxPrivate(this))
{
    setObjectName(QLatin1String("QtMultiSpinBox"));

    lineEdit()->setValidator(d_func());
}

QtMultiSpinBox::QtMultiSpinBox(QAbstractSpinBoxPrivate &dd, QWidget *parent) :
    QAbstractSpinBox(dd, parent),
    d_ptr(new QtMultiSpinBoxPrivate(this))
{
    setObjectName(QLatin1String("QtMultiSpinBox"));

    lineEdit()->setValidator(d_func());
}


QtMultiSpinBox::~QtMultiSpinBox()
{
}


void QtMultiSpinBox::clear()
{
    Q_D(QtMultiSpinBox);
    d->clear();
}


void QtMultiSpinBox::insertSpinElement(int index, QtMultiSpinBoxElement* element, const QString &suffix)
{
    Q_ASSERT(index >= 0 && index <= count());
    Q_ASSERT(element != NULL);

    QMSBDEBUG(DBG_LEVEL_INSERT) << "insert at" << index << "(suffix:" << suffix << ")";

    Q_D(QtMultiSpinBox);
    QString simplifiedSuffix = d->simplify(suffix);
    // if not the first element -> check the separation suffix
    if (d->elementDatas.count() >= 1) {
        // if not the last -> check the given suffix
        if (index != d->elementDatas.count())
            Q_ASSERT(!simplifiedSuffix.isEmpty());
        // if not the first -> check the previous element suffix
        if (index != 0) {
            QtMultiSpinBoxData* elementBeforeIndex = d->elementDatas.value(index-1);
            Q_ASSERT(!elementBeforeIndex->suffix.isEmpty());
        }
    }

    d->insert(index, element, simplifiedSuffix);
}

void QtMultiSpinBox::removeSpinElement(int index)
{
    delete takeSpinElement(index);
}

QtMultiSpinBoxElement* QtMultiSpinBox::takeSpinElement(int index)
{
    Q_ASSERT(index >= 0 && index < count());

    Q_D(QtMultiSpinBox);
    QScopedPointer<QtMultiSpinBoxData> eData(d->take(index));

    bool changeCSI = (d->currentSectionIndex == index);
    if (changeCSI) {
        d->currentSectionIndex = index-1;
        Q_EMIT currentSectionIndexChanged(d->currentSectionIndex);
    }

    return eData.data()->element;
}

QtMultiSpinBoxElement* QtMultiSpinBox::getSpinElement(int index)
{
    Q_ASSERT(index >= 0 && index < count());

    Q_D(QtMultiSpinBox);
    return d->get(index)->element;
}


int QtMultiSpinBox::count() const
{
    Q_D(const QtMultiSpinBox);
    return d->elementDatas.count();
}


//-----------------------------------------------------------------------------
// property stuff

int QtMultiSpinBox::currentSectionIndex() const
{
    Q_D(const QtMultiSpinBox);
    return d->currentSectionIndex;
}

void QtMultiSpinBox::setCurrentSectionIndex(int index)
{
    Q_D(QtMultiSpinBox);
    if (count() <= 0)
        d->currentSectionIndex = -1;
    else {
        if (index < 0 || index >= count())
            d->currentSectionIndex = 0;
        else
            d->currentSectionIndex = index;

        // check cursor!
    }

    update();
    Q_EMIT currentSectionIndexChanged(index);
}

QString QtMultiSpinBox::prefix() const
{
    Q_D(const QtMultiSpinBox);
    return d->prefix;
}

void QtMultiSpinBox::setPrefix(const QString& prefix)
{
    Q_D(QtMultiSpinBox);
    QString oldPrefix = d->prefix;
    d->prefix = prefix.simplified();

    // replacing prefix
    QString text = lineEdit()->text();
    text.replace(0, oldPrefix.length(), d->prefix);
    lineEdit()->setText(text);
}

QString QtMultiSpinBox::suffixOf(int index) const
{
    Q_ASSERT(index >= 0 && index < count());
    Q_D(const QtMultiSpinBox);
    return d->elementDatas.value(index)->suffix;
}

void QtMultiSpinBox::setSuffixOf(int index, const QString& suffix)
{
    Q_ASSERT(index >= 0 && index < count());
    Q_D(QtMultiSpinBox);
    d->elementDatas.value(index)->suffix = suffix;
    //  TODO: update text
}


//==============================================================================


QtMultiSpinBoxPrivate::QtMultiSpinBoxPrivate(QtMultiSpinBox *s) :
    q_ptr(s)
{
    clear();

    Q_Q(QtMultiSpinBox);
    q->connect(q, SIGNAL(editingFinished()),
               q, SLOT(_q_sectionEditingFinished()));
}


QtMultiSpinBoxPrivate::~QtMultiSpinBoxPrivate()
{
    qDeleteAll(elementDatas);
}


void QtMultiSpinBoxPrivate::clear()
{
    currentSectionIndex = -1;
    prefix.resize(0);
    elementDatas.clear();

    Q_Q(QtMultiSpinBox);
    q->lineEdit()->clear();
}


void QtMultiSpinBoxPrivate::insert(int index, QtMultiSpinBoxElement* element, const QString &suffix)
{
    Q_Q(QtMultiSpinBox);

    QString text = q->lineEdit()->text();
    int startIndexElement = textIndex(text, index);
    Q_ASSERT(startIndexElement >= 0);
    QMSBDEBUG(DBG_LEVEL_INSERT) << "insert at" << index
                                << "previous" << text
                                << "text_index" << startIndexElement;

    // index is valid, element not null
    QtMultiSpinBoxData* newElement = new QtMultiSpinBoxData(element, suffix);
    elementDatas.insert(index, newElement);

    QString defaultText = element->textFromValue(element->defaultValue());
    if (defaultText.isNull())
        qWarning("QtMultiSpinBox:  text of default value is invalid");
    QMSBDEBUG(DBG_LEVEL_INSERT) << "default text" << defaultText
                                << "suffix" << newElement->suffix;

    // inserting text
    text.insert(startIndexElement, defaultText.simplified() + newElement->suffix);
    QMSBDEBUG(DBG_LEVEL_INSERT) << "final text" << text;
    q->lineEdit()->setText(text);
}


QtMultiSpinBoxData* QtMultiSpinBoxPrivate::take(int index)
{
    Q_Q(QtMultiSpinBox);

    QString text = q->lineEdit()->text();
    int startIndexElement = textIndex(text, index);
    int endIndexElement = (index+1 == elementDatas.count())
            ? text.length() : textIndex(text, index+1);
    Q_ASSERT(startIndexElement >= 0);
    Q_ASSERT(endIndexElement >= 0);

    // index is valid, element exist
    QtMultiSpinBoxData* takenElementData = elementDatas.takeAt(index);

    // removing text
    text.remove(startIndexElement, endIndexElement - startIndexElement);
    q->lineEdit()->setText(text);

    return takenElementData;
}


QtMultiSpinBoxData* QtMultiSpinBoxPrivate::get(int index)
{
    // index is valid, element exist
    return elementDatas.value(index);
}


//------------------------------------------------------------------------------


void QtMultiSpinBoxPrivate::_q_sectionEditingFinished()
{
    Q_Q(QtMultiSpinBox);
    //    Q_ASSERT_X(currentSectionIndex >= 0, "editing", "editingfinished but no section is active");
    Q_EMIT q->editingFinished(currentSectionIndex);
}

//-----------------------------------------------------------------------------

QString QtMultiSpinBoxPrivate::simplify(const QString& text) const
{
    const QChar blank = QLatin1Char(' ');
    if (text.isEmpty())
        return text;
    QString s = text.simplified();
    bool wasEmpty = s.isEmpty();
    // re-add first whitespaces
    for (int index=0; index < text.length() && text.at(index).isSpace(); index++)
        s.insert(index, blank);
    if (!wasEmpty) {
        // re-add last whitespaces
        for (int index = 0; index < text.length() && text.at(text.length() - index-1).isSpace(); index++)
            s.insert(s.length() - index, blank);
    }
    return s;
}

//-----------------------------------------------------------------------------
// validation


bool QtMultiSpinBoxPrivate::checkAndSplit(const QString& input, QList<QStringRef>& result) const
{
    QStringRef r = QStringRef(&input);
    if (!prefix.isEmpty()) {
        if (!r.startsWith(prefix, Qt::CaseSensitive)) {
            QMSBDEBUG(DBG_LEVEL_CHECKANDSPLIT) << "checkAndSplit: invalid prefix";
            return false;
        }
        r = r.mid(prefix.length());
    }
    QList<QtMultiSpinBoxData*>::const_iterator it;
    for (it = elementDatas.constBegin(); it != elementDatas.constEnd(); ++it) {
        if (!(*it)->suffix.isEmpty()) {
            int index = r.indexOf((*it)->suffix, 0, Qt::CaseSensitive);
            if (index < 0) {
                QMSBDEBUG(DBG_LEVEL_CHECKANDSPLIT) << "checkAndSplit: cannot find next suffix";
                break;
            }
            QMSBDEBUG(DBG_LEVEL_CHECKANDSPLIT) << "checkAndSplit: add" << r.mid(0, index);
            result.append(r.mid(0, index));
            r = r.mid(index + (*it)->suffix.length());
        }
        else {
            // this should be the last one
            result.append(r);
            QMSBDEBUG(DBG_LEVEL_CHECKANDSPLIT) << "checkAndSplit: add" << r;
            r.clear();
            ++it;
            QMSBDEBUG(DBG_LEVEL_CHECKANDSPLIT) << "checkAndSplit: empty suffix found, it should the last";
            break;
        }
    }
    QMSBDEBUG(DBG_LEVEL_CHECKANDSPLIT) << "checkAndSplit: result for" << input << (bool)(it == elementDatas.constEnd() && r.length() == 0);
    return (it == elementDatas.constEnd() && r.length() == 0);
}

int QtMultiSpinBoxPrivate::textIndex(const QString& text, int indexElement) const
{
    int index = 0;
    if (!prefix.isEmpty()) {
        if (!text.startsWith(prefix, Qt::CaseSensitive))
            return -1;
        else
            index += prefix.length();
    }
    Q_ASSERT(indexElement <= elementDatas.count());
    for (int i=0; i<indexElement && index >= 0; i++) {
        QtMultiSpinBoxData* e = elementDatas.value(i);
        if (!e->suffix.isEmpty()) {
            index = text.indexOf(e->suffix, index, Qt::CaseSensitive);
            if (index >= 0)
                index += e->suffix.length();
        }
        else {
            // this must be the last one
            if (!(i+1 == indexElement && indexElement+1 == elementDatas.count()))
                index = -1;
        }
    }
    return index;
}

QValidator::State QtMultiSpinBoxPrivate::validate(QString &text, int &pos) const
{
    QList<QStringRef> splits;
    if (!checkAndSplit(text, splits))
        return QValidator::Invalid;

    int offsetReplace = prefix.length();
    QString newText(text);
    QValidator::State r = QValidator::Acceptable;
    foreach (QtMultiSpinBoxData* e, elementDatas) {
        QStringRef ref = splits.takeFirst();
        QString copy = ref.toString();
        QString sval = copy;
        QValidator::State rs = e->element->validate(sval, pos);
        QMSBDEBUG(DBG_LEVEL_VALIDATE) << "validate  index=" << offsetReplace << " text=" << copy;
        if (rs == QValidator::Invalid) {
            QMSBDEBUG(DBG_LEVEL_VALIDATE) << "validate  result for" << text << "Invalid";
            return QValidator::Invalid;
        }
        else if (rs == QValidator::Intermediate)
            r = QValidator::Intermediate;

        if (sval != copy)
            newText.replace(offsetReplace, copy.length(), sval);
        offsetReplace += sval.length() + e->suffix.length();
    }

    text.swap(newText);
    QMSBDEBUG(DBG_LEVEL_VALIDATE) << "validate  result for" << text  << ((r == QValidator::Acceptable) ? "acceptable" : "intermediate");
    return r;
}

void QtMultiSpinBoxPrivate::fixup(QString &) const
{

}
