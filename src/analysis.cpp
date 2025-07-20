#include "include/analysis.h"
#include "userdata.h"
#include "12306.h"
#include "mainwindow.h"
#include "nethelper.h"

#ifdef __GNUC__
#define TICKETIDX(idx) ({ Q_ASSERT(idx >= EGGNUM && idx < EYPEX); ((idx) - EGGNUM); })
#else
#define TICKETIDX(idx) ((idx) - EGGNUM)
#endif
#define TICKETSIZE (EYPEX - EGGNUM)
#define _ QStringLiteral

extern MainWindow *w;

/*
static const QVector<QPair<QString, int>> seatTypeNameIndex = {
    QPair<QString, int>(_("高级软卧"), TICKETIDX(EGRNUM)),
    QPair<QString, int>(_("其他"), TICKETIDX(EQTNUM)),
    QPair<QString, int>(_("软卧"), TICKETIDX(ERWNUM)),
    QPair<QString, int>(_("软座"), TICKETIDX(ERZNUM)),
    QPair<QString, int>(_("无座"), TICKETIDX(EWZNUM)),
    QPair<QString, int>(_("动卧"), TICKETIDX(EYBNUM)),
    QPair<QString, int>(_("硬卧"), TICKETIDX(EYWNUM)),
    QPair<QString, int>(_("硬座"), TICKETIDX(EYZNUM)),
    QPair<QString, int>(_("二等座"), TICKETIDX(EZENUM)),
    QPair<QString, int>(_("一等座"), TICKETIDX(EZYNUM)),
    QPair<QString, int>(_("商务特等座"), TICKETIDX(ESWZNUM))
};
*/

int seatTypeNameToTicketIndex(const QList<QString> &seatTypeStrList, QVector<int> &seatType)
{
    static const QMap<QString, int> seatTypeNameIndexMap = {
        { _("商务座"), TICKETIDX(ESWZNUM) },
        { _("特等座"), TICKETIDX(ETZNUM) },
        { _("优选一等座"), TICKETIDX(EGGNUM) },
        { _("一等座"), TICKETIDX(EZYNUM) },
        { _("二等座"), TICKETIDX(EZENUM) },
        { _("高级软卧"), TICKETIDX(EGRNUM) },
        { _("软卧"), TICKETIDX(ERWNUM) },
        { _("动卧"), TICKETIDX(ESRRBNUM) },
        { _("硬卧"), TICKETIDX(EYWNUM) },
        { _("软座"), TICKETIDX(ERZNUM) },
        { _("硬座"), TICKETIDX(EYZNUM) },
        { _("无座"), TICKETIDX(EWZNUM) },
        { _("其他"), TICKETIDX(EQTNUM) }
    };
    QMap<QString, int>::ConstIterator it;

    for (const QString &s : seatTypeStrList) {
        it = seatTypeNameIndexMap.constFind(s);
        if (it != seatTypeNameIndexMap.constEnd()) {
            seatType.append(it.value());
        }
    }

    return seatType.size();
}

QString ticketIndexToSeatTypeName(int ticketIndex)
{
    static const QMap<int, QString> seatTypeIndexNameMap = {
        { TICKETIDX(ESWZNUM), _("商务座") },
        { TICKETIDX(ETZNUM), _("特等座") },
        { TICKETIDX(EGGNUM), _("优选一等座") },
        { TICKETIDX(EZYNUM), _("一等座") },
        { TICKETIDX(EZENUM), _("二等座") },
        { TICKETIDX(EGRNUM), _("高级软卧") },
        { TICKETIDX(ERWNUM), _("软卧") },
        { TICKETIDX(ESRRBNUM), _("动卧") },
        { TICKETIDX(EYWNUM),  _("硬卧") },
        { TICKETIDX(ERZNUM), _("软座") },
        { TICKETIDX(EYZNUM), _("硬座") },
        { TICKETIDX(EWZNUM), _("无座") },
        { TICKETIDX(EQTNUM), _("其他") }
    };
    QMap<int, QString>::ConstIterator it;

    it = seatTypeIndexNameMap.constFind(ticketIndex);
    if (it != seatTypeIndexNameMap.constEnd()) {
        return it.value();
    }

    return _("未知");
}

QChar ticketIndexToCode(int ticketIndex)
{
    static const QMap<int, QChar> ticketIndexCodeMap = {
        { TICKETIDX(ESWZNUM), '9' },
        {TICKETIDX(ETZNUM), 'P' },
        { TICKETIDX(EGGNUM), 'D' },
        { TICKETIDX(EZYNUM), 'M' },
        { TICKETIDX(EZENUM), 'O' },
        { TICKETIDX(EGRNUM), '6' },
        { TICKETIDX(ERWNUM), '4' },
        { TICKETIDX(ESRRBNUM), 'F' },
        { TICKETIDX(EYWNUM), '3' },
        { TICKETIDX(ERZNUM), '2' },
        { TICKETIDX(EYZNUM), '1' },
        { TICKETIDX(EWZNUM), 'W' },
        { TICKETIDX(EQTNUM), 'H' }
    };
    QMap<int, QChar>::ConstIterator it;

    it = ticketIndexCodeMap.constFind(ticketIndex);
    if (it != ticketIndexCodeMap.constEnd()) {
        return it.value();
    }

    return '0';
}

