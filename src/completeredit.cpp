#include "completeedit.h"
#include <QAbstractItemView>
#include <QKeyEvent>
#include <QDebug>
#include <QPainter>
#include "userdata.h"

enum stationInfoEnum {
    ETHREEPINYINCODE = 0,
    ESTANAME,
    ESTACODE,
    EFULLSTAPINYIN,
    ESIMPLESTAPINYIN,
    ESTACURRENTCOUNT,
};

#define _ QStringLiteral

#define STATIONL1COUNT 256
#define STATIONL2COUNT 256
#define STATIONSIZE (STATIONL1COUNT * STATIONL2COUNT)
#define STATIONL1POS(x) ((x) * STATIONL1COUNT)
#define STATIONL2POS(y) (y)
#define STATIONPOS(x, y) (STATIONL1POS(x) + \
                          STATIONL2POS(y))

InputCompleter::InputCompleter(QObject *parent) :
    QCompleter(parent), m_model()
{
    setModel(&m_model);

    stationNameIndexLevel1.fill(-1, STATIONL1COUNT);
    stationNameIndexLevel2.fill(-1, STATIONL2COUNT);
    stationNameData.resize(STATIONSIZE);
    stationNameData.fill(nullptr, STATIONSIZE);

    stationFullPinYinIndexLevel1.fill(-1, STATIONL1COUNT);
    stationFullPinYinIndexLevel2.fill(-1, STATIONL2COUNT);
    stationFullPinYinData.resize(STATIONSIZE);
    stationFullPinYinData.fill(nullptr, STATIONSIZE);

    stationSimplePinYinIndexLevel1.fill(-1, STATIONL1COUNT);
    stationSimplePinYinIndexLevel2.fill(-1, STATIONL2COUNT);
    stationSimplePinYinData.resize(STATIONSIZE);
    stationSimplePinYinData.fill(nullptr, STATIONSIZE);

    isAppend = false;
}

InputCompleter::~InputCompleter()
{
    for (QVector<QPair<QByteArray, QString>> *d : stationNameData) {
        delete d;
    }
    for (QVector<QPair<QByteArray, QString>> *d : stationFullPinYinData) {
        delete d;
    }
    for (QVector<QPair<QByteArray, QString>> *d : stationSimplePinYinData) {
        delete d;
    }
}

InputCompleter &InputCompleter::operator=(const InputCompleter &other)
{
    int i;
    for (i = 0; i < other.stationNameIndexLevel1.size(); i++) {
        stationNameIndexLevel1[i] = other.stationNameIndexLevel1[i];
    }
    for (i = 0; i < other.stationNameIndexLevel2.size(); i++) {
        stationNameIndexLevel2[i] = other.stationNameIndexLevel2[i];
    }
    for (i = 0; i < other.stationNameData.size(); i++) {
        if (other.stationNameData[i]) {
            if (!stationNameData[i]) {
                stationNameData[i] = new QVector<QPair<QByteArray, QString>>;
            }
            *stationNameData[i] = *other.stationNameData[i];
        } else {
            delete stationNameData[i];
            stationNameData[i] = nullptr;
        }
    }

    for (i = 0; i < other.stationFullPinYinIndexLevel1.size(); i++) {
        stationFullPinYinIndexLevel1[i] = other.stationFullPinYinIndexLevel1[i];
    }
    for (i = 0; i < other.stationFullPinYinIndexLevel2.size(); i++) {
        stationFullPinYinIndexLevel2[i] = other.stationFullPinYinIndexLevel2[i];
    }
    for (i = 0; i < other.stationFullPinYinData.size(); i++) {
        //stationFullPinYinData[i] = other.stationFullPinYinData[i];
        if (other.stationFullPinYinData[i]) {
            if (!stationFullPinYinData[i]) {
                stationFullPinYinData[i] = new QVector<QPair<QByteArray, QString>>;
            }
            *stationFullPinYinData[i] = *other.stationFullPinYinData[i];
        } else {
            delete stationFullPinYinData[i];
            stationFullPinYinData[i] = nullptr;
        }
    }

    for (i = 0; i < other.stationSimplePinYinIndexLevel1.size(); i++) {
        stationSimplePinYinIndexLevel1[i] = other.stationSimplePinYinIndexLevel1[i];
    }
    for (i = 0; i < other.stationSimplePinYinIndexLevel2.size(); i++) {
        stationSimplePinYinIndexLevel2[i] = other.stationSimplePinYinIndexLevel2[i];
    }
    for (i = 0; i < other.stationSimplePinYinData.size(); i++) {
        //stationSimplePinYinData[i] = other.stationSimplePinYinData[i];
        if (other.stationSimplePinYinData[i]) {
            if (!stationSimplePinYinData[i]) {
                stationSimplePinYinData[i] = new QVector<QPair<QByteArray, QString>>;
            }
            *stationSimplePinYinData[i] = *other.stationSimplePinYinData[i];
        } else {
            delete stationSimplePinYinData[i];
            stationSimplePinYinData[i] = nullptr;
        }
    }

    keyWordStack = other.keyWordStack;
    m_word = other.m_word;
    isAppend = other.isAppend;

    return *this;
}

