#include "midstation.h"
#include "nethelper.h"
#include "analysis.h"
#include "mainwindow.h"

extern MainWindow *w;

MidStation::MidStation()
{

}

MidStation::~MidStation()
{

}

int MidStation::initStations(const QString &trainCode, const QVector<QString> &stations, const QString &mid)
{
    if (!pendingQueryStationQue.isEmpty()) {
        return -1;
    }
    if (stations.size() < 2) {
        return -1;
    }

    int i;
    for (i = 0; i < stations.size(); i++) {
        if ( stations[i] == mid) {
            break;
        }
    }

    if (i >= stations.size()) {
        return -1;
    }

    midPos = i;
    this->trainCode = trainCode;
    midStationVec = stations;
    midStation = mid;

    int j;

    for (j = i; j > 0; j--) {
        pendingQueryStationQue.enqueue(QPair<QString, QString>(stations[0], stations[j]));
    }

    for (j = i + 1; j < stations.size(); j++) {
        pendingQueryStationQue.enqueue(QPair<QString, QString>(stations[0], stations[j]));
    }

    return 0;
}

MidStationQuery::MidStationQuery()
{

}

MidStationQuery::~MidStationQuery()
{

}

int MidStationQuery::query()
{
    QPair<QString, QString> sta;

    if (pause) {
        return -1;
    }
    sta = midStation.nextStation();
    if (sta.first.isEmpty() || sta.second.isEmpty()) {
        return 0;
    }
    NetHelper::instance()->queryStationTicket(sta.first, sta.second);
    currentStation = sta;
    return 0;
}

int MidStationQuery::queryReply(const QVariantMap &data)
{
    QVariantList resultList = data[_("result")].toList();
    int resultListSize = resultList.size();
    QVector<QStringList> availableTrain;
    QVariantMap stationMap = data[_("map")].toMap();

    if (resultListSize == 0) {
        return -1;
    }

    for (int i = 0; i < resultListSize; i++) {
        QString train = resultList[i].toString();
        QStringList trainInfo = train.split('|');

        if (trainInfo.size() < ETRAININFOMAX) {
            continue;
        }
        if (trainInfo[ESECRETSTR].isEmpty()) {
            continue;
        }
        if (trainInfo[ESTATIONTRAINCODE].isEmpty()) {
            continue;
        }
        if (trainInfo[ECANWEBBUY] == _("Y")) {
            availableTrain.push_back(trainInfo);
        }
    }

    if (!availableTrain.isEmpty()) {
        Analysis ana(availableTrain);
        QVector<std::pair<int, int>> seatVec;
        int trainNoIdx = ana.analysisOneTrain(seatVec, stationMap, currentTrainCode);
        if (trainNoIdx != -1) {
            complete = true;
            QString msg;
            int i;
            int pos, n;

            msg = _("列车 <font color=\"#ff0000\">%1 %2-%3</font> 尚有").arg(availableTrain[trainNoIdx][ESTATIONTRAINCODE],
                                              stationMap.value(availableTrain[trainNoIdx][EFROMSTATIONTELECODE]).toString(),
                                              stationMap.value(availableTrain[trainNoIdx][ETOSTATIONTELECODE]).toString());
            for (i = 0; i < seatVec.size(); i++) {
                QString ticketStr = availableTrain[trainNoIdx][seatVec[i].first + EGGNUM];
                msg.append(_("%1余票<font color=\"#ff0000\">%2</font>，").arg(ticketIndexToSeatTypeName(seatVec[i].first),
                                                ticketStr == _("有") ? _("充足") : ticketStr + _("张")));
            }
            const QVector<QString> &stations = midStation.stationsCode();
            int midPos = midStation.midCodePos();
            pos = 0;
            for (i = 0; i < stations.size(); i++) {
                if (stations[i] == currentStation.second) {
                    pos = i;
                    break;
                }
            }
            n = midPos - pos;
            if (n < 0) {
                msg.append(_("比到达站 %1 <font color=\"#ff0000\">多买 %2 站</font>，").arg(midName).arg(n));
            } else {
                msg.append(_("比到达站 %1 需<font color=\"#ff0000\">补票 %2 站</font>，").arg(midName).arg(n));
            }
            msg.append(_("余票信息实时变化，请以查询为准"));
            QTime now = QTime::currentTime();
            w->notifyMng->notify(now.toString("hh:mm:ss"), msg, "://img/train_icon.png", data["requestUrl"].toString());
        }
    }

    return 0;
}

MidTrainQuery::MidTrainQuery()
{
    t.setInterval(5000);
    connect(&t, &QTimer::timeout, this, &MidTrainQuery::queryTimeout);
}

MidTrainQuery::~MidTrainQuery()
{

}

int MidTrainQuery::initTrain()
{
    if (!pendingTrain.isEmpty()) {
        return -1;
    }
    const QList<QString> &seletedTrain = w->trainNoDialog->getSelectedTrainList();
    QStringList train;

    for (const QString &s : seletedTrain) {
        train = s.split(' ');
        if (train.size() > 2) {
            train[1].remove(0, 1);
            pendingTrain.enqueue(train);
        }
    }

    return 0;
}

int MidTrainQuery::query(QStringList trainInfo)
{
    UserData *ud = UserData::instance();

    if (trainInfo.size() < 3) {
        return -1;
    }

    trainInfo[0] = w->trainNoDialog->getTrainNoMap().value(trainInfo[0]);
    // stationName to stationCode
    trainInfo[1] = ud->staCode->value(trainInfo[1]);
    trainInfo[2] = ud->staCode->value(trainInfo[2]);
    trainInfo.push_back(UserData::instance()->getUserConfig().tourDate);
    NetHelper::instance()->queryTrainStopStation(trainInfo, EQUERYSTOPSTATIONANALYSIS);
    return 0;
}

int MidTrainQuery::queryReply(const QVariantMap &data)
{
    if (currentTrain.size() < 3) {
        return -1;
    }
    QVariantMap data1 = data[_("data")].toMap();
    if (!data.isEmpty()) {
        QString trainCode;
        QVariantList data2 = data1[_("data")].toList();
        QVector<QString> stationVec;
        UserData *ud = UserData::instance();

        for (int i = 0; i < data2.length(); i++) {
            QVariantMap data3 = data2[i].toMap();
            QString s;
            if (i == 0) {
                trainCode = data3[_("station_train_code")].toString();
                stationVec.append(ud->staCode->value(currentTrain[1]));
            }
            s = data3["station_name"].toString();
            stationVec.append(ud->staCode->value(s));
        }
        midStationQuery.initStations(trainCode, stationVec, currentTrain[2], UserData::instance()->getStaCode()->value(currentTrain[2]));
    }

    return 0;
}