QChar seatTypeNoToCode(enum TrainInfoEnum seatTypeNo)
{
    static const QMap<enum TrainInfoEnum, QChar> seatTypeNoCodeMap = {
        { ESWZNUM, '9' },
        { ETZNUM, 'P' },
        { EGGNUM, 'D' },
        { EZYNUM, 'M' },
        { EZENUM, 'O' },
        { EGRNUM, '6' },
        { ERWNUM, '4' },
        { ESRRBNUM, 'F' },
        { EYWNUM, '3' },
        { ERZNUM, '2' },
        { EYZNUM, '1' },
        { EWZNUM, 'W' },
        { EQTNUM, 'H' }
    };
    QMap<enum TrainInfoEnum, QChar>::ConstIterator it;

    it = seatTypeNoCodeMap.constFind(seatTypeNo);

    return it != seatTypeNoCodeMap.constEnd() ? it.value() : '0';
}

QChar seatTypeNameToCode(const QString &seatTypeName)
{
    static const QMap<QString, QChar> seatTypeNameCodeMap = {
        { _("商务座"), '9' },
        { _("特等座"), 'P' },
        { _("优选一等座"), 'D' },
        { _("一等座"), 'M' },
        { _("二等座"), 'O' },
        { _("高级软卧"), '6' },
        { _("软卧"), '4' },
        { _("动卧"), 'F' },
        { _("硬卧"), '3' },
        { _("软座"), '2' },
        { _("硬座"), '1' },
        { _("无座"), 'W' },
        { _("其他"), 'H' },
    };
    QMap<QString, QChar>::ConstIterator it;

    it = seatTypeNameCodeMap.constFind(seatTypeName);

    return it != seatTypeNameCodeMap.constEnd() ? it.value() : '0';
}

QString seatTypeCodeToName(QChar code)
{
    static const QMap<QChar, QString> SeatTypeCodeNameMap = {
        { '9', _("商务座") },
        { 'P', _("特等座"), },
        { 'D', _("优选一等座") },
        { 'M', _("一等座") },
        { 'O', _("二等座") },
        { '6', _("高级软卧") },
        { '4', _("软卧") },
        { 'F', _("动卧") },
        { '3', _("硬卧") },
        { '2', _("软座") },
        { '1', _("硬座") },
        { 'W', _("无座") },
        { 'H', _("其他") },
    };
    QMap<QChar, QString>::ConstIterator it;

    it = SeatTypeCodeNameMap.constFind(code);

    return it != SeatTypeCodeNameMap.constEnd() ? it.value() : _("未知");
}

Analysis::Analysis(QVector<QStringList> &trainVec) : trainInfoVec(trainVec)
{

}

Analysis::~Analysis()
{

}

void Analysis::setAnalysisTrain(QVector<QStringList> &trainVec)
{
    trainInfoVec = trainVec;
}