void InputCompleter::setStationData(const QByteArray &nameText)
{
    int i, j;
    int nameTextSize = nameText.size();
    int sectionIndex = 0;
    QByteArray staName, staCode, staSimplePinYin, staFullPinYin;
    UserData *ud = UserData::instance();

    i = 0;
    while (i < nameTextSize && nameText[i] != '@') {
        ++i;
    }
    if (i == nameTextSize)
        return;
    // skip '@' char
    ++i;
    for (; i < nameTextSize; i = j) {
        for (j = i + 1; j < nameTextSize && nameText[j] != '|' && nameText[j] != '@'; j++) {

        }
        if (j < nameTextSize) {
            if (j > i + 1) {
                switch (sectionIndex) {
                case ESTANAME:
                    staName = nameText.sliced(i + 1, j - (i + 1));
                    break;
                case ESTACODE:
                    staCode = nameText.sliced(i + 1, j - (i + 1));
                    break;
                case EFULLSTAPINYIN:
                    staFullPinYin = nameText.sliced(i + 1, j - (i + 1));
                    break;
                case ESIMPLESTAPINYIN:
                    staSimplePinYin = nameText.sliced(i + 1, j - (i + 1));
                    break;
                default:
                    break;
                }
            }
            sectionIndex++;
            if (nameText[j] == '@') {
                if (!staName.isEmpty() && !staCode.isEmpty() &&
                    !staFullPinYin.isEmpty() && !staSimplePinYin.isEmpty() &&
                    !staCode.isEmpty()) {
                    addStationName(staName, staFullPinYin);
                    addStationFullPinYin(staName, staFullPinYin);
                    addStationSimplePinYin(staName, staSimplePinYin, staFullPinYin);
                    ud->setStationCode(staName, staCode);
                }
                sectionIndex = 0;
                staName.clear();
                staFullPinYin.clear();
                staSimplePinYin.clear();
            }
        }
    }
    if (!staName.isEmpty() && !staCode.isEmpty() &&
        !staFullPinYin.isEmpty() && !staSimplePinYin.isEmpty()) {
        addStationName(staName, staFullPinYin);
        addStationFullPinYin(staName, staFullPinYin);
        addStationSimplePinYin(staName, staSimplePinYin, staFullPinYin);
        ud->setStationCode(staName, staCode);
    }
}

void InputCompleter::addStationName(const QByteArray &staName, const QByteArray &staFullPinYin)
{
    int idx1 = 0, idx2 = 0;
    if (staName.length() > 1) {
        idx2 = static_cast<unsigned char>(staName[1]);
    }
    idx1 = static_cast<unsigned char>(staName[0]);
    stationNameIndexLevel1[idx1] = idx1;
    stationNameIndexLevel2[idx2] = idx2;
    if (!stationNameData[STATIONPOS(idx1, idx2)]) {
        stationNameData[STATIONPOS(idx1, idx2)] = new QVector<QPair<QByteArray, QString>>;
    }
    stationNameData[STATIONPOS(idx1, idx2)]->append(
        QPair<QByteArray, QString>(staName, staName + _(" ") + staFullPinYin));
}

void InputCompleter::addStationFullPinYin(const QByteArray &staName, const QByteArray &staFullPinYin)
{
    int idx1 = 0, idx2 = 0;
    if (staFullPinYin.length() > 1) {
        idx2 = static_cast<unsigned char>(staFullPinYin[1]);
    }
    idx1 = static_cast<unsigned char>(staFullPinYin[0]);
    stationFullPinYinIndexLevel1[idx1] = idx1;
    stationFullPinYinIndexLevel2[idx2] = idx2;
    if (!stationFullPinYinData[STATIONPOS(idx1, idx2)]) {
        stationFullPinYinData[STATIONPOS(idx1, idx2)] = new QVector<QPair<QByteArray, QString>>;
    }
    stationFullPinYinData[STATIONPOS(idx1, idx2)]->append(
        QPair<QByteArray, QString>(staFullPinYin, staName + _(" ") + staFullPinYin));
}

void InputCompleter::addStationSimplePinYin(const QByteArray &staName,
                                            const QByteArray &staSimplePinYin, const QByteArray &staFullPinYin)
{
    int idx1 = 0, idx2 = 0;
    if (staSimplePinYin.length() > 1) {
        idx2 = static_cast<unsigned char>(staSimplePinYin[1]);
    }
    idx1 = static_cast<unsigned char>(staSimplePinYin[0]);
    stationSimplePinYinIndexLevel1[idx1] = idx1;
    stationSimplePinYinIndexLevel2[idx2] = idx2;
    if (!stationSimplePinYinData[STATIONPOS(idx1, idx2)]) {
        stationSimplePinYinData[STATIONPOS(idx1, idx2)] = new QVector<QPair<QByteArray, QString>>;
    }
    stationSimplePinYinData[STATIONPOS(idx1, idx2)]->append(
        QPair<QByteArray, QString>(staSimplePinYin, staName + _(" ") + staFullPinYin));
}

