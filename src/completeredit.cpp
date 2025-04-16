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

InputCompleterData::InputCompleterData()
{
    stationNameIndexLevel1.fill(-1, STATIONL1COUNT);
    stationNameIndexLevel2.fill(-1, STATIONL2COUNT);
    stationNameData.resize(STATIONSIZE);

    stationFullPinYinIndexLevel1.fill(-1, STATIONL1COUNT);
    stationFullPinYinIndexLevel2.fill(-1, STATIONL2COUNT);
    stationFullPinYinData.resize(STATIONSIZE);

    stationSimplePinYinIndexLevel1.fill(-1, STATIONL1COUNT);
    stationSimplePinYinIndexLevel2.fill(-1, STATIONL2COUNT);
    stationSimplePinYinData.resize(STATIONSIZE);
}

InputCompleterData::~InputCompleterData()
{

}

void InputCompleterData::addStationName(const QByteArray &staName, const QByteArray &staFullPinYin)
{
    int idx1 = 0, idx2 = 0;
    if (staName.length() > 1) {
        idx2 = static_cast<unsigned char>(staName[1]);
    }
    idx1 = static_cast<unsigned char>(staName[0]);
    stationNameIndexLevel1[idx1] = idx1;
    stationNameIndexLevel2[idx2] = idx2;
    if (!stationNameData[STATIONPOS(idx1, idx2)]) {
        stationNameData[STATIONPOS(idx1, idx2)] = new stationData;
    }
    stationNameData[STATIONPOS(idx1, idx2)]->addData(
        QPair<QByteArray, QString>(staName, staName + _(" ") + staFullPinYin));
}

void InputCompleterData::addStationFullPinYin(const QByteArray &staName, const QByteArray &staFullPinYin)
{
    int idx1 = 0, idx2 = 0;
    if (staFullPinYin.length() > 1) {
        idx2 = static_cast<unsigned char>(staFullPinYin[1]);
    }
    idx1 = static_cast<unsigned char>(staFullPinYin[0]);
    stationFullPinYinIndexLevel1[idx1] = idx1;
    stationFullPinYinIndexLevel2[idx2] = idx2;
    if (!stationFullPinYinData[STATIONPOS(idx1, idx2)]) {
        stationFullPinYinData[STATIONPOS(idx1, idx2)] = new stationData;
    }
    stationFullPinYinData[STATIONPOS(idx1, idx2)]->addData(
        QPair<QByteArray, QString>(staFullPinYin, staName + _(" ") + staFullPinYin));
}

void InputCompleterData::addStationSimplePinYin(const QByteArray &staName,
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
        stationSimplePinYinData[STATIONPOS(idx1, idx2)] = new stationData;
    }
    stationSimplePinYinData[STATIONPOS(idx1, idx2)]->addData(
        QPair<QByteArray, QString>(staSimplePinYin, staName + _(" ") + staFullPinYin));
}

InputCompleter::InputCompleter(QObject *parent) :
    QCompleter(parent), m_model()
{
    setModel(&m_model);

    isAppend = false;
}

InputCompleter::~InputCompleter()
{

}

InputCompleter &InputCompleter::operator=(const InputCompleter &other)
{
    d = other.d;
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
                    d.addStationName(staName, staFullPinYin);
                    d.addStationFullPinYin(staName, staFullPinYin);
                    d.addStationSimplePinYin(staName, staSimplePinYin, staFullPinYin);
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
        d.addStationName(staName, staFullPinYin);
        d.addStationFullPinYin(staName, staFullPinYin);
        d.addStationSimplePinYin(staName, staSimplePinYin, staFullPinYin);
        ud->setStationCode(staName, staCode);
    }
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
        idx11 = d.stationSimplePinYinIndexLevel1[w0];
        idx21 = d.stationFullPinYinIndexLevel1[w0];
        idx31 = d.stationNameIndexLevel1[w0];
    }

    if (word.size() > 1) {
        w1 = word[1];
        idx12 = d.stationSimplePinYinIndexLevel2[w1];
        idx22 = d.stationFullPinYinIndexLevel2[w1];
        idx32 = d.stationNameIndexLevel2[w1];
    }

    const QVector<QVector<QSharedDataPointer<stationData>>> &v = {
        d.stationSimplePinYinData,
        d.stationFullPinYinData,
        d.stationNameData
    };
    QVector<int> idx1 = { idx11, idx21, idx31 };
    QVector<int> idx2 = { idx12, idx22, idx32 };
    int basePos = 0;

    for (int i = 0; i < v.size(); i++) {
        if (idx1[i] != -1) {
            if (idx2[i] == -1) {
                basePos = STATIONL1POS(idx1[i]);
                for (idx = 0; idx < STATIONL2COUNT; idx++) {
                    if (v[i][basePos +
                              STATIONL2POS(idx)].constData()) {
                        for (const QPair<QByteArray, QString> &d : v[i].at(basePos +
                                STATIONL2POS(idx))->data()) {
                            result.append(d.second);
                        }
                    }
                }
            } else {
                if (v[i].at(STATIONPOS(idx1[i], idx2[i])).constData()) {
                    meta.append(v[i].at(STATIONPOS(idx1[i], idx2[i]))->data());
                    for (const QPair<QByteArray, QString> &d : meta) {
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