int Analysis::sufficientTicketPrioSelectTrain(const QVariantMap &stationMap)
{
    int trainTicketNum, trainTicketNumMax, ticketMaxidx;
    QVector<int> selectSeatType;
    int selectSeatTypeSize = 0;
    UserData *ud = UserData::instance();
    const QList<QString> &selectedSeatTypeStrList = w->seatTypeDialog->getSelectedSeatType();
    Q_ASSERT(selectedSeatTypeStrList.size() <= TICKETSIZE);
    selectSeatTypeSize = seatTypeNameToTicketIndex(selectedSeatTypeStrList, selectSeatType);
    const QList<QString> &passengerList = w->passengerDialog->getSelectedPassenger();
    const QSet<QString> &selectedTrainSet = w->trainNoDialog->getSelectedTrainSet();
    bool filterByCondition = false;
    bool subCondition = true;
    QString trainDesc;
    NetHelper *nh = NetHelper::instance();

    trainTicketNumMax = 0;
    ticketMaxidx = -1;

again:
    for (int i = 0; i < trainInfoVec.size(); i++) {
        QStringList &trainList = trainInfoVec[i];
        QVector<int> &ticketInfo = trainTicketInfo[i];
        if (trainList.empty() || ticketInfo.empty())
            continue;

        trainDesc = _("%1 (%2 %3").arg(trainList[ESTATIONTRAINCODE],
                                               stationMap.value(trainList[EFROMSTATIONTELECODE]).toString(),
                                               stationMap.value(trainList[ETOSTATIONTELECODE]).toString());
        // Currentlly frozened
        if (nh->queryFrozenTrain(trainDesc)) {
            continue;
        }
        if (!selectedTrainSet.contains(trainDesc))
            continue;

        if (subCondition) {
            if (ud->grabSetting.trainPrio.preferGPrio &&
                !trainList[ESTATIONTRAINCODE].isEmpty() &&
                trainList[ESTATIONTRAINCODE][0] != 'G') {
                filterByCondition = true;
                continue;
            }
            if (ud->grabSetting.trainPrio.preferDPrio &&
                !trainList[ESTATIONTRAINCODE].isEmpty() &&
                trainList[ESTATIONTRAINCODE][0] != 'D') {
                filterByCondition = true;
                continue;
            }
            if (ud->grabSetting.trainPrio.preferTimeRangePrio) {
                QStringList timeList = trainList[ESTARTTIME].split(":");
                int trainHour = timeList.size() > 0 ? timeList[0].toInt() : 0;
                int trainMinute = timeList.size() > 1 ? timeList[1].toInt() : 0;
                if (!ud->isTimeInRange(trainHour, trainMinute)) {
                    filterByCondition = true;
                    continue;
                }
            }
        }
        trainTicketNum = 0;
        for (int j = 0; j < selectSeatTypeSize; j++) {
            if (ticketInfo[selectSeatType[j]] > 0) {
                trainTicketNum += ticketInfo[selectSeatType[j]];
            }
        }
        if (trainTicketNum > trainTicketNumMax) {
            trainTicketNumMax = trainTicketNum;
            ticketMaxidx = i;
        }
    }
    if (filterByCondition && ticketMaxidx == -1 && subCondition) {
        subCondition = false;
        goto again;
    }
    if (!ud->grabSetting.ticketSetting.partialSubmit) {
        return trainTicketNumMax >= passengerList.size() ? ticketMaxidx : -1;
    } else {
        return ticketMaxidx;
    }
}

int Analysis::strictTrainPrioSelectTrain(const QVariantMap &stationMap)
{
    int trainTicketNum;
    QVector<int> selectSeatType;
    int selectSeatTypeSize = 0;
    int partialTicketMax = 0;
    int partialTicketMaxIndex = -1;
    int j;
    const QList<QString> selectedSeatTypeStrList = w->seatTypeDialog->getSelectedSeatType();
    Q_ASSERT(selectedSeatTypeStrList.size() <= TICKETSIZE);
    selectSeatTypeSize = seatTypeNameToTicketIndex(selectedSeatTypeStrList, selectSeatType);
    const QList<QString> &selectedTrain = w->trainNoDialog->getSelectedTrainList();
    const QList<QString> &passengerList = w->passengerDialog->getSelectedPassenger();
    QString s;
    QHash<QString, int> trainInfoHash;
    NetHelper *nh = NetHelper::instance();
    bool partial = UserData::instance()->grabSetting.ticketSetting.partialSubmit;

    for (j = 0; j < trainInfoVec.size(); j++) {
        const QStringList &trainInfo = trainInfoVec[j];
        s = _("%1 (%2 %3").arg(trainInfo[ESTATIONTRAINCODE],
                               stationMap.value(trainInfo[EFROMSTATIONTELECODE]).toString(),
                               stationMap.value(trainInfo[ETOSTATIONTELECODE]).toString());
        trainInfoHash.insert(s, j);
    }

    for (int i = 0; i < selectedTrain.size(); i++) {
        const QString &trainDesc = selectedTrain[i];
        // Currentlly frozened
        if (nh->queryFrozenTrain(trainDesc)) {
            continue;
        }
        j = trainInfoHash.value(trainDesc, -1);
        if (j == -1) {
            continue;
        }

        trainTicketNum = 0;
        for (int k = 0; k < selectSeatTypeSize; k++) {
            if (trainTicketInfo[j][selectSeatType[k]] > 0) {
                trainTicketNum += trainTicketInfo[j][selectSeatType[k]];
            }
        }
        if (trainTicketNum >= passengerList.size()) {
            return j;
        }
        if (partial) {
            if (trainTicketNum > partialTicketMax) {
                partialTicketMax = trainTicketNum;
                partialTicketMaxIndex = j;
            }
        }
    }

    return partialTicketMaxIndex;
}