void InputCompleter::metaFilter(const QByteArray &word,
                                QVector<QPair<QByteArray, QString>> &meta,
                                QStringList &result)
{
    int idx11 = 0, idx21 = 0, idx31 = 0;
    int idx12 = -1, idx22 = -1, idx32 = -1;
    int idx;
    unsigned char w0, w1;

    if (word.size() > 0) {
        w0 = word[0];
        idx11 = stationSimplePinYinIndexLevel1[w0];
        idx21 = stationFullPinYinIndexLevel1[w0];
        idx31 = stationNameIndexLevel1[w0];
    }

    if (word.size() > 1) {
        w1 = word[1];
        idx12 = stationSimplePinYinIndexLevel2[w1];
        idx22 = stationFullPinYinIndexLevel2[w1];
        idx32 = stationNameIndexLevel2[w1];
    }

    const QVector<QVector<QVector<QPair<QByteArray, QString>> *>> &v = {
        stationSimplePinYinData,
        stationFullPinYinData,
        stationNameData
    };
    QVector<int> idx1 = { idx11, idx21, idx31 };
    QVector<int> idx2 = { idx12, idx22, idx32 };
    int basePos = 0;

    for (int i = 0; i < v.size(); i++) {
        if (idx1[i] != -1) {
            if (idx2[i] == -1) {
                basePos = STATIONL1POS(idx1[i]);
                for (idx = 0; idx < 256; idx++) {
                    if (v[i][basePos +
                              STATIONL2POS(idx)]) {
                        for (auto &d : *v[i][basePos +
                                            STATIONL2POS(idx)]) {
                            result.append(d.second);
                        }
                    }
                }
            } else {
                if (v[i][STATIONPOS(idx1[i], idx2[i])]) {
                    meta.append(*v[i][STATIONPOS(idx1[i], idx2[i])]);
                    for (auto &d : *v[i][STATIONPOS(idx1[i], idx2[i])]) {
                        result.append(d.second);
                    }
                }
            }
        }
    }
}

void InputCompleter::update(const QByteArray &word)
{
    // Do any filtering you like.
    QVector<QPair<QByteArray, QString>>::const_iterator it;
    QVector<QPair<QByteArray, QString>> previous, current;
    QStringList result;

    int count = word.length() - m_word.length();

    if (count > 0) {
        isAppend = true;
        if (m_word.length() < 2 || keyWordStack.isEmpty()) {
            isAppend = true;
            metaFilter(word, current, result);
            if (!current.isEmpty()) {
                keyWordStack.push(current);
            }
        } else {
            if (!keyWordStack.empty()) {
                previous = keyWordStack.top();
                for (it = previous.cbegin(); it != previous.cend(); it++) {
                    if (it->first.startsWith(word)) {
                        result.append(it->second);
                        current.append(*it);
                    }
                }
                if (!current.isEmpty()) {
                    keyWordStack.push(current);
                }
            }
        }
    } else {
        while (count++ < 0) {
            if (!keyWordStack.isEmpty())
                current = keyWordStack.pop();
        }
        if (isAppend && !keyWordStack.isEmpty()) {
            current = keyWordStack.top();
        }
        for (it = current.cbegin(); it != current.cend(); it++) {
            result.append(it->second);
        }
        isAppend = false;
    }

    if (!result.isEmpty()) {
        m_model.setStringList(result);
    }
    m_word = word;
    complete();
}

CompleteEdit::CompleteEdit(QWidget *parent)
    : QLineEdit(parent), c(nullptr)
{
}

CompleteEdit::~CompleteEdit()
{
}

void CompleteEdit::setCompleter(InputCompleter *completer)
{
    if (c) {
        QObject::disconnect(c, nullptr, this, nullptr);
        delete c;
    }

    c = completer;

    if (!c)
        return;

    c->setWidget(this);
    connect(completer, qOverload<const QString &>(&InputCompleter::activated),
            this, qOverload<const QString &>(&CompleteEdit::insertCompletion));
}

InputCompleter *CompleteEdit::completer() const
{
    return c;
}

void CompleteEdit::insertCompletion(const QString &completion)
{
    setText(completion.split(' ').at(0));
    selectAll();
}

void CompleteEdit::keyPressEvent(QKeyEvent *e)
{
    if (c && c->popup()->isVisible())
    {
        // The following keys are forwarded by the completer to the widget
        switch (e->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return; // Let the completer do default behavior
        }
    }

    bool isShortcut = (e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E;
    if (!isShortcut)
        QLineEdit::keyPressEvent(e); // Don't send the shortcut (CTRL-E) to the text edit.

    if (!c)
        return;

    bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!isShortcut && !ctrlOrShift && e->modifiers() != Qt::NoModifier)
    {
        c->popup()->hide();
        return;
    }

    c->update(text().toUtf8());
    c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
}

void CompleteEdit::inputMethodEvent(QInputMethodEvent *event)
{
    QLineEdit::inputMethodEvent(event);

    if (c) {
        c->update(text().toUtf8());
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
}
