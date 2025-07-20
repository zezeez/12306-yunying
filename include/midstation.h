#ifndef MIDSTATION_H
#define MIDSTATION_H
#include <QVector>
#include <QString>
#include <QQueue>
#include <QTimer>
#include <QObject>
#include <QStringList>

class MidStation
{
public:
    explicit MidStation();
    ~MidStation();

    int initStations(const QString &trainCode, const QVector<QString> &stations, const QString &mid);
    QPair<QString, QString> nextStation()
    {
        if (pendingQueryStationQue.isEmpty()) {
            return QPair<QString, QString>
                (QPair<QString, QString>(QString(), QString()));
        }

        return QPair<QString, QString>
            (pendingQueryStationQue.dequeue());
    }
    const QVector<QString> &stationsCode() const
    {
        return midStationVec;
    }
    QString midCode() const
    {
        return midStation;
    }
    int midCodePos() const
    {
        return this->midPos;
    }
    bool isEmpty() const
    {
        return pendingQueryStationQue.isEmpty();
    }
    void clear()
    {
        midStationVec.clear();
        pendingQueryStationQue.clear();
        trainCode.clear();
    }

private:
    QVector<QString> midStationVec;
    QQueue<QPair<QString, QString>> pendingQueryStationQue;
    QString midStation;
    int midPos;
    QString trainCode;
};

class MidStationQuery
{
public:
    explicit MidStationQuery();
    ~MidStationQuery();
    int initStations(const QString &trainCode, const QVector<QString> &stations, const QString &midName, const QString &midCode)
    {
        this->midName = midName;
        currentTrainCode = trainCode;
        return midStation.initStations(trainCode, stations, midCode);
    }
    int query();
    int queryReply(const QVariantMap &data);
    void start()
    {
        pause = false;
        query();
    }
    void stop()
    {
        pause = true;
    }
    bool isStart() const
    {
        return !pause;
    }
    bool isComplete() const
    {
        return complete;
    }
    bool isEmpty() const
    {
        return midStation.isEmpty();
    }
    void clear()
    {
        midStation.clear();
        currentStation.first.clear();
        currentStation.second.clear();
        currentTrainCode.clear();
        complete = false;
    }
private:
    MidStation midStation;
    QPair<QString, QString> currentStation;
    QString currentTrainCode;
    QString midName;
    bool pause;
    bool complete;
};

class MidTrainQuery : public QObject
{
    Q_OBJECT
public:
    explicit MidTrainQuery();
    ~MidTrainQuery();
    int initTrain();
    int initStations(const QString &trainCode, const QVector<QString> &stations, const QString &midName, const QString &midCode)
    {
        return midStationQuery.initStations(trainCode, stations, midName, midCode);
    }
    void queryTimeout()
    {
        if (midStationQuery.isComplete()) {
            complete = true;
            t.stop();
            return;
        }
        if (!midStationQuery.isEmpty()) {
            midStationQuery.query();
            return;
        }

        if (pendingTrain.isEmpty()) {
            complete = true;
            t.stop();
            return;
        }
        complete = false;
        currentTrain = pendingTrain.dequeue();
        query(currentTrain);
    }
    int query(QStringList trainInfo);
    int queryReply(const QVariantMap &data);
    int queryMidStationReply(const QVariantMap &data)
    {
        return midStationQuery.queryReply(data);
    }
    void start()
    {
        pause = false;
        t.start();
    }
    void stop()
    {
        pause = true;
        t.stop();
    }
    bool isStart() const
    {
        return !pause;
    }
    bool isComplete() const
    {
        return complete;
    }
    bool isEmpty() const
    {
        if (!pendingTrain.isEmpty()) {
            return false;
        }
        return midStationQuery.isEmpty();
    }
    void clear()
    {
        midStationQuery.clear();
        pendingTrain.clear();
        currentTrain.clear();
        complete = false;
    }
    void setInterval(int v)
    {
        t.setInterval(v);
    }
private:
    MidStationQuery midStationQuery;
    QQueue<QStringList> pendingTrain;
    QStringList currentTrain;
    bool pause;
    bool complete;
    QTimer t;
};

#endif // MIDSTATION_H