int Analysis::strictTravelTimePrioSelectTrain(const QVariantMap &stationMap)
{
    int trainTicketNum;
    QVector<int> selectSeatType;
    int selectSeatTypeSize = 0;
    int partialTicketMax = 0;
    int partialTicketMaxIdx = -1;
    int travelTimeShortestIdx = -1;
    const QList<QString> &selectedSeatTypeStrList = w->seatTypeDialog->getSelectedSeatType();
    Q_ASSERT(selectedSeatTypeStrList.size() <= TICKETSIZE);
    selectSeatTypeSize = seatTypeNameToTicketIndex(selectedSeatTypeStrList, selectSeatType);
    const QSet<QString> &selectedTrainSet = w->trainNoDialog->getSelectedTrainSet();
    const QList<QString> &passengerList = w->passengerDialog->getSelectedPassenger();
    int travelTimeHour;
    int travelTimeMinute;
    int travelTimeShortestHour = INT_MAX;
    int travelTimeShortestMinute = INT_MAX;
    QStringList travelTimeStrList;
    NetHelper *nh = NetHelper::instance();
    bool partial = UserData::instance()->grabSetting.ticketSetting.partialSubmit;

    for (int i = 0; i < trainInfoVec.size(); i++) {
        QStringList &trainList = trainInfoVec[i];
        if (trainList.empty() || trainTicketInfo[i].empty())
            continue;
        QString trainDesc = _("%1 (%2 %3").arg(trainList[ESTATIONTRAINCODE],
                                               stationMap.value(trainList[EFROMSTATIONTELECODE]).toString(),
                                               stationMap.value(trainList[ETOSTATIONTELECODE]).toString());
        // Currentlly frozened
        if (nh->queryFrozenTrain(trainDesc)) {
            continue;
        }
        if (!selectedTrainSet.contains(trainDesc))
            continue;
        trainTicketNum = 0;
        for (int j = 0; j < selectSeatTypeSize; j++) {
            if (trainTicketInfo[i][selectSeatType[j]] > 0) {
                trainTicketNum += trainTicketInfo[i][selectSeatType[j]];
            }
        }
        if (trainTicketNum >= passengerList.size()) {
            travelTimeStrList = trainList[ESPENDTIME].split(':', Qt::SkipEmptyParts);
            travelTimeHour = travelTimeStrList.length() > 0 ? travelTimeStrList[0].toInt() : INT_MAX;
            travelTimeMinute = travelTimeStrList.length() > 1 ? travelTimeStrList[1].toInt() : INT_MAX;
            if (travelTimeHour < travelTimeShortestHour ||
                (travelTimeHour == travelTimeShortestHour &&
                travelTimeMinute < travelTimeShortestMinute)) {
                travelTimeShortestHour = travelTimeHour;
                travelTimeShortestMinute = travelTimeMinute;
                travelTimeShortestIdx = i;
            }
        }
        if (partial) {
            if (trainTicketNum > partialTicketMax) {
                partialTicketMax = trainTicketNum;
                partialTicketMaxIdx = i;
            }
        }
    }
    return travelTimeShortestIdx != -1 ? travelTimeShortestIdx : partialTicketMaxIdx;
}

int Analysis::strictStartTimePrioSelectTrain(const QVariantMap &stationMap, const QString &trainCode)
{
    int trainTicketNum;
    QVector<int> selectSeatType;
    int selectSeatTypeSize = 0;
    int partialTicketMax = 0;
    int partialTicketMaxIdx = -1;
    const QList<QString> &selectedSeatTypeStrList = w->seatTypeDialog->getSelectedSeatType();
    Q_ASSERT(selectedSeatTypeStrList.size() <= TICKETSIZE);
    selectSeatTypeSize = seatTypeNameToTicketIndex(selectedSeatTypeStrList, selectSeatType);
    const QSet<QString> &selectedTrainSet = w->trainNoDialog->getSelectedTrainSet();
    const QList<QString> &passengerList = w->passengerDialog->getSelectedPassenger();
    NetHelper *nh = NetHelper::instance();
    bool partial = UserData::instance()->grabSetting.ticketSetting.partialSubmit;

    for (int i = 0; i < trainInfoVec.size(); i++) {
        QStringList &trainList = trainInfoVec[i];
        if (trainList.empty() || trainTicketInfo[i].empty())
            continue;
        QString trainDesc = _("%1 (%2 %3").arg(trainList[ESTATIONTRAINCODE],
                                               stationMap.value(trainList[EFROMSTATIONTELECODE]).toString(),
                                               stationMap.value(trainList[ETOSTATIONTELECODE]).toString());
        // Currentlly frozened
        if (nh->queryFrozenTrain(trainDesc)) {
            continue;
        }
        if (trainCode.isEmpty()) {
            if (!selectedTrainSet.contains(trainDesc)) {
                continue;
            }
        } else {
            if (trainList[ESTATIONTRAINCODE] != trainCode) {
                continue;
            }
        }

        trainTicketNum = 0;
        for (int j = 0; j < selectSeatTypeSize; j++) {
            if (trainTicketInfo[i][selectSeatType[j]] > 0) {
                trainTicketNum += trainTicketInfo[i][selectSeatType[j]];
            }
        }
        if (trainTicketNum >= passengerList.size()) {
            return i;
        }
        if (partial) {
            if (trainTicketNum > partialTicketMax) {
                partialTicketMax = trainTicketNum;
                partialTicketMaxIdx = i;
            }
        }
    }

    return partialTicketMaxIdx;
}

void Analysis::initTrainTicketInfo()
{
    trainTicketInfo.resize(trainInfoVec.size());
    QVector<int> trainSeatTypeDataNum = {
                                         ESWZNUM,
                                         ETZNUM,
                                         EGGNUM,
                                         EZYNUM,
                                         EZENUM,
                                         EGRNUM,
                                         ERWNUM,
                                         ESRRBNUM,
                                         EYWNUM,
                                         ERZNUM,
                                         EYZNUM,
                                         EWZNUM,
                                         EQTNUM
    };
    for (int i = 0; i < trainInfoVec.size(); i++) {
        QStringList &trainList = trainInfoVec[i];
        QVector<int> ticketVec;
        ticketVec.resize(TICKETSIZE);
        for (auto &seatTypeNum : trainSeatTypeDataNum) {
            ticketVec[TICKETIDX(seatTypeNum)] = trainList[seatTypeNum] == QStringLiteral("有") ? 100 :
                                                !trainList[seatTypeNum].isEmpty() && trainList[seatTypeNum] != QStringLiteral("无") &&
                                                trainList[seatTypeNum] != QStringLiteral("*") ?
                                                trainList[seatTypeNum].toInt() : !trainList[seatTypeNum].isEmpty() &&
                                                trainList[seatTypeNum] == _("无") ? 0 : -1;
        }

        trainTicketInfo[i] = ticketVec;
    }
}

void Analysis::sufficientSeatTypePrioSelectSeatType(const QVector<int> &selectSeatType,
                                                  QVector<std::pair<int, int>> &seatVec, int trainNoIdx, int passengerSize)
{
    int ticketSeatMax = 0;
    int ticketSeatTypeMax = 0;
    int selectSeatTypeSize = selectSeatType.size();

    w->formatOutput(_("正在选择席别，使用规则：余票充足的席别优先提交"));
    for (int j = 0; j < selectSeatTypeSize; j++) {
        int ticketNum = trainTicketInfo[trainNoIdx][selectSeatType[j]];
        if (ticketNum > ticketSeatMax) {
            ticketSeatMax = ticketNum;
            ticketSeatTypeMax = selectSeatType[j];
        }
    }
    if (ticketSeatMax) {
        seatVec.push_back(std::pair(ticketSeatTypeMax, ticketSeatMax > passengerSize ?
                                                           passengerSize : ticketSeatMax));
        trainTicketInfo[trainNoIdx][ticketSeatTypeMax] -= ticketSeatMax;
        w->formatOutput(_("已选择席别%1, 余票%2").arg(ticketIndexToSeatTypeName(ticketSeatTypeMax),
                                                      ticketSeatMax == 100 ?
                                                          _("充足") : _("%1张").arg(ticketSeatMax)));
    }
    // 座位最多的席别余票仍然不足，选择第二个席别（比如二等座，一等座）
    if (ticketSeatMax < passengerSize) {
        int remain = passengerSize - ticketSeatMax;
        for (int j = 0; j < selectSeatTypeSize; j++) {
            int ticketNum = trainTicketInfo[trainNoIdx][selectSeatType[j]];
            if (ticketNum > 0) {
                seatVec.push_back(std::pair(selectSeatType[j], ticketNum > remain ?
                                                                   remain : ticketNum));
                w->formatOutput(_("已选择席别%1, 余票%2").arg(ticketIndexToSeatTypeName(selectSeatType[j]),
                                                              ticketNum == 100 ?
                                                                  _("充足") : _("%1张").arg(ticketNum)));
                remain -= ticketNum;
                if (remain <= 0) {
                    break;
                }
            }
        }
        // remain还大于0的话说明勾选了余票不足部分提交
        if (remain > 0) {
            w->formatOutput(_("当前车次已选席别余票不足，已开启余票不足自动提交，继续提交..."));
        }
    }
}

void Analysis::strictPriceLowToHighSelectSeatType(const QVector<int> &selectSeatType, QVector<int> priceLowToHigh,
                                                  QVector<std::pair<int, int>> &seatVec, int trainNoIdx, int passengerSize)
{
    int priceLowToHighSize = priceLowToHigh.size();
    int remain = passengerSize;
    int i;
    for (i = 0; i < priceLowToHighSize; i++) {
        if (selectSeatType.contains(priceLowToHigh[i])) {
            int ticketNum = trainTicketInfo[trainNoIdx][priceLowToHigh[i]];
            if (ticketNum > 0) {
                seatVec.push_back(std::pair(priceLowToHigh[i], ticketNum > remain ?
                                                                   remain : ticketNum));
                w->formatOutput(_("已选择席别%1, 余票%2").arg(ticketIndexToSeatTypeName(priceLowToHigh[i]),
                                                              ticketNum == 100 ?
                                                                  _("充足") : _("%1张").arg(ticketNum)));
                remain -= ticketNum;
                if (remain <= 0) {
                    break;
                }
            }
        }
    }
    // remain还大于0的话说明勾选了余票不足部分提交
    if (remain > 0) {
        w->formatOutput(_("当前车次已选席别余票不足，已开启余票不足自动提交，继续提交..."));
    }
}

QVector<std::pair<int, int>> Analysis::trainSelectSeatType(int trainNoIdx, bool silent)
{
    UserData *ud = UserData::instance();
    QVector<int> selectSeatType;
    int selectSeatTypeSize = 0;
    const QList<QString> &selectedSeatTypeStrList = w->seatTypeDialog->getSelectedSeatType();
    Q_ASSERT(selectedSeatTypeStrList.size() <= TICKETSIZE);
    selectSeatTypeSize = seatTypeNameToTicketIndex(selectedSeatTypeStrList, selectSeatType);
    const QList<QString> &passengerList = w->passengerDialog->getSelectedPassenger();
    QVector<std::pair<int, int>> seatVec;

    if (trainNoIdx < trainTicketInfo.size()) {
        if (ud->grabSetting.seatTypePrio.sufficientSeatTypePrio) {
            sufficientSeatTypePrioSelectSeatType(selectSeatType, seatVec, trainNoIdx, passengerList.size());
        } else if (ud->grabSetting.seatTypePrio.strictSeatTypePrio) {
            int remain = passengerList.size();
            if (!silent)
                w->formatOutput(_("正在选择席别，使用规则：按选中席别的顺序提交"));
            for (int j = 0; j < selectSeatTypeSize; j++) {
                int ticketNum = trainTicketInfo[trainNoIdx][selectSeatType[j]];
                if (ticketNum > 0) {
                    seatVec.push_back(std::pair(selectSeatType[j], ticketNum > remain ?
                                                                       remain : ticketNum));
                    if (!silent)
                        w->formatOutput(_("已选择席别%1, 余票%2").arg(ticketIndexToSeatTypeName(selectSeatType[j]),
                                                                ticketNum == 100 ?
                                                                _("充足") : _("%1张").arg(ticketNum)));
                    remain -= ticketNum;
                    if (remain <= 0) {
                        break;
                    }
                }
            }
            // remain还大于0的话说明勾选了余票不足部分提交
            if (remain > 0) {
                if (!silent)
                    w->formatOutput(_("当前车次已选席别余票不足，已开启余票不足自动提交，继续提交..."));
            }
        } else if (ud->grabSetting.seatTypePrio.priceLowSeatTypePrioWhensufficient) {
            QStringList priceLowToHighStrList = { _("无座"), _("硬座"), _("二等座"),
                                                  _("一等座"), _("优选一等座"), _("软座"), _("硬卧"),
                                                  _("特等座"), _("商务座"), _("软卧"), _("高级软卧"),
                                                  _("动卧") };
            QVector<int> priceLowToHigh;
            int priceLowToHighSize;
            int i;
            priceLowToHighSize = seatTypeNameToTicketIndex(priceLowToHighStrList, priceLowToHigh);
            if (!silent)
                w->formatOutput(_("正在选择席别，使用规则：余票充足时价格低的席别优先提交"));
            for (i = 0; i < priceLowToHighSize; i++) {
                if (selectSeatType.contains(priceLowToHigh[i])) {
                    int ticketNum = trainTicketInfo[trainNoIdx][priceLowToHigh[i]];
                    // 余票充足，即显示为有
                    if (ticketNum == 100) {
                        seatVec.push_back(std::pair(priceLowToHigh[i], passengerList.size()));
                        if (!silent)
                            w->formatOutput(_("已选择席别%1, 余票%2").arg(ticketIndexToSeatTypeName(priceLowToHigh[i]),
                                                                      ticketNum == 100 ?
                                                                          _("充足") : _("%1张").arg(ticketNum)));
                        break;
                    }
                }
            }
            if (i == priceLowToHighSize) {
                sufficientSeatTypePrioSelectSeatType(selectSeatType, seatVec, trainNoIdx, passengerList.size());
            }
        } else if (ud->grabSetting.seatTypePrio.priceLowSeatTypePrio) {
            QStringList priceLowToHighStrList = { _("无座"), _("硬座"), _("二等座"),
                                                  _("一等座"), _("优选一等座"), _("软座"), _("硬卧"),
                                                  _("特等座"), _("商务座"), _("软卧"), _("高级软卧"),
                                                  _("动卧") };
            QVector<int> priceLowToHigh;
            seatTypeNameToTicketIndex(priceLowToHighStrList, priceLowToHigh);
            if (!silent)
                w->formatOutput(_("正在选择席别，使用规则：价格低的席别优先提交"));
            strictPriceLowToHighSelectSeatType(selectSeatType, priceLowToHigh, seatVec,
                                               trainNoIdx, passengerList.size());
        } else {
            // ud->grabSetting.seatTypePrio.priceHighSeatTypePrio
            QStringList priceHighToLowStrList = { _("动卧"),  _("高级软卧"), _("软卧"), _("商务座"), _("特等座"),
                                                 _("硬卧"), _("软座"), _("优选一等座"), _("一等座"), _("二等座"), _("硬座"), _("无座")};
            QVector<int> priceHighToLow;
            seatTypeNameToTicketIndex(priceHighToLowStrList, priceHighToLow);
            if (!silent)
                w->formatOutput(_("正在选择席别，使用规则：价格高的席别优先提交"));
            strictPriceLowToHighSelectSeatType(selectSeatType, priceHighToLow, seatVec,
                                               trainNoIdx, passengerList.size());
        }
    }
    return seatVec;
}

std::pair<QString, QString> Analysis::generateSubmitTicketInfo(int trainNoIdx, QVector<std::pair<int, int>> &seatVec,
                                                               QVector<QPair<QString, QChar>> &submitSeatType)
{
    const QStringList &passengerList = w->passengerDialog->getSelectedPassenger();
    QString submitTicketStr, oldPassengerTicketStr;
    QString passType;
    int j, idx;
    UserData *ud = UserData::instance();

    idx = 0;
    submitSeatType.clear();
    for (int i = 0; i < seatVec.size(); i++) {
        if (i >= passengerList.size())
            break;
        if (idx >= passengerList.size())
            break;
        int k;
        QChar code = ticketIndexToCode(seatVec[i].first);
        if (code == '0') {
            qDebug() << "Unsupported seat type: " << seatVec[i].first;
            w->formatOutput(_("不支持的席别类型：") + ticketIndexToSeatTypeName(seatVec[i].first));
            continue;
        }
        // 无座
        if (code == 'W') {
            code = trainTicketInfo[trainNoIdx][TICKETIDX(EZENUM)] != -1 ? 'O' : '1';  // 二等座 : 硬座
        }
        for (k = 0; k < seatVec[i].second; k++) {
            if (idx >= passengerList.size())
                break;
            for (j = 0; j < ud->passenger.size(); j++) {
                if (ud->passenger[j].passName == passengerList[idx])
                    break;
            }
            if (j >= ud->passenger.size()) {
                idx++;
                continue;
            }
            submitSeatType.append(QPair<QString, QChar>(ud->passenger[j].passName, code));
            passType = ud->grabSetting.isStudent ? ud->passenger[j].passType :
                           ud->passenger[j].passType == _("3") ? PASSENGERADULT : ud->passenger[j].passType;
            submitTicketStr.append(QStringLiteral("%1,0,%2,%3,%4,%5,%6,N,%7_").arg(code, passType,
                                                                                   ud->passenger[j].passName.toUtf8().toPercentEncoding(),
                                                                                   ud->passenger[j].passIdTypeCode,
                                                                                   ud->passenger[j].passIdNo,
                                                                                   ud->passenger[j].mobile,
                                                                                   ud->passenger[j].allEncStr));
            oldPassengerTicketStr.append(QStringLiteral("%1,%2,%3,%4_").arg(ud->passenger[j].passName.toUtf8().toPercentEncoding(),
                                                                            ud->passenger[j].passIdTypeCode,
                                                                            ud->passenger[j].passIdNo,
                                                                            passType));
            idx++;
        }
    }
    if (submitTicketStr.length() > 0)
        submitTicketStr.truncate(submitTicketStr.length() - 1);
    submitTicketStr.replace(QStringLiteral(","), QStringLiteral("%2C"));
    oldPassengerTicketStr.replace(QStringLiteral(","), QStringLiteral("%2C"));
    w->formatOutput(_("正在生成提交信息..."));

    return std::pair<QString, QString>(submitTicketStr, oldPassengerTicketStr);
}

int Analysis::analysisTrain(std::pair<QString, QString> &ticketStr, QVector<QPair<QString, QChar>> &submitSeatType, const QVariantMap &stationMap)
{
    UserData *ud = UserData::instance();
    int trainNoIdx = -1;
    int methodDescIdx = 0;

    if (trainTicketInfo.isEmpty()) {
        initTrainTicketInfo();
    }
    if (ud->grabSetting.trainPrio.sufficientTicketPrio) {
        trainNoIdx = sufficientTicketPrioSelectTrain(stationMap);
        methodDescIdx = 0;
    } else if (ud->grabSetting.trainPrio.strictTrainPrio) {
        trainNoIdx = strictTrainPrioSelectTrain(stationMap);
        methodDescIdx = 1;
    } else if (ud->grabSetting.trainPrio.travelTimeShortPrio) {
        trainNoIdx = strictTravelTimePrioSelectTrain(stationMap);
        methodDescIdx = 2;
    } else if (ud->grabSetting.trainPrio.strictStartTimePrio) {
        trainNoIdx = strictStartTimePrioSelectTrain(stationMap, QString());
        methodDescIdx = 3;
    }
    if (trainNoIdx != -1) {
        QStringList methodDesc = { _("检测到可预订的已选中车次，使用规则：余票充足的车次优先提交"),
                                   _("检测到可预订的已选中车次，使用规则：按选中车次的顺序提交"),
                                   _("检测到可预订的已选中车次，使用规则：行程时间短的车次优先提交"),
                                   _("检测到可预订的已选中车次，使用规则：按列车发车时间顺序提交")
        };
        QString startStation, endStation;
        w->formatOutput(methodDesc[methodDescIdx]);
        startStation = stationMap.value(trainInfoVec[trainNoIdx][ESTARTSTATIONTELECODE]).toString();
        endStation = stationMap.value(trainInfoVec[trainNoIdx][EENDSTATIONTELECODE]).toString();
        w->formatOutput(_("已选中车次%1 始发站：%2, 终点站：%3, 出发站：%4, 到达站：%5, 出发时间：%6, 到达时间：%7, 历时：%8")
                        .arg(trainInfoVec[trainNoIdx][ESTATIONTRAINCODE],
                            startStation,
                            endStation,
                            stationMap.value(trainInfoVec[trainNoIdx][EFROMSTATIONTELECODE]).toString(),
                            stationMap.value(trainInfoVec[trainNoIdx][ETOSTATIONTELECODE]).toString(),
                            trainInfoVec[trainNoIdx][ESTARTTIME],
                            trainInfoVec[trainNoIdx][EARRIVETIME],
                            trainInfoVec[trainNoIdx][ESPENDTIME]));
        QVector<std::pair<int, int>> seatVec = trainSelectSeatType(trainNoIdx, false);
        ticketStr = generateSubmitTicketInfo(trainNoIdx, seatVec, submitSeatType);
    }
    return trainNoIdx;
}

int Analysis::analysisOneTrain(QVector<std::pair<int, int>> &seatVec, const QVariantMap &stationMap, const QString &trainCode)
{
    int trainNoIdx = -1;

    if (trainTicketInfo.isEmpty()) {
        initTrainTicketInfo();
    }
    trainNoIdx = strictStartTimePrioSelectTrain(stationMap, trainCode);
    if (trainNoIdx != -1) {
        seatVec = trainSelectSeatType(trainNoIdx, true);
    }
    return trainNoIdx;
}

bool Analysis::mayCandidate(const QVariantMap &stationMap, const QString &date)
{
    bool ret = false;
    int i, j;
    int trainInfoVecSize = trainInfoVec.size();
    const QList<QString> &selectedSeatTypeList = w->seatTypeDialog->getSelectedSeatType();
    QVector<int> selectSeatType;
    int selectSeatTypeSize = seatTypeNameToTicketIndex(selectedSeatTypeList, selectSeatType);
    const QSet<QString> &selectedTrainSet = w->trainNoDialog->getSelectedTrainSet();
    UserData *ud = UserData::instance();
    struct CandidateDateInfo dInfo;

    if (trainTicketInfo.isEmpty()) {
        initTrainTicketInfo();
    }
    for (i = 0; i < trainInfoVecSize; i++) {
        QStringList &trainList = trainInfoVec[i];
        if (trainList[ESECRETSTR].isEmpty()) {
            continue;
        }
        if (trainList[ECANDIDATETRAINFLAG] != _("1")) {
            continue;
        }
        QString trainDesc = _("%1 (%2 %3").arg(trainList[ESTATIONTRAINCODE],
                                               stationMap.value(trainList[EFROMSTATIONTELECODE]).toString(),
                                               stationMap.value(trainList[ETOSTATIONTELECODE]).toString());
        if (!selectedTrainSet.contains(trainDesc)) {
            continue;
        }
        struct CandidateTrainInfo tInfo;
        for (j = 0; j < selectSeatTypeSize; j++) {
            // 其他 and 无座
            if (selectSeatType[j] == 2 ||
                selectSeatType[j] == 6) {
                continue;
            }
            if (trainTicketInfo[i][selectSeatType[j]] == 0 ||
                (trainTicketInfo[i][selectSeatType[j]] != -1 && ud->candidateSetting.forceCandidate)) {
                QChar code = seatTypeNameToCode(selectedSeatTypeList[j]);
                if (code != '0') {
                    if (trainList[ECANDIDATESEATLIMIT].contains(code)) {
                        w->formatOutput(_("当前%1车次%2席别提交的候补订单较多，跳过...")
                                            .arg(trainList[ESTATIONTRAINCODE],
                                                 selectedSeatTypeList[j]));
                        continue;
                    }
                    tInfo.trainCode = trainList[ESTATIONTRAINCODE];
                    tInfo.secretStr = trainList[ESECRETSTR];
                    tInfo.seatType.append(code);
                    if (!ud->candidateInfo.allSeatType.contains(code)) {
                        ud->candidateInfo.allSeatType.append(code);
                    }
                }
            }
        }
        dInfo.train.append(tInfo);
        dInfo.date = date;
        dInfo.hasUpdate = true;
    }
    if (!dInfo.train.isEmpty()) {
        NetHelper::instance()->candidateEntry(dInfo);
        ret = true;
    }
    return ret;
}
